/**
 * @file   larcore/Geometry/GeometryConfigurationWriter_service.cc
 * @brief  Service writing geometry configuration information into _art_ runs.
 * @date   October 7, 2020
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 *
 * Producing services have no header.
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
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <memory> // std::make_unique()
#include <utility> // std::move()

// -----------------------------------------------------------------------------
namespace geo { class GeometryConfigurationWriter; }
/**
 * @brief Writes geometry configuration information into _art_ runs.
 * 
 * This service is part of the mandatory version check of `geo::Geometry`
 * service.
 * It does not require any special configuration, but it must be listed
 * in the configuration in order for `Geometry` to work:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * services: {
 *   
 *   GeometryConfigurationWriter: {}
 *   
 *   Geometry: @local::experiment_geometry
 *   
 *   # ...
 *  
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * The configuration check is described in the documentation of `geo::Geometry`
 * service.
 * 
 * 
 * Produced data products
 * -----------------------
 * 
 * The service guarantees that configuration information of type
 * `sumdata::GeometryConfigurationInfo` is present into the run, accessible with
 * an input tag `GeometryConfigurationWriter`:
 * 
 * * if such information is already available in the run, no further information
 *   is added
 * * legacy: if there is no such information, but there is a `sumdata::RunData`
 *   data product, a reduced version of the configuration information is created
 *   from the information in that data product (the first one, if multiple are
 *   present)
 * * finally, if no information is present neither in the full 
 *   `sumdata::GeometryConfigurationInfo` form nor in the legacy
 *   `sumdata::RunData` form, information is put together based on the current
 *   configuration of the `Geometry` service.
 * 
 * 
 * Service dependencies
 * ---------------------
 * 
 * * `Geometry` service
 *   (for obtaining the current configuration to put into the event)
 * 
 */
class geo::GeometryConfigurationWriter: public art::ProducingService {
  
    public:
  
  /// Service configuration.
  struct Config {
    // no configuration parameters at this time
  };
  
  using Parameters = art::ServiceTable<Config>;
  
  /// Constructor: gets its configuration and does nothing with it.
  GeometryConfigurationWriter(Parameters const&);
  
  
    private:
  
  /// Writes the information from the service configuration into the `run`.
  virtual void postReadRun(art::Run& run) override;
  
  
    private:
  
  /// Alias for the pointer to the data product object to be put into the run.
  using InfoPtr_t = std::unique_ptr<sumdata::GeometryConfigurationInfo>;
  
  
  /// Loads the geometry information from the `run` (either directly or legacy).
  InfoPtr_t loadInfo(art::Run& run) const;
  
  /// Creates configuration information based on the current `Geometry` service.
  static InfoPtr_t extractInfoFromGeometry();
  
  /// Reads geometry information from the run (returns null pointer if none).
  InfoPtr_t readGeometryInformation(art::Run& run) const;
  
  /// Upgrades legacy `sumdata::RunData` in `run` to geometry information
  /// (returns null pointer if no legacy information is present).
  InfoPtr_t makeInfoFromRunData(art::Run& run) const;
  
  /// Returns a pointer to the `sumdata::RunData` in `run` (nullptr if none).
  sumdata::RunData const* readRunData(art::Run& run) const;
  
  /// Converts the legacy `data` into geometry configuration information.
  static InfoPtr_t convertRunDataToGeometryInformation
    (sumdata::RunData const& data);
  
  /// Alias to `std::make_unique<sumdata::GeometryConfigurationInfo>`.
  static InfoPtr_t makeInfoPtr(sumdata::GeometryConfigurationInfo const& info)
    { return std::make_unique<sumdata::GeometryConfigurationInfo>(info); }
  
  
}; // geo::GeometryConfigurationWriter


// -----------------------------------------------------------------------------
// --- implementation
// -----------------------------------------------------------------------------
geo::GeometryConfigurationWriter::GeometryConfigurationWriter(Parameters const&)
{
  produces<sumdata::GeometryConfigurationInfo, art::InRun>();
}


// -----------------------------------------------------------------------------
void geo::GeometryConfigurationWriter::postReadRun(art::Run& run) {
  
  InfoPtr_t confInfo = geo::GeometryConfigurationWriter::loadInfo(run);
  
  if (!confInfo) confInfo = extractInfoFromGeometry();
  
  run.put(std::move(confInfo), art::fullRun());
  
} // geo::GeometryConfigurationWriter::postReadRun()


// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::loadInfo(art::Run& run) const
  -> InfoPtr_t
{
  
  /*
   * Read geometry configuration information from the run:
   * 
   * 1. first attempt to directly read information from past runs of this
   *    service
   * 2. if none is found, attempt reading legacy information and upgrade it
   * 3. if no legacy information is found either, return a null pointer
   * 
   */
  InfoPtr_t info = readGeometryInformation(run);
  
  return info? std::move(info): makeInfoFromRunData(run);
  
} // geo::GeometryConfigurationWriter::loadInfo()


// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::extractInfoFromGeometry() -> InfoPtr_t {
  
  sumdata::GeometryConfigurationInfo confInfo
    = art::ServiceHandle<geo::Geometry>()->configurationInfo();
  
  MF_LOG_DEBUG("GeometryConfigurationWriter")
    << "Geometry configuration information from service:\n" << confInfo;
  
  return makeInfoPtr(std::move(confInfo));
  
} // geo::GeometryConfigurationWriter::extractInfoFromGeometry()


// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::readGeometryInformation
  (art::Run& run) const -> InfoPtr_t
{
  
  art::Handle<sumdata::GeometryConfigurationInfo> infoHandle;
  return
    run.getByLabel(art::InputTag{"GeometryConfigurationWriter"}, infoHandle)
    ? makeInfoPtr(*infoHandle): InfoPtr_t{};
  
} // geo::GeometryConfigurationWriter::hasGeometryInformation()


// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::makeInfoFromRunData(art::Run& run) const
  -> InfoPtr_t
{
  
  sumdata::RunData const* runData = readRunData(run);
  
  return runData? convertRunDataToGeometryInformation(*runData): InfoPtr_t{};
  
} // geo::GeometryConfigurationWriter::makeInfoFromRunData()


// -----------------------------------------------------------------------------
sumdata::RunData const* geo::GeometryConfigurationWriter::readRunData
  (art::Run& run) const
{
  //std::vector<art::Handle<sumdata::RunData>> allRunData;
  //run.getManyByType(allRunData);
  auto allRunData = run.getMany<sumdata::RunData>();
  return allRunData.empty()? nullptr: allRunData.front().product();
} // geo::GeometryConfigurationWriter::readRunData()


// -----------------------------------------------------------------------------
auto geo::GeometryConfigurationWriter::convertRunDataToGeometryInformation
  (sumdata::RunData const& data) -> InfoPtr_t
{
  
  sumdata::GeometryConfigurationInfo confInfo;
  
  // we use the simplest version 1 data format (legacy format)
  confInfo.dataVersion = sumdata::GeometryConfigurationInfo::DataVersion_t{1};
  confInfo.detectorName = data.DetName();
  
  MF_LOG_DEBUG("GeometryConfigurationInfo")
   << "Built geometry configuration information from run data:\n" << confInfo;
  
  return makeInfoPtr(std::move(confInfo));
  
} // geo::GeometryConfigurationWriter::convertRunDataToGeometryInformation()


// -----------------------------------------------------------------------------
DEFINE_ART_PRODUCING_SERVICE(geo::GeometryConfigurationWriter)


// -----------------------------------------------------------------------------
