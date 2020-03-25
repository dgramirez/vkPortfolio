#include "../Scenes.h"

class TextureScene : public Scene {
public:
	TextureScene();
	~TextureScene();

	void Update(const float& _dt) override;
	void Render(const float& _dtRatio) override;
	void Reset() override;
	void Cleanup() override;
	bool CheckRoomChange() override;

protected:
	void RenderImGui() override;
};