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
//#include "VulkanQueue.h"
#include "VulkanLED.h"

class VulkanApplication;


// Vulkan exposes one or more devices, each of which exposes one or more queues which may process 
// work asynchronously to one another.The queues supported by a device are divided into families, 
// each of which supports one or more types of functionality and may contain multiple queues with 
// similar characteristics.Queues within a single family are considered compatible with one another, 
// and work produced for a family of queues can be executed on any queue within that family
class VulkanDevice{
public:
	VulkanDevice(VkPhysicalDevice* gpu);
	~VulkanDevice();

public:
    VkDevice							device;		// Logical device
	VkPhysicalDevice*					gpu;		// Physical device
	VkPhysicalDeviceProperties			gpuProps;	// Physical device attributes
    VkPhysicalDeviceMemoryProperties	memoryProperties;

public:
	// Queue
	VkQueue									queue;							// Vulkan Queues object
	std::vector<VkQueueFamilyProperties>	queueFamilyProps;				// Store all queue families exposed by the physical device. attributes
	uint32_t								graphicsQueueIndex;				// Stores graphics queue index
	uint32_t								graphicsQueueWithPresentIndex;  // Number of queue family exposed by device
	uint32_t								queueFamilyCount;				// Device specificc layer and extensions

	// Layer and extensions
	VulkanLayerAndExtension		layerExtension;

public:
	VkResult createDevice(std::vector<const char *>& layers, std::vector<const char *>& extensions);
	void destroyDevice();

	bool memoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
	
	// Get the avaialbe queues exposed by the physical devices
	void getPhysicalDeviceQueuesAndProperties();

	// Query physical device to retrive queue properties
	uint32_t getGrahicsQueueHandle();

	// Queue related member functions.
	void getDeviceQueue();

};