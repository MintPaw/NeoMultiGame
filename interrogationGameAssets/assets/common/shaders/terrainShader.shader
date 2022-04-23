in vec3 a_position;
in vec2 a_uv;
in vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_fragPos;
out vec3 v_lightPos;
out vec3 v_viewPos;

void main() {
	gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);

	v_uv = a_uv;
	v_lightPos = u_lightPos;
	v_viewPos = u_viewPos;
	v_fragPos = vec3(u_model * vec4(a_position, 1.0));

	v_normal = mat3(transpose(inverse(u_model))) * a_normal;
}

#VERTEX_END

precision highp float;

in vec2 v_uv;
in vec3 v_normal;
in vec3 v_fragPos;
in vec3 v_lightPos;
in vec3 v_viewPos;

uniform vec3 u_ambientLight;
uniform vec3 u_diffuseLight;
uniform vec3 u_specularLight;
uniform sampler2D u_diffuseTexture;
uniform sampler2D u_slopeTexture;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

void main(void) { 
	vec3 diffuseColor = texture(u_diffuseTexture, v_uv).rgb;
	vec3 slopeColor = texture(u_slopeTexture, v_uv).rgb;

	vec3 normal = normalize(v_normal);

	float slope = clamp((1.0 + normal.z) * 100.0, 0, 1);
	diffuseColor = mix(diffuseColor, slopeColor, slope);

	// ambient
	vec3 ambientTex = u_ambientLight * diffuseColor;

	// diffuse
	vec3 lightDir = normalize(v_lightPos - v_fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuseTex = u_diffuseLight * diff * diffuseColor;

	// specular
	vec3 viewDir = normalize(v_viewPos - v_fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	float specularStrength = 0.5;
	vec3 specularTex = u_specularLight * spec;

	vec3 resultColor = ambientTex + diffuseTex + specularTex;
	fragColor = vec4(resultColor.rgb, 1);


	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		bloomColor = vec4(fragColor.rgb, 1.0);
	} else {
		bloomColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
}
