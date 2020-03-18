namespace VkCore {
	//Initializing Vulkan
	bool vkInit();

	//Cleanup Vulkan
	void vkCleanup();

	//Validation Layer Info
	namespace VkDebug {
		VkResult Init();
		VkResult Cleanup();
	}
}