from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")

from Gaudi.Configuration import EventSelector
from Configurables import Brunel

Brunel().DataType = "2010"

# file of selected large events
EventSelector().Input = [
  "DATAFILE='castor:/castor/cern.ch/user/c/cattanem/testFiles/run_69669_large_2ev.mdf' SVC='LHCb::MDFSelector'"
    ]
