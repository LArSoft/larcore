/**
 * @file   GeometryIteratorLoopTest_module.cc
 * @brief  Tests the correct iteration of the geo::Geometry iterators
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   August 25, 2014
 */

// LArSoft includes
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/test/Geometry/GeometryIteratorLoopTestAlg.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/fwd.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/fwd.h"

namespace geo {
  /**
   * @brief Performs tests on the geometry as seen by Geometry service
   *
   * Configuration parameters
   * =========================
   *
   * See GeometryIteratorLoopTestAlg.
   */
  class GeometryIteratorLoopTest : public art::EDAnalyzer {
  public:
    explicit GeometryIteratorLoopTest(fhicl::ParameterSet const& pset)
      : EDAnalyzer{pset}, tester{art::ServiceHandle<geo::Geometry const>{}.get()}
    {}

  private:
    void analyze(art::Event const&) override {}
    void beginJob() override { tester.Run(); }

    geo::GeometryIteratorLoopTestAlg tester; ///< the test algorithm

  }; // class GeometryIteratorLoopTest
}

DEFINE_ART_MODULE(geo::GeometryIteratorLoopTest)
