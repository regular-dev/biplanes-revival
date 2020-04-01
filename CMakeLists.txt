cmake_minimum_required(VERSION 3.7)

project(BiPlanes)

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

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${GAME_ICON})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE -w -O3 -s)

target_link_libraries(BiPlanes
			SDL2::Main
			SDL2::Image
			SDL2::Mixer)
			
if(WIN32)
	set_target_properties( ${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32 -static-libgcc -static-libstdc++)
	file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)
else()
	install(TARGETS BiPlanes DESTINATION bin)
	file(COPY assets DESTINATION bin)
endif()
