#ifndef __DANMAKU_IMPL
enum DanmakuType {
	DTYPE_BULLET,
	DTYPE_PLAYER_BULLET,
	DTYPE_BOMB,
	DTYPE_KOUMA_KANNON,
	DTYPE_SPREAD_ATTACKER,
	DTYPE_MINE_DROPPER,
	DTYPE_SPINNER,
	DTYPE_RAIN,
	DTYPE_SEEK,
	DTYPE_ZSTRIKE,
	DTYPE_OMNI,
	DTYPE_BOSS_FLOWERGIRL,
	DTYPE_BOSS_GURUGURUGIRL,
	DTYPE_BOSS_STARGIRL,
	DTYPE_BOSS_THORNGIRL,
	DTYPE_BOSS_KNIFEGIRL,
	DTYPE_BOSS_WOLFGIRL,
	DTYPE_SPECIAL_LASERADD,
	DTYPE_SPECIAL_STAR,
	DTYPE_SPECIAL_SNIPER,
	DTYPE_SPECIAL_VINE,
	DTYPE_SPECIAL_BARB,
	DTYPE_SPECIAL_ORBITAR,
	DTYPE_SPECIAL_EXPLODER,
	DTYPE_SPECIAL_CHAIN,
	DTYPE_SPELL_ATTACK,
};
#define STARTING_BOSS_DTYPE (DTYPE_BOSS_FLOWERGIRL)
#define ENDING_BOSS_DTYPE (DTYPE_BOSS_WOLFGIRL)
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
enum ItemType {
	LIFE=0,
	BOMB=1,
	FULL=2,
	B_POWER=3,
	POWER=4,
	MANA=5,
	S_MANA=6,
	STAR=7,
};
const char *itemGraphicStrings[] = {
	"assets/images/items/LIFE.png",
	"assets/images/items/BOMB.png",
	"assets/images/items/FULL.png",
	"assets/images/items/B_POWER.png",
	"assets/images/items/POWER.png",
	"assets/images/items/SCORE.png",
	"assets/images/items/S_SCORE.png",
	"assets/images/items/STAR.png",
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
	DanmakuType type;
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
	bool dead; // Will go
	int frame;
	DanmakuGraphic graphic; ColorName color;
	bool grazable; // Will go
	char difficulty;
};

struct Item {
	int type;
	Vec2 position;
	float angle;
	int falling;
	bool claimed;
	bool dead;
};

struct Particle {
	Vec2 position;
	float speed;
	float angle;
#define PARTICLE_TIME_MAX 180
	int spawnFrame;
	ColorName color;
};

DanmakuModifier newDanmakuModifier();
void danmakuSys();
Danmaku *createShotA(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color, int delay);
Danmaku *createShotB(Vec2 pos, Vec2 posDest, int frameTime, DanmakuGraphic graphic, ColorName color, int delay);
Danmaku *createShotS(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color); // Bomb
Danmaku *createShotU(Vec2 pos, float speed, float angle, DanmakuType type, int difficulty);
Danmaku *createShotZ(Vec2 pos, Vec2 speed, Vec2 angle, Vec2 spin, Vec2 scale, DanmakuGraphic graphic, ColorName color, DanmakuType type, bool ally, int delay, bool graze);
Danmaku *getDanmaku(int id);

Item *createItem(ItemType type, Vec2 pos, bool falling, bool claimed);
void addModifier(Danmaku *danmaku, DanmakuModifier modifier, int frame=-1);
void updateModifier(Danmaku *danmaku, DanmakuModifier *modifier);
Particle *createParticle(Vec2 position, float speed, float angle, int deathFrame, ColorName color);
char *getModelFromGraphic(DanmakuGraphic graphic);
/// FUNCTIONS ^
#else

#include "dTypes.cpp"

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

void danmakuInitSys() {
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
	playBulletSound();
	return createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(1, 0), graphic, color, DTYPE_BULLET, false, delay, true);
}

Danmaku *createShotB(Vec2 pos, Vec2 posDest, int frameTime, DanmakuGraphic graphic, ColorName color, int delay) {
	playBulletSound();
	float speed = pos.distance(posDest) / frameTime;
	float angle = degsBetween(pos, posDest);
	return createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(1, 0), graphic, color, DTYPE_BULLET, false, delay, true);
}

Danmaku *createShotP(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color) {
	return createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(1, 0), graphic, color, DTYPE_PLAYER_BULLET, true, 0, false);
}

Danmaku *createShotS(Vec2 pos, float speed, float angle, DanmakuGraphic graphic, ColorName color) {
	return createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(1, 0), graphic, color, DTYPE_BOMB, true, 0, false);
}

Danmaku *createShotU(Vec2 pos, float speed, float angle, DanmakuType type, int difficulty) {
	Danmaku *danmaku = createShotZ(pos, v2(speed, 0), v2(angle, 0), v2(), v2(5, 0), BALL_S, propDanmakuGetEnemyColor(type), type, false, 0, false);
	danmaku->difficulty = difficulty;
	return danmaku;
}

