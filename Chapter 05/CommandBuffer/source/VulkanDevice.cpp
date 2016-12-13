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

#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanApplication.h"

VulkanDevice::VulkanDevice(VkPhysicalDevice* physicalDevice) 
{
	gpu = physicalDevice;
}

VulkanDevice::~VulkanDevice() 
{
}

// Note: This function requires queue object to be in existence before
VkResult VulkanDevice::createDevice(std::vector<const char *>& layers, std::vector<const char *>& extensions)
{
	layerExtension.appRequestedLayerNames		= layers;
	layerExtension.appRequestedExtensionNames	= extensions;

	// Create Device with available queue information.

	VkResult result;
	float queuePriorities[1]			= { 0.0 };
	VkDeviceQueueCreateInfo queueInfo	= {};
	queueInfo.queueFamilyIndex			= graphicsQueueIndex;  
	queueInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pNext						= NULL;
	queueInfo.queueCount				= 1;
	queueInfo.pQueuePriorities			= queuePriorities;

	VkDeviceCreateInfo deviceInfo		= {};
	deviceInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext					= NULL;
	deviceInfo.queueCreateInfoCount		= 1;
	deviceInfo.pQueueCreateInfos		= &queueInfo;
	deviceInfo.enabledLayerCount		= 0;
	deviceInfo.ppEnabledLayerNames		= NULL;											// Device layers are deprecated
	deviceInfo.enabledExtensionCount	= (uint32_t)extensions.size();
	deviceInfo.ppEnabledExtensionNames	= extensions.size() ? extensions.data() : NULL;
	deviceInfo.pEnabledFeatures			= NULL;

	result = vkCreateDevice(*gpu, &deviceInfo, NULL, &device);
	assert(result == VK_SUCCESS);

	return result;
}

void VulkanDevice::getPhysicalDeviceQueuesAndProperties()
{
	// Query queue families count with pass NULL as second parameter.
	vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, NULL);
	
	// Allocate space to accomodate Queue properties.
	queueFamilyProps.resize(queueFamilyCount);

	// Get queue family properties
	vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, queueFamilyProps.data());
}

uint32_t VulkanDevice::getGrahicsQueueHandle()
{
	//	1. Get the number of Queues supported by the Physical device
	//	2. Get the properties each Queue type or Queue Family
	//			There could be 4 Queue type or Queue families supported by physical device - 
	//			Graphics Queue	- VK_QUEUE_GRAPHICS_BIT 
	//			Compute Queue	- VK_QUEUE_COMPUTE_BIT
	//			DMA				- VK_QUEUE_TRANSFER_BIT
	//			Spare memory	- VK_QUEUE_SPARSE_BINDING_BIT
	//	3. Get the index ID for the required Queue family, this ID will act like a handle index to queue.

	bool found = false;
	// 1. Iterate number of Queues supported by the Physical device
	for (unsigned int i = 0; i < queueFamilyCount; i++){
		// 2. Get the Graphics Queue type
		//		There could be 4 Queue type or Queue families supported by physical device - 
		//		Graphics Queue		- VK_QUEUE_GRAPHICS_BIT 
		//		Compute Queue		- VK_QUEUE_COMPUTE_BIT
		//		DMA/Transfer Queue	- VK_QUEUE_TRANSFER_BIT
		//		Spare memory		- VK_QUEUE_SPARSE_BINDING_BIT

		if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			// 3. Get the handle/index ID of graphics queue family.
			found				= true;
			graphicsQueueIndex	= i;
			break;
		}
	}

	// Assert if there is no queue found.
	assert(found);

	return 0;
}

void VulkanDevice::destroyDevice()
{
	vkDestroyDevice(device, NULL);
}
