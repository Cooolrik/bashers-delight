#include <windows.h>
#include <iostream>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <bdr/bdr_Renderer.h>

using namespace bdr;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
	VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* /*pUserData*/)
	{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
	}

void run()
	{
	glfwInit();
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

	auto window = glfwCreateWindow( 800, 600, "VulkanRenderTest", nullptr, nullptr );

	// list the needed extensions for glfw
	uint glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

	// create the renderer, list needed extensions
	Renderer::CreateParameters createParameters{};
	createParameters.EnableVulkanValidation = true;
	createParameters.EnableRayTracingExtension = true;
	createParameters.NeededExtensionsCount = glfwExtensionCount;
	createParameters.NeededExtensions = glfwExtensions;
	createParameters.DebugMessageCallback = &debugCallback;
	createParameters.DebugMessageSeverityMask =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createParameters.DebugMessageTypeMask =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	auto renderer = Renderer::Create( createParameters );

	//// create the window surface using glfw
	//if( glfwCreateWindowSurface( renderData->renderer->GetInstance(), renderData->window, nullptr, &renderData->surface ) != VK_SUCCESS )
	//	{
	//	throw std::runtime_error( "failed to create window surface!" );
	//	}

	delete renderer;

	glfwDestroyWindow( window );
	glfwTerminate();
	}

int main(int /*argc*/, char* /*argv*/)
	{
	try {
		run();
		}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
		}

	return EXIT_SUCCESS;
	}
