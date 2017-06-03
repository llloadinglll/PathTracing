
include_directories(. ${PROJECT_SOURCE_DIR}/eigen3 ${PROJECT_SOURCE_DIR}/OpenGL)


Set (SRC_LIST
main.cpp
Object.cpp
PathTracer.cpp
Sphere.cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_BUILD_TYPE "Release")

add_executable(PathTracer ${SRC_LIST})


FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
	message("OPENMP FOUND")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
	message(“[USER]:OPENMP NOT FOUND.IGNORED.”)
endif()

if(WIN32)
	message("[USER]:WIN32")
	find_library(findfreeglut freeglut ${PROJECT_SOURCE_DIR}/OpenGL)
	find_library(findfreeglutd freeglutd ${PROJECT_SOURCE_DIR}/OpenGL)
	IF (NOT findfreeglut AND NOT findfreeglutd)
    MESSAGE(FATAL_ERROR "[USER]:freeglut not found")
	ENDIF(NOT findfreeglut AND NOT findfreeglutd)
	target_link_libraries(PathTracer ${findfreeglut}) 
	target_link_libraries(PathTracer ${findfreeglutd})
else(WIN32)
	message("[USER]:NOT WIN32")

	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIR})

	find_package(GLUT REQUIRED)
	include_directories(${GLUT_INCLUDE_DIR})

	target_link_libraries(PathTracer ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
endif(WIN32)

#file(GLOB parameter_files "${CMAKE_CURRENT_SOURCE_DIR}/parameter*")
#message("[USER]:parameter_files: ${parameter_files}")
#install(FILES ${parameter_files} DESTINATION "${PROJECT_BINARY_DIR}")