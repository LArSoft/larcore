/**
 * @file   larcore/Geometry/GeometryConfigurationWriter_service.cc
 * @brief  Service writing geometry configuration information into _art_ runs.
 * @date   October 7, 2020
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 */

// LArSoft libraries
#include "larcore/Geometry/Geometry.h"
#include "larcoreobj/SummaryData/GeometryConfigurationInfo.h"
#include "larcoreobj/SummaryData/RunData.h"

// framework libraries
#include "art/Framework/Core/ProducingService.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Utilities/InputTag.h"
#include "cetlib/HorizontalRule.h"
#include "fhiclcpp/types/Atom.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <memory>  // std::make_unique()
#include <utility> // std::move()

// -----------------------------------------------------------------------------
namespace geo {
  class GeometryConfigurationWriter;
}
/**
 * @brief Writes geometry configuration information into _art_ runs.
 *
 * If included in the configuration, the version of the geometry is checked after each run
 * is read from the input source.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * services: {
 *
 *   GeometryConfigurationWriter: {}
 *   Geometry: @local::experiment_geometry
 *
 *   # ...
 *
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The compatibility check is currently very silly, but it can improved in future
 * versions. This check is the same as the legacy check, that verifies that the configured
 * detector name (`geo::GeometryCore::DetectorName()`) has not changed.
 *
 * Configuration
 * -------------
 *
 * - *SkipConfigurationCheck* (boolean, default: `false`): if set to `true`, failure of
 *   configuration consistency check described below is not fatal and it will just
 *   produce a warning on each failure;
 *
 * The configuration check is described in the documentation of `geo::Geometry` service.
 *
 * Produced data products
 * -----------------------
 *
 * The service guarantees that configuration information of type
 * `sumdata::GeometryConfigurationInfo` is present into the run, accessible with an input
 * tag `GeometryConfigurationWriter`:
 *
 * * if such information is already available in the run, no further information is added
 * * legacy: if there is no such information, but there is a `sumdata::RunData` data
 *   product, a reduced version of the configuration information is created from the
 *   information in that data product (the first one, if multiple are present)
 * * finally, if no information is present neither in the full
 *   `sumdata::GeometryConfigurationInfo` form nor in the legacy `sumdata::RunData` form,
 *   information is put together based on the current configuration of the `Geometry`
 *   service.
 *
 * Service dependencies
 * --------------------
 *
 * * `Geometry` service (for obtaining the current configuration to put into the event)
 *
 * Design details
 * --------------
 *
 * * the choice of delegating the writing of data product to a producing service rather
 *   than to modules is driven by the fact that there is a way to enforce this service
 *   to be actually run, and that no further instrumentation is needed;
 * * the choice of putting the configuration information in the `art::Run` is driven by
 *   the fact that the run is the highest available container; job-level data products
 *   (`art::Results`) behave very differently from the others and are not currently
 *   interfaced with a producing service;
 * * the information in `sumdata::GeometryConfigurationInfo` should be compact enough
 *   not to bloat the data files with very few events per run, as it may be for the
 *   selection of rare processes or signatures.
 */
class geo::GeometryConfigurationWriter : public art::ProducingService {
public:
  /// Service configuration.
  struct Config {
    fhicl::Atom<bool> SkipConfigurationCheck{fhicl::Name("SkipConfigurationCheck"), false};
  };
  using Parameters = art::ServiceTable<Config>;

  GeometryConfigurationWriter(Parameters const&);

private:
  /// Writes the information from the service configuration into the `run`.
  void postReadRun(art::Run& run) override;

  /// Alias for the pointer to the data product object to be put into the run.
  using InfoPtr_t = std::unique_ptr<sumdata::GeometryConfigurationInfo>;

  /// Loads the geometry information from the `run` (either directly or legacy).
  InfoPtr_t previousInfo(art::Run const& run) const;

  /// Creates configuration information based on the current `Geometry` service.
  static InfoPtr_t extractInfoFromGeometry();

  /// Verifies that the geometry configurations for the current and
  /// previous processes are consistent.
  void verifyConsistentConfigs(sumdata::GeometryConfigurationInfo const& A,
                               sumdata::GeometryConfigurationInfo const& B,
                               art::RunID const& id) const;

  /// Reads geometry information from the run (returns null pointer if none).
  InfoPtr_t readGeometryInformation(art::Run const& run) const;

  /// Upgrades legacy `sumdata::RunData` in `run` to geometry information
  /// (returns null pointer if no legacy information is present).
  InfoPtr_t makeInfoFromRunData(art::Run const& run) const;

  /// Returns a pointer to the `sumdata::RunData` in `run` (nullptr if none).
  sumdata::RunData const* readRunData(art::Run const& run) const;

  bool fFatalConfCheck;

}; // geo::GeometryConfigurationWriter

// -----------------------------------------------------------------------------
// --- implementation
// -----------------------------------------------------------------------------

namespace {
  // ---------------------------------------------------------------------------
  auto makeInfoPtr(sumdata::GeometryConfigurationInfo info)
  {
    return std::make_unique<sumdata::GeometryConfigurationInfo>(std::move(info));
  }

