struct VkGlobalObject {
	//Vulkan Objects
	VkInstance instance = {};
	VkSurfaceKHR surface = {};

	//Vulkan Debug Objects
	VkDebugReportCallbackEXT debugReportCallback = {};

	//Vulkan Properties 
	std::vector<const char*> instanceExtensionsActive;
	std::vector<const char*> instanceLayersActive;
	std::vector<VkExtensionProperties> instanceExtensionsAll;
	std::vector<VkLayerProperties> instanceLayersAll;
};

extern VkGlobalObject vkGlobals;
