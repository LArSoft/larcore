/**
 * @file   Geometry_service.cc
 * @brief  art framework interface to geometry description - implementation file
 * @author brebel@fnal.gov
 * @see    Geometry.h
 */

// class header
#include "larcore/Geometry/Geometry.h"

// lar includes
#include "larcorealg/Geometry/GeometryBuilderStandard.h"
#include "larcore/Geometry/ExptGeoHelperInterface.h"

// Framework includes
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "canvas/Utilities/InputTag.h"
#include "canvas/Utilities/Exception.h"
#include "fhiclcpp/types/Table.h"
#include "cetlib_except/exception.h"
#include "cetlib/search_path.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <string>
#include <algorithm> // std::min()
#include <cassert>

// check that the requirements for geo::Geometry are satisfied
template struct lar::details::ServiceRequirementsChecker<geo::Geometry>;

namespace geo {

  //......................................................................
  // Constructor.
  Geometry::Geometry(fhicl::ParameterSet const& pset, art::ActivityRegistry &reg)
    : GeometryCore(pset)
    , fRelPath          (pset.get< std::string       >("RelativePath",     ""   ))
    , fDisableWiresInG4 (pset.get< bool              >("DisableWiresInG4", false))
    , fNonFatalConfCheck(pset.get< bool              >("SkipConfigurationCheck", false))
    , fSortingParameters(pset.get<fhicl::ParameterSet>("SortingParameters", fhicl::ParameterSet() ))
    , fBuilderParameters(pset.get<fhicl::ParameterSet>("Builder",          fhicl::ParameterSet() ))
  {

    if (pset.has_key("ForceUseFCLOnly")) {
      throw art::Exception(art::errors::Configuration)
        << "Geometry service does not support `ForceUseFCLOnly` configuration parameter any more.\n";
    }

    // add a final directory separator ("/") to fRelPath if not already there
    if (!fRelPath.empty() && (fRelPath.back() != '/')) fRelPath += '/';

    // register a callback to be executed when a new run starts
    reg.sPreBeginRun.watch(this, &Geometry::preBeginRun);

    //......................................................................
    // 5.15.12 BJR: use the gdml file for both the fGDMLFile and fROOTFile
    // variables as ROOT v5.30.06 is once again able to read in gdml files
    // during batch operation, in this case think of fROOTFile meaning the
    // file used to make the ROOT TGeoManager.  I don't want to remove
    // the separate variables in case ROOT breaks again
    std::string GDMLFileName = pset.get<std::string>("GDML");
    std::string ROOTFileName = pset.get<std::string>("GDML");

    // load the geometry
    LoadNewGeometry(GDMLFileName, ROOTFileName);

    FillGeometryConfigurationInfo(pset);

  } // Geometry::Geometry()


  void Geometry::preBeginRun(art::Run const& run)
  {

    sumdata::GeometryConfigurationInfo const inputGeomInfo
      = ReadConfigurationInfo(run);
    if (!CheckConfigurationInfo(inputGeomInfo)) {
      if (fNonFatalConfCheck) {
        // disable the non-fatal option if you need the details
        mf::LogWarning("Geometry") << "Geometry used for " << run.id()
          << " is incompatible with the one configured in the job.";
      }
      else {
        throw cet::exception("Geometry")
          << "Geometry used for run " << run.id()
          << " is incompatible with the one configured in the job!"
          << "\n=== job configuration " << std::string(50, '=')
          << "\n" << fConfInfo
          << "\n=== run configuration " << std::string(50, '=')
          << "\n" << inputGeomInfo
          << "\n======================" << std::string(50, '=')
          << "\n";
      }
    }

  } // Geometry::preBeginRun()


  //......................................................................
  void Geometry::InitializeChannelMap()
  {
    // the channel map is responsible of calling the channel map configuration
    // of the geometry
    art::ServiceHandle<geo::ExptGeoHelperInterface const> helper{};
    auto channelMapAlg = helper->ConfigureChannelMapAlg(fSortingParameters,
                                                        DetectorName());
    if (!channelMapAlg) {
      throw cet::exception("ChannelMapLoadFail")
        << " failed to load new channel map";
    }
    ApplyChannelMap(move(channelMapAlg));
  } // Geometry::InitializeChannelMap()

