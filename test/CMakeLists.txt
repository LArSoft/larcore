include(CetTest)
cet_enable_asserts()
cet_test_env(FHICL_FILE_PATH=$ENV{FHICL_FILE_PATH}:.)
find_package(art_root_io REQUIRED)
add_subdirectory(Geometry)
add_subdirectory(CoreUtils)

