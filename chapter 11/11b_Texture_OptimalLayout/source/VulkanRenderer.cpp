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

#include "VulkanRenderer.h"
#include "VulkanApplication.h"
#include "Wrappers.h"
#include "MeshData.h"

VulkanRenderer::VulkanRenderer(VulkanApplication * app, VulkanDevice* deviceObject)
{
	// Note: It's very important to initilize the member with 0 or respective value other wise it will break the system
	memset(&Depth, 0, sizeof(Depth));
	memset(&connection, 0, sizeof(HINSTANCE));				// hInstance - Windows Instance

	application = app;
	deviceObj	= deviceObject;

	swapChainObj = new VulkanSwapChain(this);
	VulkanDrawable* drawableObj = new VulkanDrawable(this);
	drawableList.push_back(drawableObj);
}

VulkanRenderer::~VulkanRenderer()
{
	delete swapChainObj;
	swapChainObj = NULL;
	for each(auto d in drawableList)
	{
		delete d;
	}
	drawableList.clear();
}

void VulkanRenderer::initialize()
{

	// We need command buffers, so create a command buffer pool
	createCommandPool();
	
	// Let's create the swap chain color images and depth image
	buildSwapChainAndDepthImage();

	// Build the vertex buffer 	
	createVertexBuffer();
	
	const bool includeDepth = true;
	// Create the render pass now..
	createRenderPass(includeDepth);
	
	// Use render pass and create frame buffer
	createFrameBuffer(includeDepth);

	// Create the vertex and fragment shader
	createShaders();

	const char* filename = "../LearningVulkan.ktx";
	bool renderOptimalTexture = true;
	if (renderOptimalTexture) {
		createTextureOptimal(filename, &texture, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT);
	}
	else {
		createTextureLinear(filename, &texture, VK_IMAGE_USAGE_SAMPLED_BIT);
	}
	// Set the created texture in the drawable object.
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->setTextures(&texture);
	}

	// Create descriptor set layout
	createDescriptors();

	// Manage the pipeline state objects
	createPipelineStateManagement();
}

void VulkanRenderer::prepare()
{
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->prepare();
	}
}

void VulkanRenderer::update()
{
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->update();
	}
}

bool VulkanRenderer::render()
{
	MSG msg;   // message
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	if (msg.message == WM_QUIT) {
		return false;
	}
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	RedrawWindow(window, NULL, NULL, RDW_INTERNALPAINT);
	return true;
}

#ifdef _WIN32

// MS-Windows event handling function:
LRESULT CALLBACK VulkanRenderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	VulkanApplication* appObj = VulkanApplication::GetInstance();
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		for each (VulkanDrawable* drawableObj in appObj->rendererObj->drawableList)
		{
			drawableObj->render();
		}

		return 0;
	
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			appObj->rendererObj->width = lParam & 0xffff;
			appObj->rendererObj->height = (lParam & 0xffff0000) >> 16;
			appObj->rendererObj->getSwapChain()->setSwapChainExtent(appObj->rendererObj->width, appObj->rendererObj->height);
			appObj->resize();
		}
		break;

	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void VulkanRenderer::createPresentationWindow(const int& windowWidth, const int& windowHeight)
{
#ifdef _WIN32
	width	= windowWidth;
	height	= windowHeight; 
	assert(width > 0 || height > 0);

	WNDCLASSEX  winInfo;

	sprintf(name, "Texture demo - Optimal Layout");
	memset(&winInfo, 0, sizeof(WNDCLASSEX));
	// Initialize the window class structure:
	winInfo.cbSize			= sizeof(WNDCLASSEX);
	winInfo.style			= CS_HREDRAW | CS_VREDRAW;
	winInfo.lpfnWndProc		= WndProc;
	winInfo.cbClsExtra		= 0;
	winInfo.cbWndExtra		= 0;
	winInfo.hInstance		= connection; // hInstance
	winInfo.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winInfo.hCursor			= LoadCursor(NULL, IDC_ARROW);
	winInfo.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	winInfo.lpszMenuName	= NULL;
	winInfo.lpszClassName	= name;
	winInfo.hIconSm			= LoadIcon(NULL, IDI_WINLOGO);

	// Register window class:
	if (!RegisterClassEx(&winInfo)) {
		// It didn't work, so try to give a useful error:
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}
	
	// Create window with the registered class:
	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	window = CreateWindowEx(0,
							name,					// class name
							name,					// app name
							WS_OVERLAPPEDWINDOW |	// window style
							WS_VISIBLE |
							WS_SYSMENU,
							100, 100,				// x/y coords
							wr.right - wr.left,     // width
							wr.bottom - wr.top,     // height
							NULL,					// handle to parent
							NULL,					// handle to menu
							connection,				// hInstance
							NULL);					// no extra parameters

	if (!window) {
		// It didn't work, so try to give a useful error:
		printf("Cannot create a window in which to draw!\n");
		fflush(stdout);
		exit(1);
	}

	SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)&application);
