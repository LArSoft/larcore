# ======================================================================
#
# Testing
#
# ======================================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")

cet_test(ServiceUtil_test
  LIBRARIES
    art::Framework_Services_Registry
    art::Utilities
    ${CANVAS}
    messagefacility::MF_MessageLogger
    fhiclcpp::fhiclcpp fhiclcpp::types
    ${CETLIB_EXCEPT}
  USE_BOOST_UNIT
  )

