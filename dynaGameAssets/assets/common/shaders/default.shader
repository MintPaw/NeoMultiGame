precision highp float;
#define _F_CIRCLE_V2 (1 << 0)
#define _F_THRESHOLD (1 << 1)
#define _F_ARC (1 << 2)
#define _F_BLUR13 (1 << 3)
#define _F_PERLIN (1 << 4)
#define _F_SANDSTORM (1 << 5)
#define _F_CIRCLE (1 << 6)
#define _F_HALO (1 << 7)
#define _F_INVERSE_SRGB (1 << 8)
#define _F_RECT_AA (1 << 9)
#define _F_ARC_V2 (1 << 10)

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_matrixData0;
layout(location = 2) in vec4 a_matrixData1;
layout(location = 3) in vec4 a_matrixData2;
layout(location = 4) in float a_alpha;
layout(location = 5) in float a_flags;
layout(location = 6) in vec4 a_tintColorShiftHueTexture;
layout(location = 7) in vec4 a_params;

out vec2 v_texCoord;
out float v_alpha;
flat out float v_flags;
flat out vec4 v_tintColorShiftHueTexture;
flat out vec4 v_tint;
flat out vec4 v_params;

void main(void) {
	mat3 uvMatrix = mat3(
		vec3(a_matrixData0[0], a_matrixData0[3], 0.0),
		vec3(a_matrixData0[1], a_matrixData1[0], 0.0),
		vec3(a_matrixData0[2], a_matrixData1[1], 1.0)
	);

	mat3 matrix = mat3(
		vec3(a_matrixData1[2], a_matrixData2[1], 0.0),
		vec3(a_matrixData1[3], a_matrixData2[2], 0.0),
		vec3(a_matrixData2[0], a_matrixData2[3], 1.0)
	);

	vec3 position = matrix * vec3(a_position.xy, 1);
	gl_Position = vec4(position, 1);

	v_texCoord = vec4((uvMatrix * vec3(a_position.zw, 1.0)).xy, 0, 1).xy;

	v_alpha = a_alpha;

	v_tintColorShiftHueTexture = a_tintColorShiftHueTexture;
	v_tint = hexToVec4(floatBitsToInt(a_tintColorShiftHueTexture.x));
	v_params = a_params;

	v_flags = a_flags;
}

/// FRAGMENT
##VERTEX_END
precision highp float;

#define _F_CIRCLE_V2 (1 << 0)
#define _F_THRESHOLD (1 << 1)
#define _F_ARC (1 << 2)
#define _F_BLUR13 (1 << 3)
#define _F_PERLIN (1 << 4)
#define _F_SANDSTORM (1 << 5)
#define _F_CIRCLE (1 << 6)
#define _F_HALO (1 << 7)
#define _F_INVERSE_SRGB (1 << 8)
#define _F_RECT_AA (1 << 9)
#define _F_ARC_V2 (1 << 10)

in vec2 v_texCoord;
in float v_alpha;
flat in float v_flags;
flat in vec4 v_tintColorShiftHueTexture;
flat in vec4 v_tint;
flat in vec4 v_params;
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

uniform sampler2D u_textures[8];
uniform float u_time;


vec4 filteredTexture(sampler2D aSampler, vec2 uv) {
	vec2 res = vec2(textureSize(aSampler, 0));
	uv = uv*res + 0.5;

	vec2 fl = floor(uv);
	vec2 fr = fract(uv);
	vec2 aa = fwidth(uv)*0.75;
	fr = smoothstep( vec2(0.5)-aa, vec2(0.5)+aa, fr);

	uv = (fl+fr-0.5) / res;

	vec4 fragment = texture(aSampler, uv);

	/// Pixel filter the edges
	vec2 uvPixel = fwidth(uv);
	vec2 border = linearstep(vec2(0.0), vec2(uvPixel), uv) * linearstep(vec2(0.0), vec2(uvPixel), vec2(1.0) - uv);
	fragment.a *= border.x * border.y;

	return fragment;
}

vec4 sampleTexture(sampler2D tex, vec2 uv) {
	int flags = int(v_flags);
	if ((flags & _F_BLUR13) != 0) {
		vec2 resolution = vec2(textureSize(tex, 0)); // gets size of single texel
		// vec2 direction = vec2(1.0, 1.0);
		vec2 direction = vec2(1.0, 1.0);
		resolution *= 2.0;

		vec4 color = vec4(0.0);
		vec2 off1 = vec2(1.411764705882353) * direction;
		vec2 off2 = vec2(3.2941176470588234) * direction;
		vec2 off3 = vec2(5.176470588235294) * direction;
		color += texture(tex, uv) * 0.1964825501511404;
		color += texture(tex, uv + (off1 / resolution)) * 0.2969069646728344;
		color += texture(tex, uv - (off1 / resolution)) * 0.2969069646728344;
		color += texture(tex, uv + (off2 / resolution)) * 0.09447039785044732;
		color += texture(tex, uv - (off2 / resolution)) * 0.09447039785044732;
		color += texture(tex, uv + (off3 / resolution)) * 0.010381362401148057;
		color += texture(tex, uv - (off3 / resolution)) * 0.010381362401148057;
		return color;
	} else {
		return texture(tex, uv);
	}
}
/// }}}