#else
	const xcb_setup_t *setup;
	xcb_screen_iterator_t iter;
	int scr;

	connection = xcb_connect(NULL, &scr);
	if (connection == NULL) {
		std::cout << "Cannot find a compatible Vulkan ICD.\n";
		exit(-1);
	}

	setup = xcb_get_setup(connection);
	iter = xcb_setup_roots_iterator(setup);
	while (scr-- > 0)
		xcb_screen_next(&iter);

	screen = iter.data;
#endif // _WIN32
}

void VulkanRenderer::destroyPresentationWindow()
{
	DestroyWindow(window);
}
#else
void VulkanRenderer::createPresentationWindow()
{
	assert(width > 0);
	assert(height > 0);

	uint32_t value_mask, value_list[32];

	window = xcb_generate_id(connection);

	value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	value_list[0] = screen->black_pixel;
	value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

	xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, width, height, 0, 
		XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);

	/* Magic code that will send notification when window is destroyed */
	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
	xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(connection, cookie, 0);

	xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
	reply = xcb_intern_atom_reply(connection, cookie2, 0);

	xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1,	&(*reply).atom);
	free(reply);

	xcb_map_window(connection, window);

	// Force the x/y coordinates to 100,100 results are identical in consecutive runs
	const uint32_t coords[] = { 100,  100 };
	xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
	xcb_flush(connection);

	xcb_generic_event_t *e;
	while ((e = xcb_wait_for_event(connection))) {
		if ((e->response_type & ~0x80) == XCB_EXPOSE)
			break;
	}
}

void VulkanRenderer::destroyWindow()
{
	xcb_destroy_window(connection, window);
	xcb_disconnect(connection);
}

#endif // _WIN32

void VulkanRenderer::createCommandPool()
{
	VulkanDevice* deviceObj		= application->deviceObj;
	/* Depends on intializeSwapChainExtension() */
	VkResult  res;

	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.pNext = NULL;
	cmdPoolInfo.queueFamilyIndex = deviceObj->graphicsQueueWithPresentIndex;
	cmdPoolInfo.flags = 0;

	res = vkCreateCommandPool(deviceObj->device, &cmdPoolInfo, NULL, &cmdPool);
	assert(res == VK_SUCCESS);
}

void VulkanRenderer::createDepthImage()
{
	VkResult  result;
	bool  pass;

	VkImageCreateInfo imageInfo = {};

	// If the depth format is undefined, use fallback as 16-byte value
	if (Depth.format == VK_FORMAT_UNDEFINED) {
		Depth.format = VK_FORMAT_D16_UNORM;
	}

	const VkFormat depthFormat = Depth.format;

	imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext			= NULL;
	imageInfo.imageType		= VK_IMAGE_TYPE_2D;
	imageInfo.format		= depthFormat;
	imageInfo.extent.width	= width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth	= 1;
	imageInfo.mipLevels		= 1;
	imageInfo.arrayLayers	= 1;
	imageInfo.samples		= NUM_SAMPLES;
	imageInfo.tiling		= VK_IMAGE_TILING_OPTIMAL;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices	= NULL;
	imageInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.usage					= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.flags					= 0;

	// User create image info and create the image objects
	result = vkCreateImage(deviceObj->device, &imageInfo, NULL, &Depth.image);
	assert(result == VK_SUCCESS);

	// Get the image memory requirements
	VkMemoryRequirements memRqrmnt;
	vkGetImageMemoryRequirements(deviceObj->device,	Depth.image, &memRqrmnt);

	VkMemoryAllocateInfo memAlloc = {};
	memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAlloc.pNext = NULL;
	memAlloc.allocationSize = 0;
	memAlloc.memoryTypeIndex = 0;
	memAlloc.allocationSize = memRqrmnt.size;
	// Determine the type of memory required with the help of memory properties
	pass = deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, 0, /* No requirements */ &memAlloc.memoryTypeIndex);
	assert(pass);

	// Allocate the memory for image objects
	result = vkAllocateMemory(deviceObj->device, &memAlloc, NULL, &Depth.mem);
	assert(result == VK_SUCCESS);

	// Bind the allocated memeory
	result = vkBindImageMemory(deviceObj->device, Depth.image, Depth.mem, 0);
	assert(result == VK_SUCCESS);


	VkImageViewCreateInfo imgViewInfo = {};
	imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imgViewInfo.pNext = NULL;
	imgViewInfo.image = VK_NULL_HANDLE;
	imgViewInfo.format = depthFormat;
	imgViewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
	imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imgViewInfo.subresourceRange.baseMipLevel = 0;
	imgViewInfo.subresourceRange.levelCount = 1;
	imgViewInfo.subresourceRange.baseArrayLayer = 0;
	imgViewInfo.subresourceRange.layerCount = 1;
	imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imgViewInfo.flags = 0;

	if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
		depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
		depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
		imgViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	// Use command buffer to create the depth image. This includes -
	// Command buffer allocation, recording with begin/end scope and submission.
	CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdDepthImage);
	CommandBufferMgr::beginCommandBuffer(cmdDepthImage);
	{
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;

		// Set the image layout to depth stencil optimal
		setImageLayout(Depth.image,
			imgViewInfo.subresourceRange.aspectMask,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange, cmdDepthImage);
	}
	CommandBufferMgr::endCommandBuffer(cmdDepthImage);
	CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdDepthImage);

	// Create the image view and allow the application to use the images.
	imgViewInfo.image = Depth.image;
	result = vkCreateImageView(deviceObj->device, &imgViewInfo, NULL, &Depth.view);
	assert(result == VK_SUCCESS);
}

