"""
High level configuration tool(s) for Moore
"""
__version__ = "$Id: Configuration.py,v 1.131 2010-09-01 16:39:02 raaij Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ, path
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import HltConf
from Configurables import GaudiSequencer
from Configurables import LHCbApp, L0Conf

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint


## HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

def _ext(name) : 
    x =  path.splitext(name)[-1].lstrip('.').upper()
    if x == 'MDF' : x = 'RAW'
    if x == 'XDST' : x = 'DST'
    return x

def _datafmt(fn) : 
    pfn = 'PFN:%s' % fn if fn.find(':') == -1  else fn
    fmt = { 'RAW' : "DATAFILE='%s' SVC='LHCb::MDFSelector'"
          , 'DST' : "DATAFILE='%s' TYP='POOL_ROOTTREE' OPT='READ'" 
          }
    ext = _ext(pfn)
    if ext not in fmt.iterkeys() : ext = 'DST'
    return fmt[ ext ] % pfn

# canonicalize tck  -- eats integer + string, returns canonical string
def _tck(x) :
    if type(x) == str and x[0:2] == '0x' : return '0x%08x'%int(x,16)
    return '0x%08x'%int(x)

class Moore(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ HltConf
                             , LHCbApp
                             , L0Conf ]


    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "SkipEvents":        0
        , "Simulation":        True # True implies use SimCond
        , "DataType":          '2010' # Data type, can be [ 'DC06','2008' ]
        , "DDDBtag" :          'default' # default as set in DDDBConf for DataType
        , "CondDBtag" :        'default' # default as set in DDDBConf for DataType
        , "outputFile" :       '' # output filename
        , "inputFiles" :       [ ] # input
        , "UseTCK"     :       False # use TCK instead of options...
        , 'ForceSingleL0Configuration' : True # use one single, fixed L0 configuration location (ToolSvc.L0DUConfig)
        , 'SkipDisabledL0Channels' : False # add Hlt1L0xxx even for disabled L0 channels 
        , "L0"         :       False # run L0
        , "ReplaceL0BanksWithEmulated" : False # rerun L0
        , "CheckOdin"  :       True  # use TCK from ODIN
        , "InitialTCK" :'0x00012009'  # which configuration to use during initialize
        , "prefetchConfigDir" :'MOORE_v8r8'  # which configurations to prefetch.
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "configAlgorithms" : ['Hlt']    # which algorithms to configure (automatically including their children!)...
        , "configServices" :   ['ToolSvc','Hlt::Service','HltANNSvc' ]    # which services to configure (automatically including their dependencies!)...
        , "TCKData" :          '$HLTTCKROOT' # where do we read/write TCK data from/to?
        , "TCKpersistency" :   'tarfile' # which method to use for TCK data? valid is 'file','tarfile' and 'sqlite' 
        , "EnableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , "EnableDataOnDemand": False
        , "EnableLumiEventWriting"       : True
        , "EnableTimer" :       True
        , 'EnableRunChangeHandler' : False
        , 'EnableAcceptIfSlow' : False
        , 'WriterRequires' : [ 'HltDecisionSequence' ] # this contains Hlt1 & Hlt2
        , "Verbose" :           True # whether or not to print Hlt sequence
        , "ThresholdSettings" : ''
        , 'RequireL0ForEndSequence'     : False
        , 'SkipHltRawBankOnRejectedEvents' : True
        , 'HistogrammingLevel' : 'Line'
        , 'EnableMonitoring' : False
        , "RunOnline"         : False
        , "RunMonitoringFarm" : False
        , "UseDBSnapshot"     : True
        , "DBSnapshotDirectory" : "/group/online/hlt/conditions"
        , 'IgnoreDBHeartBeat'  : False
        , 'TimeOutThreshold'  : 10000  # milliseconds before giving up, and directing event to time out stream
        , 'TimeOutBits'       : 0x200
        , 'RequireRoutingBits' : [] # to require not lumi exclusive, set to [ 0x0, 0x4, 0x0 ]
        , 'VetoRoutingBits'    : []
        , 'REQ1' : ''

        }   
                

    def _configureDataOnDemand(self) :
        if not self.getProp("EnableDataOnDemand") :
            if 'DataOnDemandSvc' in ApplicationMgr().ExtSvc : 
                ApplicationMgr().ExtSvc.pop('DataOnDemandSvc')
        else: 
            from Configurables import DataOnDemandSvc
            dod = DataOnDemandSvc()
            if dod not in ApplicationMgr().ExtSvc :
                ApplicationMgr().ExtSvc.append( dod ) 
            importOptions('$STDOPTS/DecodeRawEvent.py')

    def _configureOnline(self) :
        from Configurables import LoKiSvc
        LoKiSvc().Welcome = False

        import OnlineEnv 
        self.setProp('UseTCK', True)
        self._configureDataOnDemand()

        from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc
        EventPersistencySvc().CnvServices.append( RawDataCnvSvc('RawDataCnvSvc') )
        EventLoopMgr().Warnings = False

        from Configurables import MonitorSvc
        MonitorSvc().disableDimPropServer      = 1
        MonitorSvc().disableDimCmdServer       = 1
	MonitorSvc().disableMonRate            = 0

        app=ApplicationMgr()
        
        # setup the histograms and the monitoring service
        from Configurables import UpdateAndReset
        app.TopAlg = [ UpdateAndReset() ] + app.TopAlg
        app.ExtSvc.append( 'MonitorSvc' ) 
        HistogramPersistencySvc().OutputFile = ''
        HistogramPersistencySvc().Warnings = False
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc().OutputEnabled = False

        # set up the event selector
        if 'EventSelector' in allConfigurables : 
            del allConfigurables['EventSelector']

        if not self.getProp('RunMonitoringFarm') :
            ## Setup Checkpoint & forking: Do this EXACTLY here. Just befor the MEPManager & event selector.
            ## It will not work if these are created before.
            if not self.getProp('RunMonitoringFarm'):
                if OnlineEnv.MooreStartupMode == 1:
                    self._configureOnlineForking()
                elif OnlineEnv.MooreStartupMode == 2:
                    self._configureOnlineCheckpointing()

            TAE = OnlineEnv.TAE != 0
            input   = 'EVENT' if not TAE else 'MEP'
            output  = 'SEND'
            mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input,output],False)
            mepMgr.ConnectWhen = 'start'
            app.Runable = OnlineEnv.evtRunable(mepMgr)
            app.ExtSvc.append(mepMgr)
            evtMerger = OnlineEnv.evtMerger(name='Output',buffer=output,location='DAQ/RawEvent',datatype=OnlineEnv.MDF_NONE,routing=1)
            evtMerger.DataType = OnlineEnv.MDF_BANKS
            eventSelector = OnlineEnv.mbmSelector(input=input, TAE=TAE)
            app.ExtSvc.append(eventSelector)
            OnlineEnv.evtDataSvc()

            # define the send sequence
            writer =  GaudiSequencer('SendSequence')
            writer.OutputLevel = OnlineEnv.OutputLevel
            writer.Members = self.getProp('WriterRequires') + [ evtMerger ]
            app.TopAlg.append( writer )
            #app.OutStream.append( writer )
        else :
            input = 'Events'
            mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input],True)
            app.Runable = OnlineEnv.evtRunable(mepMgr)
            app.ExtSvc.append(mepMgr)
            eventSelector = OnlineEnv.mbmSelector(input=input,decode=False)
            app.ExtSvc.append(eventSelector)
            OnlineEnv.evtDataSvc()
            if self.getProp('REQ1') : eventSelector.REQ1 = self.getProp('REQ1')

        #ToolSvc.SequencerTimerTool.OutputLevel = @OnlineEnv.OutputLevel;          
        from Configurables import AuditorSvc
        AuditorSvc().Auditors = []
        # Now setup the message service
        self._configureOnlineMessageSvc()

    def _configureOnlineForking(self):
        import os, socket, OnlineEnv
        from Configurables import LHCb__CheckpointSvc
        numChildren = os.sysconf('SC_NPROCESSORS_ONLN')
        host = socket.gethostname().split('.')[0].upper()
        if host[:3]=='HLT':
            if len(host)==8:
                id = int(host[6:])
                if id < 5: numChildren=7
                elif id < 12: numChildren=17
                elif id < 23: numChildren=19

        forker = LHCb__CheckpointSvc("CheckpointSvc")
        forker.NumberOfInstances   = numChildren
        forker.Partition           = OnlineEnv.PartitionName
        forker.TaskType            = 'GauchoJob'
        forker.UseCores            = False
        forker.ChildSessions       = False
        forker.FirstChild          = 1
        forker.UtgidPattern        = "%NN_%T_%d";
        forker.PrintLevel          = 3  # 1=MTCP_DEBUG 2=MTCP_INFO 3=MTCP_WARNING 4=MTCP_ERROR
        forker.OutputLevel         = 2  # 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
        ApplicationMgr().ExtSvc.append(forker)
        
    def _configureOnlineCheckpointing(self):
        pass

    def _configureOnlineMessageSvc(self):
        # setup the message service
        from Configurables import LHCb__FmcMessageSvc as MessageSvc
        if 'MessageSvc' in allConfigurables :
            del allConfigurables['MessageSvc']
        msg=MessageSvc('MessageSvc')
        app=ApplicationMgr()
        app.MessageSvcType = msg.getType()
        app.SvcOptMapping.append( msg.getFullName() )
        msg.LoggerOnly = True
        if 'LOGFIFO' not in os.environ :
            os.environ['LOGFIFO'] = '/tmp/logGaudi.fifo'
            log.warning( '# WARNING: LOGFIFO was not set -- now set to ' + os.environ['LOGFIFO'] )
        msg.fifoPath = os.environ['LOGFIFO']
        import OnlineEnv 
        msg.OutputLevel = OnlineEnv.OutputLevel
        msg.doPrintAlways = False

    def _configureDBSnapshot(self):
        tag = { "DDDB":     self.getProp('DDDBtag')
              , "LHCBCOND": self.getProp('CondDBtag')
              , "SIMCOND" : self.getProp('CondDBtag') 
              , "ONLINE"  : 'fake'
              }

        baseloc = self.getProp( "DBSnapshotDirectory" )

        from Configurables import CondDB
        conddb = CondDB()
        # hack to allow us to chance connectionstrings...
        conddb.UseOracle = True
        conddb.DisableLFC = True
        # Set alternative connection strings and tags
        # if simulation is False, we use DDDB, LHCBCOND and ONLINE
        #                  True          DDDB, SIMCOND
        # (see Det/DetCond's configurable... )
        dbPartitions = { False : [ "DDDB", "LHCBCOND", "ONLINE" ]
                       , True :  [ "DDDB", "SIMCOND" ]
                       }
        for part in dbPartitions[ self.getProp('Simulation') ] :
            if tag[part] is 'default' : raise KeyError('must specify an explicit %s tag'%part)
            conddb.PartitionConnectionString[part] = "sqlite_file:%(dir)s/%(part)s_%(tag)s.db/%(part)s" % {"dir":  baseloc,
                                                                                                           "part": part,
                                                                                                           "tag":  tag[part]}
            # always use HEAD -- blindly trust the snapshot to be
            # right (this is faster, but less safe)
            # conddb.Tags[part] = 'HEAD'
            conddb.Tags[part] = tag[part]

        # Set the location of the Online conditions
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

        conddb.IgnoreHeartBeat = self.getProp('IgnoreDBHeartBeat') 

        if self.getProp('EnableRunChangeHandler') : 
            import OnlineEnv 
            online_xml = '%s/%s/online_%%d.xml' % (baseloc, OnlineEnv.PartitionName )
            from Configurables import RunChangeHandlerSvc
            rch = RunChangeHandlerSvc()
            rch.Conditions = { "Conditions/Online/LHCb/Magnet/Set"        : online_xml
                             , "Conditions/Online/Velo/MotionSystem"      : online_xml
                             , "Conditions/Online/LHCb/Lumi/LumiSettings" : online_xml
                             , "Conditions/Online/LHCb/RunParameters"     : online_xml
                             , "Conditions/Online/Rich1/R1HltGasParameters" : online_xml
                             , "Conditions/Online/Rich2/R2HltGasParameters" : online_xml
                             }
            ApplicationMgr().ExtSvc.append(rch)


    def _configureInput(self):
        files = self.getProp('inputFiles')
        if len(files)==0 or 'DST' in [ _ext(f) for f in files ] :
            importOptions('$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts')
        if 'RAW' in [ _ext(f) for f in files ] :
            #  veto lumi events..
            #ApplicationMgr().EvtSel.REQ1 = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0;MaskType=ANY;UserType=USER;Frequency=PERC;Perc=100.0"
            EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        self._configureDataOnDemand()
        if files : EventSelector().Input = [ _datafmt(f) for f in files ]

    def _configureOutput(self):
        fname = self.getProp('outputFile')
        if not fname : return
        writer = None 
        if _ext(fname).upper() == 'RAW'  : 
            from Configurables import LHCb__MDFWriter as MDFWriter
            writer = MDFWriter( 'Writer'
                              , Compress = 0
                              , ChecksumType = 1
                              , GenerateMD5 = True
                              , Connection = 'file://' + fname
                              )
        if _ext(fname).upper() in ['DST','DIGI'] : 
            importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
            from Configurables import InputCopyStream
            writer = InputCopyStream("Writer"
                                    , RequireAlgs = self.getProp('WriterRequires')
                                    , Output = "DATAFILE='PFN:%s' TYP='POOL_ROOTTREE' OPT='REC'" % fname
                                    )
        if not writer : raise NameError('unsupported filetype for file "%s"'%fname)
        ApplicationMgr().OutStream.append( writer )

    def getRelease(self):
        import re,fileinput
        #  do not pick up the pz in vxrypz
        version = re.compile('^version (v\d+r\d+)(p\d+)?')
        for line in fileinput.input(os.environ.get('MOOREROOT')+'/cmt/requirements') :
            match = version.match(line)
            if not match: continue
            fileinput.close()
            return 'MOORE_'+match.group(1)
        raise TypeError('Could not deduce version from cmt/requirementes')

    def getConfigAccessSvc(self):
        method = self.getProp('TCKpersistency').lower()
        if method not in [ 'file', 'sqlite', 'tarfile' ] : raise TypeError("invalid TCK persistency '%s'"%method)
        TCKData = self.getProp('TCKData')
        if method == 'file' :
            from Configurables import ConfigFileAccessSvc
            return ConfigFileAccessSvc( Directory = TCKData +'/config' )
        if method == 'sqlite' :
            from Configurables import ConfigDBAccessSvc
            return ConfigDBAccessSvc( Connection = 'sqlite_file:' + TCKData +'/db/config.db' )
        if method == 'tarfile' :
            from Configurables import ConfigTarFileAccessSvc
            return ConfigTarFileAccessSvc( File = TCKData +'/config.tar' )

    def addAuditor(self,x) :
        if  'AuditorSvc' not in ApplicationMgr().ExtSvc : 
            ApplicationMgr().ExtSvc.append( 'AuditorSvc' ) 
        AuditorSvc().Auditors.append( x )
        x.Enable = True

    def _outputLevel(self) :
        from Configurables import Hlt__Service
        if not Hlt__Service().isPropertySet('Pedantic') : Hlt__Service().Pedantic = False
        # output levels...
        ToolSvc().OutputLevel                     = INFO
        from Configurables import XmlParserSvc 
        XmlParserSvc().OutputLevel                = WARNING
        MessageSvc().OutputLevel                  = INFO
        ApplicationMgr().OutputLevel              = ERROR
        SequencerTimerTool().OutputLevel          = WARNING
        # Print algorithm name with 40 characters
        MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'

    def _profile(self) :
        ApplicationMgr().AuditAlgorithms = 1
        auditors = self.getProp('EnableAuditor')
        if self.getProp('EnableTimer') : 
            from Configurables import TimingAuditor
            auditors = [ TimingAuditor('TIMER') ] + auditors
        for i in auditors : self.addAuditor( i )

    def _generateConfig(self) :
        from HltConf.ThresholdUtils import Name2Threshold
        settings = Name2Threshold(self.getProp('ThresholdSettings'))
        svcs = self.getProp("configServices")
        algs = self.getProp("configAlgorithms")
        if self.getProp('TCKpersistency').lower() == 'tarfile' :
            self.getConfigAccessSvc().Mode = 'ReadWrite'
            self.getConfigAccessSvc().OutputLevel = 1

        from Configurables import HltGenConfig
        print 'requesting following  svcs: %s ' % svcs
        gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in algs ]
                          , ConfigSvc = [ i.rsplit('/')[-1] for i in svcs ]
                          , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                          , HltType = settings.HltType()
                          , MooreRelease = self.getRelease()
                          , Label = self.getProp('configLabel'))
        # make sure gen is the very first Top algorithm...
        from HltLine.HltDecodeRaw import DecodeODIN
        ApplicationMgr().TopAlg = DecodeODIN.members() + [ gen.getFullName() ] + ApplicationMgr().TopAlg
        def genConfigAction() :
            def gather( c, overrule ) :
                    def check(config,prop,value) :
                        if prop not in config.getDefaultProperties() : return False
                        if hasattr(config,prop) : return getattr(config,prop) == value 
                        return config.getDefaultProperties()[prop] == value
                    def addOverrule(config,rule):
                        if c.name() not in overrule.keys() :
                           overrule[c.name()] = []     
                        if rule not in overrule[c.name()] :
                           overrule[c.name()] += [ rule ]

                    if check(c,'HistoProduce',False) :
                        addOverrule(c,'HistoProduce:@OnlineEnv.Monitor@False')
                    if c.getType() in ['FilterDesktop','CombineParticles' ] and  check(c,'Monitor',False) :
                        addOverrule(c,'Monitor:@OnlineEnv.Monitor@False')
                    if check(c,'Enable',False) :
                        addOverrule(c,'OutputLevel:3')
                    for p in [ 'Members','Filter0','Filter1' ] :
                        if not hasattr(c,p) : continue
                        x = getattr(c,p)
                        if list is not type(x) : x = [ x ]
                        for i in x : gather(i,overrule) 
            from Configurables import HltGenConfig,GaudiSequencer
            HltGenConfig().Overrule = { 'Hlt1ODINTechnicalPreScaler' : [ 'AcceptFraction:@OnlineEnv.AcceptRate@0' ] }
            gather( GaudiSequencer('Hlt'), HltGenConfig().Overrule )
            print HltGenConfig()

        from Gaudi.Configuration import appendPostConfigAction
        appendPostConfigAction( genConfigAction )

    def _l0(self) :
        from Configurables import L0DUFromRawAlg, L0DUFromRawTool
        #L0DUFromRawAlg('L0DUFromRaw').ProcessorDataOnTES = False
        l0du   = L0DUFromRawAlg("L0DUFromRaw")
        #l0du.WriteProcData       = False
        l0du.addTool(L0DUFromRawTool,name = "L0DUFromRawTool")
        l0du = getattr( l0du, 'L0DUFromRawTool' )
        #l0du.FillDataMap         = False
        #l0du.EncodeProcessorData = False
        #l0du.Emulate             = False
        l0du.StatusOnTES         = False

        if ( self.getProp("DataType") == 'DC06' and not self.getProp("L0") ):
            log.warning("It is mandatory to rerun the L0 emulation on DC06 data to get the HLT to work correctly")
            log.warning("Will set ReplaceL0BanksWithEmulated = True")
            self.setProp("ReplaceL0BanksWithEmulated",True)
        if ( self.getProp("ReplaceL0BanksWithEmulated") and not self.getProp("L0") ):
            log.warning("You asked to replace L0 banks with emulation. Will set L0 = True")
            self.setProp("L0",True)
        if ( self.getProp("L0") ):
            l0seq = GaudiSequencer("seqL0")
            ApplicationMgr().TopAlg += [ l0seq ]
            L0TCK = None
            if not self.getProp('UseTCK') :
                from HltConf.ThresholdUtils import Name2Threshold
                L0TCK = Name2Threshold(self.getProp('ThresholdSettings')).L0TCK()
            else  :
                L0TCK = '0x%s' % self.getProp('InitialTCK')[-4:]

            L0Conf().setProp( "TCK", L0TCK )
            L0Conf().setProp( "L0Sequencer", l0seq )
            self.setOtherProps( L0Conf(), [ "ReplaceL0BanksWithEmulated" , "DataType" ] )
            log.info("Will rerun L0")

    def _config_with_hltconf(self):
        hltConf = HltConf()
        self.setOtherProps( hltConf,  
                            [ 'ThresholdSettings'
                            , 'DataType','Verbose'
                            , 'RequireL0ForEndSequence'
                            , 'SkipHltRawBankOnRejectedEvents'
                            , 'HistogrammingLevel' 
                            , 'EnableMonitoring'
                            , "EnableLumiEventWriting"
                            , "EnableAcceptIfSlow"
                            , 'ForceSingleL0Configuration'
                            , 'SkipDisabledL0Channels'
                            , 'RequireRoutingBits'
                            , 'VetoRoutingBits' 
                            ]
                          )

    def _config_with_tck(self):
        from Configurables import HltConfigSvc
        cfg = HltConfigSvc( prefetchDir = self.getProp('prefetchConfigDir')
                          , initialTCK =  _tck(self.getProp('InitialTCK'))
                          , checkOdin = self.getProp('CheckOdin')
                          , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
        # TODO: make sure we are the first one...
        ApplicationMgr().ExtSvc.append(cfg.getFullName())
        # configure services...
        VFSSvc().FileAccessTools = ['FileReadTool', 'CondDBEntityResolver/CondDBEntityResolver'];
        from Configurables import LHCb__ParticlePropertySvc
        LHCb__ParticlePropertySvc().ParticlePropertiesFile = 'conddb:///param/ParticleTable.txt';
        ParticlePropertySvc().ParticlePropertiesFile = "conddb:///param/ParticleTable.txt";
        if (self.getProp('L0')) :
            if (self.getProp('RunOnline')) : raise RuntimeError('NEVER try to rerun L0 online! -- aborting ')
            from Hlt1Lines.HltL0Candidates import decodeL0Channels
            decodeL0Channels( '0x%04X' % ( int( _tck(self.getProp('InitialTCK') ),16) & 0xFFFF )
                            , skipDisabled               = self.getProp('SkipDisabledL0Channels')
                            , forceSingleL0Configuration = self.getProp('ForceSingleL0Configuration') 
                            )

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        # verify mutually exclusive settings:
        #  eg.  Online vs. any L0 setting
        #       Online vs. generateConfig
        #       Online vs. DB tags...
        #       Online vs. EvtMax, SkipEvents, DataType, ...
        #       Online requires UseTCK
        if not self.getProp("RunOnline") : self._l0()
        if self.getProp("RunOnline") : 
            import OnlineEnv 
            self.setProp('EnableTimer',False)
            self.setProp('UseTCK',True)
            self.setProp('Simulation',False)
            self.setProp('DataType','2010' )
            ### TODO: see if 'OnlineEnv' has InitialTCK attibute. If so, set it
            ## in case of LHCb or FEST, _REQUIRE_ it exists...
            if hasattr(OnlineEnv,'InitialTCK') :
                self.setProp('InitialTCK',OnlineEnv.InitialTCK)
                self.setProp('CheckOdin',True)
            # determine the partition we run in, and adapt settings accordingly...
            if OnlineEnv.PartitionName == 'FEST' or OnlineEnv.PartitionName == 'LHCb' :
                self.setProp('InitialTCK',OnlineEnv.InitialTCK)
                self.setProp('CheckOdin',True)


        from Configurables import MooreInitSvc
        ApplicationMgr().ExtSvc.append( MooreInitSvc() )

        ApplicationMgr().TopAlg.append( GaudiSequencer('Hlt') )


        # forward some settings... 
        # WARNING: this triggers setup of /dd -- could be avoided in PA only mode...
        app = LHCbApp()
        self.setOtherProps( app, ['EvtMax','SkipEvents','Simulation', 'DataType' ] )
        # this is a hack. Why does setOtherProps not work?
        app.CondDBtag = self.getProp('CondDBtag')
        app.DDDBtag   = self.getProp('DDDBtag')
        # Get the event time (for CondDb) from ODIN 
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # make sure we don't pick up small variations of the read current
        # Need a defined HistogramPersistency to read some calibration inputs!!!
        ApplicationMgr().HistogramPersistency = 'ROOT'
        self._outputLevel()

        if self.getProp('UseDBSnapshot') : self._configureDBSnapshot()

        if self.getProp('UseTCK') :
            self._config_with_tck()
        else:
            self._config_with_hltconf()
            

        if self.getProp("RunOnline") :
            self._configureOnline()
        else :
            self._profile()
            if self.getProp("generateConfig") : self._generateConfig()
            self._configureInput()
            self._configureOutput()
