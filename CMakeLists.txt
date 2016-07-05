cmake_minimum_required(VERSION 3.0)
project(einkleinflug)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(einkleinflug
	main.cxx
)
target_include_directories(einkleinflug PUBLIC
 	${PROJECT_SOURCE_DIR}/
	${EIGEN3_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
)
target_link_libraries(einkleinflug
	${SDL2_LIBRARIES}
	${OPENGL_LIBRARIES}
)
target_compile_definitions(einkleinflug PUBLIC
	EIGEN_DONT_ALIGN
)
target_compile_options(einkleinflug PRIVATE
	-Wno-deprecated-declarations
)
target_compile_features(einkleinflug PUBLIC
	cxx_uniform_initialization
)

install(TARGETS einkleinflug RUNTIME DESTINATION bin)
