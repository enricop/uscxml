if (UNIX)
	if (APPLE)
		# mavericks broke it - will look into it eventually
		return()
	endif()
	if (NOT PHP_CONFIG)
		find_program(PHP_CONFIG
			NAMES php-config
			PATHS
				/usr/bin
				/usr/local/bin
				/opt/local/bin
		)
	endif()
	if (PHP_CONFIG)
		execute_process(COMMAND ${PHP_CONFIG} --includes
	    OUTPUT_VARIABLE PHP_INCLUDE_DIRS
	    ERROR_VARIABLE PHP_INCLUDE_errors
	    RESULT_VARIABLE PHP_INCLUDE_result)
		string(REGEX REPLACE "-I" "" PHP_INCLUDE_DIRS ${PHP_INCLUDE_DIRS})
		string(REGEX REPLACE "[ \n\r]+" ";" PHP_INCLUDE_DIRS ${PHP_INCLUDE_DIRS})

		execute_process(COMMAND ${PHP_CONFIG} --libs
	    OUTPUT_VARIABLE PHP_LIBRARIES
	    ERROR_VARIABLE PHP_LIBRARIES_errors
	    RESULT_VARIABLE PHP_LIBRARIES_result)
		string(REGEX REPLACE "-l" "" PHP_LIBRARIES ${PHP_LIBRARIES})
		string(REGEX REPLACE "[ \n\r]+" ";" PHP_LIBRARIES ${PHP_LIBRARIES})
		list(REMOVE_DUPLICATES PHP_LIBRARIES)
		
		execute_process(COMMAND ${PHP_CONFIG} --ldflags
	    OUTPUT_VARIABLE PHP_LDFLAGS
	    ERROR_VARIABLE PHP_LDFLAGS_errors
	    RESULT_VARIABLE PHP_LDFLAGS_result)

		execute_process(COMMAND ${PHP_CONFIG} --configure-options
	    OUTPUT_VARIABLE PHP_CONFIGURE_OPTIONS
	    ERROR_VARIABLE PHP_CONFIGURE_OPTIONS_errors
	    RESULT_VARIABLE PHP_CONFIGURE_OPTIONS_result)

		set(PHP_ZTS_ENABLED OFF)
		if (PHP_CONFIGURE_OPTIONS MATCHES ".*enable-maintainer-zts.*")
			set(PHP_ZTS_ENABLED ON)
		endif()
		
		set(PHP_DEBUG_ENABLED OFF)
		if (PHP_CONFIGURE_OPTIONS MATCHES ".*enable-debug.*")
			set(PHP_DEBUG_ENABLED ON)
		endif()
		
	endif()
else()
	message(STATUS "Finding PHP5 on Windows is not supported")
endif()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PHP5 DEFAULT_MSG PHP_CONFIG PHP_INCLUDE_DIRS PHP_LIBRARIES PHP_LDFLAGS)
MARK_AS_ADVANCED(PHP_LIBRARIES PHP_INCLUDE_DIRS PHP_LDFLAGS PHP_ZTS_ENABLED)
