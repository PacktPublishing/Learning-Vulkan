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

class VulkanDevice;
class VulkanApplication;

class VulkanDescriptor
{
public:
	// Constructor
	VulkanDescriptor();
	// Destructor
	~VulkanDescriptor();

	// Creates descriptor pool and allocate descriptor set from it
	void createDescriptor(bool useTexture);
	// Deletes the created descriptor set object
	void destroyDescriptor();

	// Defines the sescriptor sets layout binding and create descriptor layout
	virtual void createDescriptorSetLayout(bool useTexture) = 0;
	// Destroy the valid descriptor layout object
	void destroyDescriptorLayout();

	// Creates the descriptor pool that is used to allocate descriptor sets
	virtual void createDescriptorPool(bool useTexture) = 0;
	// Deletes the descriptor pool
	void destroyDescriptorPool();

	// Create Descriptor set associated resources before creating the descriptor set
	virtual void createDescriptorResources() = 0;

	// Create the descriptor set from the descriptor pool allocated 
	// memory and update the descriptor set information into it.
	virtual void createDescriptorSet(bool useTexture) = 0;
	void destroyDescriptorSet();

	// Creates the pipeline layout to inject into the pipeline
	virtual void createPipelineLayout() = 0;
	// Destroys the create pipelineLayout
	void destroyPipelineLayouts();
	
public:
	// Pipeline layout object
	VkPipelineLayout pipelineLayout;

	// List of all the VkDescriptorSetLayouts 
	std::vector<VkDescriptorSetLayout> descLayout;
	
	// Decriptor pool object that will be used for allocating VkDescriptorSet object
	VkDescriptorPool descriptorPool;
	
	// List of all created VkDescriptorSet
	std::vector<VkDescriptorSet> descriptorSet;

	// Logical device used for creating the descriptor pool and descriptor sets
	VulkanDevice* deviceObj;
};