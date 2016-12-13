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

#include "VulkanSwapChain.h"

#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanRenderer.h"
#include "VulkanApplication.h"

#define INSTANCE_FUNC_PTR(instance, entrypoint){											\
    fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(instance, "vk"#entrypoint); \
    if (fp##entrypoint == NULL) {															\
        std::cout << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;				\
        exit(-1);																			\
    }																						\
}

#define DEVICE_FUNC_PTR(dev, entrypoint){													\
    fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);		\
    if (fp##entrypoint == NULL) {															\
        std::cout << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;				\
        exit(-1);																			\
    }																						\
}

VulkanSwapChain::VulkanSwapChain(VulkanRenderer* renderer)
{
	rendererObj = renderer;
	appObj		= VulkanApplication::GetInstance();
	scPublicVars.swapChain = VK_NULL_HANDLE;
}

VulkanSwapChain::~VulkanSwapChain()
{
	scPrivateVars.swapchainImages.clear();
	scPrivateVars.surfFormats.clear();
	scPrivateVars.presentModes.clear();
}

VkResult VulkanSwapChain::createSwapChainExtensions()
{
	// Dependency on createPresentationWindow()
	VkInstance& instance	= appObj->instanceObj.instance;
	VkDevice& device		= appObj->deviceObj->device;

	// Get Instance based swap chain extension function pointer
	INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceSupportKHR);
	INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
	INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
	INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
	INSTANCE_FUNC_PTR(instance, DestroySurfaceKHR);

	// Get Device based swap chain extension function pointer
	DEVICE_FUNC_PTR(device, CreateSwapchainKHR);
	DEVICE_FUNC_PTR(device, DestroySwapchainKHR);
	DEVICE_FUNC_PTR(device, GetSwapchainImagesKHR);
	DEVICE_FUNC_PTR(device, AcquireNextImageKHR);
	DEVICE_FUNC_PTR(device, QueuePresentKHR);

	return VK_SUCCESS;
}

VkResult VulkanSwapChain::createSurface()
{
	VkResult  result;
	// Depends on createPresentationWindow(), need an empty window handle
	VkInstance& instance = appObj->instanceObj.instance;

	// Construct the surface description:
#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext		= NULL;
	createInfo.hinstance	= rendererObj->connection;
	createInfo.hwnd			= rendererObj->window;

	result = vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &scPublicVars.surface);

#else  // _WIN32

	VkXcbSurfaceCreateInfoKHR createInfo = {};
	createInfo.sType		= VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext		= NULL;
	createInfo.connection	= rendererObj->connection;
	createInfo.window		= rendererObj->window;

	result = vkCreateXcbSurfaceKHR(instance, &createInfo, NULL, &surface);
#endif // _WIN32
	
	assert(result == VK_SUCCESS);
	return result;
}

uint32_t VulkanSwapChain::getGraphicsQueueWithPresentationSupport()
{
	VulkanDevice* device	= appObj->deviceObj;
	uint32_t queueCount		= device->queueFamilyCount;
	VkPhysicalDevice gpu	= *device->gpu;
	std::vector<VkQueueFamilyProperties>& queueProps = device->queueFamilyProps;

	// Iterate over each queue and get presentation status for each.
	VkBool32* supportsPresent = (VkBool32 *)malloc(queueCount * sizeof(VkBool32));
	for (uint32_t i = 0; i < queueCount; i++) {
		fpGetPhysicalDeviceSurfaceSupportKHR(gpu, i, scPublicVars.surface, &supportsPresent[i]);
	}

	 // Search for a graphics queue and a present queue in the array of queue
	 // families, try to find one that supports both
	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < queueCount; i++) {
		if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
			if (graphicsQueueNodeIndex == UINT32_MAX) {
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE) {
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	
	if (presentQueueNodeIndex == UINT32_MAX) {
		// If didn't find a queue that supports both graphics and present, then
		// find a separate present queue.
		for (uint32_t i = 0; i < queueCount; ++i) {
			if (supportsPresent[i] == VK_TRUE) {
				presentQueueNodeIndex = i;
				break;
			}
		}
	}

	free(supportsPresent);

	// Generate error if could not find both a graphics and a present queue
	if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
		return  UINT32_MAX;
	}

	return graphicsQueueNodeIndex;
}

