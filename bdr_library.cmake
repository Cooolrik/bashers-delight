# bdr library source files
set(
	bdr_library_files

	./bdr/bdr.h
	./bdr/bdr.inl
	
	./bdr/bdr_AllocationsBlock.cpp
	./bdr/bdr_AllocationsBlock.h
	./bdr/bdr_Buffer.cpp
	./bdr/bdr_Buffer.h
	./bdr/bdr_BufferMemoryBarrier.h
	./bdr/bdr_CommandPool.cpp
	./bdr/bdr_CommandPool.h
	./bdr/bdr_FramebufferPool.cpp
	./bdr/bdr_FramebufferPool.h
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
	./bdr/bdr_Helpers.cpp
	./bdr/bdr_Helpers.h
	./bdr/bdr_Image.cpp
	./bdr/bdr_Image.h
	./bdr/bdr_ImageMemoryBarrier.h
	#./bdr/bdr_IndexBuffer.cpp
	#./bdr/bdr_IndexBuffer.h
	./bdr/bdr_Instance.h
	./bdr/bdr_Instance.cpp
	./bdr/bdr_MultithreadPool.h
	./bdr/bdr_MultithreadPool.cpp
	#./bdr/bdr_Pipeline.cpp
	#./bdr/bdr_Pipeline.h
	./bdr/bdr_RenderPass.cpp
	./bdr/bdr_RenderPass.h
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
	
source_group_by_folder( ./bdr bdr ${bdr_library_files} )

if( BDR_BUILD_TESTS )

	set(
		bdr_library_test_files

		./tests/TestImages.h
		./tests/SystemTest.cpp
		)

	source_group_by_folder( ./tests bdr_library_tests ${bdr_library_test_files} )			
	
	# using c++17 in tests
	set (CMAKE_CXX_STANDARD 17)
	
	add_executable( 
		SystemTest

		${bdr_files}
		${bdr_library_test_files}
	)	

	# lots of warnings 
	if (MSVC)
		target_compile_options( SystemTest PUBLIC /W4 )
	else()
		target_compile_options( SystemTest PUBLIC -Wall -Wextra -pedantic -Werror)
	endif()

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
		${Vulkan_LIBRARIES}
		glfw
	)

endif()