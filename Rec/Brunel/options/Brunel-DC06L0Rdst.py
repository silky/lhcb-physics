##############################################################################
# File for producing .rdst in DC06 workflows, reconstructing only L0 yes events
# Syntax is:
#   gaudirun.py Brunel-DC06Rdst.py <someDataFiles>.py
##############################################################################

from Configurables import Brunel

Brunel().DataType   = "DC06"
Brunel().InputType  = "DIGI"
Brunel().OutputType = "RDST"
Brunel().RecL0Only  = True

from GaudiKernel.Constants import *
Brunel().OutputLevel = WARNING

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
