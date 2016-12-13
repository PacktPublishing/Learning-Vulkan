/*
* Learning Vulkan - ISBN: 9781786469809
*
* Author: Parminder Singh, parminder.vulkan@gmail.com
* Linkedin: https://www.linkedin.com/in/parmindersingh18
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include "Headers.h"

/***************COMMAND BUFFER WRAPPERS***************/
class CommandBufferMgr
{
public:
	static void allocCommandBuffer(const VkDevice* device, const VkCommandPool cmdPool, VkCommandBuffer* cmdBuf, const VkCommandBufferAllocateInfo* commandBufferInfo = NULL);
	static void beginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferBeginInfo* inCmdBufInfo = NULL);
	static void endCommandBuffer(VkCommandBuffer cmdBuf);
	static void submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer* cmdBufList, const VkSubmitInfo* submitInfo = NULL, const VkFence& fence = VK_NULL_HANDLE);
};

void* readFile(const char *spvFileName, size_t *fileSize);

/***************TEXTURE WRAPPERS***************/
struct TextureData{
	VkSampler				sampler;
	VkImage					image;
	VkImageLayout			imageLayout;
	VkMemoryAllocateInfo	memoryAlloc;
	VkDeviceMemory			mem;
	VkImageView				view;
	uint32_t				mipMapLevels;
	uint32_t				layerCount;
	uint32_t				textureWidth, textureHeight;
	VkDescriptorImageInfo	descsImgInfo;
};

/***************PPM PARSER CLASS***************/
#include "Headers.h"

class PpmParser
{
public:
	PpmParser();
	~PpmParser();
	bool getHeaderInfo(const char *filename);
	bool loadImageData(int rowPitch, uint8_t *rgba_data);
	int32_t getImageWidth();
	int32_t getImageHeight();
	const char* filename() { return ppmFile.c_str(); }

private:
	bool isValid;
	int32_t imageWidth;
	int32_t imageHeight;
	int32_t dataPosition;
	std::string ppmFile;
	gli::texture2D* tex2D;
};