#ifndef STARTSCENE_H
#define STARTSCENE_H

#include "../Scenes.h"
class StartScene : public Scene {
public:
	StartScene();
	~StartScene();

	void Render(const float& _dtRatio) override;
	void RenderImGui() override;

	void Initialize();
	void Cleanup() override;

private:
	bool canRender = true;
	bool yes = true;
	std::vector<VkClearValue> clearColor;
};
#endif