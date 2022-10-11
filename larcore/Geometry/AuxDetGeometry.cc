/**
 * @file   Geometry_service.cc
 * @brief  art framework interface to geometry description - implementation file
 * @author brebel@fnal.gov
 * @see    Geometry.h
 */

// class header
#include "larcore/Geometry/AuxDetGeometry.h"
#include "larcore/Geometry/AuxDetExptGeoHelperInterface.h"

// lar includes
#include "larcoreobj/SummaryData/RunData.h"

// Framework includes
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "cetlib/search_path.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <string>

namespace geo {

  //......................................................................
  // Constructor.
  AuxDetGeometry::AuxDetGeometry(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg)
    : fProvider(pset)
    , fRelPath(pset.get<std::string>("RelativePath", ""))
    , fForceUseFCLOnly(pset.get<bool>("ForceUseFCLOnly", false))
    , fSortingParameters(pset.get<fhicl::ParameterSet>("SortingParameters", {}))
  {
    // add a final directory separator ("/") to fRelPath if not already there
    if (!fRelPath.empty() && (fRelPath.back() != '/')) fRelPath += '/';

    // register a callback to be executed when a new run starts
    reg.sPreBeginRun.watch(this, &AuxDetGeometry::preBeginRun);

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

  } // Geometry::Geometry()

  void AuxDetGeometry::preBeginRun(art::Run const& run)
  {
    // FIXME this seems utterly wrong: constructor loads geometry based on an
    // explicit parameter, whereas here we load it by detector name

    // if we are requested to stick to the configured geometry, do nothing
    if (fForceUseFCLOnly) return;

    // check here to see if we need to load a new geometry.
    // get the detector id from the run object
    //std::vector< art::Handle<sumdata::RunData> > rdcol;
    //run.getManyByType(rdcol);
    auto rdcol = run.getMany<sumdata::RunData>();
    if (rdcol.empty()) {
      mf::LogWarning("LoadNewGeometry")
        << "cannot find sumdata::RunData object to grab detector name\n"
        << "this is expected if generating MC files\n"
        << "using default geometry from configuration file\n";
      return;
    }

    // if the detector name is still the same, everything is fine
    std::string newDetectorName = rdcol.front()->DetName();
    if (GetProvider().DetectorName() == newDetectorName) return;

    // else {
    //   // the detector name is specified in the RunData object
    //   SetDetectorName(newDetectorName);
    // }

    LoadNewGeometry(GetProvider().DetectorName() + ".gdml", GetProvider().DetectorName() + ".gdml");
  } // Geometry::preBeginRun()

  //......................................................................
  void AuxDetGeometry::InitializeChannelMap()
  {
    // the channel map is responsible of calling the channel map configuration
    // of the geometry
    auto channelMap =
      art::ServiceHandle<geo::AuxDetExptGeoHelperInterface>()->ConfigureAuxDetChannelMapAlg(
        fSortingParameters);
    if (!channelMap) {
      throw cet::exception("ChannelMapLoadFail") << " failed to load new channel map";
    }
    fProvider.ApplyChannelMap(move(channelMap));
  } // Geometry::InitializeChannelMap()

  //......................................................................
  void AuxDetGeometry::LoadNewGeometry(std::string gdmlfile, std::string /* rootfile */)
  {
    // start with the relative path
    std::string GDMLFileName(fRelPath), ROOTFileName(fRelPath);

    // add the base file names
    ROOTFileName.append(gdmlfile); // not rootfile (why?)
    GDMLFileName.append(gdmlfile);

    // Search all reasonable locations for the GDML file that contains
    // the detector geometry.
    // cet::search_path constructor decides if initialized value is a path
    // or an environment variable
    cet::search_path sp("FW_SEARCH_PATH");

    std::string GDMLfile;
    if (!sp.find_file(GDMLFileName, GDMLfile)) {
      throw cet::exception("AuxDetGeometry") << "cannot find the gdml geometry file:"
                                             << "\n"
                                             << GDMLFileName << "\nbail ungracefully.\n";
    }

    std::string ROOTfile;
    if (!sp.find_file(ROOTFileName, ROOTfile)) {
      throw cet::exception("AuxDetGeometry") << "cannot find the root geometry file:\n"
                                             << "\n"
                                             << ROOTFileName << "\nbail ungracefully.\n";
    }

    // initialize the geometry with the files we have found
    GetProvider().LoadGeometryFile(GDMLfile, ROOTfile);

    // now update the channel map
    InitializeChannelMap();

  } // Geometry::LoadNewGeometry()

} // namespace geo
