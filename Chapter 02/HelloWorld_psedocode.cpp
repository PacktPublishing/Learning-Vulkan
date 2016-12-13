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

/**************** 1. Enumerate Instance Layer properties ****************/
// Get number of instance layers
uint32_t instanceLayerCount;
	
// Use second parameter as NULL to return the layer count
vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL); 

VkLayerProperties *layerProperty = NULL;
vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperty);

// Get the extensions for each available instance layer
foreach  layerProperty{
	VkExtensionProperties *instanceExtensions;
	res = vkEnumerateInstanceExtensionProperties(layer_name, 
			&instanceExtensionCount, instanceExtensions);
}

/**************** 2. Instance Creation ****************/
VkInstance instance;	// Vulkan instance object
VkInstanceCreateInfo instanceInfo	 = {};

// Specify layer and extensions names that needs to be enabled on instance.
instanceInfo.ppEnabledLayerNames	 = {"VK_LAYER_LUNARG_standard_validation",
										"VK_LAYER_LUNARG_object_tracker" };

// Specify extensions that needs to be enabled on instance.
instanceInfo.ppEnabledExtensionNames = {VK_KHR_SURFACE_EXTENSION_NAME, 
										VK_KHR_WIN32_SURFACE_EXTENSION_NAME};

// Create the Instance	 object
vkCreateInstance(&instanceInfo, NULL, &instance);

/**************** 3. Enumerate physical devices ****************/
VkPhysicalDevice				gpu;		// Physical device
uint32_t						gpuCount;	// Pysical device count
std::vector<VkPhysicalDevice>	gpuList;	// List of physical devices
// Get number of GPU count
vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);

// Get GPU information
vkEnumeratePhysicalDevices(instance, &gpuCount, gpuList);

/**************** 4. Create Device ****************/
// Get Queue and Queue Type
vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueCount, queueProperties);

// Get the memory properties from the physical device or GPU.
vkGetPhysicalDeviceMemoryProperties(gpu, &memoryProperties);

//Get the physical device or GPU properties
vkGetPhysicalDeviceProperties(gpu, &gpuProps);

// Create the logical device object from physical device
VkDevice			device;	// Logical device
VkDeviceCreateInfo	deviceInfo = {};
vkCreateDevice(gpuList[0], &deviceInfo, NULL, &device);

/**************** 5. Presentation Initialization ****************/
// 
// Creat empty Window
CreateWindowEx(...);		/*Windows*/
xcb_create_window(...); 	/*Linux*/

// Query WSI extensions,store as function pointers. For example:
// vkCreateSwapchainKHR, vkCreateSwapchainKHR .....

// Create abstract surface object
VkWin32SurfaceCreateInfoKHR createInfo = {};
vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &surface);

// Among all queues select a queue which supports presentation
foreach Queue in All Queues{
	vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queueIndex, surface, &isPresentaionSupported);
	// Store this queue’s index
	if (isPresentaionSupported) {
		graphicsQueueFamilyIndex = Queue.index;
		break;
	}
}

// Acquire compatible queue supporting presentation 
// and is a graphics queue
vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &queue);

// allocate memory for total format count
uint32_t formatCount;
vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, NULL);

VkSurfaceFormatKHR *surfaceFormats = allocate memory('formatCount' * VkSurfaceFormatKHR);

// Grab the surface format into VkSurfaceFormatKHR objects
vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, surfaceFormats);

/**************** 6. Creating Swapchain ****************/

//Start recording commands into command buffer
vkBeginCommandBuffer(cmd, &cmdBufInfo);

// Getting surface capabilities
vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfCapabilities);
		
// Getting surface presentation modes
vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, NULL);
VkPresentModeKHR presentModes[presentModeCount];
vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, presentModes);
		
// Creating the Swapchain
VkSwapchainCreateInfoKHR swapChainInfo = {};
fpCreateSwapchainKHR(device, &swapChainInfo, NULL, &swapChain);
		
// Retrieve the Swapchain images
vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, NULL);
VkImage swapchainImages[swapchainImageCount];
vkGetSwapchainImagesKHR(device, swapChain, &swapchainImageCount, swapchainImages);
		
