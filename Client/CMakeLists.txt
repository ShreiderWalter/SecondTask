add_executable (Client main.cpp)

set_property(TARGET Client PROPERTY FOLDER "executables")

install (TARGETS Client RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)