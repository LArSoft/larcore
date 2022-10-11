/**
 * @file   larcore/Geometry/ChannelMapSetupTool.h
 * @brief  Interface for a tool to configure a geometry channel mapper.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   October 7, 2019
 *
 * This library is header-only.
 */

#ifndef LARCORE_GEOMETRY_CHANNELMAPSETUPTOOL_H
#define LARCORE_GEOMETRY_CHANNELMAPSETUPTOOL_H

// LArSoft libraries
#include "larcorealg/Geometry/ChannelMapAlg.h"

namespace geo {

  /**
   * @brief Interface for a tool creating a channel mapping object.
   *
   * This class creates a `geo::ChannelMapAlg` instance.
   *
   */
  class ChannelMapSetupTool {
  public:
    virtual ~ChannelMapSetupTool() noexcept = default;

    /**
     * @brief Returns a new instance of the channel mapping.
     *
     * If the call fails, a null pointer is returned. This may happen on calls
     * following the first one, if the implementation does not support multiple
     * calls.
     * For all other errors, the implementations are expected to throw
     * the proper exception.
     */
    std::unique_ptr<geo::ChannelMapAlg> setupChannelMap() { return doChannelMap(); }

  protected:
    // --- BEGIN -- Virtual interface ------------------------------------------
    /// @name Virtual interface
    /// @{

    /// Returns a pointer to the channel mapping.
    virtual std::unique_ptr<geo::ChannelMapAlg> doChannelMap() = 0;

    /// @}
    // --- END -- Virtual interface --------------------------------------------

  }; // class ChannelMapSetupTool

} // namespace geo

#endif // LARCORE_GEOMETRY_CHANNELMAPSETUPTOOL_H
