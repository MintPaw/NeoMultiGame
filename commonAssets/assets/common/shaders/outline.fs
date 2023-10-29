// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 resolution;
uniform float outlineSize;
uniform vec4 outlineColor;
uniform float outlineFadeOuter; // = 0.1;
uniform float outlineFadeInner; // = 0.01;

// Output fragment color
out vec4 finalColor;

void main()
{
	// https://www.shadertoy.com/view/XlsXRB

	vec3 texelRgb = texture(texture0, fragTexCoord).rgb * colDiffuse.rgb;

	float texelAlpha = texture(texture0, fragTexCoord).a;
	bool alphaIsOver50 = bool(step(0.5, texelAlpha) == 1.0);

	const int maxDist = 20;
	const int halfMaxDist = maxDist / 2;

	float dist = float(maxDist);

	for (int x = -halfMaxDist; x != halfMaxDist; x++) {
		for (int y = -halfMaxDist; y != halfMaxDist; y++) {
			vec2 off = vec2(float(x), float(y));
			vec2 sep = fragTexCoord + (off / resolution);

			float sepAlpha = texture(texture0, sep).a;
			bool sepAlphaIsOver50 = bool(step(0.5, sepAlpha) == 1.0);

			if ((!alphaIsOver50 && sepAlphaIsOver50) || (alphaIsOver50 && !sepAlphaIsOver50)) {
				dist = min(dist, length(off));
			}
		}
	}

	dist = clamp(dist, 0.0, float(maxDist)) / float(maxDist);

	if (alphaIsOver50) {
		dist = smoothstep(0.0, 0.4, dist);
		dist = -dist;
	} else {
		dist = smoothstep(0.0, 0.4, dist);
	}

	dist = dist * 0.5 + 0.5;
	dist = 1.0 - dist;

	float outer = smoothstep(0.5 - (outlineSize + outlineFadeOuter), 0.5, dist);

	vec4 border = mix(vec4(0.0), vec4(outlineColor.rgb, 1.0), outer);

	float inner = smoothstep(0.5, 0.5 + outlineFadeInner, dist);

	finalColor = mix(border, vec4(texelRgb, 1.0), inner);

#if (__VERSION__ == 100)
    gl_FragColor = finalColor;
#endif
}
