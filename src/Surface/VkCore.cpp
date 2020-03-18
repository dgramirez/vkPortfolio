#include "VkCore.h"
#include "../VkGlobals.h"


#if defined(_WIN32)
#include "vulkan/vulkan_win32.h"
#else if defined(__linux__)
#include "vulkan/vulkan_xlib.h"
#endif
VkGlobalObject vkGlobals;

//Prototype
VkResult DestroyInstance();
VkResult VulkanPrereq();
VkResult SetupInstance();

namespace VkCore {
	bool vkInit() {
		//0: Query Necessary Information
		VulkanPrereq();

		//1: Query Instance Propertues & Setup Instance
		SetupInstance();

		//2: Setup Debug Layer
		VkDebug::Init();

		//3: Create Surface

		//4: Setup Physical Device

		//5: Setup Logical Device
		//	dExt.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		//6: Setup VMA Allocator

		//7: Query Swapchain Properties

		//8: Return True (Initialized Successfully)
		return true;
	}

	void vkCleanup() {
		//Destroy Surface
		if (vkGlobals.surface) { vkDestroySurfaceKHR(vkGlobals.instance, vkGlobals.surface, nullptr); vkGlobals.surface = {}; }

		//Destroy Callback
		VkDebug::Cleanup();

		//Destroy Instance
		DestroyInstance();
	}
}

const char* GetPlatformSurfaceName() {
#ifdef _WIN32
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#else
	return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#endif
}
VkResult VulkanPrereq() {
	//Query Instance Extension Properties
	uint32_t eSize;
	vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &eSize, VK_NULL_HANDLE);
	vkGlobals.instanceExtensionsAll.resize(eSize);
	vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &eSize, vkGlobals.instanceExtensionsAll.data());

	//Query Instance Layer Properties
	uint32_t lSize;
	vkEnumerateInstanceLayerProperties(&lSize, VK_NULL_HANDLE);
	vkGlobals.instanceLayersAll.resize(lSize);
	vkEnumerateInstanceLayerProperties(&lSize, vkGlobals.instanceLayersAll.data());

	//Setup the vectors
	std::vector<const char*> iExt, iLyr;

	//Add the needed extensions to the vector
	iExt.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	iExt.push_back(GetPlatformSurfaceName());

	/////////////////////////////////////////////////
	// Add Any Additional Extensions & Layers Here //
	/////////////////////////////////////////////////
	iExt.push_back("VK_EXT_debug_report");
	iLyr.push_back("VK_LAYER_LUNARG_standard_validation");

	//Verify the instance extensions can be found
	uint32_t count = 0;
	for (auto extension : vkGlobals.instanceExtensionsAll) {
		for (auto active_extension : iExt) {
			if (!strcmp(active_extension, extension.extensionName)) { ++count; break; }
		}
		if (count == iExt.size()) break;
	}
	if (count < iExt.size()) return VK_ERROR_INITIALIZATION_FAILED;

	//Verify the intance layers can be found
	count = 0;
	for (auto layer : vkGlobals.instanceLayersAll) {
		for (auto active_layer : iLyr) {
			if (!strcmp(active_layer, layer.layerName)) { ++count; break; }
		}
		if (count == iExt.size()) break;
	}
	if (count < iLyr.size()) return VK_ERROR_INITIALIZATION_FAILED;

	//Record the active extensions and layers
	vkGlobals.instanceExtensionsActive = iExt;
	vkGlobals.instanceLayersActive = iLyr;

	return VK_SUCCESS;
}
VkResult SetupInstance() {
	//Application Information
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.apiVersion = VK_VERSION_1_2;
	app_info.pApplicationName = "Derrick Ramirez's VkPortfolio";
	app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	app_info.engineVersion = VK_MAKE_VERSION(GATEWARE_MAJOR, GATEWARE_MINOR, GATEWARE_PATCH);
	app_info.pEngineName = "Gateware";
	app_info.pNext = VK_NULL_HANDLE;

	//Application Create Info
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	//Extensions
	create_info.enabledExtensionCount = vkGlobals.instanceExtensionsActive.size();
	create_info.ppEnabledExtensionNames = vkGlobals.instanceExtensionsActive.data();
	create_info.enabledLayerCount = vkGlobals.instanceLayersActive.size();
	create_info.ppEnabledLayerNames = vkGlobals.instanceLayersActive.data();

	//Create the instance
	return vkCreateInstance(&create_info, VK_NULL_HANDLE, &vkGlobals.instance);
}

VkResult DestroyInstance() {
#ifndef __linux__
	//Destroy Instance [Linux cannot destroy the instance at this time for GWindow]
	if (vkGlobals.instance) { vkDestroyInstance(vkGlobals.instance, nullptr); vkGlobals.instance = {}; }
#endif
}

