# ======================================================================
#
# Testing
#
# ======================================================================

cet_test(ServiceUtil_test
  LIBRARIES
    art::Framework_Services_Registry
    ${ART_UTILITIES}
    ${CANVAS}
    messagefacility::MF_MessageLogger
    fhiclcpp::fhiclcpp
    cetlib_except::cetlib_except
  USE_BOOST_UNIT
  )

