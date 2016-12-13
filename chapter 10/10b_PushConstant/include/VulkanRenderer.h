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
#include "VulkanSwapChain.h"
#include "VulkanDrawable.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"

// Number of samples needs to be the same at image creation
// Used at renderpass creation (in attachment) and pipeline creation
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

// The Vulkan Renderer is custom class, it is not a Vulkan specific class.
// It works as a presentation manager.
// It manages the presentation windows and drawing surfaces.
class VulkanRenderer
{
public:
	VulkanRenderer(VulkanApplication* app, VulkanDevice* deviceObject);
	~VulkanRenderer();

public:
	//Simple life cycle
	void initialize();
	void prepare();
	void update();
	bool render();

	// Create an empty window
	void createPresentationWindow(const int& windowWidth = 500, const int& windowHeight = 500);
	void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer& cmdBuf);

	//! Windows procedure method for handling events.
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Destroy the presentation window
	void destroyPresentationWindow();

	// Getter functions for member variable specific to classes.
	inline VulkanApplication* getApplication()		{ return application; }
	inline VulkanDevice*  getDevice()				{ return deviceObj; }
	inline VulkanSwapChain*  getSwapChain() 		{ return swapChainObj; }
	inline std::vector<VulkanDrawable*>*  getDrawingItems() { return &drawableList; }
	inline VkCommandPool* getCommandPool()			{ return &cmdPool; }
	inline VulkanShader*  getShader()				{ return &shaderObj; }
	inline VulkanPipeline*	getPipelineObject()		{ return &pipelineObj; }

	void createCommandPool();							// Create command pool
	void buildSwapChainAndDepthImage();					// Create swapchain color image and depth image
	void createDepthImage();							// Create depth image
	void createVertexBuffer();
	void createRenderPass(bool includeDepth, bool clear = true);	// Render Pass creation
	void createFrameBuffer(bool includeDepth);
	void createShaders();
	void createPipelineStateManagement();
	void createDescriptors();
	void createPushConstants();

	void destroyCommandBuffer();
	void destroyCommandPool();
	void destroyDepthBuffer();
	void destroyDrawableVertexBuffer();
	void destroyRenderpass();										// Destroy the render pass object when no more required
	void destroyFramebuffers();
	void destroyPipeline();
	void destroyDrawableCommandBuffer();
	void destroyDrawableSynchronizationObjects();
	void destroyDrawableUniformBuffer();
public:
#ifdef _WIN32
#define APP_NAME_STR_LEN 80
	HINSTANCE					connection;				// hInstance - Windows Instance
	char						name[APP_NAME_STR_LEN]; // name - App name appearing on the window
	HWND						window;					// hWnd - the window handle
#else
	xcb_connection_t*			connection;
	xcb_screen_t*				screen;
	xcb_window_t				window;
	xcb_intern_atom_reply_t*	reply;
#endif

	struct{
		VkFormat		format;
		VkImage			image;
		VkDeviceMemory	mem;
		VkImageView		view;
	}Depth;

	VkCommandBuffer		cmdDepthImage;			// Command buffer for depth image layout
	VkCommandPool		cmdPool;				// Command pool
	VkCommandBuffer		cmdVertexBuffer;		// Command buffer for vertex buffer - Triangle geometry
	VkCommandBuffer		cmdPushConstant;		// Command buffer for push constants

	VkRenderPass		renderPass;				// Render pass created object
	std::vector<VkFramebuffer> framebuffers;	// Number of frame buffer corresponding to each swap chain
	std::vector<VkPipeline*> pipelineList;		// List of pipelines

	int					width, height;

private:
	VulkanApplication* application;
	// The device object associated with this Presentation layer.
	VulkanDevice*	   deviceObj;
	VulkanSwapChain*   swapChainObj;
	std::vector<VulkanDrawable*> drawableList;
	VulkanShader 	   shaderObj;
	VulkanPipeline 	   pipelineObj;
};