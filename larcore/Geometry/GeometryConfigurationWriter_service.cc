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
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Core/ProducingService.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Utilities/InputTag.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <memory> // std::make_unique()


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
  
  // -- BEGIN --- Interface ----------------------------------------------------
      
  /// Writes the information from the service configuration into the `run`.
  virtual void postReadRun(art::Run& run) override;
  
  // -- END --- Interface ------------------------------------------------------
  
  
  /// Writes the information from the service configuration into the `run`.
  void writeGeometryInformation(art::Run& run) const;
  
  /// Writes geometry information made up from the current run data.
  void writeGeometryInformationFromRunData
    (art::Run& run, sumdata::RunData const& data) const;
  
  /// Returns if there is already geometry configuration information in `run`.
  bool hasGeometryInformation(art::Run& run) const;
  
  /// Returns the first of the `sumdata::RunData` information records in `run`.
  sumdata::RunData const* readRunData(art::Run& run) const;
  
  /// Puts the specified configuration information into the `run`.
  void putGeometryInformationIntoRun
    (art::Run& run, sumdata::GeometryConfigurationInfo confInfo) const;
  
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
  
  if (!hasGeometryInformation(run)) {
    sumdata::RunData const* runData = readRunData(run);
    if (runData) writeGeometryInformationFromRunData(run, *runData);
    else writeGeometryInformation(run);
  }
  
} // geo::GeometryConfigurationWriter::postReadRun()


// -----------------------------------------------------------------------------
void geo::GeometryConfigurationWriter::writeGeometryInformation
  (art::Run& run) const
{
  
  sumdata::GeometryConfigurationInfo const& confInfo
    = art::ServiceHandle<geo::Geometry>()->configurationInfo();
  
  MF_LOG_DEBUG("GeometryConfigurationWriter")
    << "Writing geometry configuration information:\n" << confInfo;
  
  putGeometryInformationIntoRun(run, std::move(confInfo));
  
} // geo::GeometryConfigurationWriter::writeGeometryInformation()


// -----------------------------------------------------------------------------
void geo::GeometryConfigurationWriter::writeGeometryInformationFromRunData
  (art::Run& run, sumdata::RunData const& data) const
{
  
  // we use the simplest version 1 data format
  sumdata::GeometryConfigurationInfo confInfo;
  confInfo.dataVersion = sumdata::GeometryConfigurationInfo::DataVersion_t{1};
  confInfo.detectorName = data.DetName();
  confInfo.geometryServiceConfiguration = "{}";
  
  MF_LOG_DEBUG("GeometryConfigurationInfo")
   << "Writing geometry configuration information from run data:\n" << confInfo;
  
  putGeometryInformationIntoRun(run, std::move(confInfo));
  
} // geo::GeometryConfigurationWriter::writeGeometryInformationFromRunData()


// -----------------------------------------------------------------------------
void geo::GeometryConfigurationWriter::putGeometryInformationIntoRun
  (art::Run& run, sumdata::GeometryConfigurationInfo confInfo) const
{
  run.put(
    std::make_unique<sumdata::GeometryConfigurationInfo>(std::move(confInfo)),
    art::fullRun()
    );
} // geo::GeometryConfigurationWriter::putGeometryInformationIntoRun()


// -----------------------------------------------------------------------------
bool geo::GeometryConfigurationWriter::hasGeometryInformation
  (art::Run& run) const
{
  
  art::Handle<sumdata::GeometryConfigurationInfo> h;
  return run.getByLabel(art::InputTag{"GeometryConfigurationWriter"}, h);
  
} // geo::GeometryConfigurationWriter::hasGeometryInformation()


// -----------------------------------------------------------------------------
sumdata::RunData const* geo::GeometryConfigurationWriter::readRunData
  (art::Run& run) const
{
  std::vector<art::Handle<sumdata::RunData>> allRunData;
  run.getManyByType(allRunData);
  return allRunData.empty()? nullptr: allRunData.front().product();
} // geo::GeometryConfigurationWriter::readRunData()


// -----------------------------------------------------------------------------
DEFINE_ART_PRODUCING_SERVICE(geo::GeometryConfigurationWriter)


// -----------------------------------------------------------------------------
