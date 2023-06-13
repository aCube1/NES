include_guard()

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)


function(link_default_libraries target)
	find_package(
		SFML 2.5.1
		COMPONENTS
			window graphics audio
		REQUIRED
	)

	add_subdirectory(${LIBS_DIR}/spdlog)

	target_link_libraries(
		${target}
		PRIVATE
			spdlog::spdlog
			sfml-window
			sfml-graphics
			sfml-audio
	)
endfunction()
