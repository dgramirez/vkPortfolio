#ifndef TEXTURE_H
#define TEXTURE_H

struct Texture {
	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;
	VkExtent3D dimensions;
	uint32_t maxMip;

	Texture();
	Texture(const char* _filePath);
	~Texture();

	VkResult LoadTexture(const char* _filePath);
	void Free();
};

#endif