#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor; // Actually boneIndices
in vec4 vertexTangent; // Actually boneWeights

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;
uniform mat4 boneTransforms[32]; // Must match BONES_MAX

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

// NOTE: Add here your custom variables

void main()
{
    // Send vertex attributes to fragment shader
		mat4 boneTrans = boneTransforms[int(vertexColor.x)] * vertexTangent.x;
		boneTrans += boneTransforms[int(vertexColor.y)] * vertexTangent.y;
		boneTrans += boneTransforms[int(vertexColor.z)] * vertexTangent.z;
		boneTrans += boneTransforms[int(vertexColor.w)] * vertexTangent.w;
		vec3 modelPos = (vec4(vertexPosition, 1.0) * boneTrans).xyz;

    fragPosition = vec3(matModel*vec4(modelPos, 1.0));

    fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));

    fragTexCoord = vertexTexCoord;
    //fragColor = vertexColor;
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    fragNormal = normalize(vec3(matNormal*vec4(vertexNormal, 1.0)));

    // Calculate final vertex position
    gl_Position = mvp*vec4(modelPos, 1.0);
}
