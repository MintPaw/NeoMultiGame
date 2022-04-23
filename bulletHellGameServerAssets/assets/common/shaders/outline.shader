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

uniform vec2 u_textureSize;
uniform vec4 u_color;
uniform float u_thickness;
uniform sampler2D u_texture;

void main() { 
	vec2 stepSize = (1.0/u_textureSize) * u_thickness;

#if 1
	vec4 col = texture2D(u_texture, v_uv);
	if (col.a > 0.5) discard;
	float a =
		texture2D(u_texture, v_uv + vec2(1, 0)*stepSize).a +
		texture2D(u_texture, v_uv + vec2(-1, 0)*stepSize).a +
		texture2D(u_texture, v_uv + vec2(0, 1)*stepSize).a +
		texture2D(u_texture, v_uv + vec2(0, -1)*stepSize).a;

	if (col.a < 1.0 && a > 0.0) {
		fragColor = u_color;
	} else {
		discard;
	}
#else
	float alpha = 4*texture2D(u_texture, v_uv).a;
	alpha -= texture2D(u_texture, v_uv + vec2(1, 0)*stepSize).a;
	alpha -= texture2D(u_texture, v_uv + vec2(-1, 0)*stepSize).a;
	alpha -= texture2D(u_texture, v_uv + vec2(0, 1)*stepSize).a;
	alpha -= texture2D(u_texture, v_uv + vec2(0, -1)*stepSize).a;

	fragColor.rgb = u_color.rgb;
	fragColor.a = alpha;
#endif

	fragColor.rgb *= fragColor.a;
}
