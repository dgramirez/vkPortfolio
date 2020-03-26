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
	//Texture
	Texture Smiley;

	//Pipeline to draw the texture
	VkDescriptorPool descriptorPool;
	VkSampler sampler;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSet;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	//Helper Methods
	VkResult SetupDescriptors();
	VkResult SetupGraphicsPipeline();

};