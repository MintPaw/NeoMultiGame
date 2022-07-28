#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec4 hueShiftValue;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

vec3 hueShift(vec3 Color, float Shift) {
	vec3 P = vec3(0.55735)*dot(vec3(0.55735),Color);
	vec3 U = Color-P;
	vec3 V = cross(vec3(0.55735),U);    

	Color = U*cos(Shift*6.2832) + V*sin(Shift*6.2832) + P;

	return Color;
}

void main() {
	// Texel color fetching from texture sampler
	vec4 texelColor = texture(texture0, fragTexCoord);

	// NOTE: Implement here your fragment shader code
	// if (texelColor.a == 0.0) discard;

	finalColor = texelColor*colDiffuse*fragColor;
	finalColor.rgb = hueShift(finalColor.rgb, hueShiftValue.x);
}
