cmake_minimum_required(VERSION 3.7)

project(BiPlanes)
project(PicoJson)

set (CMAKE_CXX_STANDARD 11)

find_package(SDL2 REQUIRED)


set (BIPLANES_SRCS
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
	src/tcpsock.cpp)

include_directories(. ../ include/ 
					${SDL2_INCLUDE_DIRS} ${CURL_INCLUDE_DIR})

add_executable(BiPlanes ${BIPLANES_SRCS})

# install sdl2lib-image via apt install

target_link_libraries(BiPlanes 
			${SDL2_LIBRARIES} 
			SDL2_image
			SDL2_mixer)
