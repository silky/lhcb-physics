#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
import Gaudi.Configuration
from Moore.Configuration import Moore

# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'Default'
#Moore().ThresholdSettings = 'Commissioning_PassThrough'
#Moore().configLabel = 'ODINRandom acc=0, TELL1Error acc=1'

Moore().ThresholdSettings = 'Physics_September2012'

Moore().EvtMax = 10000
from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().ForceSingleL0Configuration = False

from PRConfig.TestFileDB import test_file_db
input = test_file_db['2012_raw_default']
input.run(configurable=Moore()) 
#input.filenames = [ '/data/bfys/graven/0x46/'+f.split('/')[-1] for f in input.filenames ]# Gerhard, no personal files in a release please!
Moore().inputFiles = input.filenames

# /data/bfys/graven/0x46
Moore().WriterRequires = [ 'Hlt1' ]  # default is HltDecisionSequence, which Split = 'Hlt1' will remove (maybe it should remove Hlt2 from HltDecisionSequence instead???)
#Moore().outputFile = '/data/bfys/graven/0x46/hlt1.raw'# Gerhard, no personal files in a release please!

Moore().Split = 'Hlt1'
