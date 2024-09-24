/**
 * @file   larcore/Geometry/Geometry.h
 * @brief  art framework interface to geometry description
 * @author brebel@fnal.gov
 * @see    larcore/Geometry/Geometry_service.cc
 */

#ifndef LARCORE_GEOMETRY_GEOMETRY_H
#define LARCORE_GEOMETRY_GEOMETRY_H

// LArSoft libraries
#include "larcorealg/Geometry/GeometryCore.h"
#include "larcoreobj/SummaryData/GeometryConfigurationInfo.h"

// framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "fhiclcpp/fwd.h"

namespace geo {

  /**
   * @brief The geometry of one entire detector, as served by art
   *
   * This class extends the interface of the geometry service provider, geo::GeometryCore,
   * to the one of an art service.
   *
   * Configuration
   * ==============
   *
   * In addition to the parameters documented in geo::GeometryCore, the following
   * parameters are supported:
   *
   * - *Builder* (a parameter set: default: empty): configuration for the geometry
   *   builder; if omitted, the standard builder (`geo::GeometryBuilderStandard`) with
   *   standard configuration will be used; if specified, currently the standard builder
   *   is nevertheless used; this interface can be "toolized", in which case this
   *   parameter set will select and configure the chosen tool.
   * - *SortingParameters* (a parameter set; default: empty): this configuration is used
   *   to create a GeoObjectSorter tool, which sorts the LArSoft geometry objects.
   */

  class Geometry : public GeometryCore {
  public:
    using provider_type = GeometryCore; ///< type of service provider

    explicit Geometry(fhicl::ParameterSet const& pset);

    /// Returns a pointer to the geometry service provider
    provider_type const* provider() const { return static_cast<provider_type const*>(this); }

    /// Returns the current geometry configuration information.
    sumdata::GeometryConfigurationInfo const& configurationInfo() const { return fConfInfo; }

  private:
    // --- BEGIN -- Configuration information checks ---------------------------
    /// @name Configuration information checks
    /// @{

    /// Fills the service configuration information into `fConfInfo`.
    void FillGeometryConfigurationInfo(fhicl::ParameterSet const& config);

    /// @}
    // --- END -- Configuration information checks -----------------------------

    sumdata::GeometryConfigurationInfo fConfInfo; ///< Summary of service configuration.
  };

} // namespace geo

DECLARE_ART_SERVICE(geo::Geometry, SHARED)

#endif // LARCORE_GEOMETRY_GEOMETRY_H
