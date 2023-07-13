add_executable(test_mahou
	test_sdf.cpp
	test_interesect.cpp
	test_octree.cpp)
target_link_libraries(test_mahou PUBLIC CONAN_PKG::catch2 mahou_core)