# ======================================================================
#
# Testing
#
# ======================================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")

cet_test(ServiceUtil_test
  LIBRARIES
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
    ${ART_UTILITIES}
    ${CANVAS}
    messagefacility::MFMessageLogger
    fhiclcpp::fhiclcpp fhiclcpp::types
    ${CETLIB_EXCEPT}
  USE_BOOST_UNIT
  )

