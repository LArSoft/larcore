/**
 * @file   Geometry_service.cc
 * @brief  art framework interface to geometry description - implementation file
 * @author brebel@fnal.gov
 * @see    Geometry.h
 */

// class header
#include "larcore/Geometry/Geometry.h"

// LArSoft includes
#include "larcore/CoreUtils/ServiceUtil.h"
#include "larcorealg/CoreUtils/SearchPathPlusRelative.h"
#include "larcorealg/Geometry/GeoObjectSorter.h"
#include "larcorealg/Geometry/GeometryBuilderStandard.h"
#include "larcoreobj/SummaryData/GeometryConfigurationInfo.h"

// Framework includes
#include "art/Utilities/make_tool.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <string>
#include <utility> // std::move()

// check that the requirements for geo::Geometry are satisfied
template struct lar::details::ServiceRequirementsChecker<geo::Geometry>;

//......................................................................
geo::Geometry::Geometry(fhicl::ParameterSet const& pset)
  : GeometryCore{
      pset,
      std::make_unique<GeometryBuilderStandard>(pset.get<fhicl::ParameterSet>("Builder", {})),
      art::make_tool<GeoObjectSorter>(pset.get<fhicl::ParameterSet>("SortingParameters", {}))}
{
  FillGeometryConfigurationInfo(pset);
}

//......................................................................
void geo::Geometry::FillGeometryConfigurationInfo(fhicl::ParameterSet const& config)
{
  sumdata::GeometryConfigurationInfo confInfo;
  confInfo.dataVersion = sumdata::GeometryConfigurationInfo::DataVersion_t{2};

  // version 1+:
  confInfo.detectorName = DetectorName();

  // version 2+:
  confInfo.geometryServiceConfiguration = config.to_indented_string();
  fConfInfo = std::move(confInfo);

  MF_LOG_TRACE("Geometry") << "Geometry configuration information:\n" << fConfInfo;
}
