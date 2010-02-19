##############################################################################
# File for running a Boole initialisation test with 2009 geometry
##############################################################################

from Boole.Configuration import *
from Configurables import CondDBAccessSvc, LHCbApp

Boole().EvtMax   = 0
Boole().Outputs  = ["DIGI","MDF","L0ETC"] # Test all output types
Boole().Histograms = "NONE"

LHCbApp().CondDBtag = "sim-20100119-vc-md100"
LHCbApp().DDDBtag   = "head-20100119"
