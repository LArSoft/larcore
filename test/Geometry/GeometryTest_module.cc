/** ****************************************************************************
 * @file   GeometryTest_module.cc
 * @brief  Runs geometry unit tests from a test algorithm
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 8th, 2015
 */

// LArSoft includes
#include "larcore/Geometry/AuxDetGeometry.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/WireReadout.h"
#include "larcorealg/test/Geometry/GeometryTestAlg.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/fwd.h"

// C/C++ standard library
#include <memory> // std::unique_ptr<>

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

  private:
    void analyze(art::Event const&) override {}
    void beginJob() override;

    GeometryTestAlg tester; ///< the test algorithm

  }; // class GeometryTest
} // namespace geo

//******************************************************************************
namespace geo {

  //......................................................................
  GeometryTest::GeometryTest(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
    , tester{art::ServiceHandle<Geometry const>{}.get(),
             &art::ServiceHandle<WireReadout const>{}->Get(),
             art::ServiceHandle<AuxDetGeometry const>{}->GetProviderPtr(),
             pset}
  {}

  //......................................................................
  void GeometryTest::beginJob() { tester.Run(); }

  //......................................................................
  DEFINE_ART_MODULE(GeometryTest)

} // namespace geo
