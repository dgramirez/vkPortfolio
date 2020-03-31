#ifndef SHADER_H
#define SHADER_H

class VkShader {
public:
	static VkShader* Create(const char* _filepath);

private:
	//Member Variables
	std::string m_SpirV;
	VkDescriptorPool m_DPool;
	VkDescriptorSetLayout m_DSetLayout;

	//Helper Methods
	std::vector<VkDescriptorPoolSize> m_DPoolSize;
	std::vector<VkDescriptorSetLayoutBinding> m_DSetLayoutBinding;
	GW::SYSTEM::GFile SetupGFile();
	void SetupExtension(VkShaderStageFlags& _shaderFlag);
	void FillBindings(const VkShaderStageFlags& _shaderFlag, GW::SYSTEM::GFile& _gFile);

	//No Creation or Copies of this.
	VkShader(const char* _filepath);
	VkShader(const VkShader& _shader) {}
	void operator=(const VkShader& _shader) {}
};

#endif
