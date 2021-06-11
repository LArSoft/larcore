# ======================================================================
#  larcore main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larcore/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required (VERSION 3.12...3.18 FATAL_ERROR)

project(larcore VERSION 9.24.02 LANGUAGES CXX)

set(gdml_install_dir gdml)

find_package(cetmodules REQUIRED)
find_package(canvas_root_io)

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
find_ups_product( art_root_io )
find_ups_root()
find_package( messagefacility REQUIRED)

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(larcore)

# tests
add_subdirectory(test)

# ups - table and config files
if(WANT_UPS)
add_subdirectory(ups)
endif()

# packaging utility
include(UseCPack)
 