void VulkanRenderer::createTextureOptimal(const char* filename, TextureData *texture, VkImageUsageFlags imageUsageFlags, VkFormat format)
{
	// Load the image 
	gli::texture2D image2D(gli::load(filename)); assert(!image2D.empty());

	// Get the image dimensions
	texture->textureWidth	= uint32_t(image2D[0].dimensions().x);
	texture->textureHeight	= uint32_t(image2D[0].dimensions().y);

	// Get number of mip-map levels
	texture->mipMapLevels	= uint32_t(image2D.levels());

	// Create a staging buffer resource states using.
	// Indicate it be the source of the transfer command.
	// .usage	= VK_BUFFER_USAGE_TRANSFER_SRC_BIT
	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType	= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size	= image2D.size();
	bufferCreateInfo.usage	= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult error;
	// Create a buffer resource (host-visible) -
	VkBuffer buffer;
	error = vkCreateBuffer(deviceObj->device, &bufferCreateInfo, NULL, &buffer);
	assert(!error);
	
	// Get the buffer memory requirements for the staging buffer -
	VkMemoryRequirements memRqrmnt;
	VkDeviceMemory	devMemory;
	vkGetBufferMemoryRequirements(deviceObj->device, buffer, &memRqrmnt);

	VkMemoryAllocateInfo memAllocInfo = {};
	memAllocInfo.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocInfo.pNext			= NULL;
	memAllocInfo.allocationSize = 0;
	memAllocInfo.memoryTypeIndex= 0;
	memAllocInfo.allocationSize = memRqrmnt.size;

	// Determine the type of memory required for the host-visible buffer  -
	deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memAllocInfo.memoryTypeIndex);
		
	// Allocate the memory for host-visible buffer objects -
	error = vkAllocateMemory(deviceObj->device, &memAllocInfo, nullptr, &devMemory);
	assert(!error);

	// Bind the host-visible buffer with allocated device memory -
	error = vkBindBufferMemory(deviceObj->device, buffer, devMemory, 0);
	assert(!error);

	// Populate the raw image data into the device memory -
	uint8_t *data;
	error = vkMapMemory(deviceObj->device, devMemory, 0, memRqrmnt.size, 0, (void **)&data);
	assert(!error);

	memcpy(data, image2D.data(), image2D.size());
	vkUnmapMemory(deviceObj->device, devMemory);

	// Create image info with optimal tiling support (.tiling = VK_IMAGE_TILING_OPTIMAL) -
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext			= NULL;
	imageCreateInfo.imageType		= VK_IMAGE_TYPE_2D;
	imageCreateInfo.format			= format;
	imageCreateInfo.mipLevels		= texture->mipMapLevels;
	imageCreateInfo.arrayLayers		= 1;
	imageCreateInfo.samples			= VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling			= VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.extent			= { texture->textureWidth, texture->textureHeight, 1 };
	imageCreateInfo.usage			= imageUsageFlags;

	// Set image object with VK_IMAGE_USAGE_TRANSFER_DST_BIT if
	// not set already. This allows to copy the source VkBuffer 
	// object (with VK_IMAGE_USAGE_TRANSFER_DST_BIT) contents
	// into this image object memory(destination).
	if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)){
		imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	error = vkCreateImage(deviceObj->device, &imageCreateInfo, nullptr, &texture->image);
	assert(!error);

	// Get the image memory requirements
	vkGetImageMemoryRequirements(deviceObj->device, texture->image, &memRqrmnt);

	// Set the allocation size equal to the buffer allocation
	memAllocInfo.allocationSize = memRqrmnt.size;

	// Determine the type of memory required with the help of memory properties
	deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

	// Allocate the physical memory on the GPU
	error = vkAllocateMemory(deviceObj->device, &memAllocInfo, nullptr, &texture->mem);
	assert(!error);

	// Bound the physical memory with the created image object 
	error = vkBindImageMemory(deviceObj->device, texture->image, texture->mem, 0);
	assert(!error);

	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask				= VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel			= 0;
	subresourceRange.levelCount				= texture->mipMapLevels;
	subresourceRange.layerCount				= 1;

	// Use a separate command buffer for texture loading
	// Start command buffer recording
	CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdTexture);
	CommandBufferMgr::beginCommandBuffer(cmdTexture);

	// set the image layout to be 
	// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	// since it is destination for copying buffer 
	// into image using vkCmdCopyBufferToImage -
	setImageLayout(texture->image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange, cmdTexture);

	// List contains the buffer image copy for each mipLevel -
	std::vector<VkBufferImageCopy> bufferImgCopyList;


	uint32_t bufferOffset = 0;
	// Iterater through each mip level and set buffer image copy -
	for (uint32_t i = 0; i < texture->mipMapLevels; i++)
	{
		VkBufferImageCopy bufImgCopyItem = {};
		bufImgCopyItem.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		bufImgCopyItem.imageSubresource.mipLevel		= i;
		bufImgCopyItem.imageSubresource.layerCount		= 1;
		bufImgCopyItem.imageSubresource.baseArrayLayer	= 0;
		bufImgCopyItem.imageExtent.width				= uint32_t(image2D[i].dimensions().x);
		bufImgCopyItem.imageExtent.height				= uint32_t(image2D[i].dimensions().y);
		bufImgCopyItem.imageExtent.depth				= 1;
		bufImgCopyItem.bufferOffset						= bufferOffset;

		bufferImgCopyList.push_back(bufImgCopyItem);

		// adjust buffer offset
		bufferOffset += uint32_t(image2D[i].size());
	}

	// Copy the staging buffer memory data contain
	// the stage raw data(with mip levels) into image object
	vkCmdCopyBufferToImage(cmdTexture, buffer, texture->image,	
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		uint32_t(bufferImgCopyList.size()), bufferImgCopyList.data());

	// Advised to change the image layout to shader read
	// after staged buffer copied into image memory -
	texture->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	setImageLayout(texture->image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		texture->imageLayout, subresourceRange, cmdTexture);


	// Submit command buffer containing copy and image layout commands-
	CommandBufferMgr::endCommandBuffer(cmdTexture);

	// Create a fence object to ensure that the command buffer is executed,
	// coping our staged raw data from the buffers to image memory with
	// respective image layout and attributes into consideration -
	VkFence fence;
	VkFenceCreateInfo fenceCI	= {};
	fenceCI.sType				= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCI.flags				= 0;

	error = vkCreateFence(deviceObj->device, &fenceCI, nullptr, &fence);
	assert(!error);

	VkSubmitInfo submitInfo			= {};
	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext				= NULL;
	submitInfo.commandBufferCount	= 1;
	submitInfo.pCommandBuffers		= &cmdTexture;

	CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdTexture, &submitInfo, fence);

	error = vkWaitForFences(deviceObj->device, 1, &fence, VK_TRUE, 10000000000);
	assert(!error);

	vkDestroyFence(deviceObj->device, fence, nullptr);

	// destroy the allocated resoureces
	vkFreeMemory(deviceObj->device, devMemory, nullptr);
	vkDestroyBuffer(deviceObj->device, buffer, nullptr);

	///////////////////////////////////////////////////////////////////////////////////////

	// Create sampler
	VkSamplerCreateInfo samplerCI = {};
	samplerCI.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCI.pNext						= NULL;
	samplerCI.magFilter					= VK_FILTER_LINEAR;
	samplerCI.minFilter					= VK_FILTER_LINEAR;
	samplerCI.mipmapMode				= VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCI.addressModeU				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCI.addressModeV				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCI.addressModeW				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCI.mipLodBias				= 0.0f;
	if (deviceObj->deviceFeatures.samplerAnisotropy == VK_TRUE)
	{
		samplerCI.anisotropyEnable		= VK_TRUE;
		samplerCI.maxAnisotropy			= 8;
	}
	else
	{
		samplerCI.anisotropyEnable		= VK_FALSE;
		samplerCI.maxAnisotropy			= 1;
	}
	samplerCI.compareOp					= VK_COMPARE_OP_NEVER;
	samplerCI.minLod					= 0.0f;
	samplerCI.maxLod					= (float)texture->mipMapLevels;
	samplerCI.borderColor				= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCI.unnormalizedCoordinates	= VK_FALSE;

	error = vkCreateSampler(deviceObj->device, &samplerCI, nullptr, &texture->sampler);
	assert(!error);

	// Create image view to allow shader to access the texture information -
	VkImageViewCreateInfo viewCI = {};
	viewCI.sType			= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCI.pNext			= NULL;
	viewCI.viewType			= VK_IMAGE_VIEW_TYPE_2D;
	viewCI.format			= format;
	viewCI.components.r		= VK_COMPONENT_SWIZZLE_R;
	viewCI.components.g		= VK_COMPONENT_SWIZZLE_G;
	viewCI.components.b		= VK_COMPONENT_SWIZZLE_B;
	viewCI.components.a		= VK_COMPONENT_SWIZZLE_A;
	viewCI.subresourceRange	= subresourceRange;
	viewCI.subresourceRange.levelCount	= texture->mipMapLevels; 	// Optimal tiling supports mip map levels very well set it.
	viewCI.image			= texture->image;

	error = vkCreateImageView(deviceObj->device, &viewCI, NULL, &texture->view);
	assert(!error);

	// Fill descriptor image info that can be used for setting up descriptor sets
	texture->descsImgInfo.imageView = texture->view;
	texture->descsImgInfo.sampler = texture->sampler;
	texture->descsImgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
}

