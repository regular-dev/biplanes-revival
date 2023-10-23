####
# Set minimum version of CMake. Since command 'project' use
# VERSION sub-option we need at least 3.0.
# Note: If you use 2.6 or 2.4, God kills a kitten. Seriously.
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

####
# Set variables:
#   * PROJECT_NAME
#   * PROJECT_VERSION
project(tiny_dnn VERSION 1.0.0 LANGUAGES C CXX)

#####
# Enables project() command manages VERSION variables.
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif(POLICY CMP0048)

#####
# Change the default build type from Debug to Release, while still
# supporting overriding the build type.
#
# The CACHE STRING logic here and elsewhere is needed to force CMake
# to pay attention to the value of these variables.
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified; defaulting to CMAKE_BUILD_TYPE=Release.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
else(NOT CMAKE_BUILD_TYPE)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("==========================================================================================")
        message(STATUS "Build type: Debug. Performance will be terrible!")
        message(STATUS "Add -DCMAKE_BUILD_TYPE=Release to the CMake command line to get an optimized build.")
        message("==========================================================================================")
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
endif(NOT CMAKE_BUILD_TYPE)

####
# Define user options
option(USE_SSE        "Build tiny-dnn with SSE library support"            ON)
option(USE_AVX        "Build tiny-dnn with AVX library support"            ON)
option(USE_AVX2       "Build tiny-dnn with AVX2 library support"           OFF)

option(USE_SERIALIZER "Build tiny-dnn with Serialization support"          ON)
option(USE_DOUBLE     "Build tiny-dnn with double precision computations"  OFF)
option(USE_IMAGE_API  "Build tiny-dnn with Image API support"              ON)

option(PROFILE          "Set to ON to build with profiling"     OFF)
option(USE_ASAN         "Set to ON to build with ASan"          OFF)

if (WIN32)
  set(USE_PTHREAD OFF)
else()
  set(USE_PTHREAD ON)
endif()

####
# Create the library target

set(project_library_target_name ${PROJECT_NAME})
set(PACKAGE_NAME TinyDNN)

add_library(${project_library_target_name} INTERFACE)

####
# Setup the optional dependencies

# Using cmake scripts and modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

if(NOT USE_SERIALIZER)
    target_compile_definitions(${project_library_target_name} INTERFACE
      CNN_NO_SERIALIZATION
    )
endif()

if(USE_DOUBLE)
    target_compile_definitions(${project_library_target_name} INTERFACE
      CNN_USE_DOUBLE
    )
endif()

if(USE_IMAGE_API)
    target_compile_definitions(${project_library_target_name} INTERFACE
      DNN_USE_IMAGE_API
    )
endif()

####
# Setup the compiler options

# set c++ standard to c++14.
# Note: not working on CMake 2.8. We assume that user has
#       a compiler with C++14 support.

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++14 support has been enabled by default.")

include(cotire)

# Unix
if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR
   CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-msse3" COMPILER_HAS_SSE_FLAG)
    check_cxx_compiler_flag("-mavx"  COMPILER_HAS_AVX_FLAG)
    check_cxx_compiler_flag("-mavx2" COMPILER_HAS_AVX2_FLAG)
    check_cxx_compiler_flag("-mfma" COMPILER_HAS_AVX2_FLAG)

    # set Streaming SIMD Extension (SSE) instructions
    if(USE_SSE AND COMPILER_HAS_SSE_FLAG)
        target_compile_definitions(${project_library_target_name} INTERFACE
          CNN_USE_SSE
        )
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -msse3")
    endif(USE_SSE AND COMPILER_HAS_SSE_FLAG)
    # set Advanced Vector Extensions (AVX)
    if(USE_AVX AND COMPILER_HAS_AVX_FLAG)
        target_compile_definitions(${project_library_target_name} INTERFACE
          CNN_USE_AVX
        )
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -mavx")
    endif(USE_AVX AND COMPILER_HAS_AVX_FLAG)
    # set Advanced Vector Extensions 2 (AVX2)
    if(USE_AVX2 AND COMPILER_HAS_AVX2_FLAG)
        target_compile_definitions(${project_library_target_name} INTERFACE
          CNN_USE_AVX2
        )
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -mavx2 -mfma -march=core-avx2")
    endif(USE_AVX2 AND COMPILER_HAS_AVX2_FLAG)

    # include extra flags to the compiler
    # TODO: add info about those flags.
    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wall -Wpedantic -Wno-narrowing -Wno-deprecated")
    set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE} -O3")
    set(EXTRA_C_FLAGS_DEBUG   "${EXTRA_C_FLAGS_DEBUG} -g3 -pthread")
