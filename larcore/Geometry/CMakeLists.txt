cet_make_library(LIBRARY_NAME ExptGeoHelperService INTERFACE
  SOURCE ExptGeoHelperInterface.h
  LIBRARIES INTERFACE
  larcorealg::Geometry
  art_plugin_types::serviceDeclaration
)

cet_write_plugin_builder(lar::ExptGeoHelperService art::service Modules
  INSTALL_BUILDER
  LIBRARIES CONDITIONAL
  larcore::ExptGeoHelperService
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

cet_build_plugin(Geometry art::service
  LIBRARIES
  PUBLIC
  larcorealg::Geometry
  larcoreobj::SummaryData
  fhiclcpp::fhiclcpp
  PRIVATE
  larcore::ServiceUtil
  art::Framework_Principal
  messagefacility::MF_MessageLogger
  canvas::canvas
  fhiclcpp::types
  cetlib::cetlib
)

cet_build_plugin(AuxDetGeometry art::service
  LIBRARIES
  PUBLIC
  larcorealg::Geometry
  PRIVATE
  larcoreobj::SummaryData
  art::Framework_Principal
  messagefacility::MF_MessageLogger
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

include(lar::ExptGeoHelperService)

cet_build_plugin(StandardGeometryHelper lar::ExptGeoHelperService
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
