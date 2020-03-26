#include "../Scenes.h"
#include "../../Asset/Texture.h"

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

private:
	VkImage tex;
	VkImageView texView;
	void LoadTexture(const char* _filePath);
	Texture Crate;
};