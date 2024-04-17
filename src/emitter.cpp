struct Particle {
	int imageIndex;

	Vec2 direction;
	Vec2 position;
	Vec2 veloFromAccel;
	Vec2 baseVelo;

	float speedMulti;

	Vec2 accel;
	float accelXMulti;
	bool accelXFlipped;

	float accelYMulti;

	float scale;
	float scaleMulti;

	float rotation;
	float angularSpeedMulti;
	bool angularSpeedFlipped;

	float lifetime;

	int colorStart, colorEnd;
	int color;

	u32 randomValue;
	float delay;
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

struct EmitterFnInfo {
	bool enabled;
	float amplitude;
	float frequency;
};

struct EmitterInfo {
#define EMITTER_INFO_NAME_MAX_LEN 64
	char name[EMITTER_INFO_NAME_MAX_LEN];

	char textureDir[PATH_MAX_LEN];
	bool animated;
	float frameRate; 

	EmitterShape shape;
	float shapeInnerCutPerc;
	float defaultRadius;

	bool explode;
  int defaultExplodeCount;
	float rate;

	Vec2 dir, dirVariance;

	float speedStart, speedStartVariance;
	float speedEnd, speedEndVariance;
	bool animateSpeed;
	CurvePlot speedPlot;
	Gradient speedGradient;

	EmitterFnInfo veloXFn;

	Vec2 accel, accelVariance;
	CurvePlot accelXPlot;
	bool randomlyFlipAccelX;
	Gradient accelXGradient;
	float accelXBase;
	CurvePlot accelYPlot;
	Gradient accelYGradient;
	float accelYBase;

	float scaleStart, scaleStartVariance;
	float scaleEnd, scaleEndVariance;
	bool animateScale;
	EmitterFnInfo scaleFn;
	Gradient scaleGradient;
	CurvePlot scalePlot;

	float rotation, rotationVariance;
	float angularSpeedStart, angularSpeedStartVariance;
	float angularSpeedEnd, angularSpeedEndVariance;
	bool animateAngularSpeed;

	CurvePlot angularSpeedPlot;
	Gradient angularSpeedGradient;
	bool randomlyFlipAngularSpeed;

	float lifetime, lifetimeVariance;
	float delay, delayVariance;

	int colorStart, colorStartVariance;
	int colorEnd, colorEndVariance;
	bool animateColor;
	Gradient colorGradient;

	float fadeInPerc;

	Texture *debugDirectionTexture;
};

struct Emitter {
	EmitterInfo info;
	char prevTextureDir[PATH_MAX_LEN];
  int id;

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

	Vec2 baseVelo;
  bool destroyWhenDone;

  bool reloadInfo;
  float timeSinceReloadedInfo;
};

struct EmitterSystem {
	Vec2 sizeScale;

	int debugExplodeCount;
	bool debugExplodingRepeatedly;
	float debugExplodingRepeatedlyDelay;
	float debugTimeTillNextExplode;
};

int _nextEmitterId = 0;

#define HANGING_EMITTERS_MAX 1024
Emitter *_hangingEmitters[HANGING_EMITTERS_MAX];
int _hangingEmittersNum;

EmitterInfo *_referenceEmitterInfos;
int _referenceEmitterInfosNum;

void checkEmitterSystemInit();
Emitter *createEmitter(char *infoName=NULL);
void updateEmitter(Emitter *emitter, float elapsed);
void emit(Emitter *emitter, int amount=1);
void drawEmitter(Emitter *emitter);
void updateAndDrawEmitter(Emitter *emitter, float elapsed);
void destroy(Emitter *emitter);

EmitterInfo *getEmitterInfo(char *name);
Emitter *createHangingEmitter(char *infoName=NULL);
Emitter *getHangingEmitter(int id);
void updateAndDrawHangingEmitters(float elapsed);

void guiInputEmitter(Emitter *emitter, float elapsed);
void guiInputEmitterInfo(EmitterInfo *info);
void writeEmitterInfo(DataStream *stream, EmitterInfo *info);
void readEmitterInfo(DataStream *stream, EmitterInfo *info);
bool saveLoadEmitterInfo(DataStream *stream, bool save, int version, EmitterInfo *info, int startVersion, int endVersion);
/// FUNCTIONS ^

EmitterSystem *emitterSys = NULL;

void checkEmitterSystemInit() {
	if (emitterSys) return;
	emitterSys = (EmitterSystem *)zalloc(sizeof(EmitterSystem));
	emitterSys->sizeScale = v2(1, 1);
}

Emitter *createEmitter(char *infoName) {
	checkEmitterSystemInit();

	Emitter *emitter = (Emitter *)zalloc(sizeof(Emitter));
	emitter->particlesMax = 128;
	emitter->particles = (Particle *)zalloc(sizeof(Particle) * emitter->particlesMax);
	emitter->id = ++_nextEmitterId;
	emitter->circleRadius = -1;

  if (infoName) {
    EmitterInfo *info = getEmitterInfo(infoName);
    if (info) {
			emitter->info = *info;
			emitter->reloadInfo = true;
		} else {
			logf("Can't find emitter info %s\n", infoName);
		}
  }

	return emitter;
}

void updateEmitter(Emitter *emitter, float elapsed) {
	checkEmitterSystemInit();

	auto applyFn = [](Particle *particle, EmitterFnInfo *fnInfo, float *value) {
		if (fnInfo->enabled) {
			float phaseShift = M_PI/2 + (2*M_PI * rndFloat(0, 1));
			(*value) += sin(particle->time * fnInfo->frequency + phaseShift) * fnInfo->amplitude;
		}
	};

	EmitterInfo *info = &emitter->info;
	if (!info) {
		logf("Updating emitter with no info!\n");
		return;
	}
	if (info->rate < 0.001) info->rate = 0.001;

  if (emitter->reloadInfo) {
#ifdef FALLOW_DEBUG
    emitter->timeSinceReloadedInfo -= elapsed;
#endif

    if (emitter->timeSinceReloadedInfo < 0) {
      emitter->timeSinceReloadedInfo = 0.25;
      EmitterInfo *newInfo = getEmitterInfo(info->name);
      if (newInfo) emitter->info = *newInfo;
    }
  }

	if (!streq(emitter->prevTextureDir, info->textureDir)) {
		strcpy(emitter->prevTextureDir, info->textureDir);
		emitter->sheet = getSpriteSheet(info->textureDir);
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
		if (particle->delay > 0) {
			particle->delay -= elapsed;
			if (particle->delay > 0) continue; // Maybe you can accumulate into particle->time, because t=0 doesn't matter
		}
		pushRndSeed(particle->randomValue);

		float lerpSpeed = 0.1;
		if (particle->time == 0) lerpSpeed = 1;

		float perc = particle->time / particle->lifetime;

		float speed = sampleCurvePlot(info->speedPlot, perc) * particle->speedMulti * 1000;
		Vec2 velo = particle->baseVelo + particle->direction*speed;

		particle->position += (velo + particle->veloFromAccel) * elapsed;

		Vec2 accel = v2();
		accel.x = sampleCurvePlot(info->accelXPlot, perc, particle->accelXFlipped) * particle->accelXMulti * 100;
		accel.y = sampleCurvePlot(info->accelYPlot, perc) * particle->accelYMulti * 100;

		particle->veloFromAccel += accel;

		particle->scale = lerp(particle->scale, sampleCurvePlot(info->scalePlot, perc) * particle->scaleMulti, lerpSpeed);

		float angularSpeed = sampleCurvePlot(info->angularSpeedPlot, perc) * particle->angularSpeedMulti * 10;
		if (particle->angularSpeedFlipped) angularSpeed *= -1;
		particle->rotation += angularSpeed;

		particle->color = sampleGradient(info->colorGradient, perc);

		popRndSeed();

		particle->time += elapsed;
		if (particle->time > particle->lifetime) {
			arraySpliceIndex(emitter->particles, emitter->particlesNum, sizeof(Particle), i);
			emitter->particlesNum--;
			i--;
			continue;
		}
	}
}

void emit(Emitter *emitter, int amount) {
	checkEmitterSystemInit();

	EmitterInfo *info = &emitter->info;

	for (int i = 0; i < amount; i++) {
		if (emitter->particlesNum > emitter->particlesMax-1) {
			emitter->particles = (Particle *)resizeArray(emitter->particles, sizeof(Particle), emitter->particlesMax, emitter->particlesMax*2);
			emitter->particlesMax *= 2;
		}

		Particle *particle = &emitter->particles[emitter->particlesNum++];
		memset(particle, 0, sizeof(Particle));

		if (emitter->sheet) particle->imageIndex = rndInt(0, emitter->sheet->imagesNum-1);

		if (info->shape == EMITTER_SHAPE_POINT) {
			particle->position = emitter->position;
		} else if (info->shape == EMITTER_SHAPE_LINE) {
			float perc = rndFloat(0, 1);
			particle->position.x = lerp(emitter->line.start.x, emitter->line.end.x, perc);
			particle->position.y = lerp(emitter->line.start.y, emitter->line.end.y, perc);
		} else if (info->shape == EMITTER_SHAPE_CIRCLE) {
			float dirPerc = rndFloat(0, 1);
			float distPerc = rndFloat(info->shapeInnerCutPerc, 1);

			float radius = emitter->circleRadius;
			if (radius == -1) radius = info->defaultRadius;

			float rads = dirPerc * 2*M_PI;
			float dist = distPerc * radius;
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

		particle->direction = dir;
		particle->baseVelo = emitter->baseVelo;

		particle->speedMulti = 1 + info->speedStartVariance*rndFloat(-1, 1);

		particle->accel = info->accel + info->accelVariance*rndFloat(-1, 1);
		particle->accelXMulti = 1 + info->accelVariance.x*rndFloat(-1, 1);
		if (info->randomlyFlipAccelX) particle->accelXFlipped = rndPerc(0.5);
		particle->accelYMulti = 1 + info->accelVariance.y*rndFloat(-1, 1);

		particle->scaleMulti = 1 + info->scaleStartVariance*rndFloat(-1, 1);

		particle->rotation = info->rotation + info->rotationVariance*rndFloat(-1, 1);
		particle->angularSpeedMulti = 1 + info->angularSpeedStartVariance*rndFloat(-1, 1);
		if (info->randomlyFlipAngularSpeed) particle->angularSpeedFlipped = rndPerc(0.5);

		particle->lifetime = info->lifetime + info->lifetimeVariance*rndFloat(-1, 1);
		if (particle->lifetime == 0) particle->lifetime = 0.01;
		particle->delay = info->delay + fabs(info->delayVariance*rndFloat(-1, 1));

		{
			int a, r, g, b;
			int va, vr, vg, vb;
			hexToArgb(info->colorStart, &a, &r, &g, &b);
			hexToArgb(info->colorStartVariance, &va, &vr, &vg, &vb);

			a = mathClamp(a + va*rndFloat(-1, 1), 0, 255);
			r = mathClamp(r + vr*rndFloat(-1, 1), 0, 255);
			g = mathClamp(g + vg*rndFloat(-1, 1), 0, 255);
			b = mathClamp(b + vb*rndFloat(-1, 1), 0, 255);

			particle->colorStart = argbToHex(a, r, g, b);
		}

		{
			int a, r, g, b;
			int va, vr, vg, vb;
			hexToArgb(info->colorEnd, &a, &r, &g, &b);
			hexToArgb(info->colorEndVariance, &va, &vr, &vg, &vb);

			a = mathClamp(a + va*rndFloat(-1, 1), 0, 255);
			r = mathClamp(r + vr*rndFloat(-1, 1), 0, 255);
			g = mathClamp(g + vg*rndFloat(-1, 1), 0, 255);
			b = mathClamp(b + vb*rndFloat(-1, 1), 0, 255);

			particle->colorEnd = argbToHex(a, r, g, b);
		}

		particle->randomValue = rnd();
	}

	return;
}

void drawEmitter(Emitter *emitter) {
	EmitterInfo *info = &emitter->info;
	if (!info) {
		logf("Drawing emitter with no info!\n");
		return;
	}

	for (int i = 0; i < emitter->particlesNum; i++) {
		Particle *particle = &emitter->particles[i];
		if (particle->delay > 0) continue;

		Vec2 scale = v2(particle->scale, particle->scale);

		if (emitter->sheet) {
			SpriteSheet *sheet = emitter->sheet;
			if (info->animated) particle->imageIndex = (int)(particle->time * info->frameRate) % sheet->imagesNum;

			SpriteSheetImage *image = &sheet->images[particle->imageIndex];

			RenderProps props = newRenderProps();
			props.srcWidth = props.srcHeight = 1;

			Vec2 position = particle->position + v2(image->destOffX, image->destOffY);
			Vec2 textureSize = v2(image->srcWidth, image->srcHeight);

			position *= emitterSys->sizeScale;
			textureSize *= emitterSys->sizeScale;

			props.matrix.TRANSLATE(position - textureSize/2);

			props.matrix.TRANSLATE(textureSize/2);
			props.matrix.SCALE(scale);
			props.matrix.ROTATE(particle->rotation);
			props.matrix.TRANSLATE(-textureSize/2);

			props.matrix.SCALE(textureSize);

			props.uv0.x = (float)image->srcX / (float)sheet->texture->width;
			props.uv0.y = (float)image->srcY / (float)sheet->texture->height;
			props.uv1.x = (float)(image->srcX+image->srcWidth) / (float)sheet->texture->width;
			props.uv1.y = (float)(image->srcY+image->srcHeight) / (float)sheet->texture->height;

			props.tint = particle->color;

			drawTexture(emitter->sheet->texture, props);
		} else {
			Circle circle = makeCircle(particle->position, 32*scale.x);
			circle.position *= emitterSys->sizeScale;
			circle.radius *= emitterSys->sizeScale.x;
			drawCircle(circle, particle->color);
		}
	}
}

void updateAndDrawEmitter(Emitter *emitter, float elapsed) {
	updateEmitter(emitter, elapsed);
	drawEmitter(emitter);
}

void destroy(Emitter *emitter) {
	free(emitter->particles);

  for (int i = 0; i < _hangingEmittersNum; i++) {
    if (emitter->id == _hangingEmitters[i]->id) {
      arraySpliceIndex(_hangingEmitters, _hangingEmittersNum, sizeof(Emitter *), i);
      _hangingEmittersNum--;
      break;
    }
  }

	free(emitter);
}

EmitterInfo *getEmitterInfo(char *name) {
	checkEmitterSystemInit();

	if (!name[0]) return NULL;

  for (int i = 0; i < _referenceEmitterInfosNum; i++) {
    EmitterInfo *info = &_referenceEmitterInfos[i];
    if (streq(info->name, name)) return info;
  }

	return NULL;
}

Emitter *createHangingEmitter(char *infoName) {
	checkEmitterSystemInit();

	if (!getEmitterInfo(infoName)) {
		logf("Can't find emitter info %s\n", infoName);
		return NULL;
	}

  if (_hangingEmittersNum > HANGING_EMITTERS_MAX-1) {
    Emitter *emitter = _hangingEmitters[_hangingEmittersNum-1];
    emitter->enabled = false;
    emitter->destroyWhenDone = true;
    _hangingEmittersNum--;
  }

  Emitter *emitter = createEmitter(infoName);
  _hangingEmitters[_hangingEmittersNum++] = emitter;

  return emitter;
}

Emitter *getHangingEmitter(int id) {
	checkEmitterSystemInit();

	if (id == 0) return NULL;

	for (int i = 0; i < _hangingEmittersNum; i++) {
    Emitter *emitter = _hangingEmitters[i];
    if (emitter->id == id) return emitter;
	}

	return NULL;
}

void updateAndDrawHangingEmitters(float elapsed) {
	checkEmitterSystemInit();

  for (int i = 0; i < _hangingEmittersNum; i++) {
    Emitter *emitter = _hangingEmitters[i];
    updateAndDrawEmitter(emitter, elapsed);
    if (emitter->particlesNum == 0 && emitter->destroyWhenDone) {
      destroy(emitter);
      i--;
      continue;
    }
  }
}

void guiInputEmitter(Emitter *emitter, float elapsed) {
	checkEmitterSystemInit();

	ImGui::PushID(emitter);

	ImGui::Begin("Emitter", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Size scale: %.2f,%.2f", emitterSys->sizeScale.x, emitterSys->sizeScale.y);
	ImGui::Text("Particles: %d/%d", emitter->particlesNum, emitter->particlesMax);

	EmitterInfo *info = &emitter->info;
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

	if (info->explode) {
    int explodeCount = emitterSys->debugExplodeCount;
    if (explodeCount == 0) explodeCount = info->defaultExplodeCount;

		ImGui::InputInt("Explode count", &emitterSys->debugExplodeCount);
		ImGui::SameLine();
		if (ImGui::Button("Explode")) {
			for (int i = 0; i < explodeCount; i++) emit(emitter);
		}

		ImGui::Checkbox("Explode repeatedly", &emitterSys->debugExplodingRepeatedly);
		if (emitterSys->debugExplodingRepeatedly) {
			ImGui::SameLine();
			if (emitterSys->debugExplodingRepeatedlyDelay == 0) emitterSys->debugExplodingRepeatedlyDelay = 2;
			ImGui::DragFloat("Delay", &emitterSys->debugExplodingRepeatedlyDelay, 0.1);
			if (emitterSys->debugExplodingRepeatedlyDelay < 0.1) emitterSys->debugExplodingRepeatedlyDelay = 0.1;

			emitterSys->debugTimeTillNextExplode -= elapsed;
			if (emitterSys->debugTimeTillNextExplode < 0) {
				emitterSys->debugTimeTillNextExplode = emitterSys->debugExplodingRepeatedlyDelay;

				for (int i = 0; i < explodeCount; i++) emit(emitter);
			}
		}
	}

	ImGui::End();

	ImGui::PopID();
}

void guiInputEmitterInfo(EmitterInfo *info) {
	checkEmitterSystemInit();

	ImGui::PushID(info);

	ImGui::InputText("Name", info->name, EMITTER_INFO_NAME_MAX_LEN);
	ImGui::InputText("Texture path", info->textureDir, PATH_MAX_LEN);
	ImGui::Checkbox("Animated", &info->animated);
	if (info->animated) ImGui::DragFloat("Frame rate", &info->frameRate, 0.1);
	ImGui::Separator();

	ImGui::Combo("Shape", (int *)&info->shape, emitterShapeStrings, ArrayLength(emitterShapeStrings));
	ImGui::SliderFloat("Shape inner cut perc", &info->shapeInnerCutPerc, 0, 1);

	if (info->shape == EMITTER_SHAPE_CIRCLE) {
		ImGui::DragFloat("Default radius", &info->defaultRadius);
	}
	ImGui::Separator();

	ImGui::Checkbox("Explode", &info->explode);

  if (info->explode) {
    ImGui::InputInt("Default explode count", &info->defaultExplodeCount);
  } else {
    ImGui::DragFloat("Rate", &info->rate, 0.0001, 0.001, 3, "%.4f");
  }

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

	setVec2WithVariance("Direction", &info->dir, &info->dirVariance, 0.001);

	ImGui::SameLine();
	{
		if (!info->debugDirectionTexture) info->debugDirectionTexture = createTexture(75, 75);
		Texture *texture = info->debugDirectionTexture;

		pushTargetTexture(texture);
		clearRenderer();
		Circle circle = makeCircle(getSize(texture)/2, texture->width/2);
		drawCircle(circle, 0xFF800000);

		Line2 line;
		line.start = circle.position;
		line.end = line.start + normalize(info->dir)*circle.radius;
		drawLine(line, 4, 0xFF0000FF);

		Line2 varStartLine;
		varStartLine.start = circle.position;
		varStartLine.end = varStartLine.start + normalize(info->dir - info->dirVariance)*circle.radius;
		drawLine(varStartLine, 4, 0xFFFFFF00);

		Line2 varEndLine;
		varEndLine.start = circle.position;
		varEndLine.end = varEndLine.start + normalize(info->dir + info->dirVariance)*circle.radius;
		drawLine(varEndLine, 4, 0xFFFFFF00);

		popTargetTexture();
		imGuiTexture(info->debugDirectionTexture);
	}


	guiInputCurvePlot("Speed", &info->speedPlot);
	ImGui::DragFloat("Speed variance", &info->speedStartVariance, 0.001);
	ImGui::Separator();

	guiInputCurvePlot("AccelX", &info->accelXPlot);
	ImGui::Checkbox("Randomly flip AccelX", &info->randomlyFlipAccelX);
	ImGui::DragFloat("AccelX variance", &info->accelVariance.x, 0.001);
	ImGui::Separator();

	guiInputCurvePlot("AccelY", &info->accelYPlot);
	ImGui::DragFloat("AccelY variance", &info->accelVariance.y, 0.001);
	ImGui::Separator();

	// guiInputGradient("Scale", &info->scaleGradient);
	guiInputCurvePlot("Scale", &info->scalePlot);
	ImGui::DragFloat("Scale variance", &info->scaleStartVariance, 0.001);
	ImGui::Separator();

	ImGui::Separator();
	setFloatWithVariance("Rotation", &info->rotation, &info->rotationVariance, 0.1);
	guiInputCurvePlot("Angular speed", &info->angularSpeedPlot);
	ImGui::DragFloat("Angular speed variance", &info->angularSpeedStartVariance, 0.001);
	ImGui::Checkbox("Randomly flip angular speed", &info->randomlyFlipAngularSpeed);
	ImGui::Separator();

	setFloatWithVariance("Lifetime", &info->lifetime, &info->lifetimeVariance, 0.01);
  if (info->explode) {
    setFloatWithVariance("Delay", &info->delay, &info->delayVariance, 0.01);
  } else {
    info->delay = 0;
  }

	guiInputGradient("Color", &info->colorGradient);

	ImGui::PopID();
}

void writeGradient(DataStream *stream, Gradient gradient);
void writeGradient(DataStream *stream, Gradient gradient) {
	writeU32(stream, gradient.colorMarkersNum);
	for (int i = 0; i < gradient.colorMarkersNum; i++) {
		GradientColorMarker *colorMarker = &gradient.colorMarkers[i];
		writeFloat(stream, colorMarker->position);
		writeVec3(stream, colorMarker->color);
		writeFloat(stream, colorMarker->intensity);
	}

	writeU32(stream, gradient.alphaMarkersNum);
	for (int i = 0; i < gradient.alphaMarkersNum; i++) {
		GradientAlphaMarker *alphaMarker = &gradient.alphaMarkers[i];
		writeFloat(stream, alphaMarker->position);
		writeFloat(stream, alphaMarker->alpha);
	}
}

void writeCurvePlot(DataStream *stream, CurvePlot plot);
void writeCurvePlot(DataStream *stream, CurvePlot plot) {
	writeU32(stream, plot.pointsNum);
	for (int i = 0; i < plot.pointsNum; i++) {
		writeVec2(stream, plot.points[i]);
	}

	writeFloat(stream, plot.min);
	writeFloat(stream, plot.max);
}

CurvePlot readCurvePlot(DataStream *stream, int version);
CurvePlot readCurvePlot(DataStream *stream, int version) {
	CurvePlot plot = {};
	plot.pointsNum = readU32(stream);
	for (int i = 0; i < plot.pointsNum; i++) {
		plot.points[i] = readVec2(stream);
	}

	if (version >= 16) {
		plot.min = readFloat(stream);
		plot.max = readFloat(stream);
	}

	plot.imSelected = -1;

	return plot;
}

void writeEmitterInfo(DataStream *stream, EmitterInfo *info) {
	auto writeFn = [](DataStream *stream, EmitterFnInfo *fnInfo) {
		writeU8(stream, fnInfo->enabled);
		writeFloat(stream, fnInfo->amplitude);
		writeFloat(stream, fnInfo->frequency);
	};

	int version = 18;
	writeU32(stream, version);

	writeString(stream, info->name);

	writeString(stream, info->textureDir);
	writeU8(stream, info->animated);
	writeFloat(stream, info->frameRate);

	writeU32(stream, (int)info->shape);
	writeFloat(stream, info->shapeInnerCutPerc);
	writeFloat(stream, info->defaultRadius);
	writeU8(stream, info->explode);

	writeU32(stream, info->defaultExplodeCount);
	writeFloat(stream, info->rate);

	writeVec2(stream, info->dir);
	writeVec2(stream, info->dirVariance);

	writeFloat(stream, info->speedStart);
	writeFloat(stream, info->speedStartVariance);
	writeFloat(stream, info->speedEnd);
	writeFloat(stream, info->speedEndVariance);
	writeU8(stream, info->animateSpeed);
	writeCurvePlot(stream, info->speedPlot);
	writeGradient(stream, info->speedGradient);

	writeFn(stream, &info->veloXFn);

	writeVec2(stream, info->accel);
	writeVec2(stream, info->accelVariance);
	writeCurvePlot(stream, info->accelXPlot);
	writeU8(stream, info->randomlyFlipAccelX);
	writeGradient(stream, info->accelXGradient);
	writeCurvePlot(stream, info->accelYPlot);
	writeGradient(stream, info->accelYGradient);

	writeFloat(stream, info->scaleStart);
	writeFloat(stream, info->scaleStartVariance);
	writeFloat(stream, info->scaleEnd);
	writeFloat(stream, info->scaleEndVariance);
	writeU8(stream, info->animateScale);
	writeFn(stream, &info->scaleFn);
	writeGradient(stream, info->scaleGradient);
	writeCurvePlot(stream, info->scalePlot);

	writeFloat(stream, info->rotation);
	writeFloat(stream, info->rotationVariance);
	writeFloat(stream, info->angularSpeedStart);
	writeFloat(stream, info->angularSpeedStartVariance);
	writeFloat(stream, info->angularSpeedEnd);
	writeFloat(stream, info->angularSpeedEndVariance);
	writeU8(stream, info->animateAngularSpeed);
	writeCurvePlot(stream, info->angularSpeedPlot);
	writeGradient(stream, info->angularSpeedGradient);
	writeU8(stream, info->randomlyFlipAngularSpeed);

	writeFloat(stream, info->lifetime);
	writeFloat(stream, info->lifetimeVariance);

	writeFloat(stream, info->delay);
	writeFloat(stream, info->delayVariance);

	writeU32(stream, info->colorStart);
	writeU32(stream, info->colorStartVariance);
	writeU32(stream, info->colorEnd);
	writeU32(stream, info->colorEndVariance);
	writeU8(stream, info->animateColor);
	writeGradient(stream, info->colorGradient);

	writeFloat(stream, info->fadeInPerc);
}

Gradient readGradient(DataStream *stream);
Gradient readGradient(DataStream *stream) {
	Gradient gradient = {};
	gradient.colorMarkersNum = readU32(stream);
	for (int i = 0; i < gradient.colorMarkersNum; i++) {
		GradientColorMarker *colorMarker = &gradient.colorMarkers[i];
		colorMarker->position = readFloat(stream);
		colorMarker->color = readVec3(stream);
		colorMarker->intensity = readFloat(stream);
	}

	gradient.alphaMarkersNum = readU32(stream);
	for (int i = 0; i < gradient.alphaMarkersNum; i++) {
		GradientAlphaMarker *alphaMarker = &gradient.alphaMarkers[i];
		alphaMarker->position = readFloat(stream);
		alphaMarker->alpha = readFloat(stream);
	}
	return gradient;
}
void readEmitterInfo(DataStream *stream, EmitterInfo *info) {
	info->animateScale = true;
	info->animateSpeed = true;
	info->animateAngularSpeed = true;
	info->animateColor = true;

	auto readFn = [](DataStream *stream, EmitterFnInfo *fnInfo, int version) {
		fnInfo->enabled = readU8(stream);
		fnInfo->amplitude = readFloat(stream);
		fnInfo->frequency = readFloat(stream);
	};

	int version = readU32(stream);

	readStringInto(stream, info->name, EMITTER_INFO_NAME_MAX_LEN);

  readStringInto(stream, info->textureDir, PATH_MAX_LEN);
  info->animated = readU8(stream);
  info->frameRate = readFloat(stream);
  info->shape = (EmitterShape)readU32(stream);
  info->shapeInnerCutPerc = readFloat(stream);
  if (version >= 13) info->defaultRadius = readFloat(stream);

  info->explode = readU8(stream);
  if (version >= 9) info->defaultExplodeCount = readU32(stream);
	info->rate = readFloat(stream);

	info->dir = readVec2(stream);
	info->dirVariance = readVec2(stream);

	info->speedStart = readFloat(stream);
	info->speedStartVariance = readFloat(stream);
	if (version >= 2) {
		info->speedEnd = readFloat(stream);
		info->speedEndVariance = readFloat(stream);
	}
	if (version >= 4) info->animateSpeed = readU8(stream);
	if (version >= 15) info->speedPlot = readCurvePlot(stream, version);
	if (version >= 11) info->speedGradient = readGradient(stream);

	if (version >= 6) readFn(stream, &info->veloXFn, version);

	info->accel = readVec2(stream);
	info->accelVariance = readVec2(stream);
	if (version >= 15) info->accelXPlot = readCurvePlot(stream, version);
	if (version >= 17) info->randomlyFlipAccelX = readU8(stream);
	if (version >= 12) info->accelXGradient = readGradient(stream);
	if (version >= 15) info->accelYPlot = readCurvePlot(stream, version);
	if (version >= 12) info->accelYGradient = readGradient(stream);

	info->scaleStart = readFloat(stream);
	info->scaleStartVariance = readFloat(stream);
	info->scaleEnd = readFloat(stream);
	info->scaleEndVariance = readFloat(stream);
	if (version >= 4) info->animateScale = readU8(stream);
	if (version >= 7) readFn(stream, &info->scaleFn, version);
	if (version >= 11) info->scaleGradient = readGradient(stream);
	if (version >= 14) info->scalePlot = readCurvePlot(stream, version);

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
	if (version >= 4) info->animateAngularSpeed = readU8(stream);
	if (version >= 15) info->angularSpeedPlot = readCurvePlot(stream, version);
	if (version >= 11) info->angularSpeedGradient = readGradient(stream);
	if (version >= 18) info->randomlyFlipAngularSpeed = readU8(stream);

	info->lifetime = readFloat(stream);
	info->lifetimeVariance = readFloat(stream);

	if (version >= 5) {
		info->delay = readFloat(stream);
		info->delayVariance = readFloat(stream);
	}

	info->colorStart = readU32(stream);
	info->colorStartVariance = readU32(stream);
	info->colorEnd = readU32(stream);
	info->colorEndVariance = readU32(stream);
	if (version >= 4) info->animateColor = readU8(stream);

	if (version >= 10) info->colorGradient = readGradient(stream);

	if (version >= 8) info->fadeInPerc = readFloat(stream);
}

bool saveLoadEmitterInfo(DataStream *stream, bool save, int version, EmitterInfo *info, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeEmitterInfo(stream, info);
	else readEmitterInfo(stream, info);
	return true;
}
