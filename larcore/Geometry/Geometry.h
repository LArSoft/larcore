/**
 * @file   larcore/Geometry/Geometry.h
 * @brief  art framework interface to geometry description
 * @author brebel@fnal.gov
 * @see    larcore/Geometry/Geometry_service.cc
 *
 * Revised <seligman@nevis.columbia.edu> 29-Jan-2009
 *         Revise the class to make it into more of a general detector interface
 * Revised <petrillo@fnal.gov> 27-Apr-2015
 *         Factorization into a framework-independent GeometryCore.h and a
 *         art framework interface
 * Revised <petrillo@fnal.gov> 10-Nov-2015
 *         Complying with the provider requirements described in ServiceUtil.h
 */

#ifndef LARCORE_GEOMETRY_GEOMETRY_H
#define LARCORE_GEOMETRY_GEOMETRY_H

// LArSoft libraries
#include "larcore/CoreUtils/ServiceUtil.h" // not used; for user's convenience
#include "larcorealg/Geometry/GeometryCore.h"
#include "larcoreobj/SummaryData/GeometryConfigurationInfo.h"

// the following are included for convenience only
#include "larcorealg/Geometry/ChannelMapAlg.h"
#include "larcorealg/Geometry/CryostatGeo.h"
#include "larcorealg/Geometry/TPCGeo.h"
#include "larcorealg/Geometry/PlaneGeo.h"
#include "larcorealg/Geometry/WireGeo.h"
#include "larcorealg/Geometry/OpDetGeo.h"
#include "larcorealg/Geometry/AuxDetGeo.h"

