#include "VkCore.h"
#include "../VkGlobals.h"

#define VK_FAIL(vk_result) if (vk_result) return false;

#if defined(_WIN32)
#include "vulkan/vulkan_win32.h"
#elif defined(__linux__)
#include "vulkan/vulkan_xlib.h"
#endif
VkGlobalObject vkGlobals;

//Prototype (Creation)
VkResult VulkanPrereq();
VkResult SetupInstance();
VkResult SetupSurface(const GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE& uwh);
VkResult SetupPhysicalDevice();
VkResult SetupLogicalDevice();
VkResult SetupVMAAllocator();

//Prototype (Destruction)
VkResult DestroyInstance();

//Prototype (Helpers)
const char* GetPlatformSurfaceName();
void PhysicalDeviceVerify();
void GetBestPhysicalDevice();
void GetQueueFamilyIndices();

namespace VkCore {
	bool vkInit(const GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE& uwh) {
		//0: Query Necessary Information
		VK_FAIL(VulkanPrereq());

		//1: Query Instance Propertues & Setup Instance
		VK_FAIL(SetupInstance());

		//2: Setup Debug Layer
		VK_FAIL(VkDebug::Init());

		//3: Create Surface
		VK_FAIL(SetupSurface(uwh));

		//4: Setup Physical Device
		VK_FAIL(SetupPhysicalDevice());

		//5: Setup Logical Device
		VK_FAIL(SetupLogicalDevice());

		//6: Setup VMA Allocator
		VK_FAIL(SetupVMAAllocator());

		//7: Return True (Initialized Successfully)
		return true;
	}
	void vkCleanup() {
		//Cleanup VMA Allocator
		if (vkGlobals.allocator) vmaDestroyAllocator(vkGlobals.allocator);

		//Cleanup Device
		if (vkGlobals.device) vkDestroyDevice(vkGlobals.device, nullptr);

		//Destroy Surface
		if (vkGlobals.surface) { vkDestroySurfaceKHR(vkGlobals.instance, vkGlobals.surface, nullptr); vkGlobals.surface = {}; }

		//Destroy Callback
		VkDebug::Cleanup();

		//Destroy Instance
		DestroyInstance();
	}
}

