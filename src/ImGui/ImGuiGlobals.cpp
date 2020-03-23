#include "ImGuiGlobals.h"
#include "../Vulkan/VkGlobals.h"

//Render to Texture Necessities
VkImage VkImGui::image = {};
VkDeviceMemory VkImGui::imageMemory = {};
VkImageView VkImGui::imageView = {};
VkRenderPass VkImGui::renderPass = {};
VkFramebuffer VkImGui::frameBuffer = {};
VkSampler VkImGui::sampler = {};
std::vector<VkClearValue> VkImGui::clearColor;

//Pipeline to draw the texture
VkDescriptorPool VkImGui::descriptorPool = {};
VkDescriptorSetLayout VkImGui::descriptorSetLayout = {};
std::vector<VkDescriptorSet> VkImGui::descriptorSet = {};
VkPipelineLayout VkImGui::pipelineLayout = {};
VkPipeline VkImGui::graphicsPipeline = {};

//Doing Commands and Synchronization
VkCommandPool VkImGui::commandPool = {};
std::vector<VkCommandBuffer> VkImGui::commandBuffer;
std::vector<VkFence> VkImGui::fence = {};
std::vector<VkSemaphore> VkImGui::semaphore = {};
ImGui_ImplVulkan_InitInfo VkImGui::init_info = {};

namespace {
	VkPipelineCache pipelineCache = {};
	VkDescriptorPool descriptorPool = {};

	VkResult SetupDescriptorPool() {
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		return vkCreateDescriptorPool(VkGlobal::device, &pool_info, VK_NULL_HANDLE, &descriptorPool);
	}
	void check_vk_result(VkResult err) {
		if (err == 0) return;
#ifdef _DEBUG
		printf("VkResult %d\n", err);
#endif
		if (err < 0)
			abort();
	}

