# Find the ZeroC ICE includes and libraries for every module (Ice, IceStorm, IceUtil, etc)

#
# ZeroCIce_INCLUDE_DIR - Where the includes are. If everything is all right, ZeroCIceXXXX_INCLUDE_DIR is always the same. You usually will use this.
# ZeroCIce_LIBRARIES - List of *all* the libraries. You usually will not use this but only ZeroCIceUtil_LIBRARY or alike
# ZerocCIce_FOUND - True if the core Ice was found
# ZeroCIceCore_FOUND
# ZeroCIceCore_INCLUDE_DIR
# ZeroCIceCore_LIBRARY
# ZeroCIceBox_FOUND
# ZeroCIceBox_INCLUDE_DIR
# ZeroCIceBox_LIBRARY
# ZeroCIceGrid_FOUND
# ZeroCIceGrid_INCLUDE_DIR
# ZeroCIceGrid_LIBRARY
# ZeroCIcePatch2_FOUND
# ZeroCIcePatch2_INCLUDE_DIR
# ZeroCIcePatch2_LIBRARY
# ZeroCIceSSL_FOUND
# ZeroCIceSSL_INCLUDE_DIR
# ZeroCIceSSL_LIBRARY
# ZeroCIceStorm_FOUND
# ZeroCIceStorm_INCLUDE_DIR
# ZeroCIceStorm_LIBRARY
# ZeroCIceUtil_FOUND
# ZeroCIceUtil_INCLUDE_DIR
# ZeroCIceUtil_LIBRARY
# ZeroCIceXML_FOUND
# ZeroCIceXML_INCLUDE_DIR
# ZeroCIceXML_LIBRARY
# ZeroCIceExecutables_FOUND

#
# Copyright (c) 2007, Pau Garcia i Quiles, <pgquiles@elpauer.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

SET( ZeroCIceCore_FIND_QUIETLY TRUE )
SET( ZeroCIceBox_FIND_QUIETLY TRUE )
SET( ZeroCIceGrid_FIND_QUIETLY TRUE )
SET( ZeroCIcePatch2_FIND_QUIETLY TRUE )
SET( ZeroCIceSSL_FIND_QUIETLY TRUE )
SET( ZeroCIceStorm_FIND_QUIETLY TRUE )
SET( ZeroCIceUtil_FIND_QUIETLY TRUE )
SET( ZeroCIceXML_FIND_QUIETLY TRUE )
SET( ZeroCIceExecutables_FIND_QUIETLY TRUE )

FIND_PACKAGE( ZeroCIceCore )
FIND_PACKAGE( ZeroCIceBox )
FIND_PACKAGE( ZeroCIceGrid )
FIND_PACKAGE( ZeroCIcePatch2 )
FIND_PACKAGE( ZeroCIceSSL )
FIND_PACKAGE( ZeroCIceStorm )
FIND_PACKAGE( ZeroCIceUtil )
FIND_PACKAGE( ZeroCIceXML )
FIND_PACKAGE( ZeroCIceExecutables )

SET( ZeroCIce_INCLUDE_DIR ${ZeroCIceCore_INCLUDE_DIR} )
SET( ZeroCIce_LIBRARIES ${ZeroCIceCore_LIBRARY} ${ZeroCIceBox_LIBRARY} ${ZeroCIceGrid_LIBRARY} ${ZeroCIcePatch2_LIBRARY} ${ZeroCIceSSL_LIBRARY} ${ZeroCIceStorm_LIBRARY} ${ZeroCIceUtil_LIBRARY} ${ZeroCIceXML_LIBRARY} )

FOREACH( exec ${ICE_EXECUTABLES} )
	IF(ZeroCIce_${exec}_FOUND)
		LIST(APPEND ZeroCIce_EXECUTABLES ${ZeroCIce_${exec}_BIN} )
	ENDIF(ZeroCIce_${exec}_FOUND)
ENDFOREACH( exec ${ICE_EXECUTABLES} )

macro( SLICE_GENERATE_CPP generated_cpp_list generated_header_list )

    set( generator_command	${ZeroCIce_slice2cpp_BIN} )
    set( proj_slice_src_dir	${PROJECT_SOURCE_DIR} )
    set( slice2cpp_binary_dir	${PROJECT_BINARY_DIR} )

    #
    # Loop through all SLICE sources we were given, add the CMake rules
    #
    set( slice_source_counter 0 )
    
    foreach( slice_source_basename ${ARGN} )
        set( slice_source "${proj_slice_src_dir}/${slice_source_basename}" )
        GET_FILENAME_COMPONENT(SRC_DIR ${slice_source_basename} PATH)
        set( slice_args ${SLICE_PROJECT_ARGS} --stream --output-dir ${slice2cpp_binary_dir}/${SRC_DIR} )


        #message( STATUS "DEBUG: Dealing with ${slice_source_basename}")

        #message( STATUS "DEBUG: Adding rule for generating header file from ${slice_source_basename}" )
        string( REGEX REPLACE "\\.ice" ".h" header_basename "${slice_source_basename}" )
        list( APPEND ${generated_header_list} ${header_basename} )
        # this command will be run at make time
        
        add_custom_command(
            OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${header_basename}
            COMMAND ${generator_command}
            ARGS ${slice_args} ${slice_source}
            COMMENT "-- Generating header file from ${slice_source}"
            VERBATIM
        )

        #message( STATUS "DEBUG: Adding rule for generating source file from ${slice_source_basename}" )
        string( REGEX REPLACE "\\.ice" ".cpp" cpp_basename "${slice_source_basename}" )
        list( APPEND ${generated_cpp_list} ${cpp_basename} )
        # this command will be run at make time
        add_custom_command(
            OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${cpp_basename}
            COMMAND ${generator_command}
            ARGS ${slice_args} ${slice_source}
            COMMENT "-- Generating source file from ${slice_source}"
            VERBATIM
        )

        math( EXPR slice_source_counter "${slice_source_counter} + 1" )

    endforeach( slice_source_basename )

    #message( STATUS "DEBUG: generated_cpp_list: ${${generated_cpp_list}}")
    #message( STATUS "DEBUG: generated_header_list: ${${generated_header_list}}")

    # this message is useful for manual generation
    message( STATUS "Will generate cpp header and source files from ${slice_source_counter} Slice definitions using this command:" )
    message( STATUS "${generator_command} <source.ice> ${slice_args}" )

endmacro( SLICE_GENERATE_CPP generated_cpp_list generated_header_list )

SET( ZeroCIce_FOUND ${ZeroCIceCore_FOUND} )

IF(ZeroCIce_FOUND)
	IF (NOT ZeroCIce_FIND_QUIETLY)
		MESSAGE(STATUS "Found the ZeroC Ice libraries at ${ZeroCIce_LIBRARIES}")
		MESSAGE(STATUS "Found the ZeroC Ice headers at ${ZeroCIce_INCLUDE_DIR}")
		MESSAGE(STATUS "Found these ZeroC Ice executables: ${ZeroCIce_EXECUTABLES}")
	ENDIF (NOT ZeroCIce_FIND_QUIETLY)
ELSE(ZeroCIce_FOUND)
	IF(ZeroCIce_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could NOT find ZeroC Ice (actually, at least the core Ice was not found)")
	ENDIF(ZeroCIce_FIND_REQUIRED)
ENDIF(ZeroCIce_FOUND)
