#ifndef SHADER_H
#define SHADER_H

class VkShader {
public:
	static VkShader* Create(const char* _filepath);
	void Bind(const VkCommandBuffer& _commandBuffer, const VkDescriptorSet& _descriptorSet);

private:
	//Member Variables
	std::string m_SpirV;
	VkDescriptorPool m_DPool;
	VkDescriptorSetLayout m_DSetLayout;
	VkPipelineLayout m_PipelineLayout;
	uint32_t countUBO;
	uint32_t countImage;

	//Helper Methods
	std::vector<VkDescriptorSetLayoutBinding> m_DSetLayoutBinding;
	GW::SYSTEM::GFile SetupGFile();
	void SetupExtension(VkShaderStageFlags& _shaderFlag);
	void FillBindings(const VkShaderStageFlags& _shaderFlag, GW::SYSTEM::GFile& _gFile);
	void SetupDescriptors();

	//No Creation or Copies of this.
	VkShader(const char* _filepath);
	VkShader(const VkShader& _shader) {}
	void operator=(const VkShader& _shader) {}
};

#endif
