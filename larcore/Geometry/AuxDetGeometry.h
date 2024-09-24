/**
 * @file   AuxDetGeometry.h
 * @brief  art framework interface to geometry description for auxiliary detectors
 * @author brebel@fnal.gov
 * @see    AuxDetGeometry_service.cc
 *
 */

#ifndef GEO_AUXDETGEOMETRY_H
#define GEO_AUXDETGEOMETRY_H

// larsoft libraries
#include "larcorealg/Geometry/AuxDetGeometryCore.h"

// framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "fhiclcpp/fwd.h"

namespace geo {

  /**
   * @brief The geometry of one entire detector, as served by art
   *
   * This class extends the interface of the AuxDet geometry service provider,
   * geo::AuxDetGeometryCore, to the one of an art service.
   *
   * Configuration
   * ==============
   *
   * In addition to the parameters documented in geo::AuxDetGeometryCore, the following
   * parameters are supported:
   *
   * - *SortingParameters* (a parameter set; default: empty): this configuration is used
   *   to create a AuxDetGeoObjectSorter tool, which sorts the LArSoft auxiliary detector
   *   geometry objects.
   * - *ReadoutInitializer* (a parameter set; default: empty): this configuration is used
   *   for creating a geo::AuxDetInitializer tool.  If no configuration is provided, then
   *   no initializer is constructed and used during intiialization of the
   *   AuxDetReadoutGeom object.
   */
  class AuxDetGeometry {
  public:
    explicit AuxDetGeometry(fhicl::ParameterSet const& pset);

    AuxDetGeometryCore const& GetProvider() const { return fAuxDetGeom; }
    AuxDetGeometryCore const* GetProviderPtr() const { return &GetProvider(); }

  private:
    AuxDetGeometryCore fAuxDetGeom; ///< the actual service provider
  };

} // namespace geo

DECLARE_ART_SERVICE(geo::AuxDetGeometry, SHARED)

#endif // GEO_AUXDETGEOMETRY_H
