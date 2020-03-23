#ifndef SCENES_H
#define SCENES_H

class Scene {
public:
	Scene() = default;
	virtual ~Scene() = default;

	virtual void Update(const float& _dt) {}
	virtual void Render(const float& _dtRatio) {}

	virtual void Cleanup() {}

	static VkResult UpdateSurfaceData();

protected:
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR surfacePresentMode;
	VkExtent2D surfaceExtent2D;
	VkExtent3D surfaceExtent3D;

	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImage;
	std::vector<VkImageView> swapchainImageView;
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> swapchainFramebuffer;
	uint32_t frameMax;
	uint32_t frameCurrent;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffer;

	std::vector<VkFence> sceneFence;
	std::vector<VkSemaphore> sceneSemaphoreIA;
	std::vector<VkSemaphore> sceneSemaphoreRF;

	VkResult CreateSwapchainPresetBasic();	//No MSAA,	NoDepth
	VkResult CreateSwapchainPresetDepth();	//No MSAA,	Has Depth
	VkResult CreateSwapchainPresetMSAA();	//Has MSAA,	No Depth
	VkResult CreateSwapchainPresetAll();	//Has MSAA	Has Depth

	VkResult CreateCommandPreset();
	VkResult CreateSyncPreset();

	void FrameStart(const VkCommandBuffer& _commandBuffer, const VkRenderPass& _renderPass, const VkFramebuffer& _frameBuffer, const std::vector<VkClearValue>& _clearColor);
	void FrameEnd(const VkCommandBuffer& _commandBuffer, const VkSemaphore& _startSemaphore, const VkSemaphore& _nextSemaphore, const VkFence& _fence);
	virtual void Present();
	virtual void RenderImGui() {}
private:
	VkResult CreateSwapchain();
	VkResult CreateRenderPass(const bool& _depth = false, const bool& _msaa = false, const VkFormat& _depthFormat = VK_FORMAT_UNDEFINED);
	VkResult CreateFramebuffer(const bool& _depth = false, const bool& _msaa = false, const VkImageView& _depthView = VK_NULL_HANDLE, const VkImageView& _msaaView = VK_NULL_HANDLE);
};

class SceneMenu : public Scene {
public:
	SceneMenu(Scene*& _pScene);
	~SceneMenu() {}

	void RenderImGui() override;
	template<typename T>
	void RegisterScene(const char* test_name) {
		m_Scenes.push_back(std::make_pair(test_name, []() { return new T(); }));
	}

private:
	Scene*& m_CurrentScene;
	std::vector<std::pair<const char*, std::function<Scene*()>>> m_Scenes;
};


#endif
