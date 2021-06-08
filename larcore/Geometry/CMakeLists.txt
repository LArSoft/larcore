art_make(SERVICE_LIBRARIES larcorealg_Geometry
                           larcoreobj_SummaryData
                           art_Framework_Services_Registry
                           art_Framework_Principal
                           art_Framework_Core
                           art_Persistency_Provenance
                           messagefacility::MF_Messagelogger
                           ROOT::Core
         MODULE_LIBRARIES larcorealg_Geometry
                          larcoreobj_SimpleTypesAndConstants
                          art_Framework_Services_Registry
                          art_Framework_Principal
                          messagefacility::MF_Messagelogger
                          ROOT::Core
                          ROOT::Geom
                          ROOT::GenVector)

install_headers()
install_fhicl()
install_source()

add_subdirectory(gdml)
