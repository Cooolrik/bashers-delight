// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_FramebufferPool.h"

namespace bdr
{
	FramebufferPool::FramebufferPool( const Instance* _module ) : MainSubmodule(_module) 
		{
		LogThis;
		}

	FramebufferPool::~FramebufferPool()
		{
		LogThis;

		this->Cleanup();
		}

	status FramebufferPool::Cleanup()
		{
		return status_code::ok;
		}

}