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

#include "VulkanDrawable.h"

#include "VulkanApplication.h"
#include "VulkanRenderer.h"

VulkanDrawable::VulkanDrawable(VulkanRenderer* parent) {
	// Note: It's very important to initilize the member with 0 or respective value other wise it will break the system
	memset(&VertexBuffer, 0, sizeof(VertexBuffer));
	rendererObj = parent;
}

VulkanDrawable::~VulkanDrawable()
{
}

void VulkanDrawable::createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture)
{
	VulkanApplication* appObj	= VulkanApplication::GetInstance();
	VulkanDevice* deviceObj		= appObj->deviceObj;

	VkResult  result;
	bool  pass;

	// Create the Buffer resourece metadata information
	VkBufferCreateInfo bufInfo		= {};
	bufInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufInfo.pNext					= NULL;
	bufInfo.usage					= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufInfo.size					= dataSize;
	bufInfo.queueFamilyIndexCount	= 0;
	bufInfo.pQueueFamilyIndices	= NULL;
	bufInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	bufInfo.flags					= 0;

	// Create the Buffer resource
	result = vkCreateBuffer(deviceObj->device, &bufInfo, NULL, &VertexBuffer.buf);
	assert(result == VK_SUCCESS);

	// Get the Buffer resource requirements
	VkMemoryRequirements memRqrmnt;
	vkGetBufferMemoryRequirements(deviceObj->device, VertexBuffer.buf, &memRqrmnt);

	// Create memory allocation metadata information
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext				= NULL;
	allocInfo.memoryTypeIndex	= 0;
	allocInfo.allocationSize	= memRqrmnt.size;

	// Get the compatible type of memory
	pass = deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex);
	assert(pass);

	// Allocate the physical backing for buffer resource
	result = vkAllocateMemory(deviceObj->device, &allocInfo, NULL, &(VertexBuffer.mem));
	assert(result == VK_SUCCESS);
	VertexBuffer.bufferInfo.range	= memRqrmnt.size;
	VertexBuffer.bufferInfo.offset	= 0;

	// Map the physical device memory region to the host 
	uint8_t *pData;
	result = vkMapMemory(deviceObj->device, VertexBuffer.mem, 0, memRqrmnt.size, 0, (void **)&pData);
	assert(result == VK_SUCCESS);

	// Copy the data in the mapped memory
	memcpy(pData, vertexData, dataSize);

	// Unmap the device memory
	vkUnmapMemory(deviceObj->device, VertexBuffer.mem);

	// Bind the allocated buffer resource to the device memory
	result = vkBindBufferMemory(deviceObj->device, VertexBuffer.buf, VertexBuffer.mem, 0);
	assert(result == VK_SUCCESS);

	// Once the buffer resource is implemented, its binding points are 
	// stored into the(
	// The VkVertexInputBinding viIpBind, stores the rate at which the information will be
	// injected for vertex input.
	viIpBind.binding		= 0;
	viIpBind.inputRate		= VK_VERTEX_INPUT_RATE_VERTEX;
	viIpBind.stride			= dataStride;

	// The VkVertexInputAttribute - Description) structure, store 
	// the information that helps in interpreting the data.
	viIpAttrb[0].binding	= 0;
	viIpAttrb[0].location	= 0;
	viIpAttrb[0].format		= VK_FORMAT_R32G32B32A32_SFLOAT;
	viIpAttrb[0].offset		= 0;
	viIpAttrb[1].binding	= 0;
	viIpAttrb[1].location	= 1;
	viIpAttrb[1].format		= useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
	viIpAttrb[1].offset		= 16; // After, 4 components - RGBA  each of 4 bytes(32bits)

}

void VulkanDrawable::destroyVertexBuffer()
{
	VulkanDevice* deviceObj = rendererObj->getDevice();

	vkDestroyBuffer(deviceObj->device, VertexBuffer.buf, NULL);
	vkFreeMemory(deviceObj->device, VertexBuffer.mem, NULL);
}