//Definitions (Creation)
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

	//Add needed (but verified later) extensions to the vector, due to lack of physical device
	vkGlobals.deviceExtensionsActive.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

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
	if (count < iExt.size()) {
		VK_ASSERT(true);
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	//Verify the intance layers can be found
	count = 0;
	for (auto layer : vkGlobals.instanceLayersAll) {
		for (auto active_layer : iLyr) {
			if (!strcmp(active_layer, layer.layerName)) { ++count; break; }
		}
		if (count == iExt.size()) break;
	}
	if (count < iLyr.size()) {
		VK_ASSERT(true);
		return VK_ERROR_LAYER_NOT_PRESENT;
	}

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
	VkResult r = vkCreateInstance(&create_info, VK_NULL_HANDLE, &vkGlobals.instance);
	VK_ASSERT(r);
	return r;
}
VkResult SetupSurface(const GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE& uwh) {
#if defined(_WIN32)
	HWND hWnd = static_cast<HWND>(uwh.window);
	HINSTANCE* hInst = reinterpret_cast<HINSTANCE*>(GetWindowLongPtr(static_cast<HWND>(hWnd), GWLP_HINSTANCE));

	VkWin32SurfaceCreateInfoKHR create_info;
	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.hinstance = (*hInst) ? *hInst : nullptr;
	create_info.hwnd = hWnd;
	create_info.flags = 0;
	create_info.pNext = 0;

	VkResult r = vkCreateWin32SurfaceKHR(vkGlobals.instance, &create_info, VK_NULL_HANDLE, &vkGlobals.surface);
	VK_ASSERT(r);
	return r;
#elif defined(__linux__)
	//Setup Window and Display
	Window wnd = *(static_cast<Window*>(uwh.window));
	Display* dpy = static_cast<Display*>(uwh.display);

	VkXlibSurfaceCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	create_info.window = wnd;
	create_info.dpy = dpy;
	create_info.flags = 0;
	create_info.pNext = VK_NULL_HANDLE;

	VkResult r = vkCreateXlibSurfaceKHR(vkGlobals.instance, &create_info, VK_NULL_HANDLE, &vkGlobals.surface);
	VK_ASSERT(r);
	return r;
#endif
	VK_ASSERT(true);
	return VK_ERROR_NOT_PERMITTED_EXT;
}
VkResult SetupPhysicalDevice() {
	//Enumerate all physical devices
	uint32_t device_count;
	vkEnumeratePhysicalDevices(vkGlobals.instance, &device_count, VK_NULL_HANDLE);
	if (device_count < 1) {
		VK_ASSERT(true);
		return VK_ERROR_NOT_PERMITTED_EXT;
	}

	vkGlobals.physicalDeviceAll.resize(device_count);
	vkEnumeratePhysicalDevices(vkGlobals.instance, &device_count, vkGlobals.physicalDeviceAll.data());

	//Verify Each Physical Devices Requirements
	PhysicalDeviceVerify();
	if (vkGlobals.physicalDeviceAll.size() < 1) {
		VK_ASSERT(true);
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	//Get a physical device from file (Future)
//	GetPhysicalDeviceFromFile();
	if (vkGlobals.physicalDevice)
		return VK_SUCCESS;

	//Find best one to use.
	GetBestPhysicalDevice();

	//Get the best Queue Family Indices Set up
	GetQueueFamilyIndices();

	return VK_SUCCESS;
}
VkResult SetupLogicalDevice() {
	//Setup Size for Create Info
	std::vector<uint16_t> qfi;

	qfi.push_back(vkGlobals.GRAPHICS_INDEX);
	if (vkGlobals.GRAPHICS_INDEX != vkGlobals.PRESENT_INDEX)
		qfi.push_back(vkGlobals.PRESENT_INDEX);

	if (vkGlobals.PRESENT_INDEX != vkGlobals.COMPUTE_INDEX)
		qfi.push_back(vkGlobals.COMPUTE_INDEX);

	if (vkGlobals.COMPUTE_INDEX != vkGlobals.TRANSFER_INDEX)
		qfi.push_back(vkGlobals.TRANSFER_INDEX);

	//Setup Create Infos
	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfo;
	QueueCreateInfo.resize(qfi.size());

	//Set up Create Info for all unique queue families
	float priority = 1.0f;
	for (uint32_t i = 0; i < qfi.size(); ++i)
	{
		VkDeviceQueueCreateInfo create_info = {};

		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		create_info.queueFamilyIndex = qfi[i];
		create_info.queueCount = 1;
		create_info.pQueuePriorities = &priority;
		QueueCreateInfo[i] = create_info;
	}

	//Verify Device Extensions are good
	bool success = false;
	for (auto extension : vkGlobals.deviceExtensionsActive) {
		success = false;
		for (auto all_extensions : vkGlobals.physicalDeviceExtensionsAll[vkGlobals.physicalDevice]) {
			if (!strcmp(extension, all_extensions.extensionName)) {
				success = true;
				break;
			}
		}
		if (!success) {
			VK_ASSERT(true);
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	//Setup Logical device create info
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = QueueCreateInfo.data();
	create_info.queueCreateInfoCount = QueueCreateInfo.size();
	create_info.pEnabledFeatures = &vkGlobals.physicalDeviceFeaturesAll[vkGlobals.physicalDevice];

	create_info.enabledExtensionCount = vkGlobals.deviceExtensionsActive.size();
	create_info.ppEnabledExtensionNames = vkGlobals.deviceExtensionsActive.data();

	//Create the Surface (With Results) [VK_SUCCESS = 0]
	VkResult r = vkCreateDevice(vkGlobals.physicalDevice, &create_info, VK_NULL_HANDLE, &vkGlobals.device);

	//If Device has been created, Setup the Device Queue for graphics and present family
	vkGetDeviceQueue(vkGlobals.device, vkGlobals.GRAPHICS_INDEX, 0, &vkGlobals.queueGraphics);
	vkGetDeviceQueue(vkGlobals.device, vkGlobals.PRESENT_INDEX, 0, &vkGlobals.queuePresent);
	vkGetDeviceQueue(vkGlobals.device, vkGlobals.COMPUTE_INDEX, 0, &vkGlobals.queueCompute);
	vkGetDeviceQueue(vkGlobals.device, vkGlobals.TRANSFER_INDEX, 0, &vkGlobals.queueTransfer);

	//Device has been created successfully!
	return r;

	return VK_SUCCESS;
}
VkResult SetupVMAAllocator() {
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = vkGlobals.physicalDevice;
	allocatorInfo.device = vkGlobals.device;

	return vmaCreateAllocator(&allocatorInfo, &vkGlobals.allocator);
}

//Definitions (Destruction)
VkResult DestroyInstance() {
#ifndef __linux__
	//Destroy Instance [Linux cannot destroy the instance at this time for GWindow]
	if (vkGlobals.instance) { vkDestroyInstance(vkGlobals.instance, nullptr); vkGlobals.instance = {}; }
#endif
	return VK_SUCCESS;
}

//Definitions (Helper)
const char* GetPlatformSurfaceName() {
#ifdef _WIN32
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#else
	return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#endif
}
void PhysicalDeviceVerify() {
	//Create pre-set variables
	std::vector<VkExtensionProperties> devExt;
	std::vector<VkQueueFamilyProperties> qfProperties;
	VkPhysicalDevice current_device;

	for (uint32_t i = 0; i < vkGlobals.physicalDeviceAll.size(); ++i) {
		//Clear & Reset Variables
		current_device = vkGlobals.physicalDeviceAll[i];
		devExt.clear();
		qfProperties.clear();

		//Check #1: Getting Device Extension Count
		uint32_t extCount;
		vkEnumerateDeviceExtensionProperties(current_device, VK_NULL_HANDLE, &extCount, VK_NULL_HANDLE);
		if (extCount < 1) {
			//This is bad. Remove from array
			vkGlobals.physicalDeviceAll.erase(vkGlobals.physicalDeviceAll.begin() + i);

			//Decrement i
			--i;

			//go to the next device
			continue;
		}
		devExt.resize(extCount);
		vkEnumerateDeviceExtensionProperties(current_device, VK_NULL_HANDLE, &extCount, devExt.data());

		//Check #2: Device Extension: Swapchain
		bool pass = false;
		for (auto extension : devExt) {
			if (!strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
				pass = true;
				break;
			}
		}
		if (!pass) {
			//This is bad. Remove from array
			vkGlobals.physicalDeviceAll.erase(vkGlobals.physicalDeviceAll.begin() + i);

			//Decrement i
			--i;

			//go to the next device
			continue;
		}

		//Check #3A: Queue Family Properties for GRAPHICS, PRESENT and COMPUTE
		uint32_t fqCount;
		vkGetPhysicalDeviceQueueFamilyProperties(current_device, &fqCount, VK_NULL_HANDLE);
		if (fqCount < 1) {
			//This is bad. Remove from array
			vkGlobals.physicalDeviceAll.erase(vkGlobals.physicalDeviceAll.begin() + i);

			//Decrement i
			--i;

			//go to the next device
			continue;
		}
		qfProperties.resize(fqCount);
		vkGetPhysicalDeviceQueueFamilyProperties(current_device, &fqCount, qfProperties.data());

		//Check #3B: Queue Family has GRAPHICS, PRESENT and COMPUTE
		uint32_t flag_check = 0;
		for (size_t j = 0; qfProperties.size(); ++j) {
			VkQueueFamilyProperties queue_fam = qfProperties[j];
			flag_check |= queue_fam.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
			if (queue_fam.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				VkBool32 presentSupport;
				VkResult r = vkGetPhysicalDeviceSurfaceSupportKHR(current_device, j, vkGlobals.surface, &presentSupport);
				if (presentSupport)
					flag_check |= 0x4;
			}
			if (flag_check > 6)
				break;
		}
		if (flag_check < 7) {
			//This is bad. Remove from array
			vkGlobals.physicalDeviceAll.erase(vkGlobals.physicalDeviceAll.begin() + i);

			//Decrement i
			--i;

			//go to the next device
			continue;
		}

		//It Passes! Collect ALL the properties for the device
		VkPhysicalDeviceFeatures feat;
		VkPhysicalDeviceMemoryProperties device_memory_props;
		VkPhysicalDeviceProperties device_props;

		vkGlobals.physicalDeviceExtensionsAll[current_device] = devExt;
		vkGlobals.physicalDeviceQueueFamilyPropertiesAll[current_device] = qfProperties;

		vkGetPhysicalDeviceFeatures(current_device, &feat);
		vkGlobals.physicalDeviceFeaturesAll[current_device] = feat;

		vkGetPhysicalDeviceProperties(current_device, &device_props);
		vkGlobals.physicalDevicePropertiesAll[current_device] = device_props;

		vkGetPhysicalDeviceMemoryProperties(current_device, &device_memory_props);
		vkGlobals.physicalDeviceMemoryPropertiesAll[current_device] = device_memory_props;
	}
}
void GetBestPhysicalDevice() {
	//There is only 1 to start with
	if (vkGlobals.physicalDeviceAll.size() == 1)
	{
		vkGlobals.physicalDevice = vkGlobals.physicalDeviceAll[0];
		return;
	}

	//Create a copy of the competitors
	std::vector<VkPhysicalDevice> copyPDev = vkGlobals.physicalDeviceAll;

	//Find the Discrete ones
	VkPhysicalDeviceProperties pDevProps;
	for (auto physical_device = copyPDev.begin(); physical_device != copyPDev.end(); ++physical_device) {
		pDevProps = vkGlobals.physicalDevicePropertiesAll[*physical_device];
		if (pDevProps.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			--physical_device;
			copyPDev.erase(physical_device + 1);
		}
	}

	//If there is one left, It Wins
	if (copyPDev.size() == 1)
	{
		vkGlobals.physicalDevice = copyPDev[0];
		return;
	}

	//Check: No Discrete Devices Available, But there are more than 1 devices (Integrated, Virtual, CPU, Others...)
	if (copyPDev.size() < 1)
		copyPDev = vkGlobals.physicalDeviceAll;

	//Best Match! [TBD]
	//vkGetPhysicalDeviceFeatures
	//vkGetPhysicalDeviceFormatProperties
	//vkGetPhysicalDeviceImageFormatProperties
	//vkGetPhysicalDeviceProperties
	//vkGetPhysicalDeviceQueueFamilyProperties
	//vkGetPhysicalDeviceMemoryProperties
}
void SetQueueFamilyIndices(const VkQueueFlags& _priority1, const VkQueueFlags& _priority2, const VkQueueFlags& _priority3, const VkQueueFlags& _priority4, uint16_t& _index) {
	//Setup Physical Device Family Queue Properties
	auto queue_families = vkGlobals.physicalDeviceQueueFamilyPropertiesAll[vkGlobals.physicalDevice];

	//Find Best for Transfer Queue
	uint16_t score = 0;
	VkQueueFlags flags = VK_QUEUE_FLAG_BITS_MAX_ENUM;
	for (uint32_t i = 0; i < queue_families.size(); ++i) {
		VkQueueFamilyProperties current_property = queue_families[i];

		//Priority 1 [Alone]
		flags = _priority1;
		if (!(current_property.queueFlags ^ flags)) {
			_index = i;
			break;
		}

		//Priority 1 + 2
		flags |= _priority2;
		if (!(current_property.queueFlags ^ flags)) {
			if (score < 3) {
				_index = i;
				score = 3;
			}
		}

		//Priority 1 + 2 + 3
		flags |= _priority3;
		if (!(current_property.queueFlags ^ flags)) {
			if (score < 2) {
				_index = i;
				score = 2;
			}
		}

		//Priority 1 + 2 + 3 + 4
		flags |= _priority4;
		if (!(current_property.queueFlags ^ flags)) {
			if (score < 1) {
				_index = i;
				score = 1;
			}
		}
	}
}
void GetQueueFamilyIndices() {
	SetQueueFamilyIndices(VK_QUEUE_TRANSFER_BIT, VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT, vkGlobals.TRANSFER_INDEX);
	SetQueueFamilyIndices(VK_QUEUE_COMPUTE_BIT, VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT, vkGlobals.COMPUTE_INDEX);
	SetQueueFamilyIndices(VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT, vkGlobals.GRAPHICS_INDEX);
	SetQueueFamilyIndices(VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT, vkGlobals.PRESENT_INDEX);
}
