// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_CommandPool.h"
#include "bdr_AllocationsBlock.h"
#include "bdr_Swapchain.h"

namespace bdr
{

AllocationsBlock::AllocationsBlock( Device *_module ) : DeviceSubmodule( _module ), CommandPools( _module ), Swapchains( _module )
	{
	LogThis;
	}

AllocationsBlock::~AllocationsBlock()
	{
	LogThis;

	this->Cleanup();
	}

status AllocationsBlock::Setup( const AllocationsBlockTemplate &/*parameters*/ )
	{
	return status_code::ok;
	}

status AllocationsBlock::Cleanup()
	{
	this->CommandPools.Cleanup();
	this->Swapchains.Cleanup();

	return status_code::ok;
	}

status_return<Swapchain *> AllocationsBlock::CreateSwapchain( const SwapchainTemplate &parameters )
	{
	return this->Swapchains.Create( parameters );
	}

status AllocationsBlock::DestroySwapchain( Swapchain *swapchain )
	{
	CheckCall( this->Swapchains.Destroy( swapchain ) );
	return status::ok;
	}

status_return<CommandPool *> AllocationsBlock::CreateCommandPool( const CommandPoolTemplate &parameters )
	{
	return this->CommandPools.Create( parameters );
	}

status AllocationsBlock::DestroyCommandPool( CommandPool *commandPool )
	{
	CheckCall( this->CommandPools.Destroy( commandPool ) );
	return status::ok;
	}

}
// namespace bdr