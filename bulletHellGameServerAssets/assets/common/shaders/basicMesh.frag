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

IN vec2 v_uv;
IN vec3 v_normal;
IN vec3 v_fragPos;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform vec3 u_lightColor;
uniform vec4 u_tint;
uniform float u_alpha;
uniform int u_hasTexture;
uniform sampler2D u_texture;

OUT vec4 fragColor;

void main(void) { 
	vec3 modelColor;

	if (u_hasTexture == 1) {
		modelColor = TEXTURE2D(u_texture, v_uv).rgb;
	} else {
		modelColor = vec3(1.0, 1.0, 1.0);
	}

	modelColor = mix(modelColor, u_tint.rgb, u_tint.a);

	// ambient
	float ambientStrength = 0.1;
	vec3 ambientLight = ambientStrength * u_lightColor;

	// diffuse
	vec3 normal = normalize(v_normal);
	vec3 lightDir = normalize(u_lightPos - v_fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuseLight = diff * u_lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(u_viewPos - v_fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specularLight = specularStrength * spec * u_lightColor;

	vec3 resultColor = (ambientLight + diffuseLight + specularLight) * modelColor;
	fragColor = vec4(resultColor.rgb, u_alpha);

#if __VERSION__ == 100
	gl_FragColor = fragColor;
#endif
}
