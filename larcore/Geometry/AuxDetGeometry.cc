// class header
#include "larcore/Geometry/AuxDetGeometry.h"

// LArSoft includes
#include "larcorealg/Geometry/AuxDetGeoObjectSorter.h"

// Framework includes
#include "art/Utilities/make_tool.h"
#include "fhiclcpp/ParameterSet.h"

// C/C++ standard libraries
#include <memory>

namespace {
  std::unique_ptr<geo::AuxDetGeoObjectSorter> sorter(fhicl::ParameterSet const& pset)
  {
    if (pset.is_empty()) { return nullptr; }
    return art::make_tool<geo::AuxDetGeoObjectSorter>(pset);
  }

  std::unique_ptr<geo::AuxDetInitializer> readout_initializer(fhicl::ParameterSet const& pset)
  {
    if (pset.is_empty()) { return nullptr; }
    return art::make_tool<geo::AuxDetInitializer>(pset);
  }
}

//......................................................................................
geo::AuxDetGeometry::AuxDetGeometry(fhicl::ParameterSet const& pset)
  : fAuxDetGeom{pset,
                sorter(pset.get<fhicl::ParameterSet>("SortingParameters", {})),
                readout_initializer(pset.get<fhicl::ParameterSet>("ReadoutInitializer", {}))}
{}