  // ---------------------------------------------------------------------------
  // Converts the legacy `data` into geometry configuration information.
  auto convertRunDataToGeometryInformation(sumdata::RunData const& data)
  {
    sumdata::GeometryConfigurationInfo confInfo;

    // we use the simplest version 1 data format (legacy format)
    confInfo.dataVersion = sumdata::GeometryConfigurationInfo::DataVersion_t{1};
    confInfo.detectorName = data.DetName();

    MF_LOG_DEBUG("GeometryConfigurationInfo")
      << "Built geometry configuration information from run data:\n"
      << confInfo;

    return makeInfoPtr(std::move(confInfo));
  }

  //......................................................................
  // Returns if `A` and `B` are compatible geometry service configurations.
  bool compareConfigurationInfo(sumdata::GeometryConfigurationInfo const& A,
                                sumdata::GeometryConfigurationInfo const& B)
  {
    /**
     * Implemented criteria:
     *
     * * both informations must be valid
     * * the detector names must exactly match
     */
    if (!A.isDataValid()) {
      mf::LogWarning("GeometryConfiguration") << "invalid version for configuration A:\n" << A;
      return false;
    }
    if (!B.isDataValid()) {
      mf::LogWarning("GeometryConfiguration") << "invalid version for configuration B:\n" << B;
      return false;
    }

    // currently used only in debug mode (assert())
    [[maybe_unused]] auto const commonVersion = std::min(A.dataVersion, B.dataVersion);

    assert(commonVersion >= 1);

    if (A.detectorName != B.detectorName) { // case sensitive so far
      mf::LogWarning("GeometryConfiguration")
        << "detector name mismatch: '" << A.detectorName << "' vs. '" << B.detectorName << "'";
      return false;
    }

    return true;
  }

}

// -----------------------------------------------------------------------------
geo::GeometryConfigurationWriter::GeometryConfigurationWriter(Parameters const& p)
  : fFatalConfCheck{not p().SkipConfigurationCheck()}
{
  produces<sumdata::GeometryConfigurationInfo, art::InRun>();
}

// -----------------------------------------------------------------------------
void geo::GeometryConfigurationWriter::postReadRun(art::Run& run)
{
  auto previousConfInfo = previousInfo(run);
  auto currentConfInfo = extractInfoFromGeometry();
  if (previousConfInfo && currentConfInfo) {
    verifyConsistentConfigs(*previousConfInfo, *currentConfInfo, run.id());
  }

  InfoPtr_t confInfo = previousConfInfo ? std::move(previousConfInfo) : std::move(currentConfInfo);
  run.put(std::move(confInfo), art::fullRun());
}

// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::previousInfo(art::Run const& run) const -> InfoPtr_t
{
  /**
   * Read geometry configuration information from the run:
   *
   * 1. first attempt to directly read information from past runs of this service
   * 2. if none is found, attempt reading legacy information and upgrade it
   * 3. if no legacy information is found either, return a null pointer
   */
  InfoPtr_t info = readGeometryInformation(run);
  return info ? std::move(info) : makeInfoFromRunData(run);
}

// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::extractInfoFromGeometry() -> InfoPtr_t
{

  sumdata::GeometryConfigurationInfo confInfo =
    art::ServiceHandle<geo::Geometry>()->configurationInfo();

  MF_LOG_DEBUG("GeometryConfigurationWriter")
    << "Geometry configuration information from service:\n"
    << confInfo;

  return makeInfoPtr(std::move(confInfo));
}

// -----------------------------------------------------------------------------
void geo::GeometryConfigurationWriter::verifyConsistentConfigs(
  sumdata::GeometryConfigurationInfo const& A,
  sumdata::GeometryConfigurationInfo const& B,
  art::RunID const& id) const
{
  auto consistentInfo = compareConfigurationInfo(A, B);
  if (consistentInfo) { return; }

  if (fFatalConfCheck) {
    cet::HorizontalRule rule{50};
    throw cet::exception("GeometryConfiguration")
      << "Geometry used for run " << id << " is incompatible with the one configured in the job!"
      << "\n=== job configuration " << rule('=') << "\n"
      << B << "\n=== run configuration " << rule('=') << "\n"
      << A << "\n======================" << rule('=') << "\n";
  }
  mf::LogWarning("GeometryConfiguration")
    << "Geometry used for " << id << " is incompatible with the one configured in the job.";
}

// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::readGeometryInformation(art::Run const& run) const
  -> InfoPtr_t
{
  if (auto h = run.getHandle<sumdata::GeometryConfigurationInfo>(
        art::InputTag{"GeometryConfigurationWriter"})) {
    return makeInfoPtr(*h);
  }
  return nullptr;
}

// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::makeInfoFromRunData(art::Run const& run) const -> InfoPtr_t
{
  sumdata::RunData const* runData = readRunData(run);
  return runData ? convertRunDataToGeometryInformation(*runData) : InfoPtr_t{};
}

// -----------------------------------------------------------------------------
sumdata::RunData const* geo::GeometryConfigurationWriter::readRunData(art::Run const& run) const
{
  auto allRunData = run.getMany<sumdata::RunData>();
  return allRunData.empty() ? nullptr : allRunData.front().product();
}

DEFINE_ART_PRODUCING_SERVICE(geo::GeometryConfigurationWriter)
