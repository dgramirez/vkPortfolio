#include "Shader.h"
#include "../Vulkan/VkGlobals.h"

void* operator new(size_t size) {
	return malloc(size);
}

VkShader* VkShader::Create(const char* _filepath) {
	return new VkShader(_filepath);
}

void VkShader::Bind(const VkCommandBuffer& _commandBuffer, const VkDescriptorSet& _descriptorSet) {
	vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &_descriptorSet, 0, nullptr);
}

GW::SYSTEM::GFile VkShader::SetupGFile() {
	GW::GReturn GReturn;
	GW::SYSTEM::GFile GFile;
	GReturn = GFile.Create();
	VK_ASSERT(!G_PASS(GReturn));

	//Open the File
	GReturn = GFile.OpenTextRead(m_SpirV.c_str());
	VK_ASSERT(!G_PASS(GReturn));

	return GFile;
}
void VkShader::SetupExtension(VkShaderStageFlags& _shaderFlag) {
	//Find out the file extension
	const char* fileExtension = m_SpirV.data() + (m_SpirV.size() - 4);
	if (!strcmp(fileExtension, "vert"))
		_shaderFlag = VK_SHADER_STAGE_VERTEX_BIT;
	else if (!strcmp(fileExtension, "frag"))
		_shaderFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
	else if (!strcmp(fileExtension, "comp"))
		_shaderFlag = VK_SHADER_STAGE_COMPUTE_BIT;
	else if (!strcmp(fileExtension, "geom"))
		_shaderFlag = VK_SHADER_STAGE_GEOMETRY_BIT;
	else if (!strcmp(fileExtension, "tesc"))
		_shaderFlag = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	else if (!strcmp(fileExtension, "tese"))
		_shaderFlag = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	else if (!strcmp(fileExtension, "glsl"))
		_shaderFlag = VK_SHADER_STAGE_ALL;

	//Add the spv File Extension
	m_SpirV += ".spv";
}
void VkShader::FillBindings(const VkShaderStageFlags& _shaderFlag, GW::SYSTEM::GFile& _gFile) {
	//Loop Setup
	char buffer[255];
	std::string sub(255, 0);
	uint32_t binding = 0xFFFFFFFF;
	uint32_t done = 0;
	const size_t neg_one_sizet = 0xFFFFFFFFFFFFFFFF; // ("0 - 1" underflows to max bits)
	VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;

	//Loop through to find all bindings
	while (+_gFile.ReadLine(&buffer[0], 255, '\n')) {
		//Create a String Stream and loop through each word
		std::stringstream stream(buffer);

		//Stream in the first word
		stream >> sub;

		//No Words & End of File Check
		if (!sub[0]) {
			++done;
			if (done == 3)
				break;
			continue;
		}
		done = 0;

		//Layout
		if (sub.find("layout") == neg_one_sizet) {
			sub.clear();
			stream.clear();
			continue;
		}

		uint32_t stage = 0;
		do {
			if (stage < 2) {
				//std140
				if (sub.find("std140") < neg_one_sizet)
					type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

				if ((sub.find("binding") < neg_one_sizet) || stage == 1) {
					if (sub.find("=") < neg_one_sizet) {
						const char* num = buffer + stream.tellg() - 2;
						while (num[0] < '0' || num[0] > '9')
							++num;
						binding = atoi(num);
						stage = 2;
					}
					else
						stage = 1;
				}
			}
			else if (stage > 1) {
				if (sub.find("buffer") < neg_one_sizet) {
					stream >> sub;

					if (sub.find("dyn_") < neg_one_sizet)
						type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

					stage = 3;
				}
				else if (sub.find("uniform") < neg_one_sizet) {
					stream >> sub;

					if (sub.find("sampler2D") < neg_one_sizet)
						type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					else if (sub.find("dyn_") < neg_one_sizet)
						type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
					else
						type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

					stage = 3;
				}
			}
			if (stage == 3) {
				VkDescriptorSetLayoutBinding dsl_binds = {};
				dsl_binds.binding = binding;
				dsl_binds.descriptorCount = 1;
				dsl_binds.descriptorType = type;
				dsl_binds.stageFlags = _shaderFlag;

				m_DSetLayoutBinding.push_back(dsl_binds);
				stage = -1;
			}
		} while (stream >> sub);

		sub.clear();
		stream.clear();
	}
}
void VkShader::SetupDescriptors() {
	//Setup Descriptor Pool Sizes
	VkDescriptorPoolSize uboPS = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER , };
	VkDescriptorPoolSize dynUboPS = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC , };
	VkDescriptorPoolSize imgPS = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER , };
	VkDescriptorPoolSize stgPS = { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER , };
	VkDescriptorPoolSize dynStgPS = { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC , };
	for (uint32_t i = 0; i < m_DSetLayoutBinding.size(); ++i) {
		switch (m_DSetLayoutBinding[i].descriptorType) {
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			uboPS.descriptorCount += 3; //Up to 3 in flight, but want no swapchain dependency.
			break;
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			dynUboPS.descriptorCount += 3; //Up to 3 in flight, but want no swapchain dependency.
			break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			++imgPS.descriptorCount += 3; //Up to 3 in flight, but want no swapchain dependency.
			break;
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			stgPS.descriptorCount += 3; //Up to 3 in flight, but want no swapchain dependency.
			break;
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			dynStgPS.descriptorCount += 3; //Up to 3 in flight, but want no swapchain dependency.
			break;
		default:
			VK_ASSERT(true);
			break;
		}
	}

	//Put them in a vector
	std::vector<VkDescriptorPoolSize> dps;
	if (uboPS.descriptorCount)
		dps.push_back(uboPS);
	if (dynUboPS.descriptorCount)
		dps.push_back(dynUboPS);
	if (imgPS.descriptorCount)
		dps.push_back(imgPS);
	if (stgPS.descriptorCount)
		dps.push_back(stgPS);
	if (dynStgPS.descriptorCount)
		dps.push_back(dynStgPS);

	//Create the Descriptor Pool
	VkDescriptorPoolCreateInfo dp_create_info = {};
	dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	dp_create_info.poolSizeCount = dps.size();
	dp_create_info.pPoolSizes = dps.data();
	dp_create_info.maxSets = 0xF;
	VkResult r = vkCreateDescriptorPool(VkGlobal::device, &dp_create_info, nullptr, &m_DPool);
	VK_ASSERT(r);

	//Create the Descriptor Set Layout
	VkDescriptorSetLayoutCreateInfo dsl_create_info = {};
	dsl_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_create_info.bindingCount = m_DSetLayoutBinding.size();
	dsl_create_info.pBindings = m_DSetLayoutBinding.data();;
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

//Private
VkShader::VkShader(const char* _filepath) {
	//SpirV Setup
	uint32_t file_length = strlen(_filepath);
	m_SpirV.reserve(file_length + 4); //4 for .spv
	m_SpirV = _filepath;

	//Create the GFile
	GW::SYSTEM::GFile GFile = SetupGFile();

	//Setup the SpirV Extension
	VkShaderStageFlags shader_flag = {};
	SetupExtension(shader_flag);

	//Fill in the Bindings
	FillBindings(shader_flag, GFile);

	//Setup Descriptors
	SetupDescriptors();

	//Cleanup GFile
	GFile.CloseFile();
	GFile = nullptr;
}
