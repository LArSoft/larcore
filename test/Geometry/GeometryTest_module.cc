/** ****************************************************************************
 * @file   GeometryTest_module.cc
 * @brief  Runs geometry unit tests from a test algorithm
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 8th, 2015
 */

// LArSoft includes
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/test/Geometry/GeometryTestAlg.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

// C/C++ standard library
#include <memory> // std::unique_ptr<>

namespace art {
  class Event;
} // art::Event declaration
namespace fhicl {
  class ParameterSet;
}

namespace geo {
  /**
   * @brief Performs tests on the geometry as seen by Geometry service
   *
   * Configuration parameters
   * =========================
   *
   * See GeometryTestAlg.
   */
  class GeometryTest : public art::EDAnalyzer {
  public:
    explicit GeometryTest(fhicl::ParameterSet const& pset);

    virtual void analyze(art::Event const&) {}
    virtual void beginJob();

  private:
    std::unique_ptr<geo::GeometryTestAlg> tester; ///< the test algorithm

  }; // class GeometryTest
} // namespace geo

//******************************************************************************
namespace geo {

  //......................................................................
  GeometryTest::GeometryTest(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset), tester(new geo::GeometryTestAlg(pset))
  {} // GeometryTest::GeometryTest()

  //......................................................................
  void GeometryTest::beginJob()
  {
    art::ServiceHandle<geo::Geometry const> geom;

    // 1. we set it up with the geometry from the environment
    tester->Setup(*geom);

    // 2. then we run it!
    tester->Run();

  } // GeometryTest::beginJob()

  //......................................................................
  DEFINE_ART_MODULE(GeometryTest)

} // namespace geo
