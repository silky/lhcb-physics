################################################################################
# Package: Moore
################################################################################
gaudi_subdir(Moore v23r5)

gaudi_depends_on_subdirs(Det/DDDB
                         GaudiConf
                         GaudiKernel
                         GaudiPython
                         GaudiSvc
                         HltSys
                         L0/L0Interfaces
                         LbcomSys
                         PhysSys
                         RecSys)

find_package(HepMC)

gaudi_install_python_modules()

gaudi_alias(Moore
            gaudirun.py)


gaudi_add_test(QMTest QMTEST)
