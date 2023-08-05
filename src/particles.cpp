struct Particle {
	int imageIndex;

	Vec2 position;
	Vec2 veloStart, veloEnd;
	Vec2 veloFromAccel;

	Vec2 accel;

	float scaleStart, scaleEnd;

	float rotation;
	float angularSpeedStart, angularSpeedEnd;

	float lifetime;

	int startColor, endColor;

	float time;
};

enum EmitterShape {
	EMITTER_SHAPE_POINT,
	EMITTER_SHAPE_LINE,
	EMITTER_SHAPE_CIRCLE,
	EMITTER_SHAPE_RECT,
};
char *emitterShapeStrings[] = {
	"Point",
	"Line",
	"Circle",
	"Rect",
};

struct EmitterInfo {
#define EMITTER_INFO_NAME_MAX_LEN 64
	char name[EMITTER_INFO_NAME_MAX_LEN];

	char textureDir[PATH_MAX_LEN];
	bool animated;
	float frameRate; 

	EmitterShape shape;
	float shapeInnerCutPerc;
	bool explode;
	float rate;

	Vec2 dir, dirVariance;

	float speedStart, speedStartVariance;
	float speedEnd, speedEndVariance;

	Vec2 accel, accelVariance;

	float scaleStart, scaleStartVariance;
	float scaleEnd, scaleEndVariance;

	float rotation, rotationVariance;
	float angularSpeedStart, angularSpeedStartVariance;
	float angularSpeedEnd, angularSpeedEndVariance;

	float lifetime, lifetimeVariance;

	int startColor, startColorVariance;
	int endColor, endColorVariance;
};

struct Emitter {
	EmitterInfo *info;
	char prevTextureDir[PATH_MAX_LEN];

	Vec2 position;
	Line2 line;
	Rect rect;
	float circleRadius;

	bool enabled;
	SpriteSheet *sheet;

	float timeSinceLastEmit;

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

	if (!info->explode) {
		if (emitter->enabled) emitter->timeSinceLastEmit += elapsed;

		if (!info->explode) {
			while (emitter->timeSinceLastEmit > info->rate) {
				emitter->timeSinceLastEmit -= info->rate;
				emit(emitter);
			}
		}
	}

