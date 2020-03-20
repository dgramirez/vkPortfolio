#include "Scenes.h"
#include "../VkGlobals.h"

////////////////////////
// Scene Menu Methods //
////////////////////////
SceneMenu::SceneMenu(Scene*& _pScene)
	: m_CurrentScene(_pScene) { }

void SceneMenu::RenderImGui() {

}

///////////////////
// Scene Methods //
///////////////////
VkResult Scene::UpdateSurfaceData() {
	//Gather The Surface Capabilities
	VkResult r = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkGlobals.physicalDevice, vkGlobals.surface, &vkGlobals.surfaceCapabilities);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Gather all Surface Formats
	uint32_t formatCount = 0;
	r = vkGetPhysicalDeviceSurfaceFormatsKHR(vkGlobals.physicalDevice, vkGlobals.surface, &formatCount, VK_NULL_HANDLE);
	if (formatCount < 1) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}
	if (r) {
		VK_ASSERT(r);
		return r;
	}
	//Resize Surface Formats Vector and Put the contents in it.
	vkGlobals.surfaceFormatsAll.resize(formatCount);
	r = vkGetPhysicalDeviceSurfaceFormatsKHR(vkGlobals.physicalDevice, vkGlobals.surface, &formatCount, vkGlobals.surfaceFormatsAll.data());
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Gather all Present Modes
	uint32_t presentModeCount = 0;
	r = vkGetPhysicalDeviceSurfacePresentModesKHR(vkGlobals.physicalDevice, vkGlobals.surface, &presentModeCount, VK_NULL_HANDLE);
	if (presentModeCount < 1) {
		VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}
	if (r) {
		VK_ASSERT(r);
		return r;
	}
	//Resize Present Modes Vector and Put the contents in it.
	vkGlobals.surfacePresentModesAll.resize(presentModeCount);
	r = vkGetPhysicalDeviceSurfacePresentModesKHR(vkGlobals.physicalDevice, vkGlobals.surface, &presentModeCount, vkGlobals.surfacePresentModesAll.data());
	if (r) { 
		VK_ASSERT(r);
	}

	return r;
}
VkResult Scene::CreateSwapchainPresetBasic()
{
	VkResult r = CreateSwapchain();
	r = CreateRenderPass();
	r = CreateFramebuffer();
	return r;
}
VkResult Scene::CreateSwapchain() {
	//Gather Swapchain Count
	if (surfaceCapabilities.minImageCount > 0 && frameMax < surfaceCapabilities.minImageCount)
		frameMax = vkGlobals.surfaceCapabilities.minImageCount;
	if (vkGlobals.surfaceCapabilities.maxImageCount > 0 && frameMax > vkGlobals.surfaceCapabilities.maxImageCount)
		frameMax = surfaceCapabilities.maxImageCount;

	//Create Info for SwapchainKHR [Part 1]
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = vkGlobals.surface;
	create_info.minImageCount = frameMax;
	create_info.imageFormat = surfaceFormat.format;
	create_info.imageColorSpace = surfaceFormat.colorSpace;
	create_info.imageExtent = surfaceExtent2D;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	create_info.preTransform = surfaceCapabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = surfacePresentMode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = vkGlobals.swapchain;

	//Setup Correct Queue Family Indices
	if (vkGlobals.uniqueIndices.size() > 1) {
		create_info.queueFamilyIndexCount = vkGlobals.uniqueIndices.size();
		create_info.pQueueFamilyIndices = vkGlobals.uniqueIndices.data();
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	}
	else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	//Create Swapchain
	VkResult r = vkCreateSwapchainKHR(vkGlobals.device, &create_info, nullptr, &swapchain);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Swapchain Image Setup
	r = vkGetSwapchainImagesKHR(vkGlobals.device, swapchain, &frameMax, VK_NULL_HANDLE);
	if (r) {
		VK_ASSERT(r);
		return r;
	}
	swapchainImage.resize(frameMax);
	r = vkGetSwapchainImagesKHR(vkGlobals.device, swapchain, &frameMax, swapchainImage.data());
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Obtain the Image and Image Views
	swapchainImageView.resize(frameMax);
	for (uint32_t i = 0; i < frameMax; ++i) {
		r = VkGlobalObject::CreateImageView(swapchainImage[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &swapchainImageView[i]);
		if (r) {
			VK_ASSERT(r);
			return r;
		}
	}

	//Set Current Frame to 0
	frameCurrent = 0;

	//Return result (VK_SUCCESS)
	return r;
}
VkResult Scene::CreateRenderPass(const bool& _depth, const bool& _msaa, const VkFormat& _depthFormat)
{
	std::vector<VkAttachmentDescription> attachments;

	//Primary Swapchain Description and Swapchain
	VkAttachmentDescription color_attachment_description = {};
	color_attachment_description.format = surfaceFormat.format;
	color_attachment_description.samples = vkGlobals.msaa;
	color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment_description.finalLayout = (_msaa) ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_attachment_reference = {};
	color_attachment_reference.attachment = attachments.size();
	color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachments.push_back(color_attachment_description);

	//Depth Swapchain Attachment & Reference
	VkAttachmentDescription depth_attachment_description = {};
	VkAttachmentReference depth_attachment_reference = {};
	if (_depth) {
		depth_attachment_description.format = _depthFormat;
		depth_attachment_description.samples = vkGlobals.msaa;
		depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		depth_attachment_reference.attachment = attachments.size();
		depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachments.push_back(depth_attachment_description);
	}

	//Backup Swapchain Attachment & Reference (Need it for MSAA)
	VkAttachmentDescription color_attachment_resolve = {};
	VkAttachmentReference color_attachment_resolve_reference = {};
	if (_msaa) {
		color_attachment_resolve.format = surfaceFormat.format;
		color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		color_attachment_resolve_reference.attachment = attachments.size();
		color_attachment_resolve_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachments.push_back(color_attachment_resolve);
	}

	//Setup the Subpass and Dependency
	VkSubpassDescription subpass_description = {};
	subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description.colorAttachmentCount = 1;
	subpass_description.pColorAttachments = &color_attachment_reference;
	subpass_description.pDepthStencilAttachment = (_depth) ? &depth_attachment_reference : nullptr;
	subpass_description.pResolveAttachments = (_msaa) ? &color_attachment_resolve_reference : nullptr;

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
	render_pass_create_info.attachmentCount = attachments.size();
	render_pass_create_info.pAttachments = attachments.data();
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pSubpasses = &subpass_description;
	render_pass_create_info.dependencyCount = 1;
	render_pass_create_info.pDependencies = &subpass_dependency;

	VkResult r = vkCreateRenderPass(vkGlobals.device, &render_pass_create_info, nullptr, &renderPass);

	return r;
}
VkResult Scene::CreateFramebuffer(const bool& _depth, const bool& _msaa, const VkImageView& _depthView, const VkImageView& _msaaView)
{
	//Setup Variables
	VkResult r;
	std::vector<VkImageView> image_attachments;
	swapchainFramebuffer.resize(frameMax);

	//Loop through the Swapchain Frame Buffers and set their create info
	for (unsigned int i = 0; i < frameMax; ++i) {
		//Clear Image Attachments
		image_attachments.clear();

		// Create an array of image attachments for create info (NOTE: There is only 1 Color Image View and Depth Buffer!)
		if (_msaa) {
			image_attachments.push_back(_msaaView);
			if (_depth)
				image_attachments.push_back(_depthView);
			image_attachments.push_back(swapchainImageView[i]);
		}
		else {
			image_attachments.push_back(swapchainImageView[i]);
			if (_depth)
				image_attachments.push_back(_depthView);
		}

		//Frame Buffer's Create Info
		VkFramebufferCreateInfo frame_buffer_create_info = {};
		frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_buffer_create_info.renderPass = renderPass;
		frame_buffer_create_info.attachmentCount = image_attachments.size();
		frame_buffer_create_info.pAttachments = image_attachments.data();
		frame_buffer_create_info.width = surfaceExtent2D.width;
		frame_buffer_create_info.height = surfaceExtent2D.height;
		frame_buffer_create_info.layers = 1;

		//Create the Surface (With Results) [VK_SUCCESS = 0]
		r = vkCreateFramebuffer(vkGlobals.device, &frame_buffer_create_info, nullptr, &swapchainFramebuffer[i]);
	}

	return r;
}
VkResult Scene::CreateCommandPreset()
{
	//Command Pool's Create Info
	VkCommandPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	create_info.queueFamilyIndex = vkGlobals.GRAPHICS_INDEX;

	VkResult r = vkCreateCommandPool(vkGlobals.device, &create_info, nullptr, &commandPool);
	if (r) {
		VK_ASSERT(r);
		return r;
	}

	//Allocate Command buffer Information
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = commandPool;
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate_info.commandBufferCount = frameMax;

	//Create Command Buffer
	commandBuffer.resize(frameMax);
	r = vkAllocateCommandBuffers(vkGlobals.device, &command_buffer_allocate_info, &commandBuffer[0]);
	if (r)
		VK_ASSERT(r);

	return r;
}
VkResult Scene::CreateSyncPreset()
{
	//Semaphore Info Create
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Fence Info Create
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//Resize Semaphores
	sceneSemaphoreIA.resize(frameMax);
	sceneSemaphoreRF.resize(frameMax);
	sceneFence.resize(frameMax);

	//Create the Semaphores and Fences
	VkResult r;
	for (unsigned int i = 0; i < frameMax; ++i) {
		r = vkCreateSemaphore(vkGlobals.device, &semaphore_create_info, nullptr, &sceneSemaphoreIA[i]);
		if (r) {
			return r;
		}
		r = vkCreateSemaphore(vkGlobals.device, &semaphore_create_info, nullptr, &sceneSemaphoreRF[i]);
		if (r) {
			return r;
		}
		r = vkCreateFence(vkGlobals.device, &fence_create_info, nullptr, &sceneFence[i]);
		if (r) {
			return r;
		}
	}

	//Semaphores and Fences has been created successfully!
	return r;
}

/////////////////////////
// Start Scene Methods //
/////////////////////////
StartScene::StartScene() {
	//Initialize the Start Scene
	Initialize();
}
StartScene::~StartScene() {
	//Remove Fence
	if (sceneFence.size()) {
		for (auto fence : sceneFence) {
			vkDestroyFence(vkGlobals.device, fence, VK_NULL_HANDLE);
		}
		sceneFence.clear();
		sceneFence.shrink_to_fit();
	}

	//Remove Semaphore Render Finished
	if (sceneSemaphoreRF.size()) {
		for (auto semaphore : sceneSemaphoreRF) {
			vkDestroySemaphore(vkGlobals.device, semaphore, VK_NULL_HANDLE);
		}
		sceneSemaphoreRF.clear();
		sceneSemaphoreRF.shrink_to_fit();
	}

	//Remove Semaphore Image Available
	if (sceneSemaphoreIA.size()) {
		for (auto semaphore : sceneSemaphoreIA) {
			vkDestroySemaphore(vkGlobals.device, semaphore, VK_NULL_HANDLE);
		}
		sceneSemaphoreIA.clear();
		sceneSemaphoreIA.shrink_to_fit();
	}


	//Remove Command Pool
	if (commandPool) {
		vkDestroyCommandPool(vkGlobals.device, commandPool, VK_NULL_HANDLE);
		commandPool = {};
		commandBuffer.clear();
		commandBuffer.shrink_to_fit();
	}

	//Remove Framebuffer
	if (swapchainFramebuffer.size()) {
		for (auto framebuffer : swapchainFramebuffer) {
			vkDestroyFramebuffer(vkGlobals.device, framebuffer, VK_NULL_HANDLE);
		}
		swapchainFramebuffer.clear();
		swapchainFramebuffer.shrink_to_fit();
	}

	//Remove Renderpass
	if (renderPass) {
		vkDestroyRenderPass(vkGlobals.device, renderPass, VK_NULL_HANDLE);
		renderPass = {};
	}

	//Remove Swapchain Image Views
	if (swapchainImageView.size()) {
		for (auto imageView : swapchainImageView) {
			vkDestroyImageView(vkGlobals.device, imageView, VK_NULL_HANDLE);
		}
		swapchainImageView.clear();
		swapchainImageView.shrink_to_fit();
	}

	//Remove Swapchain
	if (swapchain) {
		vkDestroySwapchainKHR(vkGlobals.device, swapchain, VK_NULL_HANDLE);
		swapchain = {};
		swapchainImage.clear();
		swapchainImage.shrink_to_fit();
	}
}

void StartScene::Render(const float& _dtRatio)
{

}

void StartScene::Initialize() {
	//1: Update the Surface Information based on current configurations
	UpdateSurfaceData();

	//2: Setup Default Parameters
	surfaceCapabilities = vkGlobals.surfaceCapabilities;
	surfacePresentMode = VK_PRESENT_MODE_FIFO_KHR; //Default
	surfaceExtent2D = surfaceCapabilities.currentExtent;
	surfaceExtent3D = { surfaceExtent2D.width, surfaceExtent2D.height, 1 };
	surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	frameMax = 2;

	//3: Create Swapchain, Renderpass & Framebuffers
	CreateSwapchainPresetBasic();

	//4: Create Command Pool & Buffers
	CreateCommandPreset();

	//5: Create Semaphores and Fences
	CreateSyncPreset();
}
