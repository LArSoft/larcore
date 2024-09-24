/**
 * @file    DumpChannelMap_module.cc
 * @brief   Prints on screen the current channel-wire map
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    October 27th, 2015
 *
 */

// LArSoft libraries
#include "larcore/Geometry/WireReadout.h"
#include "larcorealg/Geometry/GeometryCore.h"
#include "larcorealg/Geometry/OpDetGeo.h"
#include "larcorealg/Geometry/WireReadoutGeom.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"  // raw::ChannelID_t
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h" // geo::WireID

// framework libraries
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Utilities/Exception.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/types/Name.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <string>

namespace {
  //------------------------------------------------------------------------------
  void dumpChannelToWires(std::string const& OutputCategory,
                          geo::WireReadoutGeom const& wireReadoutGeom,
                          raw::ChannelID_t FirstChannel,
                          raw::ChannelID_t LastChannel)
  {
    /// extract general channel range information
    unsigned int const NChannels = wireReadoutGeom.Nchannels();

    if (NChannels == 0) {
      mf::LogError(OutputCategory) << "Nice detector we have here, with no channels.";
      return;
    }

    raw::ChannelID_t const PrintFirst =
      raw::isValidChannelID(FirstChannel) ? FirstChannel : raw::ChannelID_t(0);
    raw::ChannelID_t const PrintLast =
      raw::isValidChannelID(LastChannel) ? LastChannel : raw::ChannelID_t(NChannels - 1);

    // print intro
    unsigned int const NPrintedChannels = (PrintLast - PrintFirst) + 1;
    if (NPrintedChannels == NChannels) {
      mf::LogInfo(OutputCategory) << "Printing all " << NChannels << " channels";
    }
    else {
      mf::LogInfo(OutputCategory) << "Printing channels from " << PrintFirst << " to "
                                  << LastChannel << " (" << NPrintedChannels << " channels out of "
                                  << NChannels << ")";
    }

    // print map
    mf::LogVerbatim log(OutputCategory);
    for (raw::ChannelID_t channel = PrintFirst; channel <= PrintLast; ++channel) {
      std::vector<geo::WireID> const Wires = wireReadoutGeom.ChannelToWire(channel);

      log << "\n " << ((int)channel) << " ->";
      switch (Wires.size()) {
      case 0: log << " no wires"; break;
      case 1: break;
      default: log << " [" << Wires.size() << " wires]"; break;
      }

      for (geo::WireID const& wireID : Wires) {
        log << " { " << std::string(wireID) << " };";
      }
    }
  }

  //------------------------------------------------------------------------------
  void dumpWireToChannel(std::string const& OutputCategory,
                         geo::WireReadoutGeom const& wireReadoutGeom)
  {
    /// extract general channel range information
    unsigned int const NChannels = wireReadoutGeom.Nchannels();

    if (NChannels == 0) {
      mf::LogError(OutputCategory) << "Nice detector we have here, with no channels.";
      return;
    }

    // print intro
    mf::LogInfo(OutputCategory) << "Printing wire channels for up to " << NChannels << " channels";

    // print map
    mf::LogVerbatim log(OutputCategory);
    for (geo::WireID const& wireID : wireReadoutGeom.Iterate<geo::WireID>()) {
      raw::ChannelID_t channel = wireReadoutGeom.PlaneWireToChannel(wireID);
      log << "\n { " << std::string(wireID) << " } => ";
      if (raw::isValidChannelID(channel))
        log << channel;
      else
        log << "invalid!";
    } // for
  }

  //------------------------------------------------------------------------------
  geo::OpDetGeo const* getOpticalDetector(geo::WireReadoutGeom const& wireReadoutGeom,
                                          unsigned int channelID)
  {
    try {
      return &wireReadoutGeom.OpDetGeoFromOpChannel(channelID);
    }
    catch (cet::exception const&) {
      return nullptr;
    }
  }

  //------------------------------------------------------------------------------
  void dumpOpticalDetectorChannels(std::string const& OutputCategory,
                                   geo::WireReadoutGeom const& wireReadoutGeom)
  {
    /// extract general channel range information
    unsigned int const NChannels = wireReadoutGeom.NOpChannels();

    if (NChannels == 0) {
      mf::LogError(OutputCategory) << "Nice detector we have here, with no optical channels.";
      return;
    }

    // print intro
    mf::LogInfo(OutputCategory) << "Printing optical detectors for up to " << NChannels
                                << " channels";

    // print map
    mf::LogVerbatim log(OutputCategory);
    for (unsigned int channelID = 0; channelID < NChannels; ++channelID) {
      log << "\nChannel " << channelID << " => ";
      geo::OpDetGeo const* opDet = getOpticalDetector(wireReadoutGeom, channelID);
      if (!opDet) {
        log << "invalid";
        continue;
      }
      log << opDet->ID() << " at " << opDet->GetCenter() << " cm";
    } // for
  }
}

