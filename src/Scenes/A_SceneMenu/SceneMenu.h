#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "../Scenes.h"

class SceneMenu : public Scene {
public:
	SceneMenu(Scene*& _pScene);
	~SceneMenu();

	void Render(const float& _dtRatio) override;
	void RenderImGui() override;
	template<typename T>
	void RegisterScene(const char* test_name) {
		m_Scenes.push_back(std::make_pair(test_name, []() { return new T(); }));
	}

private:
	Scene*& m_CurrentScene;
	std::vector<std::pair<const char*, std::function<Scene * ()>>> m_Scenes;
	bool yes;
};

#endif