#ifndef SHADER_H
#define SHADER_H

class VkShader {
public:
	~VkShader();
	static VkShader* Create(const char* _filepath);
	const char* GetSpirVFilepath() const;
	VkShaderStageFlags GetShaderStage() const;
	std::vector<VkDescriptorPoolSize> GetDescriptorPool() const;
	std::vector<VkDescriptorSetLayoutBinding> GetDescriptorSetLayout() const;

private:
	//Member Variables
	std::string m_SpirV;
	std::vector<VkDescriptorPoolSize> m_DPoolSize;
	std::vector<VkDescriptorSetLayoutBinding> m_DSetLayoutBinding;
	VkShaderStageFlags m_ShaderStage;

	//Helper Methods
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
