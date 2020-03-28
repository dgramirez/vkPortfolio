#version 450
//The UV obtained from Vertex Shader
layout (location = 0) in vec2 vsUV;

//The color to be sent out
layout (location = 0) out vec4 psColor;

layout (binding = 0) uniform psUBO {
	//Align #1: Global & Pixelize {(8+4+4)}
	vec2 offsetUV;
	int activeEffect;
	float pxSize;

	//Align #2: Gaussian Blur & Black and White. {(12+4), (8+8), (16)}
	vec3 gbWeight;
	float bwLumPassingValue;
	vec2 gbOffset;
	vec2 gbUVWeight;
	vec4 bwLumCoeff;

	//Align #3: Swirl & Fish-Eye {(8+8), (4+4+4+4)}
	vec2 swTexSize;
	vec2 swCenter;
	float swRadius;
	float swAngle;
	float swThetaFactor;
	float feAperature;

	//Align #4: Edge Detection and Greyscale {(8+4+4)}
	vec2 edTexOffset;
	float edColorWeight;
	bool bwGreyScaled;
} ubo;
layout (binding = 1) uniform sampler2D uv_sampler;

vec4 GaussianBlur() {
	vec2 uv = vsUV + ubo.offsetUV;
	vec4 tc = texture(uv_sampler, uv) * ubo.gbWeight[0];
	for (int i = 1; i < 3; ++i) {
		tc += texture(uv_sampler, uv + vec2(0.0f, ubo.gbOffset[i-1]) * ubo.gbUVWeight[0]) * ubo.gbWeight[i];
		tc += texture(uv_sampler, uv - vec2(0.0f, ubo.gbOffset[i-1]) * ubo.gbUVWeight[1]) * ubo.gbWeight[i];
	}

	return tc;
}
vec4 Swirling() {
	vec2 uv = vsUV + ubo.offsetUV;
	vec2 texSize = ubo.swTexSize;
	vec2 tc = uv * texSize;
	tc -= ubo.swCenter;
	float dist = length(tc);
	if (dist < ubo.swRadius) {
		float p = (ubo.swRadius - dist) / ubo.swRadius;
		float theta = p * p * ubo.swAngle * ubo.swThetaFactor;
		float s = sin(theta);
		float c = cos(theta);
		tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
	}
	tc += ubo.swCenter;
	return texture(uv_sampler, tc / texSize);
}
vec4 Pixelate() {
	vec2 uv = vsUV + ubo.offsetUV;
	int u = int(uv.x * ubo.pxSize);
	int v = int(uv.y * ubo.pxSize);
	return texture(uv_sampler, vec2(float(u)/ubo.pxSize, float(v)/ubo.pxSize));
}
vec4 EdgeDetection() {
	vec2 uv = vsUV + ubo.offsetUV;
	vec4 c[9];
	c[0] = texture(uv_sampler, uv + vec2(-ubo.edTexOffset.x, -ubo.edTexOffset.y));
	c[1] = texture(uv_sampler, uv + vec2(              0.0f, -ubo.edTexOffset.y));
	c[2] = texture(uv_sampler, uv + vec2( ubo.edTexOffset.x, -ubo.edTexOffset.y));
	
	c[3] = texture(uv_sampler, uv + vec2(-ubo.edTexOffset.x, 0.0f));
	c[4] = texture(uv_sampler, uv + vec2(              0.0f, 0.0f));
	c[5] = texture(uv_sampler, uv + vec2( ubo.edTexOffset.x, 0.0f));
	
	c[6] = texture(uv_sampler, uv + vec2(-ubo.edTexOffset.x, ubo.edTexOffset.y));
	c[7] = texture(uv_sampler, uv + vec2(              0.0f, ubo.edTexOffset.y));
	c[8] = texture(uv_sampler, uv + vec2( ubo.edTexOffset.x, ubo.edTexOffset.y));

	vec4 trueColor = ubo.edColorWeight * c[4] - vec4( (c[0] + c[1] + c[2] + c[3] + c[5] + c[6] + c[7] + c[8]).rgb, c[4].w);
	return trueColor;
}
vec4 BlackAndWhite() {
	vec2 uv = vsUV + ubo.offsetUV;
	vec4 col = texture(uv_sampler, uv);
	float lum = dot(col, ubo.bwLumCoeff);
	
	if (ubo.bwGreyScaled)
		return vec4(lum, lum, lum, col.w);

	if (lum > ubo.bwLumPassingValue)
		return vec4(1, 1, 1, col.w);
	else
		return vec4(0, 0, 0, col.w); 
}
vec4 FishEye() {
	float apHalf = ubo.feAperature * 0.5f * (3.1415927f / 180.0f);
	float maxFactor = sin(apHalf);

	vec2 curUV = vsUV + ubo.offsetUV;
	vec2 uv = curUV;
	vec2 xy = 2.0f * uv - 1.0f;
	float d = length(xy);
	if (d < 2.0f - maxFactor) {
		d = length(xy * maxFactor);
		float z = sqrt(1.0f - d * d);
		float r = atan(d, z) / 3.1415927f;
		float phi = atan(xy.y, xy.x);

		uv.x = r * cos(phi) + 0.5f;
		uv.y = r * sin(phi) + 0.5f;
	}

	return texture(uv_sampler, uv);
}


void main() {
	//Case based on active effect
	switch(ubo.activeEffect) {
		case 0:
			psColor = texture(uv_sampler, vsUV + ubo.offsetUV);
			break;
		case 1:
			psColor = GaussianBlur();
			break;
		case 2:
			psColor = Swirling();
			break;
		case 3:
			psColor = Pixelate();
			break;
		case 4:
			psColor = EdgeDetection();
			break;
		case 5:
			psColor = BlackAndWhite();
			break;
		case 6:
			psColor = FishEye();
			break;
	}
}