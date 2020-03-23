#include "VkCore.h"
#include "VkGlobals.h"

namespace VkCore {
#ifdef _DEBUG
	TerminalInfo* VkConsole;

	//Prototypes
	VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(VkDebugReportFlagsEXT  flags, VkDebugReportObjectTypeEXT objectType,
		uint64_t  object, size_t location, int32_t  messageCode,
		const char* pLayerPrefix, const char* pMessage, void* pUserData);

	//Debug Initialization
	VkResult VkDebug::Init() {
		if (VkGlobal::instanceLayersActive.size())
		{
			//Console Color
			VkConsole = console_init();

			/* Load VK_EXT_debug_report entry points in debug builds */
			PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
				reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
				(vkGetInstanceProcAddr(VkGlobal::instance, "vkCreateDebugReportCallbackEXT"));

			/* Setup callback creation information */
			VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
			callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			callbackCreateInfo.pNext = nullptr;
			callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
				VK_DEBUG_REPORT_WARNING_BIT_EXT |
				VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			callbackCreateInfo.pfnCallback = &MyDebugReportCallback;

			/* Register the callback */
			VkResult result = vkCreateDebugReportCallbackEXT(VkGlobal::instance, &callbackCreateInfo, nullptr, &VkGlobal::debugReportCallback);
			return result;
		}
			return VK_SUCCESS;
	}

	//Debug Cleanup 
	//Cleanup
	VkResult VkDebug::Cleanup() {
		//Check to see if this was allocated
		if (VkGlobal::debugReportCallback) {
			//Look for the pointer to where this extension's destroy is located
			PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT =
				reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT> (vkGetInstanceProcAddr(VkGlobal::instance, "vkDestroyDebugReportCallbackEXT"));

			//Ensure the function was actually found
			if (vkDestroyDebugReportCallbackEXT) {
				//Destroy the Debug Callback
				vkDestroyDebugReportCallbackEXT(VkGlobal::instance, VkGlobal::debugReportCallback, VK_NULL_HANDLE);

				//Set to NULL
				VkGlobal::debugReportCallback = {};

				return VK_SUCCESS;
			}

			//Not Found, Will need to investigate if reaches here
			return VK_ERROR_UNKNOWN;
		}

		//Was never set, so need to do nothing
		return VK_SUCCESS;
	}
	
	//Debug Callback
	VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(VkDebugReportFlagsEXT  flags, VkDebugReportObjectTypeEXT objectType,
		uint64_t  object, size_t location, int32_t  messageCode,
		const char* pLayerPrefix, const char* pMessage, void* pUserData) {
		if (flags == VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			console_set_fg_color(VkConsole, static_cast<TerminalColor>(COLOR_RED));
			printf("%s Layer Error: %s\n", pLayerPrefix, pMessage);
		}
		else if (flags == VK_DEBUG_REPORT_WARNING_BIT_EXT) {
			console_set_fg_color(VkConsole, static_cast<TerminalColor>(COLOR_YELLOW));
			printf("%s Layer Error: %s\n", pLayerPrefix, pMessage);
		}
		else if (flags == VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
			console_set_fg_color(VkConsole, static_cast<TerminalColor>(COLOR_MAGENTA));
			printf("%s Layer Error: %s\n", pLayerPrefix, pMessage);
		}

		console_set_fg_color(VkConsole, COLOR_WHITE);
		return VK_FALSE;
	}

#else
	//Initialization
	VkResult VkDebug::Init() {
		return VK_SUCCESS;
	}

	//Cleanup
	VkResult VkDebug::Cleanup() {
		return VK_SUCCESS;
	}
#endif
}
