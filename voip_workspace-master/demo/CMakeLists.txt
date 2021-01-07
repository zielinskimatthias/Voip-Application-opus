cmake_minimum_required( VERSION 3.0 )

project( VoIP_Demos )

include_directories( ${VOIP_EXT_INC_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../ext/rtaudio)

add_executable( hello_rtaudio hello_rtaudio.cpp )
target_link_libraries( hello_rtaudio ${VOIP_EXT_LIBS} )

add_executable( tonegen tonegen.cpp )
target_link_libraries( tonegen ${VOIP_EXT_LIBS} )

add_executable( socket_demo socket_demo.cpp )
target_link_libraries( socket_demo ${VOIP_EXT_LIBS} )
