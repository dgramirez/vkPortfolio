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

VkResult ImGuiGlobal::Init_vkImGui()
{
	//Prereq: Descriptor Pool
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
	VkResult r = vkCreateDescriptorPool(vkGlobal.device, &pool_info, VK_NULL_HANDLE, &vkImGui.descriptorPool);

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

	//Primary Swapchain Description and Swapchain
	VkAttachmentDescription color_attachment_description = {};
	color_attachment_description.format = VK_FORMAT_B8G8R8A8_UNORM;
	color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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

	r = vkCreateRenderPass(vkGlobal.device, &render_pass_create_info, nullptr, &vkImGui.renderPass);

	//Initialize ImGui - Vulkan
	if (!ImGui_ImplVulkan_Init(&init_info, vkImGui.renderPass)) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}
	
	//Setup ImGui's Command Pool & Buffer
	VkCommandPoolCreateInfo cpool_create_info = {};
	cpool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cpool_create_info.queueFamilyIndex = vkGlobal.GRAPHICS_INDEX;
	cpool_create_info.flags = VK_NULL_HANDLE;
	cpool_create_info.pNext = VK_NULL_HANDLE;
	vkCreateCommandPool(vkGlobal.device, &cpool_create_info, VK_NULL_HANDLE, &vkImGui.commandPool);

	//Allocate Command buffer Information
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = vkImGui.commandPool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = 1;

	vkAllocateCommandBuffers(vkGlobal.device, &command_buffer_allocate_info, &vkImGui.commandBuffer);

	// Upload Fonts
	{
		// Use any command queue

		VkResult err = vkResetCommandPool(vkGlobal.device, vkImGui.commandPool, 0);
		check_vk_result(err);
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(vkImGui.commandBuffer, &begin_info);
		check_vk_result(err);

		ImGui_ImplVulkan_CreateFontsTexture(vkImGui.commandBuffer);

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &vkImGui.commandBuffer;
		err = vkEndCommandBuffer(vkImGui.commandBuffer);
		check_vk_result(err);
		err = vkQueueSubmit(vkGlobal.queueGraphics, 1, &end_info, VK_NULL_HANDLE);
		check_vk_result(err);

		err = vkDeviceWaitIdle(vkGlobal.device);
		check_vk_result(err);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		err = vkResetCommandPool(vkGlobal.device, vkImGui.commandPool, 0);
	}

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