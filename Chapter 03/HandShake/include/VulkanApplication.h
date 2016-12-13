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
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanLED.h"

class VulkanApplication
{
private:
	// CTOR: Application constructor responsible for layer enumeration.
	VulkanApplication();

public:
	// DTOR
	~VulkanApplication();

private:
	// Variable for Single Ton implementation
    static std::unique_ptr<VulkanApplication> instance;
    static std::once_flag onlyOnce;

public:
    static VulkanApplication* GetInstance();

	// Simple program life cycle
	void initialize();				// Initialize and allocate resources
	void prepare();					// Prepare resource
	void update();					// Update data
	bool render();					// Render primitives
	void deInitialize();			// Release resources

private:
	// Create the Vulkan instance object
	VkResult createVulkanInstance(std::vector<const char *>& layers, std::vector<const char *>& extensions,  const char* applicationName);
	VkResult handShakeWithDevice(VkPhysicalDevice* gpu, std::vector<const char *>& layers, std::vector<const char *>& extensions);
	VkResult enumeratePhysicalDevices(std::vector<VkPhysicalDevice>& gpus);

public:
    VulkanInstance  instanceObj;	// Vulkan Instance object

	VulkanDevice*      deviceObj;
};