void VulkanRenderer::createTextureLinear(const char* filename, TextureData *texture, VkImageUsageFlags imageUsageFlags, VkFormat format)
{
	// Load the image 
	gli::texture2D image2D(gli::load(filename)); assert(!image2D.empty());

	// Get the image dimensions
	texture->textureWidth	= uint32_t(image2D[0].dimensions().x);
	texture->textureHeight	= uint32_t(image2D[0].dimensions().y);

	// Get number of mip-map levels
	texture->mipMapLevels	= uint32_t(image2D.levels());

	// Create image resource states using VkImageCreateInfo
	VkImageCreateInfo imageCreateInfo   = {};
	imageCreateInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext				= NULL;
	imageCreateInfo.imageType			= VK_IMAGE_TYPE_2D;
	imageCreateInfo.format				= format;
	imageCreateInfo.extent.width		= uint32_t(image2D[0].dimensions().x);
	imageCreateInfo.extent.height		= uint32_t(image2D[0].dimensions().y);
	imageCreateInfo.extent.depth		= 1;
	imageCreateInfo.mipLevels			= texture->mipMapLevels;
	imageCreateInfo.arrayLayers			= 1;
	imageCreateInfo.samples				= VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.queueFamilyIndexCount	= 0;
	imageCreateInfo.pQueueFamilyIndices	= NULL;
	imageCreateInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.usage				= imageUsageFlags;
	imageCreateInfo.flags				= 0;
	imageCreateInfo.initialLayout		= VK_IMAGE_LAYOUT_PREINITIALIZED,
	imageCreateInfo.tiling				= VK_IMAGE_TILING_LINEAR;

	VkResult  error;
	// Use create image info and create the image objects
	error = vkCreateImage(deviceObj->device, &imageCreateInfo, NULL, &texture->image);
	assert(!error);

	// Get the buffer memory requirements
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(deviceObj->device, texture->image, &memoryRequirements);

	// Create memory allocation metadata information
	VkMemoryAllocateInfo& memAlloc	= texture->memoryAlloc;
	memAlloc.sType					= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAlloc.pNext					= NULL;
	memAlloc.allocationSize			= memoryRequirements.size;
	memAlloc.memoryTypeIndex		= 0;

	// Determine the type of memory required 
	// with the help of memory properties
	bool pass = deviceObj->memoryTypeFromProperties(memoryRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &texture->memoryAlloc.memoryTypeIndex);
	assert(pass);

	// Allocate the memory for buffer objects
	error = vkAllocateMemory(deviceObj->device, &texture->memoryAlloc, NULL, &(texture->mem));
	assert(!error);

	// Bind the image device memory 
	error = vkBindImageMemory(deviceObj->device, texture->image, texture->mem, 0);
	assert(!error);

	VkImageSubresource subresource	= {};
	subresource.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel			= 0;
	subresource.arrayLayer			= 0;

	VkSubresourceLayout layout;
	uint8_t *data;

	vkGetImageSubresourceLayout(deviceObj->device, texture->image, &subresource, &layout);

	// Map the GPU memory on to local host
	error = vkMapMemory(deviceObj->device, texture->mem, 0, texture->memoryAlloc.allocationSize, 0, (void**)&data);
	assert(!error);

	// Load image texture data in the mapped buffer
	uint8_t* dataTemp = (uint8_t*)image2D.data();
	for (int y = 0; y < image2D[0].dimensions().y; y++)
	{
		size_t imageSize = image2D[0].dimensions().y * 4;
		memcpy(data, dataTemp, imageSize);
		dataTemp += imageSize;

		// Advance row by row pitch information
		data += layout.rowPitch;
	}

	// UnMap the host memory to push the changes into the device memory
	vkUnmapMemory(deviceObj->device, texture->mem);
	
	// Command buffer allocation and recording begins
	CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdTexture);
	CommandBufferMgr::beginCommandBuffer(cmdTexture);

	VkImageSubresourceRange subresourceRange	= {};
	subresourceRange.aspectMask					= VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel				= 0;
	subresourceRange.levelCount					= texture->mipMapLevels;
	subresourceRange.layerCount					= 1;

	texture->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	setImageLayout(texture->image, VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_PREINITIALIZED, texture->imageLayout,
		subresourceRange, cmdTexture);

	// Stop command buffer recording
	CommandBufferMgr::endCommandBuffer(cmdTexture);

	// Ensure that the GPU has finished the submitted job before host takes over again 
	VkFence fence;
	VkFenceCreateInfo fenceCI = {};
	fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCI.flags = 0;

	vkCreateFence(deviceObj->device, &fenceCI, nullptr, &fence);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdTexture;

	CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdTexture, &submitInfo, fence);
	vkWaitForFences(deviceObj->device, 1, &fence, VK_TRUE, 10000000000);
	vkDestroyFence(deviceObj->device, fence, nullptr);

	// Specify a particular kind of texture using samplers
	VkSamplerCreateInfo samplerCI	= {};
	samplerCI.sType					= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCI.pNext					= NULL;
	samplerCI.magFilter				= VK_FILTER_LINEAR;
	samplerCI.minFilter				= VK_FILTER_LINEAR;
	samplerCI.mipmapMode			= VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCI.addressModeU			= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCI.addressModeV			= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCI.addressModeW			= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCI.mipLodBias			= 0.0f;
	if (deviceObj->deviceFeatures.samplerAnisotropy == VK_TRUE)
	{
		samplerCI.anisotropyEnable	= VK_TRUE;
		samplerCI.maxAnisotropy		= 8;
	}
	else
	{
		samplerCI.anisotropyEnable	= VK_FALSE;
		samplerCI.maxAnisotropy		= 1;
	}
	samplerCI.compareOp				= VK_COMPARE_OP_NEVER;
	samplerCI.minLod				= 0.0f;
	samplerCI.maxLod				= 0.0f; // Set to texture->mipLevels if Optimal tiling, generally linear does not support mip-maping
	samplerCI.borderColor			= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCI.unnormalizedCoordinates = VK_FALSE;

	// Create the sampler
	error = vkCreateSampler(deviceObj->device, &samplerCI, NULL, &texture->sampler);
	assert(!error);

	// Create image view to allow shader to access the texture information -
	VkImageViewCreateInfo viewCI	= {};
	viewCI.sType					= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCI.pNext					= NULL;
	viewCI.viewType					= VK_IMAGE_VIEW_TYPE_2D;
	viewCI.format					= format;
	viewCI.components.r				= VK_COMPONENT_SWIZZLE_R;
	viewCI.components.g				= VK_COMPONENT_SWIZZLE_G;
	viewCI.components.b				= VK_COMPONENT_SWIZZLE_B;
	viewCI.components.a				= VK_COMPONENT_SWIZZLE_A;
	viewCI.subresourceRange			= subresourceRange;
	viewCI.subresourceRange.levelCount = 1;
	viewCI.flags					= 0;
	viewCI.image					= texture->image;

	error = vkCreateImageView(deviceObj->device, &viewCI, NULL, &texture->view);
	assert(!error);

	texture->descsImgInfo.sampler		= texture->sampler;
	texture->descsImgInfo.imageView		= texture->view;
	texture->descsImgInfo.imageLayout	= VK_IMAGE_LAYOUT_GENERAL;

	// Set the created texture in the drawable object.
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->setTextures(texture);
	}
}

