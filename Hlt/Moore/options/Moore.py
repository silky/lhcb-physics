#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
from Gaudi.Configuration import *
from Moore.Configuration import *


# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'NO prescale'

Moore().hltType = 'Hlt1' # 'PA+L0+HA' # +Hlt2'
HltConf().Hlt2IgnoreHlt1Decision = False
Moore().oldStyle = False
Moore().verbose = True


#dataType = 'DC06'
#files = [ '/data/bfys/lhcb/MinBias-L0strip/MBL0-lumi2-' + str(f) +'.dst'  for f in range(1,5) ]

#dataType = '2008'
#files= [ '/data/bfys/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw' ]

#dataType = 'DC06'
#files = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

dataType = 'DC06'
files= [ '/data/bfys/lhcb/MinBias-L0strip/DC06_L0_v1_lumi2_MuonHadron_40000ev_' + str(f) +'.mdf'  for f in range(1,3) ]
#files = [ 'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_'+str(f)+'1.mdf' for f in range(1,3) ]


EventSelector().PrintFreq = 100

Moore().EvtMax = 1000
Moore().DataType   = dataType
Moore().inputFiles = files
# Moore().outputFile = '/tmp/foo.raw'

# optionally, we can enable some auditors...
# Moore().enableAuditor = [ NameAuditor() ]

#and tell the world how we are configured
print Moore()
