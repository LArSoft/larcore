/**
 * @file   larcore/Geometry/WireReadoutSetupTool.h
 * @brief  Interface for a tool to configure a geometry channel mapper.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   October 7, 2019
 *
 * This library is header-only.
 */

#ifndef LARCORE_GEOMETRY_WIREREADOUTSETUPTOOL_H
#define LARCORE_GEOMETRY_WIREREADOUTSETUPTOOL_H

// LArSoft libraries
#include "larcorealg/Geometry/WireReadoutGeom.h"
#include "larcorealg/Geometry/fwd.h"

namespace geo {

  /**
   * @brief Interface for a tool creating a channel mapping object.
   *
   * This class creates a `geo::WireReadoutGeom` instance.
   *
   */
  class WireReadoutSetupTool {
  public:
    virtual ~WireReadoutSetupTool() noexcept = default;

    /**
     * @brief Returns a new instance of the channel mapping.
     *
     * If the call fails, a null pointer is returned. This may happen on calls
     * following the first one, if the implementation does not support multiple
     * calls.
     * For all other errors, the implementations are expected to throw
     * the proper exception.
     */
    std::unique_ptr<geo::WireReadoutGeom> setupWireReadout(
      geo::GeometryCore const* geom,
      std::unique_ptr<WireReadoutSorter> sorter)
    {
      return doWireReadout(geom, std::move(sorter));
    }

  protected:
    // --- BEGIN -- Virtual interface ------------------------------------------
    /// @name Virtual interface
    /// @{

    /// Returns a pointer to the channel mapping.
    virtual std::unique_ptr<geo::WireReadoutGeom> doWireReadout(
      geo::GeometryCore const* geom,
      std::unique_ptr<WireReadoutSorter> sorter) = 0;

    /// @}
    // --- END -- Virtual interface --------------------------------------------

  }; // class WireReadoutSetupTool

} // namespace geo

#endif // LARCORE_GEOMETRY_WIREREADOUTSETUPTOOL_H
