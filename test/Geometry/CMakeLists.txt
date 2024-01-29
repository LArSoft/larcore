cet_enable_asserts()

cet_test_env("FW_SEARCH_PATH=${larcorealg_BINARY_DIR}/gdml")
cet_transitive_paths(FHICL_DIR BINARY IN_TREE)
cet_test_env_prepend(FHICL_FILE_PATH "." ${TRANSITIVE_PATHS_WITH_FHICL_DIR})
cet_transitive_paths(LIBRARY_DIR BINARY IN_TREE)
cet_test_env_prepend(CET_PLUGIN_PATH ${TRANSITIVE_PATHS_WITH_LIBRARY_DIR})

add_subdirectory(VersionCheck)

# test modules
cet_build_plugin(GeometryIteratorLoopTest art::module
  LIBRARIES PRIVATE
  larcorealg::Geometry
  larcorealg::GeometryTestLib
  larcore_Geometry_Geometry_service
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  cetlib_except::cetlib_except
)

cet_build_plugin(GeometryTest art::module
  LIBRARIES PRIVATE
  larcorealg::Geometry
  larcorealg::GeometryTestLib
  larcore_Geometry_Geometry_service
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  cetlib_except::cetlib_except
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
