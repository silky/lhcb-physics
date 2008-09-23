"""
High level configuration tools for Moore
"""
__version__ = "$Id: Configuration.py,v 1.29 2008-09-23 13:15:08 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ConfigFileAccessSvc, ConfigDBAccessSvc, HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc
from HltConf.Configuration import *

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint

class Moore(ConfigurableUser):
    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "DAQStudies":        False # use DAQ study version of options
        , "DDDBtag" :          '2008-default'
        , "condDBtag" :        '2008-default'
        , "inputType":         'dst' # must either 'mdf' or 'dst'
        , "DC06" :             False # use DC06 setup
        , "hltType" :          'PA+LU+VE'
        , "runTiming"  :       False # include additional timing information
        , "useTCK"     :       False # use TCK instead of options...
        , "prefetchTCK" :      [ ] # which TCKs to prefetch. Initial TCK used is first one...
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "TCKData" :          '$TCKDATAROOT' # where do we read TCK data from?
        , "TCKpersistency" :   'file' # which method to use for TCK data? valid is 'file' and 'sqlite'
        , "enableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , "userAlgorithms":    [ ]  # put here user algorithms to add
        , "oldStyle" :         True # old style options configuration
        }   
                
    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def setProp(self,name,value):
        return setattr(self,name,value)

    def setOtherProp(self,other,name):
        # Function to propagate properties to other component, if not already set
        if hasattr(other,name) and len(other.getProp(name)) > 0 :
            print "# %s().%s already defined, ignoring Moore().%s"%(other.name(),name,name)
        else:
            print 'Propagating ' + name + ' = ' + str(self.getProp(name)) + ' to ' + other.name()
            setattr(other,name,self.getProp(name))

    def validHltTypes(self):
        return [ 'PA',
                 'PA+LU',
                 'PA+LU+VE',
                 'PA+LU+VE+MU',
                 'PA+LU+VE+MU+HA+EL+PH',
                 'Physics_HLT1',
                 'Physics_HLT1+HLT2',
                 'DEFAULT' ]

    def getRelease(self):
        import re,fileinput
        version = re.compile('^version (\w+)')
        for line in fileinput.input(os.environ.get('MOOREROOT')+'/cmt/requirements') :
            match = version.match(line)
            if not match: continue
            fileinput.close()
            return 'MOORE_'+match.group(1)
        raise TypeError('Could not deduce version from cmt/requirementes')

    def getConfigAccessSvc(self):
        method = self.getProp('TCKpersistency').lower()
        if method not in [ 'file', 'sqlite' ] : raise TypeError("invalid TCK persistency '%s'"%method)
        TCKData = self.getProp('TCKData')
        if method == 'file' :
            return ConfigFileAccessSvc( Directory = TCKData +'/config' )
        if method == 'sqlite' :
            return ConfigDBAccessSvc( Connection = 'sqlite_file:' + TCKData +'/db/config.db' )

    def addAuditor(self,x) :
        AuditorSvc().Auditors.append( x.name() )
        x.Enable = True


    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        importOptions('$STDOPTS/DstDicts.opts')

        inputType = self.getProp('inputType').upper()
        if inputType not in [ 'MDF','DST','RAW' ] : raise TypeError("Invalid input type '%s'"%inputType)
        if inputType != 'DST' : 
            EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        importOptions('$STDOPTS/DecodeRawEvent.opts')
        ApplicationMgr().ExtSvc.append(  "DataOnDemandSvc"   ); # needed for DecodeRawEvent...

        # forward some other settings... TODO: make a dictionary..
        # self.setOtherProp( LHCbApp(), 'useOracleCondDB' )
        importOptions( "$DDDBROOT/options/DDDB.py" )
        if self.getProp('DC06') :
            importOptions( '$DDDBROOT/options/DC06.opts' )
        else : 
            # the next is for 2008 data & MC, i.e. NOT for DC'06
            importOptions( '$DDDBROOT/options/LHCb-2008.py' )
        self.setOtherProp( LHCbApp(), 'DDDBtag' )
        self.setOtherProp( LHCbApp(), 'condDBtag' )
        # self.setOtherProp( LHCbApp(), 'skipEvents' )
        self.setOtherProp( ApplicationMgr(), 'EvtMax' )
        # Get the event time (for CondDb) from ODIN 
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # make sure we don't pick up small variations of the read current
        MagneticFieldSvc().UseSetCurrent = True
        # output levels...
        ToolSvc().OutputLevel                     = INFO
        from Configurables import XmlParserSvc 
        XmlParserSvc().OutputLevel                = WARNING
        MessageSvc().OutputLevel                  = INFO
        ApplicationMgr().OutputLevel              = ERROR
        SequencerTimerTool().OutputLevel          = WARNING
        # Print algorithm name with 40 characters
        MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'
        # Profiling
        for i in [ 'ToolSvc' , 'AuditorSvc' ] : ApplicationMgr().ExtSvc.append( i ) 
        ApplicationMgr().AuditAlgorithms = 1
        AuditorSvc().Auditors.append( 'TimingAuditor/TIMER' )
        for i in self.getProp('enableAuditor') : self.addAuditor( i )
        # TODO: check for mutually exclusive options...
        if self.getProp('useTCK') :
            tcks = [ int(i) for i in self.getProp('prefetchTCK') ]
            cfg = HltConfigSvc( prefetchTCK = tcks
                              , initialTCK = tcks[0]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
            ApplicationMgr().ExtSvc.append(cfg.getFullName())
        else:
            for i in [ 'hltType','oldStyle','userAlgorithms' ] : 
                self.setOtherProp( HltConf(), i )
            HltConf().applyConf()

            
        if self.getProp("generateConfig") :
            # TODO: add properties for ConfigTop and ConfigSvc...
            
            gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] if type(i) is str else i.getName() for i in ApplicationMgr().TopAlg ]
                              , ConfigSvc = [ 'ToolSvc','HltDataSvc','HltANNSvc' ]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                              , hltType = self.getProp('hltType')
                              , mooreRelease = self.getRelease()
                              , label = self.getProp('configLabel'))
            # make sure gen is the very first Top algorithm...
            ApplicationMgr().TopAlg = [ gen.getFullName() ] + ApplicationMgr().TopAlg

            
        LHCbApp().applyConf()
