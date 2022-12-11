cmake_minimum_required(VERSION 3.21)
include(FetchContent)

project( bashers-delight )

# lots of warnings and all warnings as errors
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# big object files
if (MSVC)
  add_compile_options(/bigobj)
else ()
  add_compile_options(-Wa,-mbig-obj)
endif ()

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
				
	# dowload gtest
	FetchContent_Declare(
	  googletest
	  GIT_REPOSITORY 	https://github.com/google/googletest.git
	  GIT_TAG			58d77fa8070e8cec2dc1ed015d66b454c8d78850
	)
	FetchContent_MakeAvailable(googletest)			
		
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

	target_include_directories(	
		SystemTest 
		PUBLIC ${glm_SOURCE_DIR}
		PUBLIC ${vma_SOURCE_DIR}/include
		PUBLIC ${Vulkan_INCLUDE_DIR}
	)

	target_link_libraries( 	
		SystemTest 
		Rpcrt4
		${Vulkan_LIBRARIES}
	)

endif()