namespace geo {
  class DumpChannelMap;
}

/** ****************************************************************************
 * @brief Prints on screen the current channel-wire and optical detector maps.
 *
 * One print is performed at the beginning of each run.
 *
 *
 * Configuration parameters
 * =========================
 *
 * - *ChannelToWires* (boolean, default: true): prints all the wires
 *   corresponding to each channel
 * - *WireToChannel* (boolean, default: false): prints which channel covers
 *   each wire
 * - *OpDetChannels* (boolean, default: false): prints for each optical detector
 *   channel ID the optical detector ID and its center
 * - *FirstChannel* (integer, default: no limit): ID of the lowest channel to be
 *   printed
 * - *LastChannel* (integer, default: no limit): ID of the highest channel to be
 *   printed
 * - *OutputCategory* (string, default: DumpChannelMap): output category used
 *   by the message facility to output information (INFO level)
 *
 */

class geo::DumpChannelMap : public art::EDAnalyzer {
public:
  /// Module configuration.
  struct Config {
    using Name = fhicl::Name;
    using Comment = fhicl::Comment;

    fhicl::Atom<std::string> OutputCategory{
      Name("OutputCategory"),
      Comment("output category used by the message facility to output information (INFO level)"),
      "DumpChannelMap"};

    fhicl::Atom<bool> ChannelToWires{Name("ChannelToWires"),
                                     Comment("print all the wires corresponding to each channel"),
                                     true};

    fhicl::Atom<bool> WireToChannel{Name("WireToChannel"),
                                    Comment("print which channel covers each wire"),
                                    false};

    fhicl::Atom<bool> OpDetChannels{
      Name("OpDetChannels"),
      Comment("print for each optical detector channel ID the optical detector ID and its center"),
      false};

    fhicl::Atom<raw::ChannelID_t> FirstChannel{
      Name("FirstChannel"),
      Comment("ID of the lowest channel to be printed (default: no limit)"),
      raw::InvalidChannelID};

    fhicl::Atom<raw::ChannelID_t> LastChannel{
      Name("LastChannel"),
      Comment("ID of the highest channel to be printed (default: no limit)"),
      raw::InvalidChannelID};

  }; // Config

  using Parameters = art::EDAnalyzer::Table<Config>;

  explicit DumpChannelMap(Parameters const& config);

  // Plugins should not be copied or assigned.
  DumpChannelMap(DumpChannelMap const&) = delete;
  DumpChannelMap(DumpChannelMap&&) = delete;
  DumpChannelMap& operator=(DumpChannelMap const&) = delete;
  DumpChannelMap& operator=(DumpChannelMap&&) = delete;

  // Required functions
  void analyze(art::Event const&) override {}

  /// Drives the dumping
  void beginRun(art::Run const&) override;

private:
  std::string OutputCategory; ///< Name of the category for output.
  bool DoChannelToWires;      ///< Dump channel -> wires mapping.
  bool DoWireToChannel;       ///< Dump wire -> channel mapping.
  bool DoOpDetChannels;       ///< Dump optical detector channel -> optical detector.

  raw::ChannelID_t FirstChannel; ///< First channel to be printed.
  raw::ChannelID_t LastChannel;  ///< Last channel to be printed.

}; // geo::DumpChannelMap

//------------------------------------------------------------------------------
geo::DumpChannelMap::DumpChannelMap(Parameters const& config)
  : art::EDAnalyzer(config)
  , OutputCategory(config().OutputCategory())
  , DoChannelToWires(config().ChannelToWires())
  , DoWireToChannel(config().WireToChannel())
  , DoOpDetChannels(config().OpDetChannels())
  , FirstChannel(config().FirstChannel())
  , LastChannel(config().LastChannel())
{}

//------------------------------------------------------------------------------
void geo::DumpChannelMap::beginRun(art::Run const&)
{
  geo::WireReadoutGeom const& wireReadoutGeom = art::ServiceHandle<geo::WireReadout const>()->Get();

  if (DoChannelToWires) {
    dumpChannelToWires(OutputCategory, wireReadoutGeom, FirstChannel, LastChannel);
  }
  if (DoWireToChannel) { dumpWireToChannel(OutputCategory, wireReadoutGeom); }
  if (DoOpDetChannels) { dumpOpticalDetectorChannels(OutputCategory, wireReadoutGeom); }
}

DEFINE_ART_MODULE(geo::DumpChannelMap)
