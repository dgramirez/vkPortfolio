#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "../Scenes.h"

class SceneMenu final : public Scene {
public:
	SceneMenu(Scene*& _pScene);
	~SceneMenu();

	void Init();
	void Render(const float& _dtRatio) override;
	bool CheckRoomChange() override;

protected:
	void RenderImGui() override;

private:
	Scene*& m_CurrentScene;
	std::vector<std::pair<const char*, std::function<Scene * ()>>> m_Scenes;
	uint32_t ImGuiWindowFlags;
	bool OfCourse;
	int64_t index;
	ImGuiTabBarFlags tab_bar_flags;
	bool ExitApplication;

	template<typename T>
	void RegisterScene(const char* test_name) {
		m_Scenes.push_back(std::make_pair(test_name, []() { return new T(); }));
	}
	void RegisterScene();
};

#endif