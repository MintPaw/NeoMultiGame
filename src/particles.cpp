struct Particle {
	int imageIndex;

	Vec2 position;
	Vec2 velo;

	Vec2 startAccel, endAccel;

	float startScale, endScale;

	// float rotation;
	// float rotationVelo;
	// float startAngularAccel, endAngularAccel;
	float lifetime;

	int startColor, endColor;

	float time;
};

struct EmitterInfo {
#define EMITTER_INFO_NAME_MAX_LEN 64
	char name[EMITTER_INFO_NAME_MAX_LEN];

	char textureDir[PATH_MAX_LEN];
	int imageCount;
	float particlesPerSec;

	Vec2 startDir, startDirVariance;
	float startSpeed, startSpeedVariance;

	Vec2 startAccel, startAccelVariance;
	Vec2 endAccel, endAccelVariance;

	float startScale, startScaleVariance;
	float endScale, endScaleVariance;

	float startRotation, startRotationVariance;
	float startAngularSpeed, startAngularSpeedVariance;
	float startAngularAccel, startAngularAccelVariance;
	float endAngularAccel, endAngularAccelVariance;

	float lifetime, lifetimeVariance;

	int startColor, startColorVariance;
	int endColor, endColorVariance;
};

enum EmitterShape {
	EMITTER_SHAPE_POINT,
	// EMITTER_SHAPE_LINE,
	// EMITTER_SHAPE_RECT,
	// EMITTER_SHAPE_CIRCLE,
};
struct Emitter {
	EmitterShape shape;
	Vec2 position;

	EmitterInfo *info;
	char prevTextureDir[PATH_MAX_LEN];
	SpriteSheet *sheet;

	float timeLeftTillNextParticle;

	Particle *particles;
	int particlesNum;
	int particlesMax;
};

Emitter *createEmitter();
void updateEmitter(Emitter *emitter, float elapsed);
Particle *emit(Emitter *emitter);
void drawEmitter(Emitter *emitter);

void guiInputEmitter(Emitter *emitter);
void guiInputEmitterInfo(EmitterInfo *info);
void writeEmitterInfo(DataStream *stream, EmitterInfo *info);
void readEmitterInfo(DataStream *stream, EmitterInfo *info);
bool saveLoadEmitterInfo(DataStream *stream, bool save, int version, EmitterInfo *info, int startVersion, int endVersion);
/// FUNCTIONS ^

// struct EmitterSystem {
// };

// EmitterSystem *emitterSys = NULL;

Emitter *createEmitter() {
	Emitter *emitter = (Emitter *)zalloc(sizeof(Emitter));
	emitter->particlesMax = 128;
	emitter->particles = (Particle *)zalloc(sizeof(Particle) * emitter->particlesMax);
	return emitter;
}

void updateEmitter(Emitter *emitter, float elapsed) {
	EmitterInfo *info = emitter->info;
	if (!info) {
		logf("Updating emitter with no info!\n");
		return;
	}

	if (!streq(emitter->prevTextureDir, info->textureDir)) {
		strcpy(emitter->prevTextureDir, info->textureDir);
		if (emitter->sheet) destroySpriteSheet(emitter->sheet);
		emitter->sheet = createSpriteSheet(info->textureDir);
	}

	if (info->particlesPerSec > 0) {
		emitter->timeLeftTillNextParticle -= elapsed;
		float timeBetween = 1 / info->particlesPerSec;
		if (emitter->timeLeftTillNextParticle > timeBetween) emitter->timeLeftTillNextParticle = timeBetween;
		while (emitter->timeLeftTillNextParticle < 0) {
			emitter->timeLeftTillNextParticle += timeBetween;
			emit(emitter);
		}
	}

	for (int i = 0; i < emitter->particlesNum; i++) {
		Particle *particle = &emitter->particles[i];

		float perc = particle->time / particle->lifetime;

		Vec2 accel = lerp(particle->startAccel, particle->endAccel, perc);

		particle->velo += accel;
		particle->position += particle->velo;

		particle->time += elapsed;
		if (particle->time > particle->lifetime) {
			arraySpliceIndex(emitter->particles, emitter->particlesNum, sizeof(Particle), i);
			emitter->particlesNum--;
			i--;
			continue;
		}
	}
}

