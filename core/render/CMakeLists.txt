#ファイル取得
file(GLOB_RECURSE RENDER_SOURCES "src/*.cpp")
message(STATUS "found sources: ${RENDER_SOURCES}")
#vulkanなど読みこみ
add_library(engine_renderer STATIC)


#link
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
include_directories(include)

#file(GLOB RECURSE HEADERS "include/*.h")

target_sources(engine_renderer PRIVATE ${RENDER_SOURCES})
target_include_directories(engine_renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(engine_renderer PUBLIC ${Vulkan_INCLUDE_DIRS})

#lib link
target_link_libraries(engine_renderer PUBLIC 
    glfw
    glm::glm 
    asio::asio
    Vulkan::Vulkan
)

#add_executable("${PROJECT_NAME}_render" src/viewtest.cpp)
#target_link_libraries(${PROJECT_NAME} PRIVATE engine_renderer)