Danmaku *createShotZ(Vec2 pos, Vec2 speed, Vec2 angle, Vec2 spin, Vec2 scale, DanmakuGraphic graphic, ColorName color, DanmakuType type, bool ally, int delay, bool graze) {
	if (game->danmakuListNum > DANMAKU_MAX-1) game->danmakuListNum--;

	Danmaku *danmaku = &game->danmakuList[game->danmakuListNum++];
	memset(danmaku, 0, sizeof(Danmaku));

	danmaku->type = type;
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
	danmaku->health = propDanmakuGetHealth(type);
	danmaku->graphic = graphic;
	danmaku->color = color;
	danmaku->frame = -delay;
	danmaku->grazable = graze;
	danmaku->alpha = 1;
	danmaku->alphaAccel = 0;

	if (delay != 0) {
		float randOff = rndFloat(0, M_PI * 2);
		for (int i = 0; i < 12; i++) {
			float startAngle = i * toRad(360.0 / 12) + randOff + rndFloat(-0.16, 0.16);
			Vec2 startingPosition = danmaku->position + v2(cos(startAngle), sin(startAngle)) * rndFloat(45, 60);
			createParticle(startingPosition, startingPosition.distance(danmaku->position) / delay, degsBetween(startingPosition, danmaku->position), delay, color);
		}
	}

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
	DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[danmaku->graphic];

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

		eDanmakuUpdate(danmaku);

		danmaku->position.x += cos(toRad(danmaku->positionOffset.x + danmaku->spin)) * danmaku->positionOffset.y;
		danmaku->position.y += sin(toRad(danmaku->positionOffset.x + danmaku->spin)) * danmaku->positionOffset.y;
	}

	danmaku->frame++;
}

void updateModifier(Danmaku *danmaku, DanmakuModifier *modifier) {
	if (!modifier->repeat) {
		if (danmaku->frame != modifier->frame) return;
	} else {
		if (danmaku->frame <= 0) return;
		if (danmaku->frame % modifier->frame != 0) return;
	}

	if (modifier->aimed) danmaku->angle = degsBetween(danmaku->position, game->playerPosition);
	if (modifier->homing) {
		int targetIndex = getClosestEnemy(danmaku->position);
		if (targetIndex != -1) {
			danmaku->angle = lerpDeg(danmaku->angle, degsBetween(danmaku->position, game->danmakuList[targetIndex].position), 0.12);
		}
	}
	danmaku->speed = modifier->speedAbsolute ? modifier->speedUpdate : danmaku->speed + modifier->speedUpdate;
	danmaku->speedAccel = modifier->speedAccelAbsolute ? modifier->speedAccelUpdate : danmaku->speedAccel + modifier->speedAccelUpdate;
	danmaku->angle = modifier->angleAbsolute ? modifier->angleUpdate : danmaku->angle + modifier->angleUpdate;
	danmaku->angleAccel = modifier->angleAccelAbsolute ? modifier->angleAccelUpdate : danmaku->angleAccel + modifier->angleAccelUpdate;
	danmaku->spin = modifier->spinAbsolute ? modifier->spinUpdate : danmaku->spin + modifier->spinUpdate;
	danmaku->spinAccel = modifier->spinAccelAbsolute ? modifier->spinAccelUpdate : danmaku->spinAccel + modifier->spinAccelUpdate;
	danmaku->alpha = modifier->alphaAbsolute ? modifier->alphaUpdate : danmaku->alpha + modifier->alphaUpdate;
	danmaku->alphaAccel = modifier->alphaAccelAbsolute ? modifier->alphaAccelUpdate : danmaku->alphaAccel + modifier->alphaAccelUpdate;
	danmaku->dead = modifier->die ? true : danmaku->dead;
}

Item *createItem(ItemType type, Vec2 position, bool falling, bool claimed) {
	if (game->itemListNum > ITEM_MAX-1) game->itemListNum--;

	Item *item = &game->itemList[game->itemListNum++];
	memset(item, 0, sizeof(Item));

	item->type = type;
	item->position = position;
	item->angle = 0;
	item->falling = falling ? 0 : 72;
	item->claimed = claimed;
	item->dead = false;

	return item;
}

Particle *createParticle(Vec2 position, float speed, float angle, int deathFrame, ColorName color) {
	if (game->particleListNum > PARTICLE_MAX-1) game->particleListNum--;

	Particle *particle = &game->particleList[game->particleListNum++];
	memset(particle, 0, sizeof(Particle));

	particle->position = position;
	particle->speed = speed;
	particle->angle = angle;
	particle->spawnFrame = (game->frameCount - PARTICLE_TIME_MAX) + deathFrame;
	particle->color = color;

	return particle;
}

char *getModelFromGraphic(DanmakuGraphic graphic) {
	switch (graphic) {
		case BALL_S:/*BALL*/            return "assets/models/Bullets/Ball.model";
		case BEAM:/*BEAM*/              return "assets/models/Bullets/Knife.model";
		case KOUMA:/*KNIFE*/            return "assets/models/Bullets/Knife.model";
		case MISSILE:/*MISSILE*/        return "assets/models/Bullets/Missile.model";
		case RICE_S:/*DIAMOND*/         return "assets/models/Bullets/Rice.model";
		case SCALE:/*ARROW*/            return "assets/models/Bullets/Rice.model";
		case SHAKU:/*Merge with ARROW*/ return "assets/models/Bullets/Knife.model";
		case STAR_S:/*STAR*/            return "assets/models/Bullets/Ball.model";
		case TENGU:/*MISSILE2*/         return "assets/models/Bullets/Knife.model";
		case TWIN:/*-?-*/               return "assets/models/Bullets/Missile.model";
		case YINYANG_L:/*-?-*/          return "assets/models/Bullets/Ball.model";
	}

	return NULL;
}

#endif
