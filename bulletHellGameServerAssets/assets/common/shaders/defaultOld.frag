precision highp float;

#if __VERSION__ >= 300
# define IN in
# define OUT out
# define TEXTURE2D texture
#elif __VERSION__ >= 100
# define IN varying
# define OUT
# define TEXTURE2D texture2D
#endif

IN vec2 v_texCoord;
IN vec4 v_tint;
OUT vec4 fragColor;

uniform float u_alpha;
uniform sampler2D u_texture;
uniform bool u_unmultiplyAlpha;

void main(void) { 
	// fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	// return;

	vec4 tex = TEXTURE2D(u_texture, v_texCoord);
	fragColor = mix(tex, v_tint, v_tint.a);
	fragColor.a = tex.a;


	if (u_unmultiplyAlpha) fragColor.rgb /= fragColor.a;
	fragColor.a -= 1.0-u_alpha;

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
