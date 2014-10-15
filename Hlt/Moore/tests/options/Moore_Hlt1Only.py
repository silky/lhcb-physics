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


Moore().WriterRequires = [ 'Hlt1' ]
Moore().outputFile = 'hlt1_reqhlt1.raw'
Moore().EnableTimer = 'hlt1_timer.csv'


#use new splitting of Hlt2
Moore().Split='Hlt1'

# kill input DecReports
from Configurables import ApplicationMgr, GaudiSequencer, DecodeRawEvent, RawEventFormatConf, RawEventJuggler, RecombineRawEvent
ApplicationMgr().TopAlg+= [GaudiSequencer("JuggleSeq")]
RawEventJuggler().Input="Pit"
RawEventJuggler().Output="Moore"
RawEventJuggler().Sequencer = GaudiSequencer("JuggleSeq")
RawEventJuggler().KillInputBanksBefore = ('Hlt')
