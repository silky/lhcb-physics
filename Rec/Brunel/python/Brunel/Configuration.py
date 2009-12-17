## @package Brunel
#  High level configuration tools for Brunel
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.110 2009-12-17 15:20:10 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, RecSysConf, TrackSys, RecMoniConf,
                            GaudiSequencer, RichRecQCConf, DstConf, LumiAlgsConf, L0Conf )

## @class Brunel
#  Configurable for Brunel application
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class Brunel(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ TrackSys, RecSysConf, RecMoniConf, RichRecQCConf,
                               LHCbApp, DstConf, LumiAlgsConf, L0Conf ]

    ## Default init sequences
    DefaultInitSequence     = ["Reproc", "Brunel"]
    
    ## Known checking sequences, all run by default
    KnownCheckSubdets       = ["Pat","RICH","MUON"] 
    KnownExpertCheckSubdets = KnownCheckSubdets+["TT","IT","OT","Tr","CALO","PROTO"]
    ## Default main sequences for real and simulated data
    DefaultSequence = [ "ProcessPhase/Reco",
                        "ProcessPhase/Moni" ]
    DefaultMCSequence   = DefaultSequence + [ "ProcessPhase/MCLinks",
                                              "ProcessPhase/Check" ]
    
    # Steering options
    __slots__ = {
        "EvtMax"          : -1
       ,"SkipEvents"      : 0
       ,"PrintFreq"       : 1
       ,"DataType"        : "2009"
       ,"WithMC"          : False
       ,"Simulation"      : False
       ,"RecL0Only"       : False
       ,"InputType"       : "MDF"
       ,"DigiType"        : "Default"
       ,"OutputType"      : "DST"
       ,"PackType"        : "TES"
       ,"WriteFSR"        : True 
       ,"Histograms"      : "Default"
       ,"OutputLevel"     : INFO 
       ,"NoWarnings"      : False 
       ,"ProductionMode"  : False 
       ,"DatasetName"     : "Brunel"
       ,"DDDBtag"         : ""
       ,"CondDBtag"       : ""
       ,"MainSequence"    : None
       ,"InitSequence"    : None
       ,"RecoSequence"    : None
       ,"MCCheckSequence" : None
       ,"MCLinksSequence" : ["L0", "Unpack", "Tr"]
       ,"Monitors"        : []
       ,"SpecialData"     : []
       ,"Context"         : "Offline"
        }


    _propertyDocDct = { 
        'EvtMax'       : """ Maximum number of events to process (default -1, all events on input files) """
       ,'SkipEvents'   : """ Number of events to skip (default 0) """
       ,'PrintFreq'    : """ The frequency at which to print event numbers (default 1, prints every event) """
       ,'DataType'     : """ Data type, possible values defined in DDDBConf """
       ,'WithMC'       : """ Flags whether to enable processing with MC truth (default False) """
       ,'Simulation'   : """ Flags whether to use SIMCOND conditions (default False) """
       ,'RecL0Only'    : """ Flags whether to reconstruct and output only events passing L0 (default False) """
       ,'InputType'    : """ Type of input file. Can be one of self.KnownInputTypes (default 'MDF') """
       ,'DigiType'     : """ Type of digi, can be ['Minimal','Default','Extended'] """
       ,'OutputType'   : """ Type of output file. Can be one of self.KnownOutputTypes (default 'DST') """
       ,'PackType'     : """ Type of packing for the output file. Can be one of ['TES','MDF','NONE'] (default 'TES') """
       ,'WriteFSR'     : """ Flags whether to write out an FSR """
       ,'Histograms'   : """ Type of histograms. Can be one of self.KnownHistograms """
       ,'OutputLevel'  : """ The printout level to use (default INFO) """
       ,'NoWarnings'   : """ OBSOLETE, kept for Dirac compatibility. Please use ProductionMode """
       ,'ProductionMode'  : """ Enables special settings for running in production """
       ,'DatasetName'  : """ String used to build output file names """
       ,'DDDBtag'      : """ Tag for DDDB """
       ,'CondDBtag'    : """ Tag for CondDB """
       ,'MainSequence' : """ List of main sequences, default self.DefaultSequence """
       ,'InitSequence' : """ List of initialisation sequences """
       ,'RecoSequence' : """ List of reconstruction sequences, default defined in RecSys """
       ,'MCLinksSequence' : """ List of MC truth link sequences """
       ,'MCCheckSequence' : """ List of MC Check sequence, default self.KnownCheckSubdets """
       ,'Monitors'     : """ List of monitors to execute, see self.KnownMonitors. Default none """
       ,'SpecialData'  : """ Various special data processing options. See RecSys.KnownSpecialData for all options """
       ,'Context'      : """ The context within which to run (default 'Offline') """
       }

    KnownInputTypes  = [ "MDF",  "DST", "RDST", "XDST", "DIGI", "ETC" ]
    KnownOutputTypes = [ "NONE", "DST", "RDST", "XDST" ]
    KnownHistograms  = [ "None", "Default", "Expert" ]

    def defineGeometry(self):
        # DIGI is always simulation, as is usage of MC truth!
        if self.getProp( "WithMC" ) or self.getProp( "InputType" ).upper() == 'DIGI':
            self.setProp( "Simulation", True )

        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["DataType","CondDBtag","DDDBtag","Simulation"])

    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

    def defineOptions(self):

        inputType = self.getProp( "InputType" ).upper()
        if inputType not in self.KnownInputTypes:
            raise TypeError( "Invalid inputType '%s'"%inputType )

        outputType = self.getProp( "OutputType" ).upper()
        if outputType not in self.KnownOutputTypes:
            raise TypeError( "Invalid outputType '%s'"%outputType )

        histOpt = self.getProp("Histograms").capitalize()
        if histOpt not in self.KnownHistograms:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        withMC = self.getProp("WithMC")
        if withMC:
            if inputType in [ "MDF", "RDST" ]:
                log.warning( "WithMC = True, but InputType = '%s'. Forcing WithMC = False"%inputType )
                withMC = False # Force it, MDF and RDST never contain MC truth
            if outputType == "RDST":
                log.warning( "WithMC = True, but OutputType = '%s'. Forcing WithMC = False"%inputType )
                withMC = False # Force it, RDST never contains MC truth

        if self.getProp("WriteFSR") and self.getProp("PackType").upper() in ["MDF"]:
            if hasattr( self, "WriteFSR" ): log.warning("Don't know how to write FSR to MDF output file")
            self.setProp("WriteFSR", False)


        # Flag to handle or not LumiEvents
        #handleLumi = inputType in ["MDF","ETC"] and not withMC
        #don't do it for ETC, lumi has no meaning on an ETC, as we can't say you processed everything
        handleLumi = inputType in ["MDF"] and not withMC
        
        self.configureSequences( withMC, handleLumi )

        self.configureInit( inputType )
        
        self.configureInput( inputType )

        self.configureOutput( outputType, withMC, handleLumi )

        if withMC:
            # Create associators for checking and for DST
            from Configurables import ProcessPhase
            ProcessPhase("MCLinks").DetectorList += self.getProp("MCLinksSequence")
            # Unpack Sim data
            GaudiSequencer("MCLinksUnpackSeq").Members += [ "UnpackMCParticle",
                                                            "UnpackMCVertex" ]
            GaudiSequencer("MCLinksTrSeq").Members += [ "TrackAssociator" ]

            # activate all configured checking (uses MC truth)
            self.configureCheck( histOpt == "Expert" )
            
            # data on demand needed to pack RichDigitSummary for DST, when reading unpacked DIGI
            # Also needed to unpack MCHit containers when expert checking enabled
            ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]

        # ROOT persistency for histograms
        importOptions('$STDOPTS/RootHist.opts')
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        if histOpt == "None" or histOpt == "":
            # HistogramPersistency still needed to read in CaloPID DLLs.
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        # Pass expert checking option to RecSys and RecMoni
        if histOpt == "Expert":
            RecSysConf().setProp( "ExpertHistos", True )
            RecMoniConf().setProp( "ExpertHistos", True )
            DstConf().EnablePackingChecks = True

        # Use a default histogram file name if not already set
        if not HistogramPersistencySvc().isPropertySet( "OutputFile" ):
            histosName   = self.getProp("DatasetName")
            if histosName == "": histosName = "Brunel"
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

        # Use TimingAuditor for timing, suppress printout from SequencerTimerTool
        from Configurables import (ApplicationMgr,AuditorSvc,SequencerTimerTool)
        ApplicationMgr().ExtSvc += [ 'ToolSvc', 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ] 
        SequencerTimerTool().OutputLevel = 4
        
    def configureSequences(self, withMC, handleLumi):
        brunelSeq = GaudiSequencer("BrunelSequencer")
        brunelSeq.Context = self.getProp("Context")
        ApplicationMgr().TopAlg += [ brunelSeq ]
        brunelSeq.Members += [ "ProcessPhase/Init" ]
        physicsSeq = GaudiSequencer( "PhysicsSeq" )

        #treatment of the FSR
        lumiSeq = GaudiSequencer("LumiSeq")
        if self.getProp("WriteFSR") and self.getProp("InputType") in ['MDF','DST']:
            self.setOtherProps(LumiAlgsConf(),["Context","DataType","InputType"])
            lumiCounters = GaudiSequencer("LumiCounters")
            lumiSeq.Members += [ lumiCounters ]
            LumiAlgsConf().LumiSequencer = lumiCounters
                                                                            
        
        # Treatment of luminosity events
        if handleLumi:
            # Filter out Lumi only triggers from further processing, but still write to output
            from Configurables import HltRoutingBitsFilter
            physFilter = HltRoutingBitsFilter( "PhysFilter", RequireMask = [ 0x0, 0x4, 0x0 ] )
            physicsSeq.Members += [ physFilter ]
            lumiFilter = HltRoutingBitsFilter( "LumiFilter", RequireMask = [ 0x0, 0x2, 0x0 ] )
            lumiSeq.Members += [ lumiFilter, physFilter ]
            lumiSeq.ModeOR = True
            # Call DST packing algorithms if physics sequence not called
            notPhysSeq = GaudiSequencer("NotPhysicsSeq")
            notPhysSeq.ModeOR = True
            dummyPackerSeq = GaudiSequencer("PackDST")
            notPhysSeq.Members = [ physFilter, dummyPackerSeq ]

            brunelSeq.Members += [ lumiSeq, notPhysSeq ]

        # Convert Calo 'packed' banks to 'short' banks if needed
        physicsSeq.Members += ["GaudiSequencer/CaloBanksHandler"]

        # Setup L0 filtering if requested, runs L0 before Reco
        if self.getProp("RecL0Only"):
            l0TrgSeq = GaudiSequencer("L0TriggerSeq")
            physicsSeq.Members += [ l0TrgSeq ]
            L0Conf().L0Sequencer = l0TrgSeq
            L0Conf().FilterL0FromRaw = True
            self.setOtherProps( L0Conf(), ["DataType"] )

        importOptions("$CALODAQROOT/options/CaloBankHandler.opts")
        if not self.isPropertySet("MainSequence"):
            if withMC:
                mainSeq = self.DefaultMCSequence
            else:
                mainSeq = self.DefaultSequence
            self.MainSequence = mainSeq
        physicsSeq.Members += self.getProp("MainSequence")
        from Configurables import ProcessPhase
        outputPhase = ProcessPhase("Output")
        brunelSeq.Members  += [ physicsSeq, outputPhase ]

    def configureInit(self, inputType):
        # Init sequence
        if not self.isPropertySet("InitSequence"):
            self.setProp( "InitSequence", self.DefaultInitSequence )
        from Configurables import ProcessPhase
        ProcessPhase("Init").DetectorList += self.getProp("InitSequence")

        from Configurables import RecInit, MemoryTool
        recInit = RecInit( "BrunelInit",
                           PrintFreq = self.getProp("PrintFreq"))
        GaudiSequencer("InitBrunelSeq").Members += [ recInit ]
        recInit.addTool( MemoryTool(), name = "BrunelMemory" )
        recInit.BrunelMemory.HistoTopDir = "Brunel/"
        recInit.BrunelMemory.HistoDir    = "MemoryTool"
        # count events
        from Configurables import EventCountHisto
        evtC = EventCountHisto("BrunelEventCount")
        evtC.HistoTopDir = "Brunel/"
        GaudiSequencer("InitBrunelSeq").Members += [ evtC ]
        
        # Do not print event number at every event (done already by BrunelInit)
        EventSelector().PrintFreq = -1

        # Kept for Dirac backward compatibility
        if self.getProp( "NoWarnings" ) :
            log.warning("Brunel().NoWarnings=True property is obsolete and maintained for Dirac compatibility. Please use Brunel().ProductionMode=True instead")
            self.setProp( "ProductionMode", True )
            
        # Special settings for production
        if self.getProp( "ProductionMode" ) :
            self.setProp("OutputLevel", ERROR)
            if not LHCbApp().isPropertySet( "TimeStamp" ) :
                LHCbApp().setProp( "TimeStamp", True )

        # OutputLevel
        self.setOtherProp(LHCbApp(),"OutputLevel")
        if self.isPropertySet( "OutputLevel" ) :
            level = self.getProp("OutputLevel")
            if level == ERROR or level == WARNING :
                # Suppress known warnings
                importOptions( "$BRUNELOPTS/SuppressWarnings.opts" )
                if not recInit.isPropertySet( "OutputLevel" ): recInit.OutputLevel = INFO
        self.setOtherProps(RecSysConf(), ["OutputLevel"])
        self.setOtherProps(RecMoniConf(),["OutputLevel"])

        # Switch off LoKi banner
        from Configurables import LoKiSvc
        LoKiSvc().Welcome = False


    def configureInput(self, inputType):
        """
        Tune initialisation according to input type
        """

        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        # By default, Brunel only needs to open one input file at a time
        # Only set to zero if not previously set to something else.
        if not IODataManager().isPropertySet("AgeLimit") : IODataManager().AgeLimit = 0
        
        if inputType in [ "XDST", "DST", "RDST", "ETC" ]:
            # Kill knowledge of any previous Brunel processing
            from Configurables import ( TESCheck, EventNodeKiller )
            InitReprocSeq = GaudiSequencer( "InitReprocSeq" )
            if ( self.getProp("WithMC") and inputType in ["XDST","DST"] ):
                # Load linkers, to kill them (avoid appending to them later)
                InitReprocSeq.Members.append( "TESCheck" )
                TESCheck().Inputs = ["Link/Rec/Track/Best"]
            InitReprocSeq.Members.append( "EventNodeKiller" )
            EventNodeKiller().Nodes = [ "pRec", "Rec", "Raw", "Link/Rec" ]

        if inputType == "ETC":
            from Configurables import  TagCollectionSvc
            ApplicationMgr().ExtSvc  += [ TagCollectionSvc("EvtTupleSvc") ]
            # Read ETC selection results into TES for writing to DST
            IODataManager().AgeLimit += 1

        if inputType in [ "MDF", "RDST", "ETC" ]:
            # In case raw data resides in MDF file
            EventPersistencySvc().CnvServices.append("LHCb::RawDataCnvSvc")

        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";

    def configureOutput(self, dstType, withMC, handleLumi):
        """
        Set up output stream
        """
        if dstType in [ "XDST", "DST", "RDST" ]:
            writerName = "DstWriter"
            packType  = self.getProp( "PackType" )
            # Do not pack DC06 DSTs, for consistency with existing productions
            if self.getProp("DataType") == "DC06": packType = "NONE"

            # event output
            dstWriter = OutputStream( writerName )
            dstWriter.AcceptAlgs += ["Reco"] # Write only if Rec phase completed
            if handleLumi:
                dstWriter.AcceptAlgs += ["LumiSeq"] # Write also if Lumi sequence completed
            # Set a default output file name if not already defined in the user job options
            if not dstWriter.isPropertySet( "Output" ):
                outputFile = self.outputName()
                outputFile = outputFile + '.' + self.getProp("OutputType").lower()
                dstWriter.Output = "DATAFILE='PFN:" + outputFile + "' TYP='POOL_ROOTTREE' OPT='REC'"
            if self.getProp( "ProductionMode" ) and not dstWriter.isPropertySet( "OutputLevel" ):
                dstWriter.OutputLevel = INFO

            # FSR output stream
            if self.getProp("WriteFSR"):
                FSRWriter = RecordStream( "FSRWriter",
                                          ItemList = [ "/FileRecords#999" ],
                                          EvtDataSvc = "FileRecordDataSvc",
                                          EvtConversionSvc = "FileRecordPersistencySvc" )
                # Write the FSRs to the same file as the events
                if FSRWriter.isPropertySet( "Output" ):
                    log.warning("Ignoring FSRWriter.Output option, FSRs will be written to same file as the events")
                FSRWriter.Output = dstWriter.getProp("Output")

                ApplicationMgr().OutStream.append(FSRWriter)
                if self.getProp( "ProductionMode" ) and not FSRWriter.isPropertySet( "OutputLevel" ):
                    FSRWriter.OutputLevel = INFO
                # Suppress spurious error when reading POOL files without run records
                if self.getProp( "InputType" ).upper() not in [ "MDF" ]:
                    from Configurables import FileRecordDataSvc
                    FileRecordDataSvc().OutputLevel = FATAL

            if dstType == "XDST":
                # Allow multiple files open at once (SIM,DST,DIGI etc.)
                IODataManager().AgeLimit += 1

            from Configurables import TrackToDST
            if dstType == "RDST":
                # Sequence for altering content of rDST compared to DST
                # Filter Track States to be written
                trackFilter = TrackToDST("TrackToRDST")
                trackFilter.veloStates = ["ClosestToBeam"]
                trackFilter.longStates = ["ClosestToBeam"]
                trackFilter.TTrackStates = ["FirstMeasurement"]
                trackFilter.downstreamStates = ["FirstMeasurement"]
                trackFilter.upstreamStates = ["ClosestToBeam"]
            else:
                # Sequence for altering DST content
                # Filter Track States to be written
                trackFilter = TrackToDST()
                
            from Configurables import ProcessPhase
            ProcessPhase("Output").DetectorList += [ "DST" ]
            GaudiSequencer("OutputDSTSeq").Members += [ trackFilter ]

            if packType != "NONE":
                # Add the sequence to pack the DST containers
                packSeq = GaudiSequencer("PackDST")
                DstConf().PackSequencer = packSeq
                DstConf().AlwaysCreate  = True
                GaudiSequencer("OutputDSTSeq").Members += [ packSeq ]
                # Run the packers also on Lumi only events to write empty containers
