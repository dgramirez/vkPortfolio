#include "Texture.h"
#include "../Vulkan/VkGlobals.h"

namespace {
	bool LoadTexture(const char* _filePath, void** _texture, uint32_t& _width, uint32_t& _height, uint32_t& _maxMipLevel) {
		int x, y, c;
		*_texture = stbi_load(_filePath, &x, &y, &c, 4);
		if (!_texture)
			return false;

		_width = x, _height = y;
		_maxMipLevel = log2(std::min(_width, _height));
		return true;
	}
	VkResult CreateMipMaps(const VkImage& _image, const VkExtent3D& _dimensions, const uint32_t& _mipLevel) {
		//Setup the Command Buffer's Allocation Information
		VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandPool = VkSwapchain::commandPool;
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

		//Create the Image Memory Barrier for Mipmapping
		VkImageMemoryBarrier image_memory_barrier = {};
		image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_memory_barrier.image = _image;
		image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_memory_barrier.subresourceRange.baseArrayLayer = 0;
		image_memory_barrier.subresourceRange.layerCount = 1;
		image_memory_barrier.subresourceRange.levelCount = 1;

		//Save the texture width and height for Mip levels
		int32_t width = _dimensions.width;
		int32_t height = _dimensions.height;

		//Loop for every Mip levels. NOTE: (i-1) is the current mip level, while (i) is the next mip level
		for (uint32_t i = 1; i < _mipLevel; ++i) {
			//Setup the current mip level for blitting the image
			image_memory_barrier.subresourceRange.baseMipLevel = i - 1;
			image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			//Transfer the layout and Access Mask Information
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr, 0, nullptr, 1, &image_memory_barrier);

			//Create the Blit Image. Src is (i-1), or current mip level. dst is (i), or next mip level.
			VkImageBlit image_blit = {};
			image_blit.srcSubresource.mipLevel = i - 1;
			image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_blit.srcSubresource.baseArrayLayer = 0;
			image_blit.srcSubresource.layerCount = 1;
			image_blit.srcOffsets[0] = { 0, 0, 0 };
			image_blit.srcOffsets[1] = { width, height, 1 };

			image_blit.dstSubresource.mipLevel = i;
			image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_blit.dstSubresource.baseArrayLayer = 0;
			image_blit.dstSubresource.layerCount = 1;
			image_blit.dstOffsets[0] = image_blit.srcOffsets[0];
			image_blit.dstOffsets[1] = { width > 1 ? (width >> 1) : 1 , height > 1 ? (height >> 1) : 1, 1 };

			//Blit the texture
			vkCmdBlitImage(command_buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &image_blit, VK_FILTER_LINEAR);

			//Set the layout and Access Mask (again) for the shader to read
			image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			//Transfer the layout and Access Mask Information (Again, based on above values)
			vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

			//Reduce the Mip level down by 1 level [By cutting width and height in half]
			if (width > 1) { width >>= 1; }
			if (height > 1) { height >>= 1; }
		}

		image_memory_barrier.subresourceRange.baseMipLevel = _mipLevel - 1;
		image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

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
		r = vkQueueSubmit(VkGlobal::queueGraphics, 1, &submit_info, VK_NULL_HANDLE);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		//Wait for that specific Queue
		r = vkQueueWaitIdle(VkGlobal::queueGraphics);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		//Free the command buffer from memory
		vkFreeCommandBuffers(VkGlobal::device, VkSwapchain::commandPool, 1, &command_buffer);

		//The Command Buffer has ended successfully!
		return VK_SUCCESS;
	}
}

Texture::Texture() {
	dimensions = {0, 0, 1};
	maxMip = 0;
	image = VK_NULL_HANDLE;
	imageMemory = {};
	imageView = VK_NULL_HANDLE;
}

Texture::Texture(const char* _filePath) {
	dimensions = {0, 0, 1};
	maxMip = 0;
	image = VK_NULL_HANDLE;
	imageMemory = {};
	imageView = VK_NULL_HANDLE;
	LoadTexture(_filePath);
}

Texture::~Texture() {
	Free();
}

VkResult Texture::LoadTexture(const char* _filePath) {
	//Load the texture
	void* texture;
	::LoadTexture(_filePath, &texture, dimensions.width, dimensions.height, maxMip);
	if (!texture)
		return VK_ERROR_FEATURE_NOT_PRESENT;

	//Create a Staging Buffer
	VkDeviceSize imageSize = dimensions.width * dimensions.height * sizeof(uint32_t);
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkResult r = VkGlobal::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Transfer the data into the buffer
	void* data = nullptr;
	r = vkMapMemory(VkGlobal::device, stagingBufferMemory, 0, imageSize, 0, &data);
	if (r) {
		VK_ASSERT(r);
		return r;
	}
	memcpy(data, texture, imageSize);
	vkUnmapMemory(VkGlobal::device, stagingBufferMemory);

	//Create the Image
	r = VkGlobal::CreateImage(VK_FORMAT_R8G8B8A8_UNORM, dimensions, maxMip, VkGlobal::msaa, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &image, &imageMemory);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Transition the Layout
	r = VkGlobal::TransitionImageLayout(VkSwapchain::commandPool, VkGlobal::queueGraphics, maxMip, image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Copy the buffer to the image
	r = VkGlobal::CopyBufferToImage(VkSwapchain::commandPool, VkGlobal::queueGraphics, stagingBuffer, image, dimensions);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Free up the staging buffer memory
	vkDestroyBuffer(VkGlobal::device, stagingBuffer, nullptr);
	vkFreeMemory(VkGlobal::device, stagingBufferMemory, nullptr);
	stbi_image_free(texture);

	//Create the Mip Maps
	r = CreateMipMaps(image, dimensions, maxMip);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Create the Image View
	r = VkGlobal::CreateImageView(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &imageView);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	return VK_SUCCESS;
}

void Texture::Free() {
	if (imageView) {
		vkDestroyImageView(VkGlobal::device, imageView, VK_NULL_HANDLE);
		imageView = VK_NULL_HANDLE;
	}

	if (image) {
		vkDestroyImage(VkGlobal::device, image, VK_NULL_HANDLE);
		vkFreeMemory(VkGlobal::device, imageMemory, VK_NULL_HANDLE);
		image = VK_NULL_HANDLE;
		imageMemory = {};
	}

	dimensions = {};
	maxMip = 0;
}
