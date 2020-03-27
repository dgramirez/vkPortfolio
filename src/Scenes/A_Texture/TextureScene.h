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
		//Align #1: Basics (and Black & White)
		float offsetUV[2];
		int32_t activeEffect;
		uint32_t bwGreyScaled;

		//Align #2: Gauss Offset + Pixel Size
		float gbOffset[3];
		float pxSize;
		
		//Align #3: Gauss Weight + Fisheye Aperature
		float gbWeight[3];
		float aperature;

		//Align #4: Swirling
		float swCenter[2];
		float swRadius;
		float swAngle;

		//Align #5: Black & White Luminence Coefficent
		float bwLumCoeff[4];
		
		//Align #6 & 7: Edge Detection 
		float edLumCoeff[4];
		float edTexOffset[4];
	};
	psUniform uniform;
	std::vector<VkBuffer> uniformBuffer;
	std::vector<VkDeviceMemory> uniformMemory;
	void DefaultUBOValues();
};