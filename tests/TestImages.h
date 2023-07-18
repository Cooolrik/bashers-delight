// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#pragma once

// TestImages has different templates for generating test images
#include <bdr/bdr.h>
#include <ctle/log.h>

#include <glm/glm.hpp>

#include <bdr/bdr_Image.h>

template<class _componentType, size_t _tupleSize, size_t oneValue, VkFormat _vulkanFormat> struct TestImageType
	{
	using componentType = typename _componentType; 
	static constexpr const size_t tupleSize = _tupleSize; 
	static constexpr const componentType one = (componentType)oneValue;
	static constexpr const VkFormat vulkanFormat = _vulkanFormat;
	};

using sRGB = TestImageType<uint8_t, 4, 255, VK_FORMAT_R8G8B8A8_SRGB>;

template<class _Ty>
class TestImage
	{
	public:
		static constexpr const size_t tupleSize = _Ty::tupleSize;
		using componentType = typename _Ty::componentType;
		static constexpr const componentType one = _Ty::one;
		static constexpr const VkFormat vulkanFormat = _Ty::vulkanFormat;

		// convert normalized value (0->1) to component type value
		componentType getComponentValue( float n )
			{
			return componentType( n * float( one ) );
			}

		std::vector<componentType> imageData;

		size_t mipmaps = 0; // number of mipmaps including original image
		size_t layers = 0;

		struct MipmapInfo
			{
			size_t width;
			size_t height;
			size_t depth;
			size_t offset;
			size_t size;
			};

		std::vector<MipmapInfo> mipmapInfos; // info on each mipmap (including the original image, which is mipmap 0)
		size_t layerSize = 0; // total size of each layer (in number of components)

		void Setup(
			const size_t _width = 512,
			const size_t _height = 512,
			const size_t _depth = 1,
			const size_t _layers = 1 )
			{
			this->layers = _layers;

			// get the number of mipmaps 
			const size_t maxDim = std::max( _width, std::max( _height, _depth ) );
			this->mipmaps = 1 + (size_t)( std::log2( maxDim ) );

			// calculate the mipmap infos, and size of each layer (total size of all mipmaps in one layer)
			this->mipmapInfos.resize( this->mipmaps );
			for( size_t mmap = 0; mmap < this->mipmaps; ++mmap )
				{
				if( mmap == 0 )
					{
					this->mipmapInfos[0].width = _width;
					this->mipmapInfos[0].height = _height;
					this->mipmapInfos[0].depth = _depth;
					}
				else
					{
					this->mipmapInfos[mmap].width = ( this->mipmapInfos[mmap-1].width + 1 ) / 2;
					this->mipmapInfos[mmap].height = ( this->mipmapInfos[mmap-1].height + 1 ) / 2;
					this->mipmapInfos[mmap].depth = ( this->mipmapInfos[mmap-1].depth + 1 ) / 2;
					}

				this->mipmapInfos[mmap].size = this->mipmapInfos[mmap].width * this->mipmapInfos[mmap].height * this->mipmapInfos[mmap].depth * tupleSize;
				this->mipmapInfos[mmap].offset = this->layerSize;

				this->layerSize += mipmapInfos[mmap].size;
				}

			// allocate number of layers times size of each layer
			imageData.resize( this->layerSize * this->layers );

			// start calculating layers
			for( size_t layerIndex = 0; layerIndex < this->layers; ++layerIndex )
				{
				this->MakeLayer( layerIndex );
				}
			}

		bdr::ImageTemplate GetTexture2DTemplate( bdr::CommandPool *commandPool , VkImageAspectFlagBits aspectType = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT )
			{
			std::vector<VkBufferImageCopy> copyRegions( this->mipmaps );
			for( size_t m=0; m<this->mipmaps; ++m )
				{
				copyRegions[m].bufferOffset = mipmapInfos[m].offset;
				copyRegions[m].bufferRowLength = 0;
				copyRegions[m].bufferImageHeight = 0;
				copyRegions[m].imageSubresource.aspectMask = aspectType;
				copyRegions[m].imageSubresource.mipLevel = (uint32_t)m;
				copyRegions[m].imageSubresource.baseArrayLayer = 0;
				copyRegions[m].imageSubresource.layerCount = 1;
				copyRegions[m].imageOffset = {0,0,0};
				copyRegions[m].imageExtent = {
					(uint32_t)mipmapInfos[m].width,
					(uint32_t)mipmapInfos[m].height,
					(uint32_t)mipmapInfos[m].depth
					};
				}

			return bdr::ImageTemplate::Texture2D( 
				this->vulkanFormat,
				(uint32_t)this->mipmapInfos[0].width,
				(uint32_t)this->mipmapInfos[0].height,
				(uint32_t)this->mipmaps,
				commandPool,
				(void*)imageData.data(), 
				imageData.size() * sizeof(componentType), 
				copyRegions
				);
			}

		uint32_t hashUint( uint32_t x )
			{
			x = ( ( x >> 16 ) ^ x ) * 0x45d9f3bU;
			x = ( ( x >> 16 ) ^ x ) * 0x45d9f3bU;
			x = ( x >> 16 ) ^ x;
			return x;
			}

		// to unhash the above:
		//inline uint32_t unhashUint(uint32_t x) {
		//    x = ((x >> 16) ^ x) * 0x119de1f3U;
		//    x = ((x >> 16) ^ x) * 0x119de1f3U;
		//    x = (x >> 16) ^ x;
		//    return x;
		//}

		void MakeLayer( size_t layerIndex )
			{
			const size_t depth = mipmapInfos[0].depth;

			// create the hashed corner values 
			glm::vec3 cubeCornerColors[8] = {};
			for( uint32_t inx = 0; inx < 8; ++inx )
				{
				uint32_t hashed = hashUint( (uint32_t)( layerIndex << 3 ) + inx );
				cubeCornerColors[inx].x = float( ( hashed >> 0 ) & 0x3ff ) / 1023.f;
				cubeCornerColors[inx].y = float( ( hashed >> 10 ) & 0x3ff ) / 1023.f;
				cubeCornerColors[inx].z = float( ( hashed >> 20 ) & 0x3ff ) / 1023.f;
				}

			// make all the 2D images in the one 3D image
			for( size_t z = 0; z < depth; ++z )
				{
				const float alpha = ( depth > 1 ) ? ( float( z ) / float( depth - 1 ) ) : 0.f; // avoid division by zero if depth == 1
				const glm::vec3 squareCornerColors[4] = {
					( cubeCornerColors[0] * ( 1.f - alpha ) + cubeCornerColors[1] * alpha ),
					( cubeCornerColors[2] * ( 1.f - alpha ) + cubeCornerColors[3] * alpha ),
					( cubeCornerColors[4] * ( 1.f - alpha ) + cubeCornerColors[5] * alpha ),
					( cubeCornerColors[6] * ( 1.f - alpha ) + cubeCornerColors[7] * alpha ),
					};

				this->Make2DImage( z, layerIndex, squareCornerColors );
				}

			// generate mip maps for the image
			this->MakeMipMaps( layerIndex );
			}

		void Make2DImage( size_t z, size_t layerIndex, const glm::vec3 squareCornerColors[4] )
			{
			const size_t width = mipmapInfos[0].width;
			const size_t height = mipmapInfos[0].height;
			const size_t layerOffset = layerIndex * this->layerSize;
			const size_t imageOffset = layerOffset + ( z * height * width * tupleSize );

			// draw the interpolated color
			for( size_t y = 0; y < height; ++y )
				{
				const size_t lineOffset = imageOffset + ( y * width * tupleSize );

				const float lineAlpha = ( height > 1 ) ? ( float( y ) / float( height - 1 ) ) : 0.f; // avoid division by zero if height == 1
				const glm::vec3 lineColors[2] = {
					( squareCornerColors[0] * ( 1.f - lineAlpha ) + squareCornerColors[1] * lineAlpha ),
					( squareCornerColors[2] * ( 1.f - lineAlpha ) + squareCornerColors[3] * lineAlpha ),
					};

				const glm::vec3 stepper = ( width > 1 ) ? ( lineColors[1] - lineColors[0] ) / float( width - 1 ) : glm::vec3( 0 );

				glm::vec3 val = lineColors[0];
				size_t pos = lineOffset;
				for( size_t x = 0; x < width; ++x )
					{
					imageData[pos] = getComponentValue( val.x );
					if constexpr ( tupleSize >= 2 )
						imageData[pos + 1] = getComponentValue( val.y );
					if constexpr ( tupleSize >= 3 )
						imageData[pos + 2] = getComponentValue( val.z );
					if constexpr ( tupleSize >= 4 )
						imageData[pos + 3] = getComponentValue( 1.f );
					val += stepper;
					pos += tupleSize;
					}
				}

			}

		// simple nearest-neighbor mip map
		void MakeMipMaps( size_t layerIndex )
			{
			const size_t layerOffset = layerIndex * this->layerSize;
			
			for( size_t mmap = 1; mmap < this->mipmaps; ++mmap )
				{
				const auto &src = this->mipmapInfos[mmap-1];
				const auto &dest = this->mipmapInfos[mmap];

				const size_t srcMapOffset = src.offset + layerOffset;
				size_t destPos = dest.offset + layerOffset;

				for( size_t destZ=0; destZ<dest.depth; ++destZ )
					{
					const size_t srcZ = destZ*2;
					const size_t srcPlaneOffset = (srcZ * src.height * src.width * tupleSize) + srcMapOffset;

					for( size_t destY=0; destY<dest.height; ++destY )
						{
						const size_t srcY = destY*2;

						size_t srcPos = (srcY * src.width * tupleSize) + srcPlaneOffset;
						for( size_t destX=0; destX<dest.width; ++destX )
							{
							for( size_t c=0; c<tupleSize; ++c )
								{
								this->imageData[destPos + c] = this->imageData[srcPos + c];
								}

							srcPos += 2 * tupleSize;
							destPos += tupleSize;
							}
						}
					}
				}
			}

	};
