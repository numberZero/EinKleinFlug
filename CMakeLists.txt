cmake_minimum_required(VERSION 3.1)
project(einkleinflug)

cmake_policy(SET CMP0004 OLD)
set(CMAKE_CXX_STANDARD 11)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

add_executable(einkleinflug
	common/main.cxx
	game/game.cxx
	logic/ship.cxx
	menu/menu.cxx
	particles/beam.cxx
	particles/emitter.cxx
	particles/explosion.cxx
	particles/jet.cxx
	particles/system.cxx
	physics/body.cxx
	physics/manifold.cxx
	physics/world.cxx
	visual/text.cxx
)
target_compile_definitions(einkleinflug PRIVATE
	GL_GLEXT_PROTOTYPES
)
target_compile_options(einkleinflug PRIVATE
	-Wall -Wextra
)
target_include_directories(einkleinflug PUBLIC
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/common/
	${GLUT_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${SDL2_INCLUDE_DIRS}
)
target_link_libraries(einkleinflug
	${CMAKE_THREAD_LIBS_INIT}
	${GLUT_LIBRARY}
	${OPENGL_LIBRARIES}
	${SDL2_LIBRARIES}
)