// Retrieve the Swapchain images
foreach swapchainImages{
	// Set the implementation compatible layout
	SetImageLayout();

	// Insert pipeline barrier
	VkImageMemoryBarrier imgMemoryBarrier = { ... };
	vkCmdPipelineBarrier(cmd,srcStages,destStages,0,0,NULL,0,NULL,1,&imgMemoryBarrier);
	SwapChainBuffer scBuffer = {...};
	// Insert pipeline barrier
	vkCreateImageView(device, &colorImageView, NULL, &scBuffer.view);
	// Save the image view for application use
	buffers.push_back(scBuffer);
}

/**************** 7. Creating Depth buffer ****************/

// Query supported format features of the physical device
vkGetPhysicalDeviceFormatProperties(gpuList, depthFormat, &properties);

// Create image object
vkCreateImage(device, &imageInfo, NULL, &imageObject);

// Get the memory requirements for a image resource
vkGetImageMemoryRequirements(device, image, &memRequirements);

// Allocate memory
vkAllocateMemory(device, &memAlloc, NULL, &memorys);

// Bind memory
vkBindImageMemory(device, imageObject, mem, 0);

// Set the implementation compatible layout
SetImageLayout(. . .)

// Pipeline barrier
vkCmdPipelineBarrier(cmd, srcStages, destStages, 0, 0, NULL, 0, NULL, 1, &imgPipelineBarrier);

// Create Image View
vkCreateImageView(device, &imgViewInfo, NULL, &view);


/**************** 8. Building shader module ****************/
#version 450
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;
layout (location = 0) out vec4 outColor;
out gl_PerVertex {
	vec4 gl_Position;
};
void main() {
	outColor = inColor;
	gl_Position = pos;
	gl_Position.y = -gl_Position.y;
	gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
}

#version 450
layout (location = 0) in vec4 color;
layout (location = 0) out vec4 outColor;
void main() {
	outColor = color;
};

VkPipelineShaderStageCreateInfo vtxShdrStages = {....};
VkShaderModuleCreateInfo moduleCreateInfo = { ... };
moduleCreateInfo.pCode = spvVertexShaderData/*SPIRV form shader data*/;
// Create Shader module on the device
vkCreateShaderModule(device, &moduleCreateInfo, NULL, &vtxShdrStages.module);

/**************** 9. Creating descriptor layout and pipeline layout ****************/

// Descriptor layout specfies the type of information associated with shaders
VkDescriptorSetLayoutBinding layoutBindings[2];
layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
layoutBindings[0].binding		 = 0;
layoutBindings[0].stageFlags	 = VK_SHADER_STAGE_VERTEX_BIT;
layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
layoutBindings[1].binding		 = 0;
layoutBindings[1].stageFlags     = VK_SHADER_STAGE_FRAGMENT_BIT;

// Use layout bindings and create a descriptor set layout
VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
descriptorLayout.pBindings					     = layoutBindings;
VkDescriptorSetLayout descLayout[2];
vkCreateDescriptorSetLayout(device, &descriptorLayout, NULL, descLayout.data());

// Now use the descriptor layout to create a pipeline layout
VkPipelineLayoutCreateInfo pipelineLayoutCI = { ... };
pipelineLayoutCI.pSetLayouts				= descLayout.data();
vkCreatePipelineLayout(device, &pipelineLayoutCI, NULL, &pipelineLayout);


/**************** 10. Render Pass ****************/

// Define two attachment for color and depth buffer
VkAttachmentDescription attachments[2];
attachments[0].format = colorImageformat;
attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR 
							: VK_ATTACHMENT_LOAD_OP_DONT_CARE;
attachments[1].format = depthImageformat;
attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

VkAttachmentReference colorReference, depthReference = {...};

// Describe the subpass, use color image and depth image
VkSubpassDescription subpass	= {};
subpass.pColorAttachments		= &colorReference;
subpass.pDepthStencilAttachment = &depthReference;

// Define RenderPass control structure
VkRenderPassCreateInfo rpInfo	= { &attachments,&subpass ...};

VkRenderPass renderPass; // Create Render Pass object
vkCreateRenderPass(device, &rpInfo, NULL, &renderPass);

/**************** 11. Creating Frame buffers ****************/
VkImageView attachments[2];						// [0] for color, [1] for depth
attachments[1] = Depth.view;

VkFramebufferCreateInfo fbInfo = {};
fbInfo.renderPass				= renderPass;	// Pass render buffer object
fbInfo.pAttachments				= attachments;	// Image view attachments
fbInfo.width					= width;		// Frame buffer width
fbInfo.height					= height;		// Frame buffer height

