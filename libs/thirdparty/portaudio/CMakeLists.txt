INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/Common)
IF (APPLE)
   ADD_DEFINITIONS(-DPA_USE_COREAUDIO)
   ADD_LIBRARY(portaudio
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_allocation.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_converters.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_cpuload.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_debugprint.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_dither.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_front.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_process.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_ringbuffer.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_skeleton.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_stream.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_trace.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/coreaudio/pa_mac_core.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/coreaudio/pa_mac_core_blocking.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/coreaudio/pa_mac_core_utilities.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/mac_osx/pa_mac_hostapis.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/unix/pa_unix_util.c )
ENDIF (APPLE) 
