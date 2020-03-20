#include "VkGlobals.h"

VkResult VkGlobalObject::CreateImage(const VkFormat& _format, const VkExtent3D& _imageExtent, const VkSampleCountFlagBits& _samples, const VkImageTiling& _tiling, const VkImageUsageFlags& _usageFlags, const VkMemoryPropertyFlags& _memoryPropertyFlags, VkImage* _outImage, VkDeviceMemory* _outImageMemory)
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
	VkResult r = vkCreateImage(vkGlobals.device, &create_info, VK_NULL_HANDLE, _outImage);
	if (r) return r;

	//Gather Memory Information from image & Physical Device
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(vkGlobals.device, *_outImage, &memory_requirements);
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(vkGlobals.physicalDevice, &memory_properties);

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
	r = vkAllocateMemory(vkGlobals.device, &memory_allocate_info, VK_NULL_HANDLE, _outImageMemory);
	if (r) {
		vkDestroyImage(vkGlobals.device, *_outImage, VK_NULL_HANDLE);
		return r;
	}

	//Bind the memory created
	r = vkBindImageMemory(vkGlobals.device, *_outImage, *_outImageMemory, 0);
	if (r) {
		vkDestroyImage(vkGlobals.device, *_outImage, VK_NULL_HANDLE);
		vkFreeMemory(vkGlobals.device, *_outImageMemory, VK_NULL_HANDLE);
		return r;
	}

	//Image Creation has been successful!
	return r;
}
VkResult VkGlobalObject::CreateImageView(const VkImage& _image, const VkFormat& _format, const VkImageAspectFlags& _imageAspectFlags, VkImageView* _outImageView)
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
	VkResult r = vkCreateImageView(vkGlobals.device, &create_info, nullptr, _outImageView);

	//Image View has been created successfully, return it
	return r;
}
