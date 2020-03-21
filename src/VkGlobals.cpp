#include "vkGlobals.h"

ImGuiGlobal vkImGui;

VkResult VkGlobal::CreateImage(const VkFormat& _format, const VkExtent3D& _imageExtent, const VkSampleCountFlagBits& _samples, const VkImageTiling& _tiling, const VkImageUsageFlags& _usageFlags, const VkMemoryPropertyFlags& _memoryPropertyFlags, VkImage* _outImage, VkDeviceMemory* _outImageMemory)
{
	//Create image info
	VkImageCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create_info.imageType = VK_IMAGE_TYPE_2D;
	create_info.extent = _imageExtent;
	create_info.mipLevels = 1;
	create_info.arrayLayers = 1;
	create_info.format = _format;
	create_info.tiling = _tiling;
	create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	create_info.usage = _usageFlags;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.samples = _samples;
	create_info.flags = 0;

	//Create the image
	VkResult r = vkCreateImage(vkGlobal.device, &create_info, VK_NULL_HANDLE, _outImage);
	if (r) return r;

	//Gather Memory Information from image & Physical Device
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(vkGlobal.device, *_outImage, &memory_requirements);
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(vkGlobal.physicalDevice, &memory_properties);

	//Loop through the memory type count and see if there is a match with both the filter and property flags
	int32_t memory_type_index = -1;
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
		if ((memory_requirements.memoryTypeBits & (1 << i)) &&
			(memory_properties.memoryTypes[i].propertyFlags & _memoryPropertyFlags) == _memoryPropertyFlags) {
			memory_type_index = i;
			break;
		}
	}
	if (memory_type_index == -1)
		return VK_ERROR_NOT_PERMITTED_EXT;

	//Memory Allocate Info
	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = memory_type_index;

	//Allocate the memory created
	r = vkAllocateMemory(vkGlobal.device, &memory_allocate_info, VK_NULL_HANDLE, _outImageMemory);
	if (r) {
		vkDestroyImage(vkGlobal.device, *_outImage, VK_NULL_HANDLE);
		return r;
	}

	//Bind the memory created
	r = vkBindImageMemory(vkGlobal.device, *_outImage, *_outImageMemory, 0);
	if (r) {
		vkDestroyImage(vkGlobal.device, *_outImage, VK_NULL_HANDLE);
		vkFreeMemory(vkGlobal.device, *_outImageMemory, VK_NULL_HANDLE);
		return r;
	}

	//Image Creation has been successful!
	return r;
}
VkResult VkGlobal::CreateImageView(const VkImage& _image, const VkFormat& _format, const VkImageAspectFlags& _imageAspectFlags, VkImageView* _outImageView)
{
	//Image View Create Info
	VkImageViewCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	create_info.image = _image;
	create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	create_info.format = _format;
	create_info.subresourceRange.aspectMask = _imageAspectFlags;
	create_info.subresourceRange.baseMipLevel = 0;
	create_info.subresourceRange.levelCount = 1;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount = 1;

	//Create the Surface (With Results) [VK_SUCCESS = 0]
	VkResult r = vkCreateImageView(vkGlobal.device, &create_info, nullptr, _outImageView);

	//Image View has been created successfully, return it
	return r;
}

