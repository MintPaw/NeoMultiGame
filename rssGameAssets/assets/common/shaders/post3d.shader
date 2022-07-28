in vec4 a_position;
uniform mat3 u_matrix;
uniform vec2 u_destSize;

out vec2 v_uv;
out vec4 v_posPos;

void main() {
	gl_Position = vec4(u_matrix * vec3(a_position.xy, 1.0), 1.0);
	mat3 flipMatrix = mat3(
		1,  0,  0,
		0, -1,  0,
		0,  1,  1
	);
	vec2 uv = vec3(flipMatrix * vec3(a_position.zw, 1.0)).xy;
	v_uv = uv;

	vec2 rcpFrame = vec2(1.0/u_destSize.x, 1.0/u_destSize.y);
	float fxaaSubPixelShift = 1.0/4.0;
	v_posPos.xy = uv;
	v_posPos.zw = uv - (rcpFrame * (0.5 + fxaaSubPixelShift));
}

##VERTEX_END

precision highp float;

in vec2 v_uv;
in vec4 v_posPos;
out vec4 fragColor;
uniform sampler2D u_texture;
uniform vec2 u_destSize;
uniform vec4 u_tint;

void main(void) { 
	vec4 c = vec4(0.0);
	vec2 rcpFrame = vec2(1.0/u_destSize.x, 1.0/u_destSize.y);
	c.rgb = FxaaPixelShader(v_posPos, u_texture, rcpFrame);
	//c.rgb = 1.0 - texture2D(tex, v_fxaaPosPos.xy).rgb;
	c.a = texture(u_texture, v_uv).a;

	fragColor.rgb = mix(c.rgb, u_tint.rgb, u_tint.a);
	fragColor.a = c.a;

	// fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
