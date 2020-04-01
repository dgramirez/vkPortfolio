#include "Descriptors.h"
#include "../Vulkan/VkGlobals.h"

VkDescriptor* VkDescriptor::Create(const VkShader* _shader1, const VkShader* _shader2) {
	return new VkDescriptor(_shader1, _shader2);
}

VkDescriptor::VkDescriptor(const VkShader* _shader1, const VkShader* _shader2) {
	//There has to be a Shader in first parameter
	VK_ASSERT(!_shader1);

	//The first parameter must be a compute or vertex
	VK_ASSERT(!(_shader1->GetShaderStage() & (VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT)));

	//The second parameter, if Vertex, must be a fragment shader
	if (_shader1->GetShaderStage() & VK_SHADER_STAGE_VERTEX_BIT) {
		VK_ASSERT(!_shader2);
		VK_ASSERT(!(_shader2->GetShaderStage() & VK_SHADER_STAGE_FRAGMENT_BIT));
	}

	//Push back the shaders
	m_Shader.push_back(_shader1);
	if (_shader2)
		m_Shader.push_back(_shader2);
}

void VkDescriptor::AddShader(const VkShader* _shader1, const VkShader* _shader2) {
	//There has to be a Shader in first parameter
	VK_ASSERT(!_shader1);

	//The first parameter must be a compute or vertex
	VK_ASSERT(!(_shader1->GetShaderStage() & (VK_SHADER_STAGE_ALL & ~(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT))));

	//The second parameter, if Tessellation Control, must be a Tessellation Evaluation
	if (_shader1->GetShaderStage() & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT) {
		VK_ASSERT(!_shader2);
		VK_ASSERT(!(_shader2->GetShaderStage() & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT));
	}

	//Push back the shaders
	m_Shader.push_back(_shader1);
	if (_shader2)
		m_Shader.push_back(_shader2);
}

void VkDescriptor::Init() {
	//Get all the descriptor pool size
	std::vector<VkDescriptorPoolSize> dps;
	for (uint32_t i = 0; i < m_Shader.size(); ++i) {
		
	}

	//Create the Descriptor Pool
	VkDescriptorPoolCreateInfo dp_create_info = {};
	dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	dp_create_info.poolSizeCount = dps.size();
	dp_create_info.pPoolSizes = dps.data();
	dp_create_info.maxSets = 0xF;
	VkResult r = vkCreateDescriptorPool(VkGlobal::device, &dp_create_info, nullptr, &m_DPool);
	VK_ASSERT(r);

	//Get all the Descriptor Set Layout Bindings
	std::vector<VkDescriptorSetLayoutBinding> dsl;
	for (uint32_t i = 0; i < m_Shader.size(); ++i) {

	}

	//Create the Descriptor Set Layout
	VkDescriptorSetLayoutCreateInfo dsl_create_info = {};
	dsl_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_create_info.bindingCount = dsl.size();
	dsl_create_info.pBindings = dsl.data();
	r = vkCreateDescriptorSetLayout(VkGlobal::device, &dsl_create_info, nullptr, &m_DSetLayout);
	VK_ASSERT(r);

	//Create Pipeline Layout
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &m_DSetLayout;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = nullptr;
	r = vkCreatePipelineLayout(VkGlobal::device, &pipeline_layout_create_info, nullptr, &m_PipelineLayout);
	VK_ASSERT(r);
}

void VkDescriptor::Bind(const VkCommandBuffer& _commandBuffer, const VkDescriptorSet& _descriptorSet) {
	vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &_descriptorSet, 0, nullptr);
}
