/**
 * @file   LegacyGeometryInfoWriter_module.cc
 * @brief  Writes a sumdata::RunData record into the run(s).
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 12, 2020
 */

// LArSoft libraries
#include "larcoreobj/SummaryData/RunData.h"

// framework libraries
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "fhiclcpp/types/Atom.h"

// C/C++ standard library
#include <algorithm> // std::transform()
#include <iterator>  // std::back_inserter()
#include <memory>    // std::make_unique()
#include <string>

// -----------------------------------------------------------------------------
namespace art {
  class Event;
}

// -----------------------------------------------------------------------------
/**
 * @brief Writes a sumdata::RunData record into the run(s).
 *
 * A copy of the same data product is put into each of the runs on opening.
 *
 *
 * Configuration parameters
 * =========================
 *
 * * `Name` (string, mandatory): name of the detector to be stored in the
 *     `sumdata::RunData` data product
 *
 */
namespace geo {
  class LegacyGeometryInfoWriter;
}
class geo::LegacyGeometryInfoWriter : public art::EDProducer {
public:
  struct Config {

    fhicl::Atom<std::string> Name{fhicl::Name{"Name"},
                                  fhicl::Comment{"Name of the detector to be stored in the data"}};

  }; // struct Config

  using Parameters = art::EDProducer::Table<Config>;

  explicit LegacyGeometryInfoWriter(Parameters const& config);

  virtual void beginRun(art::Run& run) override;

  virtual void produce(art::Event&) override {}

private:
  std::string fDetectorName; ///< Name of the detector.

}; // class geo::LegacyGeometryInfoWriter

// -----------------------------------------------------------------------------
// ---  geo::LegacyGeometryInfoWriter implementation
// -----------------------------------------------------------------------------

namespace {

  std::string toLower(std::string const& S)
  {

    std::string s;
    s.reserve(S.length());
    std::transform(S.begin(), S.end(), back_inserter(s), ::tolower);
    return s;

  } // toLower()

} // local namespace

// -----------------------------------------------------------------------------
geo::LegacyGeometryInfoWriter::LegacyGeometryInfoWriter(Parameters const& config)
  : art::EDProducer(config), fDetectorName(::toLower(config().Name()))
{

  produces<sumdata::RunData, art::InRun>();

} // geo::LegacyGeometryInfoWriter::LegacyGeometryInfoWriter()

// -----------------------------------------------------------------------------
void geo::LegacyGeometryInfoWriter::beginRun(art::Run& run)
{

  run.put(std::make_unique<sumdata::RunData>(fDetectorName), art::fullRun());

} // geo::LegacyGeometryInfoWriter::beginRun()

// -----------------------------------------------------------------------------
DEFINE_ART_MODULE(geo::LegacyGeometryInfoWriter)

// -----------------------------------------------------------------------------
