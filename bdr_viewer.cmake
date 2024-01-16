if( BDR_BUILD_VIEWER )

	# bdr library source files
	set(
		bdr_viewer_files

		./src/viewer/main.cpp
		)
		
	source_group_by_folder( ./src/viewer viewer ${bdr_viewer_files} )
	
	# using c++17 
	set (CMAKE_CXX_STANDARD 17)
	
	add_executable( 
		bdr_viewer

		${bdr_library_files}
		${bdr_viewer_files}
		
		./bdr_library.cmake
		./bdr_viewer.cmake
		./dependencies.cmake
	)	

	# lots of warnings 
	if (MSVC)
		target_compile_options( bdr_viewer PUBLIC /W4 )
	else()
		target_compile_options( bdr_viewer PUBLIC -Wall -Wextra -pedantic -Werror)
	endif()

	target_include_directories(	
		bdr_viewer 
		PUBLIC .
		PUBLIC ${glm_SOURCE_DIR}
		PUBLIC ${vma_SOURCE_DIR}/include
		PUBLIC ${glfw_SOURCE_DIR}/include
		PUBLIC ${ctle_SOURCE_DIR}
		PUBLIC ${Vulkan_INCLUDE_DIR}
	)

	target_link_libraries( 	
		bdr_viewer 
		${Vulkan_LIBRARIES}
		glfw
	)

endif()