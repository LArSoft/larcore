# ======================================================================
#
# Testing
#
# ======================================================================

cet_test(ServiceUtil_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
  larcore::ServiceUtil
  art::Framework_Services_Registry
  canvas::canvas
)

