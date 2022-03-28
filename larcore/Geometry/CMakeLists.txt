
cet_build_plugin(AuxDetGeometry art::service
                 LIBRARIES PUBLIC
                           larcorealg::Geometry
                           larcoreobj::SummaryData
                           art::Framework_Services_Registry
                           art::Framework_Core
                           art::Persistency_Provenance
                         PRIVATE
                           art::Framework_Principal
                           messagefacility::MF_MessageLogger
                           ROOT::Core)
cet_build_plugin(DumpChannelMap  art::module
                 LIBRARIES REG
                          larcorealg::Geometry
                          larcoreobj::SimpleTypesAndConstants
                          art::Framework_Services_Registry
                          messagefacility::MF_MessageLogger
                          ROOT::Core
                          ROOT::Geom
                          ROOT::GenVector)
cet_build_plugin(DumpGeometry  art::module
                 LIBRARIES REG
                          larcorealg::Geometry
                          larcoreobj::SimpleTypesAndConstants
                          art::Framework_Services_Registry
                          messagefacility::MF_MessageLogger
                          ROOT::Core
                          ROOT::Geom
                          ROOT::GenVector)
cet_build_plugin(GeometryConfigurationWriter art::service
                 LIBRARIES REG
                           larcorealg::Geometry
                           larcoreobj::SummaryData
                           art::Framework_Services_Registry
                           art::Framework_Principal
                           art::Framework_Core
                           art::Persistency_Provenance
                           messagefacility::MF_MessageLogger
                           ROOT::Core)
cet_build_plugin(Geometry art::service
                 LIBRARIES PUBLIC
                           larcorealg::Geometry
                           larcoreobj::SummaryData
                           art::Framework_Services_Registry
                           art::Framework_Core
                           art::Persistency_Provenance
                         PRIVATE
                           art::Framework_Principal
                           messagefacility::MF_MessageLogger
                           ROOT::Core)
cet_build_plugin(StandardGeometryHelper art::service
                 LIBRARIES
                           larcorealg::Geometry
                           larcoreobj::SummaryData
                           art::Framework_Services_Registry
                           art::Framework_Principal
                           art::Framework_Core
                           art::Persistency_Provenance
                           messagefacility::MF_MessageLogger
                           ROOT::Core)

install_headers()
install_fhicl()
install_source()

add_subdirectory(gdml)
