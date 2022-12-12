cet_make_library(LIBRARY_NAME WireReadout INTERFACE
  SOURCE WireReadout.h
  LIBRARIES INTERFACE
  larcorealg::Geometry
  art_plugin_types::serviceDeclaration
)

cet_write_plugin_builder(lar::WireReadout art::service Modules
  INSTALL_BUILDER
  LIBRARIES CONDITIONAL
  larcore::WireReadout
)

cet_make_library(LIBRARY_NAME ChannelMapSetupTool INTERFACE
  SOURCE ChannelMapSetupTool.h
  LIBRARIES CONDITIONAL
  larcorealg::Geometry
)


cet_write_plugin_builder(lar::ChannelMapSetupTool art::tool Modules
  INSTALL_BUILDER
  LIBRARIES CONDITIONAL
  larcorealg::ChannelMapSetupTool
)

cet_build_plugin(AuxDetGeoObjectSorterStandard art::tool
  LIBRARIES REG larcorealg::Geometry)

cet_build_plugin(GeoObjectSorterStandard art::tool
  LIBRARIES REG larcorealg::Geometry)

cet_build_plugin(WireReadoutSorterStandard art::tool
  LIBRARIES REG larcorealg::Geometry)

cet_build_plugin(Geometry art::service
  LIBRARIES
  PUBLIC
  larcorealg::Geometry
  larcoreobj::SummaryData
  PRIVATE
  larcore::ServiceUtil
  art::Framework_Principal
  messagefacility::MF_MessageLogger
  canvas::canvas
  fhiclcpp::fhiclcpp
  fhiclcpp::types
  cetlib::cetlib
)

cet_build_plugin(AuxDetGeometry art::service
  LIBRARIES
  PUBLIC
  larcorealg::Geometry
  PRIVATE
  cetlib::cetlib
  cetlib_except::cetlib_except
)

cet_build_plugin(GeometryConfigurationWriter art::ProducingService
  LIBRARIES PRIVATE
  larcore::Geometry_Geometry_service
  larcoreobj::SummaryData
  art::Framework_Services_Registry
  art::Framework_Principal
  messagefacility::MF_MessageLogger
  canvas::canvas
)

include(lar::WireReadout)

cet_build_plugin(StandardWireReadout lar::WireReadout
  LIBRARIES PRIVATE
  messagefacility::MF_MessageLogger
)

cet_build_plugin(DumpChannelMap art::EDAnalyzer
  LIBRARIES PRIVATE
  larcore::Geometry_Geometry_service
  larcorealg::Geometry
  larcoreobj::SimpleTypesAndConstants
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
)

cet_build_plugin(DumpGeometry art::EDAnalyzer
  LIBRARIES PRIVATE
  larcorealg::Geometry
  larcoreobj::SimpleTypesAndConstants
  art::Framework_Principal
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
)

install_headers()
install_fhicl()
install_source()

add_subdirectory(gdml)
