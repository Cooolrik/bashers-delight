// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

// disable some VMA implementation warnings
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable: 4100 4127 4324 4189 )
#elif defined(__GNUC__)
#endif

#ifndef NDEBUG
#define VMA_DEBUG_MARGIN 16
#define VMA_DEBUG_DETECT_CORRUPTION 1
#endif
#define VMA_IMPLEMENTATION

#include <bdr/bdr.h>

// re-enable warnings 
#if defined(_MSC_VER)
#pragma warning( pop )
#elif defined(__GNUC__)
#endif