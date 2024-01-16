
#if defined(_MSC_VER)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__GNUC__)
#endif

#include <bdr/bdr.h>

#include <iostream>

#include <bdr/bdr.h>
#include <bdr/bdr_Instance.h>
#include <bdr/bdr_Device.h>
#include <bdr/bdr_CommandPool.h>
#include <bdr/bdr_AllocationsBlock.h>
#include <bdr/bdr_Image.h>
#include <bdr/bdr_Buffer.h>
#include <bdr/bdr_RenderPass.h>
#include <bdr/bdr_FramebufferPool.h>
#include <bdr/bdr_Swapchain.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <ctle/_macros.inl>

namespace bdr
{
namespace viewer
{

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
	VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* /*pUserData*/)
	{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
	}

status run( int /*argc*/ , char */*argv*/[] )
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
	ctStatusAutoReturnCall( device , instance->CreateDevice( dparams ) );

	ctStatusAutoReturnCall( swapchain , device->CreateObject<Swapchain>( SwapchainTemplate::DeviceCompatible( device ).value() ) );

	ctStatusAutoReturnCall( allocationsBlock , device->CreateAllocationsBlock() );

	ctStatusAutoReturnCall( commandPool , allocationsBlock->CreateCommandPool( bdr::CommandPoolTemplate() ) );

	ctStatusAutoReturnCall( renderPass, device->CreateObject<RenderPass>( RenderPassTemplate::SingleSubPass( VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_D24_UNORM_S8_UINT, VK_SAMPLE_COUNT_1_BIT ) ) );

	ctStatusAutoReturnCall( framebufferPool, device->CreateObject<FramebufferPool>( FramebufferPoolTemplate::FromRenderPass( 3, 400, 300, renderPass.get(), commandPool ).value() ) );

	//ubo uboobj = {};
	//uboobj.a = 1.f;
	//uboobj.b = 2.f;
	//ctStatusReturnCall( buffer , device->CreateObject<Buffer>( BufferTemplate::UniformBuffer( sizeof(uboobj), &uboobj, commandPool ) ) );
	//buffer.reset();
	//
	//TestImage<sRGB> texture2d;
	//texture2d.Setup(8,8);
	//ctStatusReturnCall( image, device->CreateObject<Image>( texture2d.GetTexture2DTemplate(commandPool) ) );
	//image.reset();

	std::cout << commandPool << std::endl;

	status = Release( framebufferPool );

	status = Release( renderPass );

	status = Release( swapchain );

	status = Release( instance );

	glfwDestroyWindow( window );

	glfwTerminate();

	return status::ok;
	}
}
// namespace viewer
}
// bdr

int main( int argc , char *argv[] )
	{
	ctle::status res = {};
	try {
		res = bdr::viewer::run(argc, argv);
		}
	catch (const std::exception& e) 
		{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
		}

	return int(res.value());
	}