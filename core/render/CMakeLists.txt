#vulkanなど読みこみ
add_library(engine_renderer STATIC)


#link
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
include_directories(include)

#file(GLOB RECURSE HEADERS "include/*.h")
file(GLOB RECURSE RENDER_SOURCES "src/*.cpp")

target_sources(engine_renderer PRIVATE ${RENDER_SOURCES})
target_include_directories(engine_renderer PUBLIC include)
target_include_directories(engine_renderer PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(engine_renderer PUBLIC 
    glfw
    glm::glm 
    asio::asio
    Vulkan::Vulkan
)

echo("debug")

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE engine_renderer)