cmake_minimum_required(VERSION 2.8)
project(capture)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)

link_directories(include/)
add_executable(${PROJECT_NAME} src/capture.cpp src/main.cpp)

find_package(OpenCV REQUIRED )
if(OpenCV_FOUND)
	include_directories(${OpenCV_INCLUDE_DIRS})
	target_link_libraries(capture ${OpenCV_LIBRARIES})
else(OpenCV_FOUND)
	message(FATAL ERROR "Librarie OpenCV introuvable!")
endif(OpenCV_FOUND)
