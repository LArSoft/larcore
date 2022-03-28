
install_gdml( SUBDIRS GDMLSchema )

# install gdml executables
# NOTE: gdml_install_dir is defined in local scope after the call to install_gdml
file(GLOB gdml_bin *.pl genmake )
install( PROGRAMS ${gdml_bin} DESTINATION ${${CETMODULES_CURRENT_PROJECT_NAME}_GDML_DIR}$ )

