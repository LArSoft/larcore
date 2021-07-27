/**
 * @file   AuxDetGeometry.h
 * @brief  art framework interface to geometry description for auxiliary detectors
 * @author brebel@fnal.gov
 * @see    AuxDetGeometry_service.cc
 *
 */

#ifndef GEO_AUXDETGEOMETRY_H
#define GEO_AUXDETGEOMETRY_H

// LArSoft libraries

// the following are included for convenience only
#include "larcorealg/Geometry/AuxDetGeometryCore.h"
#include "larcorealg/Geometry/AuxDetChannelMapAlg.h"
#include "larcorealg/Geometry/AuxDetGeo.h"

// framework libraries
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h" // for the convenience of includers

// C/C++ standard libraries
#include <vector>
#include <map>
#include <set>
#include <cstring>
#include <memory>
#include <iterator> // std::forward_iterator_tag


namespace geo {

  /**
   * @brief The geometry of one entire detector, as served by art
   *
   * This class extends the interface of the geometry service provider,
   * GeometryCore, to the one of an art service.
   * It handles the correct initialization of the provider using information
   *
   * It relies on geo::ExptGeoHelperInterface service to obtain the
   * channel mapping algorithm proper for the selected geometry.
   *
   * The geometry initialization happens immediately on construction.
   * Optionally, the geometry is automatically reinitialized on each run based
   * on the information contained in the art::Run object.
   *
   * Configuration
   * ==============
   *
   * In addition to the parameters documented in geo::GeometryCore, the
   * following parameters are supported:
   *
   * - *RelativePath* (string, default: no path): this path is prepended to the
   *   geometry file names before searching from them; the path string does not
   *   affect the file name
   * - *GDML* (string, mandatory): path of the GDML file to be served to Geant4
   *   for detector simulation. The full file is composed out of the optional
   *   relative path specified by `RelativePath` path and the base name
   *   specified in `GDML` parameter; this path is searched for in the
   *   directories configured in the `FW_SEARCH_PATH` environment variable;
   * - *ROOT* (string, mandatory): currently overridden by `GDML` parameter,
   *   whose value is used instead;
   *   this path is assembled in the same way as the one for `GDML` parameter,
   *   except that no alternative (wireless) geometry is used even if
   *   `DisableWiresInG4` is specified (see below); this file is used to load
   *   the geometry used in the internal simulation and reconstruction,
   *   basically everywhere except for the Geant4 simulation
   * - *DisableWiresInG4* (boolean, default: false): if true, Geant4 is loaded
   *   with an alternative geometry from a file with the standard name as
   *   configured with the /GDML/ parameter, but with an additional "_nowires"
   *   appended before the ".gdml" suffix
   * - *ForceUseFCLOnly* (boolean, default: false): information on the current
   *   geometry is stored in each run by the event generator producers; if this
   *   information does not describe the current geometry, a new geometry is
   *   loaded according to the information in the run. If `ForceUseFCLOnly`
   *   is set to `true`, this mechanism is disabled and the geometry is just
   *   loaded at the beginning of the job from the information in the job
   *   configuration, once and for all.
   * - *SortingParameters* (a parameter set; default: empty): this configuration
   *   is directly passed to the channel mapping algorithm (see
   *   geo::ChannelMapAlg); its content is dependent on the chosen
   *   implementation of ChannelMapAlg
   *
   * @note Currently, the file defined by `GDML` parameter is also served to
   * ROOT for the internal geometry representation.
   *
   */
  class AuxDetGeometry
  {
  public:

    AuxDetGeometry(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

    /// Returns a constant reference to the service provider
    AuxDetGeometryCore const& GetProvider() const { return fProvider; }

    /// Returns a constant pointer to the service provider
    AuxDetGeometryCore const* GetProviderPtr() const { return &GetProvider(); }

  private:

    /// Updates the geometry if needed at the beginning of each new run
    void preBeginRun(art::Run const& run);

    /// Expands the provided paths and loads the geometry description(s)
    void LoadNewGeometry(std::string gdmlfile, std::string rootfile);

    void InitializeChannelMap();

    /// Returns a reference to the service provider
    AuxDetGeometryCore& GetProvider() { return fProvider; }

    /// Returns a pointer to the service provider
    AuxDetGeometryCore* GetProviderPtr() { return &GetProvider(); }


    AuxDetGeometryCore fProvider; ///< the actual service provider

    std::string               fRelPath;          ///< Relative path added to FW_SEARCH_PATH to search for
                                                 ///< geometry file
    bool                      fForceUseFCLOnly;  ///< Force Geometry to only use the geometry
                                                 ///< files specified in the fcl file
    fhicl::ParameterSet       fSortingParameters;///< Parameter set to define the channel map sorting
  };

} // namespace geo

DECLARE_ART_SERVICE(geo::AuxDetGeometry, SHARED)

#endif // GEO_AUXDETGEOMETRY_H
