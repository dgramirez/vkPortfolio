#include "VkCore.h"
#include "VkGlobals.h"

#define VK_FAIL(vk_result) if (vk_result) return false;

#if defined(_WIN32)
#include "vulkan/vulkan_win32.h"
#elif defined(__linux__)
#include "vulkan/vulkan_xlib.h"
#endif

namespace {
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

		for (uint32_t i = 0; i < VkGlobal::physicalDeviceAll.size(); ++i) {
			//Clear & Reset Variables
			current_device = VkGlobal::physicalDeviceAll[i];
			devExt.clear();
			qfProperties.clear();

			//Check #1: Getting Device Extension Count
			uint32_t extCount;
			vkEnumerateDeviceExtensionProperties(current_device, VK_NULL_HANDLE, &extCount, VK_NULL_HANDLE);
			if (extCount < 1) {
				//This is bad. Remove from array
				VkGlobal::physicalDeviceAll.erase(VkGlobal::physicalDeviceAll.begin() + i);

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
				VkGlobal::physicalDeviceAll.erase(VkGlobal::physicalDeviceAll.begin() + i);

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
				VkGlobal::physicalDeviceAll.erase(VkGlobal::physicalDeviceAll.begin() + i);

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
					VkResult r = vkGetPhysicalDeviceSurfaceSupportKHR(current_device, j, VkGlobal::surface, &presentSupport);
					if (presentSupport)
						flag_check |= 0x4;
				}
				if (flag_check > 6)
					break;
			}
			if (flag_check < 7) {
				//This is bad. Remove from array
				VkGlobal::physicalDeviceAll.erase(VkGlobal::physicalDeviceAll.begin() + i);

				//Decrement i
				--i;

				//go to the next device
				continue;
			}

			//It Passes! Collect ALL the properties for the device
			VkPhysicalDeviceFeatures feat;
			VkPhysicalDeviceMemoryProperties device_memory_props;
			VkPhysicalDeviceProperties device_props;

			VkGlobal::physicalDeviceExtensionsAll[current_device] = devExt;
			VkGlobal::physicalDeviceQueueFamilyPropertiesAll[current_device] = qfProperties;

			vkGetPhysicalDeviceFeatures(current_device, &feat);
			VkGlobal::physicalDeviceFeaturesAll[current_device] = feat;

			vkGetPhysicalDeviceProperties(current_device, &device_props);
			VkGlobal::physicalDevicePropertiesAll[current_device] = device_props;

