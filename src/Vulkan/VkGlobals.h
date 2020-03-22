#include "imgui_impl_vulkan.h"

struct VkGlobal {
	//Vulkan Objects
	static VkInstance instance;
	static VkSurfaceKHR surface;
	static VkPhysicalDevice physicalDevice;
	static VkSampleCountFlagBits msaa;
	static VkDevice device;
	static VmaAllocator allocator;
	static VkSwapchainKHR swapchain;
	static VkRenderPass renderPass;

	//Queue Family Info
	static uint16_t GRAPHICS_INDEX; static VkQueue queueGraphics;
	static uint16_t PRESENT_INDEX;  static VkQueue queuePresent;
	static uint16_t COMPUTE_INDEX;  static VkQueue queueCompute;
	static uint16_t TRANSFER_INDEX; static VkQueue queueTransfer;
	static std::vector<uint32_t> uniqueIndices;

	//Vulkan Debug Objects
	static VkDebugReportCallbackEXT debugReportCallback;

	//Vulkan Extensions & Layers that are Active
	static std::vector<const char*> instanceExtensionsActive;
	static std::vector<const char*> instanceLayersActive;
	static std::vector<const char*> deviceExtensionsActive;

	//Vulkan ALL Extensions & Layers
	static std::vector<VkExtensionProperties> instanceExtensionsAll;
	static std::vector<VkLayerProperties> instanceLayersAll;
	static std::vector<VkPhysicalDevice> physicalDeviceAll;

	//Vulkan Physical Device Properties
	static std::unordered_map<VkPhysicalDevice,std::vector<VkExtensionProperties>> physicalDeviceExtensionsAll;
	static std::unordered_map<VkPhysicalDevice,std::vector<VkQueueFamilyProperties>> physicalDeviceQueueFamilyPropertiesAll;
	static std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceFeatures> physicalDeviceFeaturesAll;
	static std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceProperties> physicalDevicePropertiesAll;
	static std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceMemoryProperties> physicalDeviceMemoryPropertiesAll;

	//Vulkan Surface Properties
	static VkSurfaceCapabilitiesKHR surfaceCapabilities ;
	static std::vector<VkSurfaceFormatKHR> surfaceFormatsAll;
	static std::vector<VkPresentModeKHR> surfacePresentModesAll;
	static uint32_t frameMax;

	//Helper Methods
	static VkResult CreateImage(const VkFormat& _format, const VkExtent3D& _imageExtent, const VkSampleCountFlagBits& _samples, const VkImageTiling& _tiling, const VkImageUsageFlags& _usageFlags, const VkMemoryPropertyFlags& _memoryPropertyFlags, VkImage* _outImage, VkDeviceMemory* _outImageMemory);
	static VkResult CreateImageView(const VkImage& _image, const VkFormat& _format, const VkImageAspectFlags& _imageAspectFlags, VkImageView* _outImageView);
};

struct ImGuiGlobal {
	ImGui_ImplVulkan_InitInfo init_info = {};
	VkPipelineCache pipelineCache;
	VkDescriptorPool descriptorPoolImGui;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffer;

	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	VkRenderPass renderPass;
	VkFramebuffer frameBuffer;
	VkSampler sampler;
	std::vector<VkClearValue> clearColor;

	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSet;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkFence> fence;
	std::vector<VkSemaphore> semaphore;

	static void check_vk_result(VkResult err);
	static VkResult Init_vkImGui();
};
extern ImGuiGlobal vkImGui;

//Defines for Windows
#if defined(_DEBUG)

	#if defined(_WIN32)
		#define VK_ASSERT(vkResult) if (vkResult) __debugbreak()
	#elif defined(__linux__)
		#include <signal.h>
		#define VK_ASSERT(vkResult) if (vkResult) raise(SIGTRAP);
	#endif

#else

	#if defined(_WIN32)
		#define VK_ASSERT(vkResult)
	#elif defined(__linux__)
		#define VK_ASSERT(vkResult)
	#endif

#endif