void main() {
	int flags = int(v_flags);

	float textureIndex = v_tintColorShiftHueTexture.w;

	vec4 tex = vec4(0.0);

#if 1
	switch (int(textureIndex)) {
		case 0: tex = sampleTexture(u_textures[0], v_texCoord); break;
		case 1: tex = sampleTexture(u_textures[1], v_texCoord); break;
		case 2: tex = sampleTexture(u_textures[2], v_texCoord); break;
		case 3: tex = sampleTexture(u_textures[3], v_texCoord); break;
		case 4: tex = sampleTexture(u_textures[4], v_texCoord); break;
		case 5: tex = sampleTexture(u_textures[5], v_texCoord); break;
		case 6: tex = sampleTexture(u_textures[6], v_texCoord); break;
		case 7: tex = sampleTexture(u_textures[7], v_texCoord); break;
	}
#else
	if (int(textureIndex) == 0) tex = sampleTexture(u_textures[0], v_texCoord);
	if (int(textureIndex) == 1) tex = sampleTexture(u_textures[1], v_texCoord);
	if (int(textureIndex) == 2) tex = sampleTexture(u_textures[2], v_texCoord);
	if (int(textureIndex) == 3) tex = sampleTexture(u_textures[3], v_texCoord);
	if (int(textureIndex) == 4) tex = sampleTexture(u_textures[4], v_texCoord);
	if (int(textureIndex) == 5) tex = sampleTexture(u_textures[5], v_texCoord);
	if (int(textureIndex) == 6) tex = sampleTexture(u_textures[6], v_texCoord);
	if (int(textureIndex) == 7) tex = sampleTexture(u_textures[7], v_texCoord);
#endif

	tex = vec4(tex.a > 0.0 ? tex.rgb / tex.a : vec3(0), tex.a);

	if ((flags & _F_PERLIN) != 0) {
		float v = snoise(v_texCoord);

		tex.a = v;
	}

	if ((flags & _F_RECT_AA) != 0) {
	}
	// vec2 uvPixel = fwidth(v_texCoord); // filteredTexture does this
	// vec2 border = linearstep(vec2(0.0), vec2(uvPixel), v_texCoord) * linearstep(vec2(0.0), vec2(uvPixel), vec2(1.0) - v_texCoord);
	// tex.a *= border.x * border.y;

	if ((flags & _F_CIRCLE) != 0) {
		float v = circleSDF(v_texCoord + vec2(0.5, 0.5));
		v = 1.0 - smoothstep(0.45, 0.5, v);
		tex.a = v;
	}

	if ((flags & _F_HALO) != 0) {
		vec2 coords = vec2(0.0, -1.0) + v_texCoord + v_params.xy*vec2(-1.0, 1.0);
		float v = length(coords * v_params.w);
		v = smoothstep((1.0-v_params.z)/2.0, 0.5, v);
		tex.a = v;
	}

	if ((flags & _F_CIRCLE_V2) != 0) {
		float v = circleSDF(v_texCoord + vec2(0.5, 0.5));
		v = 1.0 - smoothstep(0.1, 0.55, v);
		tex.a = v;
	}

	if ((flags & _F_ARC) != 0) {
		float ringSize = 1.0-v_params.x;
		float perc = v_params.y;

		vec2 uv = v_texCoord;
		uv = 1.0 - uv;
		vec2 st = (uv + vec2(0.5, 0.5)) - 1.0;
		float v = length(st);
		float res = atan(st.y, st.x) + PI;
		if (res > perc * 2.0*PI) discard;

		tex.a = (smoothstep(ringSize/2.0, 0.5, v) * smoothstep(0.5, ringSize/2.0, v) * 6.0);
	}

	if ((flags & _F_ARC_V2) != 0) {
		float perc = v_params.x;

		vec2 uv = v_texCoord;
		uv = 1.0 - uv;
		vec2 st = (uv + vec2(0.5, 0.5)) - 1.0;
		float res = atan(st.y, st.x) + PI;
		if (res > perc * 2.0*PI) discard;
	}

	if ((flags & _F_THRESHOLD) != 0) {
		if (tex.a < 0.9) {
			tex.a = 0.0;
		} else {
			tex.a = 1.0;
		}
		// tex.a = smoothstep(0.9, 1.0, tex.a);
	}

	fragColor = tex * v_tint;
	fragColor.a *= v_alpha;

	fragColor.rgb *= fragColor.a;

	bloomColor = vec4(0.0, 0.0, 0.0, 0.0);
}
