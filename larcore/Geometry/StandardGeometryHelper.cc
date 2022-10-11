////////////////////////////////////////////////////////////////////////////////
/// \file StandardGeometryHelper_service.cc
///
/// \author  rs@fnal.gov
////////////////////////////////////////////////////////////////////////////////

// class header
#include "larcore/Geometry/StandardGeometryHelper.h"

// LArSoft libraries
#include "larcorealg/Geometry/ChannelMapStandardAlg.h"

// framework libraries
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>

namespace geo {

  //----------------------------------------------------------------------------
  StandardGeometryHelper::StandardGeometryHelper(fhicl::ParameterSet const&) {}

  //----------------------------------------------------------------------------
  StandardGeometryHelper::ChannelMapAlgPtr_t StandardGeometryHelper::doConfigureChannelMapAlg(
    fhicl::ParameterSet const& sortingParameters,
    std::string const& /*detectorName*/) const
  {
    mf::LogInfo("StandardGeometryHelper") << "Loading channel mapping: ChannelMapStandardAlg";
    return std::make_unique<geo::ChannelMapStandardAlg>(sortingParameters);
  }

} // namespace geo
