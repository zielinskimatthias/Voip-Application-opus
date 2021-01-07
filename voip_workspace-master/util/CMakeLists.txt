cmake_minimum_required( VERSION 3.0 )

project( VoIP_Utilities )

include_directories( include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../ext/rtaudio )

# Common sources
set( SRC  src/audiobuffer.cpp
          src/soundcard.cpp )

# Linux / OSX only
set( POSIX_SRC src/posix/socket_posix.cpp )

# Windows only
set( WIN_SRC src/win/socket_win.cpp )

if( ${CMAKE_SYSTEM_NAME} STREQUAL Windows )
  list( APPEND SRC ${WIN_SRC} )
else()
  list( APPEND SRC ${POSIX_SRC} )
endif()

set( HDR include/audiobuffer.h
         include/soundcard.h
         include/socket.h )

add_library( voiputil ${SRC} )
