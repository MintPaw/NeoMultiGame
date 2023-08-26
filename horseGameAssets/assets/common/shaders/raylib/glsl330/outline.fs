#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 resolution;
uniform float outlineSize;
uniform vec4 outlineColor;
uniform float outlineFadeOuter = 0.1;
uniform float outlineFadeInner = 0.01;

// Output fragment color
out vec4 finalColor;

void main()
{
#if 1
	// https://www.shadertoy.com/view/XlsXRB
	vec2 res = resolution;

	vec3 texelRgb = texture(texture0, fragTexCoord).rgb;

	float texelAlpha = texture(texture0, fragTexCoord).a;
	bool alphaIsOver50 = bool(step(0.5, texelAlpha) == 1.0);

	const int maxDist = 20;
	const int halfMaxDist = maxDist / 2;

	float dist = float(maxDist);

	for (int x = -halfMaxDist; x != halfMaxDist; x++) {
		for (int y = -halfMaxDist; y != halfMaxDist; y++) {
			vec2 off = vec2(float(x), float(y));
			vec2 sep = fragTexCoord + (off / res);

			float sepAlpha = texture(texture0, sep).a;
			bool sepAlphaIsOver50 = bool(step(0.5, sepAlpha) == 1.0);

			if ((!alphaIsOver50 && sepAlphaIsOver50) || (alphaIsOver50 && !sepAlphaIsOver50)) {
				dist = min(dist, length(off));
			}
		}
	}

	dist = clamp(dist, 0.0, float(maxDist)) / float(maxDist);

	if (alphaIsOver50) {
		dist = smoothstep(0, 0.4, dist);
		dist = -dist;
	} else {
		dist = smoothstep(0, 0.4, dist);
	}

	dist = dist * 0.5 + 0.5;
	dist = 1.0 - dist;

	float outer = smoothstep(0.5 - (outlineSize + outlineFadeOuter), 0.5, dist);

	vec4 border = mix(vec4(0.0), vec4(outlineColor.rgb, 1.0), outer);

	float inner = smoothstep(0.5, 0.5 + outlineFadeInner, dist);

	vec4 color = mix(border, vec4(texelRgb, 1.0), inner);

	finalColor = color;
#else
		/// Raylib version
    vec4 texel = texture(texture0, fragTexCoord);   // Get texel color
    vec2 texelScale = vec2(0.0);
    texelScale.x = outlineSize/textureSize.x;
    texelScale.y = outlineSize/textureSize.y;

    // We sample four corner texels, but only for the alpha channel (this is for the outline)
    vec4 corners = vec4(0.0);
    corners.x = texture(texture0, fragTexCoord + vec2(texelScale.x, texelScale.y)).a;
    corners.y = texture(texture0, fragTexCoord + vec2(texelScale.x, -texelScale.y)).a;
    corners.z = texture(texture0, fragTexCoord + vec2(-texelScale.x, texelScale.y)).a;
    corners.w = texture(texture0, fragTexCoord + vec2(-texelScale.x, -texelScale.y)).a;

    vec4 edges = vec4(0.0);
    edges.x = texture(texture0, fragTexCoord + vec2(texelScale.x, 0)).a;
    edges.y = texture(texture0, fragTexCoord + vec2(0, texelScale.y)).a;
    edges.z = texture(texture0, fragTexCoord + vec2(-texelScale.x, 0)).a;
    edges.w = texture(texture0, fragTexCoord + vec2(0, -texelScale.y)).a;

    float outline = min((dot(corners, vec4(1.0)) + dot(edges, vec4(1.0))), 1.0);
    vec4 color = mix(vec4(0.0), outlineColor, outline);
    finalColor = mix(color, texel, texel.a);
#endif
}
