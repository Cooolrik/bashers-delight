cmake_minimum_required(VERSION 3.21)
include(FetchContent)
project( bashers-delight )

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)	
	set( BDR_MAIN_PROJECT ON )
else()
	set( BDR_MAIN_PROJECT OFF )
endif()

option( BDR_BUILD_TESTS "Build basher delight's tests" ${BDR_MAIN_PROJECT} )
option( BDR_BUILD_VIEWER "Build basher delight's viewer" ${BDR_MAIN_PROJECT} )
			
include( dependencies.cmake )			

# macro which groups files by their file folder 
macro( source_group_by_folder rootPath projectFolder )
  file( REAL_PATH ${rootPath} rootAbs )
  foreach( file ${ARGN} )
	file( REAL_PATH ${file} fileAbs )
	file(RELATIVE_PATH relative_file ${rootAbs} ${fileAbs} )
	get_filename_component(dir "${relative_file}" DIRECTORY )
	source_group("${projectFolder}/${dir}" FILES ${file})
  endforeach()
endmacro()

include( bdr_library.cmake )

include( bdr_viewer.cmake )
