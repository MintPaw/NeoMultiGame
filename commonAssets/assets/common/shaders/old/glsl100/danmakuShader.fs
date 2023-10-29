#version 100

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec4 hueShiftValue;

// NOTE: Add here your custom variables

vec3 hueShift(vec3 Color, float Shift) {
	vec3 P = vec3(0.55735)*dot(vec3(0.55735),Color);
	vec3 U = Color-P;
	vec3 V = cross(vec3(0.55735),U);    

	Color = U*cos(Shift*6.2832) + V*sin(Shift*6.2832) + P;

	return Color;
}

void main()
{
	// Texel color fetching from texture sampler
	vec4 texelColor = texture2D(texture0, fragTexCoord);

	// NOTE: Implement here your fragment shader code
	vec4 finalColor = texelColor*colDiffuse*fragColor;
	finalColor.rgb = hueShift(finalColor.rgb, hueShiftValue.x);
	gl_FragColor = finalColor;
}