void VulkanRenderer::createRenderPass(bool isDepthSupported, bool clear)
{
	// Dependency on VulkanSwapChain::createSwapChain() to 
	// get the color surface image and VulkanRenderer::createDepthBuffer()
	// to get the depth buffer image.
	
	VkResult  result;
	// Attach the color buffer and depth buffer as an attachment to render pass instance
	VkAttachmentDescription attachments[2];
	attachments[0].format					= swapChainObj->scPublicVars.format;
	attachments[0].samples					= NUM_SAMPLES;
	attachments[0].loadOp					= clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].storeOp					= VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout				= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachments[0].flags					= VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;

	// Is the depth buffer present the define attachment properties for depth buffer attachment.
	if (isDepthSupported)
	{
		attachments[1].format				= Depth.format;
		attachments[1].samples				= NUM_SAMPLES;
		attachments[1].loadOp				= clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].storeOp				= VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp		= VK_ATTACHMENT_LOAD_OP_LOAD;
		attachments[1].stencilStoreOp		= VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout			= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[1].flags				= VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	}

	// Define the color buffer attachment binding point and layout information
	VkAttachmentReference colorReference	= {};
	colorReference.attachment				= 0;
	colorReference.layout					= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Define the depth buffer attachment binding point and layout information
	VkAttachmentReference depthReference = {};
	depthReference.attachment				= 1;
	depthReference.layout					= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Specify the attachments - color, depth, resolve, preserve etc.
	VkSubpassDescription subpass			= {};
	subpass.pipelineBindPoint				= VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags							= 0;
	subpass.inputAttachmentCount			= 0;
	subpass.pInputAttachments				= NULL;
	subpass.colorAttachmentCount			= 1;
	subpass.pColorAttachments				= &colorReference;
	subpass.pResolveAttachments				= NULL;
	subpass.pDepthStencilAttachment			= isDepthSupported ? &depthReference : NULL;
	subpass.preserveAttachmentCount			= 0;
	subpass.pPreserveAttachments			= NULL;

	// Specify the attachement and subpass associate with render pass
	VkRenderPassCreateInfo rpInfo			= {};
	rpInfo.sType							= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpInfo.pNext							= NULL;
	rpInfo.attachmentCount					= isDepthSupported ? 2 : 1;
	rpInfo.pAttachments						= attachments;
	rpInfo.subpassCount						= 1;
	rpInfo.pSubpasses						= &subpass;
	rpInfo.dependencyCount					= 0;
	rpInfo.pDependencies					= NULL;

	// Create the render pass object
	result = vkCreateRenderPass(deviceObj->device, &rpInfo, NULL, &renderPass);
	assert(result == VK_SUCCESS);
}

