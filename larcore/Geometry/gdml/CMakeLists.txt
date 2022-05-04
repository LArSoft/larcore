install_gdml()

# install gdml executables
# NOTE: project variable GDML_DIR is defined after the install_gdml() call above.
file(GLOB gdml_bin *.pl genmake )
install( PROGRAMS ${gdml_bin} DESTINATION ${${CETMODULES_CURRENT_PROJECT_NAME}_GDML_DIR} )

