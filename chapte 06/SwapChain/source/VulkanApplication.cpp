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

#include "VulkanApplication.h"

std::unique_ptr<VulkanApplication> VulkanApplication::instance;
std::once_flag VulkanApplication::onlyOnce;

extern std::vector<const char *> instanceExtensionNames;
extern std::vector<const char *> layerNames;
extern std::vector<const char *> deviceExtensionNames;


// Application constructor responsible for layer enumeration.
VulkanApplication::VulkanApplication() 
{
	// At application start up, enumerate instance layers
	instanceObj.layerExtension.getInstanceLayerProperties();

	deviceObj = NULL;
	debugFlag = false;
	rendererObj = NULL;
}

VulkanApplication::~VulkanApplication()
{
	delete rendererObj;
	rendererObj = NULL;
}

// Returns the Single ton object of VulkanApplication
VulkanApplication* VulkanApplication::GetInstance(){
    std::call_once(onlyOnce, [](){instance.reset(new VulkanApplication()); });
    return instance.get();
}

VkResult VulkanApplication::createVulkanInstance( std::vector<const char *>& layers, std::vector<const char *>& extensionNames, const char* applicationName)
{
	return instanceObj.createInstance(layers, extensionNames, applicationName);
}

// This function is responsible for creating the logical device.
// The process of logical device creation requires the following steps:-
// 1. Get the physical device specific layer and corresponding extensions [Optional]
// 2. Create user define VulkanDevice object
// 3. Provide the list of layer and extension that needs to enabled in this physical device
// 4. Get the physical device or GPU properties
// 5. Get the memory properties from the physical device or GPU
// 6. Query the physical device exposed queues and related properties
// 7. Get the handle of graphics queue
// 8. Create the logical device, connect it to the graphics queue.

// High level function for creating device and queues
VkResult VulkanApplication::handShakeWithDevice(VkPhysicalDevice* gpu, std::vector<const char *>& layers, std::vector<const char *>& extensions )
{

	// The user define Vulkan Device object this will manage the
	// Physical and logical device and their queue and properties
	deviceObj = new VulkanDevice(gpu);
	if (!deviceObj){
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}
	
	// Print the devices available layer and their extension 
	deviceObj->layerExtension.getDeviceExtensionProperties(gpu);

	// Get the physical device or GPU properties
	vkGetPhysicalDeviceProperties(*gpu, &deviceObj->gpuProps);

	// Get the memory properties from the physical device or GPU.
	vkGetPhysicalDeviceMemoryProperties(*gpu, &deviceObj->memoryProperties);

	// Query the availabe queues on the physical device and their properties.
	deviceObj->getPhysicalDeviceQueuesAndProperties();

	// Retrive the queue which support graphics pipeline.
	deviceObj->getGrahicsQueueHandle();

	// Create Logical Device, ensure that this device is connecte to graphics queue
	return deviceObj->createDevice(layers, extensions);
}

VkResult VulkanApplication::enumeratePhysicalDevices(std::vector<VkPhysicalDevice>& gpuList)
{
	uint32_t gpuDeviceCount;

	VkResult result = vkEnumeratePhysicalDevices(instanceObj.instance, &gpuDeviceCount, NULL);
	assert(result == VK_SUCCESS);

	assert(gpuDeviceCount);

	gpuList.resize(gpuDeviceCount);

	result = vkEnumeratePhysicalDevices(instanceObj.instance, &gpuDeviceCount, gpuList.data());
	assert(result == VK_SUCCESS);

	return result;
}

void VulkanApplication::initialize()
{
	char title[] = "Hello World!!!";

	// Check if the supplied layer are support or not
	if (debugFlag) {
		instanceObj.layerExtension.areLayersSupported(layerNames);
	}

	// Create the Vulkan instance with specified layer and extension names.
	createVulkanInstance(layerNames, instanceExtensionNames, title);

	// Create the debugging report if debugging is enabled
	if (debugFlag) {
		instanceObj.layerExtension.createDebugReportCallback();
	}

	// Get the list of physical devices on the system
	std::vector<VkPhysicalDevice> gpuList;
	enumeratePhysicalDevices(gpuList);

	// This example use only one device which is available first.
	if (gpuList.size() > 0) {
		handShakeWithDevice(&gpuList[0], layerNames, deviceExtensionNames);
	}

	rendererObj = new VulkanRenderer(this, deviceObj);

	rendererObj->initialize();
}

void VulkanApplication::deInitialize()
{
	rendererObj->destroyDepthBuffer();
	rendererObj->getSwapChain()->destroySwapChain();
	rendererObj->destroyCommandBuffer();
	rendererObj->destroyCommandPool();
	rendererObj->destroyPresentationWindow();
	deviceObj->destroyDevice();
	if (debugFlag) {
		instanceObj.layerExtension.destroyDebugReportCallback();
	}
	instanceObj.destroyInstance();
}

void VulkanApplication::prepare()
{
	// Place holder, this will be utilized in the upcoming chapters
}

void VulkanApplication::update()
{
	// Place holder, this will be utilized in the upcoming chapters
}

bool VulkanApplication::render() 
{
	return rendererObj->render();
}