VkResult SetupDescriptorPool() {
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	return vkCreateDescriptorPool(vkGlobal.device, &pool_info, VK_NULL_HANDLE, &vkImGui.descriptorPool);
}
VkResult SetupRenderPass() {
	//Primary Swapchain Description and Swapchain
	VkAttachmentDescription color_attachment_description = {};
	color_attachment_description.format = VK_FORMAT_B8G8R8A8_UNORM;
	color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_attachment_reference = {};
	color_attachment_reference.attachment = 0;
	color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Setup the Subpass and Dependency
	VkSubpassDescription subpass_description = {};
	subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description.colorAttachmentCount = 1;
	subpass_description.pColorAttachments = &color_attachment_reference;

	VkSubpassDependency subpass_dependency = {};
	subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency.dstSubpass = 0;
	subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcAccessMask = 0;
	subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	//Setup and Create the RenderPass
	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.attachmentCount = 1;
	render_pass_create_info.pAttachments = &color_attachment_description;
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pSubpasses = &subpass_description;
	render_pass_create_info.dependencyCount = 1;
	render_pass_create_info.pDependencies = &subpass_dependency;

	return vkCreateRenderPass(vkGlobal.device, &render_pass_create_info, nullptr, &vkImGui.renderPass);
}
VkResult SetupCommandObjects() {
	//Setup ImGui's Command Pool & Buffer
	VkCommandPoolCreateInfo cpool_create_info = {};
	cpool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cpool_create_info.queueFamilyIndex = vkGlobal.GRAPHICS_INDEX;
	cpool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cpool_create_info.pNext = VK_NULL_HANDLE;
	vkCreateCommandPool(vkGlobal.device, &cpool_create_info, VK_NULL_HANDLE, &vkImGui.commandPool);

	//Allocate Command buffer Information
	vkImGui.commandBuffer.resize(vkGlobal.frameMax);
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = vkImGui.commandPool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = 2;

	return vkAllocateCommandBuffers(vkGlobal.device, &command_buffer_allocate_info, vkImGui.commandBuffer.data());
}
VkResult SetupFonts() {
	// Use any command queue
	VkResult err = vkResetCommandPool(vkGlobal.device, vkImGui.commandPool, 0);
	ImGuiGlobal::check_vk_result(err);
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	err = vkBeginCommandBuffer(vkImGui.commandBuffer[0], &begin_info);
	ImGuiGlobal::check_vk_result(err);

	ImGui_ImplVulkan_CreateFontsTexture(vkImGui.commandBuffer[0]);

	VkSubmitInfo end_info = {};
	end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	end_info.commandBufferCount = 1;
	end_info.pCommandBuffers = &vkImGui.commandBuffer[0];
	err = vkEndCommandBuffer(vkImGui.commandBuffer[0]);
	ImGuiGlobal::check_vk_result(err);
	err = vkQueueSubmit(vkGlobal.queueGraphics, 1, &end_info, VK_NULL_HANDLE);
	ImGuiGlobal::check_vk_result(err);

	err = vkDeviceWaitIdle(vkGlobal.device);
	ImGuiGlobal::check_vk_result(err);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
	err = vkResetCommandPool(vkGlobal.device, vkImGui.commandPool, 0);

	return err;
}
VkResult SetupImage() {
	VkExtent3D ext = {
		vkGlobal.surfaceCapabilities.currentExtent.width,
		vkGlobal.surfaceCapabilities.currentExtent.height,
		1
	};
	VkResult r = VkGlobal::CreateImage(VK_FORMAT_B8G8R8A8_UNORM, ext, vkGlobal.msaa, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vkImGui.image, &vkImGui.memory);
	r = VkGlobal::CreateImageView(vkImGui.image, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &vkImGui.imageView);
	return VK_SUCCESS;
}
VkResult SetupFrameBuffer() {
	//Setup Variables
	VkResult r;

	//Frame Buffer's Create Info
	VkFramebufferCreateInfo frame_buffer_create_info = {};
	frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frame_buffer_create_info.renderPass = vkImGui.renderPass;
	frame_buffer_create_info.attachmentCount = 1;
	frame_buffer_create_info.pAttachments = &vkImGui.imageView;
	frame_buffer_create_info.width = vkGlobal.surfaceCapabilities.currentExtent.width;
	frame_buffer_create_info.height = vkGlobal.surfaceCapabilities.currentExtent.height;
	frame_buffer_create_info.layers = 1;

	//Create the Surface (With Results) [VK_SUCCESS = 0]
	r = vkCreateFramebuffer(vkGlobal.device, &frame_buffer_create_info, nullptr, &vkImGui.frameBuffer);

	return r;
}
VkResult SetupSyncObjects() {
	//Semaphore Info Create
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Fence Info Create
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//Resize Semaphores
	vkImGui.fence.resize(vkGlobal.frameMax);
	vkImGui.semaphore.resize(vkGlobal.frameMax);

	//Create the Semaphores and Fences
	VkResult r;
	for (unsigned int i = 0; i < vkGlobal.frameMax; ++i) {
		r = vkCreateSemaphore(vkGlobal.device, &semaphore_create_info, nullptr, &vkImGui.semaphore[i]);
		if (r) {
			return r;
		}
		r = vkCreateFence(vkGlobal.device, &fence_create_info, nullptr, &vkImGui.fence[i]);
		if (r) {
			return r;
		}
	}

	//Semaphores and Fences has been created successfully!
	return r;
}
VkResult ImGuiGlobal::Init_vkImGui()
{
	//Prereq: Descriptor Pool
	SetupDescriptorPool();
	SetupRenderPass();

	//Setup the initinfo
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = vkGlobal.instance;
	init_info.PhysicalDevice = vkGlobal.physicalDevice;
	init_info.Device = vkGlobal.device;
	init_info.QueueFamily = vkGlobal.GRAPHICS_INDEX;
	init_info.Queue = vkGlobal.queueGraphics;
	init_info.PipelineCache = vkImGui.pipelineCache;
	init_info.DescriptorPool = vkImGui.descriptorPool;
	init_info.Allocator = VK_NULL_HANDLE;
	init_info.MinImageCount = vkGlobal.surfaceCapabilities.minImageCount;
	init_info.ImageCount = vkGlobal.frameMax;
	init_info.CheckVkResultFn = check_vk_result;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	//Initialize ImGui - Vulkan
	if (!ImGui_ImplVulkan_Init(&init_info, vkImGui.renderPass)) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}

	//Setup Command Objects
	SetupCommandObjects();

	//Setup ImGui Font
	SetupFonts();

	//Setup Image
	SetupImage();

	//Setup Framebuffer
	SetupFrameBuffer();

	//Setup Sync Objects
	SetupSyncObjects();

	return VK_SUCCESS;
}

void ImGuiGlobal::check_vk_result(VkResult err) {
	if (err == 0) return;
#ifdef _DEBUG
	printf("VkResult %d\n", err);
#endif
	if (err < 0)
		abort();
}
