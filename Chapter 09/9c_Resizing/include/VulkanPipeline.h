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
class VulkanShader;
class VulkanDrawable;
class VulkanDevice;
class VulkanApplication;

// While creating the pipeline the number of viewports and number 
// of scissors.                 
#define NUMBER_OF_VIEWPORTS 1
#define NUMBER_OF_SCISSORS NUMBER_OF_VIEWPORTS

class VulkanPipeline
{
public:
	VulkanPipeline();

	~VulkanPipeline();
	
	// Creates the pipeline cache object and stores pipeline object
	void createPipelineCache();
	
	// Returns the created pipeline object, it takes the drawable object which 
	// contains the vertex input rate and data interpretation information, 
	// shader files, boolean flag checking enabled depth, and flag to check
	// if the vertex input are available. 	
	bool createPipeline(VulkanDrawable* drawableObj, VkPipeline* pipeline, VulkanShader* shaderObj, VkBool32 includeDepth, VkBool32 includeVi = true);

	// Destruct the pipeline cache object
	void destroyPipelineCache();

public:
	// Pipeline preparation member variables
	// Pipeline cache object
	VkPipelineCache						pipelineCache;
	VkPipelineLayout 					pipelineLayout;
	VulkanApplication*					appObj;
	VulkanDevice*						deviceObj;
};