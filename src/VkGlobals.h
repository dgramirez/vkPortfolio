struct VkGlobalObject {

	//Vulkan Objects
	VkInstance instance = {};
	VkSurfaceKHR surface = {};
	VkPhysicalDevice physicalDevice = {};
	VkDevice device = {};
	VmaAllocator allocator = {};

	//Queue Family Info
	uint16_t GRAPHICS_INDEX = 0; VkQueue queueGraphics = {};
	uint16_t PRESENT_INDEX = 0;  VkQueue queuePresent = {};
	uint16_t COMPUTE_INDEX = 0;  VkQueue queueCompute = {};
	uint16_t TRANSFER_INDEX = 0; VkQueue queueTransfer = {};

	//Vulkan Debug Objects
	VkDebugReportCallbackEXT debugReportCallback = {};

	//Vulkan Properties 
	std::vector<const char*> instanceExtensionsActive;
	std::vector<const char*> instanceLayersActive;
	std::vector<const char*> deviceExtensionsActive;
	std::vector<VkExtensionProperties> instanceExtensionsAll;
	std::vector<VkLayerProperties> instanceLayersAll;
	std::vector<VkPhysicalDevice> physicalDeviceAll;
	std::unordered_map<VkPhysicalDevice,std::vector<VkExtensionProperties>> physicalDeviceExtensionsAll;
	std::unordered_map<VkPhysicalDevice,std::vector<VkQueueFamilyProperties>> physicalDeviceQueueFamilyPropertiesAll;
	std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceFeatures> physicalDeviceFeaturesAll;
	std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceProperties> physicalDevicePropertiesAll;
	std::unordered_map<VkPhysicalDevice, VkPhysicalDeviceMemoryProperties> physicalDeviceMemoryPropertiesAll;
	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
};

extern VkGlobalObject vkGlobals;

//Defines for Windows
#if defined(_DEBUG)

	#if defined(_WIN32)
		#define VK_ASSERT(truth) if (truth) __debugbreak()
	#elif defined(__linux__)
		#include <signal.h>
		#define VK_ASSERT(truth) if (truth) raise(SIGTRAP);
	#endif

#else

	#if defined(_WIN32)
		#define VK_ASSERT(truth)
	#elif defined(__linux__)
		#define VK_ASSERT(truth)
	#endif

#endif