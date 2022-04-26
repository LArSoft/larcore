////////////////////////////////////////////////////////////////////////////////
/// \file  AuxDetExptGeoHelperInterface.h
/// \brief Interface to a service that handles any experiment-specific knowledge
///        that is needed by the AuxDetGeometry service.
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


#ifndef GEO_AuxDetExptGeoHelperInterface_h
#define GEO_AuxDetExptGeoHelperInterface_h


// framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
namespace fhicl { class ParameterSet; }

// C/C++ standard libraries
#include <memory> // std::unique_ptr<>
#include <vector>

// LArSoft includes
#include "larcorealg/Geometry/AuxDetChannelMapAlg.h"

namespace geo
{

  /**
   * @brief Interface to a service with detector-specific geometry knowledge
   *
   * This is an interface to a service that virtualizes detector or
   * experiment-specific knowledge that is required by the Geometry service.
   * Experiments implement the private virtual functions within a concrete
   * service provider class to perform the specified actions as appropriate for
   * the particular experiment.
   * It is expected that such requests will occur infrequently within a job.
   * Calculations that occur frequently should be handled via interfaces that
   * are passed back to the Geometry service.
   *
   * @note The public interface for this service cannot be overriden.
   * The experiment-specific sub-classes should implement only the private
   * methods without promoting their visibility.
   */
  class AuxDetExptGeoHelperInterface
  {
  public:
    using AuxDetChannelMapAlgPtr_t = std::unique_ptr<AuxDetChannelMapAlg>;

    /// Virtual destructor; does nothing
    virtual ~AuxDetExptGeoHelperInterface() = default;

    /**
     * @brief Configure and initialize the channel map
     * @param sortingParameters parameters for the channel map algorithm
     * @param geom pointer to a geometry description object
     * @return a (shared) pointer to the channel mapping algorithm
     *
     * This method creates a new ChannelMapAlg according to the geometry and
     * specified configuration, then it configures the geometry itself
     * according to the channel map (usually, it resorts the data).
     */
    AuxDetChannelMapAlgPtr_t
    ConfigureAuxDetChannelMapAlg(fhicl::ParameterSet const & sortingParameters) const;

  private:

    /// Implementation of ConfigureChannelMapAlg (pure virtual)
    virtual AuxDetChannelMapAlgPtr_t
    doConfigureAuxDetChannelMapAlg(fhicl::ParameterSet const & sortingParameters) const = 0;

  }; // end ExptGeoHelperInterface class declaration



  //-------------------------------------------------------------------------------------------

  inline
  AuxDetExptGeoHelperInterface::AuxDetChannelMapAlgPtr_t
  AuxDetExptGeoHelperInterface::ConfigureAuxDetChannelMapAlg(fhicl::ParameterSet const& sortingParameters) const
  {
    return doConfigureAuxDetChannelMapAlg(sortingParameters);
  }

}

DECLARE_ART_SERVICE_INTERFACE(geo::AuxDetExptGeoHelperInterface, SHARED)

#endif // GEO_ExptGeoHelperInterface_h
