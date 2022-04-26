/**
 * @file   StandardGeometryHelper.h
 * @brief  Geometry helper service for detectors with strictly standard mapping
 * @author rs@fnal.gov
 *
 * Handles detector-specific information for the generic Geometry service
 * within LArSoft. Derived from the ExptGeoHelperInterface class. This version
 * provides strictly standard functionality
 */

#ifndef GEO_StandardGeometryHelper_h
#define GEO_StandardGeometryHelper_h

// LArSoft libraries
#include "larcore/Geometry/ExptGeoHelperInterface.h"

// framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

namespace geo
{
  /**
   * @brief Simple implementation of channel mapping
   *
   * This ExptGeoHelperInterface implementation serves a ChannelMapStandardAlg
   * for experiments that are known to work well with it.
   */
  class StandardGeometryHelper : public ExptGeoHelperInterface {
  public:
    explicit StandardGeometryHelper(fhicl::ParameterSet const& pset);

  private:
    ChannelMapAlgPtr_t
    doConfigureChannelMapAlg(fhicl::ParameterSet const& sortingParameters,
                             std::string const& detectorName) const override;
  };

}

DECLARE_ART_SERVICE_INTERFACE_IMPL(geo::StandardGeometryHelper,
                                   geo::ExptGeoHelperInterface,
                                   SHARED)

#endif // GEO_StandardGeometryHelper_h