void VulkanSwapChain::getSupportedFormats()
{
	VkPhysicalDevice gpu = *rendererObj->getDevice()->gpu;
	VkResult  result;

	// Get the list of VkFormats that are supported:
	uint32_t formatCount;
	result = fpGetPhysicalDeviceSurfaceFormatsKHR(gpu, scPublicVars.surface, &formatCount, NULL);
	assert(result == VK_SUCCESS);
	scPrivateVars.surfFormats.clear();
	scPrivateVars.surfFormats.resize(formatCount);

	// Get VkFormats in allocated objects
	result = fpGetPhysicalDeviceSurfaceFormatsKHR(gpu, scPublicVars.surface, &formatCount, &scPrivateVars.surfFormats[0]);
	assert(result == VK_SUCCESS);

	// In case it’s a VK_FORMAT_UNDEFINED, then surface has no 
	// preferred format. We use BGRA 32 bit format
	if (formatCount == 1 && scPrivateVars.surfFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		scPublicVars.format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		assert(formatCount >= 1);
		scPublicVars.format = scPrivateVars.surfFormats[0].format;
	}
}

void VulkanSwapChain::intializeSwapChain()
{
	// Querying swapchain extensions
	createSwapChainExtensions();

	// Create surface and associate with Windowing
	createSurface();

	// Getting a graphics queue with presentation support
	uint32_t index = getGraphicsQueueWithPresentationSupport();
	if (index == UINT32_MAX)
	{
		std::cout << "Could not find a graphics and a present queue\nCould not find a graphics and a present queue\n";
		exit(-1);
	}
	rendererObj->getDevice()->graphicsQueueWithPresentIndex = index;

	// Get the list of formats that are supported
	getSupportedFormats();
}

void VulkanSwapChain::createSwapChain(const VkCommandBuffer& cmd)
{
	// use extensions and get the surface capabilities, present mode
	getSurfaceCapabilitiesAndPresentMode();

	// Gather necessary information for present mode.
	managePresentMode();

	// Create the swap chain images
	createSwapChainColorImages();

	// Get the create color image drawing surfaces
	createColorImageView(cmd);
}

void VulkanSwapChain::getSurfaceCapabilitiesAndPresentMode()
{
	VkResult  result;
	VkPhysicalDevice gpu = *appObj->deviceObj->gpu;
	result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, scPublicVars.surface, &scPrivateVars.surfCapabilities);
	assert(result == VK_SUCCESS);

	result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, scPublicVars.surface, &scPrivateVars.presentModeCount, NULL);
	assert(result == VK_SUCCESS);

	scPrivateVars.presentModes.clear();
	scPrivateVars.presentModes.resize(scPrivateVars.presentModeCount);
	assert(scPrivateVars.presentModes.size()>=1);

	result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, scPublicVars.surface, &scPrivateVars.presentModeCount, &scPrivateVars.presentModes[0]);
	assert(result == VK_SUCCESS);

	if (scPrivateVars.surfCapabilities.currentExtent.width == (uint32_t)-1)
	{
		// If the surface width and height is not defined, the set the equal to image size.
		scPrivateVars.swapChainExtent.width = rendererObj->width;
		scPrivateVars.swapChainExtent.height = rendererObj->height;
	}
	else
	{
		// If the surface size is defined, then it must match the swap chain size
		scPrivateVars.swapChainExtent = scPrivateVars.surfCapabilities.currentExtent;
	}
}

