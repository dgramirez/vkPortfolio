namespace VkCore {
	//Initializing Vulkan
	bool vkInit(const GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE& uwh);

	//Cleanup Vulkan
	void vkCleanup();

	//Validation Layer Info
	namespace VkDebug {
		VkResult Init();
		VkResult Cleanup();
	}
}