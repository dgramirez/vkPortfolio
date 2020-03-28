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
	//Set UV with offset
	vec2 uv = vsUV + ubo.offsetUV;

	//Set Texture Color, Weighted
	vec4 texColor = texture(uv_sampler, uv) * ubo.gbWeight[0];
	
	//Vertical Pass
	for (int i = 1; i < 3; ++i) {
		texColor += texture(uv_sampler, uv + vec2(0.0f, ubo.gbOffset[i-1]) * ubo.gbUVWeight[0]) * ubo.gbWeight[i];
		texColor += texture(uv_sampler, uv - vec2(0.0f, ubo.gbOffset[i-1]) * ubo.gbUVWeight[1]) * ubo.gbWeight[i];
	}

	//Horizontal Pass
		for (int i = 1; i < 3; ++i) {
		texColor += texture(uv_sampler, uv + vec2(ubo.gbOffset[i-1], 0.0f) * ubo.gbUVWeight[0]) * ubo.gbWeight[i];
		texColor += texture(uv_sampler, uv - vec2(ubo.gbOffset[i-1], 0.0f) * ubo.gbUVWeight[1]) * ubo.gbWeight[i];
	}

	//Return the Final Texture Color
	return texColor;
}
vec4 Swirling() {
	//Set UV with offset
	vec2 uv = vsUV + ubo.offsetUV;

	//Sizing U and V based on the Textel Size
	vec2 tc = uv * ubo.swTexSize;

	//Subtract the Center
	tc -= ubo.swCenter;

	//Get the distance and check if that distance is less than the Radius
	float dist = length(tc);
	if (dist < ubo.swRadius) {

		//Get the Percentage from the difference of the radius and distance
		float p = (ubo.swRadius - dist) / ubo.swRadius;

		//Get theta (p^2 * angle * constant)
		float theta = p * p * ubo.swAngle * ubo.swThetaFactor;

		//Get the Sin of Theta
		float s = sin(theta);

		//Get the Cosine of Theta
		float c = cos(theta);
		
		//Set the Textel by dotting itself to the sin and cos vectors
		tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
	}

	//Add The Center Back
	tc += ubo.swCenter;

	//Return the Texture, with the uv based on the textel ratio of the textel size
	return texture(uv_sampler, tc / ubo.swTexSize);
}
vec4 Pixelate() {
	//Set UV with offset
	vec2 uv = vsUV + ubo.offsetUV;

	//Sizing U and V based on Pixel Size, then Casted into an integer
	int u = int(uv.x * ubo.pxSize);
	int v = int(uv.y * ubo.pxSize);
	
	//Return The Texture Coordinate Based on the ratio between uv and Pixel Size.
	return texture(uv_sampler, vec2(float(u)/ubo.pxSize, float(v)/ubo.pxSize));
}
vec4 EdgeDetection() {
	//Set UV with offset
	vec2 uv = vsUV + ubo.offsetUV;

	//Create 9 Texture Colors, all with offsets based on negative, 0, and positive NEIGHBORING offset values
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

	//Return the Color based on weighted Base Color and the sum of all the Neighbor texture values.
	return ubo.edColorWeight * c[4] - vec4( (c[0] + c[1] + c[2] + c[3] + c[5] + c[6] + c[7] + c[8]).rgb, c[4].w);
}
vec4 BlackAndWhite() {
	//Set UV with offset
	vec2 uv = vsUV + ubo.offsetUV;

	//Get the color of the texture
	vec4 col = texture(uv_sampler, uv);

	//Get the Luminance by dotting the color and luminosity Coefficient
	float lum = dot(col, ubo.bwLumCoeff);
	
	//If Greyscaled is checked, return the color with rgb being the luminosity
	if (ubo.bwGreyScaled)
		return vec4(lum, lum, lum, col.w);

	//If luminosity value passes, return White (With Alpha!) 
	if (lum > ubo.bwLumPassingValue)
		return vec4(1, 1, 1, col.w);

	//Return Black (With Alpha!)
	return vec4(0, 0, 0, col.w); 
}
vec4 FishEye() {
	//Set UV with offset
	vec2 uv = vsUV + ubo.offsetUV;

	//Get Half the Aperature (As a angle in degree) and Set it in Radians 
	float apHalf = ubo.feAperature * 0.5f * (3.1415927f / 180.0f);

	//Set the Max Factor to the sin of Half of Aperature.
	float maxFactor = sin(apHalf);

	//Set the uv (as x and y) to be between -1 and 1 (Assuming uv is 0-1)
	vec2 xy = 2.0f * uv - 1.0f;

	//Get the length of the above xy value
	float d = length(xy);

	//if the length is less than the 
	if (d < 2.0f - maxFactor) {
		//Set the length to the xy value times maxFactor		
		d = length(xy * maxFactor);

		//Get the Sqrt of (1 - d^2)
		float z = sqrt(1.0f - d * d);

		//Do the Arc Tangent with the length and the Sqrt, then divide it by PI.
		float r = atan(d, z) / 3.1415927f;

		//Get the Phi from doing Arc Tangent of xy value's y and x
		float phi = atan(xy.y, xy.x);

		//Set UVs to the coordinates and add 0.5
		uv.x = r * cos(phi) + 0.5f;
		uv.y = r * sin(phi) + 0.5f;
	}

	//Return the color based on the uv, modified or not.
	return texture(uv_sampler, uv);
}


void main() {
	//Case based on active effect
	switch(ubo.activeEffect) {
		case 0:
			//Set Color to the texture color
			psColor = texture(uv_sampler, vsUV + ubo.offsetUV);
			break;
		case 1:
			//Set Color to the texture through the Gaussian Blur Function
			psColor = GaussianBlur();
			break;
		case 2:
			//Set Color to the texture through the Swirling Function
			psColor = Swirling();
			break;
		case 3:
			//Set Color to the texture through the Pixelation Function
			psColor = Pixelate();
			break;
		case 4:
			//Set Color to the texture through the Edge Detection Function
			psColor = EdgeDetection();
			break;
		case 5:
			//Set Color to the texture through the Black & White Function
			psColor = BlackAndWhite();
			break;
		case 6:
			//Set Color to the texture through the Fish Eye Function
			psColor = FishEye();
			break;
	}
}