// Allocate memory for frame buffer objects, for each image
// in the swapchain, there is one frame buffer For each respective 
// drawing surface there is one frame buffer  
VkFramebuffer framebuffers[number of draw surface image in swap chain];

foreach(drawing buffer in swapchain) {
	attachments[0] = currentSwapChainDrawImage.view;
	vkCreateFramebuffer(device, &fbInfo, NULL, &framebuffers[i]);
}

/****************  12. Populate Geometry – storing vertex into GPU memory ****************/

static const VertexWithColor triangleData[] = {
	/*{  x ,     y,    z,    w,    r,    g,    b,   a },*/
	{ 0.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0 },
	{ 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0 },
	{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0 },
};

VkBuffer				buffer;
VkMemoryRequirements	mem_requirement;
VkDeviceMemory			deviceMemory;

// Create buffer object, query required memory, allocate
VkBufferCreateInfo buffer_info = { ... };
vkCreateBuffer(device, &buffer_info, NULL, &buffer);

vkGetBufferMemoryRequirements(device, buffer, &mem_requirement);

VkMemoryAllocateInfo alloc_info = { ... };

vkAllocateMemory(device, &alloc_info, NULL, &(deviceMemory));

// Copy the triangleData to GPU using mapping and unmapping.
uint8_t *pData;
vkMapMemory(device, deviceMemory, 0, mem_requirement.size, 0, &pData);
memcpy(pData, triangleData, dataSize); /**** Copying data ****/
vkUnmapMemory(device, deviceMemory);

// Bind the memory
vkBindBufferMemory(device, buffer, deviceMemory, 0);

/****************  13. Vertex binding ****************/
VkVertexInputBindingDescription viBinding;
viBinding.binding = 0;
viBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
viBinding.stride = sizeof(triangleData) /*data Stride*/;

VkVertexInputAttributeDescription viAttribs[2];
viAttribs[0].binding = 0;
viAttribs[0].location = 0;
viAttribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
viAttribs[0].offset = 0;
viAttribs[1].binding = 0;
viAttribs[1].location = 1;
viAttribs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
viAttribs[1].offset = 16;

/****************  14. Defining states ****************/
// Vertex Input state
VkPipelineVertexInputStateCreateInfo vertexInputStateInfo	= { ... };
vertexInputStateInfo.vertexBindingDescriptionCount			= 1;
vertexInputStateInfo.pVertexBindingDescriptions				= &viBinding;
vertexInputStateInfo.vertexAttributeDescriptionCount		= 2;
vertexInputStateInfo.pVertexAttributeDescriptions			= viAttribs;

// Dynamic states
VkPipelineDynamicStateCreateInfo dynamicState				= { ... };
// Input assembly state control structure
VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo	= { ... };
// Rasterization state control structure
VkPipelineRasterizationStateCreateInfo rasterStateInfo		= { ... };
// Color blend Attachment state control structure
VkPipelineColorBlendAttachmentState colorBlendSI			= { ... };
// Color blend state control structure
VkPipelineColorBlendStateCreateInfo colorBlendStateInfo		= { ... };
// View port state control structure
VkPipelineViewportStateCreateInfo viewportStateInfo			= { ... };
// Depth stencil state control structure
VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = { ... };
// Multi sampling state control structure
VkPipelineMultisampleStateCreateInfo   multiSampleStateInfo = { ... };

/****************  15. Creating Graphics Pipeline ****************/
VkPipelineCache	pipelineCache; 
VkPipelineCacheCreateInfo pipelineCacheInfo;
vkCreatePipelineCache(device, &pipelineCacheInfo, NULL, &pipelineCache);

// Define the control structure of graphics pipeline 
VkGraphicsPipelineCreateInfo pipelineInfo;
pipelineInfo.layout					= pipelineLayout;
pipelineInfo.pVertexInputState		= &vertexInputStateInfo;
pipelineInfo.pInputAssemblyState	= &inputAssemblyInfo;
pipelineInfo.pRasterizationState	= &rasterStateInfo;
pipelineInfo.pColorBlendState		= &colorBlendStateInfo;
pipelineInfo.pMultisampleState		= &multiSampleStateInfo;
pipelineInfo.pDynamicState			= &dynamicState;
pipelineInfo.pViewportState			= &viewportStateInfo;
pipelineInfo.pDepthStencilState		= &depthStencilStateInfo;
pipelineInfo.pStages				= shaderStages;
pipelineInfo.stageCount				= 2;
pipelineInfo.renderPass				= renderPass;

