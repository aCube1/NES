include_guard()

function(set_debug_options target)
	target_compile_options(${target} PRIVATE -g -Og)

	# Use address sanitizer flags.
	if (USE_SANITIZER_ASAN)
		target_compile_options(
			${target}
			PRIVATE
				-fsanitize=address
				-fsanitize=leak
				-fsanitize=undefined
				-fno-omit-frame-pointer
		)

		target_link_options(
			${target}
			PRIVATE
				-fsanitize=address
				-fsanitize=leak
				-fsanitize=undefined
				-fno-omit-frame-pointer
		)
	endif()
endfunction()