elseif(MSVC)
    if(USE_SSE)
        target_compile_definitions(${project_library_target_name} INTERFACE
          CNN_USE_SSE
        )
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /arch:SSE2")
    endif(USE_SSE)
    if(USE_AVX)
        target_compile_definitions(${project_library_target_name} INTERFACE
          CNN_USE_AVX
        )
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /arch:AVX")
    endif(USE_AVX)
    if(USE_AVX2)
        target_compile_definitions(${project_library_target_name} INTERFACE
          CNN_USE_AVX2
        )
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /arch:AVX2")
    endif(USE_AVX2)
    # include specific flags for release and debug modes.
    set(EXTRA_C_FLAGS_RELEASE "${EXTRA_C_FLAGS_RELEASE}
        /Ox /Oi /Ot /Oy /GL /fp:fast /GS-")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    set(EXTRA_C_FLAGS_DEBUG "${EXTRA_C_FLAGS_DEBUG}")
    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /W4 /bigobj")
    # this is fine
    target_compile_definitions(${project_library_target_name} INTERFACE
       _CRT_SECURE_NO_WARNINGS
       _SCL_SECURE_NO_WARNINGS
    )
    # prolly powerless with header-only project
    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} /MP")
endif()

####
# Set compiler options
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${EXTRA_C_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${EXTRA_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_C_FLAGS_DEBUG}")

if(PROFILE AND (CMAKE_COMPILER_IS_GNUCXX OR MINGW OR
    CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -pg")
endif()

if (USE_ASAN)
    #enable ASan
    set(ENV{ASAN_OPTIONS} "strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:symbolize=1")
    CHECK_CXX_COMPILER_FLAG(-fsanitize-address-use-after-scope HasUseAfterScope)
    set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
    if (HasUseAfterScope)
        set(ASAN_FLAGS "${ASAN_FLAGS} -fsanitize-address-use-after-scope")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
endif()
####
# Write the config.h
# TODO: replace for tiny_dnn/config.h
# configure_file(cmake/Templates/tinydnn_config.h.in
#               "${PROJECT_BINARY_DIR}/tinydnn_config.h")

####
# Setup the cmake config files
string(REGEX REPLACE "_" "" PROJECT_NAME_JOINED ${PROJECT_NAME})

set(cmake_conf_file         "${PROJECT_NAME_JOINED}-config.cmake")
set(cmake_conf_version_file "${PROJECT_NAME_JOINED}-config-version.cmake")
set(cmake_targets_file      "${PROJECT_NAME_JOINED}-targets.cmake")

set(targets_export_name "${PROJECT_NAME_JOINED}-targets")
set(namespace "${PACKAGE_NAME}::")

# Set up install directories. INCLUDE_INSTALL_DIR and
# CMAKECONFIG_INSTALL_DIR must not be absolute paths.
if(WIN32)
    set(include_install_dir Include)
    set(include_install_dir_full Include)
    set(config_install_dir CMake)
elseif(UNIX)
    set(include_install_dir include)
    set(include_install_dir_postfix "${project_library_target_name}")
    set(include_install_dir_full    "${include_install_dir}/${include_install_dir_postfix}")

    set(config_install_dir share/${PACKAGE_NAME})
else()
    message(FATAL_ERROR "Not supported system type. Options: UNIX or WIN32.")
endif()

# configure the library target
target_include_directories(
    ${project_library_target_name} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${include_install_dir_full}>)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Templates/cmake-uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake-uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake-uninstall.cmake)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Templates/${cmake_conf_file}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_file}"
    PATH_VARS include_install_dir_full
    INSTALL_DESTINATION ${config_install_dir})

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_version_file}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${EXTRA_C_FLAGS}")
# Create *-targets.cmake file for build directory
install(TARGETS ${project_library_target_name}
        EXPORT  ${targets_export_name}
        INCLUDES DESTINATION ${include_install_dir})

export(EXPORT ${targets_export_name}
       FILE   ${CMAKE_CURRENT_BINARY_DIR}/${cmake_targets_file})

# Install *-targets.cmake file
install(EXPORT      ${targets_export_name}
        NAMESPACE   ${namespace}
        DESTINATION ${config_install_dir})

# Install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_file}"
    "${CMAKE_CURRENT_BINARY_DIR}/${cmake_conf_version_file}"
    DESTINATION ${config_install_dir} COMPONENT cmake)

# Install headers
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/${project_library_target_name}
        DESTINATION ${include_install_dir})

####
# Configuration summary
include(cmake/summary.cmake)
tinydnn_print_configuration_summary()

