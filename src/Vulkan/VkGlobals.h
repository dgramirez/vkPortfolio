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
	static VkSurfaceCapabilitiesKHR surfaceCapabilities;
	static std::vector<VkSurfaceFormatKHR> surfaceFormatsAll;
	static std::vector<VkPresentModeKHR> surfacePresentModesAll;
	static uint32_t frameMax;

	//Helper Methods
	static VkResult CreateImage(const VkFormat& _format, const VkExtent3D& _imageExtent, const VkSampleCountFlagBits& _samples, const VkImageTiling& _tiling, const VkImageUsageFlags& _usageFlags, const VkMemoryPropertyFlags& _memoryPropertyFlags, VkImage* _outImage, VkDeviceMemory* _outImageMemory);
	static VkResult CreateImageView(const VkImage& _image, const VkFormat& _format, const VkImageAspectFlags& _imageAspectFlags, VkImageView* _outImageView);
	static VkResult TransitionImageLayout(const VkCommandPool& _commandPool, const VkImage& _image, const VkFormat& _format, const VkImageLayout& _previousLayout, const VkImageLayout& _currentLayout);
};

struct VkSwapchain {
	//Vulkan Swapchain-Based Objects
	static VkSwapchainKHR swapchain;
	static std::vector<VkImage> swapchainImage;
	static std::vector<VkImageView> swapchainImageView;
	static VkRenderPass renderPass;
	static std::vector<VkFramebuffer> frameBuffer;
	static std::vector<VkSemaphore> renderSemaphore;
	static std::vector<VkSemaphore> presentSemaphore;
	static std::vector<VkFence> fence;

	//Setup Command Pool & Command Buffers
	static VkCommandPool commandPool;
	static std::vector<VkCommandBuffer> commandBuffer;

	//Additioanl Buffers (Depth)
	static VkFormat depthFormat;
	static VkImage depthImage;
	static VkImageView depthImageView;
	static VkDeviceMemory depthMemory;

	//Additional Buffers (MSAA)
	static VkImage msaaImage;
	static VkImageView msaaImageView;
	static VkDeviceMemory msaaMemory;

	//Current Surface Information
	static VkSurfaceCapabilitiesKHR surfaceCapabilities;
	static VkSurfaceFormatKHR surfaceFormat;
	static VkPresentModeKHR surfacePresentMode;
	static VkExtent2D surfaceExtent2D;
	static VkExtent3D surfaceExtent3D;
	static std::vector<VkClearValue> clearValue;
	static VkViewport viewport;
	static VkRect2D scissor;

	//Frame-Based Data
	static uint32_t frameCurrent;
	static uint32_t frameMax;
	static uint32_t presetFlags;

	//Swapchain Functions
	static VkResult UpdateSurfaceData();
	static VkResult CreateCommandAndSyncBuffers();
	static VkResult CreatePreset(const bool& _includeRenderPass = true);
	static VkResult Destroy();
	static VkResult Cleanup(const bool &_includeRenderPass = false);
};

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