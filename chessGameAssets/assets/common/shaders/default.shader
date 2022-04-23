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
#define _F_RTS_TEAM1 (1 << 10)

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_destSize;
layout(location = 2) in vec4 a_matrixData0;
layout(location = 3) in vec4 a_matrixData1;
layout(location = 4) in vec4 a_matrixData2;
layout(location = 5) in float a_alpha;
layout(location = 6) in float a_flags;
layout(location = 7) in vec4 a_tintColorShiftHueTexture;
layout(location = 8) in vec4 a_params;

out vec2 v_texCoord;
out float v_alpha;
flat out vec4 v_destSize;
out vec4 v_fxaaPosPos;
flat out float v_flags;
flat out vec4 v_tintColorShiftHueTexture;
flat out vec4 v_tint;
flat out vec4 v_colorShift;
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
	v_colorShift = hexToVec4(floatBitsToInt(v_tintColorShiftHueTexture.y));
	v_params = a_params;

	v_destSize = a_destSize;
	v_flags = a_flags;

	vec2 rcpFrame = vec2(1.0/a_destSize.x, 1.0/a_destSize.y);
	float fxaaSubPixelShift = 1.0/4.0;
	v_fxaaPosPos.xy = v_texCoord;
	v_fxaaPosPos.zw = v_texCoord - (rcpFrame * (0.5 + fxaaSubPixelShift));
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
#define _F_RTS_TEAM1 (1 << 10)

in vec2 v_texCoord;
in float v_alpha;
flat in vec4 v_destSize;
in vec4 v_fxaaPosPos;
flat in float v_flags;
flat in vec4 v_tintColorShiftHueTexture;
flat in vec4 v_tint;
flat in vec4 v_colorShift;
flat in vec4 v_params;
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

uniform sampler2D u_textures[16];
uniform float u_time;

/// {{{

//vec3 getFbmVortex(vec2 uv) {
//	// float noise = fbm(uv*100.0);
//	// vec4 ret = vec4(noise, noise, noise, texture(tex, uv));
//	// return ret;

//	float time = u_time * 3.0;
//	vec2 st = uv * 50.0;
//	//st += st * abs(sin(time*0.1)*3.0);
//	vec3 color = vec3(0.0);

//	vec2 q = vec2(0.);
//	q.x = fbm( st + 0.00*time);
//	q.y = fbm( st + vec2(1.0));

//	vec2 r = vec2(0.);
//	r.x = fbm( st + 1.0*q + vec2(1.7,9.2)+ 0.15*time );
//	r.y = fbm( st + 1.0*q + vec2(8.3,2.8)+ 0.126*time);

//	float f = fbm(st+r);

//	// color = mix(vec3(0.101961,0.619608,0.666667),
//	// 	vec3(0.666667,0.666667,0.498039),
//	// 	clamp((f*f)*4.0,0.0,1.0));

//	color = mix(color,
//		vec3(0.830,0.799,0.254),
//		clamp(length(q),0.0,1.0));

//	color = mix(color,
//		vec3(1.000,0.501,0.074),
//		clamp(length(r.x),0.0,1.0));

//	return vec3(color);
//}

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

	vec4 tint = v_tint;
	vec4 colorShift = v_colorShift;

	float textureIndex = v_tintColorShiftHueTexture.w;

	vec4 tex = vec4(0.0);

	switch (int(textureIndex)) {
		case 0: tex = sampleTexture(u_textures[0], v_texCoord); break;
		case 1: tex = sampleTexture(u_textures[1], v_texCoord); break;
		case 2: tex = sampleTexture(u_textures[2], v_texCoord); break;
		case 3: tex = sampleTexture(u_textures[3], v_texCoord); break;
		case 4: tex = sampleTexture(u_textures[4], v_texCoord); break;
		case 5: tex = sampleTexture(u_textures[5], v_texCoord); break;
		case 6: tex = sampleTexture(u_textures[6], v_texCoord); break;
		case 7: tex = sampleTexture(u_textures[7], v_texCoord); break;
		case 8: tex = sampleTexture(u_textures[8], v_texCoord); break;
		case 9: tex = sampleTexture(u_textures[9], v_texCoord); break;
		case 10: tex = sampleTexture(u_textures[10], v_texCoord); break;
		case 11: tex = sampleTexture(u_textures[11], v_texCoord); break;
		case 12: tex = sampleTexture(u_textures[12], v_texCoord); break;
		case 13: tex = sampleTexture(u_textures[13], v_texCoord); break;
		case 14: tex = sampleTexture(u_textures[14], v_texCoord); break;
		case 15: tex = sampleTexture(u_textures[15], v_texCoord); break;
	}

	tex = vec4(tex.a > 0.0 ? tex.rgb / tex.a : vec3(0), tex.a);

	if ((flags & _F_PERLIN) != 0) {
		float v = snoise(v_texCoord);

		tex.a = v;
	}

	if ((flags & _F_RECT_AA) != 0) {
	}
	vec2 uvPixel = fwidth(v_texCoord);
	vec2 border = linearstep(vec2(0.0), vec2(uvPixel), v_texCoord) * linearstep(vec2(0.0), vec2(uvPixel), vec2(1.0) - v_texCoord);
	tex.a *= border.x * border.y;

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
		float rads = 3.14159 * 0.5;
		if (res > perc * 2.0*PI) discard;

		tex.a = (smoothstep(ringSize/2.0, 0.5, v) * smoothstep(0.5, ringSize/2.0, v) * 6.0);
	}

	if ((flags & _F_THRESHOLD) != 0) {
		if (tex.a < 0.9) {
			tex.a = 0.0;
		} else {
			tex.a = 1.0;
		}
		// tex.a = smoothstep(0.9, 1.0, tex.a);
	}

	fragColor = mix(tex, tint, tint.a); // You could still be using tex here if you simplified tint and colorShift to just use rgb
	fragColor = fragColor * (vec4(colorShift.rgb, 1)*colorShift.a);
	fragColor.rgb = hueShift(fragColor.rgb, v_tintColorShiftHueTexture.z);
	// fragColor += (0.000001 * vec4(v_destSize.xy, 0.0, 0.0));
	fragColor.a = tex.a * v_alpha;

	fragColor.rgb *= fragColor.a;

	if ((flags & _F_INVERSE_SRGB) != 0) {
		fragColor.rgb = sqrt(fragColor.rgb);
	}

	bloomColor = vec4(0.0, 0.0, 0.0, 0.0);
}
