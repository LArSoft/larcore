/**
 * @file   GeometryInfoCheck_module.cc
 * @brief  Verifies that the geometry check information is available in the run.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 11, 2020
 */

// LArSoft libraries
#include "larcore/Geometry/Geometry.h"
#include "larcoreobj/SummaryData/GeometryConfigurationInfo.h"
#include "larcoreobj/SummaryData/RunData.h"

// framework libraries
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Provenance.h"
#include "canvas/Utilities/InputTag.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/types/OptionalTable.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/Atom.h"
#include "cetlib_except/exception.h"

// C/C++ standard library
#include <vector>
#include <string>
#include <optional>
#include <utility> // std::move()

// -----------------------------------------------------------------------------
namespace art { class Event; }

// -----------------------------------------------------------------------------
/**
 * @brief Verifies that the geometry check information is available.
 *
 * Configuration parameters
 * =========================
 *
 */
namespace geo { class GeometryInfoCheck; }
class geo::GeometryInfoCheck: public art::EDAnalyzer {
    public:
  
  struct GeometryInfoConfig {
    
    fhicl::OptionalAtom<std::string> Name {
      fhicl::Name{ "Name" },
      fhicl::Comment{ "Name of the geometry (if omitted, any will do)" }
      };
    
    fhicl::Atom<bool> Required {
      fhicl::Name{ "Required" },
      fhicl::Comment{ "Whether the presence of this item is mandatory" },
      true
      };
    
  }; // struct GeometryInfoConfig
  
  
  struct Config {
    
    fhicl::OptionalTable<GeometryInfoConfig> GeometryInfo {
      fhicl::Name{ "GeometryInfo" },
      fhicl::Comment{ "Information to check about geometry" }
      };
    
    fhicl::OptionalTable<GeometryInfoConfig> GeometryLegacyInfo {
      fhicl::Name{ "LegacyInfo" },
      fhicl::Comment
        { "Information to check about geometry (legacy from RunData)" }
      };
    
  }; // struct Config
  
  using Parameters = art::EDAnalyzer::Table<Config>;
  
  explicit GeometryInfoCheck(Parameters const& config);

  virtual void beginRun(art::Run const& run) override;

  virtual void analyze(art::Event const&) override {}
  
    private:
  
  struct GeometryInfoCheckInfo {
    bool required; ///< Whether the information must be present.
    std::string detectorName; ///< Name of the detector; empty: don't check.
  };
  
  /// Information on the check on the regular geometry information.
  std::optional<GeometryInfoCheckInfo> fCheckInfo;
  
  /// Information on the check on the legacy geometry information.
  std::optional<GeometryInfoCheckInfo> fLegacyCheckInfo;
  
  /// Performs a geometry information check.
  /// @throw cet::exception on check failures
  void CheckGeometryInfo
    (art::Run const& run, GeometryInfoCheckInfo const& config) const;
  
  /// Performs a legacy geometry information check
  /// @throw cet::exception on check failures
  void CheckLegacyGeometryInfo
    (art::Run const& run, GeometryInfoCheckInfo const& config) const;
  
  
  /// The name of the tag for the geometry information.
  static art::InputTag const GeometryConfigurationWriterTag;
  
  /// Fills a `GeometryInfoCheckInfo` out of the specified configuration.
  static std::optional<GeometryInfoCheckInfo> makeGeometryInfoCheckInfo
    (fhicl::OptionalTable<GeometryInfoConfig> const& config);
  
}; // class geo::GeometryInfoCheck


// -----------------------------------------------------------------------------
// ---  geo::GeometryInfoCheck implementation
// -----------------------------------------------------------------------------

namespace {
  
  bool case_insensitive_equal(std::string const& a, std::string const& b) {
    
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
      [](auto a, auto b)
        {
          return ::tolower(static_cast<unsigned char>(a))
            == ::tolower(static_cast<unsigned char>(b));
        }
      );
    
  } // case_insensitive_equal()
  
} // local namespace

// -----------------------------------------------------------------------------
art::InputTag const geo::GeometryInfoCheck::GeometryConfigurationWriterTag
  { "GeometryConfigurationWriter" };


// -----------------------------------------------------------------------------
geo::GeometryInfoCheck::GeometryInfoCheck(Parameters const& config)
  : art::EDAnalyzer(config)
  , fCheckInfo(makeGeometryInfoCheckInfo(config().GeometryInfo))
  , fLegacyCheckInfo(makeGeometryInfoCheckInfo(config().GeometryLegacyInfo))
{
  
  if (fCheckInfo)
   consumes<sumdata::GeometryConfigurationInfo>(GeometryConfigurationWriterTag);
  if (fLegacyCheckInfo) consumesMany<sumdata::RunData>();
  
  
  // --- BEGIN -- Configuration dump -------------------------------------------
  {
    mf::LogDebug log { "GeometryInfoCheck" };
    log << "Configuration:"
      << "\n - geometry configuration check:";
    if (fCheckInfo) {
      if (fCheckInfo->detectorName.empty()) log << " (any name)";
      else log << " must match '" << fCheckInfo->detectorName << "'";
      log << " [" << (fCheckInfo->required? "mandatory": "optional") << "]";
    }
    else log << "not requested";
    
    log << "\n - legacy geometry configuration information check:";
    if (fLegacyCheckInfo) {
      if (fLegacyCheckInfo->detectorName.empty()) log << " (any name)";
      else log << " must match '" << fLegacyCheckInfo->detectorName << "'";
      log << " [" << (fLegacyCheckInfo->required? "mandatory": "optional") << "]";
    }
    else log << "not requested";
    
  }
  // --- END -- Configuration dump ---------------------------------------------
  
} // geo::GeometryInfoCheck::GeometryInfoCheck()


