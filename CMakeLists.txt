cmake_minimum_required(VERSION 3.21)
include(FetchContent)

project( bashers-delight )

# dependendices

# Vulkan SDK needs to be installed
find_package( Vulkan REQUIRED )

# glm 
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG        bf71a834948186f4097caa076cd2663c69a10e1e # 0.9.9.8
)
FetchContent_MakeAvailable( glm )

# vma
FetchContent_Declare(
	vma
	GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
	GIT_TAG        a6bfc237255a6bac1513f7c1ebde6d8aed6b5191 # 3.0.1
)
FetchContent_MakeAvailable( vma )			
			
# ctle 
FetchContent_Declare(
	ctle
	GIT_REPOSITORY https://github.com/Cooolrik/ctle.git
	GIT_TAG		   main
)
FetchContent_MakeAvailable( ctle )						
			
			
# build test if this is the main cmake file
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)			
				
	# googletest
	FetchContent_Declare(
	  googletest
	  GIT_REPOSITORY 	https://github.com/google/googletest.git
	  GIT_TAG			58d77fa8070e8cec2dc1ed015d66b454c8d78850 # 1.12.1 
	)
	FetchContent_MakeAvailable(googletest)			
		
	# glfw
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(GLFW_INCLUDE_VULKAN ON CACHE BOOL "" FORCE)
	FetchContent_Declare(
	  glfw
	  GIT_REPOSITORY 	https://github.com/glfw/glfw.git
	  GIT_TAG			7482de6071d21db77a7236155da44c172a7f6c9e # 3.3.8
	)
	FetchContent_MakeAvailable(glfw)			
		
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
	
	# source files in bdr
	set(
		bdr_files

		./bdr/bdr.h
		./bdr/bdr.inl
		
		#./bdr/bdr_Buffer.cpp
		#./bdr/bdr_Buffer.h
		./bdr/bdr_CommandPool.cpp
		./bdr/bdr_CommandPool.h
		#./bdr/bdr_Common.inl
		#./bdr/bdr_ComputePipeline.cpp
		#./bdr/bdr_ComputePipeline.h
		#./bdr/bdr_DescriptorPool.cpp
		#./bdr/bdr_DescriptorPool.h
		#./bdr/bdr_DescriptorSetLayout.cpp
		#./bdr/bdr_DescriptorSetLayout.h
		./bdr/bdr_Device.h
		./bdr/bdr_Device.cpp
		./bdr/bdr_Extension.cpp
		./bdr/bdr_Extension.h
		#./bdr/bdr_GraphicsPipeline.cpp
		#./bdr/bdr_GraphicsPipeline.h
		#./bdr/bdr_Helpers.cpp
		#./bdr/bdr_Helpers.h
		#./bdr/bdr_Image.cpp
		#./bdr/bdr_Image.h
		#./bdr/bdr_IndexBuffer.cpp
		#./bdr/bdr_IndexBuffer.h
		./bdr/bdr_Instance.h
		./bdr/bdr_Instance.cpp
		#./bdr/bdr_Pipeline.cpp
		#./bdr/bdr_Pipeline.h
		#./bdr/bdr_Sampler.cpp
		#./bdr/bdr_Sampler.h
		#./bdr/bdr_ShaderModule.cpp
		#./bdr/bdr_ShaderModule.h
		./bdr/bdr_Swapchain.cpp
		./bdr/bdr_Swapchain.h
		#./bdr/bdr_VertexBuffer.cpp
		#./bdr/bdr_VertexBuffer.h

		./bdr/extensions/bdr_BufferDeviceAddressExtension.h
		./bdr/extensions/bdr_BufferDeviceAddressExtension.cpp

		./bdr/extensions/bdr_DescriptorIndexingExtension.h
		./bdr/extensions/bdr_DescriptorIndexingExtension.cpp

		./bdr/extensions/ray_tracing/bdr_RayTracingExtension.h
		./bdr/extensions/ray_tracing/bdr_RayTracingExtension.cpp
		#./bdr/extensions/RayTracing/bdr_RayTracingAccelerationStructure.cpp
		#./bdr/extensions/RayTracing/bdr_RayTracingAccelerationStructure.h
		#./bdr/extensions/RayTracing/bdr_RayTracingBLASEntry.cpp
		#./bdr/extensions/RayTracing/bdr_RayTracingBLASEntry.h

		#./bdr/extensions/RayTracing/bdr_RayTracingPipeline.cpp
		#./bdr/extensions/RayTracing/bdr_RayTracingPipeline.h
		#./bdr/extensions/RayTracing/bdr_RayTracingShaderBindingTable.cpp
		#./bdr/extensions/RayTracing/bdr_RayTracingShaderBindingTable.h
		#./bdr/extensions/RayTracing/bdr_RayTracingTLASEntry.cpp
		#./bdr/extensions/RayTracing/bdr_RayTracingTLASEntry.h


		./bdr/implementations/bdr_ctle_implementation.cpp
		./bdr/implementations/bdr_vma_implementation.cpp
		
		)
		
	source_group_by_folder( ./bdr bdr ${bdr_files} )			
	source_group_by_folder( ./tests tests ./tests/SystemTest.cpp )			
	
	add_executable( 
		SystemTest
		
		# bdr
		${bdr_files}
		
		# tests
		./tests/SystemTest.cpp 
	)	

	# lots of warnings and all warnings as errors
	if (MSVC)
		target_compile_options( SystemTest PUBLIC /W4 /WX)
	else()
		target_compile_options( SystemTest PUBLIC -Wall -Wextra -pedantic -Werror)
	endif()

	# big object files
	if (MSVC)
	  target_compile_options( SystemTest PUBLIC /bigobj)
	else ()
	  target_compile_options( SystemTest PUBLIC -Wa,-mbig-obj)
	endif ()

	target_include_directories(	
		SystemTest 
		PUBLIC .
		PUBLIC ${glm_SOURCE_DIR}
		PUBLIC ${vma_SOURCE_DIR}/include
		PUBLIC ${glfw_SOURCE_DIR}/include
		PUBLIC ${ctle_SOURCE_DIR}
		PUBLIC ${Vulkan_INCLUDE_DIR}
	)

	target_link_libraries( 	
		SystemTest 
		Rpcrt4
		${Vulkan_LIBRARIES}
		glfw
	)

endif()