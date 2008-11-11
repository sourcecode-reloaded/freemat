FILE( GLOB f_src src/*.f )
IF( MSVC )
	CONVERT_F2C( lapack c_src ${f_src} )
	ADD_LIBRARY( lapack ${c_src} src/dsecnd.c src/second.c )
ELSE( MSVC )
	ADD_LIBRARY( lapack ${f_src} )
ENDIF( MSVC )
#GET_TARGET_PROPERTY(LAPACK_LIBRARY lapack LOCATION)

IF(NOT MSVC)
	INSTALL( TARGETS lapack ARCHIVE DESTINATION lib )
ELSE(NOT MSVC)
	INSTALL( TARGETS lapack ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/bin )
ENDIF(NOT MSVC)