void VulkanSwapChain::managePresentMode()
{
	// If mailbox mode is available, use it, as is the lowest-latency non-
	// tearing mode.  If not, try IMMEDIATE which will usually be available,
	// and is fastest (though it tears).  If not, fall back to FIFO which is
	// always available.
	scPrivateVars.swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (size_t i = 0; i < scPrivateVars.presentModeCount; i++) {
		if (scPrivateVars.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			scPrivateVars.swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((scPrivateVars.swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
			(scPrivateVars.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
			scPrivateVars.swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	// Determine the number of VkImage's to use in the swap chain (we desire to
	// own only 1 image at a time, besides the images being displayed and
	// queued for display):
	scPrivateVars.desiredNumberOfSwapChainImages = scPrivateVars.surfCapabilities.minImageCount + 1;
	if ((scPrivateVars.surfCapabilities.maxImageCount > 0) &&
		(scPrivateVars.desiredNumberOfSwapChainImages > scPrivateVars.surfCapabilities.maxImageCount))
	{
		// Application must settle for fewer images than desired:
		scPrivateVars.desiredNumberOfSwapChainImages = scPrivateVars.surfCapabilities.maxImageCount;
	}

	if (scPrivateVars.surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		scPrivateVars.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		scPrivateVars.preTransform = scPrivateVars.surfCapabilities.currentTransform;
	}
}

void VulkanSwapChain::createSwapChainColorImages()
{
	VkResult  result;
	VkSwapchainKHR oldSwapchain = scPublicVars.swapChain;

	VkSwapchainCreateInfoKHR swapChainInfo = {};
	swapChainInfo.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainInfo.pNext					= NULL;
	swapChainInfo.surface				= scPublicVars.surface;
	swapChainInfo.minImageCount			= scPrivateVars.desiredNumberOfSwapChainImages;
	swapChainInfo.imageFormat			= scPublicVars.format;
	swapChainInfo.imageExtent.width		= scPrivateVars.swapChainExtent.width;
	swapChainInfo.imageExtent.height	= scPrivateVars.swapChainExtent.height;
	swapChainInfo.preTransform			= scPrivateVars.preTransform;
	swapChainInfo.compositeAlpha		= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainInfo.imageArrayLayers		= 1;
	swapChainInfo.presentMode			= scPrivateVars.swapchainPresentMode;
	swapChainInfo.oldSwapchain			= oldSwapchain;
	swapChainInfo.clipped				= true;
	swapChainInfo.imageColorSpace		= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapChainInfo.imageUsage			= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapChainInfo.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	swapChainInfo.queueFamilyIndexCount = 0;
	swapChainInfo.pQueueFamilyIndices	= NULL;

	result = fpCreateSwapchainKHR(rendererObj->getDevice()->device, &swapChainInfo, NULL, &scPublicVars.swapChain);
	assert(result == VK_SUCCESS);

	// Create the swapchain object
	result = fpGetSwapchainImagesKHR(rendererObj->getDevice()->device, scPublicVars.swapChain, &scPublicVars.swapchainImageCount, NULL);
	assert(result == VK_SUCCESS);

	scPrivateVars.swapchainImages.clear();
	// Get the number of images the swapchain has
	scPrivateVars.swapchainImages.resize(scPublicVars.swapchainImageCount);
	assert(scPrivateVars.swapchainImages.size() >= 1);

	// Retrieve the swapchain image surfaces 
	result = fpGetSwapchainImagesKHR(rendererObj->getDevice()->device, scPublicVars.swapChain, &scPublicVars.swapchainImageCount, &scPrivateVars.swapchainImages[0]);
	assert(result == VK_SUCCESS);

	if (oldSwapchain != VK_NULL_HANDLE) {
		fpDestroySwapchainKHR(rendererObj->getDevice()->device, oldSwapchain, NULL);
	}
}

void VulkanSwapChain::createColorImageView(const VkCommandBuffer& cmd)
{
	VkResult  result;
	scPublicVars.colorBuffer.clear();
	for (uint32_t i = 0; i < scPublicVars.swapchainImageCount; i++) {
		SwapChainBuffer sc_buffer;

		VkImageViewCreateInfo imgViewInfo = {};
		imgViewInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgViewInfo.pNext			= NULL;
		imgViewInfo.format			= scPublicVars.format;
		imgViewInfo.components		= { VK_COMPONENT_SWIZZLE_IDENTITY };
		imgViewInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		imgViewInfo.subresourceRange.baseMipLevel	= 0;
		imgViewInfo.subresourceRange.levelCount		= 1;
		imgViewInfo.subresourceRange.baseArrayLayer	= 0;
		imgViewInfo.subresourceRange.layerCount		= 1;
		imgViewInfo.viewType						= VK_IMAGE_VIEW_TYPE_2D;
		imgViewInfo.flags							= 0;

		sc_buffer.image = scPrivateVars.swapchainImages[i];

		// Since the swapchain is not owned by us we cannot set the image layout
		// upon setting the implementation may give error, the images layout were
		// create by the WSI implementation not by us. 

		imgViewInfo.image = sc_buffer.image;

		result = vkCreateImageView(rendererObj->getDevice()->device, &imgViewInfo, NULL, &sc_buffer.view);
		scPublicVars.colorBuffer.push_back(sc_buffer);
		assert(result == VK_SUCCESS);
	}
	scPublicVars.currentColorBuffer = 0;
}

void VulkanSwapChain::destroySwapChain()
{
	VulkanDevice* deviceObj = appObj->deviceObj;

	for (uint32_t i = 0; i < scPublicVars.swapchainImageCount; i++) {
		vkDestroyImageView(deviceObj->device, scPublicVars.colorBuffer[i].view, NULL);
	}
	
	if (!appObj->isResizing) {
		// This piece code will only executes at application shutdown.
        // During resize the old swapchain image is delete in createSwapChainColorImages()
		fpDestroySwapchainKHR(deviceObj->device, scPublicVars.swapChain, NULL);
		vkDestroySurfaceKHR(appObj->instanceObj.instance, scPublicVars.surface, NULL);
	}
}

void VulkanSwapChain::setSwapChainExtent(uint32_t swapChainWidth, uint32_t swapChainHeight)
{
	scPrivateVars.swapChainExtent.width = swapChainWidth;
	scPrivateVars.swapChainExtent.height = swapChainHeight;
}
