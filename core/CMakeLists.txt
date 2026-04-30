add_subdirectory(file)
add_subdirectory(network)
add_subdirectory(object)
add_subdirectory(physical)
add_subdirectory(render)

add_executable(${PROJECT_NAME} render/src/viewtest.cpp)