	VkResult SetupRenderPass() {
		//Primary Swapchain Description and Swapchain
		VkAttachmentDescription color_attachment_description = {};
		color_attachment_description.format = VK_FORMAT_B8G8R8A8_UNORM;
		color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference color_attachment_reference = {};
		color_attachment_reference.attachment = 0;
		color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//Setup the Subpass and Dependency
		VkSubpassDescription subpass_description = {};
		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_description.colorAttachmentCount = 1;
		subpass_description.pColorAttachments = &color_attachment_reference;

		VkSubpassDependency subpass_dependency = {};
		subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpass_dependency.dstSubpass = 0;
		subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpass_dependency.srcAccessMask = 0;
		subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		//Setup and Create the RenderPass
		VkRenderPassCreateInfo render_pass_create_info = {};
		render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_create_info.attachmentCount = 1;
		render_pass_create_info.pAttachments = &color_attachment_description;
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pSubpasses = &subpass_description;
		render_pass_create_info.dependencyCount = 1;
		render_pass_create_info.pDependencies = &subpass_dependency;

		return vkCreateRenderPass(VkGlobal::device, &render_pass_create_info, nullptr, &VkImGui::renderPass);
	}
	VkResult SetupCommandObjects() {
		//Setup ImGui's Command Pool & Buffer
		VkCommandPoolCreateInfo cpool_create_info = {};
		cpool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cpool_create_info.queueFamilyIndex = VkGlobal::GRAPHICS_INDEX;
		cpool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		cpool_create_info.pNext = VK_NULL_HANDLE;
		vkCreateCommandPool(VkGlobal::device, &cpool_create_info, VK_NULL_HANDLE, &VkImGui::commandPool);

		//Allocate Command buffer Information
		VkImGui::commandBuffer.resize(VkImGui::init_info.ImageCount);
		VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.commandPool = VkImGui::commandPool;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = 2;

		return vkAllocateCommandBuffers(VkGlobal::device, &command_buffer_allocate_info, VkImGui::commandBuffer.data());
	}
	VkResult SetupFonts() {
		// Use any command queue
		VkResult err = vkResetCommandPool(VkGlobal::device, VkImGui::commandPool, 0);
		::check_vk_result(err);
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(VkImGui::commandBuffer[0], &begin_info);
		::check_vk_result(err);

		ImGui_ImplVulkan_CreateFontsTexture(VkImGui::commandBuffer[0]);

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &VkImGui::commandBuffer[0];
		err = vkEndCommandBuffer(VkImGui::commandBuffer[0]);
		::check_vk_result(err);
		err = vkQueueSubmit(VkGlobal::queueGraphics, 1, &end_info, VK_NULL_HANDLE);
		::check_vk_result(err);

		err = vkDeviceWaitIdle(VkGlobal::device);
		::check_vk_result(err);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		err = vkResetCommandPool(VkGlobal::device, VkImGui::commandPool, 0);

		return err;
	}
	VkResult SetupImage() {
		VkResult r = VkGlobal::CreateImage(VK_FORMAT_B8G8R8A8_UNORM, VkSwapchain::surfaceExtent3D, VkGlobal::msaa, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &VkImGui::image, &VkImGui::imageMemory);
		r = VkGlobal::CreateImageView(VkImGui::image, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, &VkImGui::imageView);
		return VK_SUCCESS;
	}
	VkResult SetupFrameBuffer() {
		//Setup Variables
		VkResult r;

		//Frame Buffer's Create Info
		VkFramebufferCreateInfo frame_buffer_create_info = {};
		frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_buffer_create_info.renderPass = VkImGui::renderPass;
		frame_buffer_create_info.attachmentCount = 1;
		frame_buffer_create_info.pAttachments = &VkImGui::imageView;
		frame_buffer_create_info.width = VkGlobal::surfaceCapabilities.currentExtent.width;
		frame_buffer_create_info.height = VkGlobal::surfaceCapabilities.currentExtent.height;
		frame_buffer_create_info.layers = 1;

		//Create the Surface (With Results) [VK_SUCCESS = 0]
		r = vkCreateFramebuffer(VkGlobal::device, &frame_buffer_create_info, nullptr, &VkImGui::frameBuffer);

		return r;
	}
	VkResult SetupSyncObjects() {
		//Semaphore Info Create
		VkSemaphoreCreateInfo semaphore_create_info = {};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		//Fence Info Create
		VkFenceCreateInfo fence_create_info = {};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		//Resize Semaphores
		VkImGui::fence.resize(VkImGui::init_info.ImageCount);
		VkImGui::semaphore.resize(VkImGui::init_info.ImageCount);

		//Create the Semaphores and Fences
		VkResult r;
		for (unsigned int i = 0; i < VkImGui::init_info.ImageCount; ++i) {
			r = vkCreateSemaphore(VkGlobal::device, &semaphore_create_info, nullptr, &VkImGui::semaphore[i]);
			if (r) {
				return r;
			}
			r = vkCreateFence(VkGlobal::device, &fence_create_info, nullptr, &VkImGui::fence[i]);
			if (r) {
				return r;
			}
		}

		//Semaphores and Fences has been created successfully!
		return r;
	}
	VkResult SetupDescriptors() {
		//Descriptor Pool
		VkDescriptorPoolSize dps = {};
		dps.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dps.descriptorCount = 0xFF;

		VkDescriptorPoolCreateInfo dp_create_info = {};
		dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		dp_create_info.poolSizeCount = 1;
		dp_create_info.pPoolSizes = &dps;
		dp_create_info.maxSets = 0xFF;
		vkCreateDescriptorPool(VkGlobal::device, &dp_create_info, nullptr, &VkImGui::descriptorPool);

		//Descriptor Set Layout
		VkDescriptorSetLayoutBinding ps_img = {};
		ps_img.binding = 0;
		ps_img.descriptorCount = 1;
		ps_img.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		ps_img.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo dsl_create_info = {};
		dsl_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		dsl_create_info.bindingCount = 1;
		dsl_create_info.pBindings = &ps_img;
		vkCreateDescriptorSetLayout(VkGlobal::device, &dsl_create_info, nullptr, &VkImGui::descriptorSetLayout);

		//Descriptor Sets
		VkImGui::descriptorSet.resize(VkImGui::init_info.ImageCount);
		std::vector<VkDescriptorSetLayout> dsl_list(VkImGui::init_info.ImageCount, VkImGui::descriptorSetLayout);
		VkDescriptorSetAllocateInfo ds_allocate_info = {};
		ds_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		ds_allocate_info.descriptorSetCount = VkImGui::init_info.ImageCount;
		ds_allocate_info.descriptorPool = VkImGui::descriptorPool;
		ds_allocate_info.pSetLayouts = &dsl_list[0];
		vkAllocateDescriptorSets(VkGlobal::device, &ds_allocate_info, VkImGui::descriptorSet.data());

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

		vkCreateSampler(VkGlobal::device, &sampler_create_info, nullptr, &VkImGui::sampler);

		for (uint32_t i = 0; i < VkImGui::init_info.ImageCount; ++i)
		{
			VkDescriptorImageInfo dii = {};
			dii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			dii.imageView = VkImGui::imageView;
			dii.sampler = VkImGui::sampler;

			VkWriteDescriptorSet wds = {};
			wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			wds.dstSet = VkImGui::descriptorSet[i];
			wds.dstBinding = 0;
			wds.dstArrayElement = 0;
			wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			wds.descriptorCount = 1;
			wds.pBufferInfo = nullptr;
			wds.pImageInfo = &dii;
			wds.pTexelBufferView = nullptr;
			wds.pNext = nullptr;

			vkUpdateDescriptorSets(VkGlobal::device, 1, &wds, 0, nullptr);
		}

		return VK_SUCCESS;
	}
	VkResult SetupPipelines()
	{
		//Const Variables
		const uint32_t VERTEX = 0;
		const uint32_t FRAGMENT = 1;

		//Setup Shader Info
		VkShaderModule shader[2] = {};
		VkPipelineShaderStageCreateInfo stage_create_info[2] = {};

		//Create the GFile
		const char* vsFilename = "../../../src/ImGui/imgui.vert.spv";
		const char* psFilename = "../../../src/ImGui/imgui.frag.spv";

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
		rasterization_create_info.cullMode = VK_CULL_MODE_FRONT_BIT;
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

		//Descriptor pipeline layout
		VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = 1;
		pipeline_layout_create_info.pSetLayouts = &VkImGui::descriptorSetLayout;
		pipeline_layout_create_info.pushConstantRangeCount = 0;
		pipeline_layout_create_info.pPushConstantRanges = nullptr;
		r = vkCreatePipelineLayout(VkGlobal::device, &pipeline_layout_create_info, nullptr, &VkImGui::pipelineLayout);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

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

		pipeline_create_info.layout = VkImGui::pipelineLayout;
		pipeline_create_info.renderPass = VkSwapchain::renderPass;
		pipeline_create_info.subpass = 0;

		pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
		pipeline_create_info.basePipelineIndex = -1;

		r = vkCreateGraphicsPipelines(VkGlobal::device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &VkImGui::graphicsPipeline);
		if (r) {
			VK_ASSERT(r);
			return r;
		}

		//Cleanup
		vkDestroyShaderModule(VkGlobal::device, shader[VERTEX], nullptr);
		vkDestroyShaderModule(VkGlobal::device, shader[FRAGMENT], nullptr);

		return VK_SUCCESS;
	}
}

