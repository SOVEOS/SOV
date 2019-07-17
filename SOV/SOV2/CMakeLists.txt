add_contract(sovmintofeos sovmintofeos ${CMAKE_CURRENT_SOURCE_DIR}/src/sovmintofeos.cpp)

target_include_directories(sovmintofeos
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(sovmintofeos
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/ricardian/sovmintofeos.contracts.md.in ${CMAKE_CURRENT_BINARY_DIR}/ricardian/sovmintofeos.contracts.md @ONLY )

target_compile_options( sovmintofeos PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR}/ricardian -R${CMAKE_CURRENT_BINARY_DIR}/ricardian )
