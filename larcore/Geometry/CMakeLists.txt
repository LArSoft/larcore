art_make(SERVICE_LIBRARIES larcorealg_Geometry
                           larcoreobj_SummaryData
                           art::Framework_Services_Registry
                           art::Framework_Principal
                           art_Framework_Core
                           art::Persistency_Provenance
                           messagefacility::MF_MessageLogger
                           ROOT::Core
         MODULE_LIBRARIES larcorealg_Geometry
                          larcoreobj_SimpleTypesAndConstants
                          art::Framework_Services_Registry
                          messagefacility::MF_MessageLogger
                          ROOT::Core
                          ROOT::Geom
                          ROOT::GenVector)

install_headers()
install_fhicl()
install_source()

add_subdirectory(gdml)
