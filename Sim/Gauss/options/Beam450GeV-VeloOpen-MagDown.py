# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam450GeV-VeloOpen-MagDown
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam450GeV-VeloClose-MagDown.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND tag to generate Open VELO
from Configurables import CondDB
## from DetCond.Configuration import addCondDBLayer
## simCondVelo = allConfigurables["SIMCOND"].clone("VELOCOND")
## simCondVelo.DefaultTAG = "velo-open"
## addCondDBLayer(simCondVelo)
CondDB().LocalTags["SIMCOND"] = ["velo-open"]

#--Tell to use 450 GeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, and set crossingle angle for collisions
#--for beam gas Hijing cannot take inot account crossing angle, so ignore! 
importOptions("$GAUSSOPTS/PilotRun.opts")
setCrossingAngle(2.1*SystemOfUnits.mrad)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 110000*SystemOfUnits.ns
