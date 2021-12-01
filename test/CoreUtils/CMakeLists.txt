# ======================================================================
#
# Testing
#
# ======================================================================

cet_test(ServiceUtil_test
  LIBRARIES
    art::Framework_Services_Registry
    art::Utilities
    canvas::canvas
    messagefacility::MF_MessageLogger
    fhiclcpp::fhiclcpp
    cetlib_except::cetlib_except
  USE_BOOST_UNIT
  )