// framework libraries
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
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
   * - *SkipConfigurationCheck* (boolean, default: `false`): if set to `true`,
   *   failure of configuration consistency check described below is not fatal
   *   and it will just produce a warning on each failure;
   * - *SortingParameters* (a parameter set; default: empty): this configuration
   *   is directly passed to the channel mapping algorithm (see
   *   geo::ChannelMapAlg); its content is dependent on the chosen
   *   implementation of `geo::ChannelMapAlg`
   * - *Builder* (a parameter set: default: empty): configuration for the
   *   geometry builder; if omitted, the standard builder
   *   (`geo::GeometryBuilderStandard`) with standard configuration will be
   *   used; if specified, currently the standard builder is nevertheless used;
   *   this interface can be "toolized", in which case this parameter set will
   *   select and configure the chosen tool.
   *
   * @note Currently, the file defined by `GDML` parameter is also served to
   * ROOT for the internal geometry representation.
   *
   *
   *
   * Configuration consistency check
   * ================================
   * 
   * The `Geometry` service checks that the input files were processed with a
   * configuration of `Geometry` service compatible with the current one.
   * 
   * Two checks may be performed: the standard check, and a legacy check.
   * 
   * 
   * Consistency check
   * ------------------
   * 
   * The `Geometry` service checks at the beginning of each run that the
   * current configuration is compatible with the geometry configuration
   * declared in the input file.
   * The `Geometry` service requires that an additional service,
   * `GeometryConfigurationWriter`, is run: this service is charged with writing
   * the configuration information into the output files, for the checks in
   * the future job.
   * 
   * The compatibility check is currently very silly, but it can improved in
   * future versions. This check is the same as the legacy check, that verifies
   * that the configured detector name (`geo::GeometryCore::DetectorName()`)
   * has not changed.
   * 
   * To allow this check to operate correctly, the only requirement is that
   * the service `GeometryConfigurationWriter` be included in the job:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * services.GeometryConfigurationWriter: {}
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * This must happen on the first job in the processing chain that configures
   * `Geometry` service. It is irrelevant, but not harmful, in the jobs that
   * follow.
   * 
   * 
   * ### Design details
   * 
   * This section describes the full design of the check from a technical point
   * of view. Users do not need to understand the mechanisms of this check in
   * order to configure their jobs to successfully pass it.
   * 
   * The check happens based on the data contained in the
   * `sumdata::GeometryConfigurationInfo` data product.
   * Starting from after the construction is complete, `geo::Geometry` is able
   * to provide at any time an instance of `sumdata::GeometryConfigurationInfo`
   * describing the geometry configuration for this job, whether the geometry
   * is already configured or not.
   * 
   * The `Geometry` service loads the geometry at the beginning of the job.
   * At the start of each run from the input file, the `Geometry` service
   * reads a configuration information `sumdata::GeometryConfigurationInfo` from
   * the `art::Run` record and verifies that it is compatible with the current
   * configuration. It is a fatal error for this information not to be available
   * in `art::Run`, and it is a fatal check failure if the available
   * information is not compatible with the current configuration.
   * 
   * The `sumdata::GeometryConfigurationInfo` information is put into `art::Run`
   * record by the `geo::GeometryConfigurationWriter` producing service.
   * This service verifies whether there is already such information in the run.
   * If no information is available yet in `art::Run`, the service obtains the
   * current configuration information from the `Geometry` service, and then
   * puts it into the `art::Run` record. The _art_ framework guarantees that
   * this happens _before_ the `Geometry` service itself is notified by _art_
   * of the start of the new run.
   * If some `geo::GeometryConfigurationWriter` information is already in the
   * run record, `geo::GeometryConfigurationWriter` performs no action.
   * As legacy check, if there is no information in the
   * `sumdata::GeometryConfigurationInfo` form but there is a `sumdata::RunData`
   * data product, the latter is used as a base for the check.
   * 
   * 
   * Design notes:
   * 
   * * the choice of delegating the writing of data product to a producing
   *   service rather than to modules is driven by the fact that there is a way
   *   to enforce this service to be actually run, and that no further
   *   instrumentation is needed;
   * * the choice of putting the configuration information in the `art::Run` is
   *   driven by the fact that the run is the highest available container;
   *   job-level data products (`art::Results`) behave very differently from
   *   the others and are not currently interfaced with a producing service;
   * * the information in `sumdata::GeometryConfigurationInfo` should be compact
   *   enough not to bloat the data files with very few events per run, as it
   *   may be for the selection of rare processes or signatures.
   * 
   *
   */
  class Geometry: public GeometryCore
  {
  public:

    using provider_type = GeometryCore; ///< type of service provider

    Geometry(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

    /// Returns a pointer to the geometry service provider
    provider_type const* provider() const
      { return static_cast<provider_type const*>(this); }

    /// Returns the current geometry configuration information.
    sumdata::GeometryConfigurationInfo const& configurationInfo() const
      { return fConfInfo; }
    
  private:

    /// Updates the geometry if needed at the beginning of each new run
    void preBeginRun(art::Run const& run);

    /// Expands the provided paths and loads the geometry description(s)
    void LoadNewGeometry(
      std::string gdmlfile, std::string rootfile,
      bool bForceReload = false
      );

    // --- BEGIN -- Configuration information checks ---------------------------
    /// @name Configuration information checks
    /// @{
    
    /// Fills the service configuration information into `fConfInfo`.
    void FillGeomeryConfigurationInfo(fhicl::ParameterSet const& config);
    
    /// Returns if the `other` configuration is compatible with our current.
    bool CheckConfigurationInfo
      (sumdata::GeometryConfigurationInfo const& other) const;
    
    /// Reads and returns the geometry configuration information from the run.
    static sumdata::GeometryConfigurationInfo const& ReadConfigurationInfo
      (art::Run const& run);
    
    /// Returns if `A` and `B` are compatible geometry service configurations.
    static bool CompareConfigurationInfo(
      sumdata::GeometryConfigurationInfo const& A,
      sumdata::GeometryConfigurationInfo const& B
      );
    
    /// @}
    // --- END -- Configuration information checks -----------------------------
    
    void InitializeChannelMap();

    std::string               fRelPath;          ///< Relative path added to FW_SEARCH_PATH to search for
                                                 ///< geometry file
    bool                      fDisableWiresInG4; ///< If set true, supply G4 with GDMLfileNoWires
                                                 ///< rather than GDMLfile
    bool                      fNonFatalConfCheck;///< Don't stop if configuration check fails.
                                                 ///< files specified in the fcl file
    fhicl::ParameterSet       fSortingParameters;///< Parameter set to define the channel map sorting
    fhicl::ParameterSet       fBuilderParameters;///< Parameter set for geometry builder.
    
    sumdata::GeometryConfigurationInfo fConfInfo;///< Summary of service configuration.
    
  };

} // namespace geo

DECLARE_ART_SERVICE(geo::Geometry, SHARED)

#endif // LARCORE_GEOMETRY_GEOMETRY_H
