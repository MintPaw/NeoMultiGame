// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);

    // NOTE: Implement here your fragment shader code

    finalColor = texelColor*fragColor;

#if (__VERSION__ == 100)
    gl_FragColor = finalColor;
#endif
}