void VulkanRenderer::createFrameBuffer(bool includeDepth)
{
	// Dependency on createDepthBuffer(), createRenderPass() and createSwapChain()
	VkResult  result;
	VkImageView attachments[2];
	attachments[1] = Depth.view;

	VkFramebufferCreateInfo fbInfo	= {};
	fbInfo.sType					= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbInfo.pNext					= NULL;
	fbInfo.renderPass				= renderPass;
	fbInfo.attachmentCount			= includeDepth ? 2 : 1;
	fbInfo.pAttachments				= attachments;
	fbInfo.width					= width;
	fbInfo.height					= height;
	fbInfo.layers					= 1;

	uint32_t i;

	framebuffers.clear();
	framebuffers.resize(swapChainObj->scPublicVars.swapchainImageCount);
	for (i = 0; i < swapChainObj->scPublicVars.swapchainImageCount; i++) {
		attachments[0] = swapChainObj->scPublicVars.colorBuffer[i].view;
		result = vkCreateFramebuffer(deviceObj->device, &fbInfo, NULL, &framebuffers.at(i));
		assert(result == VK_SUCCESS);
	}
}

void VulkanRenderer::destroyFramebuffers()
{
	for (uint32_t i = 0; i < swapChainObj->scPublicVars.swapchainImageCount; i++) {
		vkDestroyFramebuffer(deviceObj->device, framebuffers.at(i), NULL);
	}
	framebuffers.clear();
}

