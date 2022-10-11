/**
 * @file   ServiceUtil.h
 * @brief  Utilities related to art service access
 * @author Jonathan Paley (jpaley@fnal.gov),
 *         Gianluca Petrillo (petrillo@fnal.gov)
 *
 * This library is currently a pure header.
 * It provides:
 *
 * - lar::providerFrom(), extracting and returning the provider from a single
 *   service
 * - lar::providersFrom(), extracting and returning providers from a set of
 *   services
 * - lar::providersFrom_t, a type defined as a provider pack with the providers
 *   from all the specified services
 *
 */

#ifndef LARCORE_COREUTILS_SERVICEUTIL_H
#define LARCORE_COREUTILS_SERVICEUTIL_H

// LArSoft libraries
#include "larcorealg/CoreUtils/ProviderPack.h"
#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"

// framework libraries
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

// C/C++ standard libraries
#include <type_traits> // std::decay<>, std::is_same<>, std::add_const_t<>
#include <typeinfo>

namespace lar {

  namespace details {
    template <typename PROVIDER>
    struct ServiceRequirementsChecker;

    template <typename... Services>
    struct ProviderPackExtractor;

  } // namespace details

  /** **************************************************************************
   * @brief Returns a constant pointer to the provider of specified service.
   * @tparam T type of the service
   * @return a constant pointer to the provider of specified service
   * @throws art::Exception (category `art::errors::NotFound`) if pointer is null
   *
   * This function relies on the following service and provider interface:
   * - provider is not movable nor copyable
   * - service contains a type `provider_type` defined as the class of the
   *   service provider
   * - service contains a method "provider()" that returns a non-null pointer
   *   to a service provider; the service provider is owned and managed by
   *   the service, and the caller is not responsible of regulating the object
   *   lifetime, nor it should attempt to
   *
   * Violations of the protocol yield compilation errors (in case non-compliance
   * can be statically detected), or throw of exceptions.
   *
   * Example of usage:
   *
   *     auto const* geom = lar::providerFrom<geo::Geometry>();
   *
   * retrieves the service provider for LArSoft geometry.
   * This requires the inclusion of "Geometry/Geometry.h" header, where the
   * service is declared. Typically, both ServiceUtil.h and the header of the
   * provider class are included in the service header.
   *
   */
  template <typename T>
  typename T::provider_type const* providerFrom()
  {
    using Service_t = std::add_const_t<T>;
    using Provider_t = typename Service_t::provider_type;

    (void)details::ServiceRequirementsChecker<Service_t>();

    // retrieve the provider
    art::ServiceHandle<Service_t> h;
    Provider_t const* const pProvider{h->provider()};
    if (!pProvider) {
      throw art::Exception(art::errors::NotFound)
        << "ServiceHandle <" << cet::demangle_symbol(typeid(Service_t).name())
        << "> offered a null provider";
    }

    return pProvider;

  } // providerFrom()

  /** **************************************************************************
   * @brief Returns a lar::ProviderPack with providers from all services
   * @tparam Services a list of service types
   * @return a lar::ProviderPack with providers from all specified services
   * @throws art::Exception as lar::providerFrom()
   * @see lar::providerFrom()
   *
   * This function relies on `lar::providerFrom()` to extract providers from
   * all the specified services.
   * The parameter pack stores the providers in the same order as the services
   * were specified, but this is not very relevant since provider packs can
   * be implicitly converted in other provider packs with the same providers
   * in a different order.
   *
   * Example of usage:
   *
   *     prov->setup
   *       (lar::providersFrom<geo::Geometry, detinfo::LArPropertiesService>());
   *
   * retrieves the service providers for LArSoft geometry and
   * `LArPropertiesService`, and passes them as a provider pack to a setup()
   * method, presumably from a algorithm or service provider that needs them.
   * This requires the inclusion of "Geometry/Geometry.h" and
   * "LArPropertiesService.h" headers, where the services are declared.
   * Typically, both ServiceUtil.h and the header of the provider class are
   * included in the service headers.
   */
  template <typename... Services>
  auto providersFrom()
  {
    return details::ProviderPackExtractor<Services...>::parameterPack();
  }

  /** **************************************************************************
   * @brief Type of a provider pack with a provider from each of the Services
   * @tparam Services the list of services to extract the provider type of
   *
   * Example of usage in a art service class declaration:
   *
   *     using needed_providers_t = lar::providersFrom_t
   *       <geo::Geometry, detinfo::LArPropertiesService>;
   *
   */
  template <typename... Services>
  using providersFrom_t = lar::ProviderPack<typename Services::provider_type...>;

  //----------------------------------------------------------------------------
  namespace details {
    /// Compiles only if PROVIDER class satisfied service provider requirements
    template <typename PROVIDER>
    struct ServiceProviderRequirementsChecker {

      using provider_type = PROVIDER;

      // static checks on provider class: not copiable nor movable
      static_assert(!std::is_copy_constructible<provider_type>::value,
                    "Service provider classes must not be copiable");
      static_assert(!std::is_copy_assignable<provider_type>::value,
                    "Service provider classes must not be copiable");
      static_assert(!std::is_move_constructible<provider_type>::value,
                    "Service provider classes must not be movable");
      static_assert(!std::is_move_assignable<provider_type>::value,
                    "Service provider classes must not be movable");

    }; // ServiceProviderRequirementsChecker

    template <typename SERVICE>
    struct ServiceRequirementsChecker {

      // require SERVICE::provider_type to be a type
      using provider_type = typename SERVICE::provider_type;

      // expected type for SERVICE::provider() method
      using provider_func_type = provider_type const* (SERVICE::*)() const;

      /// Checker for the provider
      ServiceProviderRequirementsChecker<provider_type> provider_checker;

      // check the provider() method
      static_assert(std::is_same<decltype(&SERVICE::provider), provider_func_type>::value,
                    "provider() method has unsupported signature");

    }; // ServiceRequirementsChecker

    //--------------------------------------------------------------------------
    template <typename First, typename... Others>
    struct ProviderPackExtractor<First, Others...> {
      static ProviderPack<typename First::provider_type, typename Others::provider_type...>
      parameterPack()
      {
        return {ProviderPackExtractor<Others...>::parameterPack(), lar::providerFrom<First>()};
      }
    };

    template <typename Service>
    struct ProviderPackExtractor<Service> {
      static auto parameterPack() { return lar::makeProviderPack(lar::providerFrom<Service>()); }
    };

  } // namespace details

} // namespace lar

#endif //#LARCORE_COREUTILS_SERVICEUTIL_H
