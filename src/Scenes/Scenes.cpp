#include "Scenes.h"
#include "../VkGlobals.h"

SceneMenu::SceneMenu(Scene*& _pScene)
	:m_CurrentScene(_pScene) { }

SceneMenu::~SceneMenu() {
}

void SceneMenu::RenderImGui() {
}

VkResult GetSurfaceFormat() {
	uint32_t formatCount = 0;
	VkResult r = vkGetPhysicalDeviceSurfaceFormatsKHR(vkGlobals.physicalDevice, vkGlobals.surface, &formatCount, VK_NULL_HANDLE);
	if (formatCount < 1) {
		VK_ASSERT(false);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}
	if (r) {
		VK_ASSERT(false);
		return r;
	}

	//Resize and fill in surface formats
//	vkGlobals.surfaceFormatsAll.resize(formatCount);
//	r = vkGetPhysicalDeviceSurfaceFormatsKHR(vkGlobals.physicalDevice, vkGlobals.surface, &formatCount, vkGlobals.surfaceFormatsAll.data());
//	if (r) {
//		VK_ASSERT(false);
//		return r;
//	}
//
//	//If Format is undefined, set it to best automatically
//	if (vkGlobals.surfaceFormatsAll.size() == 1 && vkGlobals.surfaceFormatsAll[0].format == VK_FORMAT_UNDEFINED) {
//		vkGlobals.surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
//		return VK_SUCCESS;
//	}
//
//	//Find VK_FORMAT_B8G8R8A8_UNORM and VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
//	for (size_t i = 0; i < formatCount; ++i)
//		if (vkGlobals.surfaceFormatsAll[i].format == VK_FORMAT_B8G8R8A8_UNORM && vkGlobals.surfaceFormatsAll[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
//			vkGlobals.surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
//			return VK_SUCCESS;
//		}
//
//	//Best wasn't found, take [0]
//	vkGlobals.surfaceFormat = vkGlobals.surfaceFormatsAll[0];
//	VK_ASSERT(false);
	return VK_SUCCESS;
}
VkResult GetSurfacePresentMode() {
	//Get the Size
	uint32_t presentModeCount = 0;
	VkResult r = vkGetPhysicalDeviceSurfacePresentModesKHR(vkGlobals.physicalDevice, vkGlobals.surface, &presentModeCount, VK_NULL_HANDLE);
	if (presentModeCount < 1) { 
		VK_ASSERT(false);
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}
	if (r) {
		VK_ASSERT(false);
		return r;
	}

	//Resize Vector and Put the contents in it.
	vkGlobals.surfacePresentModesAll.resize(presentModeCount);
	r = vkGetPhysicalDeviceSurfacePresentModesKHR(vkGlobals.physicalDevice, vkGlobals.surface, &presentModeCount, vkGlobals.surfacePresentModesAll.data());
	if (r) {
		VK_ASSERT(false);
		return r;
	}

	//Find the best mode (best: Mailbox, runner-up: Immediate, Default: FIFO)
//	const bool FLAG_CHECK = vkGlobals.presetSwapchainFlags & vkConst.SWAPFLAG_VSYNC;
//	VkPresentModeKHR VSyncModes[2] = { VK_PRESENT_MODE_IMMEDIATE_KHR , VK_PRESENT_MODE_MAILBOX_KHR };
//	VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;	//This is best by default. It is because Vulkan requires this to be supported, and not any of others.
//	for (uint32_t i = 0; i < presentModeCount; ++i) {	//So if any of these fail (Mailbox, Immediate, Shared), FIFO is guaranteed to work!
//		if (vkGlobals.surfacePresentModesAll[i] == VSyncModes[FLAG_CHECK]) {
//			best_mode = VSyncModes[FLAG_CHECK];
//			vkGlobals.surfacePresentMode = best_mode;
//			break;
//		}
//		else if (vkGlobals.surfacePresentModesAll[i] == VSyncModes[!FLAG_CHECK])
//			vkGlobals.surfacePresentMode = VSyncModes[!FLAG_CHECK];
//	}
//
	return VK_SUCCESS;
}
VkResult Scene::UpdateSurfaceData() {
	//Gather The Surface Capabilities
	VkResult r = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkGlobals.physicalDevice, vkGlobals.surface, &vkGlobals.surfaceCapabilities);
	if (r) {
		VK_ASSERT(false);
		return r;
	}

	//Set the Extent [Should Automatically Change upon window size change, Adjust to Surface Capabilities]
