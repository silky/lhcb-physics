################################################################################
# Package: GENSER
################################################################################
gaudi_subdir(GENSER v15r0)

find_package(LHAPDF)
find_package(Pythia6)

message(WARNING "FIXME: the Pythia6 headers in GENSER must be installed")

string(REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
gaudi_add_library(pythia6forgauss
                  src/pythia6/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES Pythia6 LHAPDF)
set_property(TARGET pythia6forgauss
             APPEND PROPERTY LINK_FLAGS ${PYTHIA6_LIBRARY_DIRS}/pydata.o)

gaudi_install_cmake_modules()
#gaudi_env(SET PYTHIA8DATA ${PYTHIA8_XML})

