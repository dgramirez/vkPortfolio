#include "VkGlobals.h"

namespace {
	VkResult CreateSwapchain() {
	//Gather Swapchain Count
		if (VkSwapchain::surfaceCapabilities.minImageCount > 0 && VkSwapchain::frameMax < VkSwapchain::surfaceCapabilities.minImageCount)
			VkSwapchain::frameMax = VkGlobal::surfaceCapabilities.minImageCount;
		if (VkGlobal::surfaceCapabilities.maxImageCount > 0 && VkSwapchain::frameMax > VkSwapchain::surfaceCapabilities.maxImageCount)
			VkSwapchain::frameMax = VkSwapchain::surfaceCapabilities.maxImageCount;

		//Create Info for SwapchainKHR [Part 1]
		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = VkGlobal::surface;
		create_info.minImageCount = VkSwapchain::frameMax;
		create_info.imageFormat = VkSwapchain::surfaceFormat.format;
		create_info.imageColorSpace = VkSwapchain::surfaceFormat.colorSpace;
		create_info.imageExtent = VkSwapchain::surfaceExtent2D;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		create_info.preTransform = VkSwapchain::surfaceCapabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = VkSwapchain::surfacePresentMode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VkSwapchain::swapchain;

		std::vector<uint32_t> uniqueIndices;
		uniqueIndices.push_back(VkGlobal::GRAPHICS_INDEX);
		if (VkGlobal::GRAPHICS_INDEX ^ VkGlobal::PRESENT_INDEX)
			uniqueIndices.push_back(VkGlobal::PRESENT_INDEX);
		if (VkGlobal::PRESENT_INDEX ^ VkGlobal::COMPUTE_INDEX)
			uniqueIndices.push_back(VkGlobal::COMPUTE_INDEX);

		//Setup Correct Queue Family Indices
		if (uniqueIndices.size() > 1) {
			create_info.queueFamilyIndexCount = uniqueIndices.size();
			create_info.pQueueFamilyIndices = uniqueIndices.data();
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		}
		else {
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		//Create Swapchain
		VkResult r = vkCreateSwapchainKHR(VkGlobal::device, &create_info, nullptr, &VkSwapchain::swapchain);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		//Swapchain Image Setup
		r = vkGetSwapchainImagesKHR(VkGlobal::device, VkSwapchain::swapchain, &VkSwapchain::frameMax, VK_NULL_HANDLE);
		if (r) {
			VK_ASSERT(r);
			return r;
		}
		VkSwapchain::swapchainImage.resize(VkSwapchain::frameMax);
		r = vkGetSwapchainImagesKHR(VkGlobal::device, VkSwapchain::swapchain, &VkSwapchain::frameMax, VkSwapchain::swapchainImage.data());
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		//Obtain the Image and Image Views
		VkSwapchain::swapchainImageView.resize(VkSwapchain::frameMax);
		for (uint32_t i = 0; i < VkSwapchain::frameMax; ++i) {
			r = VkGlobal::CreateImageView(VkSwapchain::swapchainImage[i], VkSwapchain::surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &VkSwapchain::swapchainImageView[i]);
			if (r) {
				VK_ASSERT(r);
				return r;
			}
		}

		//Set Current Frame to 0
		VkSwapchain::frameCurrent = 0;

		//Quick Check: If Old Swapchain exists
		if (create_info.oldSwapchain)
			vkDestroySwapchainKHR(VkGlobal::device, create_info.oldSwapchain, VK_NULL_HANDLE);

		//Return result (VK_SUCCESS)
		return r;
	}
	VkResult CreateRenderPass() {
		//Preliminary Setup
		std::vector<VkAttachmentDescription> attachments;

		//Primary Swapchain Description and Swapchain
		VkAttachmentDescription color_attachment_description = {};
		color_attachment_description.format = VkSwapchain::surfaceFormat.format;
		color_attachment_description.samples = VkGlobal::msaa;
		color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_description.finalLayout = (VkSwapchain::msaaImage) ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_reference = {};
		color_attachment_reference.attachment = attachments.size();
		color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachments.push_back(color_attachment_description);

		//Depth Swapchain Attachment & Reference
		VkAttachmentDescription depth_attachment_description = {};
		VkAttachmentReference depth_attachment_reference = {};
		if (VkSwapchain::presetFlags & 0x01) {
			depth_attachment_description.format = VkSwapchain::depthFormat;
			depth_attachment_description.samples = VkGlobal::msaa;
			depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			depth_attachment_reference.attachment = attachments.size();
			depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachments.push_back(depth_attachment_description);
		}

		//Backup Swapchain Attachment & Reference (Need it for MSAA)
		VkAttachmentDescription color_attachment_resolve = {};
		VkAttachmentReference color_attachment_resolve_reference = {};
		if (VkSwapchain::presetFlags & 0x02) {
			color_attachment_resolve.format = VkSwapchain::surfaceFormat.format;
			color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
			color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			color_attachment_resolve_reference.attachment = attachments.size();
			color_attachment_resolve_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			attachments.push_back(color_attachment_resolve);
		}

		//Setup the Subpass and Dependency
		VkSubpassDescription subpass_description = {};
		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_description.colorAttachmentCount = 1;
		subpass_description.pColorAttachments = &color_attachment_reference;
		subpass_description.pDepthStencilAttachment = (VkSwapchain::presetFlags & 0x01) ? &depth_attachment_reference : nullptr;
		subpass_description.pResolveAttachments = (VkSwapchain::presetFlags & 0x02) ? &color_attachment_resolve_reference : nullptr;

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
		render_pass_create_info.attachmentCount = attachments.size();
		render_pass_create_info.pAttachments = attachments.data();
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pSubpasses = &subpass_description;
		render_pass_create_info.dependencyCount = 1;
		render_pass_create_info.pDependencies = &subpass_dependency;

		VkResult r = vkCreateRenderPass(VkGlobal::device, &render_pass_create_info, nullptr, &VkSwapchain::renderPass);
		VK_ASSERT(r);
		return r;
	}
	VkResult CreateFramebuffer() {
		//Setup Variables
		VkResult r;
		std::vector<VkImageView> image_attachments;
		VkSwapchain::frameBuffer.resize(VkSwapchain::frameMax);

		//Loop through the Swapchain Frame Buffers and set their create info
		for (unsigned int i = 0; i < VkSwapchain::frameMax; ++i) {
			// Create an array of image attachments for create info (NOTE: There is only 1 Color Image View and Depth Buffer!)
			image_attachments.clear();
			if (VkSwapchain::presetFlags & 0x02) {
				image_attachments.push_back(VkSwapchain::msaaImageView);
				if (VkSwapchain::presetFlags & 0x01)
					image_attachments.push_back(VkSwapchain::depthImageView);
				image_attachments.push_back(VkSwapchain::swapchainImageView[i]);
			}
			else {
				image_attachments.push_back(VkSwapchain::swapchainImageView[i]);
				if (VkSwapchain::presetFlags & 0x01)
					image_attachments.push_back(VkSwapchain::depthImageView);
			}

			//Frame Buffer's Create Info
			VkFramebufferCreateInfo frame_buffer_create_info = {};
			frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frame_buffer_create_info.renderPass = VkSwapchain::renderPass;
			frame_buffer_create_info.attachmentCount = image_attachments.size();
			frame_buffer_create_info.pAttachments = image_attachments.data();
			frame_buffer_create_info.width = VkSwapchain::surfaceExtent2D.width;
			frame_buffer_create_info.height = VkSwapchain::surfaceExtent2D.height;
			frame_buffer_create_info.layers = 1;

			//Create the Surface (With Results) [VK_SUCCESS = 0]
			r = vkCreateFramebuffer(VkGlobal::device, &frame_buffer_create_info, nullptr, &VkSwapchain::frameBuffer[i]);
		}

		VkSwapchain::viewport.x = 0.0f;
		VkSwapchain::viewport.y = 0.0f;
		VkSwapchain::viewport.width = VkSwapchain::surfaceExtent2D.width;
		VkSwapchain::viewport.height = VkSwapchain::surfaceExtent2D.height;
		VkSwapchain::viewport.minDepth = 0.0f;
		VkSwapchain::viewport.maxDepth = 1.0f;

		VkSwapchain::scissor.offset = { 0,0 };
		VkSwapchain::scissor.extent = VkSwapchain::surfaceExtent2D;

		return r;
	}
	VkResult CreateDepthBuffer() {
		//Create the image and image view for Depth Buffer
		VkResult r = VkGlobal::CreateImage(VkSwapchain::depthFormat, VkSwapchain::surfaceExtent3D, 1, VkGlobal::msaa, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &VkSwapchain::depthImage, &VkSwapchain::depthMemory);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		r = VkGlobal::CreateImageView(VkSwapchain::depthImage, VkSwapchain::depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &VkSwapchain::depthImageView);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		//Transition the image layout from Undefined to Color Attachment (Optimal)
		r = VkGlobal::TransitionImageLayout(VkSwapchain::commandPool, VkGlobal::queueGraphics, 1, VkSwapchain::depthImage, VkSwapchain::depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		VK_ASSERT(r);
		return r;
	}
	VkResult CreateMSAABuffer() {
		//Create the image and image view for MSAA
		VkResult r = VkGlobal::CreateImage(VkSwapchain::surfaceFormat.format, VkSwapchain::surfaceExtent3D, 1, VkGlobal::msaa, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &VkSwapchain::msaaImage, &VkSwapchain::msaaMemory);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		r = VkGlobal::CreateImageView(VkSwapchain::msaaImage, VkSwapchain::surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &VkSwapchain::msaaImageView);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		//Transition the image layout from Undefined to Color Attachment (Optimal)
		r = VkGlobal::TransitionImageLayout(VkSwapchain::commandPool, VkGlobal::queueGraphics, 1, VkSwapchain::msaaImage, VkSwapchain::surfaceFormat.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		VK_ASSERT(r);
		return r;
	}
}

VkResult VkGlobal::CreateBuffer(const VkDeviceSize& _bufferSize, const VkBufferUsageFlags& _usageFlags, const VkMemoryPropertyFlags& _propertyFlags, VkBuffer* _outBuffer, VkDeviceMemory* _outBufferMemory)
{
	//Create Buffer Create Info
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = _bufferSize;
	buffer_create_info.usage = _usageFlags;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult r = vkCreateBuffer(VkGlobal::device, &buffer_create_info, nullptr, _outBuffer);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Gather Memory Information from image & Physical Device
	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(VkGlobal::device, *_outBuffer, &memory_requirements);
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(VkGlobal::physicalDevice, &memory_properties);

	//Loop through the memory type count and see if there is a match with both the filter and property flags
	int32_t memory_type_index = -1;
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
		if ((memory_requirements.memoryTypeBits & (1 << i)) &&
			(memory_properties.memoryTypes[i].propertyFlags & _propertyFlags) == _propertyFlags) {
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
	r = vkAllocateMemory(VkGlobal::device, &memory_allocate_info, VK_NULL_HANDLE, _outBufferMemory);
	if (r) {
		vkDestroyBuffer(VkGlobal::device, *_outBuffer, VK_NULL_HANDLE);
		return r;
	}

	//Bind the memory created
	r = vkBindBufferMemory(VkGlobal::device, *_outBuffer, *_outBufferMemory, 0);
	if (r) {
		vkDestroyBuffer(VkGlobal::device, *_outBuffer, VK_NULL_HANDLE);
		vkFreeMemory(VkGlobal::device, *_outBufferMemory, VK_NULL_HANDLE);
		return r;
	}

	//Image Creation has been successful!
	return r;
}

//VkGlobal Functions
VkResult VkGlobal::CreateImage(const VkFormat& _format, const VkExtent3D& _imageExtent, const uint32_t& _mipLevel, const VkSampleCountFlagBits& _samples, const VkImageTiling& _tiling, const VkImageUsageFlags& _usageFlags, const VkMemoryPropertyFlags& _memoryPropertyFlags, VkImage* _outImage, VkDeviceMemory* _outImageMemory) {
	//Create image info
	VkImageCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	create_info.imageType = VK_IMAGE_TYPE_2D;
	create_info.extent = _imageExtent;
	create_info.mipLevels = _mipLevel;
	create_info.arrayLayers = 1;
	create_info.format = _format;
	create_info.tiling = _tiling;
	create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	create_info.usage = _usageFlags;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.samples = _samples;
	create_info.flags = 0;

	//Create the image
	VkResult r = vkCreateImage(VkGlobal::device, &create_info, VK_NULL_HANDLE, _outImage);
	if (r) return r;

	//Gather Memory Information from image & Physical Device
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(VkGlobal::device, *_outImage, &memory_requirements);
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(VkGlobal::physicalDevice, &memory_properties);

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
	r = vkAllocateMemory(VkGlobal::device, &memory_allocate_info, VK_NULL_HANDLE, _outImageMemory);
	if (r) {
		vkDestroyImage(VkGlobal::device, *_outImage, VK_NULL_HANDLE);
		return r;
	}

	//Bind the memory created
	r = vkBindImageMemory(VkGlobal::device, *_outImage, *_outImageMemory, 0);
	if (r) {
		vkDestroyImage(VkGlobal::device, *_outImage, VK_NULL_HANDLE);
		vkFreeMemory(VkGlobal::device, *_outImageMemory, VK_NULL_HANDLE);
		return r;
	}

	//Image Creation has been successful!
	return r;
}
VkResult VkGlobal::CreateImageView(const VkImage& _image, const VkFormat& _format, const VkImageAspectFlags& _imageAspectFlags, VkImageView* _outImageView) {
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
	VkResult r = vkCreateImageView(VkGlobal::device, &create_info, nullptr, _outImageView);

	//Image View has been created successfully, return it
	return r;
}
VkResult VkGlobal::CopyBufferToImage(const VkCommandPool& _commandPool, const VkQueue& _queue, const VkBuffer& _buffer, const VkImage& _image, const VkExtent3D& _extent)
{
	//Setup the Command Buffer's Allocation Information
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandPool = _commandPool;
	command_buffer_allocate_info.commandBufferCount = 1;

	//Allocate the Command Buffer
	VkCommandBuffer command_buffer = VK_NULL_HANDLE;
	VkResult r = vkAllocateCommandBuffers(VkGlobal::device, &command_buffer_allocate_info, &command_buffer);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Start the command buffer's begin info
	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//Begin the Command Buffer's recording process
	r = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Setup The Buffer Image Copy
	VkBufferImageCopy buffer_image_copy = {};
	buffer_image_copy.bufferOffset = 0;
	buffer_image_copy.bufferRowLength = 0;
	buffer_image_copy.bufferImageHeight = 0;
	buffer_image_copy.imageSubresource.layerCount = 1;
	buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	buffer_image_copy.imageSubresource.mipLevel = 0;
	buffer_image_copy.imageSubresource.baseArrayLayer = 0;
	buffer_image_copy.imageOffset = { 0,0,0 };
	buffer_image_copy.imageExtent = _extent;

	//Send Command to Copy Buffer to Image
	vkCmdCopyBufferToImage(command_buffer, _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);

	//End the Command Buffer's recording Process
	r = vkEndCommandBuffer(command_buffer);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Create the submit info
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	//Submit The Commands Recorded into the Queue.
	r = vkQueueSubmit(_queue, 1, &submit_info, VK_NULL_HANDLE);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Wait for that specific Queue
	r = vkQueueWaitIdle(_queue);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Free the command buffer from memory
	vkFreeCommandBuffers(VkGlobal::device, _commandPool, 1, &command_buffer);

	//The Command Buffer has ended successfully!
	return VK_SUCCESS;
}
VkResult VkGlobal::TransitionImageLayout(const VkCommandPool& _commandPool, const VkQueue& _queue, const uint32_t& _mipLevel, const VkImage& _image, const VkFormat& _format, const VkImageLayout& _previousLayout, const VkImageLayout& _currentLayout) {
	//Setup the Command Buffer's Allocation Information
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandPool = _commandPool;
	command_buffer_allocate_info.commandBufferCount = 1;

	//Allocate the Command Buffer
	VkCommandBuffer command_buffer = VK_NULL_HANDLE;
	VkResult r = vkAllocateCommandBuffers(VkGlobal::device, &command_buffer_allocate_info, &command_buffer);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Start the command buffer's begin info
	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//Begin the Command Buffer's recording process
	r = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Create the image memory barrier
	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.image = _image;
	image_memory_barrier.oldLayout = _previousLayout;
	image_memory_barrier.newLayout = _currentLayout;
	image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = _mipLevel;
	image_memory_barrier.subresourceRange.baseArrayLayer = 0;
	image_memory_barrier.subresourceRange.layerCount = 1;

	//Setup the source and destination stage flags. Will be set based on the Old and New Layout set from outside
	VkPipelineStageFlags source_stage = VK_NULL_HANDLE;
	VkPipelineStageFlags destrination_stage = VK_NULL_HANDLE;

	if (_currentLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (_format == VK_FORMAT_D24_UNORM_S8_UINT || _format == VK_FORMAT_D32_SFLOAT_S8_UINT)
			image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	if (_previousLayout == VK_IMAGE_LAYOUT_UNDEFINED)
	{
		if (_currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destrination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (_currentLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destrination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (_currentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			image_memory_barrier.srcAccessMask = 0;
			image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destrination_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
	}
	else if (_previousLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _currentLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destrination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}

	vkCmdPipelineBarrier(command_buffer, source_stage, destrination_stage, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

	//End the Command Buffer's recording Process
	r = vkEndCommandBuffer(command_buffer);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Create the submit info
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	//Submit The Commands Recorded into the Queue.
	r = vkQueueSubmit(_queue, 1, &submit_info, VK_NULL_HANDLE);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Wait for that specific Queue
	r = vkQueueWaitIdle(_queue);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Free the command buffer from memory
	vkFreeCommandBuffers(VkGlobal::device, _commandPool, 1, &command_buffer);

	//The Command Buffer has ended successfully!
	return VK_SUCCESS;
}

//VkSwapchain Functions
VkResult VkSwapchain::UpdateSurfaceData() {
	//Gather The Surface Capabilities
	VkResult r = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkGlobal::physicalDevice, VkGlobal::surface, &VkGlobal::surfaceCapabilities);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Gather all Surface Formats
	uint32_t formatCount = 0;
	r = vkGetPhysicalDeviceSurfaceFormatsKHR(VkGlobal::physicalDevice, VkGlobal::surface, &formatCount, VK_NULL_HANDLE);
	if (formatCount < 1) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Resize Surface Formats Vector and Put the contents in it.
	VkGlobal::surfaceFormatsAll.resize(formatCount);
	r = vkGetPhysicalDeviceSurfaceFormatsKHR(VkGlobal::physicalDevice, VkGlobal::surface, &formatCount, VkGlobal::surfaceFormatsAll.data());
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Gather all Present Modes
	uint32_t presentModeCount = 0;
	r = vkGetPhysicalDeviceSurfacePresentModesKHR(VkGlobal::physicalDevice, VkGlobal::surface, &presentModeCount, VK_NULL_HANDLE);
	if (presentModeCount < 1) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Resize Present Modes Vector and Put the contents in it.
	VkGlobal::surfacePresentModesAll.resize(presentModeCount);
	r = vkGetPhysicalDeviceSurfacePresentModesKHR(VkGlobal::physicalDevice, VkGlobal::surface, &presentModeCount, VkGlobal::surfacePresentModesAll.data());
	VK_ASSERT(r);
	return r;
}
VkResult VkSwapchain::CreateCommandAndSyncBuffers()
{
	//Command Pool's Create Info
	VkCommandPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	create_info.queueFamilyIndex = VkGlobal::GRAPHICS_INDEX;

	VkResult r = vkCreateCommandPool(VkGlobal::device, &create_info, nullptr, &commandPool);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Allocate Command buffer Information
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = commandPool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = frameMax;

	//Create Command Buffer
	commandBuffer.resize(frameMax);
	r = vkAllocateCommandBuffers(VkGlobal::device, &command_buffer_allocate_info, commandBuffer.data());
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Semaphore Info Create
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Fence Info Create
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//Resize Semaphores
	renderSemaphore.resize(frameMax);
	presentSemaphore.resize(frameMax);
	fence.resize(frameMax);

	//Create the Semaphores and Fences
	for (unsigned int i = 0; i < frameMax; ++i) {
		r = vkCreateSemaphore(VkGlobal::device, &semaphore_create_info, nullptr, &renderSemaphore[i]);
		if (r) {
			VK_ASSERT(r);
			return r;
		}
		r = vkCreateSemaphore(VkGlobal::device, &semaphore_create_info, nullptr, &presentSemaphore[i]);
		if (r) {
			VK_ASSERT(r);
			return r;
		}
		r = vkCreateFence(VkGlobal::device, &fence_create_info, nullptr, &fence[i]);
		if (r) {
			VK_ASSERT(r);
			return r;
		}
	}

	//Semaphores and Fences has been created successfully!
	return r;
}
VkResult VkSwapchain::CreatePreset(const bool& _includeRenderPass)
{
	//Create The Swapchain
	VkResult r = ::CreateSwapchain();
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Create the Depth Buffer (If Needed)
	if (presetFlags & 0x01) {
		r = ::CreateDepthBuffer();
		if (r) {
			VK_ASSERT(r);
			return r;
		}
	}

	//Create the MSAA Buffer (If Needed)
	if (presetFlags & 0x02) {
		r = ::CreateMSAABuffer();
		if (r) {
			VK_ASSERT(r);
			return r;
		}
	}

	//Create the RenderPass
	if (_includeRenderPass) {
		r = ::CreateRenderPass();
		if (r) {
			VK_ASSERT(r);
			return r;
		}
	}

	//Create the Framebuffer
	r = ::CreateFramebuffer();
	VK_ASSERT(r);
	return r;
}
VkResult VkSwapchain::Destroy() {
	//Device Wait
	if (VkGlobal::device)
		vkDeviceWaitIdle(VkGlobal::device);

	//Destroy fence
	if (fence.size()) {
		for (auto f : fence) {
			vkDestroyFence(VkGlobal::device, f, nullptr);
		}

		fence.clear();
		fence.shrink_to_fit();
	}

	//Destroy Present Semaphore
	if (presentSemaphore.size()) {
		for (auto semaphore : presentSemaphore) {
			vkDestroySemaphore(VkGlobal::device, semaphore, nullptr);
		}

		presentSemaphore.clear();
		presentSemaphore.shrink_to_fit();
	}

	//Destroy Render Semaphore
	if (renderSemaphore.size()) {
		for (auto semaphore : renderSemaphore) {
			vkDestroySemaphore(VkGlobal::device, semaphore, nullptr);
		}

		renderSemaphore.clear();
		renderSemaphore.shrink_to_fit();
	}

	//Cleanup the Swapchain Stuff
	Cleanup(true);
	
	//Destroy Command Objects
	if (commandPool) {
		vkDestroyCommandPool(VkGlobal::device, commandPool, nullptr);
		commandPool = nullptr;
		commandBuffer.clear();
		commandBuffer.shrink_to_fit();
	}

	return VK_SUCCESS;
}
VkResult VkSwapchain::Cleanup(const bool &_includeRenderPass) {
	//Device Wait
	if (VkGlobal::device)
		vkDeviceWaitIdle(VkGlobal::device);

	//Destroy Framebuffer
	if (frameBuffer.size()) {
		for (auto fb : frameBuffer) {
			vkDestroyFramebuffer(VkGlobal::device, fb, nullptr);
		}

		frameBuffer.clear();
		frameBuffer.shrink_to_fit();
	}

	//Destroy Renderpass
	if (renderPass && _includeRenderPass) {
		vkDestroyRenderPass(VkGlobal::device, renderPass, nullptr);
		renderPass = nullptr;
	}

	//Destroy Depth Buffer & Co.
	if (msaaImage) {
		vkDestroyImageView(VkGlobal::device, msaaImageView, nullptr);
		vkDestroyImage(VkGlobal::device, msaaImage, nullptr);
		vkFreeMemory(VkGlobal::device, msaaMemory, nullptr);

		msaaImage = nullptr;
		msaaImageView = nullptr;
		msaaMemory = nullptr;
	}

	//Destroy Depth Buffer & Co.
	if (depthImage) {
		vkDestroyImageView(VkGlobal::device, depthImageView, nullptr);
		vkDestroyImage(VkGlobal::device, depthImage, nullptr);
		vkFreeMemory(VkGlobal::device, depthMemory, nullptr);

		depthImage = nullptr;
		depthImageView = nullptr;
		depthMemory = nullptr;
	}

	//Destroy Image View
	if (swapchainImageView.size()) {
		for (auto imageView : swapchainImageView) {
			vkDestroyImageView(VkGlobal::device, imageView, nullptr);
		}

		swapchainImageView.clear();
		swapchainImageView.shrink_to_fit();
	}

	return VK_SUCCESS;
}

//Vulkan Swapchain-Based Objects
VkSwapchainKHR VkSwapchain::swapchain = {};
std::vector<VkImage> VkSwapchain::swapchainImage;
std::vector<VkImageView> VkSwapchain::swapchainImageView;
VkRenderPass VkSwapchain::renderPass = {};
std::vector<VkFramebuffer> VkSwapchain::frameBuffer;
std::vector<VkSemaphore> VkSwapchain::renderSemaphore;
std::vector<VkSemaphore> VkSwapchain::presentSemaphore;
std::vector<VkFence> VkSwapchain::fence;

//Setup Command Pool & Command Buffers
VkCommandPool VkSwapchain::commandPool = {};
std::vector<VkCommandBuffer> VkSwapchain::commandBuffer;

//Additioanl Buffers (Depth)
VkFormat VkSwapchain::depthFormat = {};
VkImage VkSwapchain::depthImage = {};
VkImageView VkSwapchain::depthImageView = {};
VkDeviceMemory VkSwapchain::depthMemory = {};

//Additional Buffers (MSAA)
VkImage VkSwapchain::msaaImage = {};
VkImageView VkSwapchain::msaaImageView = {};
VkDeviceMemory VkSwapchain::msaaMemory = {};

//Current Surface Information
VkSurfaceCapabilitiesKHR VkSwapchain::surfaceCapabilities = {};
VkSurfaceFormatKHR VkSwapchain::surfaceFormat = {};
VkPresentModeKHR VkSwapchain::surfacePresentMode = {};
VkExtent2D VkSwapchain::surfaceExtent2D = {};
VkExtent3D VkSwapchain::surfaceExtent3D = {};
std::vector<VkClearValue> VkSwapchain::clearValue;
VkViewport VkSwapchain::viewport = {};
VkRect2D VkSwapchain::scissor = {};

//Frame-Based Data
uint32_t VkSwapchain::frameCurrent = {};
uint32_t VkSwapchain::frameMax = {};
uint32_t VkSwapchain::presetFlags = {};

//Vulkan Objects
VkInstance VkGlobal::instance = {};
VkSurfaceKHR VkGlobal::surface = {};
VkPhysicalDevice VkGlobal::physicalDevice = {};
VkSampleCountFlagBits VkGlobal::msaa = VK_SAMPLE_COUNT_1_BIT;
VkDevice VkGlobal::device = {};
VmaAllocator VkGlobal::allocator = {};
VkSwapchainKHR VkGlobal::swapchain = {};
VkRenderPass VkGlobal::renderPass = {};

//Queue Family Info
uint16_t VkGlobal::GRAPHICS_INDEX = 0; VkQueue VkGlobal::queueGraphics = {};
uint16_t VkGlobal::PRESENT_INDEX = 0;  VkQueue VkGlobal::queuePresent = {};
uint16_t VkGlobal::COMPUTE_INDEX = 0;  VkQueue VkGlobal::queueCompute = {};
uint16_t VkGlobal::TRANSFER_INDEX = 0; VkQueue VkGlobal::queueTransfer = {};
std::vector<uint32_t> VkGlobal::uniqueIndices;

//Vulkan Debug Objects
VkDebugReportCallbackEXT VkGlobal::debugReportCallback = {};

//Vulkan Extensions & Layers that are Active
std::vector<const char*> VkGlobal::instanceExtensionsActive;
std::vector<const char*> VkGlobal::instanceLayersActive;
std::vector<const char*> VkGlobal::deviceExtensionsActive;

//Vulkan ALL Extensions & Layers
std::vector<VkExtensionProperties> VkGlobal::instanceExtensionsAll;
std::vector<VkLayerProperties> VkGlobal::instanceLayersAll;
std::vector<VkPhysicalDevice> VkGlobal::physicalDeviceAll;

//Vulkan Physical Device Properties
std::unordered_map<VkPhysicalDevice, std::vector<VkExtensionProperties>> VkGlobal::physicalDeviceExtensionsAll;
std::unordered_map<VkPhysicalDevice, std::vector<VkQueueFamilyProperties>> VkGlobal::physicalDeviceQueueFamilyPropertiesAll;
std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceFeatures> VkGlobal::physicalDeviceFeaturesAll;
std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceProperties> VkGlobal::physicalDevicePropertiesAll;
std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceMemoryProperties> VkGlobal::physicalDeviceMemoryPropertiesAll;

//Vulkan Surface Properties
VkSurfaceCapabilitiesKHR VkGlobal::surfaceCapabilities = {};
std::vector<VkSurfaceFormatKHR> VkGlobal::surfaceFormatsAll;
std::vector<VkPresentModeKHR> VkGlobal::surfacePresentModesAll;
uint32_t VkGlobal::frameMax = {};
