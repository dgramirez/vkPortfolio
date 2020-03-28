#ifndef TEXTURE_H
#define TEXTURE_H

struct Texture {
	VkExtent3D dimensions;
	uint32_t maxMip;
	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;

	Texture();
	Texture(const char* _filePath);
	~Texture();

	VkResult LoadTexture(const char* _filePath);
	void Free();
};

#endif