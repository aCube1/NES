include_guard()

set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)

function(link_default_libraries target)
	find_package(
		SFML 2.6.0
		COMPONENTS
			window graphics audio
		REQUIRED
	)

	set(SPDLOG_ENABLE_PCH ON)
	add_subdirectory(${LIBS_DIR}/spdlog)

	target_link_libraries(
		${target}
		PRIVATE
			sfml-window
			sfml-graphics
			sfml-audio

			spdlog::spdlog
	)
endfunction()
