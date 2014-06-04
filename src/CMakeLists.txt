cmake_minimum_required(VERSION 2.8)
project( phototwix )
SET(COMPILE_DEFINITIONS -Werror -Wall -c -ansi)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(OpenCV REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
include_directories(/opt/vc/include)
include_directories(/opt/vc/userland-master/interface/vcos)
include_directories(/opt/vc/userland-master)
include_directories(/opt/vc/userland-master/interface/vcos/pthreads)
include_directories(/opt/vc/userland-master/interface/vmcs_host/linux)
include_directories(/opt/vc/userland-master/host_applications/linux/libs/bcm_host/include)
link_directories(/opt/vc/lib)
add_executable(phototwix main.cpp 
						util.cpp 
						main_phototwix.cpp 
						screen.cpp 
						screen_camera.cpp
						graphic_util.cpp
						template.cpp
						./lib/camera/camera.cpp
						./lib/camera/cameracontrol.cpp
						)

configure_file(		${CMAKE_CURRENT_SOURCE_DIR}/img/back-black.jpg
					${CMAKE_CURRENT_BINARY_DIR}/img/back-black.jpg COPYONLY)
					
configure_file(		${CMAKE_CURRENT_SOURCE_DIR}/shaders/simplefragshader.glsl
					${CMAKE_CURRENT_BINARY_DIR}/shaders/simplefragshader.glsl COPYONLY)
configure_file(		${CMAKE_CURRENT_SOURCE_DIR}/shaders/simplevertshader.glsl
					${CMAKE_CURRENT_BINARY_DIR}/shaders/simplevertshader.glsl COPYONLY)
configure_file(		${CMAKE_CURRENT_SOURCE_DIR}/shaders/yuvfragshader.glsl
					${CMAKE_CURRENT_BINARY_DIR}/shaders/yuvfragshader.glsl COPYONLY)
configure_file(		${CMAKE_CURRENT_SOURCE_DIR}/shaders/sepiashader.glsl
					${CMAKE_CURRENT_BINARY_DIR}/shaders/sepiashader.glsl COPYONLY)					
						
target_link_libraries(phototwix libmmal_core.so 
								libmmal_util.so 
								libmmal_vc_client.so 
								libvcos.so 
								libbcm_host.so 
								GLESv2 
								EGL
								ncurses 
								${OpenCV_LIBS}
								${SDL2_LIBRARY}
								${SDL2_IMAGE_LIBRARIES})
