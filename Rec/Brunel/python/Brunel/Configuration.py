## @package Brunel
#  High level configuration tools for Brunel
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.40 2008-12-04 13:38:28 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys,
                            ProcessPhase, GaudiSequencer, RichRecQCConf )

## @class Brunel
#  Configurable for Brunel application
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class Brunel(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ TrackSys, RecSysConf, RichRecQCConf, LHCbApp ]
    
    # Steering options
    __slots__ = {
        "EvtMax":          -1 # Maximum number of events to process
       ,"SkipEvents":   0     # events to skip
       ,"PrintFreq":    1     # The frequency at which to print event numbers
       ,"DataType"   : "2008" # Data type, can be ['DC06','2008']
       ,"WithMC":       False # set to True to use MC truth
       ,"Simulation":   False # set to True to use SimCond
       ,"RecL0Only":    False # set to True to reconstruct only L0-yes events
       ,"InputType":    "MDF" # or "DIGI" or "ETC" or "RDST" or "DST"
       ,"OutputType":   "DST" # or "RDST" or "NONE"
       ,"Histograms": "Default" # Type of histograms: ['None','Default','Expert']
       ,"NoWarnings":   False # suppress all messages with MSG::WARNING or below 
       ,"DatasetName":  ""    # string used to build file names
       ,"DDDBtag":      "" # Tag for DDDB. Default as set in DDDBConf for DataType
       ,"CondDBtag":    "" # Tag for CondDB. Default as set in DDDBConf for DataType
       ,"UseOracle": False  # if False, use SQLDDDB instead
       ,"MainSequence": [ "ProcessPhase/Init",
                          "ProcessPhase/Reco",
                          "ProcessPhase/Moni",
                          "ProcessPhase/MCLinks",
                          "ProcessPhase/Check",
                          "ProcessPhase/Output" ]
       ,"MCCheckSequence": ["Pat","RICH","MUON"] # The default MC Check sequence
       ,"MCLinksSequence": [ "L0", "Unpack", "Tr" ] # The default MC Link sequence
       ,"InitSequence": ["Reproc", "Brunel", "Calo"] # The default init sequence
       ,"MoniSequence": ["CALO","RICH","MUON","VELO","Track","ST"]    # The default Moni sequence
       ,"Monitors": []        # list of monitors to execute, see KnownMonitors
        # Following are options forwarded to RecSys
       ,"RecoSequence"   : [] # The Sub-detector reconstruction sequencing. See RecSys for default
       ,"SpecialData"    : [] # Various special data processing options. See KnownSpecialData for all options
        # Following are options forwarded to TrackSys
       ,"ExpertTracking": []  # list of expert Tracking options, see KnownExpertTracking
       ,"Context":     "Offline" # The context within which to run
        }

    def defineGeometry(self):
        # DIGI is always simulation, as is usage of MC truth!
        if self.getProp( "WithMC" ) or self.getProp( "InputType" ).upper() == 'DIGI':
            self.setProp( "Simulation", True )

        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","UseOracle","Simulation"]) 

    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

    def defineOptions(self):

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in [ "MDF", "DST", "DIGI", "ETC", "RDST" ]:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()
        if outputType not in [ "NONE", "DST", "RDST" ]:
            raise TypeError( "Invalid outputType '%s'"%outputType )

        withMC = self.getProp("WithMC")
        if inputType in [ "MDF", "RDST" ]:
            withMC = False # Force it, MDF and RDST never contain MC truth
        if outputType == "RDST":
            withMC = False # Force it, RDST never contains MC truth

        self.configureInput( inputType )

        self.configureOutput( outputType, withMC )

        # Set up monitoring (i.e. not using MC truth)
        ProcessPhase("Moni").DetectorList += self.getProp("MoniSequence")
        self.setOtherProps(RichRecQCConf(),["Context","DataType"])

        # Histograms filled both in real and simulated data cases
        importOptions("$BRUNELOPTS/BrunelMoni.py")

        if withMC:
            # Setup up MC truth processing and checking
            ProcessPhase("MCLinks").DetectorList += self.getProp("MCLinksSequence")
            # Unpack Sim data
            GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCParticle",
                                                            "UnpackMCVertex" ]
            GaudiSequencer("MCLinksTrSeq").Members += [ "TrackAssociator" ]

            # "Check" histograms filled only with simulated data 
            ProcessPhase("Check").DetectorList += self.getProp("MCCheckSequence")
            # Tracking handled inside TrackSys configurable
            TrackSys().setProp( "WithMC", True )
            # Muon
            importOptions("$MUONPIDCHECKERROOT/options/MuonPIDChecker.opts")
            # RICH
            RichRecQCConf().MoniSequencer = GaudiSequencer("CheckRICHSeq")
        else:
            # Add here histograms to be filled only with real data 
            RichRecQCConf().MoniSequencer = GaudiSequencer("MoniRICHSeq")

        # Setup L0 filtering if requested, runs L0 before Reco
        if self.getProp("RecL0Only"):
            ProcessPhase("Init").DetectorList.append("L0")
            importOptions( "$L0DUROOT/options/L0Sequence.opts" )
            GaudiSequencer("InitL0Seq").Members += [ "GaudiSequencer/L0FilterFromRawSeq" ]

    def defineHistos(self):
        """
        Define histograms to save according to Brunel().Histograms option
        """
        knownOptions = ["","None","Default","Expert"]
        histOpt = self.getProp("Histograms").capitalize()
        if histOpt not in knownOptions:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        if histOpt == "None" or histOpt == "":
            # HistogramPersistency still needed to read in CaloPID DLLs.
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        if histOpt == "Expert":
            # activate all configured expert checking
            self.expertCheck()

        # Use a default histogram file name if not already set
        if not hasattr( HistogramPersistencySvc(), "OutputFile" ):
            histosName   = self.getProp("DatasetName")
            if self.getProp( "RecL0Only" ): histosName += '-L0Yes'
            if (self.evtMax() > 0): histosName += '-' + str(self.evtMax()) + 'ev'
            if histOpt == "Expert": histosName += '-expert'
            histosName += '-histos.root'
            HistogramPersistencySvc().OutputFile = histosName

    def defineMonitors(self):
        
        # get all defined monitors
        monitors = self.getProp("Monitors") + LHCbApp().getProp("Monitors")
        # Currently no Brunel specific monitors, so pass them all to LHCbApp
        LHCbApp().setProp("Monitors", monitors)

    def configureInput(self, inputType):
        """
        Tune initialisation according to input type
        """

        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        # By default, Brunel only needs to open one input file at a time
        IODataManager().AgeLimit = 0
        
        if inputType in [ "DST", "RDST", "ETC" ]:
            # Kill knowledge of any previous Brunel processing
            from Configurables import ( TESCheck, EventNodeKiller )
            InitReprocSeq = GaudiSequencer( "InitReprocSeq" )
            InitReprocSeq.Members.append( "TESCheck" )
            TESCheck().Inputs = ["Link/Rec/Track/Best"]
            # in case above container is not on input (e.g. RDST)
            TESCheck().Stop = False
            TESCheck().OutputLevel = ERROR
            InitReprocSeq.Members.append( "EventNodeKiller" )
            EventNodeKiller().Nodes = [ "Rec", "Raw", "Link/Rec" ]

        # Read ETC selection results into TES for writing to DST
        if inputType == "ETC":
            from Configurables import ReadStripETC
            GaudiSequencer("InitBrunelSeq").Members.append("ReadStripETC/TagReader")
            ReadStripETC("TagReader").CollectionName = "TagCreator"
            IODataManager().AgeLimit += 1

        if inputType not in [ "DIGI", "DST" ]:
            # In case raw data resides in MDF file
            EventPersistencySvc().CnvServices.append("LHCb::RawDataCnvSvc")

        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";

        # Convert Calo 'packed' banks to 'short' banks if needed
        GaudiSequencer("InitCaloSeq").Members += ["GaudiSequencer/CaloBanksHandler"]
        importOptions("$CALODAQROOT/options/CaloBankHandler.opts")


    def configureOutput(self, dstType, withMC):
        """
        Set up output stream
        """
        if dstType in [ "DST", "RDST" ]:
            if( dstType == "DST" ):
                importOptions( "$BRUNELOPTS/DstContent.opts" )
            else:
                importOptions( "$BRUNELOPTS/rDstContent.opts" )
            dstWriter = OutputStream( "DstWriter" )
            ApplicationMgr().OutStream.append( dstWriter )
            dstWriter.Preload = False
            dstWriter.RequireAlgs += ["Reco"] # Write only if Rec phase completed
            if not hasattr( dstWriter, "Output" ):
                dstWriter.Output  = "DATAFILE='PFN:" + self.outputName() + "' TYP='POOL_ROOTTREE' OPT='REC'"

        # Add MC truth to DST
        if withMC and dstType == "DST":
            importOptions("$STDOPTS/MCDstContent.opts")
            
        # Always write an ETC if ETC input
        if self.getProp( "InputType" ).upper() == "ETC":
            etcWriter = TagCollectionSvc("EvtTupleSvc")
            ApplicationMgr().ExtSvc.append(etcWriter)
            ApplicationMgr().OutStream.append("GaudiSequencer/SeqTagWriter")
            importOptions( "$BRUNELOPTS/DefineETC.opts" )
            if not hasattr( etcWriter, "Output" ):
               etcWriter.Output = [ "EVTTAGS2 DATAFILE='" + self.getProp("DatasetName") + "-etc.root' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]

        # Do not print event number at every event (done already by Brunel)
        EventSelector().PrintFreq = -1;
        # Modify printout defaults
        if self.getProp( "NoWarnings" ):
            importOptions( "$BRUNELOPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if self.getProp( "RecL0Only" ): outputName += '-L0Yes'
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        outputType = self.getProp("OutputType").lower()
        return outputName + '.' + outputType

    def evtMax(self):
        return LHCbApp().evtMax()

    def expertCheck(self):

        # Get the list of sub dets configured to run in the Check sequence
        checkSeq = self.getProp("MCCheckSequence")
        # Get the list of sub dets configured to run in the Moni sequence
        moniSeq  = self.getProp("MoniSequence")

        # Data on Demand for MCParticle to MCHit association, needed by ST, IT, Tr
        ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]
        importOptions( "$ASSOCIATORSROOT/options/MCParticleToMCHit.py" )

        # TT
        if "TT" in moniSeq :
            from Configurables import STClusterMonitor
            clusMoni = STClusterMonitor("TTClusterMonitor")
            clusMoni.FullDetail = True
            GaudiSequencer("MoniTTSeq").Members += [clusMoni]
        if "TT" in checkSeq :
            from Configurables import ( STEffChecker, MCParticleSelector )
            effCheck = STEffChecker("TTEffChecker")
            effCheck.FullDetail = True
            effCheck.addTool(MCParticleSelector)
            effCheck.MCParticleSelector.zOrigin = 50.0
            effCheck.MCParticleSelector.pMin = 1.0*GeV
            effCheck.MCParticleSelector.betaGammaMin = 1.0
            GaudiSequencer("CheckTTSeq").Members += [effCheck]

        # IT
        if "IT" in moniSeq :
            from Configurables import STClusterMonitor
            clusMoni = STClusterMonitor("ITClusterMonitor")
            clusMoni.FullDetail = True
            clusMoni.DetType = "IT"
            GaudiSequencer("MoniITSeq").Members += [clusMoni]
        if "IT" in checkSeq :
            from Configurables import ( STEffChecker, MCParticleSelector )
            effCheck = STEffChecker("ITEffChecker")
            effCheck.FullDetail = True
            effCheck.addTool(MCParticleSelector)
            effCheck.MCParticleSelector.zOrigin = 50.0
            effCheck.MCParticleSelector.pMin = 1.0*GeV
            effCheck.MCParticleSelector.betaGammaMin = 1.0
            effCheck.DetType = "IT"
            GaudiSequencer("CheckITSeq").Members += [effCheck]

        # OT - These histograms should be identical to those already done in Boole.
        if "OT" in moniSeq  : GaudiSequencer("MoniOTSeq").Members  += ["OTTimeMonitor"]
        if "OT" in checkSeq : GaudiSequencer("CheckOTSeq").Members += ["OTTimeChecker"] # needs MCHits

        # Checking on the tracks in the "best" container - needs MCHits
        if "Tr" in  checkSeq :
            importOptions( "$TRACKSYSROOT/options/TrackChecking.opts" )

        # Calorimeters
        if "CALO" in  checkSeq : 
            importOptions( "$CALOASSOCIATORSROOT/options/CaloAssociators.opts" )
            importOptions( "$CALOMONIDSTOPTS/CaloChecker.opts" )

        # RICH
        if "RICH" in checkSeq :
            from RichRecQC.Configuration import RichRecQCConf
            RichRecQCConf().setProp( "ExpertHistos", True )

        # ProtoParticles
        if "PROTO" in checkSeq :
            from Configurables import ( NTupleSvc, ChargedProtoParticleTupleAlg )
            protoChecker = ChargedProtoParticleTupleAlg("ChargedProtoTuple")
            protoChecker.NTupleLUN = "PROTOTUPLE"
            GaudiSequencer("CheckPROTOSeq").Members += [protoChecker]
            NTupleSvc().Output += ["PROTOTUPLE DATAFILE='protoparticles.tuples.root' TYP='ROOT' OPT='NEW'"]

        # Pass expert checking option to RecSys
        RecSysConf().setProp( "ExpertHistos", True )

        # Allow multiple files open at once (SIM,DST,DIGI etc.)
        IODataManager().AgeLimit += 1
        
    ## Apply the configuration
    def __apply_configuration__(self):
        
        log.info( self )
        GaudiKernel.ProcessJobOptions.PrintOff()
        self.setOtherProp(TrackSys(),"ExpertTracking")
        self.setOtherProps(RecSysConf(),["SpecialData","RecoSequence"])
        brunelSeq = GaudiSequencer("BrunelSequencer")
        ApplicationMgr().TopAlg = [ brunelSeq ]
        brunelSeq.Members += self.getProp("MainSequence")
        ProcessPhase("Init").DetectorList += self.getProp("InitSequence")
        GaudiSequencer("InitBrunelSeq").Members += [ "RecInit/BrunelInit" ]
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineHistos()
        self.defineMonitors()
        from Configurables import RecInit
        RecInit("BrunelInit").PrintFreq = self.getProp("PrintFreq")
