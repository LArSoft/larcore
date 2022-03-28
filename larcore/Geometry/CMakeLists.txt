art_make(SERVICE_LIBRARIES larcorealg::Geometry
                           larcoreobj::SummaryData
                           art::Framework_Services_Registry
                           art::Framework_Principal
                           art_Framework_Core
                           art::Persistency_Provenance
                           messagefacility::MF_MessageLogger
                           ROOT::Core
         MODULE_LIBRARIES larcorealg::Geometry
                          larcoreobj::SimpleTypesAndConstants
                          art::Framework_Services_Registry
                          messagefacility::MF_MessageLogger
                          ROOT::Core
                          ROOT::Geom
                          ROOT::GenVector) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.06.01) - deprecated: use art_make_library(), art_dictonary(), and cet_build_plugin() with explicit source lists and plugin base types

install_headers()
install_fhicl()
install_source()

add_subdirectory(gdml)
