#!/usr/bin/env gaudirun.py
from Gaudi.Configuration import *
from Moore.Configuration import *

Moore().HltType = 'Hlt1' # +Hlt2'
Moore().EvtMax = 2000
Moore().Simulation = True
Moore().DataType = 'DC06'
Moore().inputFiles = [ 'castor:/castor/cern.ch/grid/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]

LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"

EventSelector().PrintFreq = 100
