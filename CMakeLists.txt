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

  include/fwd.hpp
  include/enums.hpp
  include/platform.hpp

  include/game_state.hpp
  include/network_data.hpp
  include/network_state.hpp
  include/sizes.hpp
  include/stats.hpp
  include/sounds.hpp
  include/textures.hpp
  include/variables.h

  lib/Net.h

  src/icon.rc
  src/version.rc


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
  src/menu_input.cpp
  src/menu_navigation.cpp
  src/menu_sp.cpp
  src/menu_mp.cpp
  src/menu_help.cpp
  src/menu_stats.cpp
  include/menu.h

  src/network.cpp
  include/network.hpp

  src/plane.cpp
  src/plane_input.cpp
  src/plane_pilot.cpp
  include/plane.h

  src/render.cpp
  include/render.h

  src/sdl.cpp
  include/sdl.h

  src/time.cpp
  include/time.hpp

  src/timer.cpp
  include/timer.hpp

  src/utility.cpp
  include/utility.h

  src/zeppelin.cpp
  include/zeppelin.h
)

target_compile_definitions(${TARGET} PRIVATE
  BIPLANES_EXE_NAME="${TARGET}"
  BIPLANES_VERSION="${${TARGET}_VERSION}"
)

include(GNUInstallDirs)

target_include_directories(${TARGET} PRIVATE
  ${SDL2_INCLUDE_DIR}

  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_options(${TARGET} PUBLIC
  -Wno-narrowing
)

if (NOT ${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PRIVATE
    -static
    -static-libgcc
    -static-libstdc++
  )
endif()

target_link_libraries(${TARGET} PRIVATE
  SDL2::Main
  SDL2::Image
  SDL2::Mixer
)

if (WIN32)
  set_target_properties(${TARGET}
    PROPERTIES
    WIN32_EXECUTABLE FALSE
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
  )

  target_link_libraries(${TARGET} PUBLIC
    ws2_32
    -static-libgcc
    -static-libstdc++
    -mconsole
  )
endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

install(TARGETS ${TARGET}
  EXPORT ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
