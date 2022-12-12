/**
 * @file   StandardWireReadout.h
 * @brief  Geometry helper service for detectors with strictly standard mapping
 * @author rs@fnal.gov
 *
 * Handles detector-specific information for the generic Geometry service within
 * LArSoft. Derived from the WireReadout class. This version provides strictly
 * standard functionality
 */

#ifndef GEO_StandardWireReadout_h
#define GEO_StandardWireReadout_h

// LArSoft libraries
#include "larcore/Geometry/WireReadout.h"
#include "larcorealg/Geometry/WireReadoutStandardGeom.h"

// framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

namespace geo {
  /**
   * @brief Simple implementation of channel mapping
   *
   * This WireReadout implementation serves a WireReadoutStandardGeom
   * for experiments that are known to work well with it.
   */
  class StandardWireReadout : public WireReadout {
  public:
    explicit StandardWireReadout(fhicl::ParameterSet const& pset);

  private:
    WireReadoutGeom const& wireReadoutGeom() const override;
    WireReadoutStandardGeom alg_;
  };

}

DECLARE_ART_SERVICE_INTERFACE_IMPL(geo::StandardWireReadout, geo::WireReadout, SHARED)

#endif // GEO_StandardWireReadout_h
