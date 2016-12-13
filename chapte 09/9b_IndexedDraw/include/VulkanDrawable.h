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
class VulkanRenderer;

class VulkanDrawable
{
public:
	VulkanDrawable(VulkanRenderer* parent = 0);
	~VulkanDrawable();

	void createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture);
	void createVertexIndex(const void *indexData, uint32_t dataSize, uint32_t dataStride);
	void prepare();
	void render();
	void setPipeline(VkPipeline* vulkanPipeline) { pipeline = vulkanPipeline; }
	VkPipeline* getPipeline() { return pipeline; }

	void initViewports(VkCommandBuffer* cmd);
	void initScissors(VkCommandBuffer* cmd);

	void destroyVertexBuffer();
	void destroyVertexIndex();
	void destroyCommandBuffer();
	void destroySynchronizationObjects();

public:
	// Structure storing vertex buffer metadata
	struct {
		VkBuffer buf;
		VkDeviceMemory mem;
		VkDescriptorBufferInfo bufferInfo;
	} VertexBuffer;

	struct {
		VkBuffer idx;
		VkDeviceMemory mem;
		VkDescriptorBufferInfo bufferInfo;
	} VertexIndex;

	// Stores the vertex input rate
	VkVertexInputBindingDescription		viIpBind;
	// Store metadata helpful in data interpretation
	VkVertexInputAttributeDescription	viIpAttrb[2];

private:
	std::vector<VkCommandBuffer> vecCmdDraw;			// Command buffer for drawing
	void recordCommandBuffer(int currentImage, VkCommandBuffer* cmdDraw);
	VkViewport viewport;
	VkRect2D   scissor;
	VkSemaphore presentCompleteSemaphore;
	VkSemaphore drawingCompleteSemaphore;

	VulkanRenderer* rendererObj;
	VkPipeline*		pipeline;
};