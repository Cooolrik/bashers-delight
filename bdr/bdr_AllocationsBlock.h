// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Device.h"

namespace bdr
{

class AllocationsBlock : public DeviceSubmodule
	{
	public:
		~AllocationsBlock();

	private:
		friend status_return<unique_ptr<AllocationsBlock>> Device::CreateObject<AllocationsBlock, AllocationsBlockTemplate>( const AllocationsBlockTemplate &parameters );
		AllocationsBlock( Device *_module );
		status Setup( const AllocationsBlockTemplate &parameters );

		// allocation maps for the object types held by this allocations block
		DeviceAllocationMap<CommandPool> CommandPools;
		DeviceAllocationMap<Swapchain> Swapchains;

	public:
		// explicitly cleanups the object. deletes all owned objects.
		status Cleanup();

		// creates a swapchain with framebuffer objects attached
		status_return<Swapchain *> CreateSwapchain( const SwapchainTemplate &parameters );

		// destroys a swapchain object
		status DestroySwapchain( Swapchain *swapchain );

		// create a command pool object
		status_return<CommandPool *> CreateCommandPool( const CommandPoolTemplate &parameters );

		// destroy a command pool object
		status DestroyCommandPool( CommandPool *commandPool );

	};

class AllocationsBlockTemplate
	{
	public:
	};

}
// namespace bdr