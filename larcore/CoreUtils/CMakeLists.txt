cet_make_library(LIBRARY_NAME ServiceUtil INTERFACE
  SOURCE ServiceUtil.h
  LIBRARIES INTERFACE
  larcorealg::headers
  art::Framework_Services_Registry
  canvas::canvas
  cetlib_except::cetlib_except
)

cet_make_library(LIBRARY_NAME ServiceProviderWrappers INTERFACE
  SOURCE ServiceProviderWrappers.h
  LIBRARIES INTERFACE
  larcore::ServiceUtil
  art::Framework_Services_Registry
)

cet_make_library(LIBRARY_NAME EnsureOnlyOneSchedule INTERFACE
  SOURCE EnsureOnlyOneSchedule.h
  LIBRARIES INTERFACE
  art::Utilities
  canvas::canvas
  cetlib_except::cetlib_except
)

install_headers()
install_source()
