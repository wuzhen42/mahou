add_library(mahou_core
	ddg.cpp
	triangulation.cpp
	normal.cpp
	octree.cpp
	intersect.cpp
	sdf.cpp
	halfedge.cpp
	mesh.cpp)
target_include_directories(mahou_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/mahou)
target_include_directories(mahou_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mahou_core PUBLIC CONAN_PKG::boost CONAN_PKG::glm CONAN_PKG::fmt CONAN_PKG::eigen)
