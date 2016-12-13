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

// Shader class managing the shader conversion, compilation, linking
class VulkanShader
{
public:
	// Constructor
	VulkanShader() {}
	
	// Destructor
	~VulkanShader() {}

	// Use .spv and build shader module
	void buildShaderModuleWithSPV(uint32_t *vertShaderText, size_t vertexSPVSize, uint32_t *fragShaderText, size_t fragmentSPVSize);

	// Kill the shader when not required
	void destroyShaders();

#ifdef AUTO_COMPILE_GLSL_TO_SPV
	// Convert GLSL shader to SPIR-V shader
	bool GLSLtoSPV(const VkShaderStageFlagBits shaderType, const char *pshader, std::vector<unsigned int> &spirv);

	// Entry point to build the shaders
	void buildShader(const char *vertShaderText, const char *fragShaderText);

	// Type of shader language. This could be - EShLangVertex,Tessellation Control, 
	// Tessellation Evaluation, Geometry, Fragment and Compute
	EShLanguage getLanguage(const VkShaderStageFlagBits shader_type);

	// Initialize the TBuitInResource
	void initializeResources(TBuiltInResource &Resources);
#endif

	// Vk structure storing vertex & fragment shader information
	VkPipelineShaderStageCreateInfo shaderStages[2];
};
