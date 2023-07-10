add_library(mahou_core
	octree.cpp
)
target_include_directories(mahou_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mahou_core INTERFACE CONAN_PKG::boost CONAN_PKG::glm CONAN_PKG::fmt CONAN_PKG::eigen)
