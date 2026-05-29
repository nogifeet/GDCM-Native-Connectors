cmake_minimum_required(VERSION 3.20)

project(gdcm_connector
  VERSION 0.1.0
  LANGUAGES CXX)

option(GDCM_CONNECTOR_BUILD_PYTHON "Build the CPython extension" ON)
option(GDCM_CONNECTOR_BUILD_JAVA "Build the JNI library" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(GDCM REQUIRED CONFIG)

if(DEFINED GDCM_USE_FILE AND EXISTS "${GDCM_USE_FILE}")
  include("${GDCM_USE_FILE}")
endif()

set(GDCM_CONNECTOR_GDCM_LIBS "${GDCM_LIBRARIES}" CACHE STRING
  "GDCM libraries to link. Override when a GDCM package does not set GDCM_LIBRARIES.")

if(NOT GDCM_CONNECTOR_GDCM_LIBS)
  set(GDCM_CONNECTOR_GDCM_LIBS gdcmMSFF)
endif()

add_library(gdcm_connector_core STATIC
  src/gdcm_connector.cpp)

target_include_directories(gdcm_connector_core
  PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    ${GDCM_INCLUDE_DIRS})

target_link_libraries(gdcm_connector_core
  PUBLIC
    ${GDCM_CONNECTOR_GDCM_LIBS})

set_target_properties(gdcm_connector_core PROPERTIES
  POSITION_INDEPENDENT_CODE ON)

if(GDCM_CONNECTOR_BUILD_PYTHON)
  find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

  Python3_add_library(gdcm_connector_python MODULE
    bindings/python/gdcm_connector_module.cpp)

  target_link_libraries(gdcm_connector_python
    PRIVATE
      gdcm_connector_core)

  target_include_directories(gdcm_connector_python
    PRIVATE
      "${PROJECT_SOURCE_DIR}/include")

  set_target_properties(gdcm_connector_python PROPERTIES
    OUTPUT_NAME "gdcm_connector"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python")
endif()

if(GDCM_CONNECTOR_BUILD_JAVA)
  find_package(Java REQUIRED)
  find_package(JNI REQUIRED)

  add_library(gdcm_connector_jni SHARED
    bindings/java/GdcmNative.cpp)

  target_include_directories(gdcm_connector_jni
    PRIVATE
      "${PROJECT_SOURCE_DIR}/include"
      ${JNI_INCLUDE_DIRS})

  target_link_libraries(gdcm_connector_jni
    PRIVATE
      gdcm_connector_core)

  set_target_properties(gdcm_connector_jni PROPERTIES
    OUTPUT_NAME "gdcm_connector_jni"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/java")
endif()
