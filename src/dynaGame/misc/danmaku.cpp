#ifndef __DYNA_IMPL
enum DanmakuGraphic {
	DELAY_RECT,
	BALL_S,
	BALL_M,
	BEAM,
	BUTTERFLY,
	KOUMA,
	KUNAI,
	MISSILE,
	NEEDLE,
	RICE_S,
	RICE_M,
	SCALE,
	SHAKU,
	STAR_S,
	STAR_M,
	TENGU,
	TWIN,
	YINYANG_L,
	YOUMU,
	DANMAKU_GRAPHIC_END,
};
char *danmakuGraphicStrings[] = {
	"DELAY_RECT",
	"BALL_S",
	"BALL_M",
	"BEAM",
	"BUTTERFLY",
	"KOUMA",
	"KUNAI",
	"MISSILE",
	"NEEDLE",
	"RICE_S",
	"RICE_M",
	"SCALE",
	"SHAKU",
	"STAR_S",
	"STAR_M",
	"TENGU",
	"TWIN",
	"YINYANG_L",
	"YOUMU",
};
struct DanmakuGraphicInfo {
	char *name;
	char *path;
	Vec2 size; //Does this really need size?
	float radius;
	float bulletToBulletRadius;
	float textureRadius;
};
enum ColorName {
	RED=0,
	ORANGE=1,
	YELLOW=2,
	LIME=3,
	GREEN=4,
	TEAL=5,
	CYAN=6,
	BLUE=7,
	INDIGO=8,
	PURPLE=9,
	VIOLET=10,
	MAGENTA=11,
};
char *colorNamesStrings[] = {
	"Red", 
	"Orange", 
	"Yellow", 
	"Lime", 
	"Green", 
	"Teal", 
	"Cyan", 
	"Blue", 
	"Indigo", 
	"Purple", 
	"Violet", 
	"Magenta", 
};

struct DanmakuModifier {
	float speedUpdate;      bool speedAbsolute;
	float speedAccelUpdate; bool speedAccelAbsolute;
	float angleUpdate;      bool angleAbsolute;
	float angleAccelUpdate; bool angleAccelAbsolute;
	float spinUpdate;       bool spinAbsolute;
	float spinAccelUpdate;  bool spinAccelAbsolute;
	float scaleUpdate;      bool scaleAbsolute;
	float scaleAccelUpdate; bool scaleAccelAbsolute;
	float alphaUpdate;      bool alphaAbsolute;
	float alphaAccelUpdate; bool alphaAccelAbsolute;
	int frame;
	bool die;
	bool aimed;//This should probably override angleAbsolute;
	bool homing;//This one is also weird;
	bool repeat;
};

DanmakuModifier deathDanmakuModifier;
DanmakuModifier freezeDanmakuModifier;
DanmakuModifier aimedDanmakuModifier;
DanmakuModifier homingDanmakuModifier;

#define _F_DF_INVIS     (1 << 0)
#define _F_DF_BLEND_ADD (1 << 1)
struct Danmaku {
	int id;
	int flags;

#define DANMAKU_MODIFIERS_MAX 16
	DanmakuModifier modifiers[DANMAKU_MODIFIERS_MAX];
	int modifiersNum;

	Vec2 position; Vec2 positionOffset;
	float speed;   float speedAccel;
	float angle;   float angleAccel;
	float spin;    float spinAccel;
	float scale;   float scaleAccel;
	float alpha;   float alphaAccel;
	int health;
	int frame;
	DanmakuGraphic graphic; ColorName color;
	bool grazable; // Will go?
	char difficulty; // Will go?
};

DanmakuModifier newDanmakuModifier();
void danmakuInit();
Danmaku *createShotA(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color, int delay);
Danmaku *createShotB(Vec2 pos, Vec2 posDest, int frameTime, DanmakuGraphic graphic, ColorName color, int delay);
Danmaku *createShotS(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color); // Bomb
Danmaku *createShotU(Vec2 pos, float speed, float angle, int difficulty);
Danmaku *createShotZ(Vec2 pos, Vec2 speed, Vec2 angle, Vec2 spin, Vec2 scale, DanmakuGraphic graphic, ColorName color, bool ally, int delay, bool graze);
Danmaku *getDanmaku(int id);
void addModifier(Danmaku *danmaku, DanmakuModifier modifier, int frame=-1);
void preformActions(Danmaku *danmaku);
void preformDraw(Danmaku *danmaku);
void updateModifier(Danmaku *danmaku, DanmakuModifier *modifier);
/// FUNCTIONS ^
#else

