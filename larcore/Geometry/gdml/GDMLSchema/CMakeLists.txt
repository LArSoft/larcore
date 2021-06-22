# install schema files
file(GLOB gdml_schema *.xsd)
install( FILES ${gdml_schema} DESTINATION ${${CMAKE_PROJECT_NAME}_gdml_dir}/GDMLSchema )
