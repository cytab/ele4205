cmake_minimum_required(VERSION 2.8)
project(odroid)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)

# Configurer le projet en mode débogage.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDebug")

# Sélectionner la compilation du client ou du serveur.
option(BUILD_CLIENT "Build client program" OFF)
option(BUILD_SERVER "Build server programs" OFF)

if (BUILD_CLIENT)
	message(STATUS "Building the client")
	add_executable(client src/client.cpp)
elseif (BUILD_SERVER)
	message(STATUS "Building the server")
	add_executable(server src/server.cpp)
	add_executable(musicPlayer src/musicPlayer.cpp)
else()
	message(FATAL_ERROR "
		To compile the client, run `cmake -DBUILD_CLIENT=ON ..`.
		To compile the server, run `cmake -DBUILD_SERVER=ON ..`.
		No target created.")
endif()

find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
	if (BUILD_CLIENT)
		include_directories(${OpenCV_INCLUDE_DIRS}
			/usr/include/tesseract
			/usr/include/leptonica)
		target_link_libraries(client ${OpenCV_LIBRARIES}
			tesseract lept)
	endif(BUILD_CLIENT)
	if (BUILD_SERVER)
		target_link_libraries(server ${OpenCV_LIBRARIES})
	endif (BUILD_SERVER)
else(OpenCV_FOUND)
	message(FATAL ERROR "Librarie OpenCV introuvable!")
endif(OpenCV_FOUND)

