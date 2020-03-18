struct VkGlobalObject {

	//Vulkan Objects
	VkInstance instance = {};
	VkSurfaceKHR surface = {};
	VkPhysicalDevice physicalDevice = {};
	VkDevice device = {};

	//Queue Family Info
	uint8_t GRAPHIC_INDEX = 0;  VkQueue queueGraphic = {};
	uint8_t PRESENT_INDEX = 0;  VkQueue queuePresent = {};
	uint8_t COMPUTE_INDEX = 0;  VkQueue queueCompute = {};

	//Vulkan Debug Objects
	VkDebugReportCallbackEXT debugReportCallback = {};

	//Vulkan Properties 
	std::vector<const char*> instanceExtensionsActive;
	std::vector<const char*> instanceLayersActive;
	std::vector<VkExtensionProperties> instanceExtensionsAll;
	std::vector<VkLayerProperties> instanceLayersAll;
	std::vector<VkPhysicalDevice> physicalDeviceAll;
};

extern VkGlobalObject vkGlobals;

//Defines for Windows
#if defined(_DEBUG)

	#if defined(_WIN32)
		#define VK_ASSERT(truth) if (truth) __debugbreak()
	#elif defined(__linux__)
		#define VK_ASSERT(truth) if (truth) raise(SIGTRAP);
	#endif

#else

	#if defined(_WIN32)
		#define VK_ASSERT(truth)
	#elif defined(__linux__)
		#define VK_ASSERT(truth)
	#endif

#endif