add_executable (Server main.cpp)

set_property(TARGET Server PROPERTY FOLDER "executables")

install (TARGETS Server RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)