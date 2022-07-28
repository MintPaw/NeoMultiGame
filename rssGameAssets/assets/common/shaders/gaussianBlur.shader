in vec4 a_position;
uniform mat3 u_matrix;

out vec2 v_uv;

void main() {
	gl_Position = vec4(u_matrix * vec3(a_position.xy, 1.0), 1.0);
	mat3 flipMatrix = mat3(
		1,  0,  0,
		0, -1,  0,
		0,  1,  1
	);
	vec2 uv = vec3(flipMatrix * vec3(a_position.zw, 1.0)).xy;
	v_uv = uv;
}

##VERTEX_END

precision highp float;

in vec2 v_uv;
out vec4 fragColor;
uniform sampler2D u_texture;
uniform bool u_horizontal;


#if __VERSION__ == 330
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
#else  
#endif

void main(void) { 
#if __VERSION__ == 330
	vec2 texOffset = 1.0 / vec2(textureSize(u_texture, 0)); // gets size of single texel
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
#else  
	fragColor = vec4(0.0);
#endif

	// fragColor = texture(u_texture, uv);

}