void VulkanRenderer::destroyRenderpass()
{
	vkDestroyRenderPass(deviceObj->device, renderPass, NULL);
}

void VulkanRenderer::destroyDrawableVertexBuffer()
{
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->destroyVertexBuffer();
	}
}

void VulkanRenderer::destroyDrawableUniformBuffer()
{
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->destroyUniformBuffer();
	}
}

void VulkanRenderer::destroyTextureResource()
{
	vkFreeMemory(deviceObj->device, texture.mem, NULL);
	vkDestroySampler(deviceObj->device, texture.sampler, NULL);
	vkDestroyImage(deviceObj->device, texture.image, NULL);
	vkDestroyImageView(deviceObj->device, texture.view, NULL);
}

void VulkanRenderer::destroyDrawableCommandBuffer()
{
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->destroyCommandBuffer();
	}
}

void VulkanRenderer::destroyDrawableSynchronizationObjects()
{
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->destroySynchronizationObjects();
	}
}

void VulkanRenderer::destroyDepthBuffer()
{
	vkDestroyImageView(deviceObj->device, Depth.view, NULL);
	vkDestroyImage(deviceObj->device, Depth.image, NULL);
	vkFreeMemory(deviceObj->device, Depth.mem, NULL);
}

void VulkanRenderer::destroyCommandBuffer()
{
	VkCommandBuffer cmdBufs[] = { cmdDepthImage, cmdVertexBuffer, cmdTexture };
	vkFreeCommandBuffers(deviceObj->device, cmdPool, sizeof(cmdBufs)/sizeof(VkCommandBuffer), cmdBufs);
}

void VulkanRenderer::destroyCommandPool()
{
	VulkanDevice* deviceObj		= application->deviceObj;

	vkDestroyCommandPool(deviceObj->device, cmdPool, NULL);
}

