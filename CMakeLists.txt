cmake_minimum_required(VERSION 3.7)

set(TARGET Biplanes)
project(${TARGET})

set (CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2       REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

set (PROJECT_SOURCES
  src/biplanes.cpp
  src/matchmake.cpp
  src/controls.cpp
  src/menu.cpp
  src/plane.cpp
  src/bullet.cpp
  src/cloud.cpp
  src/zeppelin.cpp
  src/sdl.cpp
  src/render.cpp
  src/utility.cpp
  src/init_vars.cpp
)

set (GAME_ICON src/icon.rc)

add_executable(${TARGET} ${PROJECT_SOURCES} ${GAME_ICON} src/version.rc)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET} PRIVATE ${SDL2_INCLUDE_DIR})

target_compile_options(${TARGET} PRIVATE -w -O3 -s -Wno-narrowing)

target_link_libraries(${TARGET}
  SDL2::Main
  SDL2::Image
  SDL2::Mixer
)

if(WIN32)
  set_target_properties( ${TARGET}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )
  target_link_libraries(${TARGET} -static -static-libgcc -static-libstdc++ wsock32 ws2_32)
  file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)
elseif(APPLE)
  install(TARGETS ${TARGET} DESTINATION bin)
  file(COPY assets DESTINATION bin)
else()
  install(TARGETS ${TARGET} DESTINATION bin)
  file(COPY assets DESTINATION bin)
endif()