Particle *emit(Emitter *emitter) {
	EmitterInfo *info = emitter->info;

	if (emitter->particlesNum > emitter->particlesMax-1) {
		emitter->particles = (Particle *)resizeArray(emitter->particles, sizeof(Particle), emitter->particlesMax, emitter->particlesMax*2);
		emitter->particlesMax *= 2;
	}

	Particle *particle = &emitter->particles[emitter->particlesNum++];
	memset(particle, 0, sizeof(Particle));

	if (emitter->sheet) {
		particle->imageIndex = rndInt(0, emitter->sheet->imagesNum-1);
	}

	if (emitter->shape == EMITTER_SHAPE_POINT) {
		particle->position = emitter->position;
	}

	Vec2 dir;
	dir.x = info->startDir.x + info->startDirVariance.x*rndFloat(-1, 1);
	dir.y = info->startDir.y + info->startDirVariance.y*rndFloat(-1, 1);
	float speed = info->startSpeed + info->startSpeedVariance*rndFloat(-1, 1);

	particle->velo = normalize(dir) * speed;

	particle->startAccel = info->startAccel + info->startAccelVariance*rndFloat(-1, 1);
	particle->endAccel = info->endAccel + info->endAccelVariance*rndFloat(-1, 1);

	particle->startScale = info->startScale + info->startScaleVariance*rndFloat(-1, 1);
	particle->endScale = info->endScale + info->endScaleVariance*rndFloat(-1, 1);
	// @todo
	// float rotation;
	// float rotationVelo;
	// float startAngularAccel, endAngularAccel;

	particle->lifetime = info->lifetime + info->lifetimeVariance*rndFloat(-1, 1);

	{
		int a, r, g, b;
		int va, vr, vg, vb;
		hexToArgb(info->startColor, &a, &r, &g, &b);
		hexToArgb(info->startColorVariance, &va, &vr, &vg, &vb);

		a = mathClamp(a + va*rndFloat(-1, 1), 0, 255);
		r = mathClamp(r + vr*rndFloat(-1, 1), 0, 255);
		g = mathClamp(g + vg*rndFloat(-1, 1), 0, 255);
		b = mathClamp(b + vb*rndFloat(-1, 1), 0, 255);

		particle->startColor = argbToHex(a, r, g, b);
	}

	{
		int a, r, g, b;
		int va, vr, vg, vb;
		hexToArgb(info->endColor, &a, &r, &g, &b);
		hexToArgb(info->endColorVariance, &va, &vr, &vg, &vb);

		a = mathClamp(a + va*rndFloat(-1, 1), 0, 255);
		r = mathClamp(r + vr*rndFloat(-1, 1), 0, 255);
		g = mathClamp(g + vg*rndFloat(-1, 1), 0, 255);
		b = mathClamp(b + vb*rndFloat(-1, 1), 0, 255);

		particle->endColor = argbToHex(a, r, g, b);
	}

	return particle;
}

void drawEmitter(Emitter *emitter) {
	// EmitterInfo *info = emitter->info;
	// if (!info) {
	// 	logf("Drawing emitter with no info!\n");
	// 	return;
	// }

	for (int i = 0; i < emitter->particlesNum; i++) {
		Particle *particle = &emitter->particles[i];
		float perc = particle->time / particle->lifetime;

		int color = lerpColor(particle->startColor, particle->endColor, perc);

		float scale = lerp(particle->startScale, particle->endScale, perc);

		if (emitter->sheet) {
			SpriteSheet *sheet = emitter->sheet;
			SpriteSheetImage *image = &sheet->images[particle->imageIndex];

			RenderProps props = newRenderProps();
			props.srcWidth = props.srcHeight = 1;
			props.matrix.TRANSLATE(particle->position);
			props.matrix.SCALE(image->srcWidth, image->srcHeight);

			props.uv0.x = (float)image->srcX / (float)sheet->texture->width;
			props.uv0.y = (float)image->srcY / (float)sheet->texture->height;
			props.uv1.x = (float)(image->srcX+image->srcWidth) / (float)sheet->texture->width;
			props.uv1.y = (float)(image->srcY+image->srcHeight) / (float)sheet->texture->height;

			props.tint = color;

			drawTexture(emitter->sheet->texture, props);
		} else {
			Circle circle = makeCircle(particle->position, 32*scale);
			drawCircle(circle, color);
		}
	}
}

