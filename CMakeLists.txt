cmake_minimum_required(VERSION 3.8)
project(Shadow-mapping)

set(CMAKE_CXX_STANDARD 11)
set(PROJECT_NAME Shadow-mapping)


set(SOURCE_FILES
        Maths/COLOR.cpp
        Maths/MATRIX4X4.cpp
        Maths/PLANE.cpp
        Maths/VECTOR2D.cpp
        Maths/VECTOR3D.cpp
        Maths/VECTOR4D.cpp
        main.cpp
        scene.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )