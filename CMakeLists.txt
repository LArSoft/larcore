# ======================================================================
#  larcore main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larcore/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(cetmodules 3.12.00 REQUIRED)
project(larcore VERSION 09.07.01 LANGUAGES CXX)

# cetbuildtools contains our cmake modules

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

cet_cmake_module_directories(Modules BINARY)

find_package(larcoreobj REQUIRED EXPORT)
find_package(larcorealg REQUIRED EXPORT)

find_package(art REQUIRED EXPORT)
find_package(canvas REQUIRED EXPORT)
find_package(messagefacility REQUIRED EXPORT)
find_package(fhiclcpp REQUIRED EXPORT)
find_package(cetlib REQUIRED EXPORT)
find_package(cetlib_except REQUIRED EXPORT)

# macros for artdaq_dictionary and simple_plugin
include(BuildPlugins)

# source
add_subdirectory(larcore)

# tests
add_subdirectory(test)

# ups - table and config files

# packaging utility
cet_cmake_config()
