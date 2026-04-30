#ファイル取得
file(GLOB_RECURSE FILESYS_SOURCES "src/*.cpp")
message(STATUS "found sources: ${FILESYS_SOURCES}")
#vulkanなど読みこみ
add_library(engine_filesys STATIC)


#link

include_directories(include)

#file(GLOB RECURSE HEADERS "include/*.h")

target_sources(engine_filesys PRIVATE ${FILESYS_SOURCES})
target_include_directories(engine_filesys PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(engine_filesys PUBLIC ${Vulkan_INCLUDE_DIRS})

#lib link
target_link_libraries(engine_filesys PUBLIC 
)

#add_executable("${PROJECT_NAME}_FileSystem" src/asset_path.cpp)
#target_link_libraries(${PROJECT_NAME} PRIVATE engine_renderer)