// -----------------------------------------------------------------------------
void geo::GeometryInfoCheck::beginRun(art::Run const& run) {
  
  if (fCheckInfo) CheckGeometryInfo(run, fCheckInfo.value());
  if (fLegacyCheckInfo) CheckLegacyGeometryInfo(run, fLegacyCheckInfo.value());
  
} // geo::GeometryInfoCheck::beginRun()


// -----------------------------------------------------------------------------
void geo::GeometryInfoCheck::CheckGeometryInfo
  (art::Run const& run, GeometryInfoCheckInfo const& config) const
{
  
  mf::LogDebug log { "GeometryInfoCheck" };
  log << "Check on geometry information.";
  
  //
  // fetch geometry information
  //
  art::Handle<sumdata::GeometryConfigurationInfo> hInfo;
  if (!run.getByLabel(GeometryConfigurationWriterTag, hInfo)) {
    log << "\nNo information found.";
    if (!config.required) return; // well... all ok?
    
    log << "\nUnfortunately, that was required...";
    throw cet::exception("GeometryInfoCheck")
      << "Required geometry information not found as '"
      << GeometryConfigurationWriterTag.encode() << "'\n";
  } // if no info
  
  sumdata::GeometryConfigurationInfo const& info { *hInfo };
  log
    << "\nFound geometry information (version " << info.dataVersion << ")"
      << " from '" << hInfo.provenance()->inputTag().encode() << "'"
    << "\nGeometry name is '" << info.detectorName << "'."
    ;
  
  //
  // check: the name
  //
  if (!config.detectorName.empty()
    && !case_insensitive_equal(info.detectorName, config.detectorName)
  ) {
    throw cet::exception("GeometryInfoCheck")
      << "Geometry information reports an unexpected name '"
      << info.detectorName << "' ('" << config.detectorName << "' expected)\n"
      ;
  } // if
  
  //
  // all checks done
  //
  
} // geo::GeometryInfoCheck::CheckGeometryInfo()


// -----------------------------------------------------------------------------
void geo::GeometryInfoCheck::CheckLegacyGeometryInfo
  (art::Run const& run, GeometryInfoCheckInfo const& config) const
{
  
  mf::LogDebug log { "GeometryInfoCheck" };
  log << "Check on legacy geometry information.";
  
  //
  // fetch geometry information
  //
  //std::vector<art::Handle<sumdata::RunData>> hInfoList;
  //run.getManyByType(hInfoList);
  auto hInfoList = run.getMany<sumdata::RunData>();
  if (hInfoList.empty()) {
    log << "\nNo information found.";
    if (!config.required) return; // well... all ok?
    
    log << "\nUnfortunately, that was required...";
    throw cet::exception("GeometryInfoCheck")
      << "No legacy geometry information found!\n";
  } // if no info
  
  log << "\nFound " << hInfoList.size() << " legacy geometry records:";
  art::Handle<sumdata::RunData> const* hInfo = nullptr;
  for (auto const& handle: hInfoList) {
    log << "\n - " << handle.provenance()->inputTag().encode();
    if (handle.failedToGet()) log << " (not present)";
    else if (!hInfo) {
      hInfo = &handle;
      log << " (this will be used for the check)";
    }
  } // for all handles
  
  sumdata::RunData const& info { *(hInfo->product()) };
  log
    << "\nFound legacy geometry information "
    << "\nGeometry name is '" << info.DetName() << "'."
    ;
  
  //
  // check: the name
  //
  if (!config.detectorName.empty()
    && !case_insensitive_equal(info.DetName(), config.detectorName)
  ) {
    throw cet::exception("GeometryInfoCheck")
      << "Geometry legacy information reports an unexpected name '"
      << info.DetName() << "' ('" << config.detectorName << "' expected)\n"
      ;
  } // if
  
  //
  // all checks done
  //
  
} // geo::GeometryInfoCheck::CheckLegacyGeometryInfo()


// -----------------------------------------------------------------------------
auto geo::GeometryInfoCheck::makeGeometryInfoCheckInfo
  (fhicl::OptionalTable<GeometryInfoConfig> const& config)
  -> std::optional<GeometryInfoCheckInfo>
{
  GeometryInfoConfig infoConfig;
  if (!config(infoConfig)) return {};
  
  GeometryInfoCheckInfo info;
  info.required = infoConfig.Required();
  infoConfig.Name(info.detectorName); // so far, not specifying it defaults to empty
  
  return { std::move(info) };
} // geo::GeometryInfoCheck::makeGeometryInfoCheckInfo()


// -----------------------------------------------------------------------------
DEFINE_ART_MODULE(geo::GeometryInfoCheck)


// -----------------------------------------------------------------------------
