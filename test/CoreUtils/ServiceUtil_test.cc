/**
 * @file   ServiceUtil_test.cc
 * @brief  Tests the utilities in ServiceUtil.h
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   April 28, 2016
 * @see    ServiceUtil.h
 *
 * This test takes no command line argument.
 *
 */

/*
 * Boost Magic: define the name of the module;
 * and do that before the inclusion of Boost unit test headers
 * because it will change what they provide.
 * Among the those, there is a main() function and some wrapping catching
 * unhandled exceptions and considering them test failures, and probably more.
 */
#define BOOST_TEST_MODULE (ServiceUtil_test)

// LArSoft libraries
#include "larcore/CoreUtils/ServiceUtil.h"
#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"

// art libraries
#include "art/Framework/Services/Registry/ServiceScope.h"
#include "canvas/Utilities/Exception.h"

// Boost libraries
#include <boost/test/unit_test.hpp>

//------------------------------------------------------------------------------
//
// here are some services: three of them, all different classes.
//
struct MyProvider : protected lar::UncopiableAndUnmovableClass {};

struct MyOtherProvider : protected lar::UncopiableAndUnmovableClass {};

struct YetAnotherProvider : protected lar::UncopiableAndUnmovableClass {};

template <typename Provider>
class MyServiceTemplate {
  Provider* prov = nullptr;

public:
  MyServiceTemplate(Provider* ptr = nullptr) : prov(ptr) {}

  using provider_type = Provider;

  provider_type const* provider() const { return prov; }

}; // MyServiceTemplate

using MyService = MyServiceTemplate<MyProvider>;
using MyOtherService = MyServiceTemplate<MyOtherProvider>;
using YetAnotherService = MyServiceTemplate<YetAnotherProvider>;

namespace lar {
  namespace details {

    template struct ServiceProviderRequirementsChecker<MyProvider>;

    template struct ServiceRequirementsChecker<MyService>;

  } // namespace details
} // namespace lar

//
// And this is art. Well, kind of.
//
// We are actually hijacking `art::ServiceHandle` for a few specific classes.
//
struct GlobalServicesClass {
  std::unique_ptr<MyService> myServicePtr;
  std::unique_ptr<MyOtherService> myOtherServicePtr;
  std::unique_ptr<YetAnotherService> yetAnotherServicePtr;
}; // GlobalServicesClass
GlobalServicesClass GlobalServices;

template <typename Service>
class ServiceHandleBase {
public:
  Service* operator->() const { return instance; }
  Service& operator*() const { return *instance; }

protected:
  Service* instance = nullptr;
}; // ServiceHandleBase<>

namespace art {

  namespace detail {
    template <>
    struct ServiceHelper<MyService> {
      static constexpr art::ServiceScope scope_val = art::ServiceScope::LEGACY;
    };
    template <>
    struct ServiceHelper<MyOtherService> {
      static constexpr art::ServiceScope scope_val = art::ServiceScope::LEGACY;
    };
    template <>
    struct ServiceHelper<YetAnotherService> {
      static constexpr art::ServiceScope scope_val = art::ServiceScope::LEGACY;
    };

  } // namespace detail

  template <>
  struct ServiceHandle<MyService, art::ServiceScope::LEGACY> : ::ServiceHandleBase<MyService> {
    ServiceHandle() { instance = GlobalServices.myServicePtr.get(); }
  };

  template <>
  struct ServiceHandle<MyService const, art::ServiceScope::LEGACY>
    : ServiceHandle<MyService, art::ServiceScope::LEGACY> {};

  template <>
  struct ServiceHandle<MyOtherService, art::ServiceScope::LEGACY>
    : ::ServiceHandleBase<MyOtherService> {
    ServiceHandle() { instance = GlobalServices.myOtherServicePtr.get(); }
  };

  template <>
  struct ServiceHandle<MyOtherService const, art::ServiceScope::LEGACY>
    : ServiceHandle<MyOtherService, art::ServiceScope::LEGACY> {};

  template <>
  struct ServiceHandle<YetAnotherService, art::ServiceScope::LEGACY>
    : ::ServiceHandleBase<YetAnotherService> {
    ServiceHandle() { instance = GlobalServices.yetAnotherServicePtr.get(); }
  };

  template <>
  struct ServiceHandle<YetAnotherService const, art::ServiceScope::LEGACY>
    : ServiceHandle<YetAnotherService, art::ServiceScope::LEGACY> {};

} // namespace art

BOOST_AUTO_TEST_CASE(providerFromTest)
{

  // on the first try, there is no service provider.

  GlobalServices.myServicePtr = std::make_unique<MyService>();
  BOOST_CHECK_EXCEPTION(
    lar::providerFrom<MyService>(), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::NotFound;
    });

  // now let's create a "real" provider
  MyProvider prov;
  GlobalServices.myServicePtr = std::make_unique<MyService>(&prov);
  BOOST_TEST(lar::providerFrom<MyService>() == &prov);

  // that's enough; let's clean up
  GlobalServices.myServicePtr.reset();

} // BOOST_AUTO_TEST_CASE(providerFromTest)

BOOST_AUTO_TEST_CASE(providersFromTest)
{

  // on the first try, there is no "other" service provider.

  MyProvider prov;
  GlobalServices.myServicePtr = std::make_unique<MyService>(&prov);
  GlobalServices.myOtherServicePtr = std::make_unique<MyOtherService>();
  GlobalServices.yetAnotherServicePtr = std::make_unique<YetAnotherService>();

  auto provPack1 = lar::providersFrom<MyService>();
  BOOST_TEST(provPack1.get<MyProvider>() == &prov);
  BOOST_CHECK_EXCEPTION(
    lar::providersFrom<MyOtherService>(), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::NotFound;
    });
  BOOST_CHECK_EXCEPTION(
    (lar::providersFrom<MyService, MyOtherService>()), art::Exception, [](art::Exception const& e) {
      return e.categoryCode() == art::errors::NotFound;
    });

  // now let's create a "real" provider
  MyOtherProvider oprov;
  YetAnotherProvider yaprov;
  GlobalServices.myOtherServicePtr = std::make_unique<MyOtherService>(&oprov);
  GlobalServices.yetAnotherServicePtr = std::make_unique<YetAnotherService>(&yaprov);

  auto provPack = lar::providersFrom<MyService, MyOtherService, YetAnotherService>();

  BOOST_TEST((provPack == lar::makeProviderPack(&prov, &oprov, &yaprov)));
  BOOST_TEST(lar::providerFrom<MyService>() == &prov);
  BOOST_TEST(lar::providerFrom<MyOtherService>() == &oprov);
  BOOST_TEST(lar::providerFrom<YetAnotherService>() == &yaprov);

  // that's enough; let's clean up
  GlobalServices.myServicePtr.reset();

} // BOOST_AUTO_TEST_CASE(providersFromTest)

//------------------------------------------------------------------------------
