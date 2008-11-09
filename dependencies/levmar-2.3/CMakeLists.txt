# levmar CMake file; see http://www.cmake.org and 
#                        http://www.insightsoftwareconsortium.org/wiki/index.php/CMake_Tutorial

PROJECT(LEVMAR C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

# compiler flags
#ADD_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY) # do not free memory between linear solvers calls
REMOVE_DEFINITIONS(-DLINSOLVERS_RETAIN_MEMORY)

# levmar library source files
ADD_LIBRARY(levmar STATIC
  lm.c Axb.c misc.c lmlec.c lmbc.c lmblec.c
  lm.h misc.h compiler.h
)

INSTALL(TARGETS levmar ARCHIVE DESTINATION lib)
INSTALL(FILES lm.h DESTINATION include)
