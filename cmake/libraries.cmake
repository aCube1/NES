include_guard()

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)

set(SDL2_VERSION 2.24.2)

function(link_default_libraries target)
	find_package(SDL2 ${SDL2_VERSION} REQUIRED)

	add_subdirectory(${LIBS_DIR}/spdlog)

	target_link_libraries(
		${target}
		PRIVATE
			SDL2::SDL2
			spdlog::spdlog
	)
endfunction()
