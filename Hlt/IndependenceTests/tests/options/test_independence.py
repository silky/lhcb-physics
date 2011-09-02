#!/usr/bin/env python
#
# Script to use several processes to write events with differing Hlt Decisions to a file

# MultiProcessing
from multiprocessing import Process, Queue, Event, Condition, Lock
from time import sleep
from copy import copy
import os
import sys
import select
from collections import defaultdict

# General imports
import random, optparse, subprocess

# Gaudi configuration
from Gaudi.Configuration import *
from GaudiConf.Configuration import *

# Local imports
from IndependenceTests.Tasks import DecisionReporter, time_string
from HltMonitor.Base import ProcessWrapper

from GaudiPython.Bindings import gbl
dm = { 'all'    : gbl.DecisionMap(),
       'single' : gbl.DecisionMap() }

def index( seq, item ):
    """Return the index of the first item in seq."""
    for index in xrange( len( seq ) ):
        if seq[ index ] == item:
            return index

def remove( lines ):
    remove = []
    bad = set( [ 'Hlt1ErrorEvent', 'Hlt1ODINTechnical', 'Hlt1Tell1Error', 'Hlt1Incident',
                 'Hlt1Global', 'Hlt2Global', 'Hlt1Lumi', 'Hlt1VeloClosingMicroBias',
                 'Hlt1L0Any', 'Hlt1MBMicroBiasTStation',
                 'Hlt1MBNoBias', 'Hlt1LumiMidBeamCrossing', 'Hlt1CharmCalibrationNoBias' ] )
    for line in lines:
        if line.find( 'RateLimited' ) != -1: remove.append( line )
        elif line.find( 'PassThrough' ) != -1: remove.append( line )
        elif line.find( 'BeamGas' ) != -1: remove.append( line )
        elif line in bad: remove.append( line )
    for line in remove: lines.remove( line )
    return lines
    
def find_lines( options, args ):
    print 'Determining available lines'
    jobArgs = [ '-d', options.DataType,
                '-n', str( options.EvtMax ),
                '--dddbtag', options.DDDBtag,
                '--conddbtag', options.CondDBtag,
                '--settings', args[ 0 ] ]

    command = os.path.expandvars( '$INDEPENDENCETESTSROOT/scripts/lines.sh' )
    p = subprocess.Popen( [command] + jobArgs, stdout = subprocess.PIPE,
                          stderr = subprocess.STDOUT )
    o = p.communicate()[ 0 ]
    if ( p.returncode ):
        print "failed finding lines"
        return p.returncode

    # Get the list of hlt lines from the output
    output = []
    for line in o.split( "\n" ):
        if len( line.strip() ):
            output.append( line )

    hlt1Lines = set()
    for m in output[ index( output, "HLT1LINES" ) + 1 : index( output, "HLT2LINES" ) ]:
        hlt1Lines.add( m )

    hlt2Lines = set()
    for m in output[ index( output, "HLT2LINES" ) + 1 : index( output, "ENDLINES" ) ]:
        hlt2Lines.add( m )

    return remove( hlt1Lines ), remove( hlt2Lines )

