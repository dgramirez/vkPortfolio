#include "Shader.h"

class VkDescriptor {
public:
	static VkDescriptor* Create(const VkShader* _shader1, const VkShader* _shader2 = nullptr);
	void AddShader(const VkShader* _shader1, const VkShader* _shader2 = nullptr);
	void Init();
	void Bind(const VkCommandBuffer& _commandBuffer, const VkDescriptorSet& _descriptorSet);

private:
	//Member Variables
	std::vector<const VkShader*> m_Shader;
	VkDescriptorPool m_DPool;
	VkDescriptorSetLayout m_DSetLayout;
	VkPipelineLayout m_PipelineLayout;

	//Constructors
	VkDescriptor() {}
	VkDescriptor(const VkShader* _shader1, const VkShader* _shader2);
	VkDescriptor(const VkDescriptor& _descriptor) {}
	VkDescriptor(VkDescriptor&& _descriptor) {}
	void operator=(const VkDescriptor& _descriptor) {}
	void operator=(VkDescriptor&& _descriptor) {}
};
