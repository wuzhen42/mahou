add_executable(test_mahou test.cpp)
target_link_libraries(test_mahou PUBLIC CONAN_PKG::catch2 mahou_core)