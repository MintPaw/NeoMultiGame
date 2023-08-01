#define MAX_LIGHTS 4

struct Light {
	int type;
	Raylib::Vector3 position;
	Raylib::Vector3 target;
	Raylib::Color color;
	bool enabled;

	int enabledLoc;
	int typeLoc;
	int posLoc;
	int targetLoc;
	int colorLoc;
};

enum LightType {
	LIGHT_DIRECTIONAL,
	LIGHT_POINT
};

Light createLight(int type, Raylib::Vector3 position, Raylib::Vector3 target, Raylib::Color color, Raylib::Shader shader);
void updateLightValues(Raylib::Shader shader, Light light);

static int lightsCount = 0;

Light createLight(int type, Raylib::Vector3 position, Raylib::Vector3 target, Raylib::Color color, Raylib::Shader shader) {
	Light light = {};

	if (lightsCount < MAX_LIGHTS) {
		light.enabled = true;
		light.type = type;
		light.position = position;
		light.target = target;
		light.color = color;

		char *frameSprintf(const char *msg, ...);

		light.enabledLoc = GetShaderLocation(shader, frameSprintf("lights[%d].enabled", lightsCount));
		light.typeLoc = GetShaderLocation(shader, frameSprintf("lights[%d].type", lightsCount));
		light.posLoc = GetShaderLocation(shader, frameSprintf("lights[%d].position", lightsCount));
		light.targetLoc = GetShaderLocation(shader, frameSprintf("lights[%d].target", lightsCount));
		light.colorLoc = GetShaderLocation(shader, frameSprintf("lights[%d].color", lightsCount));

		updateLightValues(shader, light);

		lightsCount++;
	}

	return light;
}

void updateLightValues(Raylib::Shader shader, Light light) {
	SetShaderValue(shader, light.enabledLoc, &light.enabled, Raylib::SHADER_UNIFORM_INT);
	SetShaderValue(shader, light.typeLoc, &light.type, Raylib::SHADER_UNIFORM_INT);

	SetShaderValue(shader, light.posLoc, &light.position, Raylib::SHADER_UNIFORM_VEC3);

	SetShaderValue(shader, light.targetLoc, &light.target, Raylib::SHADER_UNIFORM_VEC3);

	float color[4] = { (float)light.color.r/(float)255, (float)light.color.g/(float)255, (float)light.color.b/(float)255, (float)light.color.a/(float)255 };
	SetShaderValue(shader, light.colorLoc, color, Raylib::SHADER_UNIFORM_VEC4);
}
