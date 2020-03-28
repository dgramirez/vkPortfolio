#include "TextureScene.h"
#include "../../Vulkan/VkGlobals.h"
#include "../../ImGui/ImGuiGlobals.h"

TextureScene::TextureScene() {
	//1.) Update the Surface Information
	VkSwapchain::UpdateSurfaceData();

	//2.) Setup Surface Information for Swapchain
	VkSwapchain::surfaceCapabilities = VkGlobal::surfaceCapabilities;
	VkSwapchain::surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchain::surfaceExtent2D = VkSwapchain::surfaceCapabilities.currentExtent;
	VkSwapchain::surfaceExtent3D = { VkSwapchain::surfaceExtent2D.width, VkSwapchain::surfaceExtent2D.height, 1 };
	VkSwapchain::surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	VkSwapchain::surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

	//3.) Setup Other Swapchain Proeprties	
	VkSwapchain::presetFlags = 0x1;
	VkSwapchain::frameMax = 2;
	VkSwapchain::depthFormat = VK_FORMAT_D32_SFLOAT;
	VkSwapchain::clearValue.push_back({ 0.098f, 0.098f, 0.439f, 1.0f });

	//4.) Create a basic Swapchain
	VkSwapchain::CreateCommandAndSyncBuffers();
	VkSwapchain::CreatePreset();
	VkImGui::Init();

	//5.) Setup Smiley Texture
	Smiley.LoadTexture("../../../assets/SmileyFace/smileyface.png");

	//6.) Setup Uniform Buffer
	TexTech[0] = "None";
	TexTech[1] = "Gaussian Blur";
	TexTech[2] = "Swirling";
	TexTech[3] = "Pixelate";
	TexTech[4] = "Edge Detection";
	TexTech[5] = "Black & White";
	TexTech[6] = "Fish Eye";
	uniformBuffer.resize(VkSwapchain::frameMax);
	uniformMemory.resize(VkSwapchain::frameMax);
	for (uint32_t i = 0; i < VkSwapchain::frameMax; ++i)
		VkGlobal::CreateBuffer(sizeof(uniform),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&uniformBuffer[i], &uniformMemory[i]);
	DefaultUBOValues();

	//6.) Setup Descriptor Sets & Pipeline Layout
	SetupDescriptors();

	//7.) Setup Graphics Pipeline
	SetupGraphicsPipeline();
}
TextureScene::~TextureScene() {
	//Destroy Graphics Pipeline
	vkDestroyPipeline(VkGlobal::device, graphicsPipeline, VK_NULL_HANDLE);
	
	//Destroy Descriptors
	vkDestroyPipelineLayout(VkGlobal::device, pipelineLayout, VK_NULL_HANDLE);
	vkDestroySampler(VkGlobal::device, sampler, VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(VkGlobal::device, descriptorSetLayout, VK_NULL_HANDLE);
	vkDestroyDescriptorPool(VkGlobal::device, descriptorPool, VK_NULL_HANDLE);

	//Destroy Buffers
	for (uint32_t i = 0; i < VkSwapchain::frameMax; ++i) {
		vkDestroyBuffer(VkGlobal::device, uniformBuffer[i], VK_NULL_HANDLE);
		vkFreeMemory(VkGlobal::device, uniformMemory[i], VK_NULL_HANDLE);
	}

	//Destroy Smiley Texture
	Smiley.Free();
}

void TextureScene::Update(const float& _dt) {

}
void TextureScene::Render(const float& _dtRatio) {
	//Wait for Queue to be ready
	vkWaitForFences(VkGlobal::device, 1, &VkSwapchain::fence[VkSwapchain::frameCurrent], VK_TRUE, 0xFFFFFFFFFFFFFFFF);

	//Get the Frame Result
	VkResult frame_result = vkAcquireNextImageKHR(VkGlobal::device, VkSwapchain::swapchain, 0xFFFFFFFFFFFFFFFF, VkSwapchain::presentSemaphore[VkSwapchain::frameCurrent], VK_NULL_HANDLE, &VkSwapchain::frameCurrent);

	//Render to Texture ImGui
	FrameStart(VkImGui::commandBuffer[VkSwapchain::frameCurrent], VkImGui::renderPass, VkSwapchain::surfaceExtent2D, VkImGui::frameBuffer, VkImGui::clearColor);
		RenderImGui();
	FrameEnd(VkImGui::commandBuffer[VkSwapchain::frameCurrent], VkSwapchain::presentSemaphore[VkSwapchain::frameCurrent], VkImGui::semaphore[VkSwapchain::frameCurrent], VkImGui::fence[VkSwapchain::frameCurrent]);

	//Update Uniform Buffer
	VkGlobal::WriteToBuffer(uniform, uniformMemory[VkSwapchain::frameCurrent]);

	//Render to Swapchain
	FrameStart(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VkSwapchain::renderPass, VkSwapchain::surfaceExtent2D, VkSwapchain::frameBuffer[VkSwapchain::frameCurrent], VkSwapchain::clearValue);
		//Set Dynamic Viewport and Scissor
		vkCmdSetViewport(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], 0, 1, &VkSwapchain::viewport);
		vkCmdSetScissor(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], 0, 1, &VkSwapchain::scissor);
	
		//Draw Texture
		vkCmdBindDescriptorSets(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet[VkSwapchain::frameCurrent], 0, nullptr);
		vkCmdBindPipeline(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdDraw(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], 6, 1, 0, 0);

		//Draw ImGui
		vkCmdBindDescriptorSets(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VK_PIPELINE_BIND_POINT_GRAPHICS, VkImGui::pipelineLayout, 0, 1, &VkImGui::descriptorSet[VkSwapchain::frameCurrent], 0, nullptr);
		vkCmdBindPipeline(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VK_PIPELINE_BIND_POINT_GRAPHICS, VkImGui::graphicsPipeline);
		vkCmdDraw(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], 3, 1, 0, 0);
	FrameEnd(VkSwapchain::commandBuffer[VkSwapchain::frameCurrent], VkImGui::semaphore[VkSwapchain::frameCurrent], VkSwapchain::renderSemaphore[VkSwapchain::frameCurrent], VkSwapchain::fence[VkSwapchain::frameCurrent]);

	//Present
	Present();
}
void TextureScene::RenderImGui() {
	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Texturing Scene");

	//Start Tabbing
	if (ImGui::BeginTabBar("Scenes Bar", 0)) {

		//Scene Tab
		if (ImGui::BeginTabItem("Scenes")) {
			ImGui::Combo("Texture Technique", &uniform.activeEffect, TexTech, IM_ARRAYSIZE(TexTech));
			ImGui::DragFloat2("Global UV Offset", reinterpret_cast<float*>(&uniform.offsetUV), 0.001f, -1.0f, 1.0f);

			switch (uniform.activeEffect) {
			case 0: //No Texture Technique Applied
				break;
			case 1: //Gaussian Blur
				ImGui::DragFloat2("Blur Vertical Offset", reinterpret_cast<float*>(&uniform.gbOffset), 0.01f, -50.0f, 50.0f);
				ImGui::DragFloat2("Blur Horizontal Offset", reinterpret_cast<float*>(&uniform.gbOffsetH), 0.01f, -50.0f, 50.0f);
				ImGui::DragFloat3("Blur Weight", reinterpret_cast<float*>(&uniform.gbWeight), 0.001f, 0.0f, 1.0f);
				ImGui::DragFloat2("Blur UV Weight", reinterpret_cast<float*>(&uniform.gbUVWeight), 0.001f, 0.0f, 1.0f);
				break;
			case 2: //Swirling
				ImGui::DragFloat2("Swirl Texture Size", reinterpret_cast<float*>(&uniform.swTexSize), 1.0f, 0.0f, 200.0f);
				ImGui::DragFloat2("Swirl Center", reinterpret_cast<float*>(&uniform.swCenter), 1.0f, -100.0f, 100.0f);
				ImGui::DragFloat("Swirl Radius", &uniform.swRadius, 1.0f, -100.0f, 100.0f);
				ImGui::DragFloat("Swirl Angle", &uniform.swAngle, 0.001f, -3.142f, 3.142f);
				ImGui::DragFloat("Swirl Theta Factor", &uniform.swThetaFactor, 0.01f, -50.0f, 50.0f);
				break;
			case 3: //Pixelate
				ImGui::DragFloat("Pixel Size", &uniform.pxSize, 1.0f, -200.0f, 200.0f);
				break;
			case 4: //Edge Detection
				ImGui::DragFloat("Color Weight", &uniform.edColorWeight, 0.1f, -100, 100);
				ImGui::DragFloat2("Texture Offset", reinterpret_cast<float*>(&uniform.edTexOffset), 0.001f, 0, 1);
				break;
			case 5: //Black & White
				if (ImGui::Checkbox("GreyScaled?", &checkbox)) uniform.bwGreyScaled = checkbox;
				ImGui::DragFloat("Passing Value", &uniform.bwLumPassingValue, 0.001f, 0.0f, 1.0f);
				ImGui::DragFloat4("Lum. Coefficient", reinterpret_cast<float*>(&uniform.bwLumCoeff), 0.001f, 0.0f, 1.0f);
				break;
			case 6: //Fish-Eye
				ImGui::DragFloat("Aperature", &uniform.feAperature, 1.0f, -200.0f, 200.0f);
				break;
			}
			ImGui::EndTabItem();
		}
		
		//Options Tab
		if (ImGui::BeginTabItem("Options")) {
			ImGui::Text("Comming soon . . .");
			ImGui::EndTabItem();
		}

		//Credits Tab
		if (ImGui::BeginTabItem("Credits")) {
			ImGui::Text("Credits: ");
			ImGui::BulletText("OpenClipart-Vectors - Smiley Happy Face");
			ImGui::EndTabItem();
		}
		
		//End the Tab System
		ImGui::EndTabBar();
	}
	
	//Back
	ImGui::NewLine();
	bool backButton = ImGui::Button("<-");
	ImGui::SameLine();
	ImGui::Text("Back to Scene Menu");
	if (backButton)
		Scene::ChangeRoom = true;

	//Set to Render
	ImGui::End();
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), VkImGui::commandBuffer[VkSwapchain::frameCurrent]);
}
VkResult  TextureScene::SetupDescriptors() {
	//Descriptor Pool
	VkDescriptorPoolSize ubo_dps = {};
	ubo_dps.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_dps.descriptorCount = VkSwapchain::frameMax;

	VkDescriptorPoolSize img_dps = {};
	img_dps.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	img_dps.descriptorCount = VkSwapchain::frameMax;

	std::array<VkDescriptorPoolSize, 2> dps = { ubo_dps, img_dps };
	VkDescriptorPoolCreateInfo dp_create_info = {};
	dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	dp_create_info.poolSizeCount = dps.size();
	dp_create_info.pPoolSizes = dps.data();
	dp_create_info.maxSets = 2;
	VkResult r = vkCreateDescriptorPool(VkGlobal::device, &dp_create_info, nullptr, &descriptorPool);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Descriptor Set Layout
	VkDescriptorSetLayoutBinding ps_uniform = {};
	ps_uniform.binding = 0;
	ps_uniform.descriptorCount = 1;
	ps_uniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ps_uniform.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding ps_img = {};
	ps_img.binding = 1;
	ps_img.descriptorCount = 1;
	ps_img.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	ps_img.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo dsl_create_info = {};
	std::array<VkDescriptorSetLayoutBinding, 2> dsl_binds = { ps_uniform, ps_img };
	dsl_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_create_info.bindingCount = dsl_binds.size();
	dsl_create_info.pBindings = dsl_binds.data();;
	r = vkCreateDescriptorSetLayout(VkGlobal::device, &dsl_create_info, nullptr, &descriptorSetLayout);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Descriptor Sets
	descriptorSet.resize(VkSwapchain::frameMax);
	std::vector<VkDescriptorSetLayout> dsl_list(VkSwapchain::frameMax, descriptorSetLayout);
	VkDescriptorSetAllocateInfo ds_allocate_info = {};
	ds_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	ds_allocate_info.descriptorSetCount = VkSwapchain::frameMax;
	ds_allocate_info.descriptorPool = descriptorPool;
	ds_allocate_info.pSetLayouts = &dsl_list[0];
	r = vkAllocateDescriptorSets(VkGlobal::device, &ds_allocate_info, descriptorSet.data());
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Create Sampler
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.magFilter = VK_FILTER_LINEAR;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.anisotropyEnable = VK_TRUE;
	sampler_create_info.maxAnisotropy = static_cast<float>(VkGlobal::msaa);
	sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_create_info.unnormalizedCoordinates = VK_FALSE;
	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.minLod = 0.0f;
	sampler_create_info.maxLod = 1.0f;

	r = vkCreateSampler(VkGlobal::device, &sampler_create_info, nullptr, &sampler);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	VkDescriptorImageInfo dii = {};
	dii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	dii.imageView = Smiley.imageView;
	dii.sampler = sampler;

	for (uint32_t i = 0; i < VkSwapchain::frameMax; ++i)
	{
		VkDescriptorBufferInfo dbi = {};
		dbi.buffer = uniformBuffer[i];
		dbi.offset = 0;
		dbi.range = sizeof(uniform);

		std::array<VkWriteDescriptorSet, 2> wds;

		wds[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wds[0].dstSet = descriptorSet[i];
		wds[0].dstBinding = 0;
		wds[0].dstArrayElement = 0;
		wds[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		wds[0].descriptorCount = 1;
		wds[0].pBufferInfo = &dbi;
		wds[0].pImageInfo = nullptr;
		wds[0].pTexelBufferView = nullptr;
		wds[0].pNext = nullptr;

		wds[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wds[1].dstSet = descriptorSet[i];
		wds[1].dstBinding = 1;
		wds[1].dstArrayElement = 0;
		wds[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wds[1].descriptorCount = 1;
		wds[1].pBufferInfo = nullptr;
		wds[1].pImageInfo = &dii;
		wds[1].pTexelBufferView = nullptr;
		wds[1].pNext = nullptr;

		vkUpdateDescriptorSets(VkGlobal::device, wds.size(), wds.data(), 0, VK_NULL_HANDLE);
	}

	//Create Pipeline Layout
	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &descriptorSetLayout;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = nullptr;
	r = vkCreatePipelineLayout(VkGlobal::device, &pipeline_layout_create_info, nullptr, &pipelineLayout);
	VK_ASSERT(r);
	return r;
}
VkResult TextureScene::SetupGraphicsPipeline()
{
	//Const Variables
	const uint32_t VERTEX = 0;
	const uint32_t FRAGMENT = 1;

	//Setup Shader Info
	VkShaderModule shader[2] = {};
	VkPipelineShaderStageCreateInfo stage_create_info[2] = {};

	//Create the GFile
	const char* vsFilename = "../../../src/Scenes/A_Texture/shader.vert.spv";
	const char* psFilename = "../../../src/Scenes/A_Texture/shader.frag.spv";

	GW::SYSTEM::GFile ShaderFile; ShaderFile.Create();

	//Get the size of the Vertex Shader
	uint32_t vsFileSize;
	ShaderFile.GetFileSize(vsFilename, vsFileSize);

	//Open the Vertex Shader
	if (-ShaderFile.OpenBinaryRead(vsFilename)) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}

	//Copy the Contents of the Vertex Shader
	char* tempShaderFile = new char[vsFileSize];
	ShaderFile.Read(tempShaderFile, vsFileSize);

	//Create Shader Module for Vertex Shader
	VkShaderModuleCreateInfo vsModuleCreateInfo = {};
	vsModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vsModuleCreateInfo.codeSize = vsFileSize;
	vsModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(tempShaderFile);
	VkResult r = vkCreateShaderModule(VkGlobal::device, &vsModuleCreateInfo, VK_NULL_HANDLE, &shader[VERTEX]);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Create Stage Info for Vertex Shader
	stage_create_info[VERTEX].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage_create_info[VERTEX].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stage_create_info[VERTEX].module = shader[VERTEX];
	stage_create_info[VERTEX].pName = "main";

	//Cleanup
	delete[] tempShaderFile;
	ShaderFile.CloseFile();

	//Get the size of the Fragment Shader
	uint32_t psFileSize;
	ShaderFile.GetFileSize(psFilename, psFileSize);

	//Open the Fragment Shader
	if (-ShaderFile.OpenBinaryRead(psFilename)) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}

	//Copy the Contents of the Fragment Shader
	tempShaderFile = new char[psFileSize];
	ShaderFile.Read(tempShaderFile, psFileSize);

	//Create Shader Module for Fragment Shader
	VkShaderModuleCreateInfo psModuleCreateInfo = {};
	psModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	psModuleCreateInfo.codeSize = psFileSize;
	psModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(tempShaderFile);
	r = vkCreateShaderModule(VkGlobal::device, &psModuleCreateInfo, VK_NULL_HANDLE, &shader[FRAGMENT]);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Create Stage Info for Fragment Shader
	stage_create_info[FRAGMENT].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage_create_info[FRAGMENT].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stage_create_info[FRAGMENT].module = shader[FRAGMENT];
	stage_create_info[FRAGMENT].pName = "main";

	//Cleanup
	delete[] tempShaderFile;
	ShaderFile.CloseFile();

	//Assembly State
	VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
	assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assembly_create_info.primitiveRestartEnable = false;

	VkPipelineVertexInputStateCreateInfo input_vertex_info = {};
	input_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	input_vertex_info.vertexBindingDescriptionCount = 0;
	input_vertex_info.pVertexBindingDescriptions = VK_NULL_HANDLE;
	input_vertex_info.vertexAttributeDescriptionCount = 0;
	input_vertex_info.pVertexAttributeDescriptions = VK_NULL_HANDLE;

	//Viewport State
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = VkGlobal::surfaceCapabilities.currentExtent.width;
	viewport.height = VkGlobal::surfaceCapabilities.currentExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = VkGlobal::surfaceCapabilities.currentExtent;

	VkPipelineViewportStateCreateInfo viewport_create_info = {};
	viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_create_info.viewportCount = 1;
	viewport_create_info.pViewports = &viewport;
	viewport_create_info.scissorCount = 1;
	viewport_create_info.pScissors = &scissor;

	//Rasterizer State
	VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
	rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_create_info.lineWidth = 1.0f;
	rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_create_info.depthClampEnable = VK_FALSE;
	rasterization_create_info.depthBiasEnable = VK_FALSE;
	rasterization_create_info.depthBiasClamp = 0.0f;
	rasterization_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_create_info.depthBiasSlopeFactor = 0.0f;

	//Multisampling State
	VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
	multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_create_info.sampleShadingEnable = VK_FALSE;
	multisample_create_info.rasterizationSamples = VkGlobal::msaa;
	multisample_create_info.minSampleShading = 1.0f;
	multisample_create_info.pSampleMask = VK_NULL_HANDLE;
	multisample_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_create_info.alphaToOneEnable = VK_FALSE;

	//Depth-Stencil State
	VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {};
	depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_create_info.depthTestEnable = VK_FALSE;
	depth_stencil_create_info.depthWriteEnable = VK_FALSE;
	depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_create_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_create_info.minDepthBounds = 0.0f;
	depth_stencil_create_info.maxDepthBounds = 1.0f;
	depth_stencil_create_info.stencilTestEnable = VK_FALSE;

	//Color Blending Attachment & State
	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
	color_blend_attachment_state.colorWriteMask = 0xF; //<-- RGBA Flags on... although blend is disabled
	color_blend_attachment_state.blendEnable = VK_TRUE;
	color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_MAX;

	VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
	color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_create_info.logicOpEnable = VK_FALSE;
	color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
	color_blend_create_info.attachmentCount = 1;
	color_blend_create_info.pAttachments = &color_blend_attachment_state;
	color_blend_create_info.blendConstants[0] = 0.0f;
	color_blend_create_info.blendConstants[1] = 0.0f;
	color_blend_create_info.blendConstants[2] = 0.0f;
	color_blend_create_info.blendConstants[3] = 0.0f;

	//Dynamic State
	VkDynamicState dynamicState[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamic_create_info = {};
	dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_create_info.dynamicStateCount = 2;
	dynamic_create_info.pDynamicStates = dynamicState;

	//////////////////////////////////////////////////
	//												//
	//		FINALLY: GRAPHICS PIPELINE CREATION!	//
	//												//
	//////////////////////////////////////////////////

	VkGraphicsPipelineCreateInfo pipeline_create_info = {};
	pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.stageCount = 2;
	pipeline_create_info.pStages = stage_create_info;
	pipeline_create_info.pInputAssemblyState = &assembly_create_info;
	pipeline_create_info.pVertexInputState = &input_vertex_info;
	pipeline_create_info.pViewportState = &viewport_create_info;
	pipeline_create_info.pRasterizationState = &rasterization_create_info;
	pipeline_create_info.pMultisampleState = &multisample_create_info;
	pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
	pipeline_create_info.pColorBlendState = &color_blend_create_info;
	pipeline_create_info.pDynamicState = &dynamic_create_info;

	pipeline_create_info.layout = pipelineLayout;
	pipeline_create_info.renderPass = VkSwapchain::renderPass;
	pipeline_create_info.subpass = 0;

	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_create_info.basePipelineIndex = -1;

	r = vkCreateGraphicsPipelines(VkGlobal::device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &graphicsPipeline);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Cleanup
	vkDestroyShaderModule(VkGlobal::device, shader[VERTEX], nullptr);
	vkDestroyShaderModule(VkGlobal::device, shader[FRAGMENT], nullptr);

	return VK_SUCCESS;
}
void TextureScene::DefaultUBOValues() {
	//Global Options
	uniform.offsetUV[0] = 0.0f;
	uniform.offsetUV[1] = 0.0f;
	uniform.activeEffect = 0;
	
	//Gaussian Blur:
	uniform.gbOffset[0] = 1.384f;
	uniform.gbOffset[1] = 3.230f;
	uniform.gbOffsetH[0] = 1.384f;
	uniform.gbOffsetH[1] = 3.230f;

	uniform.gbWeight[0] = 0.227f;
	uniform.gbWeight[1] = 0.316f;
	uniform.gbWeight[2] = 0.070f;

	uniform.gbUVWeight[0] = 0.01f;
	uniform.gbUVWeight[1] = 0.01f;

	//Swirling:
	uniform.swTexSize[0] = 100.0f;
	uniform.swTexSize[1] = 100.0f;
	uniform.swCenter[0] = 30.0f;
	uniform.swCenter[1] = 30.0f;
	uniform.swRadius = 50.0f;
	uniform.swAngle = 0.8f;
	uniform.swThetaFactor = 8.0f;

	//Pixelate
	uniform.pxSize = 10.0f;

	//Edge Detection
	uniform.edColorWeight = 8.0f;
	uniform.edTexOffset[0] = 0.01f;
	uniform.edTexOffset[1] = 0.01f;

	//Black and White
	uniform.bwGreyScaled = checkbox = false;
	uniform.bwLumPassingValue = 0.8f;
	uniform.bwLumCoeff[0] = 0.299f;
	uniform.bwLumCoeff[1] = 0.587f;
	uniform.bwLumCoeff[2] = 0.114f;
	uniform.bwLumCoeff[3] = 0.000f;

	//Fish-Eye
	uniform.feAperature = 178.0f;
}
bool TextureScene::CheckRoomChange() {
	if (Scene::ChangeRoom) {
		Cleanup();
		Scene::ChangeRoom = false;
		return true;
	}
	return false;
}

void TextureScene::Reset() {
	//Cleanup
	VkImGui::CleanupImage();
	VkSwapchain::Cleanup(false);

	//Gather Info
	VkSwapchain::UpdateSurfaceData();
	VkSwapchain::surfaceCapabilities = VkGlobal::surfaceCapabilities;
	VkSwapchain::surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkSwapchain::surfaceExtent2D = VkSwapchain::surfaceCapabilities.currentExtent;
	VkSwapchain::surfaceExtent3D = { VkSwapchain::surfaceExtent2D.width, VkSwapchain::surfaceExtent2D.height, 1 };
	VkSwapchain::surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	VkSwapchain::surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	VkSwapchain::depthFormat = VK_FORMAT_D32_SFLOAT;

	//Reset
	VkSwapchain::CreatePreset(false);
	VkImGui::ResetImage();
}
void TextureScene::Cleanup() {
	Scene::Cleanup();
}
