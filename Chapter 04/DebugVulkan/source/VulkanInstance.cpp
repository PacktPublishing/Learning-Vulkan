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

#include "VulkanInstance.h"

VkResult VulkanInstance::createInstance(std::vector<const char *>& layers, std::vector<const char *>& extensionNames, char const*const appName)
{
	layerExtension.appRequestedExtensionNames	= extensionNames;
	layerExtension.appRequestedLayerNames		= layers;
	
	// Define the Vulkan application structure 
	VkApplicationInfo appInfo	= {};
	appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext				= NULL;
	appInfo.pApplicationName	= appName;
	appInfo.applicationVersion	= 1;
	appInfo.pEngineName			= appName;
	appInfo.engineVersion		= 1;
	// VK_API_VERSION is now deprecated, use VK_MAKE_VERSION instead.
	appInfo.apiVersion			= VK_MAKE_VERSION(1, 0, 0);

	// Define the Vulkan instance create info structure 
	VkInstanceCreateInfo instInfo	= {};
	instInfo.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext					= &layerExtension.dbgReportCreateInfo;
	instInfo.flags					= 0;
	instInfo.pApplicationInfo		= &appInfo;

	// Specify the list of layer name to be enabled.
	instInfo.enabledLayerCount		= (uint32_t)layers.size();
	instInfo.ppEnabledLayerNames	= layers.size() ? layers.data() : NULL;

	// Specify the list of extensions to be used in the application.
	instInfo.enabledExtensionCount	= (uint32_t)extensionNames.size();
	instInfo.ppEnabledExtensionNames = extensionNames.size() ? extensionNames.data() : NULL;

	VkResult result = vkCreateInstance(&instInfo, NULL, &instance);
	assert(result == VK_SUCCESS);

	return result;
}

void VulkanInstance::destroyInstance()
{
	vkDestroyInstance(instance, NULL);
}
