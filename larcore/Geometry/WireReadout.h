#ifndef GEO_WireReadout_h
#define GEO_WireReadout_h

// LArSoft libraries
#include "larcorealg/Geometry/WireReadoutGeom.h"
#include "larcorealg/Geometry/fwd.h"

// framework libraries
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/fwd.h"

// C/C++ standard libraries
#include <memory> // std::unique_ptr<>
#include <string>

namespace geo {

  /**
   * @brief Interface to a service with a detector-specific, wire-readout geometry
   *
   * This is an interface to a service that virtualizes detector or experiment-specific
   * knowledge for wire-readout geometries. Experiments implement the private virtual
   * functions within a concrete service provider class to perform the specified actions
   * as appropriate for the particular experiment. It is expected that such requests will
   * occur infrequently within a job.
   *
   * @note The public interface for this service cannot be overriden.  The
   * experiment-specific sub-classes should implement only the private methods without
   * promoting their visibility.
   */
  class WireReadout {
  public:
    virtual ~WireReadout() = default;

    WireReadoutGeom const& Get() const { return wireReadoutGeom(); }

  private:
    virtual WireReadoutGeom const& wireReadoutGeom() const = 0;
  };

}

DECLARE_ART_SERVICE_INTERFACE(geo::WireReadout, SHARED)

#endif // GEO_WireReadout_h