	for (int i = 0; i < emitter->particlesNum; i++) {
		Particle *particle = &emitter->particles[i];
		float perc = particle->time / particle->lifetime;

		Vec2 velo = lerp(particle->veloStart, particle->veloEnd, perc);
		float angularSpeed = lerp(particle->angularSpeedStart, particle->angularSpeedEnd, perc);

		particle->position += (velo + particle->veloFromAccel) * elapsed;
		particle->veloFromAccel += particle->accel;

		particle->rotation += angularSpeed;

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

	if (info->shape == EMITTER_SHAPE_POINT) {
		particle->position = emitter->position;
	} else if (info->shape == EMITTER_SHAPE_LINE) {
		float perc = rndFloat(0, 1);
		particle->position.x = lerp(emitter->line.start.x, emitter->line.end.x, perc);
		particle->position.y = lerp(emitter->line.start.y, emitter->line.end.y, perc);
	} else if (info->shape == EMITTER_SHAPE_CIRCLE) {
		float dirPerc = rndFloat(0, 1);
		float distPerc = rndFloat(info->shapeInnerCutPerc, 1);

		float rads = dirPerc * 2*M_PI;
		float dist = distPerc * emitter->circleRadius;
		particle->position = emitter->position;
		particle->position.x += cos(rads) * dist;
		particle->position.y += sin(rads) * dist;
	} else if (info->shape == EMITTER_SHAPE_RECT) {
		float percX;
		float percY;
		if (rndPerc(0.5)) {
			percX = rndFloat(info->shapeInnerCutPerc, 1);
			percY = rndFloat(0, 1);
		} else {
			percX = rndFloat(0, 1);
			percY = rndFloat(info->shapeInnerCutPerc, 1);
		}

		if (rndPerc(0.5)) percX *= -1;
		if (rndPerc(0.5)) percY *= -1;

		particle->position = getCenter(emitter->rect);
		particle->position.x += percX * emitter->rect.width/2;
		particle->position.y += percY * emitter->rect.height/2;
	}

	Vec2 dir;
	dir.x = info->dir.x + info->dirVariance.x*rndFloat(-1, 1);
	dir.y = info->dir.y + info->dirVariance.y*rndFloat(-1, 1);
	dir = normalize(dir);

	float veloStart = info->speedStart + info->speedStartVariance*rndFloat(-1, 1);
	float veloEnd = info->speedEnd + info->speedEndVariance*rndFloat(-1, 1);

	particle->veloStart = dir * veloStart;
	particle->veloEnd = dir * veloEnd;

	particle->accel = info->accel + info->accelVariance*rndFloat(-1, 1);

	particle->scaleStart = info->scaleStart + info->scaleStartVariance*rndFloat(-1, 1);
	particle->scaleEnd = info->scaleEnd + info->scaleEndVariance*rndFloat(-1, 1);

	particle->rotation = info->rotation + info->rotationVariance*rndFloat(-1, 1);
	particle->angularSpeedStart = info->angularSpeedStart + info->angularSpeedStartVariance*rndFloat(-1, 1);
	particle->angularSpeedEnd = info->angularSpeedEnd + info->angularSpeedEndVariance*rndFloat(-1, 1);

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
	EmitterInfo *info = emitter->info;
	if (!info) {
		logf("Drawing emitter with no info!\n");
		return;
	}

	for (int i = 0; i < emitter->particlesNum; i++) {
		Particle *particle = &emitter->particles[i];
		float perc = particle->time / particle->lifetime;

		int color = lerpColor(particle->startColor, particle->endColor, perc);
		float scale = lerp(particle->scaleStart, particle->scaleEnd, perc); //@incomplete

		if (emitter->sheet) {
			SpriteSheet *sheet = emitter->sheet;
			if (info->animated) particle->imageIndex = (int)(particle->time * info->frameRate) % sheet->imagesNum;

			SpriteSheetImage *image = &sheet->images[particle->imageIndex];

			RenderProps props = newRenderProps();
			props.srcWidth = props.srcHeight = 1;
			Vec2 textureSize = v2(image->srcWidth, image->srcHeight);
			props.matrix.TRANSLATE(particle->position + v2(image->destOffX, image->destOffY));

			props.matrix.TRANSLATE(textureSize/2);
			props.matrix.SCALE(scale);
			props.matrix.ROTATE(particle->rotation);
			props.matrix.TRANSLATE(-textureSize/2);

			props.matrix.SCALE(textureSize);

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

	EmitterInfo *info = emitter->info;
	if (info) {
		if (info->shape == EMITTER_SHAPE_POINT) {
			ImGui::DragFloat2("Position", &emitter->position.x);
		} else if (info->shape == EMITTER_SHAPE_LINE) {
			ImGui::DragFloat2("Line start", &emitter->line.start.x);
			ImGui::DragFloat2("Line end", &emitter->line.end.x);
		} else if (info->shape == EMITTER_SHAPE_CIRCLE) {
			ImGui::DragFloat2("Position", &emitter->position.x);
			ImGui::DragFloat("Radius", &emitter->circleRadius);
		} else if (info->shape == EMITTER_SHAPE_RECT) {
			ImGui::DragFloat4("Rect", &emitter->rect.x);
		}
	}

	ImGui::End();

	ImGui::PopID();
}

void guiInputEmitterInfo(EmitterInfo *info) {
	ImGui::PushID(info);

	ImGui::InputText("Name", info->name, EMITTER_INFO_NAME_MAX_LEN);
	ImGui::InputText("Texture path", info->textureDir, PATH_MAX_LEN);
	ImGui::Checkbox("Animated", &info->animated);
	if (info->animated) ImGui::DragFloat("Frame rate", &info->frameRate, 0.1);

	ImGui::Combo("Shape", (int *)&info->shape, emitterShapeStrings, ArrayLength(emitterShapeStrings));
	ImGui::SliderFloat("Shape inner cut perc", &info->shapeInnerCutPerc, 0, 1);
	ImGui::Checkbox("Explode", &info->explode);

	auto setFloatWithVariance = [](char *label, float *value, float *variance, float speed) {
		ImGui::Text("%s", label);
		ImGui::SameLine();
		ImGui::DragFloat(frameSprintf("###%s base", label), value, speed);
		ImGui::SameLine();
		ImGui::Text("+/-");
		ImGui::SameLine();
		ImGui::DragFloat(frameSprintf("###%s variance", label), variance, speed);
	};

	auto setVec2WithVariance = [](char *label, Vec2 *value, Vec2 *variance, float speed) {
		ImGui::Text("%s", label);
		ImGui::SameLine();
		ImGui::DragFloat2(frameSprintf("###%s base", label), &value->x, speed);
		ImGui::SameLine();
		ImGui::Text("+/-");
		ImGui::SameLine();
		ImGui::DragFloat2(frameSprintf("###%s variance", label), &variance->x, speed);
	};

	ImGui::DragFloat("Rate", &info->rate, 0.001);
	if (info->rate < 0.001) info->rate = 0.001;

	setVec2WithVariance("Direction", &info->dir, &info->dirVariance, 0.1);
	setFloatWithVariance("Speed start", &info->speedStart, &info->speedStartVariance, 0.1);
	setFloatWithVariance("Speed end", &info->speedEnd, &info->speedEndVariance, 0.1);

	setVec2WithVariance("Accel", &info->accel, &info->accelVariance, 0.01);

	setFloatWithVariance("Scale start", &info->scaleStart, &info->scaleStartVariance, 0.01);
	setFloatWithVariance("Scale end", &info->scaleEnd, &info->scaleEndVariance, 0.01);

	setFloatWithVariance("Rotation", &info->rotation, &info->rotationVariance, 0.1);
	setFloatWithVariance("Angular speed start", &info->angularSpeedStart, &info->angularSpeedStartVariance, 0.01);
	setFloatWithVariance("Angular speed end", &info->angularSpeedEnd, &info->angularSpeedEndVariance, 0.01);

	setFloatWithVariance("Lifetime", &info->lifetime, &info->lifetimeVariance, 0.1);

	guiInputArgb("Start color", &info->startColor);
	ImGui::SameLine();
	guiInputArgb("+/-###Start color variance", &info->startColorVariance);

	guiInputArgb("End color", &info->endColor);
	ImGui::SameLine();
	guiInputArgb("+/-###End color variance", &info->endColorVariance);

	ImGui::PopID();
}

void writeEmitterInfo(DataStream *stream, EmitterInfo *info) {
	int version = 3;
	writeU32(stream, version);

	writeString(stream, info->name);

	writeString(stream, info->textureDir);
	writeU8(stream, info->animated);
	writeFloat(stream, info->frameRate);

	writeU32(stream, (int)info->shape);
	writeFloat(stream, info->shapeInnerCutPerc);
	writeU8(stream, info->explode);

	writeFloat(stream, info->rate);

	writeVec2(stream, info->dir);
	writeVec2(stream, info->dirVariance);

	writeFloat(stream, info->speedStart);
	writeFloat(stream, info->speedStartVariance);
	writeFloat(stream, info->speedEnd);
	writeFloat(stream, info->speedEndVariance);

	writeVec2(stream, info->accel);
	writeVec2(stream, info->accelVariance);

	writeFloat(stream, info->scaleStart);
	writeFloat(stream, info->scaleStartVariance);
	writeFloat(stream, info->scaleEnd);
	writeFloat(stream, info->scaleEndVariance);

	writeFloat(stream, info->rotation);
	writeFloat(stream, info->rotationVariance);
	writeFloat(stream, info->angularSpeedStart);
	writeFloat(stream, info->angularSpeedStartVariance);
	writeFloat(stream, info->angularSpeedEnd);
	writeFloat(stream, info->angularSpeedEndVariance);

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
	if (version >= 3) {
		info->animated = readU8(stream);
		info->frameRate = readFloat(stream);
		info->shape = (EmitterShape)readU32(stream);
		info->shapeInnerCutPerc = readFloat(stream);
		info->explode = readU8(stream);
	} else {
		readU32(stream);
	}
	info->rate = readFloat(stream);

	info->dir = readVec2(stream);
	info->dirVariance = readVec2(stream);

	info->speedStart = readFloat(stream);
	info->speedStartVariance = readFloat(stream);
	if (version >= 2) {
		info->speedEnd = readFloat(stream);
		info->speedEndVariance = readFloat(stream);
	}

	info->accel = readVec2(stream);
	info->accelVariance = readVec2(stream);
	if (version <= 1) {
		readVec2(stream);
		readVec2(stream);
	}

	info->scaleStart = readFloat(stream);
	info->scaleStartVariance = readFloat(stream);
	info->scaleEnd = readFloat(stream);
	info->scaleEndVariance = readFloat(stream);

	info->rotation = readFloat(stream);
	info->rotationVariance = readFloat(stream);
	info->angularSpeedStart = readFloat(stream);
	info->angularSpeedStartVariance = readFloat(stream);
	info->angularSpeedEnd = readFloat(stream);
	info->angularSpeedEndVariance = readFloat(stream);
	if (version <= 1) {
		readFloat(stream);
		readFloat(stream);
	}

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
