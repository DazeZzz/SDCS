cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(SDCS
        VERSION 0.1
        DESCRIPTION "A simple distributed cache system for the distributed systems course at UESTC."
        LANGUAGES CXX
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include_directories("${PROJECT_SOURCE_DIR}/src/include" "${PROJECT_BINARY_DIR}")

add_subdirectory(src)

set(RELEASE_DIR "release/${CMAKE_PROJECT_VERSION}")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "The build type is release")
  set_target_properties(SDCS
  PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY ${RELEASE_DIR})
endif()

install(TARGETS SDCS DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "${SDCS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SDCS_VERSION_MINOR}")
set(CPACK_PACKAGE_DIRECTORY "${RELEASE_DIR}")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)