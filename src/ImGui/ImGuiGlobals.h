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