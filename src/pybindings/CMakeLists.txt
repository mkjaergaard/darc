find_package(PythonLibs)
find_package(Boost COMPONENTS python thread)

include_directories(${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_library(darc-pybindings SHARED EXCLUDE_FROM_ALL darc.cpp)
target_link_libraries(darc-pybindings ${Boost_LIBRARIES} darc)
set_target_properties(darc-pybindings
  PROPERTIES
  OUTPUT_NAME darc
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  )

add_custom_target(pybindings)
add_dependencies(pybindings darc-pybindings)
