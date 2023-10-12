cmake_minimum_required(VERSION 3.8)

set(TARGET BiplanesRevival)
set(${TARGET}_VERSION 1.0.5)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES C CXX)

add_executable(${TARGET})

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 14
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2       REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

target_sources(${TARGET} PRIVATE
  src/biplanes.cpp

  src/icon.rc
  src/version.rc

  include/structures.h
  include/variables.h

  src/bullet.cpp
  include/bullet.h

  src/cloud.cpp
  include/cloud.h

  src/controls.cpp
  include/controls.h

  src/init_vars.cpp
  include/init_vars.h

  src/matchmake.cpp
  include/matchmake.hpp

  src/menu.cpp
  include/menu.h

  src/plane.cpp
  include/plane.h

  src/render.cpp
  include/render.h

  src/sdl.cpp
  include/sdl.h

  src/utility.cpp
  include/utility.h

  src/zeppelin.cpp
  include/zeppelin.h
)


include(GNUInstallDirs)

target_include_directories(${TARGET} PRIVATE
  ${SDL2_INCLUDE_DIR}

  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lib>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_options(${TARGET} PRIVATE -w -O3 -s -Wno-narrowing)

target_link_libraries(${TARGET} PRIVATE
  SDL2::Main
  SDL2::Image
  SDL2::Mixer
)

if(WIN32)
  set_target_properties(${TARGET}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

  if(NOT ${BUILD_SHARED_LIBS})
    target_link_libraries(${TARGET}
      -static
      -static-libgcc
      -static-libstdc++
    )
  endif()

  target_link_libraries(${TARGET} wsock32 ws2_32)
  file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

elseif(APPLE)
  install(TARGETS ${TARGET} DESTINATION bin)
  file(COPY assets DESTINATION bin)

else()
  install(TARGETS ${TARGET} DESTINATION bin)
  file(COPY assets DESTINATION bin)
endif()
