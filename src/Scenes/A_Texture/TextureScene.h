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
	bool checkbox;
	struct psUniform {
		//Align #1: Global & Pixelize {(8+4+4)}
		float offsetUV[2];
		int32_t activeEffect;
		float pxSize;

		//Align #2: Gaussian Blur & Black and White. {(12+4), (8+8), (16)}
		float gbWeight[3];
		float bwLumPassingValue;
		float gbOffset[2];
		float gbUVWeight[2];
		float bwLumCoeff[4];

		//Align #3: Swirl & Fish-Eye {(8+8), (4+4+4+4)}
		float swTexSize[2];
		float swCenter[2];
		float swRadius;
		float swAngle;
		float swThetaFactor;
		float feAperature;

		//Align #4: Edge Detection and Greyscale {(16) + (4+4)}
		float edTexOffset[2];
		float edColorWeight;
		uint32_t bwGreyScaled;

		//Align #5: Gaussian Blur Horizontal Pass {8}
		float gbOffsetH[2];
	};
	psUniform uniform;
	std::vector<VkBuffer> uniformBuffer;
	std::vector<VkDeviceMemory> uniformMemory;
	void DefaultUBOValues();
};