//	if (vkGlobals.surfaceCapabilities.currentExtent.width != 0xFFFFFFFF) {
//		vkGlobals.surfaceExtent2D = { vkGlobals.surfaceCapabilities.currentExtent.width, vkGlobals.surfaceCapabilities.currentExtent.height };
//		vkGlobals.surfaceExtent3D = { vkGlobals.surfaceCapabilities.currentExtent.width, vkGlobals.surfaceCapabilities.currentExtent.height, 1 };
//	}

	//Get the best Surface Format
	r = GetSurfaceFormat();
	if (r) {
		VK_ASSERT(false);
		return r;
	}

	//Get the best present mode
	r = GetSurfacePresentMode();
	if (r) {
		VK_ASSERT(false);
		return r;
	}

	return r;
}

VkResult CreateSwapchain() {
	//Gather Swapchain Count
//	vkGlobals.frameMax = 3;
//	if (vkGlobals.surfaceCapabilities.minImageCount > 0 && vkGlobals.frameMax < vkGlobals.surfaceCapabilities.minImageCount)
//		vkGlobals.frameMax = vkGlobals.surfaceCapabilities.minImageCount;
//	if (vkGlobals.surfaceCapabilities.maxImageCount > 0 && vkGlobals.frameMax > vkGlobals.surfaceCapabilities.maxImageCount)
//		vkGlobals.frameMax = vkGlobals.surfaceCapabilities.maxImageCount;
//
//	//Create Info for SwapchainKHR [Part 1]
//	VkSwapchainCreateInfoKHR create_info = {};
//	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//	create_info.surface = vkGlobals.surface;
//	create_info.minImageCount = vkGlobals.frameMax;
//	create_info.imageFormat = vkGlobals.surfaceFormat.format;
//	create_info.imageColorSpace = vkGlobals.surfaceFormat.colorSpace;
//	create_info.imageExtent = vkGlobals.surfaceExtent2D;
//	create_info.imageArrayLayers = 1;
//	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
//	create_info.preTransform = vkGlobals.surfaceCapabilities.currentTransform;
//	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//	create_info.presentMode = vkGlobals.surfacePresentMode;
//	create_info.clipped = VK_TRUE;
//	create_info.oldSwapchain = VK_NULL_HANDLE;
//
//	//Setup Correct Queue Family Indices
//	if (vkGlobals.uniqueIndices.size() > 1) {
//		create_info.queueFamilyIndexCount = vkGlobals.uniqueIndices.size();
//		create_info.pQueueFamilyIndices = vkGlobals.uniqueIndices.data();
//		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//	}
//	else {
//		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//	}
//
//	//Create Swapchain
//	VkResult r = vkCreateSwapchainKHR(vkGlobals.device, &create_info, nullptr, &vkGlobals.swapchain);
//
//	//Swapchain Image Setup
//	r = vkGetSwapchainImagesKHR(vkGlobals.device, vkGlobals.swapchain, &vkGlobals.frameMax, VK_NULL_HANDLE);
//	vkGlobals.swapchainImage.resize(vkGlobals.frameMax);
//	r = vkGetSwapchainImagesKHR(vkGlobals.device, vkGlobals.swapchain, &vkGlobals.frameMax, vkGlobals.swapchainImage.data());
//
//	//Obtain the Image and Image Views
//	for (uint32_t i = 0; i < vkGlobals.frameMax; ++i)
//		r = CreateImageView(vkGlobals.swapchainImage[i], vkGlobals.surfaceFormat, VK_IMAGE_ASPECT_COLOR_BIT, &vkGlobals.swapchainImage[i]);
//
//	//Set Current Frame to 0
//	vkGlobals.frameCurrent = 0;
//
	return VK_SUCCESS;
}
VkResult Scene::SwapchainPreset(const uint16_t& _presetFlags) {
	//1: Update the Surface Information based on current configurations
	UpdateSurfaceData();

	//2: Create Swapchain
//	CreateSwapchain();

	//3: Create the RenderPass
//	CreateVkRenderPass(_presetFlags);

	//Create the Depth Buffer (If Needed)
//	if (_presetFlags & 0x1)
//		CreateDepthBuffer();

	//Create the MSAA Buffer (If Needed)
//	if (_presetFlags & 0x2)
//		CreateMSAABuffer();

	//Create the Framebuffer
//	CreateVkFramebuffer();

	return VK_SUCCESS;
}