void VulkanRenderer::buildSwapChainAndDepthImage()
{
	// Get the appropriate queue to submit the command into
	deviceObj->getDeviceQueue();

	// Create swapchain and get the color image
	swapChainObj->createSwapChain(cmdDepthImage);
	
	// Create the depth image
	createDepthImage();
}

void VulkanRenderer::createVertexBuffer()
{
	CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdVertexBuffer);
	CommandBufferMgr::beginCommandBuffer(cmdVertexBuffer);

	for each (VulkanDrawable* drawableObj in drawableList)
	{
		drawableObj->createVertexBuffer(geometryData, sizeof(geometryData), sizeof(geometryData[0]), false);
	}
	CommandBufferMgr::endCommandBuffer(cmdVertexBuffer);
	CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdVertexBuffer);
}

void VulkanRenderer::createShaders()
{
	if (application->isResizing)
		return;

	void* vertShaderCode, *fragShaderCode;
	size_t sizeVert, sizeFrag;

#ifdef AUTO_COMPILE_GLSL_TO_SPV
	vertShaderCode = readFile("./../Texture.vert", &sizeVert);
	fragShaderCode = readFile("./../Texture.frag", &sizeFrag);
	
	shaderObj.buildShader((const char*)vertShaderCode, (const char*)fragShaderCode);
#else
	vertShaderCode = readFile("./../Texture-vert.spv", &sizeVert);
	fragShaderCode = readFile("./../Texture-frag.spv", &sizeFrag);

	shaderObj.buildShaderModuleWithSPV((uint32_t*)vertShaderCode, sizeVert, (uint32_t*)fragShaderCode, sizeFrag);
#endif
}

// Create the descriptor set
void VulkanRenderer::createDescriptors()
{
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		// It is upto an application how it manages the 
		// creation of descriptor. Descriptors can be cached 
		// and reuse for all similar objects.
		drawableObj->createDescriptorSetLayout(true);

		// Create the descriptor set
		drawableObj->createDescriptor(true);
	}

}

void VulkanRenderer::createPipelineStateManagement()
{
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		// Use the descriptor layout and create the pipeline layout.
		drawableObj->createPipelineLayout();
	}

	pipelineObj.createPipelineCache();

	const bool depthPresent = true;
	for each (VulkanDrawable* drawableObj in drawableList)
	{
		VkPipeline* pipeline = (VkPipeline*)malloc(sizeof(VkPipeline));
		if (pipelineObj.createPipeline(drawableObj, pipeline, &shaderObj, depthPresent))
		{
			pipelineList.push_back(pipeline);
			drawableObj->setPipeline(pipeline);
		}
		else
		{
			free(pipeline);
			pipeline = NULL;
		}
	}
}

void VulkanRenderer::setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, const VkImageSubresourceRange& subresourceRange, const VkCommandBuffer& cmd)
{
	// Dependency on cmd
	assert(cmd != VK_NULL_HANDLE);
	
	// The deviceObj->queue must be initialized
	assert(deviceObj->queue != VK_NULL_HANDLE);

	VkImageMemoryBarrier imgMemoryBarrier = {};
	imgMemoryBarrier.sType			= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imgMemoryBarrier.pNext			= NULL;
	imgMemoryBarrier.srcAccessMask	= 0;
	imgMemoryBarrier.dstAccessMask	= 0;
	imgMemoryBarrier.oldLayout		= oldImageLayout;
	imgMemoryBarrier.newLayout		= newImageLayout;
	imgMemoryBarrier.image			= image;
	imgMemoryBarrier.subresourceRange = subresourceRange;

	if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		imgMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	// Source layouts (old)
	switch (oldImageLayout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			imgMemoryBarrier.srcAccessMask = 0;
			break;
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			imgMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			imgMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			imgMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	switch (newImageLayout)
	{
	// Ensure that anything that was copying from this image has completed
	// An image in this layout can only be used as the destination operand of the commands
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		imgMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		break;

	// Ensure any Copy or CPU writes to image are flushed
	// An image in this layout can only be used as a read-only shader resource
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imgMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		imgMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;

	// An image in this layout can only be used as a framebuffer color attachment
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imgMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		break;

	// An image in this layout can only be used as a framebuffer depth/stencil attachment
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		imgMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		break;
	}

	VkPipelineStageFlags srcStages	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(cmd, srcStages, destStages, 0, 0, NULL, 0, NULL, 1, &imgMemoryBarrier);
}

// Destroy each pipeline object existing in the renderer
void VulkanRenderer::destroyPipeline()
{
	for each (VkPipeline* pipeline in pipelineList)
	{
		vkDestroyPipeline(deviceObj->device, *pipeline, NULL);
		free(pipeline);
	}
	pipelineList.clear();
}
