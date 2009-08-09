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

IF (MSVC)
   ADD_DEFINITIONS(-DPA_USE_WDMKSDEVICE_INFO)
   INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/os/win)
   ADD_LIBRARY(portaudio
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_allocation.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_converters.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_cpuload.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_debugprint.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_dither.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_front.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_process.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_skeleton.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_stream.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_trace.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/asio/pa_asio.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/asio/ASIOSDK/common/asio.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/asio/ASIOSDK/host/ASIOConvertSamples.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/asio/ASIOSDK/host/asiodrivers.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/asio/ASIOSDK/host/pc/asiolist.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/asio/ASIOSDK/common/combase.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/asio/ASIOSDK/common/debugmessage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/asio/ASIOSDK/common/register.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/dsound/pa_win_ds.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/dsound/pa_win_ds_dynlink.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/wmme/pa_win_wmme.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/wasapi/pa_win_wasapi.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_win_hostapis.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_win_util.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_win_waveformat.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_win_wdmks_utils.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_x86_plain_converters.c)
ENDIF (MSVC)
