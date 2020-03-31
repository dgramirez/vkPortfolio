#include "Shader.h"

VkShader* VkShader::Create(const char* _filepath) {
	return new VkShader(_filepath);
}

GW::SYSTEM::GFile VkShader::SetupGFile() {
	GW::GReturn GReturn;
	GW::SYSTEM::GFile GFile;
	GReturn = GFile.Create();
	assert(G_PASS(GReturn));

	//Open the File
	GReturn = GFile.OpenTextRead(m_SpirV.c_str());
	assert(G_PASS(GReturn));

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
	char buffer[128];
	char sub[128];
	uint32_t binding = 0xFFFFFFFF;
	uint32_t done = 0;
	VkDescriptorType type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
	while (+_gFile.ReadLine(&buffer[0], 128, '\n')) {
		//Create a String Stream and loop through each word
		std::stringstream stream(buffer);

		//Stream in the first word
		stream >> sub;

		//No Words & End of File Check
		if (sub[0] == 0) {
			++done;
			if (done == 3)
				break;
			continue;
		}
		done = 0;

		//Layout [If it is, continue the stream]
		if (!strcmp(sub, "layout")) {
			stream >> sub;
		}
		else
			continue;

		if (!strcmp(sub, "(std140,")) {
			type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			stream >> sub;
		}

		//Binding
		assert(strcmp(sub, "(binding=0)") && "Please space out the binding=# (binding = #)");
		if (!strcmp(sub, "(binding")) {
			// Need to get to the number
			stream >> sub;
			stream >> sub;

			//Extract the number
			binding = atoi(sub);

			//Continue the stream
			stream >> sub;
		}
		else
			continue;

		//Buffers
		if (!strcmp(sub, "uniform")) {
			//The Fun Part: Uniform Buffer or Image
			stream >> sub;

			//Sanity Check: binding is not max
			assert(binding != 0xFFFFFFFF);

			//UBO or Image Check
			if (type == VK_DESCRIPTOR_TYPE_MAX_ENUM) {
				if (!strcmp(sub, "sampler2D"))
					type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				else {
					stream >> sub;
					if (!strcmp(sub, "uboDynamic"))
						type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
					else
						type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				}
			}
			else if (type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
				if (!strcmp(sub, "storageDynamic"))
					type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			}

			assert(type != VK_DESCRIPTOR_TYPE_MAX_ENUM);

			//Add the bind to the vector
			VkDescriptorSetLayoutBinding img_bind = {};
			img_bind.binding = binding;
			img_bind.descriptorCount = 1;
			img_bind.descriptorType = type;
			img_bind.stageFlags = _shaderFlag;
			img_bind.pImmutableSamplers = VK_NULL_HANDLE;

			//Push Back Binding
			m_DSetLayoutBinding.push_back(img_bind);

			//Reset Variables
			binding = 0xFFFFFFFF;
			type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}
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

	//Cleanup GFile
	GFile.CloseFile();
	GFile = nullptr;
}
