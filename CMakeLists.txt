# ======================================================================
#  larcore main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larcore/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

project(larcore 09.24.02 LANGUAGES CXX)

find_package(cetmodules REQUIRED)
find_package(canvas_root_io)

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larcoreobj )
find_ups_product( larcorealg )
find_ups_product( art )
find_ups_product(art_root_io)
find_ups_product( messagefacility )
find_ups_root()

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(larcore)

# tests
add_subdirectory(test)
add_subdirectory(cmakeConfig)
# ups - table and config files
#add_subdirectory(ups)

# packaging utility
#include(UseCPack)
