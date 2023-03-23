add_library(mahou_core INTERFACE)
target_include_directories(mahou_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mahou_core INTERFACE boost::boost glm::glm fmt::fmt-header-only Eigen3::Eigen)

add_subdirectory(bin)
add_subdirectory(hou)