set(HOUDINI_VERSION 19.0 CACHE STRING "houdini version defaulting to '19.0'")
file(GLOB HOUDINI_ROOTS "C:/Program Files/Side Effects Software/Houdini ${HOUDINI_VERSION}.[0-9.]*")
foreach(HOUDINI_ROOT ${HOUDINI_ROOTS})
	list(APPEND CMAKE_PREFIX_PATH "${HOUDINI_ROOT}/toolkit/cmake")
endforeach()
find_package(Houdini REQUIRED)

add_library(mahou_hou SHARED
	SOP_measure.cpp
	initialize.cpp)
target_link_libraries(mahou_hou PRIVATE Houdini)
target_link_libraries(mahou_hou PRIVATE mahou_core)
set_target_properties(mahou_hou PROPERTIES OUTPUT_NAME mahou)

houdini_configure_target(mahou_hou)
