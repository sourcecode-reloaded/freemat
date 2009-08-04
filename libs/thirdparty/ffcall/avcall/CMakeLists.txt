IF (MSVC)
   INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
   CONFIGURE_FILE(avcall.h.msvc avcall.h)
   ADD_LIBARY(avcall
   avcall-i386-msvc.c
   structcpy.c)
ENDIF (MSVC)
