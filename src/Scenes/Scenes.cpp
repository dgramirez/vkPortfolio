#include "Scenes.h"
#include "../Vulkan/VkGlobals.h"
#include "../ImGui/ImGuiGlobals.h"

void Scene::FrameStart(const VkCommandBuffer & _commandBuffer, const VkRenderPass& _renderPass, const VkExtent2D& _renderExtent, const VkFramebuffer& _frameBuffer, const std::vector<VkClearValue>& _clearColor) {
	//Create the Command Buffer's Begin Info
	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	command_buffer_begin_info.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(_commandBuffer, &command_buffer_begin_info);

	//Setup the Render Pass
	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = _renderPass;
	render_pass_begin_info.framebuffer = _frameBuffer;
	render_pass_begin_info.renderArea.extent = _renderExtent;
	render_pass_begin_info.clearValueCount = _clearColor.size();
	render_pass_begin_info.pClearValues = _clearColor.data();

	//Begin the Render Pass
	vkCmdBeginRenderPass(_commandBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}
void Scene::FrameEnd(const VkCommandBuffer& _commandBuffer, const VkSemaphore& _startSemaphore, const VkSemaphore& _nextSemaphore, const VkFence& _fence) {
	//Stop the Render Pass
	vkCmdEndRenderPass(_commandBuffer);
	vkEndCommandBuffer(_commandBuffer);

	//Setup the Semaphores and Command Buffer to be sent into Queue Submit
	VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	//Setup the Queue Submit Info
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &_startSemaphore;
	submit_info.pWaitDstStageMask = &wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &_commandBuffer;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &_nextSemaphore;

	//Reset the Fence
	vkWaitForFences(VkGlobal::device, 1, &_fence, VK_TRUE, ~(static_cast<uint64_t>(0)));
	vkResetFences(VkGlobal::device, 1, &_fence);

	//Submit Queue <--Something to come back to.
	VkResult r;
	r = vkQueueSubmit(VkGlobal::queueGraphics, 1, &submit_info, _fence);
	if (r) {
		VK_ASSERT(r);
	}

}
void Scene::Present() {
	//Setup the Present Info
	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &VkSwapchain::renderSemaphore[VkSwapchain::frameCurrent];
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &VkSwapchain::swapchain;
	present_info.pImageIndices = &VkSwapchain::frameCurrent;
	present_info.pResults = nullptr;

	//Present onto the surface
	VkResult frame_result = vkQueuePresentKHR(VkGlobal::queuePresent, &present_info);

	//Error Check for Swapchain and VSync Changes
	if (frame_result == VK_ERROR_OUT_OF_DATE_KHR || frame_result == VK_SUBOPTIMAL_KHR) {
		VK_ASSERT(frame_result);
	}
	else if (frame_result) {
		VK_ASSERT(frame_result);
	}

	//Go to the next frame
	VkSwapchain::frameCurrent = (VkSwapchain::frameCurrent + 1) % VkSwapchain::frameMax;
}
