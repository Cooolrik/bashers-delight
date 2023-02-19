// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_Swapchain.h"

namespace bdr
{
	Swapchain::Swapchain( const Instance* _module ) : MainSubmodule(_module) 
		{
		LogThis;
		}

	Swapchain::~Swapchain()
		{
		LogThis;

		this->Cleanup();
		}

	status Swapchain::Cleanup()
		{
		return status_code::ok;
		}

}