DanmakuModifier newDanmakuModifier() {
	DanmakuModifier modifier = {};
	modifier.speedAbsolute = false;
	modifier.speedAccelAbsolute = false;
	modifier.angleAbsolute = false;
	modifier.angleAccelAbsolute = false;
	modifier.spinAbsolute = false;
	modifier.spinAccelAbsolute = false;
	modifier.scaleAbsolute = false;
	modifier.scaleAccelAbsolute = false;
	modifier.alphaAbsolute = false;
	modifier.alphaAccelAbsolute = false;
	modifier.die = false;
	modifier.aimed = false;
	modifier.homing = false;
	modifier.repeat = false;
	return modifier;
}

void danmakuInit() {
	deathDanmakuModifier = newDanmakuModifier();
	deathDanmakuModifier.die = true;

	freezeDanmakuModifier = newDanmakuModifier();
	freezeDanmakuModifier.speedAbsolute = true;

	aimedDanmakuModifier = newDanmakuModifier();
	aimedDanmakuModifier.aimed = true;

	homingDanmakuModifier = newDanmakuModifier();
	homingDanmakuModifier.homing = true;
	homingDanmakuModifier.repeat = true;
}

Danmaku *createShotA(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color, int delay) {
	return createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(1, 0), graphic, color, false, delay, true);
}

Danmaku *createShotB(Vec2 pos, Vec2 posDest, int frameTime, DanmakuGraphic graphic, ColorName color, int delay) {
	float speed = pos.distance(posDest) / frameTime;
	float angle = degsBetween(pos, posDest);
	return createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(1, 0), graphic, color, false, delay, true);
}

Danmaku *createShotP(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color) {
	return createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(1, 0), graphic, color, true, 0, false);
}

Danmaku *createShotS(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color) {
	return createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(1, 0), graphic, color, true, 0, false);
}

Danmaku *createShotU(Vec2 pos, float speed, float angle, int difficulty) {
	Danmaku *danmaku = createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(5, 0), BALL_S, RED, false, 0, false);
	danmaku->difficulty = difficulty;
	return danmaku;
}

Danmaku *createShotZ(Vec2 pos, Vec2 speed, Vec2 angle, Vec2 spin, Vec2 scale, DanmakuGraphic graphic, ColorName color, bool ally, int delay, bool graze) {
	if (game->danmakuListNum > DANMAKU_MAX-1) game->danmakuListNum--;

	Danmaku *danmaku = &game->danmakuList[game->danmakuListNum++];
	memset(danmaku, 0, sizeof(Danmaku));

	danmaku->id = ++game->nextDanmakuId;
	danmaku->position = pos;
	danmaku->speed = speed.x;
	danmaku->speedAccel = speed.y;
	danmaku->angle = angle.x;
	danmaku->angleAccel = angle.y;
	danmaku->spin = spin.x;
	danmaku->spinAccel = spin.y;
	danmaku->scale = scale.x;
	danmaku->scaleAccel = scale.y;
	danmaku->health = 60;
	danmaku->graphic = graphic;
	danmaku->color = color;
	danmaku->frame = -delay;
	danmaku->grazable = graze;
	danmaku->alpha = 1;
	danmaku->alphaAccel = 0;

	return danmaku;
}

Danmaku *getDanmaku(int id) {
	for (int i = 0; i < game->danmakuListNum; i++) {
		Danmaku *danmaku = &game->danmakuList[i];
		if (danmaku->id == id) return danmaku;
	}

	return NULL;
}

void addModifier(Danmaku *danmaku, DanmakuModifier modifier, int frame) {
	if (danmaku->modifiersNum > DANMAKU_MODIFIERS_MAX) {
		logf("Too many modifier!\n");
		return;
	}

	if (danmaku->frame != -1) modifier.frame = frame;
	danmaku->modifiers[danmaku->modifiersNum++] = modifier;
}

