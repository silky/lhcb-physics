##############################################################################
# File for running Gauss with latest tags, using the default configuration
#
# Syntax is:
#   gaudirun.py Gauss-DEV.py <someInputJobConfiguration>.py
##############################################################################

from Configurables import Gauss
theApp = Gauss()

#--Use latest database tags 
from Configurables import DDDBConf, CondDB, LHCbApp

LHCbApp().DDDBtag   = "head-20130312-1"   # Must be set for selecting VELO
                                          # geometry list in Gauss()

DDDBConf(DataType = "2012")
CondDB().UseLatestTags = ["2012"]
