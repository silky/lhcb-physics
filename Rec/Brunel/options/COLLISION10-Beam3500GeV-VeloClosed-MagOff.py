# Example 2010 collisions options for Brunel

# Syntax is:
#   gaudirun.py COLLISION10-Beam3500GeV-VeloClosed-MagOff.py
#
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")
importOptions("$APPCONFIGOPTS/Brunel/moff.py")
#importOptions("$APPCONFIGOPTS/UseOracle.py")

from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2010 database tags for real data
Brunel().DataType = "2010"

EventSelector().Input = [
  "DATAFILE='castor:/castor/cern.ch/grid/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/69947/069947_0000000004.raw' SVC='LHCb::MDFSelector'"
    ]