  //......................................................................
  void Geometry::LoadNewGeometry(
    std::string gdmlfile, std::string /* rootfile */,
    bool bForceReload /* = false */
  ) {
    // start with the relative path
    std::string GDMLFileName(fRelPath), ROOTFileName(fRelPath);

    // add the base file names
    ROOTFileName.append(gdmlfile); // not rootfile (why?)
    GDMLFileName.append(gdmlfile);

    // special for GDML if geometry with no wires is used for Geant4 simulation
    if(fDisableWiresInG4)
      GDMLFileName.insert(GDMLFileName.find(".gdml"), "_nowires");

    // Search all reasonable locations for the GDML file that contains
    // the detector geometry.
    // cet::search_path constructor decides if initialized value is a path
    // or an environment variable
    cet::search_path const sp{"FW_SEARCH_PATH"};

    std::string GDMLfile;
    if( !sp.find_file(GDMLFileName, GDMLfile) ) {
      throw cet::exception("Geometry")
        << "cannot find the gdml geometry file:"
        << "\n" << GDMLFileName
        << "\nbail ungracefully.\n";
    }

    std::string ROOTfile;
    if( !sp.find_file(ROOTFileName, ROOTfile) ) {
      throw cet::exception("Geometry")
        << "cannot find the root geometry file:\n"
        << "\n" << ROOTFileName
        << "\nbail ungracefully.\n";
    }

    {
      fhicl::Table<geo::GeometryBuilderStandard::Config> const config{fBuilderParameters, {"tool_type"}};
      geo::GeometryBuilderStandard builder{config()};

      // initialize the geometry with the files we have found
      LoadGeometryFile(GDMLfile, ROOTfile, builder, bForceReload);
    }

    // now update the channel map
    InitializeChannelMap();

  } // Geometry::LoadNewGeometry()

  //......................................................................
  void Geometry::FillGeometryConfigurationInfo
    (fhicl::ParameterSet const& config)
  {

    sumdata::GeometryConfigurationInfo confInfo;
    confInfo.dataVersion = sumdata::GeometryConfigurationInfo::DataVersion_t{2};

    // version 1+:
    confInfo.detectorName = DetectorName();

    // version 2+:
    confInfo.geometryServiceConfiguration = config.to_indented_string();
    fConfInfo = std::move(confInfo);

    MF_LOG_TRACE("Geometry")
      << "Geometry configuration information:\n" << fConfInfo;

  } // Geometry::FillGeometryConfigurationInfo()

  //......................................................................
  bool Geometry::CheckConfigurationInfo
    (sumdata::GeometryConfigurationInfo const& other) const
  {

    MF_LOG_DEBUG("Geometry") << "New geometry information:\n" << other;

    return CompareConfigurationInfo(fConfInfo, other);

  } // Geometry::CheckConfigurationInfo()

  //......................................................................
  sumdata::GeometryConfigurationInfo const& Geometry::ReadConfigurationInfo
    (art::Run const& run)
  {

    try {
      return run.getProduct<sumdata::GeometryConfigurationInfo>
        (art::InputTag{"GeometryConfigurationWriter"});
    }
    catch (art::Exception const& e) {
      throw art::Exception{
        e.categoryCode(),
        "Can't read geometry configuration information.\n"
        "Is `GeometryConfigurationWriter` service configured?\n",
        e
        };
    }

  } // Geometry::ReadConfigurationInfo()


  //......................................................................
  bool Geometry::CompareConfigurationInfo(
    sumdata::GeometryConfigurationInfo const& A,
    sumdata::GeometryConfigurationInfo const& B
    )
  {
    /*
     * Implemented criteria:
     *
     * * both informations must be valid
     * * the detector names must exactly match
     *
     */

    if (!A.isDataValid()) {
      mf::LogWarning("Geometry") << "Geometry::CompareConfigurationInfo(): "
        "invalid version for configuration A:\n" << A;
      return false;
    }
    if (!B.isDataValid()) {
      mf::LogWarning("Geometry") << "Geometry::CompareConfigurationInfo(): "
        "invalid version for configuration B:\n" << B;
      return false;
    }

    // currently used only in debug mode (assert())
    [[maybe_unused]] auto const commonVersion = std::min(A.dataVersion, B.dataVersion);

    assert(commonVersion >= 1);

    if (A.detectorName != B.detectorName) { // case sensitive so far
      mf::LogWarning("Geometry") << "Geometry::CompareConfigurationInfo(): "
        "detector name mismatch: '" << A.detectorName << "' vs. '"
        << B.detectorName << "'";
      return false;
    }

    return true;
  } // CompareConfigurationInfo()


  //......................................................................
  DEFINE_ART_SERVICE(Geometry)
} // namespace geo
