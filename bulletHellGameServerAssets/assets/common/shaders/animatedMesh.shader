#define IN in
#define OUT out

IN vec3 a_position;
IN vec2 a_uv;
IN vec3 a_normal;
IN vec4 a_boneIndices;
IN vec4 a_boneWeights;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_boneTransforms[128]; // Must match BONES_PER_ANIMATION_LIMIT

OUT vec2 v_uv;
OUT vec3 v_normal;
OUT vec3 v_fragPos;

void main() {
	mat4 boneTrans = u_boneTransforms[int(a_boneIndices.x)] * a_boneWeights.x;
	boneTrans += u_boneTransforms[int(a_boneIndices.y)] * a_boneWeights.y;
	boneTrans += u_boneTransforms[int(a_boneIndices.z)] * a_boneWeights.z;
	boneTrans += u_boneTransforms[int(a_boneIndices.w)] * a_boneWeights.w;
	vec3 modelPos = (vec4(a_position, 1.0) * boneTrans).xyz;
	vec3 normalPos = a_normal * mat3(transpose(inverse(boneTrans)));

	gl_Position = u_projection * u_view * u_model * vec4(modelPos, 1.0);
	v_uv = a_uv;

	v_fragPos = vec3(u_model * vec4(modelPos, 1.0));
	// v_fragPos = modelPos;
	v_normal = mat3(transpose(inverse(u_model))) * normalPos;
	// v_normal = normalPos;
}

#VERTEX_END

precision highp float;

#define IN in
#define OUT out
#define TEXTURE2D texture

#define _F_3D_BLOOM (1 << 0)
#define _F_3D_NO_GEOM (1 << 1)

IN vec2 v_uv;
IN vec3 v_normal;
IN vec3 v_fragPos;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform vec3 u_lightColor;
uniform vec4 u_tint;
uniform int u_hasTexture;
uniform int u_flags;
uniform vec4 u_alpha;
uniform sampler2D u_texture;

layout (location = 0) OUT vec4 fragColor;
layout (location = 1) OUT vec4 bloomColor;

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
	fragColor = vec4(resultColor.rgb, u_alpha.r) * (~u_flags & _F_3D_NO_GEOM);

	if ((u_flags & _F_3D_BLOOM) == 1) {
		bloomColor = vec4(modelColor, u_alpha.r);
	} else {
		bloomColor = vec4(0.0, 0.0, 0.0, 0.0);
	}

	// bloomColor = vec4(modelColor, 1.0) * (u_flags & _F_3D_BLOOM);
}
