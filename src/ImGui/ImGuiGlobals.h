struct VkImGui {
	//Render to Texture Necessities
	static VkImage image;
	static VkDeviceMemory memory;
	static VkImageView imageView;
	static VkRenderPass renderPass;
	static VkFramebuffer frameBuffer;
	static VkSampler sampler;
	static std::vector<VkClearValue> clearColor;

	//Pipeline to draw the texture
	static VkDescriptorPool descriptorPool;
	static VkDescriptorSetLayout descriptorSetLayout;
	static std::vector<VkDescriptorSet> descriptorSet;
	static VkPipelineLayout pipelineLayout;
	static VkPipeline graphicsPipeline;

	//Doing Commands and Synchronization
	static VkCommandPool commandPool;
	static std::vector<VkCommandBuffer> commandBuffer;
	static std::vector<VkFence> fence;
	static std::vector<VkSemaphore> semaphore;
	static ImGui_ImplVulkan_InitInfo init_info;

	//Static Functions
	static VkResult Init_vkImGui();
};
