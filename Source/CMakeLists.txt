# Link to the actual SDL3 library and add executable.
foreach(target hello demo)
	add_executable(${target} WIN32 ${target}.cpp)
	target_link_libraries(${target} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
	set_target_properties(${target} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
endforeach()

target_link_libraries(demo PRIVATE glm::glm-header-only)