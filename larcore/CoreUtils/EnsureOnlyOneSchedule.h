#ifndef larcore_CoreUtils_EnsureOnlyOneSchedule_h
#define larcore_CoreUtils_EnsureOnlyOneSchedule_h

#include "art/Utilities/Globals.h"
#include "canvas/Utilities/Exception.h"
#include "cetlib_except/demangle.h"

/**
 * @file   EnsureOnlyOneSchedule.h
 * @brief  Type whose constructor throws if more than one art schedule is configured
 *
 * This class is intended to be used in the context of an art job.  It
 * is helpful for services that have the notion of a "current event,"
 * but are thread-safe within that event.  The constructor of this
 * class will throw an exception if more than one art schedule is
 * configured.  It should be used via private inheritance:
 *
 *   class MyService : lar::EnsureOnlyOneSchedule<MyService> {
 *     ..
 *   };
 *
 */

namespace lar {
  template <typename T>
  class EnsureOnlyOneSchedule {
  public:
    EnsureOnlyOneSchedule()
    {
      if (auto const nschedules = art::Globals::instance()->nschedules(); nschedules > 1) {
        throw art::Exception{art::errors::Configuration}
          << "This job uses " << nschedules << " schedules, but the type '"
          << cet::demangle_symbol(typeid(T).name()) << " supports\n"
          << "processing only one event at a time. Please reconfigure your job to use only one "
             "schedule.\n";
      }
    }
  };
}

#endif /* larcore_CoreUtils_EnsureOnlyOneSchedule_h */

// Local Variables:
// mode: c++
// End:
