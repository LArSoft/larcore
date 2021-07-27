/**
 * @file    ServiceProviderWrappers.h
 * @brief   Skeleton for a art service interface to service providers
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    April 19, 2016
 *
 * This is a pure template library.
 * The callers will need to link to:
 *
 * * `${ART_FRAMEWORK_SERVICES_REGISTRY}`
 *
 * It provides:
 *
 * * SimpleServiceProviderWrapper: wrap a service with a single implementation
 * * ServiceProviderImplementationWrapper: wrap a concrete implementation of a
 *   service provider interface supporting multiple implementations
 *
 */

#ifndef LARCORE_COREUTILS_SERVICEPROVIDERWRAPPERS_H
#define LARCORE_COREUTILS_SERVICEPROVIDERWRAPPERS_H 1


// LArSoft libraries
#include "larcore/CoreUtils/ServiceUtil.h" // lar::providerFrom() (for includers)

// framework and support libraries
#include "art/Framework/Services/Registry/ServiceTable.h"

// C/C++ standard libraries
#include <memory> // std::unique_ptr<>


// forward declarations
namespace art { class ActivityRegistry; }
namespace fhicl { class ParameterSet; }


namespace lar {

   /** **********************************************************************
    * @brief Service returning a provider
    * @tparam PROVIDER type of service provider to be returned
    *
    * This class provides the simplest possible art service to wrap a LArSoft
    * service provider.
    * The service is not reacting to any framework events.
    *
    * The configuration is passed directly to the provider.
    *
    * The simplest way to create an art service with this template is:
    *
    *     #include "larcore/CoreUtils/ServiceProviderWrappers.h"
    *     #include "path/to/MyProvider.h"
    *
    *
    *     namespace myprov {
    *
    *       using MyService = lar::SimpleServiceProviderWrapper<MyProvider>;
    *
    *     }
    *
    *     DECLARE_ART_SERVICE(myprov::MyService, LEGACY)
    *
    * An implementation file is necessary too, that will look like:
    *
    *     #include "path/to/MyService.h"
    *
    *     DEFINE_ART_SERVICE(myprov::MyService)
    *
    * If callback registration is needed, a class can derive from this template
    * and still gain a bit of boilerplate facility. That is:
    * - a `provider_type` definition (required by `lar::providerFrom()`)
    * - a `provider()` method to access the provider (required by
    *   `lar::providerFrom()`)
    * - a `Parameters` definition (used by art to print accepted configuration)
    * - a constructor supporting FHiCL configuration validation
    * - transparent life management of the provider instance
    *
    * Requirements on the service provider:
    * - a data type `Config` being the configuration object. This is the object
    *   wrapped by `fhicl::Table` when performing FHiCL validation.
    * - a constructor with as argument a constant reference to a Config object.
    *   The provider should be configured by that constructor.
    *
    */
   template <class PROVIDER>
   class SimpleServiceProviderWrapper {

         public:
      using provider_type = PROVIDER; ///< type of the service provider

      /// Type of configuration parameter (for art description)
      using Parameters = art::ServiceTable<typename provider_type::Config>;


      /// Constructor (using a configuration table)
      SimpleServiceProviderWrapper
         (Parameters const& config, art::ActivityRegistry&)
         : prov(std::make_unique<provider_type>(config()))
         {}


      /// Returns a constant pointer to the service provider
      provider_type const* provider() const { return prov.get(); }


         private:

      std::unique_ptr<provider_type> prov; ///< service provider

   }; // SimpleServiceProviderWrapper<>



   /** **********************************************************************
    * @brief Service returning a provider interface
    * @tparam PROVIDER type of service provider interface to be returned
    * @see ServiceProviderImplementationWrapper
    *
    * This class provides a art service to wrap a LArSoft service provider
    * interface.
    *
    * The behaviour of the implementations is not constrained by this interface.
    * It only adds support for `lar::providerFrom()` function.
    *
    * Implementations must override `do_provider()` to return a pointer which
    * `provider()` will forward to the caller.
    *
    */
   template <class PROVIDER>
   class ServiceProviderInterfaceWrapper {

         public:
      using provider_type = PROVIDER; ///< type of the service provider

      /// Virtual destructor
      virtual ~ServiceProviderInterfaceWrapper() = default;


      /// Returns a constant pointer to the service provider interface
      provider_type const* provider() const { return do_provider(); }


         protected:

