cet_enable_asserts()

# test modules
simple_plugin ( GeometryInfoCheck "module"
  larcore_Geometry_Geometry_service
  larcorealg_Geometry
  )

simple_plugin ( LegacyGeometryInfoWriter "module"
  larcoreobj_SummaryData
  )

# ------------------------------------------------------------------------------

# no input file (source: EmptyEvent)
# expected: geometry information added to output file
#   (check is performed by an analyzer at begin run)

cet_test(GeometryInfoCheckEmptyInput HANDBUILT
  TEST_EXEC lar
  TEST_ARGS
    --rethrow-all --config ./test_geometry_check_empty.fcl
    --output data_withGeometryA.root
  DATAFILES test_geometries.fcl test_geometry_checks.fcl test_geometry_check_empty.fcl
)


# no input file, with a job including Geometry service
# but not the new GeometryConfigurationWriter service;
# expected: `ProductNotFound` exception

cet_test(GeometryInfoCheckNoWriter HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./test_geometry_check_nogeoconfwriter.fcl
  DATAFILES
    test_geometries.fcl test_geometry_checks.fcl
    test_geometry_check_nogeoconfwriter.fcl
  TEST_PROPERTIES
    WILL_FAIL TRUE
)


# no input file, with a job including Geometry service
# but not the new GeometryConfigurationWriter service,
# and Geometry service is required to skip the check;
# expected: still `ProductNotFound` exception

cet_test(GeometryInfoCheckNoWriterSkipCheck HANDBUILT
  TEST_EXEC lar
  TEST_ARGS --rethrow-all --config ./test_geometry_check_nogeoconfwriter_skipcheck.fcl
  DATAFILES
    test_geometries.fcl test_geometry_checks.fcl
    test_geometry_check_nogeoconfwriter.fcl
    test_geometry_check_nogeoconfwriter_skipcheck.fcl
  TEST_PROPERTIES
    WILL_FAIL TRUE
)


# input file with sumdata::RunData data product compatible with current Geometry
# expected: geometry information added to output file

cet_test(GeometryInfoCheckMakeLegacyInput HANDBUILT
  TEST_EXEC lar
  TEST_ARGS
    --rethrow-all
    --config ./test_geometry_check_make_legacy_input.fcl
    --output data_withGeometryA_legacy.root
  DATAFILES
    test_geometries.fcl
    test_geometry_check_make_legacy_input.fcl
)

cet_test(GeometryInfoCheckLegacyCompatible HANDBUILT
  TEST_EXEC lar
  TEST_ARGS
    --rethrow-all
    --config ./test_geometry_check_require_geometryA.fcl
    --source ../GeometryInfoCheckMakeLegacyInput.d/data_withGeometryA_legacy.root
  DATAFILES
    test_geometries.fcl test_geometry_checks.fcl
    test_geometry_check_require_geometryA.fcl
  TEST_PROPERTIES
    DEPENDS GeometryInfoCheckMakeLegacyInput
)


# input file with sumdata::RunData data product not compatible with current Geometry
# exception: exception showing the different configurations

cet_test(GeometryInfoCheckLegacyIncompatible HANDBUILT
  TEST_EXEC lar
  TEST_ARGS
    --rethrow-all
    --config ./test_geometry_check_require_geometryB.fcl
    --source ../GeometryInfoCheckMakeLegacyInput.d/data_withGeometryA_legacy.root
  DATAFILES
    test_geometries.fcl test_geometry_checks.fcl
    test_geometry_check_require_geometryB.fcl
  TEST_PROPERTIES
    DEPENDS GeometryInfoCheckMakeLegacyInput
    WILL_FAIL TRUE
)


# input file with sumdata::GeometryConfigurationInfo data product
# compatible with current Geometry
# expected: data product declared but not put, all seems ok

cet_test(GeometryInfoCheckCompatible HANDBUILT
  TEST_EXEC lar
  TEST_ARGS
    --rethrow-all
    --config ./test_geometry_check_require_geometryA.fcl
    --source ../GeometryInfoCheckEmptyInput.d/data_withGeometryA.root
    --output data_withGeometryA_OnceMore.root
  DATAFILES
    test_geometries.fcl test_geometry_checks.fcl
    test_geometry_check_require_geometryA.fcl
  TEST_PROPERTIES
    DEPENDS GeometryInfoCheckEmptyInput
)


# input file with sumdata::GeometryConfigurationInfo data product
# not compatible with current Geometry
# expected: exception showing the different configurations

cet_test(GeometryInfoCheckIncompatible HANDBUILT
  TEST_EXEC lar
  TEST_ARGS
    --rethrow-all
    --config ./test_geometry_check_require_geometryB.fcl
    --source ../GeometryInfoCheckEmptyInput.d/data_withGeometryA.root
  DATAFILES
    test_geometries.fcl test_geometry_checks.fcl
    test_geometry_check_require_geometryB.fcl
  TEST_PROPERTIES
    DEPENDS GeometryInfoCheckEmptyInput
    WILL_FAIL TRUE
)


# input file with sumdata::GeometryConfigurationInfo data product
# not compatible with current Geometry; Geometry configuration skips the check
# expected: pass (with warning)

cet_test(GeometryInfoCheckIncompatibleSkipCheck HANDBUILT
  TEST_EXEC lar
  TEST_ARGS
    --rethrow-all
    --config ./test_geometry_check_geometryB_skipCheck.fcl
    --source ../GeometryInfoCheckEmptyInput.d/data_withGeometryA.root
  DATAFILES
    test_geometries.fcl test_geometry_checks.fcl
    test_geometry_check_geometryB_skipCheck.fcl
  TEST_PROPERTIES
    DEPENDS GeometryInfoCheckEmptyInput
)


# input file with sumdata::GeometryConfigurationInfo data product
#   (one present, one only promised)
# compatible with current Geometry
# expected: success (and again data product declared but not put)

cet_test(GeometryInfoCheckCompatible2 HANDBUILT
  TEST_EXEC lar
  TEST_ARGS
    --rethrow-all
    --config ./test_geometry_check_require_geometryA.fcl
    --process-name MoreProc
    --source ../GeometryInfoCheckCompatible.d/data_withGeometryA_OnceMore.root
  DATAFILES
    test_geometries.fcl test_geometry_checks.fcl
    test_geometry_check_require_geometryA.fcl
  TEST_PROPERTIES
    DEPENDS GeometryInfoCheckCompatible
)


# ------------------------------------------------------------------------------
