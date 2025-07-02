cmake_minimum_required(VERSION 3.15)

set(TARGET BiplanesRevival)
set(${TARGET}_VERSION 1.2.1)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES C CXX)

add_executable(${TARGET})

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(${TARGET}_ENABLE_STEP_DEBUGGING
  "${TARGET}: Enable step-by-step mode controls for easier debugging" OFF)

if (WIN32)
  option(${TARGET}_DISABLE_CONSOLE "${TARGET}: Don't show console window" ON)
endif()

include(GNUInstallDirs)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 17
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

  find_package(SDL2 2.0.10 REQUIRED)
  find_package(SDL2_image REQUIRED)
  find_package(SDL2_mixer REQUIRED)
endif()

target_sources(${TARGET} PRIVATE
  src/biplanes.cpp

  include/fwd.hpp
  include/enums.hpp
  include/constants.hpp

  include/game_state.hpp
  include/network_data.hpp
  include/network_state.hpp
  include/canvas.hpp
  include/color.hpp
  include/stats.hpp
  include/sounds.hpp
  include/textures.hpp
  include/variables.hpp

  src/icon.rc
  src/version.rc


  src/bullet.cpp
  include/bullet.hpp

  src/cloud.cpp
  include/cloud.hpp

  src/controls.cpp
  include/controls.hpp

  src/effects.cpp
  include/effects.hpp

  src/math.cpp
  include/math.hpp

  src/menu.cpp
  src/menu_input.cpp
  src/menu_navigation.cpp
  src/menu_sp.cpp
  src/menu_mp.cpp
  src/menu_help.cpp
  src/menu_stats.cpp
  include/menu.hpp

  src/plane.cpp
  src/plane_input.cpp
  src/plane_pilot.cpp
  include/plane.hpp

  src/render.cpp
  include/render.hpp

  src/resources.cpp
  include/resources.hpp

  src/sdl.cpp
  include/sdl.hpp

  src/time.cpp
  include/time.hpp

  src/timer.cpp
  include/timer.hpp

  src/utility.cpp
  include/utility.hpp

  src/zeppelin.cpp
  include/zeppelin.hpp

  src/ai_stuff.cpp
  include/ai_stuff.hpp
)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  target_sources(${TARGET} PRIVATE
    lib/Net.h

    src/matchmake.cpp
    include/matchmake.hpp

    src/network.cpp
    include/network.hpp
  )
endif()

if (${BUILD_SHARED_LIBS})
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${TARGET} PROPERTIES
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
  endif()
else()
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${TARGET} PROPERTIES
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
  else()
    target_link_libraries(${TARGET} PUBLIC
      -static
      -static-libgcc
      -static-libstdc++
    )
  endif()
endif()

add_subdirectory(deps/TimeUtils)


target_compile_definitions(${TARGET} PRIVATE
  BIPLANES_EXE_NAME="${TARGET}"
  BIPLANES_VERSION="${${TARGET}_VERSION}"
  _USE_MATH_DEFINES
)

if (${${TARGET}_ENABLE_STEP_DEBUGGING})
  target_compile_definitions(${TARGET} PRIVATE
    BIPLANES_STEP_DEBUGGING_ENABLED
  )
endif()


target_include_directories(${TARGET} PRIVATE
  ${SDL2_INCLUDE_DIR}

  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${TARGET} PUBLIC -Wno-narrowing)
endif()


if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  target_compile_options(${TARGET} PUBLIC
    --use-port=sdl2
    --use-port=sdl2_mixer
    --use-port=sdl2_image:formats=png
  )

  target_link_options(${TARGET} PUBLIC
    --use-port=sdl2
    --use-port=sdl2_mixer
    --use-port=sdl2_image:formats=png
    --embed-file ${CMAKE_INSTALL_BINDIR}/assets@/assets
  )
else()
  target_link_libraries(${TARGET} PUBLIC
    SDL2::Main
    SDL2::Image
    SDL2::Mixer
  )
endif()

target_link_libraries(${TARGET} PUBLIC
  TimeUtils::TimeUtils
)


if (WIN32)
  target_link_libraries(${TARGET} PUBLIC
    ws2_32
  )

  if (${${TARGET}_DISABLE_CONSOLE})
    set_target_properties(${TARGET} PROPERTIES
      WIN32_EXECUTABLE TRUE
    )

    target_link_libraries(${TARGET} PUBLIC
      -mwindows
    )
  else()
    set_target_properties(${TARGET} PROPERTIES
      WIN32_EXECUTABLE FALSE
    )

    target_link_libraries(${TARGET} PUBLIC
      -mconsole
    )
  endif()

endif()


file(
  COPY assets
  DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  file(
    COPY wasm-data/index.html
    DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
  )
endif()

install(TARGETS ${TARGET}
  EXPORT ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

