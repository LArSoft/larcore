cet_enable_asserts()

add_subdirectory(VersionCheck)

# test modules
simple_plugin ( GeometryIteratorLoopTest "module"
                    larcorealg_Geometry
                    larcorealg::GeometryTestLib
                    larcore_Geometry_Geometry_service
                    messagefacility::MF_MessageLogger
                    
                    fhiclcpp::fhiclcpp
                    cetlib::cetlib cetlib_except
             )

simple_plugin ( GeometryTest "module"
                    larcorealg_Geometry
                    larcorealg::GeometryTestLib
                    larcore_Geometry_Geometry_service
                    messagefacility::MF_MessageLogger
                    
                    fhiclcpp::fhiclcpp
                    cetlib::cetlib cetlib_except
                    ${ROOT_BASIC_LIB_LIST}
              )

# ------------------------------------------------------------------------------
# geometry test on "standard" geometry

# This test is equivalent to geometry_test, but run in art environment
cet_test(geometry HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config test_geometry.fcl
)

# This test is equivalent to geometry_iterator_loop_test, but run in art environment
cet_test(geometry_iterator_loop HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config test_geometry_iterator_loop.fcl
)

# this test just dumps the geometry on a file
cet_test(dump_geometry_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config dump_lartpcdetector_geometry.fcl
)


# FCL files need to be copied to the test area (DATAFILES directive) since they
# are not installed.
cet_test(dump_channel_map_test HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./dump_lartpcdetector_channelmap.fcl
  DATAFILES dump_lartpcdetector_channelmap.fcl
)

# ------------------------------------------------------------------------------
install_headers()
install_fhicl()
install_source()

