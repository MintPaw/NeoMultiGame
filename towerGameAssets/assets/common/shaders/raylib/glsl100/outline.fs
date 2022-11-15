#version 100

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 resolution;
uniform float outlineSize;
uniform vec4 outlineColor;
uniform float outlineFadeOuter;
uniform float outlineFadeInner;

void main()
{
	vec2 res = resolution;

	vec3 texelRgb = texture2D(texture0, fragTexCoord).rgb;

	float texelAlpha = texture2D(texture0, fragTexCoord).a;
	bool alphaIsOver50 = bool(step(0.5, texelAlpha) == 1.0);

	const int maxDist = 20;
	const int halfMaxDist = maxDist / 2;

	float dist = float(maxDist);

	for (int x = -halfMaxDist; x != halfMaxDist; x++) {
		for (int y = -halfMaxDist; y != halfMaxDist; y++) {
			vec2 off = vec2(float(x), float(y));
			vec2 sep = fragTexCoord + (off / res);

			float sepAlpha = texture2D(texture0, sep).a;
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

	vec4 color = mix(border, vec4(texelRgb, 1.0), inner);

	gl_FragColor = color;
}
