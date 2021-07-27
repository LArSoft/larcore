////////////////////////////////////////////////////////////////////////////////
/// \file  ExptGeoHelperInterface.h
/// \brief Interface to a service that handles any experiment-specific knowledge
///        that is needed by the Geometry service.
///
///  This is an interface to a service that virtualizes detector or experiment-specific
///  knowledge that is required by the Geometry service. Experiments implement the
///  private virtual functions within a concrete service provider class to perform
///  the specified actions as appropriate for the particular experiment. It is
///  expected that such requests will occur infrequently within a job. Calculations
///  that occur frequently should be handled via interfaces that are passed
///  back to the Geometry service.
///
///  Note that the public interface for this service cannot be overriden. The
///  experiment-specific sub-classes should implement only the private methods
///  without promoting their visibility.
///
/// \author  rs@fnal.gov
////////////////////////////////////////////////////////////////////////////////


#ifndef GEO_ExptGeoHelperInterface_h
#define GEO_ExptGeoHelperInterface_h


// framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "fhiclcpp/ParameterSet.h"

// C/C++ standard libraries
#include <memory> // std::shared_ptr<>
#include <string>
#include <vector>

// prototypes of geometry classes
namespace geo
{
  class ChannelMapAlg;
}

namespace geo
{

  /**
   * @brief Interface to a service with detector-specific geometry knowledge
   *
   * This is an interface to a service that virtualizes detector or
   * experiment-specific knowledge that is required by the Geometry service.
   * Experiments implement the private virtual function within a concrete
   * service provider class to perform the specified actions as appropriate for
   * the particular experiment.
   *
   * It is expected that such requests will occur infrequently within a job.
   * Calculations that occur frequently should be handled via interfaces that
   * are passed back to the Geometry service.
   *
   * @note The public interface for this service cannot be overriden.
   * The experiment-specific sub-classes should implement only the private
   * methods without promoting their visibility.
   */
  class ExptGeoHelperInterface
  {
  public:
    using ChannelMapAlgPtr_t = std::unique_ptr<ChannelMapAlg>;

    /// Virtual destructor; does nothing
    virtual ~ExptGeoHelperInterface() = default;

    /**
     * @brief Configure and initialize the channel map
     * @param sortingParameters parameters for the channel map algorithm
     * @param detectorName name of detector described by geometry
     * @return a (shared) pointer to the channel mapping algorithm
     *
     * This method creates a new ChannelMapAlg according to the geometry and
     * specified configuration.
     */
    ChannelMapAlgPtr_t
    ConfigureChannelMapAlg(fhicl::ParameterSet const& sortingParameters,
                           std::string const& detectorName) const
    {
      return doConfigureChannelMapAlg(sortingParameters, detectorName);
    }

  private:

    virtual
    ChannelMapAlgPtr_t
    doConfigureChannelMapAlg(fhicl::ParameterSet const& sortingParameters,
                             std::string const& detectorName) const = 0;

  }; // end ExptGeoHelperInterface class declaration

}

DECLARE_ART_SERVICE_INTERFACE(geo::ExptGeoHelperInterface, SHARED)

#endif // GEO_ExptGeoHelperInterface_h