#                GaudiSequencer("DummyPackerSeq").Members += [ packSeq ]

            # Define the file content
            DstConf().Writer     = writerName
            DstConf().DstType    = dstType
            DstConf().PackType   = packType
            if withMC:
                DstConf().SimType = "Full"
            elif self.getProp("DigiType").capitalize() == "Minimal":
                from Configurables import PackMCVertex
                GaudiSequencer("OutputDSTSeq").Members += [PackMCVertex()]
                DstConf().SimType = "Minimal"
            DstConf().OutputName = self.outputName()
            self.setOtherProps(DstConf(),["DataType"])

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if self.getProp( "RecL0Only" ): outputName += '-L0Yes'
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        return outputName

    def evtMax(self):
        return LHCbApp().evtMax()

    def configureCheck(self,expert):
        # "Check" histograms filled only with simulated data

        RecMoniConf().setProp( "CheckEnabled", True )
        
        if not self.isPropertySet("MCCheckSequence"):
            if expert:
                checkSeq = self.KnownExpertCheckSubdets
            else:
                checkSeq = self.KnownCheckSubdets
            self.MCCheckSequence = checkSeq
        else:
            for seq in self.getProp("MCCheckSequence"):
                if expert:
                    if seq not in self.KnownExpertCheckSubdets:
                        log.warning("Unknown subdet '%s' in MCCheckSequence"%seq)
                else:
                    if seq not in self.KnownCheckSubdets:
                        log.warning("Unknown subdet '%s' in MCCheckSequence"%seq)
                        
        checkSeq = self.getProp("MCCheckSequence")
        from Configurables import ProcessPhase
        ProcessPhase("Check").DetectorList += checkSeq

        # Tracking handled inside TrackSys configurable
        TrackSys().setProp( "WithMC", True )

        if "MUON" in checkSeq :
            from MuonPIDChecker import ConfigureMuonPIDChecker as cmuon
            mydata =  self.getProp("DataType")
            mycheckconf = cmuon.ConfigureMuonPIDChecker(data = mydata)
            mycheckconf.configure(mc = True, expertck = expert)

        if "RICH" in checkSeq :
            # Unpacking RICH summaries
            from Configurables import DataPacking__Unpack_LHCb__MCRichDigitSummaryPacker_
            unp = DataPacking__Unpack_LHCb__MCRichDigitSummaryPacker_("MCRichDigitSummaryUnpack")
            from Configurables import GaudiSequencer
            GaudiSequencer("MCLinksUnpackSeq").Members += [unp]

            self.setOtherProps(RichRecQCConf(), ["Context","OutputLevel","DataType","WithMC"])
            RichRecQCConf().setProp("MoniSequencer", GaudiSequencer("CheckRICHSeq"))

        if expert:
            # Data on Demand for MCParticle to MCHit association, needed by ST, IT, OT, Tr, Muon
            importOptions( "$ASSOCIATORSROOT/options/MCParticleToMCHit.py" )

            # Allow multiple files open at once (SIM,DST,DIGI etc.)
            IODataManager().AgeLimit += 1
        
            if "TT" in checkSeq :
                from Configurables import ( STEffChecker, MCParticleSelector )
                from GaudiKernel.SystemOfUnits import GeV
                effCheck = STEffChecker("TTEffChecker")
                effCheck.FullDetail = True
                effCheck.addTool(MCParticleSelector)
                effCheck.MCParticleSelector.zOrigin = 50.0
                effCheck.MCParticleSelector.pMin = 1.0*GeV
                effCheck.MCParticleSelector.betaGammaMin = 1.0
                GaudiSequencer("CheckTTSeq").Members += [effCheck]

            if "IT" in checkSeq :
                from Configurables import ( STEffChecker, MCParticleSelector )
                from GaudiKernel.SystemOfUnits import GeV
                effCheck = STEffChecker("ITEffChecker")
                effCheck.FullDetail = True
                effCheck.addTool(MCParticleSelector)
                effCheck.MCParticleSelector.zOrigin = 50.0
                effCheck.MCParticleSelector.pMin = 1.0*GeV
                effCheck.MCParticleSelector.betaGammaMin = 1.0
                effCheck.DetType = "IT"
                GaudiSequencer("CheckITSeq").Members += [effCheck]

            if "OT" in checkSeq :
                GaudiSequencer("CheckOTSeq").Members += ["OTTimeCreator", "OTTimeChecker"] # needs MCHits

            if "Tr" in  checkSeq :
                # Checking on the tracks in the "best" container - needs MCHits
                importOptions( "$TRACKSYSROOT/options/TrackChecking.opts" )

            if "CALO" in  checkSeq :
                GaudiSequencer("CheckCALOSeq").Members += [ "CaloDigit2MCLinks2Table", "CaloClusterMCTruth" ]
                importOptions( "$STDOPTS/PreloadUnits.opts" )
                importOptions( "$CALOMONIDSTOPTS/CaloChecker.opts" )

            if "RICH" in checkSeq :
                RichRecQCConf().setProp( "ExpertHistos", True )

            if "PROTO" in checkSeq :
                from Configurables import ( GaudiSequencer, NTupleSvc, ChargedProtoParticleTupleAlg )
                protoChecker = ChargedProtoParticleTupleAlg("ChargedProtoTuple")
                protoChecker.NTupleLUN = "PROTOTUPLE"
                GaudiSequencer("CheckPROTOSeq").Members += [protoChecker]
                NTupleSvc().Output += ["PROTOTUPLE DATAFILE='protoparticles.tuples.root' TYP='ROOT' OPT='NEW'"]

    ## Apply the configuration
    def __apply_configuration__(self):
        
        GaudiKernel.ProcessJobOptions.PrintOff()
        self.setOtherProps(RecSysConf(),["SpecialData","Context",
                                         "OutputType","DataType"])
        self.setOtherProps(RecMoniConf(),["Context","DataType"])
        if self.isPropertySet("RecoSequence") :
            self.setOtherProp(RecSysConf(),"RecoSequence")
        self.defineGeometry()
        self.defineEvents()
        self.defineOptions()
        self.defineMonitors()
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        log.info( RecSysConf() )
        log.info( TrackSys() )
        log.info( RecMoniConf() )
        GaudiKernel.ProcessJobOptions.PrintOff()