      /// Implementation of the provider() function, to be overridden
      virtual provider_type const* do_provider() const = 0;

   }; // ServiceProviderInterfaceWrapper<>



   /** *************************************************************************
    * @brief Service implementation returning a provider
    * @tparam INTERFACE type of art service being implemented
    * @tparam PROVIDER type of service provider to be returned
    * @see ServiceProviderInterfaceWrapper
    *
    * This class is suitable for a simple art service implementing an interface.
    * This is the case of a service that can have multiple implementations,
    * for example an experiment-specific access to a database.
    * In this case, the art service is described by an interface, and many
    * services can inherit and implement it.
    *
    * In the factorisation model recommended in LArSoft, this hierarchy of art
    * service implementations derived by a common interface is mirrored in a
    * matching hierarchy of service provider implementation derived by a common
    * service provider interface.
    * In this model, services are basically trivial classes that return a
    * pointer to the provider through a prescribed interface. The only relevant
    * exception is that services may also register framework callbacks.
    *
    * Given the nature of the service returning a provider, its structure is
    * repetitive. This class implements most of that repetitive frame.
    * The resulting service is not reacting to any framework events.
    *
    * The configuration of the service is passed directly to the provider.
    *
    * Given that we want to have a service that implements the service
    * `MyServiceInterface` and returns a provider implementation "A" called
    * `MyProviderA`, the simplest way to create it with this template is:
    *
    *     #include "larcore/CoreUtils/ServiceProviderWrappers.h"
    *     #include "path/to/MyProviderA.h"
    *     #include "path/to/MyServiceInterface.h"
    *
    *
    *     namespace myprov {
    *
    *       using MyServiceA = lar::ServiceProviderImplementationWrapper
    *         <MyProviderA, MyServiceInterface>;
    *
    *     }
    *
    *     DECLARE_ART_SERVICE_INTERFACE_IMPL
    *       (myprov::MyServiceA, myprov::MyService, LEGACY)
    *
    *
    * The class `MyServiceInterface` is expected to have been defined via
    * `ServiceProviderInterfaceWrapper`.
    * An implementation file is necessary too, that will look like:
    *
    *     #include "path/to/MyServiceA.h"
    *
    *     DEFINE_ART_SERVICE_INTERFACE_IMPL
    *       (myprov::MyServiceA, myprov::MyService)
    *
    * If callback registration is needed, a class can derive from this template
    * and still gain a bit of boilerplate facility. That is:
    *
    * * a `provider_type` definition (required by `lar::providerFrom()`)
    * * a `provider()` method to access the provider (required by
    *   `lar::providerFrom()`)
    * - a `Parameters` definition (used by art to print accepted configuration)
    * - a constructor supporting FHiCL configuration validation
    * - transparent life management of the provider instance
    * * a `concrete_provider_type` definition (that is PROVIDER)
    * * a `service_interface_type` definition (that is INTERFACE)
    *
    * Requirements on the service provider (PROVIDER):
    * - a data type `Config` being the configuration object. This is the object
    *   wrapped by `fhicl::Table` when performing FHiCL validation.
    * - a constructor with as argument a constant reference to a Config object.
    *   The provider should be configured by that constructor.
    *
    * Requirements on the service interface (INTERFACE):
    * - a data type `provider_type` representing the abstract interface of the
    *   service provider
    * - a virtual method `do_provider()` where the retrieval of the provider
    *   is expected to happen; this method is overridden
    *
    */
   template <typename PROVIDER, typename INTERFACE>
   class ServiceProviderImplementationWrapper: public INTERFACE {

         public:
      /// type of service provider implementation
      using concrete_provider_type = PROVIDER;

      /// art service interface class
      using service_interface_type = INTERFACE;

      /// type of service provider interface
      using provider_type = typename service_interface_type::provider_type;

      /// Type of configuration parameter (for art description)
      using Parameters
        = art::ServiceTable<typename concrete_provider_type::Config>;


      /// Constructor (using a configuration table)
      ServiceProviderImplementationWrapper
         (Parameters const& config, art::ActivityRegistry&)
         : prov(std::make_unique<concrete_provider_type>(config()))
         {}


         private:
      std::unique_ptr<concrete_provider_type> prov; ///< service provider

      /// Returns a constant pointer to the service provider
      virtual provider_type const* do_provider() const override
         { return prov.get(); }

   }; // ServiceProviderImplementationWrapper


} // namespace lar


#endif // LARCORE_COREUTILS_SERVICEPROVIDERWRAPPERS_H
