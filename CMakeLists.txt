
#LinuxでなんでDドライブ出てくるわけ?
set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "" FORCE)
set(VCPKG_HOST_TRIPLET "x64-linux" CACHE STRING "" FORCE)
unset(VCPKG_CHAINLOAD_TOOLCHAIN_FILE CACHE)


cmake_minimum_required(VERSION 3.13)
project(QuortzMediaEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(core)
add_subdirectory(editor)