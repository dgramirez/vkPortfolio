#ifndef SCENES_H
#define SCENES_H

class Scene {
public:
	Scene() = default;
	virtual ~Scene() = default;

	virtual void Update(const float& _dt) {}
	virtual void Render() {}
	virtual void RenderImGui() {}

	static VkResult UpdateSurfaceData();
	static VkResult SwapchainPreset(const uint16_t& _presetFlags);
};

class SceneMenu : public Scene {
public:
	SceneMenu(Scene*& _pScene);
	~SceneMenu();

	void RenderImGui() override;
	template<typename T>
	void RegisterScene(const char* test_name) {
		m_Scenes.push_back(std::make_pair(test_name, []() { return new T(); }));
	}

private:
	Scene*& m_CurrentScene;
	std::vector<std::pair<const char*, std::function<Scene*()>>> m_Scenes;
};

#endif
