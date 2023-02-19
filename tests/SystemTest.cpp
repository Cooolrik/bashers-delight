// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include <bdr/bdr.h>
#include <bdr/bdr_Instance.h>
#include <bdr/bdr_Device.h>
#include <bdr/bdr_CommandPool.h>
//#include <bdr/bdr_Swapchain.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

using namespace bdr;

#include <system_error>

#define CheckCall( scall )\
	{\
	auto BDRCall_statuspair = scall; \
	if( !BDRCall_statuspair ) {\
		std::cerr << "Call: " << #scall << " failed, returned status_code: " << BDRCall_statuspair << std::endl;\
		exit(1);\
		}\
	}

#define CheckRetValCall( retval , scall )\
	decltype( scall )::value_type retval = {};\
	{\
	auto BDRCall_statuspair = scall; \
	if( !BDRCall_statuspair.status() ) {\
		std::cerr << "Call: " << #scall << " failed, returned status_code: " << BDRCall_statuspair.status() << std::endl;\
		exit(1);\
		}\
	retval = std::move(BDRCall_statuspair.value());\
	}

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

	ctle::set_global_log_level( ctle::log_level::debug );

	// create the renderer, list needed extensions
	InstanceTemplate params;
	params.EnableValidation = true;
	params.EnableRayTracingExtension = true;
	params.NeededExtensionsCount = glfwExtensionCount;
	params.NeededExtensions = glfwExtensions;
	params.DebugMessageCallback = &debugCallback;
	params.DebugMessageSeverityMask =
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	params.DebugMessageTypeMask =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	status status;

	auto instanceCreateResult = Instance::Create( params );
	std::unique_ptr<Instance> instance = std::move(instanceCreateResult.value());
	status = instanceCreateResult.status();

	// create the window surface using glfw
	VkSurfaceKHR surface = {};
	if( glfwCreateWindowSurface( instance->GetInstanceHandle(), window, nullptr, &surface ) != VK_SUCCESS )
		{
		throw std::runtime_error( "failed to create window surface!" );
		}
	 
	DeviceTemplate dparams = { surface };
	CheckRetValCall( device , instance->CreateDevice( dparams ) );

	CheckRetValCall( commandPool , device->CreateCommandPool( bdr::CommandPoolTemplate() ) );

	status = Release( instance );

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
