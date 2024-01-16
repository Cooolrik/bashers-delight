
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

if( BDR_BUILD_TESTS OR BDR_BUILD_VIEWER )
			
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
		
endif()
