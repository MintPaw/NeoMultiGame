in vec4 a_xyuv;
out vec2 v_uv;

void main() {
	v_uv = a_xyuv.zw;
	gl_Position = vec4(a_xyuv.xy, 0, 1);
}

/// FRAGMENT
##VERTEX_END

precision highp float;

in vec2 v_uv;
out vec4 fragColor;

uniform sampler2D u_texture;
uniform bool u_horizontal;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {             
	vec2 texOffset = 1.0 / textureSize(u_texture, 0); // gets size of single texel
	vec2 uv = v_uv;
	vec4 result = texture(u_texture, uv) * weight[0]; // current fragment's contribution

	if (u_horizontal) {
		for (int i = 1; i < 5; i++) {
			result += texture(u_texture, uv + vec2(texOffset.x * i, 0.0)) * weight[i];
			result += texture(u_texture, uv - vec2(texOffset.x * i, 0.0)) * weight[i];
		}
	} else {
		for (int i = 1; i < 5; i++) {
			result += texture(u_texture, uv + vec2(0.0, texOffset.y * i)) * weight[i];
			result += texture(u_texture, uv - vec2(0.0, texOffset.y * i)) * weight[i];
		}
	}

	fragColor = result;
	// fragColor = texture(u_texture, uv);
}
