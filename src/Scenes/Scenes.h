#ifndef SCENES_H
#define SCENES_H

class Scene {
public:
	Scene() = default;
	virtual ~Scene() = default;

	virtual void Update(const float& _dt) {}
	virtual void Render(const float& _dtRatio) {}
	virtual void Reset();
	virtual void Cleanup();
	virtual bool CheckRoomChange() { return false; }

protected:
	virtual void FrameStart(const VkCommandBuffer& _commandBuffer, const VkRenderPass& _renderPass, const VkExtent2D& _renderExtent, const VkFramebuffer& _frameBuffer, const std::vector<VkClearValue>& _clearColor);
	virtual void FrameEnd(const VkCommandBuffer& _commandBuffer, const VkSemaphore& _startSemaphore, const VkSemaphore& _nextSemaphore, const VkFence& _fence);
	virtual void Present();
	virtual void RenderImGui() {}
	static bool ChangeRoom;
};

#endif
