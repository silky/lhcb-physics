################################################################################
# Package: LbPGuns
################################################################################
gaudi_subdir(LbPGuns v6r0p1)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/GenEvent
                         GaudiAlg
                         Gen/Generators)

gaudi_add_module(LbPGuns
                 src/*.cpp
                 LINK_LIBRARIES DetDescLib GenEvent GaudiAlgLib GeneratorsLib)

gaudi_install_headers(LbPGuns)