VkResult VkImGui::Init()
{
	//Setup the initinfo
	init_info = {};
	init_info.Instance = VkGlobal::instance;
	init_info.PhysicalDevice = VkGlobal::physicalDevice;
	init_info.Device = VkGlobal::device;
	init_info.QueueFamily = VkGlobal::GRAPHICS_INDEX;
	init_info.Queue = VkGlobal::queueGraphics;
	init_info.Allocator = VK_NULL_HANDLE;
	init_info.MinImageCount = VkSwapchain::surfaceCapabilities.minImageCount;
	init_info.ImageCount = VkSwapchain::frameMax;
	init_info.CheckVkResultFn = ::check_vk_result;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.PipelineCache = ::pipelineCache;

	//Setup Descriptor Pool
	::SetupDescriptorPool();
	init_info.DescriptorPool = ::descriptorPool;

	//Initialize ImGui - Vulkan
	VkImGui::clearColor.push_back({});
	::SetupRenderPass();
	if (!ImGui_ImplVulkan_Init(&init_info, VkImGui::renderPass)) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}

	//Setup Command Objects
	::SetupCommandObjects();

	//Setup ImGui Font
	::SetupFonts();

	//Setup Image
	::SetupImage();

	//Setup Framebuffer
	::SetupFrameBuffer();

	//Setup Sync Objects
	::SetupSyncObjects();

	//Setup Descriptor Set
	::SetupDescriptors();

	//Setup Pipelines
	::SetupPipelines();

	return VK_SUCCESS;
}
VkResult VkImGui::Cleanup() {
	//Wait for Device to finish
	vkDeviceWaitIdle(VkGlobal::device);

	//Destroy Pipelines
	if (VkImGui::graphicsPipeline) {
		vkDestroyPipeline(VkGlobal::device, VkImGui::graphicsPipeline, VK_NULL_HANDLE);
		VkImGui::graphicsPipeline = VK_NULL_HANDLE;
	}
	if (VkImGui::pipelineLayout) {
		vkDestroyPipelineLayout(VkGlobal::device, VkImGui::pipelineLayout, VK_NULL_HANDLE);
		VkImGui::pipelineLayout = VK_NULL_HANDLE;
	}

	//Cleanup Descriptor Stuff
	if (VkImGui::descriptorSetLayout) {
		vkDestroyDescriptorSetLayout(VkGlobal::device, VkImGui::descriptorSetLayout, VK_NULL_HANDLE);
		VkImGui::descriptorSetLayout = VK_NULL_HANDLE;
	}
	if (VkImGui::descriptorPool) {
		vkDestroyDescriptorPool(VkGlobal::device, VkImGui::descriptorPool, VK_NULL_HANDLE);
		VkImGui::descriptorPool = VK_NULL_HANDLE;
		VkImGui::descriptorSet.clear();
		VkImGui::descriptorSet.shrink_to_fit();
	}

	//Cleanup Sync Objects
	if (VkImGui::semaphore.size()) {
		for (auto semaphore : VkImGui::semaphore) {
			vkDestroySemaphore(VkGlobal::device, semaphore, VK_NULL_HANDLE);
		}
		VkImGui::semaphore.clear();
		VkImGui::semaphore.shrink_to_fit();
	}
	if (VkImGui::fence.size()) {
		for (auto fence : VkImGui::fence) {
			vkDestroyFence(VkGlobal::device, fence, VK_NULL_HANDLE);
		}
		VkImGui::fence.clear();
		VkImGui::fence.shrink_to_fit();
	}

	//Remove Framebuffer
	if (VkImGui::frameBuffer) {
		vkDestroyFramebuffer(VkGlobal::device, VkImGui::frameBuffer, VK_NULL_HANDLE);
		VkImGui::frameBuffer = VK_NULL_HANDLE;
	}

	//Remove Sampler
	if (VkImGui::sampler) {
		vkDestroySampler(VkGlobal::device, VkImGui::sampler, VK_NULL_HANDLE);
		VkImGui::sampler = VK_NULL_HANDLE;
	}

	//Remove Image View
	if (VkImGui::imageView) {
		vkDestroyImageView(VkGlobal::device, VkImGui::imageView, VK_NULL_HANDLE);
		VkImGui::imageView = VK_NULL_HANDLE;
	}

	//Remove Image
	if (VkImGui::image) {
		vkDestroyImage(VkGlobal::device, VkImGui::image, VK_NULL_HANDLE);
		vkFreeMemory(VkGlobal::device, VkImGui::imageMemory, VK_NULL_HANDLE);
		VkImGui::image = VK_NULL_HANDLE;
		VkImGui::imageMemory = 0;
	}

	//Remove Command Objects
	if (VkImGui::commandPool) {
		vkDestroyCommandPool(VkGlobal::device, VkImGui::commandPool, VK_NULL_HANDLE);
		VkImGui::commandPool = VK_NULL_HANDLE;
		VkImGui::commandBuffer.clear();
		VkImGui::commandBuffer.shrink_to_fit();
	}

	//Cleanup ImGui Vulkan
	ImGui_ImplVulkan_Shutdown();

	//Remove Renderpass
	if (VkImGui::renderPass) {
		vkDestroyRenderPass(VkGlobal::device, VkImGui::renderPass, VK_NULL_HANDLE);
		VkImGui::renderPass = VK_NULL_HANDLE;
	}

	//Remove Anonymouse Namespace's Descriptor Pool & Pipeline Cache
	if (::descriptorPool) {
		vkDestroyDescriptorPool(VkGlobal::device, ::descriptorPool, VK_NULL_HANDLE);
		::descriptorPool = VK_NULL_HANDLE;
	}
	if (::pipelineCache) {
		vkDestroyPipelineCache(VkGlobal::device, ::pipelineCache, VK_NULL_HANDLE);
		::pipelineCache = VK_NULL_HANDLE;
	}

	return VK_SUCCESS;
}