void guiInputEmitter(Emitter *emitter) {
	ImGui::PushID(emitter);

	ImGui::Begin("Emitter", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Particles: %d/%d", emitter->particlesNum, emitter->particlesMax);
	ImGui::DragFloat2("Position", &emitter->position.x);
	ImGui::End();

	ImGui::PopID();
}

void guiInputEmitterInfo(EmitterInfo *info) {
	ImGui::PushID(info);

	ImGui::InputText("Name", info->name, EMITTER_INFO_NAME_MAX_LEN);
	ImGui::InputText("Texture path", info->textureDir, PATH_MAX_LEN);
	ImGui::InputInt("Image count", &info->imageCount);

	ImGui::DragFloat("Particles per sec", &info->particlesPerSec);

	ImGui::DragFloat2("Start direction", &info->startDir.x, 0.1);
	ImGui::SameLine();
	ImGui::DragFloat2("+/-###Start direction variance", &info->startDirVariance.x, 0.1);

	ImGui::DragFloat("Start speed", &info->startSpeed, 0.1);
	ImGui::SameLine();
	ImGui::DragFloat("+/-###Start speed variance", &info->startSpeedVariance, 0.1);

	ImGui::DragFloat2("Start accel", &info->startAccel.x, 0.01);
	ImGui::SameLine();
	ImGui::DragFloat2("+/-###Start accel variance", &info->startAccelVariance.x, 0.01);

	ImGui::DragFloat2("End accel", &info->endAccel.x, 0.01);
	ImGui::SameLine();
	ImGui::DragFloat2("+/-###End accel variance", &info->endAccelVariance.x, 0.01);

	ImGui::DragFloat("Start scale", &info->startScale, 0.001);
	ImGui::SameLine();
	ImGui::DragFloat("+/-###Start scale variance", &info->startScaleVariance, 0.001);

	ImGui::DragFloat("End scale", &info->endScale, 0.001);
	ImGui::SameLine();
	ImGui::DragFloat("+/-###End scale variance", &info->endScaleVariance, 0.001);

	// @todo
	// float startRotation, startRotationVariance;
	// float startAngularSpeed, startAngularSpeedVariance;
	// float startAngularAccel, startAngularAccelVariance;
	// float endAngularAccel, endAngularAccelVariance;

	ImGui::DragFloat("Lifetime", &info->lifetime, 0.1);
	ImGui::SameLine();
	ImGui::DragFloat("+/-###Lifetime variance", &info->lifetimeVariance, 0.1);

	guiInputArgb("Start color", &info->startColor);
	ImGui::SameLine();
	guiInputArgb("+/-###Start color variance", &info->startColorVariance);

	guiInputArgb("End color", &info->endColor);
	ImGui::SameLine();
	guiInputArgb("+/-###End color variance", &info->endColorVariance);

	ImGui::PopID();
}

void writeEmitterInfo(DataStream *stream, EmitterInfo *info) {
	int version = 1;
	writeU32(stream, version);

	writeString(stream, info->name);

	writeString(stream, info->textureDir);
	writeU32(stream, info->imageCount);
	writeFloat(stream, info->particlesPerSec);

	writeVec2(stream, info->startDir);
	writeVec2(stream, info->startDirVariance);
	writeFloat(stream, info->startSpeed);
	writeFloat(stream, info->startSpeedVariance);

	writeVec2(stream, info->startAccel);
	writeVec2(stream, info->startAccelVariance);
	writeVec2(stream, info->endAccel);
	writeVec2(stream, info->endAccelVariance);

	writeFloat(stream, info->startScale);
	writeFloat(stream, info->startScaleVariance);
	writeFloat(stream, info->endScale);
	writeFloat(stream, info->endScaleVariance);

	writeFloat(stream, info->startRotation);
	writeFloat(stream, info->startRotationVariance);
	writeFloat(stream, info->startAngularSpeed);
	writeFloat(stream, info->startAngularSpeedVariance);
	writeFloat(stream, info->startAngularAccel);
	writeFloat(stream, info->startAngularAccelVariance);
	writeFloat(stream, info->endAngularAccel);
	writeFloat(stream, info->endAngularAccelVariance);

	writeFloat(stream, info->lifetime);
	writeFloat(stream, info->lifetimeVariance);

	writeU32(stream, info->startColor);
	writeU32(stream, info->startColorVariance);
	writeU32(stream, info->endColor);
	writeU32(stream, info->endColorVariance);
}

void readEmitterInfo(DataStream *stream, EmitterInfo *info) {
	int version = readU32(stream);

	readStringInto(stream, info->name, EMITTER_INFO_NAME_MAX_LEN);

	readStringInto(stream, info->textureDir, PATH_MAX_LEN);
	info->imageCount = readU32(stream);
	info->particlesPerSec = readFloat(stream);

	info->startDir = readVec2(stream);
	info->startDirVariance = readVec2(stream);
	info->startSpeed = readFloat(stream);
	info->startSpeedVariance = readFloat(stream);

	info->startAccel = readVec2(stream);
	info->startAccelVariance = readVec2(stream);
	info->endAccel = readVec2(stream);
	info->endAccelVariance = readVec2(stream);

	info->startScale = readFloat(stream);
	info->startScaleVariance = readFloat(stream);
	info->endScale = readFloat(stream);
	info->endScaleVariance = readFloat(stream);

	info->startRotation = readFloat(stream);
	info->startRotationVariance = readFloat(stream);
	info->startAngularSpeed = readFloat(stream);
	info->startAngularSpeedVariance = readFloat(stream);
	info->startAngularAccel = readFloat(stream);
	info->startAngularAccelVariance = readFloat(stream);
	info->endAngularAccel = readFloat(stream);
	info->endAngularAccelVariance = readFloat(stream);

	info->lifetime = readFloat(stream);
	info->lifetimeVariance = readFloat(stream);

	info->startColor = readU32(stream);
	info->startColorVariance = readU32(stream);
	info->endColor = readU32(stream);
	info->endColorVariance = readU32(stream);
}

bool saveLoadEmitterInfo(DataStream *stream, bool save, int version, EmitterInfo *info, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeEmitterInfo(stream, info);
	else readEmitterInfo(stream, info);
	return true;
}