// Create graphics pipeline
vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, NULL, &pipeline);

/****************  16. Acquiring drawing image ****************/

// Define semaphore for synchronizing the acquire of draw image.
// Only acquire draw image when drawing is completed
VkSemaphore imageAcquiredSemaphore ;
VkSemaphoreCreateInfo imageAcquiredSemaphore CreateInfo = { ... };
imageAcquiredSemaphore CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo,
					NULL, &imageAcquiredSemaphore );

// Get the index of the next available swapchain image:
vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, 
						imageAcquiredSemaphore , NULL, &currentSwapChainImageIndex);

/****************  17. Preparing render pass control structure ****************/

// Define clear color value and depth stencil values
const VkClearValue clearValues[2] = {
	[0] = { .color.float32 = { 0.2f, 0.2f, 0.2f, 0.2f } },
	[1] = { .depthStencil = { 1.0f, 0 } },
};

// Render pass execution control structure for a given frame buffer
VkRenderPassBeginInfo renderPassBegin;
renderPassBegin.sType		= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
renderPassBegin.pNext		= NULL;
renderPassBegin.renderPass	= renderPass;
renderPassBegin.framebuffer = framebuffers[currentSwapchainImageIndex];
renderPassBegin.renderArea.offset.x			= 0;
renderPassBegin.renderArea.offset.y			= 0;
renderPassBegin.renderArea.extent.width		= width;
renderPassBegin.renderArea.extent.height	= height;
renderPassBegin.clearValueCount				= 2;
renderPassBegin.pClearValues				= clearValues;

		
/****************  18. Render pass execute ****************/
/**** START RENDER PASS ****/
vkCmdBeginRenderPass(cmd, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE); 
		
vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline); // Bind the pipeline
const VkDeviceSize offsets[1] = { 0 };
vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, offsets);	 // Bind the triangle buffer data
vkCmdSetViewport(cmd, 0, NUM_VIEWPORTS, &viewport); // viewport = {0, 0, 500, 500, 0 ,1}
vkCmdSetScissor(cmd, 0, NUM_SCISSORS, &scissor);	 // scissor  = {0, 0, 500, 500}
vkCmdDraw(cmd, 3, 1, 0, 0);			 // 3 vertices, 1 instance, 0th first index

/**** END RENDER PASS ****/
vkCmdEndRenderPass(cmd);

// Insert pipeline barrier 
setImageLayout()
vkCmdPipelineBarrier(cmd, ....);

/**** COMMAND BUFFER RECORDING ENDS HERE ****/
vkEndCommandBuffer(cmd);

/****************  19. Queue Submission ****************/

VkFenceCreateInfo fenceInfo = { ... }; VkFence drawFence;
// Create fence to determine completion of execution of submissions to queues
vkCreateFence(device, &fenceInfo, NULL, &drawFence);

// Fill the command buffer submission control sturctures
VkSubmitInfo submitInfo[1]			= { ... };
submitInfo[0].pNext					= NULL;
submitInfo[0].sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
submitInfo[0].pWaitSemaphores		= &imageAcquiredSemaphore ;
submitInfo[0].commandBufferCount	= 1;
submitInfo[0].pCommandBuffers		= &cmd;

// Queue the command buffer for execution
vkQueueSubmit(queue, 1, submitInfo, NULL);

/****************  20. Present the draw result on the display window ****************/

// Define the presentation control structure
VkPresentInfoKHR present = { ... };
present.sType			 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
present.pNext			 = NULL;
present.swapchainCount	 = 1;
present.pSwapchains		 = &swapChain;
present.pImageIndices	 = &swapChainObjCurrent_buffer;

// Check if all the submitted command buffers are finished before displaying
do {
	res = vkWaitForFences(device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
} while (res == VK_TIMEOUT);

// Handover the swapchain image to presentation queue
// for presentation purposeRender to the display window
vkQueuePresentKHR(queue, &present);

// Destroy Synchronization objects
vkDestroySemaphore(device, imageAcquiredSemaphore , NULL);
vkDestroyFence(device, drawFence, NULL);
