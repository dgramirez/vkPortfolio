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

	//Uniform Buffers
	const char* TexTech[7];
	struct psUniform {
		float offsetUV[2];
		int32_t activeEffect;

		//Gaussian Blur
		float gbOffset[3];
		float gbWeight[3];

		//Swirling
		float swRadius;
		float swAngle;
		float swCenter[2];

		//Pixelate
		float pxSize;

		//Edge Detection
		float edLumCoeff[4];
		float edTexOffset[2];

		//Black and White
		bool bwGreyScaled;
		float bwLumCoeff[4];

		//Fish-Eye
		float aperature;
	};
	psUniform uniform;
	std::vector<VkBuffer> uniformBuffer;
	std::vector<VkDeviceMemory> uniformMemory;
};