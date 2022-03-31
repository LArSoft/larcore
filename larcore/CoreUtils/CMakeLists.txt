cet_make_library(HEADERS_TARGET_ONLY)
cet_make_library(LIBRARY_NAME ServiceUtil INTERFACE
  SOURCE ServiceUtil.h
  LIBRARIES INTERFACE
  larcorealg::headers
  art::Framework_Services_Registry
  canvas::canvas
  cetlib_except::cetlib_except
  )

install_headers()
install_source()