def run( options, args ):

    evtMax = options.EvtMax

    EventSelector().Input = [
        "DATAFILE='PFN:castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/user/a/apuignav/81349_0x002a_MBNB_L0Phys.raw?svcClass=lhcbuser&castorVersion=2' SVC='LHCb::MDFSelector'",
        "DATAFILE='PFN:castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/user/a/apuignav/80881_0x002a_MBNB_L0Phys.raw?svcClass=lhcbuser&castorVersion=2' SVC='LHCb::MDFSelector'",
        "DATAFILE='PFN:castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/user/a/apuignav/79647_0x002a_MBNB_L0Phys.raw?svcClass=lhcbuser&castorVersion=2' SVC='LHCb::MDFSelector'",
        "DATAFILE='PFN:castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/user/a/apuignav/79646_0x002a_MBNB_L0Phys.raw?svcClass=lhcbuser&castorVersion=2' SVC='LHCb::MDFSelector'"
        ]

    # Put the options into a dictionary
    reporterConfig = dict()
    reporterConfig[ 'EvtMax' ] = options.EvtMax
    reporterConfig[ 'Verbose' ] = options.Verbose
    reporterConfig[ 'UseDBSnapshot' ] = options.UseDBSnapshot
    reporterConfig[ 'DDDBtag' ] = options.DDDBtag
    reporterConfig[ 'CondDBtag' ] = options.CondDBtag
    reporterConfig[ 'Simulation' ] = options.Simulation
    reporterConfig[ 'DataType' ] = options.DataType
    reporterConfig[ 'ThresholdSettings' ] = args[ 0 ]
    reporterConfig[ 'Input' ]  = EventSelector().Input
    reporterConfig[ 'Catalogs' ] = FileCatalog().Catalogs
    if options.L0:
        reporterConfig[ 'L0' ] = True
        reporterConfig[ 'ReplaceL0BanksWithEmulated' ] = True

    ## Don't wait after each event
    reporterConfig[ 'Wait' ] = False
    
    # Find all lines in this configuration
    all1Lines, all2Lines = find_lines( options, args )

    hlt1Lines = set()
    # Process the options to setup Hlt lines to be run
    if options.Hlt1Lines == 'none':
        pass
    elif options.Hlt1Lines == 'all':
        hlt1Lines = copy( all1Lines )
    else:
        # Put the comma separated lists of lines into lists
        for line in options.Hlt1Lines.split( ";" ):
            if ( len( line.strip() ) ):
                hlt1Lines.add( line )

    hlt2Lines = set()
    if options.Hlt2Lines == 'none':
        pass
    elif options.Hlt2Lines == 'all':
        hlt2Lines = copy( all2Lines )
    else:
        for line in options.Hlt2Lines.split( ";" ):
            if ( len( line.strip() ) ):
                hlt2Lines.add( line )

    if options.Verbose:
        for line in hlt1Lines:
            print line
        for line in hlt2Lines:
            print line

    # Check if the specified lines exist
    allLines = all1Lines.union( all2Lines )
    for line in hlt1Lines.union( hlt2Lines ):
        if line not in allLines:
            print "Error, " + line + " is not a valid Hlt 1 or 2 line."
            print "Available lines:"
            for l in allLines:
                print l
            return 1
    del allLines

    wrappers = dict()
    i = 0
    # Setup the process with all lines
    # Setup the processes running a all Hlt lines
    allConfig = reporterConfig.copy()
    allConfig[ 'Hlt1Lines' ] = list( hlt1Lines )
    allConfig[ 'Hlt2Lines' ] = list( hlt2Lines )
    wrappers[ 'allLines' ] = ProcessWrapper( i, DecisionReporter, 'allLines', allConfig,
                                             options.Verbose )

    # Setup the processes running a single Hlt1 line
    for lineName in hlt1Lines:
        i += 1
        config = reporterConfig.copy()
        config[ 'Hlt1Lines' ] = [ lineName ]
        config[ 'Hlt2Lines' ] = []
        wrappers[ lineName ] = ProcessWrapper( i, DecisionReporter, lineName, config,
                                               options.Verbose )

    # Setup the processes running a single Hlt2 line
    for lineName in hlt2Lines:
        i += 1
        config = reporterConfig.copy()
        config[ 'Hlt1Lines' ] = list( hlt1Lines )
        config[ 'Hlt2Lines' ] = [ lineName ]
        wrappers[ lineName ] = ProcessWrapper( i, DecisionReporter, lineName, config,
                                               options.Verbose )

    # Keep track of jobs that have completed
    completed = set()
    running = set()
    todo = set()

    # start the allLines process first
    wrappers[ 'allLines' ].start()
    print 'Running allLines'
    running.add( 'allLines' )

    # start the rest of the processes
    for name, wrapper in wrappers.iteritems():
        if len( running ) == options.NProcesses:
            break
        wrapper.start()
        running.add( name )
    todo = set( wrappers.keys() ).difference( running )

    while True:
        ready = None
        try:
            ( ready, [], [] ) = select.select( [ wrappers[ name ] for name in running ], [], [] )

            for wrapper in ready:
                data = wrapper.getData( False )
                if data == None:
                    continue
                elif type( data ) == type( "" ):
                    # Message
                    if data == 'DONE':
                        print '%s done' % wrapper.name()
                        completed.add( wrapper.name() )
                        wrapper.join()
                        if len( todo ) != 0:
                            to_start = todo.pop()
                            print 'Running %s' % to_start
                            wrappers[ to_start ].start()
                            running.add( to_start )
                        running.difference_update( completed )
                else:
                    run = data.pop( 'run' )
                    event = data.pop( 'event' )
                    name = wrapper.name()
                    if name == 'allLines':
                        dm_all = dm[ 'all' ]
                        for line, dec in data.iteritems():
                            dm_all.addDecision( run, event, line, dec )
                    else:
                        name = name + 'Decision'
                        dm_single = dm[ 'single' ]
                        dm_single.addDecision( run, event, name, data[ name ] )

            if len( completed ) == len( wrappers ):
                break

        except select.error:
            sleep( 5 )

    mismatches = defaultdict(set)
    dm_all = dm[ 'all' ]
    dm_single = dm[ 'single' ]
    events = dm_all.events()
    for entry in events:
        for line in hlt1Lines.union( hlt2Lines ):
            all_dec = dm_all.decision( entry.first, entry.second, line )
            single_dec = dm_single.decision( entry.first, entry.second, line )
            if all_dec != single_dec:
                mismatches[ line ].add( entry )

    if len( mismatches ):
        print 'Found mismatches:'
        for line, events in mismatches.iteritems():
            print '%s %d' % ( line, len( events ) )
    else:
        print 'No mismatches found.'

    return 0
    
if __name__ == "__main__":

    # Setup the option parser
    usage = "usage: %prog settings"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2011", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", action = "store", type = 'int',
                       dest = "EvtMax", default = 1e4, help = "Number of events to run" )
    parser.add_option( "--nprocesses", action = "store", type = 'int',
                       dest = "NProcesses", default = 8,
                       help = "Number of simultaneous processes to run." )
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='MC09-20090602', help="DDDBTag to use" )
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'sim-20090402-vc-md100', help = "CondDBtag to use" )
    parser.add_option( "-s", "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")
    parser.add_option( "--dbsnapshot", action = "store_true", dest = "UseDBSnapshot",
                       default = False, help = "Use a DB snapshot" )
    parser.add_option( "-v", "--verbose", action = "store_true", dest = "Verbose",
                       default = False, help = "Verbose output" )
    parser.add_option( "--hlt1lines", action = "store", dest = "Hlt1Lines",
                       default = "", help = "Colon seperated list of additional hlt1 lines" )
    parser.add_option( "--hlt2lines", action = "store", dest = "Hlt2Lines",
                       default = "", help = "Colon seperated list of additional hlt2 lines" )
    parser.add_option( "--l0", action="store_true", dest="L0",
                       default=False, help="Rerun L0" )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    ## multiprocessing.log_to_stderr()
    ## logger = multiprocessing.get_logger()
    ## logger.setLevel(logging.INFO)

    if not len( args ):
        print "No settings specified"
        exit( 1 )

    sys.exit( run( options, args ) )