void preformActions(Danmaku *danmaku) {
	DanmakuGraphicInfo *info = &game->danmakuGraphicInfos[danmaku->graphic];

	bool offScreen = false;
	if (danmaku->position.x + info->textureRadius < 0) offScreen = true;
	if (danmaku->position.x - info->textureRadius > game->playArea.width) offScreen = true;
	if (danmaku->position.y + info->textureRadius < 0) offScreen = true;
	if (danmaku->position.y - info->textureRadius > game->playArea.height) offScreen = true;

	if (offScreen) danmaku->health--;

	if (danmaku->frame >= 0) {
		if (danmaku->frame != 0) {
			danmaku->position.x -= cos(toRad(danmaku->positionOffset.x + danmaku->spin)) * danmaku->positionOffset.y;
			danmaku->position.y -= sin(toRad(danmaku->positionOffset.x + danmaku->spin)) * danmaku->positionOffset.y;
		}

		for (int i = 0; i < danmaku->modifiersNum; i++) {
			DanmakuModifier *modifier = &danmaku->modifiers[i];
			updateModifier(danmaku, modifier);
		}

		danmaku->position.x += cos(toRad(danmaku->angle)) * danmaku->speed;
		danmaku->position.y += sin(toRad(danmaku->angle)) * danmaku->speed;

		danmaku->speed += danmaku->speedAccel;
		danmaku->angle += danmaku->angleAccel;
		danmaku->spin += danmaku->spinAccel;
		danmaku->scale += danmaku->scaleAccel;
		danmaku->alpha += danmaku->alphaAccel;

		//eDanmakuUpdate(danmaku);

		danmaku->position.x += cos(toRad(danmaku->positionOffset.x + danmaku->spin)) * danmaku->positionOffset.y;
		danmaku->position.y += sin(toRad(danmaku->positionOffset.x + danmaku->spin)) * danmaku->positionOffset.y;
	}

	danmaku->frame++;
}

void preformDraw(Danmaku *danmaku) {
	Texture *texture = getTexture(frameSprintf("assets/images/bullets/%s.png", danmakuGraphicStrings[danmaku->graphic]));
	RenderProps props = newRenderProps();
	props.matrix.TRANSLATE(danmaku->position + v2(game->playArea.x, game->playArea.y));
	props.matrix.ROTATE(danmaku->spin + danmaku->angle);
	props.matrix.SCALE(danmaku->scale);
	props.matrix.TRANSLATE(-texture->width/2, -texture->height/2);
	props.hueShift = danmaku->color * toRad(30);
	props.alpha = danmaku->alpha;
	drawTexture(texture, props);
}

void updateModifier(Danmaku *danmaku, DanmakuModifier *modifier) {
	if (!modifier->repeat) {
		if (danmaku->frame != modifier->frame) return;
	} else {
		if (danmaku->frame <= 0) return;
		if (danmaku->frame % modifier->frame != 0) return;
	}

	//if (modifier->aimed) danmaku->angle = degsBetween(danmaku->position, game->playerPosition);
	//if (modifier->homing) {
	//	int targetIndex = getClosestEnemy(danmaku->position);
	//	if (targetIndex != -1) {
	//		danmaku->angle = lerpDeg(danmaku->angle, degsBetween(danmaku->position, game->danmakuList[targetIndex].position), 0.12);
	//	}
	//}
	danmaku->speed = modifier->speedAbsolute ? modifier->speedUpdate : danmaku->speed + modifier->speedUpdate;
	danmaku->speedAccel = modifier->speedAccelAbsolute ? modifier->speedAccelUpdate : danmaku->speedAccel + modifier->speedAccelUpdate;
	danmaku->angle = modifier->angleAbsolute ? modifier->angleUpdate : danmaku->angle + modifier->angleUpdate;
	danmaku->angleAccel = modifier->angleAccelAbsolute ? modifier->angleAccelUpdate : danmaku->angleAccel + modifier->angleAccelUpdate;
	danmaku->spin = modifier->spinAbsolute ? modifier->spinUpdate : danmaku->spin + modifier->spinUpdate;
	danmaku->spinAccel = modifier->spinAccelAbsolute ? modifier->spinAccelUpdate : danmaku->spinAccel + modifier->spinAccelUpdate;
	danmaku->alpha = modifier->alphaAbsolute ? modifier->alphaUpdate : danmaku->alpha + modifier->alphaUpdate;
	danmaku->alphaAccel = modifier->alphaAccelAbsolute ? modifier->alphaAccelUpdate : danmaku->alphaAccel + modifier->alphaAccelUpdate;
}

#endif
