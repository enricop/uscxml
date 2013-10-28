FIND_PATH(EVENT_INCLUDE_DIR event2/event.h
  PATH_SUFFIXES include
  PATHS
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
  HINTS $ENV{EVENT_SRC}
)

FIND_LIBRARY(EVENT_LIBRARY_RELEASE
  NAMES event libevent
  HINTS $ENV{EVENT_SRC}/.libs/
)
if (EVENT_LIBRARY_RELEASE)
	list(APPEND EVENT_LIBRARY optimized ${EVENT_LIBRARY_RELEASE})
endif()

FIND_LIBRARY(EVENT_LIBRARY_DEBUG
  NAMES event_d libevent_d
  HINTS $ENV{EVENT_SRC}/.libs/
)
if (EVENT_LIBRARY_DEBUG)
	list(APPEND EVENT_LIBRARY debug ${EVENT_LIBRARY_DEBUG})
else()
	if (EVENT_LIBRARY_RELEASE)
		list(APPEND EVENT_LIBRARY debug ${EVENT_LIBRARY_RELEASE})
	endif()
endif()



FIND_LIBRARY(EVENT_SSL_LIBRARY_RELEASE
  NAMES event_openssl libevent_openssl
  HINTS $ENV{EVENT_SRC}/.libs/
)
if (EVENT_SSL_LIBRARY_RELEASE)
	list(APPEND EVENT_LIBRARY optimized ${EVENT_SSL_LIBRARY_RELEASE})
	SET(EVENT_SSL_FOUND ON)
endif()

FIND_LIBRARY(EVENT_SSL_LIBRARY_DEBUG
  NAMES event_openssl_d libevent_openssl_d
  HINTS $ENV{EVENT_SRC}/.libs/
)
if (EVENT_SSL_LIBRARY_DEBUG)
	list(APPEND EVENT_LIBRARY debug ${EVENT_SSL_LIBRARY_DEBUG})
else()
	if (EVENT_SSL_LIBRARY_RELEASE)
		list(APPEND EVENT_LIBRARY debug ${EVENT_SSL_LIBRARY_RELEASE})
	endif()
endif()



if (NOT WIN32)
	FIND_LIBRARY(EVENT_LIBRARY_THREADS
	  NAMES event_pthreads
	  HINTS $ENV{EVENT_SRC}/.libs/
	)
	list (APPEND EVENT_LIBRARY ${EVENT_LIBRARY_THREADS})
	
	FIND_LIBRARY(EVENT_LIBRARY_EXTRA
	  NAMES event_extra
	  HINTS $ENV{EVENT_SRC}/.libs/
	)
	list (APPEND EVENT_LIBRARY ${EVENT_LIBRARY_EXTRA})

	FIND_LIBRARY(EVENT_LIBRARY_CORE
	  NAMES event_core
	  HINTS $ENV{EVENT_SRC}/.libs/
	)
	list (APPEND EVENT_LIBRARY ${EVENT_LIBRARY_CORE})

endif()

#message(FATAL_ERROR "EVENT_SSL_FOUND: ${EVENT_SSL_FOUND}")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EVENT DEFAULT_MSG EVENT_LIBRARY EVENT_INCLUDE_DIR)
MARK_AS_ADVANCED(EVENT_LIBRARY EVENT_INCLUDE_DIR EVENT_SSL_FOUND)
