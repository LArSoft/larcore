/**
 * @file    DumpGeometry_module.cc
 * @brief   Prints on screen the current geometry.
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    May 30, 2018
 *
 */

// LArSoft libraries
#include "larcore/CoreUtils/ServiceUtil.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/GeometryCore.h"

// framework libraries
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Run.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "canvas/Persistency/Provenance/RunID.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"

// C/C++ standard libraries
#include <string>

namespace geo {
  class DumpGeometry;
}

/** ****************************************************************************
 * @brief Describes on screen the current geometry.
 *
 * One print is performed at the beginning of each run.
 *
 *
 * Configuration parameters
 * =========================
 *
 * - *OutputCategory* (string, default: DumpGeometry): output category used
 *   by the message facility to output information (INFO level)
 *
 */
class geo::DumpGeometry: public art::EDAnalyzer {

    public:
  struct Config {
    using Name = fhicl::Name;
    using Comment = fhicl::Comment;

    fhicl::Atom<std::string> outputCategory {
      Name("outputCategory"),
      Comment
        ("name of message facility output category to stream the information into (INFO level)"),
      "DumpGeometry"
      };

  }; // struct Config

  using Parameters = art::EDAnalyzer::Table<Config>;

  explicit DumpGeometry(Parameters const& config);

  // Plugins should not be copied or assigned.
  DumpGeometry(DumpGeometry const&) = delete;
  DumpGeometry(DumpGeometry &&) = delete;
  DumpGeometry& operator = (DumpGeometry const&) = delete;
  DumpGeometry& operator = (DumpGeometry &&) = delete;

  // Required functions
  virtual void analyze(art::Event const&) override {}

  /// Dumps the geometry at once.
  virtual void beginJob() override;

  /// Dumps the geometry if changed from the previous run.
  virtual void beginRun(art::Run const& run) override;

    private:

  std::string fOutputCategory; ///< Name of the category for output.
  std::string fLastDetectorName; ///< Name of the last geometry dumped.

  /// Dumps the specified geometry into the specified output stream.
  template <typename Stream>
  void dumpGeometryCore(Stream&& out, geo::GeometryCore const& geom) const;

  /// Dumps the geometry and records it.
  template <typename Stream>
  void dump(Stream&& out, geo::GeometryCore const& geom);

  /// Returns whether the specified geometry should be dumped.
  bool shouldDumpGeometry(geo::GeometryCore const& geom) const;

}; // class geo::DumpGeometry


//==============================================================================
//=== Module implementation
//===

//------------------------------------------------------------------------------
geo::DumpGeometry::DumpGeometry(Parameters const& config)
  : EDAnalyzer(config)
  , fOutputCategory(config().outputCategory())
  {}


//------------------------------------------------------------------------------
void geo::DumpGeometry::beginJob() {

  auto const& geom = *(lar::providerFrom<geo::Geometry>());
  dump(mf::LogVerbatim(fOutputCategory), geom);

} // geo::DumpGeometry::beginJob()


//------------------------------------------------------------------------------
void geo::DumpGeometry::beginRun(art::Run const& run) {

  auto const& geom = *(lar::providerFrom<geo::Geometry>());
  if (shouldDumpGeometry(geom)) {
    mf::LogVerbatim log(fOutputCategory);
    log << "\nGeometry used in " << run.id() << ":\n";
    dump(log, geom);
  }

} // geo::DumpGeometry::beginRun()


//------------------------------------------------------------------------------
template <typename Stream>
void geo::DumpGeometry::dumpGeometryCore
  (Stream&& out, geo::GeometryCore const& geom) const
{

  out << "Detector description: '" << geom.ROOTFile() << "'\n";
  geom.Print(std::forward<Stream>(out));

} // geo::DumpGeometry::dumpGeometryCore()


//------------------------------------------------------------------------------
template <typename Stream>
void geo::DumpGeometry::dump(Stream&& out, geo::GeometryCore const& geom) {

  fLastDetectorName = geom.DetectorName();
  dumpGeometryCore(std::forward<Stream>(out), geom);

} // geo::DumpGeometry::dump()


//------------------------------------------------------------------------------
bool geo::DumpGeometry::shouldDumpGeometry(geo::GeometryCore const& geom) const
{

  // only dump if not already dumped
  return geom.DetectorName() != fLastDetectorName;

} // geo::DumpGeometry::shouldDumpGeometry()


//------------------------------------------------------------------------------
DEFINE_ART_MODULE(geo::DumpGeometry)

//==============================================================================
