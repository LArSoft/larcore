/**
 * @file   GeometryIteratorLoopTest_module.cc
 * @brief  Tests the correct iteration of the geo::Geometry iterators
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   August 25, 2014
 */

// LArSoft includes
#include "larcorealg/test/Geometry/GeometryIteratorLoopTestAlg.h"
#include "larcore/Geometry/Geometry.h"

// Framework includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"

// C/C++ standard library
#include <memory> // std::unique_ptr<>


namespace art { class Event; } // art::Event declaration
namespace fhicl { class ParameterSet; }

namespace geo {
  /**
   * @brief Performs tests on the geometry as seen by Geometry service
   *
   * Configuration parameters
   * =========================
   *
   * See GeometryIteratorLoopTestAlg.
   */
  class GeometryIteratorLoopTest: public art::EDAnalyzer {
      public:
    explicit GeometryIteratorLoopTest(fhicl::ParameterSet const& pset);

    virtual void analyze(art::Event const&) {}
    virtual void beginJob();

      private:

    std::unique_ptr<geo::GeometryIteratorLoopTestAlg> tester; ///< the test algorithm

  }; // class GeometryIteratorLoopTest
} // namespace geo


//******************************************************************************
namespace geo {

  //......................................................................
  GeometryIteratorLoopTest::GeometryIteratorLoopTest(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
    , tester(new geo::GeometryIteratorLoopTestAlg(pset))
  {
  } // GeometryIteratorLoopTest::GeometryIteratorLoopTest()


  //......................................................................
  void GeometryIteratorLoopTest::beginJob()
  {
    art::ServiceHandle<geo::Geometry const> geom;

    // 1. we set it up with the geometry from the environment
    tester->Setup(*geom);

    // 2. then we run it!
    tester->Run();

  } // GeometryTest::beginJob()


  //......................................................................
  DEFINE_ART_MODULE(GeometryIteratorLoopTest)

} // namespace geo
