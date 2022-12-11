cmake_minimum_required(VERSION 3.21)
include(FetchContent)

project( bashers-delight )

# dependendices

# Vulkan SDK needs to be installed
find_package( Vulkan REQUIRED FATAL_ERROR )

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
			
# build test if this is the main cmake file
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)			
				
	# dowload googletest
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
				
	add_executable( 
		SystemTest
		./bdr/bdr_Buffer.cpp
		./bdr/bdr_BufferDeviceAddressExtension.cpp
		./bdr/bdr_CommandPool.cpp
		./bdr/bdr_ComputePipeline.cpp
		./bdr/bdr_DescriptorIndexingExtension.cpp
		./bdr/bdr_DescriptorPool.cpp
		./bdr/bdr_DescriptorSetLayout.cpp
		./bdr/bdr_Extension.cpp
		./bdr/bdr_GraphicsPipeline.cpp
		./bdr/bdr_Helpers.cpp
		./bdr/bdr_Image.cpp
		./bdr/bdr_IndexBuffer.cpp
		./bdr/bdr_Pipeline.cpp
		./bdr/bdr_RayTracingAccelerationStructure.cpp
		./bdr/bdr_RayTracingBLASEntry.cpp
		./bdr/bdr_RayTracingExtension.cpp
		./bdr/bdr_RayTracingPipeline.cpp
		./bdr/bdr_RayTracingShaderBindingTable.cpp
		./bdr/bdr_RayTracingTLASEntry.cpp
		./bdr/bdr_Renderer.cpp
		./bdr/bdr_Sampler.cpp
		./bdr/bdr_ShaderModule.cpp
		./bdr/bdr_VertexBuffer.cpp
		./bdr/bdr_vma_implementation.cpp
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
		PUBLIC ${Vulkan_INCLUDE_DIR}
	)

	target_link_libraries( 	
		SystemTest 
		Rpcrt4
		${Vulkan_LIBRARIES}
		glfw
	)

endif()