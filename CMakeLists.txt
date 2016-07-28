cmake_minimum_required(VERSION 3.1)
project(einkleinflug)

cmake_policy(SET CMP0004 OLD)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(einkleinflug
	body.cxx
	main.cxx
	manifold.cxx
	particles/particles.cxx
	ship.cxx
	text.cxx
	world.cxx
)
target_compile_definitions(einkleinflug PRIVATE
	GL_GLEXT_PROTOTYPES
)
target_include_directories(einkleinflug PUBLIC
 	${PROJECT_SOURCE_DIR}/
	${EIGEN3_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${GLUT_INCLUDE_DIR}
)
target_link_libraries(einkleinflug
	${SDL2_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
)
