// larsoft libraries
#include "larcore/Geometry/StandardWireReadout.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/WireReadoutSorter.h"

// framework libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Utilities/make_tool.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace {
  auto default_wire_sorter()
  {
    fhicl::ParameterSet result;
    result.put("tool_type", std::string{"WireReadoutSorterStandard"});
    return result;
  }
}

namespace geo {
  StandardWireReadout::StandardWireReadout(fhicl::ParameterSet const& pset)
    : alg_{pset,
           art::ServiceHandle<Geometry>{}.get(),
           art::make_tool<WireReadoutSorter>(
             pset.get<fhicl::ParameterSet>("SortingParameters", default_wire_sorter()))}
  {
    mf::LogInfo("StandardWireReadout") << "Loading wire readout: WireReadoutStandardGeom";
  }

  WireReadoutGeom const& StandardWireReadout::wireReadoutGeom() const
  {
    return alg_;
  }
}