			vkGetPhysicalDeviceMemoryProperties(current_device, &device_memory_props);
			VkGlobal::physicalDeviceMemoryPropertiesAll[current_device] = device_memory_props;
		}
	}
	void GetBestPhysicalDevice() {
		//There is only 1 to start with
		if (VkGlobal::physicalDeviceAll.size() == 1)
		{
			VkGlobal::physicalDevice = VkGlobal::physicalDeviceAll[0];
			return;
		}

		//Create a copy of the competitors
		std::vector<VkPhysicalDevice> copyPDev = VkGlobal::physicalDeviceAll;

		//Find the Discrete ones
		VkPhysicalDeviceProperties pDevProps;
		for (auto physical_device = copyPDev.begin(); physical_device != copyPDev.end(); ++physical_device) {
			pDevProps = VkGlobal::physicalDevicePropertiesAll[*physical_device];
			if (pDevProps.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				--physical_device;
				copyPDev.erase(physical_device + 1);
			}
		}

		//If there is one left, It Wins
		if (copyPDev.size() == 1)
		{
			VkGlobal::physicalDevice = copyPDev[0];
			return;
		}

		//Check: No Discrete Devices Available, But there are more than 1 devices (Integrated, Virtual, CPU, Others...)
		if (copyPDev.size() < 1)
			copyPDev = VkGlobal::physicalDeviceAll;

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
		auto queue_families = VkGlobal::physicalDeviceQueueFamilyPropertiesAll[VkGlobal::physicalDevice];

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
		SetQueueFamilyIndices(VK_QUEUE_TRANSFER_BIT, VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT, VkGlobal::TRANSFER_INDEX);
		SetQueueFamilyIndices(VK_QUEUE_COMPUTE_BIT, VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT, VkGlobal::COMPUTE_INDEX);
		SetQueueFamilyIndices(VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT, VkGlobal::GRAPHICS_INDEX);
		SetQueueFamilyIndices(VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_SPARSE_BINDING_BIT, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT, VkGlobal::PRESENT_INDEX);
	}

	//Definitions (Destruction)
	VkResult DestroyInstance() {
#ifndef __linux__
		//Destroy Instance [Linux cannot destroy the instance at this time for GWindow]
		if (VkGlobal::instance) { vkDestroyInstance(VkGlobal::instance, nullptr); VkGlobal::instance = {}; }
#endif
		return VK_SUCCESS;
	}

	//Definitions (Creation)
	VkResult VulkanPrereq() {
		//Query Instance Extension Properties
		uint32_t eSize;
		vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &eSize, VK_NULL_HANDLE);
		VkGlobal::instanceExtensionsAll.resize(eSize);
		vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &eSize, VkGlobal::instanceExtensionsAll.data());

		//Query Instance Layer Properties
		uint32_t lSize;
		vkEnumerateInstanceLayerProperties(&lSize, VK_NULL_HANDLE);
		VkGlobal::instanceLayersAll.resize(lSize);
		vkEnumerateInstanceLayerProperties(&lSize, VkGlobal::instanceLayersAll.data());

		//Setup the vectors
		std::vector<const char*> iExt, iLyr;

		//Add the needed extensions to the vector
		iExt.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		iExt.push_back(GetPlatformSurfaceName());

		//Add needed (but verified later) extensions to the vector, due to lack of physical device
		VkGlobal::deviceExtensionsActive.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		/////////////////////////////////////////////////
		// Add Any Additional Extensions & Layers Here //
		/////////////////////////////////////////////////
		iExt.push_back("VK_EXT_debug_report");
		iLyr.push_back("VK_LAYER_LUNARG_standard_validation");
		iLyr.push_back("VK_LAYER_RENDERDOC_Capture");

		//Verify the instance extensions can be found
		uint32_t count = 0;
		for (auto extension : VkGlobal::instanceExtensionsAll) {
			for (auto active_extension : iExt) {
				if (!strcmp(active_extension, extension.extensionName)) { ++count; break; }
			}
			if (count == iExt.size()) break;
		}
		if (count < iExt.size()) {
			VK_ASSERT(VK_ERROR_EXTENSION_NOT_PRESENT);
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		//Verify the intance layers can be found
		count = 0;
		for (auto layer : VkGlobal::instanceLayersAll) {
			for (auto active_layer : iLyr) {
				if (!strcmp(active_layer, layer.layerName)) { ++count; break; }
			}
			if (count == iExt.size()) break;
		}
		if (count < iLyr.size()) {
			VK_ASSERT(VK_ERROR_LAYER_NOT_PRESENT);
			return VK_ERROR_LAYER_NOT_PRESENT;
		}

		//Record the active extensions and layers
		VkGlobal::instanceExtensionsActive = iExt;
		VkGlobal::instanceLayersActive = iLyr;

#if !defined(_DEBUG)
		//Clear All Layers.
		VkGlobal::instanceLayersActive.clear();

		//Clear Capacity.
		VkGlobal::instanceLayersActive.shrink_to_fit();
#endif

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
		create_info.enabledExtensionCount = VkGlobal::instanceExtensionsActive.size();
		create_info.ppEnabledExtensionNames = VkGlobal::instanceExtensionsActive.data();
		create_info.enabledLayerCount = VkGlobal::instanceLayersActive.size();
		create_info.ppEnabledLayerNames = VkGlobal::instanceLayersActive.data();

		//Create the instance
		VkResult r = vkCreateInstance(&create_info, VK_NULL_HANDLE, &VkGlobal::instance);
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

		VkResult r = vkCreateWin32SurfaceKHR(VkGlobal::instance, &create_info, VK_NULL_HANDLE, &VkGlobal::surface);
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

		VkResult r = vkCreateXlibSurfaceKHR(VkGlobal::instance, &create_info, VK_NULL_HANDLE, &VkGlobal::surface);
		VK_ASSERT(r);
		return r;
#endif
		VK_ASSERT(VK_ERROR_NOT_PERMITTED_EXT);
		return VK_ERROR_NOT_PERMITTED_EXT;
	}
	VkResult SetupPhysicalDevice() {
		//Enumerate all physical devices
		uint32_t device_count;
		vkEnumeratePhysicalDevices(VkGlobal::instance, &device_count, VK_NULL_HANDLE);
		if (device_count < 1) {
			VK_ASSERT(VK_ERROR_NOT_PERMITTED_EXT);
			return VK_ERROR_NOT_PERMITTED_EXT;
		}

		VkGlobal::physicalDeviceAll.resize(device_count);
		vkEnumeratePhysicalDevices(VkGlobal::instance, &device_count, VkGlobal::physicalDeviceAll.data());

		//Verify Each Physical Devices Requirements
		PhysicalDeviceVerify();
		if (VkGlobal::physicalDeviceAll.size() < 1) {
			VK_ASSERT(VK_ERROR_EXTENSION_NOT_PRESENT);
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		//Get a physical device from file (Future)
	//	GetPhysicalDeviceFromFile();
		if (VkGlobal::physicalDevice)
			return VK_SUCCESS;

		//Find best one to use.
		GetBestPhysicalDevice();

		//Get the best Queue Family Indices Set up
		GetQueueFamilyIndices();

		return VK_SUCCESS;
	}
	VkResult SetupLogicalDevice() {
		//Setup Size for Create Info
		VkGlobal::uniqueIndices.push_back(VkGlobal::GRAPHICS_INDEX);
		if (VkGlobal::GRAPHICS_INDEX != VkGlobal::PRESENT_INDEX)
			VkGlobal::uniqueIndices.push_back(VkGlobal::PRESENT_INDEX);

		if (VkGlobal::PRESENT_INDEX != VkGlobal::COMPUTE_INDEX)
			VkGlobal::uniqueIndices.push_back(VkGlobal::COMPUTE_INDEX);

		if (VkGlobal::COMPUTE_INDEX != VkGlobal::TRANSFER_INDEX)
			VkGlobal::uniqueIndices.push_back(VkGlobal::TRANSFER_INDEX);

		//Setup Create Infos
		std::vector<VkDeviceQueueCreateInfo> QueueCreateInfo;
		QueueCreateInfo.resize(VkGlobal::uniqueIndices.size());

		//Set up Create Info for all unique queue families
		float priority = 1.0f;
		for (uint32_t i = 0; i < VkGlobal::uniqueIndices.size(); ++i)
		{
			VkDeviceQueueCreateInfo create_info = {};

			create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			create_info.queueFamilyIndex = VkGlobal::uniqueIndices[i];
			create_info.queueCount = 1;
			create_info.pQueuePriorities = &priority;
			QueueCreateInfo[i] = create_info;
		}

		//Verify Device Extensions are good
		bool success = false;
		for (auto extension : VkGlobal::deviceExtensionsActive) {
			success = false;
			for (auto all_extensions : VkGlobal::physicalDeviceExtensionsAll[VkGlobal::physicalDevice]) {
				if (!strcmp(extension, all_extensions.extensionName)) {
					success = true;
					break;
				}
			}
			if (!success) {
				VK_ASSERT(VK_ERROR_EXTENSION_NOT_PRESENT);
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		//Setup Logical device create info
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pQueueCreateInfos = QueueCreateInfo.data();
		create_info.queueCreateInfoCount = QueueCreateInfo.size();
		create_info.pEnabledFeatures = &VkGlobal::physicalDeviceFeaturesAll[VkGlobal::physicalDevice];

		create_info.enabledExtensionCount = VkGlobal::deviceExtensionsActive.size();
		create_info.ppEnabledExtensionNames = VkGlobal::deviceExtensionsActive.data();

		//Create the Surface (With Results) [VK_SUCCESS = 0]
		VkResult r = vkCreateDevice(VkGlobal::physicalDevice, &create_info, VK_NULL_HANDLE, &VkGlobal::device);

		//If Device has been created, Setup the Device Queue for graphics and present family
		vkGetDeviceQueue(VkGlobal::device, VkGlobal::GRAPHICS_INDEX, 0, &VkGlobal::queueGraphics);
		vkGetDeviceQueue(VkGlobal::device, VkGlobal::PRESENT_INDEX, 0, &VkGlobal::queuePresent);
		vkGetDeviceQueue(VkGlobal::device, VkGlobal::COMPUTE_INDEX, 0, &VkGlobal::queueCompute);
		vkGetDeviceQueue(VkGlobal::device, VkGlobal::TRANSFER_INDEX, 0, &VkGlobal::queueTransfer);

		//Device has been created successfully!
		return r;

		return VK_SUCCESS;
	}
	VkResult SetupVMAAllocator() {
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = VkGlobal::physicalDevice;
		allocatorInfo.device = VkGlobal::device;

		return vmaCreateAllocator(&allocatorInfo, &VkGlobal::allocator);
	}
	
}

namespace VkCore {
	bool vkInit(const GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE& uwh) {
		//0: Query Necessary Information
		VK_FAIL(::VulkanPrereq());

		//1: Query Instance Propertues & Setup Instance
		VK_FAIL(::SetupInstance());

		//2: Setup Debug Layer
		VK_FAIL(VkDebug::Init());

		//3: Create Surface
		VK_FAIL(::SetupSurface(uwh));

		//4: Setup Physical Device
		VK_FAIL(::SetupPhysicalDevice());

		//5: Setup Logical Device
		VK_FAIL(::SetupLogicalDevice());

		//6: Setup VMA Allocator
		VK_FAIL(::SetupVMAAllocator());

		//7: Return True (Initialized Successfully)
		return true;
	}
	void vkCleanup() {
		//Cleanup VMA Allocator
		if (VkGlobal::allocator) {
			vmaDestroyAllocator(VkGlobal::allocator); 
			VkGlobal::allocator = {};
		}

		//Cleanup Device
		if (VkGlobal::device) {
			vkDestroyDevice(VkGlobal::device, nullptr);
			VkGlobal::device = {};
		}

		//Destroy Surface
		if (VkGlobal::surface) { 
			vkDestroySurfaceKHR(VkGlobal::instance, VkGlobal::surface, nullptr);
			VkGlobal::surface = {};
		}

		//Destroy Callback
		VkDebug::Cleanup();

		//Destroy Instance
		DestroyInstance();
	}
}

