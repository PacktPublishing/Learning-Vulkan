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

#include "Wrappers.h"

void CommandBufferMgr::allocCommandBuffer(const VkDevice* device, const VkCommandPool cmdPool, VkCommandBuffer* cmdBuf, const VkCommandBufferAllocateInfo* commandBufferInfo)
{
	// Dependency on the intialize SwapChain Extensions and initialize CommandPool
	VkResult result;

	// If command information is available use it as it is.
	if (commandBufferInfo) {
		result = vkAllocateCommandBuffers(*device, commandBufferInfo, cmdBuf);
		assert(!result);
		return;
	}

	// Default implementation, create the command buffer
	// allocation info and use the supplied parameter into it
	VkCommandBufferAllocateInfo cmdInfo = {};
	cmdInfo.sType		= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdInfo.pNext		= NULL;
	cmdInfo.commandPool = cmdPool;
	cmdInfo.level		= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdInfo.commandBufferCount = (uint32_t) sizeof(cmdBuf) / sizeof(VkCommandBuffer);;

	result = vkAllocateCommandBuffers(*device, &cmdInfo, cmdBuf);
	assert(!result);
}

void CommandBufferMgr::beginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferBeginInfo* inCmdBufInfo)
{
	// Dependency on  the initialieCommandBuffer()
	VkResult  result;
	// If the user has specified the custom command buffer use it
	if (inCmdBufInfo) {
		result = vkBeginCommandBuffer(cmdBuf, inCmdBufInfo);
		assert(result == VK_SUCCESS);
		return;
	}

	// Otherwise, use the default implementation.
	VkCommandBufferInheritanceInfo cmdBufInheritInfo = {};
	cmdBufInheritInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	cmdBufInheritInfo.pNext					= NULL;
	cmdBufInheritInfo.renderPass			= VK_NULL_HANDLE;
	cmdBufInheritInfo.subpass				= 0;
	cmdBufInheritInfo.framebuffer			= VK_NULL_HANDLE;
	cmdBufInheritInfo.occlusionQueryEnable	= VK_FALSE;
	cmdBufInheritInfo.queryFlags			= 0;
	cmdBufInheritInfo.pipelineStatistics	= 0;
	
	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufInfo.pNext				= NULL;
	cmdBufInfo.flags				= 0;
	cmdBufInfo.pInheritanceInfo		= &cmdBufInheritInfo;

	result = vkBeginCommandBuffer(cmdBuf, &cmdBufInfo);

	assert(result == VK_SUCCESS);
}

void CommandBufferMgr::endCommandBuffer(VkCommandBuffer commandBuffer)
{
	VkResult  result;
	result = vkEndCommandBuffer(commandBuffer);
	assert(result == VK_SUCCESS);
}

void CommandBufferMgr::submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer* commandBuffer, const VkSubmitInfo* inSubmitInfo, const VkFence& fence)
{
	VkResult result;
	
	// If Subimt information is avialable use it as it is, this assumes that 
	// the commands are already specified in the structure, hence ignore command buffer 
	if (inSubmitInfo){
		result = vkQueueSubmit(queue, 1, inSubmitInfo, fence);
		assert(!result);

		result = vkQueueWaitIdle(queue);
		assert(!result);
		return;
	}

	// Otherwise, create the submit information with specified buffer commands
	VkSubmitInfo submitInfo = {};
	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext				= NULL;
	submitInfo.waitSemaphoreCount	= 0;
	submitInfo.pWaitSemaphores		= NULL;
	submitInfo.pWaitDstStageMask	= NULL;
	submitInfo.commandBufferCount	= (uint32_t)sizeof(commandBuffer)/sizeof(VkCommandBuffer);
	submitInfo.pCommandBuffers		= commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores	= NULL;

	result = vkQueueSubmit(queue, 1, &submitInfo, fence);
	assert(!result);

	result = vkQueueWaitIdle(queue);
	assert(!result);
}

void* readFile(const char *spvFileName, size_t *fileSize) {

	FILE *fp = fopen(spvFileName, "rb");
	if (!fp) {
		return NULL;
	}

	size_t retval;
	long int size;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	void* spvShader = malloc(size+1); // Plus for NULL character '\0'
	memset(spvShader, 0, size+1);

	retval = fread(spvShader, size, 1, fp);
	assert(retval == 1);

	*fileSize = size;
	fclose(fp);
	return spvShader;
}

