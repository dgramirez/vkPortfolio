struct VkGlobalObject {

	//Vulkan Objects
	VkInstance instance = {};
	VkSurfaceKHR surface = {};
	VkPhysicalDevice physicalDevice = {};
	VkDevice device = {};
	VmaAllocator allocator = {};
	VkSwapchainKHR swapchain = {};

	//Queue Family Info
	uint16_t GRAPHICS_INDEX = 0; VkQueue queueGraphics = {};
	uint16_t PRESENT_INDEX = 0;  VkQueue queuePresent = {};
	uint16_t COMPUTE_INDEX = 0;  VkQueue queueCompute = {};
	uint16_t TRANSFER_INDEX = 0; VkQueue queueTransfer = {};
	std::vector<uint32_t> uniqueIndices;

	//Vulkan Debug Objects
	VkDebugReportCallbackEXT debugReportCallback = {};

	//Vulkan Extensions & Layers that are Active
	std::vector<const char*> instanceExtensionsActive;
	std::vector<const char*> instanceLayersActive;
	std::vector<const char*> deviceExtensionsActive;

	//Vulkan ALL Extensions & Layers
	std::vector<VkExtensionProperties> instanceExtensionsAll;
	std::vector<VkLayerProperties> instanceLayersAll;
	std::vector<VkPhysicalDevice> physicalDeviceAll;

	//Vulkan Physical Device Properties
	std::unordered_map<VkPhysicalDevice,std::vector<VkExtensionProperties>> physicalDeviceExtensionsAll;
	std::unordered_map<VkPhysicalDevice,std::vector<VkQueueFamilyProperties>> physicalDeviceQueueFamilyPropertiesAll;
	std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceFeatures> physicalDeviceFeaturesAll;
	std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceProperties> physicalDevicePropertiesAll;
	std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceMemoryProperties> physicalDeviceMemoryPropertiesAll;

	//Vulkan Surface Properties
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormatsAll;
	std::vector<VkPresentModeKHR> surfacePresentModesAll;
};

extern VkGlobalObject vkGlobals;

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