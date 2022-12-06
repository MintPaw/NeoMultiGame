#ifdef CORE_HEADER
#undef CORE_HEADER

#define TILE_SIZE 64

float maxXpPerLevels[] = {
	100,
	300,
	1000,
	5000,
	50000,
	300000,
};

struct ActorTypeInfo {
#define ACTOR_TYPE_NAME_MAX_LEN 128
	char name[ACTOR_TYPE_NAME_MAX_LEN];
	bool isTower;
	bool isEnemy;
	int tier;

	float damage;
	float hpDamageMulti;
	float armorDamageMulti;
	float shieldDamageMulti;
	float baseRange;
	float rpm;
	float mana;
	int price;
	float priceMulti;

	float maxHp;
	float maxArmor;
	float maxShield;
	float hpGainPerSec;
	float armorGainPerSec;
	float shieldGainPerSec;
	float movementSpeed;

	float bulletSpeed;

	int enemySpawnStartingWave;

	int primaryColor;
	Vec3 size;
};

enum Priority {
	PRIORITY_PROGRESS,
	PRIORITY_MOST_HEALTH,
	PRIORITY_MOST_ARMOR,
	PRIORITY_MOST_SHIELD,
	PRIORITY_LEAST_HEALTH,
	PRIORITY_LEAST_ARMOR,
	PRIORITY_LEAST_SHIELD,
	PRIORITY_FASTEST,
	PRIORITY_SLOWEST,
	PRIORITIES_MAX,
};
char *priorityStrings[] = {
	"Progress",
	"Most health",
	"Most armor",
	"Most shield",
	"Least health",
	"Least armor",
	"Least shield",
	"Fastest",
	"Slowest",
};

enum DotType {
	DOT_POISON,
	DOT_BURN,
	DOT_BLEED,
};
struct Dot {
	DotType type;
	int src;
	int ticks;
};

struct Stats {
	int investment;
	int shots;
	float shieldDamage;
	float armorDamage;
	float hpDamage;
};

enum ActorType {
	ACTOR_NONE=0,
	ACTOR_BALLISTA, ACTOR_MORTAR_TOWER, ACTOR_TESLA_COIL, ACTOR_FROST_KEEP, ACTOR_FLAME_THROWER, ACTOR_POISON_SPRAYER, ACTOR_SHREDDER, ACTOR_ENCAMPENT,
	ACTOR_LOOKOUT, ACTOR_RADAR, ACTOR_OBELISK, ACTOR_PARTICLE_CANNON, ACTOR_TOWER13, ACTOR_TOWER14, ACTOR_TOWER15, ACTOR_MANA_SIPHON,
	ACTOR_ENEMY1, ACTOR_ENEMY2, ACTOR_ENEMY3, ACTOR_ENEMY4, ACTOR_ENEMY5, ACTOR_ENEMY6, ACTOR_ENEMY7, ACTOR_ENEMY8,
	ACTOR_ENEMY9, ACTOR_ENEMY10, ACTOR_ENEMY11, ACTOR_ENEMY12, ACTOR_ENEMY13, ACTOR_ENEMY14, ACTOR_ENEMY15, ACTOR_ENEMY16,
	ACTOR_ENEMY17, ACTOR_ENEMY18, ACTOR_ENEMY19, ACTOR_ENEMY20, ACTOR_ENEMY21, ACTOR_ENEMY22, ACTOR_ENEMY23, ACTOR_ENEMY24,
	ACTOR_ENEMY25, ACTOR_ENEMY26, ACTOR_ENEMY27, ACTOR_ENEMY28, ACTOR_ENEMY29, ACTOR_ENEMY30, ACTOR_ENEMY31, ACTOR_ENEMY32,
	ACTOR_ENEMY33, ACTOR_ENEMY34, ACTOR_ENEMY35, ACTOR_ENEMY36, ACTOR_ENEMY37, ACTOR_ENEMY38, ACTOR_ENEMY39, ACTOR_ENEMY40,
	ACTOR_ENEMY41, ACTOR_ENEMY42, ACTOR_ENEMY43, ACTOR_ENEMY44, ACTOR_ENEMY45, ACTOR_ENEMY46, ACTOR_ENEMY47, ACTOR_ENEMY48,
	ACTOR_ENEMY49, ACTOR_ENEMY50, ACTOR_ENEMY51, ACTOR_ENEMY52, ACTOR_ENEMY53, ACTOR_ENEMY54, ACTOR_ENEMY55, ACTOR_ENEMY56,
	ACTOR_ENEMY57, ACTOR_ENEMY58, ACTOR_ENEMY59, ACTOR_ENEMY60, ACTOR_ENEMY61, ACTOR_ENEMY62, ACTOR_ENEMY63, ACTOR_ENEMY64,
	ACTOR_ARROW, ACTOR_MORTAR, ACTOR_FROST, ACTOR_SAW,
	ACTOR_BULLET5, ACTOR_BULLET6, ACTOR_BULLET7, ACTOR_BULLET8,
	ACTOR_BULLET9, ACTOR_BULLET10, ACTOR_BULLET11, ACTOR_BULLET12,
	ACTOR_BULLET13, ACTOR_BULLET14, ACTOR_BULLET15, ACTOR_BULLET16,
	ACTOR_MANA_CRYSTAL,
	ACTOR_TYPES_MAX,
};
struct Actor {
	ActorType type;
	int id;

	Vec2 position;
	Vec2 velo;
	Vec2 accel;
	float aimRads;
	int aimTarget;

	float hp;
	float armor;
	float shield;

	float timeTillNextShot;
	float timeSinceLastShot;
	bool markedForDeletion;

	Dot *dots;
	int dotsNum;
	float slow;
	float movementSpeed;

	Stats *stats;
	int statsNum;

	Priority priority;

	int bulletTarget;
	Vec2 bulletTargetPosition;
	int parentTower;

#define SAW_HIT_LIST_MAX 64
	int *sawHitList;
	int sawHitListNum;

	int amountPaid;

	int level;
	float xp;

	float time;

	/// Unserialized
	int dotsMax;
};

enum TileType {
	TILE_NONE,
	TILE_HOME,
	TILE_GROUND,
	TILE_ROAD,
};
struct Tile {
	TileType type;
	Vec2 flow;
	int costSoFar;
	int dijkstraValue;
	u8 elevation;
	float perlinValue;
};

struct Chunk {
	Vec2i position;
	Rect rect;
#define CHUNK_SIZE 7
	Tile tiles[CHUNK_SIZE*CHUNK_SIZE];

	Vec2i connections[4];
	int connectionsNum;

	bool visible;
};

enum Tool {
	TOOL_NONE,
	TOOL_BUILDING,
	TOOL_SELECTED,
	TOOL_TILE_SELECTION,
};

enum UpgradeEffectType {
	UPGRADE_EFFECT_UNLOCK_BALLISTA=0,
	UPGRADE_EFFECT_UNLOCK_MORTAR_TOWER=1,
	UPGRADE_EFFECT_UNLOCK_TESLA_COIL=2,
	UPGRADE_EFFECT_UNLOCK_FLAME_THROWER=3,
	UPGRADE_EFFECT_UNLOCK_POISON_SPRAYER=4,
	UPGRADE_EFFECT_UNLOCK_SHREDDER=5,
	UPGRADE_EFFECT_UNLOCK_MANA_SIPHON=6,
	UPGRADE_EFFECT_UNLOCK_FROST_KEEP=7,
	//...
	UPGRADE_EFFECT_DAMAGE_MULTI=32,
	UPGRADE_EFFECT_RANGE_MULTI=33,
	UPGRADE_EFFECT_RPM_MULTI=34,
	UPGRADE_EFFECT_EXTRA_CARDS=35,
	UPGRADE_EFFECT_EXTRA_MONEY=36,
	UPGRADE_EFFECT_MANA_GAIN_MULTI=37,
	UPGRADE_EFFECT_EXTRA_TIME_SCALE=38,
	UPGRADE_EFFECT_RELOAD=39,
	UPGRADE_EFFECT_EXTRA_SAW_PIERCE=40,
	UPGRADE_EFFECT_MIN_TOWER_LEVEL_PERC=41,
	UPGRADE_EFFECT_BULLET_SPEED_MULTI=42,
	UPGRADE_EFFECT_MORE_POISON_TICKS=43,
	UPGRADE_EFFECT_MORE_BURN_TICKS=44,
	UPGRADE_EFFECT_MORE_BLEED_TICKS=45,
	UPGRADE_EFFECT_GAIN_MONEY=46,
	UPGRADE_EFFECT_TYPES_MAX,
};

struct UpgradeEffect {
	UpgradeEffectType type;
	ActorType actorType;
	float value;
};
struct Upgrade {
	int id;
#define UPGRADE_EFFECTS_MAX 8
	UpgradeEffect effects[UPGRADE_EFFECTS_MAX];
	int effectsNum;

#define UPGRADE_PREREQS_MAX 8
	int prereqUpgrades[UPGRADE_PREREQS_MAX];
	int prereqUpgradesNum;

	UpgradeEffectType prereqEffects[UPGRADE_PREREQS_MAX];
	int prereqEffectsNum;
};

enum CoreEventType {
	CORE_EVENT_DAMAGE,
	CORE_EVENT_SHOOT,
	CORE_EVENT_HIT,
	CORE_EVENT_SHOW_GHOST,
	CORE_EVENT_MORTAR_EXPLOSION,
};
struct CoreEvent {
	CoreEventType type;
	float floatValue;

	float shieldValue;
	float armorValue;
	float hpValue;

	Vec2 ghostOrMortarPosition;
	ActorType ghostActorType;
	int srcId;
	int destId;
};

enum Phase {
	PHASE_PLANNING,
	PHASE_WAVE,
	PHASE_RESULTS,
};

enum MapGenMode {
	MAP_GEN_NONE,
	MAP_GEN_RANDOM,
	MAP_GEN_CONTROLLED_SPLIT,
};

struct GameData {
	MapGenMode mapGenMode;

#define ACTORS_MAX 65535
	Actor actors[ACTORS_MAX];
	int actorsNum;
	int nextActorId;

#define CHUNKS_MAX 512
	Chunk chunks[CHUNKS_MAX];
	int chunksNum;

#define CAMPAIGN_NAME_MAX_LEN 64
	char campaignName[CAMPAIGN_NAME_MAX_LEN];

	float time;

	Vec2 cameraPosition;
	float cameraZoom;

	Tool prevTool;
	Tool tool;
	float toolTime;
	ActorType actorToBuild;

	int hp;
	int money;
	float mana;
	float maxMana;

	Phase phase;
	Phase prevPhase;
	float phaseTime;

	int wave;

	ActorType actorsToSpawn[ACTORS_MAX];
	int actorsToSpawnNum;
	int startingActorsToSpawnNum;
	float timeTillNextSpawn;

	float timeTillNextDot;
	int dotQuadrant;

#define SELECTED_ACTORS_MAX 2048
	int selectedActors[SELECTED_ACTORS_MAX];
	int selectedActorsNum;

#define UPGRADES_MAX 256
	int ownedUpgrades[UPGRADES_MAX];
	int ownedUpgradesNum;

	Stats *actorTypeStats[ACTOR_TYPES_MAX];
	int actorTypeStatsEach;

	bool hasUsedPoison;
	bool hasUsedBurn;
	bool hasUsedBleed;
};

struct Core {
	ActorTypeInfo actorTypeInfos[ACTOR_TYPES_MAX];
	int actorTypeCounts[ACTOR_TYPES_MAX];

	Upgrade upgrades[UPGRADES_MAX];
	int upgradesNum;
	int nextUpgradeId;

	float manaToGain;
	int enemiesAlive;

	GameData data;

	int presentedUpgrades[UPGRADES_MAX];
	int presentedUpgradesNum;

#define CORE_EVENTS_MAX 1024
	CoreEvent coreEvents[CORE_EVENTS_MAX];
	int coreEventsNum;
};

#else

void initCore(MapGenMode mapGenMode);
void stepGame(float elapsed);

bool generateMap(MapGenMode mapGenMode);
void connect(Chunk *chunkA, Chunk *chunkB);
Vec2i *getAdjecentOpenChunkPositions(Vec2i position, int *outPossiblePositionsNum);
void carveChunkConnections(Chunk *chunk);
void generateMapFields();
Chunk *createChunk(Vec2i position);
void removeConnection(Chunk *chunk, Vec2i toRemove);
void removeChunk(Chunk *chunk);
Chunk *getChunkAt(Vec2i position);
Tile *getTileAt(Vec2i position);
Vec2i chunkTileToWorldTile(Chunk *chunk, Vec2i tile);
Chunk *worldToChunk(Vec2 position);
Vec2i worldToTile(Vec2 position);
Vec2 tileToWorld(Vec2i tile);
Rect tileToWorldRect(Vec2i tile);
bool tileBlocksPathing(TileType type);

Actor *createActor(ActorType type);
void initActor(Actor *actor);
void deinitActor(Actor *actor);
Actor *getActor(int id);

Rect getRect(Actor *actor);
float getRange(ActorType actorType, Vec2i tilePos);
float getRange(Actor *actor, Vec2i tilePos);
float getDamage(Actor *actor);
float getRpm(Actor *actor);
float getBulletSpeed(Actor *actor);
int getMaxLevel(ActorType actorType);
ActorTypeInfo *getInfo(Actor *actor);

Actor *createBullet(Actor *src, Actor *target);
void dealDamage(Actor *dest, int srcId, float amount, float shieldDamageMulti, float armorDamageMulti, float hpDamageMulti, bool noCoreEvent=false);
void dealDamage(Actor *bullet, Actor *dest);

void createDot(Actor *src, Actor *dest, DotType type, int ticks);
int sumDotTicks(Actor *actor, DotType type);

Upgrade *createUpgrade();
Upgrade *getUpgrade(int id);
void unlockUpgrade(Upgrade *upgrade);
bool hasUpgrade(int id);
bool hasUpgradeEffect(UpgradeEffectType effectType, ActorType actorType=ACTOR_NONE);
bool hasPrereqs(int upgradeId);

Actor **getActorsInRange(Circle range, int *outNum, bool enemiesOnly);
Actor **getActorsInRange(Tri2 range, int *outNum, bool enemiesOnly);
ActorType *generateWave(int wave, int *outActorTypesNum);
void startNextWave();
Tri2 getAttackTri(Vec2 start, float range, float angle, float deviation);

void addShotStat(Actor *actor);
void addDamageStat(Actor *actor, float shieldAmount, float armorAmount, float hpAmount);
void addInvestmentStat(Actor *actor, int amount);

void saveState(char *path);
void writeStats(DataStream *stream, Stats stats);
void writeActor(DataStream *stream, Actor *actor);
void writeChunk(DataStream *stream, Chunk *chunk);
void writeTile(DataStream *stream, Tile tile);

void loadState(char *path);
void readStats(DataStream *stream, Stats *stats, int version);
void readActor(DataStream *stream, Actor *actor, int version);
void readChunk(DataStream *stream, Chunk *chunk, int version);
Tile readTile(DataStream *stream, int version);
/// FUNCTIONS ^

void initCore(MapGenMode mapGenMode) {
	for (int i = 0; i < data->actorsNum; i++) deinitActor(&data->actors[i]);
	memset(core, 0, sizeof(Core));

	{ /// Setup actor type infos
		for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
			ActorTypeInfo *info = &core->actorTypeInfos[i];
			sprintf(info->name, "Actor %d", i);
			info->size = v3(TILE_SIZE*0.5, TILE_SIZE*0.5, TILE_SIZE*0.5);
			info->primaryColor = 0xFF000000;
		}

		for (int i = ACTOR_BALLISTA; i <= ACTOR_PARTICLE_CANNON; i++) {
			ActorTypeInfo *info = &core->actorTypeInfos[i];
			info->isTower = true;
			info->size = v3(TILE_SIZE*0.75, TILE_SIZE*0.75, TILE_SIZE*0.75);
		}

		for (int i = ACTOR_ENEMY1; i <= ACTOR_ENEMY64; i++) {
			ActorTypeInfo *info = &core->actorTypeInfos[i];
			info->isEnemy = true;
			info->size = v3(TILE_SIZE*0.25, TILE_SIZE*0.25, TILE_SIZE*0.25);
		}

		ActorTypeInfo *info = NULL;

		info = &core->actorTypeInfos[ACTOR_BALLISTA];
		strncpy(info->name, "Ballista", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 3;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 5;
		info->shieldDamageMulti = 5;
		info->baseRange = 5 * TILE_SIZE;
		info->rpm = 16;
		info->mana = 0;
		info->price = 10;
		info->priceMulti = 15;
		info->primaryColor = 0xFF800000;

		info = &core->actorTypeInfos[ACTOR_MORTAR_TOWER];
		strncpy(info->name, "Mortar", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 8;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 15;
		info->shieldDamageMulti = 5;
		info->baseRange = 10 * TILE_SIZE;
		info->rpm = 10;
		info->mana = 0;
		info->price = 200;
		info->priceMulti = 75;
		info->primaryColor = 0xFF525252;

		info = &core->actorTypeInfos[ACTOR_TESLA_COIL];
		strncpy(info->name, "Tesla Coil", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 10;
		info->hpDamageMulti = 6;
		info->armorDamageMulti = 3;
		info->shieldDamageMulti = 9;
		info->baseRange = 1.5 * TILE_SIZE;
		info->rpm = 30;
		info->mana = 5;
		info->price = 200;
		info->priceMulti = 75;
		info->primaryColor = 0xFFA0A0F0;

		info = &core->actorTypeInfos[ACTOR_FROST_KEEP];
		strncpy(info->name, "Frost Keep", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 5;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 5;
		info->shieldDamageMulti = 5;
		info->baseRange = 2 * TILE_SIZE;
		info->rpm = 180;
		info->mana = 0.3;
		info->price = 250;
		info->priceMulti = 100;
		info->primaryColor = 0xFFE3F0F5;

		info = &core->actorTypeInfos[ACTOR_FLAME_THROWER];
		strncpy(info->name, "Flame Thrower", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 6;
		info->hpDamageMulti = 1;
		info->armorDamageMulti = 1;
		info->shieldDamageMulti = 1;
		info->baseRange = 4 * TILE_SIZE;
		info->rpm = 60;
		info->mana = 1;
		info->price = 300;
		info->priceMulti = 75;
		info->primaryColor = lerpColor(BURN_COLOR, 0xFF000000, 0.75);

		info = &core->actorTypeInfos[ACTOR_POISON_SPRAYER];
		strncpy(info->name, "Poison Sprayer", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 6;
		info->hpDamageMulti = 1;
		info->armorDamageMulti = 1;
		info->shieldDamageMulti = 1;
		info->baseRange = 4 * TILE_SIZE;
		info->rpm = 60;
		info->mana = 1;
		info->price = 300;
		info->priceMulti = 75;
		info->primaryColor = lerpColor(POISON_COLOR, 0xFF000000, 0.75);

		info = &core->actorTypeInfos[ACTOR_SHREDDER];
		strncpy(info->name, "Shredder", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 6;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 10;
		info->shieldDamageMulti = 10;
		info->baseRange = 5 * TILE_SIZE;
		info->rpm = 5;
		info->price = 500;
		info->priceMulti = 100;
		info->primaryColor = 0xFF800000;

		info = &core->actorTypeInfos[ACTOR_ENCAMPENT];
		strncpy(info->name, "Encampent", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 20;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 15;
		info->shieldDamageMulti = 5;
		info->baseRange = 2 * TILE_SIZE;
		info->rpm = 5;
		info->price = 500;
		info->priceMulti = 100;

		info = &core->actorTypeInfos[ACTOR_LOOKOUT];
		strncpy(info->name, "Lookout", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 1;
		info->hpDamageMulti = 2;
		info->armorDamageMulti = 1;
		info->shieldDamageMulti = 3;
		info->baseRange = 8 * TILE_SIZE;
		info->rpm = 0;
		info->price = 500;
		info->priceMulti = 100;

		info = &core->actorTypeInfos[ACTOR_RADAR];
		strncpy(info->name, "Radar", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 20;
		info->hpDamageMulti = 20;
		info->armorDamageMulti = 10;
		info->shieldDamageMulti = 10;
		info->baseRange = 30 * TILE_SIZE;
		info->rpm = 700;
		info->price = 1000;
		info->priceMulti = 250;

		info = &core->actorTypeInfos[ACTOR_OBELISK];
		strncpy(info->name, "Obelisk", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 8;
		info->hpDamageMulti = 5;
		info->armorDamageMulti = 10;
		info->shieldDamageMulti = 2;
		info->baseRange = 5 * TILE_SIZE;
		info->rpm = 360;
		info->mana = 2;
		info->price = 1000;
		info->priceMulti = 250;

		info = &core->actorTypeInfos[ACTOR_PARTICLE_CANNON];
		strncpy(info->name, "Particle Cannon", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 50;
		info->hpDamageMulti = 15;
		info->armorDamageMulti = 10;
		info->shieldDamageMulti = 20;
		info->baseRange = 20 * TILE_SIZE;
		info->rpm = 360;
		info->mana = 12;
		info->price = 1000;
		info->priceMulti = 250;

		info = &core->actorTypeInfos[ACTOR_MANA_SIPHON];
		strncpy(info->name, "Mana Siphon", ACTOR_TYPE_NAME_MAX_LEN);
		info->price = 100;
		info->priceMulti = 10;
		info->primaryColor = 0xFFA4CCC8;

		info = &core->actorTypeInfos[ACTOR_MANA_CRYSTAL];
		strncpy(info->name, "Mana Crystal", ACTOR_TYPE_NAME_MAX_LEN);
		info->primaryColor = 0xFFA4B0CC;

		info = &core->actorTypeInfos[ACTOR_ENEMY1]; // Goblin
		info->tier = 1;
		info->enemySpawnStartingWave = 1;
		info->movementSpeed = 2;
		info->maxHp = 100;

		info = &core->actorTypeInfos[ACTOR_ENEMY2]; // Orc
		info->tier = 1;
		info->enemySpawnStartingWave = 3;
		info->movementSpeed = 2;
		info->maxHp = 300;

		info = &core->actorTypeInfos[ACTOR_ENEMY3]; // Armored Goblin
		info->tier = 1;
		info->enemySpawnStartingWave = 5;
		info->movementSpeed = 1.75;
		info->maxHp = 400;
		info->maxArmor = 200;

		info = &core->actorTypeInfos[ACTOR_ENEMY4]; // Troll
		info->tier = 1;
		info->enemySpawnStartingWave = 7;
		info->movementSpeed = 1.75;
		info->maxHp = 800;
		info->hpGainPerSec = 25;

		info = &core->actorTypeInfos[ACTOR_ENEMY5]; // Armored Orc
		info->tier = 1;
		info->enemySpawnStartingWave = 9;
		info->movementSpeed = 1.75;
		info->maxHp = 400;
		info->maxArmor = 600;

		info = &core->actorTypeInfos[ACTOR_ENEMY6]; // Battering Ram
		info->tier = 1;
		info->enemySpawnStartingWave = 11;
		info->movementSpeed = 1;
		info->maxHp = 300;
		info->maxArmor = 1500;

		info = &core->actorTypeInfos[ACTOR_ENEMY7]; // Cyclops
		info->tier = 1;
		info->enemySpawnStartingWave = 13;
		info->movementSpeed = 1.25;
		info->maxHp = 2000;

		info = &core->actorTypeInfos[ACTOR_ENEMY8]; // Ooogie
		info->tier = 999;
		info->enemySpawnStartingWave = 15;
		info->movementSpeed = 1;
		info->maxHp = 20000;

		info = &core->actorTypeInfos[ACTOR_ENEMY9]; // Witch
		info->tier = 2;
		info->enemySpawnStartingWave = 16;
		info->movementSpeed = 3;
		info->maxHp = 1000;
		info->maxShield = 1000;

		info = &core->actorTypeInfos[ACTOR_ENEMY10]; // Bat
		info->tier = 2;
		info->enemySpawnStartingWave = 18;
		info->movementSpeed = 3.6;
		info->maxHp = 2000;

		info = &core->actorTypeInfos[ACTOR_ENEMY11]; // Vampire
		info->tier = 2;
		info->enemySpawnStartingWave = 20;
		info->movementSpeed = 2;
		info->maxHp = 3000;
		info->hpGainPerSec = 100;
		info->maxShield = 1000;

		info = &core->actorTypeInfos[ACTOR_ENEMY12]; // Jack'o'Lantern
		info->tier = 2;
		info->enemySpawnStartingWave = 22;
		info->movementSpeed = 1.5;
		info->maxHp = 1000;
		info->maxArmor = 5000;

		info = &core->actorTypeInfos[ACTOR_ENEMY13]; // Werewolf
		info->tier = 2;
		info->enemySpawnStartingWave = 24;
		info->movementSpeed = 2;
		info->maxHp = 3000;
		info->hpGainPerSec = 3000;
		info->maxShield = 2000;

		info = &core->actorTypeInfos[ACTOR_ENEMY14]; // Ooogie von Ooogovich
		info->tier = 2;
		info->enemySpawnStartingWave = 999;
		info->movementSpeed = 1.1;
		info->maxHp = 40000;
		info->hpGainPerSec = 100;

		info = &core->actorTypeInfos[ACTOR_ARROW];
		info->size = v3(0.1, 0.1, 0.1) * TILE_SIZE;
		info->bulletSpeed = 20;

		info = &core->actorTypeInfos[ACTOR_MORTAR];
		strncpy(info->name, "Mortar", ACTOR_TYPE_NAME_MAX_LEN);
		info->bulletSpeed = 1;
		info->baseRange = 2 * TILE_SIZE;
	} ///

	{ /// Setup upgrades
		Upgrade *upgrade = NULL;
		UpgradeEffect *effect = NULL;

		upgrade = createUpgrade();
		effect = &upgrade->effects[upgrade->effectsNum++];
		effect->type = UPGRADE_EFFECT_UNLOCK_BALLISTA;
		unlockUpgrade(upgrade);

		upgrade = createUpgrade();
		effect = &upgrade->effects[upgrade->effectsNum++];
		effect->type = UPGRADE_EFFECT_UNLOCK_MORTAR_TOWER;

		upgrade = createUpgrade();
		effect = &upgrade->effects[upgrade->effectsNum++];
		effect->type = UPGRADE_EFFECT_UNLOCK_TESLA_COIL;

		upgrade = createUpgrade();
		effect = &upgrade->effects[upgrade->effectsNum++];
		effect->type = UPGRADE_EFFECT_UNLOCK_FLAME_THROWER;

		upgrade = createUpgrade();
		effect = &upgrade->effects[upgrade->effectsNum++];
		effect->type = UPGRADE_EFFECT_UNLOCK_POISON_SPRAYER;

		upgrade = createUpgrade();
		effect = &upgrade->effects[upgrade->effectsNum++];
		effect->type = UPGRADE_EFFECT_UNLOCK_SHREDDER;

		// upgrade = createUpgrade();
		// effect = &upgrade->effects[upgrade->effectsNum++];
		// effect->type = UPGRADE_EFFECT_UNLOCK_FROST_KEEP;

		ActorType actorsCouldUpgrade[] = {
			ACTOR_BALLISTA, ACTOR_MORTAR_TOWER, ACTOR_TESLA_COIL, ACTOR_FLAME_THROWER, ACTOR_POISON_SPRAYER, ACTOR_SHREDDER,
		};
		// ACTOR_FROST_KEEP, ACTOR_ENCAMPENT, ACTOR_LOOKOUT, ACTOR_RADAR, ACTOR_OBELISK, ACTOR_PARTICLE_CANNON,

		for (int i = 0; i < ArrayLength(actorsCouldUpgrade); i++) {
			ActorType actorType = actorsCouldUpgrade[i];

			UpgradeEffectType unlockEffect;
			if (actorType == ACTOR_BALLISTA) unlockEffect = UPGRADE_EFFECT_UNLOCK_BALLISTA;
			if (actorType == ACTOR_MORTAR_TOWER) unlockEffect = UPGRADE_EFFECT_UNLOCK_MORTAR_TOWER;
			if (actorType == ACTOR_TESLA_COIL) unlockEffect = UPGRADE_EFFECT_UNLOCK_TESLA_COIL;
			if (actorType == ACTOR_FLAME_THROWER) unlockEffect = UPGRADE_EFFECT_UNLOCK_FLAME_THROWER;
			if (actorType == ACTOR_POISON_SPRAYER) unlockEffect = UPGRADE_EFFECT_UNLOCK_POISON_SPRAYER;
			if (actorType == ACTOR_SHREDDER) unlockEffect = UPGRADE_EFFECT_UNLOCK_SHREDDER;
			if (actorType == ACTOR_MANA_SIPHON) unlockEffect = UPGRADE_EFFECT_UNLOCK_MANA_SIPHON;
			if (actorType == ACTOR_FROST_KEEP) unlockEffect = UPGRADE_EFFECT_UNLOCK_FROST_KEEP;

			Upgrade *prevUpgrade = NULL;

			for (int i = 0; i < 2; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_DAMAGE_MULTI;
				effect->actorType = actorType;
				effect->value = 1.5;
				if (prevUpgrade) upgrade->prereqUpgrades[upgrade->prereqUpgradesNum++] = prevUpgrade->id;
				upgrade->prereqEffects[upgrade->prereqEffectsNum++] = unlockEffect;
				prevUpgrade = upgrade;
			}

			prevUpgrade = NULL;
			for (int i = 0; i < 2; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_RANGE_MULTI;
				effect->actorType = actorType;
				effect->value = 1.5;
				if (effect->actorType == ACTOR_TESLA_COIL) effect->value = 1.1;
				if (prevUpgrade) upgrade->prereqUpgrades[upgrade->prereqUpgradesNum++] = prevUpgrade->id;
				upgrade->prereqEffects[upgrade->prereqEffectsNum++] = unlockEffect;
				prevUpgrade = upgrade;
			}

			prevUpgrade = NULL;
			for (int i = 0; i < 2; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_RPM_MULTI;
				effect->actorType = actorType;
				effect->value = 1.5;
				if (effect->actorType == ACTOR_TESLA_COIL) effect->value = 1.25;
				if (prevUpgrade) upgrade->prereqUpgrades[upgrade->prereqUpgradesNum++] = prevUpgrade->id;
				upgrade->prereqEffects[upgrade->prereqEffectsNum++] = unlockEffect;
				prevUpgrade = upgrade;
			}
		}

		{
			Upgrade *prevUpgrade = NULL;
			for (int i = 0; i < 3; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_EXTRA_CARDS;
				effect->value = 2;
				if (prevUpgrade) upgrade->prereqUpgrades[upgrade->prereqUpgradesNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}
		}

		{
			Upgrade *prevUpgrade = NULL;
			for (int i = 0; i < 5; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_EXTRA_MONEY;
				effect->value = i+1;
				if (prevUpgrade) upgrade->prereqUpgrades[upgrade->prereqUpgradesNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}
		}

		{
			Upgrade *prevUpgrade = NULL;
			for (int i = 0; i < 3; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_MANA_GAIN_MULTI;
				effect->value = 1.1;
				if (prevUpgrade) upgrade->prereqUpgrades[upgrade->prereqUpgradesNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}
		}

		{
			Upgrade *upgrade = createUpgrade();
			UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_EXTRA_TIME_SCALE;
			effect->value = 0.01;
		}

		{
			Upgrade *upgrade = createUpgrade();
			UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_RELOAD;
		}

		{
			Upgrade *upgrade = createUpgrade();
			UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_EXTRA_SAW_PIERCE;
			effect->value = 6;
			upgrade->prereqEffects[upgrade->prereqEffectsNum++] = UPGRADE_EFFECT_UNLOCK_SHREDDER;
		}

		{
			Upgrade *upgrade = createUpgrade();
			UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_MIN_TOWER_LEVEL_PERC;
			effect->value = 1;
		}

		{
			int prevId = -1;
			upgrade = createUpgrade();
			effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_BULLET_SPEED_MULTI;
			effect->actorType = ACTOR_MORTAR;
			effect->value = 2;
			upgrade->prereqEffects[upgrade->prereqEffectsNum++] = UPGRADE_EFFECT_UNLOCK_MORTAR_TOWER;
			prevId = upgrade->id;

			upgrade = createUpgrade();
			effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_BULLET_SPEED_MULTI;
			effect->actorType = ACTOR_MORTAR;
			effect->value = 2;
			upgrade->prereqUpgrades[upgrade->prereqUpgradesNum++] = prevId;
		}

		{
			for (int i = 0; i < 3; i++) {
				UpgradeEffectType effectType;
				if (i == 0) effectType = UPGRADE_EFFECT_MORE_POISON_TICKS;
				else if (i == 1) effectType = UPGRADE_EFFECT_MORE_BURN_TICKS;
				else effectType = UPGRADE_EFFECT_MORE_BLEED_TICKS;

				upgrade = createUpgrade();
				effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = effectType;
				effect->value = 1;

				upgrade = createUpgrade();
				effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = effectType;
				effect->value = 1;
				upgrade->prereqEffects[upgrade->prereqEffectsNum++] = effectType;
			}
		}

		{
			upgrade = createUpgrade();
			effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_GAIN_MONEY;
			effect->value = 1000;

			upgrade = createUpgrade();
			effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_GAIN_MONEY;
			effect->value = 5000;
			upgrade->prereqEffects[upgrade->prereqEffectsNum++] = UPGRADE_EFFECT_GAIN_MONEY;
		}
	} ///

	for (int i = 0; ; i++) {
		if (generateMap(mapGenMode)) break;
		if (i == 1000) {
			logf("Failed to generate map 1000 times\n");
			break;
		}
	}

	if (mapGenMode != MAP_GEN_NONE) generateMapFields();
	data->mapGenMode = mapGenMode;

	data->hp = 10;
	data->money = 1000;
	data->mana = 100;
	data->maxMana = 100;

	data->cameraZoom = 1;
}

void stepGame(float elapsed) {
	float timeScale = elapsed / (1/60.0);
	core->coreEventsNum = 0;

	if (data->prevPhase != data->phase) {
		data->prevPhase = data->phase;
		data->phaseTime = 0;
	}

	memset(core->actorTypeCounts, 0, sizeof(int) * ACTOR_TYPES_MAX);
	for (int i = 0; i < data->actorsNum; i++) {
		Actor *actor = &data->actors[i];
		core->actorTypeCounts[actor->type]++;
	}

	int statsMax = data->wave+2; //@copyPastedStatsMax
	if (data->actorTypeStatsEach < statsMax-1) {
		for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
			data->actorTypeStats[i] = (Stats *)resizeArray(data->actorTypeStats[i], sizeof(Stats), data->actorTypeStatsEach, statsMax);
		}
		data->actorTypeStatsEach = statsMax;
	}

	core->manaToGain = 1 * elapsed;
	core->enemiesAlive = 0;
	{ /// Update actors
		for (int i = 0; i < data->actorsNum; i++) {
			Actor *actor = &data->actors[i];
			ActorTypeInfo *info = &core->actorTypeInfos[actor->type];

			if (actor->statsNum < statsMax-1) {
				actor->stats = (Stats *)resizeArray(actor->stats, sizeof(Stats), actor->statsNum, statsMax);
				actor->statsNum = statsMax;
			}

			actor->movementSpeed = info->movementSpeed;
			actor->movementSpeed *= clampMap(actor->slow, 0, 10, 1, 0.4);

			actor->slow -= 6*elapsed;

			bool towerCaresAboutTargets = true;
			bool towerIsActiveBetweenWaves = true;
			if (actor->type == ACTOR_FROST_KEEP) {
				towerCaresAboutTargets = false;
				towerIsActiveBetweenWaves = false;
			}

			bool towerShouldFire = false;
			Actor *target = NULL;
			if (info->isTower) {
				if (towerCaresAboutTargets) {
					Circle range = makeCircle(actor->position, getRange(actor, worldToTile(actor->position)));

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(range, &enemiesInRangeNum, true);

					Actor *bestEnemy = NULL;
					float bestEnemyScore = 0;
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						// ActorTypeInfo *enemyInfo = &core->actorTypeInfos[enemy->type];
						float score = 0;
						if (actor->priority == PRIORITY_PROGRESS) {
							Tile *tile = getTileAt(worldToTile(enemy->position));
							if (tile) score = -tile->dijkstraValue;
						}
						if (actor->priority == PRIORITY_MOST_HEALTH) score = enemy->hp;
						if (actor->priority == PRIORITY_MOST_ARMOR) score = enemy->armor;
						if (actor->priority == PRIORITY_MOST_SHIELD) score = enemy->shield;
						if (actor->priority == PRIORITY_LEAST_HEALTH) score = -enemy->hp;
						if (actor->priority == PRIORITY_LEAST_ARMOR) score = -enemy->armor;
						if (actor->priority == PRIORITY_LEAST_SHIELD) score = -enemy->shield;
						if (actor->priority == PRIORITY_FASTEST) score = enemy->movementSpeed;
						if (actor->priority == PRIORITY_SLOWEST) score = -enemy->movementSpeed;

						if (bestEnemy == NULL || bestEnemyScore < score) {
							bestEnemy = enemy;
							bestEnemyScore = score;
						}
					}
					target = bestEnemy;
					// if (enemiesInRangeNum > 0) target = enemiesInRange[enemiesInRangeNum-1];
					if (target) {
						actor->aimTarget = target->id;
						actor->aimRads = radsBetween(actor->position, target->position);
					}
				}

				bool isActive = true;
				if (data->phase != PHASE_WAVE && !towerIsActiveBetweenWaves) isActive = false;
				if (!target && towerCaresAboutTargets) isActive = false;

				actor->timeTillNextShot -= elapsed;
				if (isActive) {
					if (actor->timeTillNextShot < 0) {
						float rpm = getRpm(actor);
						actor->timeTillNextShot = 1.0/(rpm/60.0);

						float manaCost = info->mana;
						if (data->mana > manaCost) {
							data->mana -= manaCost;
							actor->timeSinceLastShot = 0;
							towerShouldFire = true;
							createCoreEvent(CORE_EVENT_SHOOT, actor);
							addShotStat(actor);
						}
					}

					if (actor->level < getMaxLevel(actor->type)) actor->xp += XP_PER_SEC * elapsed;
					if (actor->level > getMaxLevel(actor->type)) actor->level = getMaxLevel(actor->type);
				}

				{
					float minLevelPerc = 0;
					StartForEachUpgradeEffect;
					if (effect->type == UPGRADE_EFFECT_MIN_TOWER_LEVEL_PERC) {
						minLevelPerc = MaxNum(minLevelPerc, effect->value);
					}
					EndForEachUpgradeEffect;

					int minLevel = getMaxLevel(actor->type) * minLevelPerc;
					if (actor->level < minLevel) actor->xp += 50;

					if (actor->xp > maxXpPerLevels[actor->level]) {
						actor->level++;
						actor->xp = 0;
					}
				}
			}

			if (info->isTower || actor->type == ACTOR_MANA_SIPHON) {
				if (data->tool == TOOL_NONE || data->tool == TOOL_SELECTED) {
					if (isMouseClicked() && isHoveringActor(actor)) {
						data->prevTool = TOOL_NONE;
						data->tool = TOOL_SELECTED;
						data->selectedActorsNum = 0;
						data->selectedActors[data->selectedActorsNum++] = actor->id;
					}
				}
			}

			if (actor->type == ACTOR_BALLISTA) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}
			} else if (actor->type == ACTOR_MORTAR_TOWER) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}
			} else if (actor->type == ACTOR_TESLA_COIL) {
				if (towerShouldFire) {
					Circle circle = makeCircle(actor->position, getRange(actor, worldToTile(actor->position)));
					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(circle, &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						dealDamage(actor, enemy);
					}
				}
			} else if (actor->type == ACTOR_FROST_KEEP) {
				if (towerShouldFire) {
					float range = getRange(actor, worldToTile(actor->position));
					Vec2i towerTilePos = worldToTile(actor->position);
					Vec2i min = towerTilePos;
					min.x -= (range / TILE_SIZE);
					min.y -= (range / TILE_SIZE);

					Vec2i max = towerTilePos;
					max.x += (range / TILE_SIZE);
					max.y += (range / TILE_SIZE);

					Vec2i *possibleTiles = (Vec2i *)frameMalloc(sizeof(Vec2i) * ((max.x - min.x) + (max.y - min.y) + 1));
					int possibleTilesNum = 0;

					for (int y = min.y; y <= max.y; y++) {
						for (int x = min.x; x <= max.x; x++) {
							Vec2i tilePos = v2i(x, y);
							Rect rect = tileToWorldRect(tilePos);
							Tile *tile = getTileAt(tilePos);
							if (!tile) continue;
							if (!tileBlocksPathing(tile->type)) {
								possibleTiles[possibleTilesNum++] = tilePos;
							}
						}
					} 

					if (possibleTilesNum > 0) {
						Vec2i tilePos = possibleTiles[rndInt(0, possibleTilesNum-1)];
						Actor *bullet = createBullet(actor, NULL);
						bullet->position = tileToWorld(tilePos);
						bullet->position.x += rndFloat(-0.5, 0.5) * (float)TILE_SIZE;
						bullet->position.y += rndFloat(-0.5, 0.5) * (float)TILE_SIZE;
						bullet->position.y -= FROST_FALL_DISTANCE;
						bullet->position.y += TILE_SIZE/2;
					}
				}
			} else if (actor->type == ACTOR_FLAME_THROWER) {
				if (towerShouldFire) {
					float range = getRange(actor, worldToTile(actor->position));
					Tri2 tri = getAttackTri(actor->position, range, actor->aimRads, FLAME_RADS);

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(tri, &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						createDot(actor, enemy, DOT_BURN, getDamage(actor));
					}
				}
			} else if (actor->type == ACTOR_POISON_SPRAYER) {
				if (towerShouldFire) {
					float range = getRange(actor, worldToTile(actor->position));
					Tri2 tri = getAttackTri(actor->position, range, actor->aimRads, FLAME_RADS);

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(tri, &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						createDot(actor, enemy, DOT_POISON, getDamage(actor));
					}
				}
			} else if (actor->type == ACTOR_SHREDDER) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}
			} else if (actor->type == ACTOR_MANA_SIPHON) {
				int count = 0;
				Circle range = makeCircle(actor->position, TILE_SIZE);
				int actorsInRangeNum = 0;
				Actor **actorsInRange = getActorsInRange(range, &actorsInRangeNum, false);
				for (int i = 0; i < actorsInRangeNum; i++) {
					Actor *other = actorsInRange[i];
					if (other->type == ACTOR_MANA_CRYSTAL) count++;
				}

				core->manaToGain += (float)count * elapsed;
			} else if (actor->type == ACTOR_MANA_CRYSTAL) {
			} else if (actor->type >= ACTOR_ENEMY1 && actor->type <= ACTOR_ENEMY64) {
				core->enemiesAlive++;

				Vec2 dir = v2();
				{ // getFlowDirForRect(Rect rect);
					Rect rect = getRect(actor);
					Vec2 position = getPosition(rect);
					Vec2 size = getSize(rect);
					Vec2 corners[4] = {
						position + size*v2(0, 0),
						position + size*v2(0, 1),
						position + size*v2(1, 0),
						position + size*v2(1, 1)
					};

					for (int i = 0; i < 4; i++) {
						Vec2i tilePosition = worldToTile(corners[i]);
						Tile *tile = getTileAt(tilePosition);
						if (tile && !isZero(tile->flow)) dir += tile->flow;
					}
					dir = normalize(dir);
				}

				actor->accel = dir * (actor->movementSpeed * 1/60.0) * 5;

				Vec2i goal = v2i(CHUNK_SIZE/2, CHUNK_SIZE/2);
				Rect goalRect = tileToWorldRect(goal);
				if (overlaps(getRect(actor), goalRect)) {
					data->hp--;
					actor->markedForDeletion = true;
				}

				actor->hp = mathClamp(actor->hp, 0, info->maxHp);
				actor->armor = mathClamp(actor->armor, 0, info->maxArmor);
				actor->shield = mathClamp(actor->shield, 0, info->maxShield);
			} else if (actor->type == ACTOR_ARROW) {
				Actor *target = getActor(actor->bulletTarget);
				if (target) {
					actor->position = moveTowards(actor->position, target->position, info->bulletSpeed*timeScale);
					if (equal(actor->position, target->position)) {
						dealDamage(actor, target);
						actor->markedForDeletion = true;
					}
				} else {
					actor->markedForDeletion = true;
				}
			} else if (actor->type == ACTOR_MORTAR) {
				if (actor->time == 0) {
					Actor *target = getActor(actor->bulletTarget);
					if (target) actor->position = target->position;
				}

				float delayTime = 2 / getBulletSpeed(actor);
				float explodeRange = info->baseRange;
				if (actor->time >= delayTime) {
					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(makeCircle(actor->position, explodeRange), &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						dealDamage(actor, enemy);
					}

					actor->markedForDeletion = true;

					CoreEvent *event = createCoreEvent(CORE_EVENT_MORTAR_EXPLOSION, actor);
					event->ghostOrMortarPosition = actor->position;
				}
			} else if (actor->type == ACTOR_FROST) {
				int maxTime = 10;
				float fallSpeedPerSec = FROST_FALL_DISTANCE / (float)maxTime;
				actor->position.y += fallSpeedPerSec * elapsed;

				Circle range = makeCircle(actor->position, getRect(actor).width/2);

				int enemiesInRangeNum = 0;
				Actor **enemiesInRange = getActorsInRange(range, &enemiesInRangeNum, true);
				for (int i = 0; i < enemiesInRangeNum; i++) {
					Actor *enemy = enemiesInRange[i];
					if (overlaps(getRect(enemy), getRect(actor))) {
						enemy->slow += 6;
						actor->markedForDeletion = true;
					}
				}

				if (actor->time > maxTime) actor->markedForDeletion = true;
			} else if (actor->type == ACTOR_SAW) {
				float bulletSpeed = 5;
				if (!isZero(actor->bulletTargetPosition)) {
					if (distance(actor->position, actor->bulletTargetPosition) < bulletSpeed*5) {
						actor->position = actor->bulletTargetPosition;
						actor->bulletTargetPosition = v2();
					} else {
						Vec2 dir = normalize(actor->bulletTargetPosition - actor->position);
						actor->accel = dir * (bulletSpeed * elapsed) * 5;
					}
				} else {
					Vec2i tilePos = worldToTile(actor->position);
					Tile *tile = getTileAt(tilePos);

					Vec2i next = v2i();
					int nextCost = 0;
					for (int i = 0; i < 8; i++) {
						Vec2i neighbor = tilePos;
						if (i == 0) neighbor += v2i(-1, 0);
						if (i == 1) neighbor += v2i(1, 0);
						if (i == 2) neighbor += v2i(0, -1);
						if (i == 3) neighbor += v2i(0, 1);
						if (i == 4) neighbor += v2i(-1, -1);
						if (i == 5) neighbor += v2i(1, 1);
						if (i == 6) neighbor += v2i(1, -1);
						if (i == 7) neighbor += v2i(-1, 1);

						Tile *tile = getTileAt(neighbor);
						if (!tile) continue;
						if (tileBlocksPathing(tile->type)) continue;

						Chunk *chunk = worldToChunk(tileToWorld(neighbor));
						if (!chunk) continue;
						if (!chunk->visible) continue;

						if (nextCost < tile->dijkstraValue) {
							next = neighbor;
							nextCost = tile->dijkstraValue;
						}
					}

					if (!isZero(next)) {
						Vec2 nextWorld = tileToWorld(next);
						Vec2 dir = normalize(nextWorld - actor->position);
						actor->accel = dir * (bulletSpeed * elapsed) * 5;
					} else {
						logf("No next tile for saw??\n"); //@incomplete
					}
				}

				if (!actor->markedForDeletion) {
					Actor *saw = actor;

					float sawSize = 32;
					Circle circle = makeCircle(saw->position, sawSize);

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(circle, &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];

						bool canHit = true;
						for (int i = 0; i < saw->sawHitListNum; i++) {
							if (saw->sawHitList[i] == enemy->id) {
								canHit = false;
								break;
							}
						}

						if (!canHit) continue;

						int sawPierceMax = 6;

						StartForEachUpgradeEffect;
						if (effect->type == UPGRADE_EFFECT_EXTRA_SAW_PIERCE) sawPierceMax += effect->value;
						EndForEachUpgradeEffect;

						if (saw->sawHitListNum > sawPierceMax-1) saw->markedForDeletion = true;
						saw->sawHitList[saw->sawHitListNum++] = enemy->id;
						dealDamage(saw, enemy);

						Actor *tower = getActor(saw->parentTower);
						if (tower) createDot(tower, enemy, DOT_BLEED, getDamage(tower)*1.5);
					}
				}
			}

			Vec2 damping = v2(0.1, 0.1);
			actor->velo += (actor->accel - damping*actor->velo) * timeScale;
			actor->accel = v2();

			actor->position += actor->velo * timeScale;

			if (actor->hp <= 0 && info->maxHp > 0) actor->markedForDeletion = true;

			actor->time += elapsed;
		}
	} ///

	{ /// Update Dot
		data->timeTillNextDot -= elapsed;
		if (data->timeTillNextDot < 0) {
			data->timeTillNextDot = 0.25;
			data->dotQuadrant++;
			if (data->dotQuadrant == 4) data->dotQuadrant = 0;

			auto tickDot = [](Actor *actor, DotType dotType) {
				for (int i = 0; i < actor->dotsNum; i++) {
					Dot *dot = &actor->dots[i];
					if (dot->type == dotType) {
						float amountPerTick = 5;
						dot->ticks--;
						if (dot->type == DOT_POISON) {
							dealDamage(actor, dot->src, amountPerTick, 2, 0.5, 1);
						} else if (dot->type == DOT_BURN) {
							dealDamage(actor, dot->src, amountPerTick, 0.5, 2, 1);
						} else if (dot->type == DOT_BLEED) {
							dealDamage(actor, dot->src, amountPerTick, 0.5, 0.5, 2);
						} else {
							logf("Unknown DotType %d\n", dot->type);
						}

						if (dot->ticks <= 0) {
							arraySpliceIndex(actor->dots, actor->dotsNum, sizeof(Dot), i);
							actor->dotsNum--;
						}

						break;
					}
				}
			};

			int poisonQuadrantsAllowed = 1;
			int burnQuadrantsAllowed = 1;
			int bleedQuadrantsAllowed = 1;

			StartForEachUpgradeEffect;
			if (effect->type == UPGRADE_EFFECT_MORE_POISON_TICKS) poisonQuadrantsAllowed += effect->value;
			if (effect->type == UPGRADE_EFFECT_MORE_BURN_TICKS) burnQuadrantsAllowed += effect->value;
			if (effect->type == UPGRADE_EFFECT_MORE_BLEED_TICKS) bleedQuadrantsAllowed += effect->value;
			EndForEachUpgradeEffect;

			for (int i = 0; i < data->actorsNum; i++) {
				Actor *actor = &data->actors[i];
				if (data->dotQuadrant < poisonQuadrantsAllowed) tickDot(actor, DOT_POISON);
				if (data->dotQuadrant < burnQuadrantsAllowed) tickDot(actor, DOT_BURN);
				if (data->dotQuadrant < bleedQuadrantsAllowed) tickDot(actor, DOT_BLEED);
			}
		}
	} ///

	{ /// Post update actors
		for (int i = 0; i < data->actorsNum; i++) {
			Actor *actor = &data->actors[i];
			actor->timeSinceLastShot += elapsed;

			if (actor->markedForDeletion) {
				ActorTypeInfo *info = &core->actorTypeInfos[actor->type];
				if (info->isEnemy) {
					int moneyToGain = 0;
					if (actor->type == ACTOR_ENEMY1) {
						moneyToGain += 4; 
					} else {
						moneyToGain += info->enemySpawnStartingWave;
					}
					moneyToGain *= 2;

					StartForEachUpgradeEffect;
					if (effect->type == UPGRADE_EFFECT_EXTRA_MONEY) moneyToGain += effect->value;
					EndForEachUpgradeEffect;

					data->money += moneyToGain;
				}

				deinitActor(actor);
				arraySpliceIndex(data->actors, data->actorsNum, sizeof(Actor), i);
				data->actorsNum--;
				i--;
				continue;
			}
		}
	} ///

	{ /// Gain mana
		StartForEachUpgradeEffect;
		if (effect->type == UPGRADE_EFFECT_MANA_GAIN_MULTI) core->manaToGain *= effect->value;
		EndForEachUpgradeEffect;

		if (data->phase == PHASE_WAVE) data->mana += core->manaToGain;
		if (data->mana > data->maxMana) data->mana = data->maxMana;
	} ///

	if (data->phase == PHASE_WAVE) {
		{ /// Spawn enemies
			if (data->actorsToSpawnNum > 0) data->timeTillNextSpawn -= elapsed;
			if (data->timeTillNextSpawn <= 0) {
				data->timeTillNextSpawn += 1;

				Vec2 *spawnPoints = (Vec2 *)frameMalloc(sizeof(Vec2) * CHUNKS_MAX);
				int spawnPointsNum = 0;
				for (int i = 0; i < data->chunksNum; i++) {
					Chunk *chunk = &data->chunks[i];

					bool isPortal = false; //@copyPastedIsPortal
					if (chunk->connectionsNum == 1 && !isZero(chunk->position)) isPortal = true;

					if (!chunk->visible) continue;
					if (isPortal) {
						spawnPoints[spawnPointsNum++] = tileToWorld(chunkTileToWorldTile(chunk, v2i(CHUNK_SIZE/2, CHUNK_SIZE/2)));
					}
					for (int i = 0; i < chunk->connectionsNum; i++) {
						Chunk *newChunk = getChunkAt(chunk->connections[i]);
						Assert(newChunk);
						if (newChunk->visible) continue;

						Vec2 middle = (getCenter(chunk->rect) + getCenter(newChunk->rect)) / 2;
						spawnPoints[spawnPointsNum++] = middle;
					}
				}

				for (int i = 0; i < spawnPointsNum; i++) {
					ActorType toSpawn = data->actorsToSpawn[0];
					Actor *actor = createActor(toSpawn);
					actor->position = spawnPoints[i];

					memmove(&data->actorsToSpawn[0], &data->actorsToSpawn[1], sizeof(ActorType) * (data->actorsToSpawnNum-1));
					data->actorsToSpawnNum--;
					if (data->actorsToSpawnNum == 0) break;
				}
			}
		} ///

		if (data->phaseTime > 1 && core->enemiesAlive == 0 && data->actorsToSpawnNum == 0) { /// End wave
			data->phase = PHASE_RESULTS;
			core->presentedUpgradesNum = 0;

			int cardEveryXTurns = 2;
			if ((data->wave + cardEveryXTurns-1) % cardEveryXTurns == 0) {
				int *possible = (int *)frameMalloc(sizeof(int) * UPGRADES_MAX);
				int possibleNum = 0;
				for (int i = 0; i < core->upgradesNum; i++) {
					Upgrade *upgrade = &core->upgrades[i];
					if (hasUpgrade(upgrade->id)) continue;

					if (!hasPrereqs(upgrade->id)) continue;

					possible[possibleNum++] = upgrade->id;
				}

				int maxUpgradeCards = 4;
				StartForEachUpgradeEffect;
				if (effect->type == UPGRADE_EFFECT_EXTRA_CARDS) maxUpgradeCards += effect->value;
				EndForEachUpgradeEffect;

				core->presentedUpgradesNum = 0;
				for (int i = 0; i < maxUpgradeCards; i++) {
					if (possibleNum == 0) continue;
					int chosenIndex = rndInt(0, possibleNum-1);
					core->presentedUpgrades[core->presentedUpgradesNum++] = possible[chosenIndex];
					arraySpliceIndex(possible, possibleNum, sizeof(int), chosenIndex);
					possibleNum--;
				}
			}
		} ///
	}
	data->phaseTime += elapsed;

	{ /// Update tool
		if (data->prevTool != data->tool) {
			data->prevTool = data->tool;
			data->toolTime = 0;
		}

		if (data->tool == TOOL_NONE) {
			/// Nothing...
		} else if (data->tool == TOOL_BUILDING) {
			if (platform->rightMouseDown) data->tool = TOOL_NONE;

			ActorTypeInfo *info = &core->actorTypeInfos[data->actorToBuild];

			Vec2i tilePosition = getTileHovering();
			Vec2 center = getCenter(tileToWorldRect(tilePosition));

			{
				CoreEvent *event = createCoreEvent(CORE_EVENT_SHOW_GHOST);
				event->ghostActorType = data->actorToBuild;
				event->ghostOrMortarPosition = center;
			}

			Tile *tile = getTileAt(tilePosition);

			bool canBuild = true;
			Chunk *chunk = worldToChunk(center);
			if (!chunk) canBuild = false;
			if (canBuild && !chunk->visible) canBuild = false;
			if (canBuild && !tile) canBuild = false;
			if (canBuild && tile->type != TILE_GROUND) canBuild = false;

			for (int i = 0; i < data->actorsNum; i++) {
				Actor *other = &data->actors[i];
				if (equal(worldToTile(other->position), tilePosition)) canBuild = false;
			}

			if (canBuild && isMouseClicked()) {
				float price = info->price + info->priceMulti*core->actorTypeCounts[data->actorToBuild];
				if (data->money >= price) {
					data->money -= price;
					Actor *newTower = createActor(data->actorToBuild);
					newTower->position = center;
					addInvestmentStat(newTower, price);
					if (!keyPressed(KEY_SHIFT)) data->tool = TOOL_NONE;
				} else {
					infof("Not enough money\n");
				}
			}
		} else if (data->tool == TOOL_SELECTED) {
			if (isMouseClicked()) data->selectedActorsNum = 0;
			if (data->selectedActorsNum == 0) data->tool = TOOL_NONE;
		}

		data->toolTime += elapsed;
	} ///

	data->time += elapsed;
}

bool generateMap(MapGenMode mapGenMode) {
	data->chunksNum = 0;

	if (mapGenMode == MAP_GEN_NONE) {
		// Nothing...
	} else if (mapGenMode == MAP_GEN_RANDOM) {
		Chunk **chunksCouldExpand = (Chunk **)frameMalloc(sizeof(Chunk *) * CHUNKS_MAX);
		int chunksCouldExpandNum = 0;

		Chunk *chunk = createChunk(v2i(0, 0));

		int maxChunks = 45;

		for (;;) {
			if (data->chunksNum > maxChunks-1) {
				logf("Done generating.\n");
				break;
			}

			if (chunksCouldExpandNum == 0) {
				Chunk *randomChunk = &data->chunks[rndInt(0, data->chunksNum-1)];
				chunksCouldExpand[chunksCouldExpandNum++] = randomChunk;
				continue;
			}

			int expandIndex = rndInt(0, chunksCouldExpandNum-1);
			Chunk *chunkToExpand = chunksCouldExpand[expandIndex];

			int possiblePositionsNum = 0;
			Vec2i *possiblePositions = getAdjecentOpenChunkPositions(chunkToExpand->position, &possiblePositionsNum);

			if (possiblePositionsNum > 0) {
				int chosenIndex = rndInt(0, possiblePositionsNum-1);
				Vec2i position = possiblePositions[chosenIndex];
				Chunk *newChunk = createChunk(position);
				connect(newChunk, chunkToExpand);

				chunksCouldExpand[chunksCouldExpandNum++] = newChunk;
			}

			arraySpliceIndex(chunksCouldExpand, chunksCouldExpandNum, sizeof(Chunk *), expandIndex);
			chunksCouldExpandNum--;
		}

		for (int i = 0; i < data->chunksNum; i++) {
			Chunk *chunk = &data->chunks[i];
			carveChunkConnections(chunk);
		}
		data->chunks[0].visible = true;
	} else if (mapGenMode == MAP_GEN_CONTROLLED_SPLIT) {
		int maxChunks = 45;

		Chunk *chunk = createChunk(v2i(0, 0));

		Chunk *splitChunk = NULL;

		for (int i = 0; i < maxChunks; i++) {
			if (i == 10) splitChunk = chunk;
			if (i == 30) chunk = splitChunk;

			int possiblePositionsNum = 0;
			Vec2i *possiblePositions = getAdjecentOpenChunkPositions(chunk->position, &possiblePositionsNum);

			if (possiblePositionsNum == 0) {
				logf("Failed to generate\n");
				return false;
			}

			Vec2i nextPos = possiblePositions[rndInt(0, possiblePositionsNum-1)];
			Chunk *newChunk = createChunk(nextPos);
			connect(newChunk, chunk);
			chunk = newChunk;
		}

		for (int i = 0; i < data->chunksNum; i++) {
			Chunk *chunk = &data->chunks[i];
			carveChunkConnections(chunk);
		}
		data->chunks[0].visible = true;
	}

	for (int i = 0; i < data->chunksNum; i++) {
		Chunk *chunk = &data->chunks[i];
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				int tileIndex = y*CHUNK_SIZE + x;
				Tile *tile = &chunk->tiles[tileIndex];
				if (tile->type != TILE_GROUND) continue;
				if (rndPerc(0.01)) {
					Actor *actor = createActor(ACTOR_MANA_CRYSTAL);
					actor->position = tileToWorld(chunkTileToWorldTile(chunk, v2i(x, y)));
				}
			}
		}
	}

	return true;
}

void connect(Chunk *chunkA, Chunk *chunkB) {
	if (chunkA->connectionsNum > 4-1) logf("Too many connections!\n");
	if (chunkB->connectionsNum > 4-1) logf("Too many connections!\n");
	chunkA->connections[chunkA->connectionsNum++] = chunkB->position;
	chunkB->connections[chunkB->connectionsNum++] = chunkA->position;
}

Vec2i *getAdjecentOpenChunkPositions(Vec2i position, int *outPossiblePositionsNum) {
	Vec2i *possiblePositions = (Vec2i *)frameMalloc(sizeof(Vec2i) * 4);
	int possiblePositionsNum = 0;

	if (!getChunkAt(position + v2i(-1, 0))) possiblePositions[possiblePositionsNum++] = position + v2i(-1, 0);
	if (!getChunkAt(position + v2i(1, 0))) possiblePositions[possiblePositionsNum++] = position + v2i(1, 0);
	if (!getChunkAt(position + v2i(0, -1))) possiblePositions[possiblePositionsNum++] = position + v2i(0, -1);
	if (!getChunkAt(position + v2i(0, 1))) possiblePositions[possiblePositionsNum++] = position + v2i(0, 1);

	*outPossiblePositionsNum = possiblePositionsNum;
	return possiblePositions;
}

void carveChunkConnections(Chunk *chunk) {
	Vec2i centerTile = v2i(CHUNK_SIZE/2, CHUNK_SIZE/2);
	bool cutUp = false;
	bool cutDown = false;
	bool cutLeft = false;
	bool cutRight = false;
	for (int i = 0; i < chunk->connectionsNum; i++) {
		Vec2i connection = chunk->connections[i];
		cutUp = cutUp || connection.y < chunk->position.y;
		cutDown = cutDown || connection.y > chunk->position.y;
		cutLeft = cutLeft || connection.x < chunk->position.x;
		cutRight = cutRight || connection.x > chunk->position.x;
	}

	Vec2i *toMakeRoad = (Vec2i *)frameMalloc(sizeof(Vec2i) * (CHUNK_SIZE * CHUNK_SIZE));
	int toMakeRoadNum = 0;
	for (int i = 0; i < ceilf(CHUNK_SIZE/2.0); i++) {
		if (cutLeft) toMakeRoad[toMakeRoadNum++] = centerTile + v2i(-i, 0);
		if (cutRight) toMakeRoad[toMakeRoadNum++] = centerTile + v2i(i, 0);
		if (cutUp) toMakeRoad[toMakeRoadNum++] = centerTile + v2i(0, -i);
		if (cutDown) toMakeRoad[toMakeRoadNum++] = centerTile + v2i(0, i);
	}

	for (int i = 0; i < toMakeRoadNum; i++) {
		Vec2i pos = toMakeRoad[i];
		Tile *tile = &chunk->tiles[pos.y * CHUNK_SIZE + pos.x];
		tile->type = TILE_ROAD;
		tile->elevation = 0;
	}
}

void generateMapFields() {
	for (int i = 0; i < data->chunksNum; i++) {
		Chunk *chunk = &data->chunks[i];
		for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
			Tile *tile = &chunk->tiles[i];
			tile->flow = v2();
			tile->dijkstraValue = 0;
			tile->costSoFar = 0;
		}
	}

	/// Build dijkstra
	Vec2i goal = v2i(CHUNK_SIZE/2, CHUNK_SIZE/2);
	Tile *goalTile = getTileAt(goal);
	goalTile->type = TILE_HOME;

	{
		Allocator priorityQueueAllocator = {};
		priorityQueueAllocator.type = ALLOCATOR_FRAME;
		PriorityQueue *frontier = createPriorityQueue(sizeof(Vec2i), &priorityQueueAllocator);
		priorityQueuePush(frontier, &goal, 0);

		Tile *startTile = getTileAt(goal);
		startTile->costSoFar = 1;
		startTile->dijkstraValue = 1;

		while (frontier->length > 0) {
			Vec2i current;
			priorityQueueShift(frontier, &current);

			Tile *currentTile = getTileAt(current);

			for (int i = 0; i < 4; i++) {
				Vec2i neighbor = current;
				if (i == 0) neighbor += v2i(-1, 0);
				if (i == 1) neighbor += v2i(1, 0);
				if (i == 2) neighbor += v2i(0, -1);
				if (i == 3) neighbor += v2i(0, 1);

				Tile *neighborTile = getTileAt(neighbor);
				if (!neighborTile) continue;

				int cost = 1;
				if (tileBlocksPathing(neighborTile->type)) {
					cost += 9999;
				}

				int newCost = currentTile->costSoFar + cost;

				if (neighborTile->costSoFar == 0 || newCost < neighborTile->costSoFar) {
					neighborTile->dijkstraValue = newCost;
					neighborTile->costSoFar = newCost;

					priorityQueuePush(frontier, &neighbor, newCost);
				}
			}
		}

		destroyPriorityQueue(frontier);
	}

	/// Build Flow Field
	for (int i = 0; i < data->chunksNum; i++) {
		Chunk *chunk = &data->chunks[i];

		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				Vec2i localPos = v2i(x, y);
				Vec2i worldPos = chunkTileToWorldTile(chunk, localPos);

				Tile *currentTile = getTileAt(worldPos);

				if (currentTile->dijkstraValue <= 0 || currentTile->type == TILE_HOME) {
					currentTile->flow = v2();
					continue;
				}

				bool canUseDiagonals = false;

				//Go through all neighbours and find the one with the lowest distance
				Vec2i min = v2i();
				float minDist = 2;

				for (int i = 0; i < 8; i++) {
					if (!canUseDiagonals && i >= 4) continue;
					Vec2i neighbor = worldPos;
					if (i == 0) neighbor += v2i(-1, 0);
					if (i == 1) neighbor += v2i(1, 0);
					if (i == 2) neighbor += v2i(0, -1);
					if (i == 3) neighbor += v2i(0, 1);
					if (i == 4) neighbor += v2i(-1, -1);
					if (i == 5) neighbor += v2i(1, 1);
					if (i == 6) neighbor += v2i(1, -1);
					if (i == 7) neighbor += v2i(-1, 1);

					Tile *neighborTile = getTileAt(neighbor);
					if (!neighborTile) continue;
					if (neighborTile->dijkstraValue <= 0) continue;

					int dist = neighborTile->dijkstraValue - currentTile->dijkstraValue;

					if (dist < minDist) {
						min = neighbor;
						minDist = dist;
					}
				}

				//If we found a valid neighbour, point in its direction
				if (!isZero(min)) {
					currentTile->flow = vectorBetween(v2(worldPos), v2(min));
				} else {
					currentTile->flow = v2();
				}

			}
		}
	}
}

Chunk *createChunk(Vec2i position) {
	if (data->chunksNum > CHUNKS_MAX-1) {
		logf("Too many chunks!\n");
		return NULL;
	}

	Chunk *chunk = &data->chunks[data->chunksNum++];
	memset(chunk, 0, sizeof(Chunk));
	chunk->position = position;
	chunk->rect.width = CHUNK_SIZE * TILE_SIZE;
	chunk->rect.height = CHUNK_SIZE * TILE_SIZE;
	chunk->rect.x = chunk->position.x * chunk->rect.width;
	chunk->rect.y = chunk->position.y * chunk->rect.height;

	Vec2 baseHeightPerlinPos = v2(chunk->position + v2i(1000, 1000));
	if (baseHeightPerlinPos.x < 0 || baseHeightPerlinPos.y < 0) logf("baseHeightPerlinPos is %f %f\n", baseHeightPerlinPos.x, baseHeightPerlinPos.y);

	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			Tile *tile = &chunk->tiles[y * CHUNK_SIZE + x];
			tile->type = TILE_GROUND;

			Vec2 heightPerlinPos = baseHeightPerlinPos + (v2(x, y) / v2(CHUNK_SIZE, CHUNK_SIZE));
			float perlinValue = perlin2d(heightPerlinPos.x, heightPerlinPos.y);
			tile->perlinValue = perlinValue;
			tile->elevation = clampMap(perlinValue, 0.5, 1, 0, 3);
		}
	}

	return chunk;
}

void removeConnection(Chunk *chunk, Vec2i toRemove) {
	for (int i = 0; i < chunk->connectionsNum; i++) {
		if (equal(chunk->connections[i], toRemove)) {
			arraySpliceIndex(chunk->connections, chunk->connectionsNum, sizeof(Vec2i), i);
			chunk->connectionsNum--;
			return;
		}
	}

	logf("Failed to remove connection\n");
}

void removeChunk(Chunk *chunk) {
	for (int i = 0; i < chunk->connectionsNum; i++) {
		Chunk *otherChunk = getChunkAt(chunk->connections[i]);
		removeConnection(otherChunk, chunk->position);
	}
	for (int i = 0; i < data->chunksNum; i++) {
		if (&data->chunks[i] == chunk) {
			arraySpliceIndex(data->chunks, data->chunksNum, sizeof(Chunk), i);
			data->chunksNum--;
			return;
		}
	}

	logf("Failed to remove chunk\n");
}

Chunk *getChunkAt(Vec2i position) {
	for (int i = 0; i < data->chunksNum; i++) {
		Chunk *chunk = &data->chunks[i];
		if (equal(chunk->position, position)) return chunk;
	}

	return NULL;
}

Tile *getTileAt(Vec2i position) {
	Vec2i chunkPosition = {};
	while (position.x > CHUNK_SIZE-1) {
		position.x -= CHUNK_SIZE;
		chunkPosition.x++;
	}
	while (position.x < 0) {
		position.x += CHUNK_SIZE;
		chunkPosition.x--;
	}
	while (position.y > CHUNK_SIZE-1) {
		position.y -= CHUNK_SIZE;
		chunkPosition.y++;
	}
	while (position.y < 0) {
		position.y += CHUNK_SIZE;
		chunkPosition.y--;
	}

	Chunk *chunk = getChunkAt(chunkPosition);
	if (!chunk) return NULL;

	Tile *tile = &chunk->tiles[position.y * CHUNK_SIZE + position.x];
	return tile;
}

Vec2i chunkTileToWorldTile(Chunk *chunk, Vec2i tile) {
	tile.x += chunk->position.x * CHUNK_SIZE;
	tile.y += chunk->position.y * CHUNK_SIZE;
	return tile;
}

Chunk *worldToChunk(Vec2 position) {
	Vec2i chunkPosition = {};
	while (position.x > CHUNK_SIZE-1) {
		position.x -= CHUNK_SIZE * TILE_SIZE;
		chunkPosition.x++;
	}
	while (position.x < 0) {
		position.x += CHUNK_SIZE * TILE_SIZE;
		chunkPosition.x--;
	}
	while (position.y > CHUNK_SIZE-1) {
		position.y -= CHUNK_SIZE * TILE_SIZE;
		chunkPosition.y++;
	}
	while (position.y < 0) {
		position.y += CHUNK_SIZE * TILE_SIZE;
		chunkPosition.y--;
	}
	Chunk *chunk = getChunkAt(chunkPosition);
	return chunk;
}

Vec2i worldToTile(Vec2 position) {
	Vec2i tile;
	tile.x = floorf(position.x / TILE_SIZE);
	tile.y = floorf(position.y / TILE_SIZE);
	return tile;
}

Vec2 tileToWorld(Vec2i tile) {
	Vec2 position;
	position.x = tile.x * TILE_SIZE + TILE_SIZE/2;
	position.y = tile.y * TILE_SIZE + TILE_SIZE/2;
	return position;
}

Rect tileToWorldRect(Vec2i tile) {
	Rect rect;
	rect.x = tile.x * TILE_SIZE;
	rect.y = tile.y * TILE_SIZE;
	rect.width = TILE_SIZE;
	rect.height = TILE_SIZE;
	return rect;
}

bool tileBlocksPathing(TileType type) {
	if (type == TILE_GROUND) return true;
	return false;
}

Actor *createActor(ActorType type) {
	if (data->actorsNum > ACTORS_MAX-1) {
		Panic("Too many actors\n"); //@robustness
	}

	Actor *actor = &data->actors[data->actorsNum++];
	memset(actor, 0, sizeof(Actor));
	actor->type = type;
	initActor(actor);
	actor->id = ++data->nextActorId;

	ActorTypeInfo *info = &core->actorTypeInfos[actor->type];
	actor->hp = info->maxHp;
	actor->armor = info->maxArmor;
	actor->shield = info->maxShield;

	int statsMax = data->wave+2; //@copyPastedStatsMax
	actor->stats = (Stats *)zalloc(sizeof(Stats) * statsMax);
	actor->statsNum = statsMax;

	return actor;
}

void initActor(Actor *actor) {
	if (actor->type == ACTOR_SAW) {
		actor->sawHitList = (int *)zalloc(sizeof(int) * SAW_HIT_LIST_MAX);
	}
}
void deinitActor(Actor *actor) {
	if (actor->sawHitList) {
		free(actor->sawHitList);
		actor->sawHitList = NULL;
	}
	if (actor->dots) {
		free(actor->dots);
		actor->dots = NULL;
	}
}

Actor *getActor(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < data->actorsNum; i++) {
		Actor *actor = &data->actors[i];
		if (actor->id == id) return actor;
	}
	return NULL;
}

Rect getRect(Actor *actor) {
	Rect rect = makeCenteredRect(actor->position, v2(getInfo(actor)->size));
	return rect;
}

float getRange(ActorType actorType, Vec2i tilePos) {
	ActorTypeInfo *info = &core->actorTypeInfos[actorType];
	float range = info->baseRange;

	Tile *tile = getTileAt(tilePos);
	if (tile) range += tile->elevation * TILE_SIZE;

	StartForEachUpgradeEffect;
	if (effect->actorType == actorType && effect->type == UPGRADE_EFFECT_RANGE_MULTI) range *= effect->value;
	EndForEachUpgradeEffect;

	return range;
}

float getRange(Actor *actor, Vec2i tilePos) {
	float range = getRange(actor->type, tilePos);
	return range;
}

float getDamage(Actor *actor) {
	ActorTypeInfo *info = &core->actorTypeInfos[actor->type];
	float damage = info->damage;

	StartForEachUpgradeEffect;
	if (effect->actorType == actor->type && effect->type == UPGRADE_EFFECT_DAMAGE_MULTI) damage *= effect->value;
	EndForEachUpgradeEffect;

	damage *= 1 + actor->level*0.1;
	return damage;
}

float getRpm(Actor *actor) {
	ActorTypeInfo *info = &core->actorTypeInfos[actor->type];
	float rpm = info->rpm;

	StartForEachUpgradeEffect;
	if (effect->actorType == actor->type && effect->type == UPGRADE_EFFECT_RPM_MULTI) rpm *= effect->value;
	EndForEachUpgradeEffect;

	return rpm;
}

float getBulletSpeed(Actor *actor) {
	float bulletSpeed = getInfo(actor)->bulletSpeed;

	StartForEachUpgradeEffect;
	if (effect->actorType == actor->type && effect->type == UPGRADE_EFFECT_BULLET_SPEED_MULTI) bulletSpeed *= effect->value;
	EndForEachUpgradeEffect;

	return bulletSpeed;
}

int getMaxLevel(ActorType actorType) {
	return 2;
}

ActorTypeInfo *getInfo(Actor *actor) {
	return &core->actorTypeInfos[actor->type];
}

Actor *createBullet(Actor *src, Actor *target) {
	ActorType bulletType = ACTOR_ARROW;
	if (src->type == ACTOR_BALLISTA) bulletType = ACTOR_ARROW;
	else if (src->type == ACTOR_MORTAR_TOWER) bulletType = ACTOR_MORTAR;
	else if (src->type == ACTOR_FROST_KEEP) bulletType = ACTOR_FROST;
	else if (src->type == ACTOR_SHREDDER) bulletType = ACTOR_SAW;

	Actor *bullet = createActor(bulletType);
	bullet->position = src->position;
	if (target) bullet->bulletTarget = target->id;
	if (bullet->type == ACTOR_SAW && target) bullet->bulletTargetPosition = target->position;
	bullet->parentTower = src->id;
	return bullet;
}

void dealDamage(Actor *src, Actor *dest) {
	Actor *tower = NULL;

	ActorTypeInfo *srcInfo = &core->actorTypeInfos[src->type];
	if (srcInfo->isTower) {
		tower = src;
	} else {
		tower = getActor(src->parentTower);
	}
	if (!tower) {
		logf("Null damage source?\n");
		return;
	}

	float damage = getDamage(tower);

	ActorTypeInfo *towerInfo = &core->actorTypeInfos[tower->type];
	dealDamage(dest, tower->id, damage, towerInfo->shieldDamageMulti, towerInfo->armorDamageMulti, towerInfo->hpDamageMulti);

	createCoreEvent(CORE_EVENT_HIT, tower, dest);
}

void dealDamage(Actor *dest, int srcId, float amount, float shieldDamageMulti, float armorDamageMulti, float hpDamageMulti, bool noCoreEvent) {
	float damageLeft = amount;

	float toBreakShield = dest->shield / shieldDamageMulti;
	float shieldDamage = MinNum(damageLeft, toBreakShield*1.01);
	damageLeft -= shieldDamage;
	float shieldRealDamage = shieldDamage * shieldDamageMulti;
	dest->shield -= shieldRealDamage;

	float toBreakArmor = dest->armor / armorDamageMulti;
	float armorDamage = MinNum(damageLeft, toBreakArmor*1.01);
	damageLeft -= armorDamage;
	float armorRealDamage = armorDamage * armorDamageMulti;
	dest->armor -= armorRealDamage;

	float hpRealDamage = damageLeft * hpDamageMulti;
	dest->hp -= hpRealDamage;

	Actor *src = getActor(srcId);
	if (src) {
		addDamageStat(src, shieldRealDamage, armorRealDamage, hpRealDamage);
	} else {
		logf("No damage src for stats\n");
	}

	if (!noCoreEvent) {
		CoreEvent *event = createCoreEvent(CORE_EVENT_DAMAGE, dest);
		event->armorValue = armorRealDamage;
		event->shieldValue = shieldRealDamage;
		event->hpValue = hpRealDamage;
	}
}

void createDot(Actor *src, Actor *dest, DotType type, int ticks) {
	if (type == DOT_POISON) data->hasUsedPoison = true;
	if (type == DOT_BURN) data->hasUsedBurn = true;
	if (type == DOT_BLEED) data->hasUsedBleed = true;

	if (dest->dotsNum > dest->dotsMax-1) {
		dest->dots = (Dot *)resizeArray(dest->dots, sizeof(Dot), dest->dotsNum, dest->dotsMax+1);
		dest->dotsMax += 1;
	}
	Dot *dot = &dest->dots[dest->dotsNum++];
	memset(dot, 0, sizeof(Dot));
	dot->type = type;
	dot->src = src->id;
	dot->ticks = ticks;
}

int sumDotTicks(Actor *actor, DotType type) {
	int sum = 0;
	for (int i = 0; i < actor->dotsNum; i++) {
		Dot *dot = &actor->dots[i];
		if (dot->type == type) sum += dot->ticks;
	}
	return sum;
}

Upgrade *createUpgrade() {
	if (core->upgradesNum > UPGRADES_MAX-1) Panic("Too many upgrades");

	Upgrade *upgrade = &core->upgrades[core->upgradesNum++];
	memset(upgrade, 0, sizeof(Upgrade));
	upgrade->id = ++core->nextUpgradeId;
	return upgrade;
}

Upgrade *getUpgrade(int id) {
	for (int i = 0; i < core->upgradesNum; i++) {
		Upgrade *upgrade = &core->upgrades[i];
		if (upgrade->id == id) return upgrade;
	}

	logf("No upgrade with id %d\n", id);
	return NULL;
}

void unlockUpgrade(Upgrade *upgrade) {
	data->ownedUpgrades[data->ownedUpgradesNum++] = upgrade->id;

	for (int i = 0; i < upgrade->effectsNum; i++) {
		UpgradeEffect *effect = &upgrade->effects[i];
		if (effect->type == UPGRADE_EFFECT_GAIN_MONEY) data->money += effect->value;
	}
}

bool hasUpgrade(int id) {
	for (int i = 0; i < data->ownedUpgradesNum; i++) {
		if (data->ownedUpgrades[i] == id) return true;
	}

	return false;
}

bool hasUpgradeEffect(UpgradeEffectType effectType, ActorType actorType) {
	StartForEachUpgradeEffect;
	if (effect->type == effectType && effect->actorType == actorType) return true;
	EndForEachUpgradeEffect;

	return false;
}

bool hasPrereqs(int upgradeId) {
	Upgrade *upgrade = getUpgrade(upgradeId);
	for (int i = 0; i < upgrade->prereqUpgradesNum; i++) {
		if (!hasUpgrade(upgrade->prereqUpgrades[i])) return false;
	}
	for (int i = 0; i < upgrade->prereqEffectsNum; i++) {
		if (!hasUpgradeEffect(upgrade->prereqEffects[i], ACTOR_NONE)) return false;
	}

	for (int i = 0; i < upgrade->effectsNum; i++) {
		UpgradeEffect *effect = &upgrade->effects[i];
		if (effect->type == UPGRADE_EFFECT_MORE_POISON_TICKS && !data->hasUsedPoison) return false;
		if (effect->type == UPGRADE_EFFECT_MORE_BURN_TICKS && !data->hasUsedBurn) return false;
		if (effect->type == UPGRADE_EFFECT_MORE_BLEED_TICKS && !data->hasUsedBleed) return false;
	}
	return true;
}

Actor **getActorsInRange(Circle range, int *outNum, bool enemiesOnly) {
	Actor **enemiesInRange = (Actor **)frameMalloc(sizeof(Actor **) * data->actorsNum);
	int enemiesInRangeNum = 0;
	for (int i = 0; i < data->actorsNum; i++) {
		Actor *actor = &data->actors[i];
		ActorTypeInfo *otherInfo = &core->actorTypeInfos[actor->type];
		if (enemiesOnly && !otherInfo->isEnemy) continue;

		if (contains(getRect(actor), range)) enemiesInRange[enemiesInRangeNum++] = actor;
	}

	*outNum = enemiesInRangeNum;
	return enemiesInRange;
}

Actor **getActorsInRange(Tri2 range, int *outNum, bool enemiesOnly) {
	Actor **enemiesInRange = (Actor **)frameMalloc(sizeof(Actor **) * data->actorsNum);
	int enemiesInRangeNum = 0;
	for (int i = 0; i < data->actorsNum; i++) {
		Actor *actor = &data->actors[i];
		ActorTypeInfo *otherInfo = &core->actorTypeInfos[actor->type];
		if (enemiesOnly && !otherInfo->isEnemy) continue;

		if (overlaps(getRect(actor), range)) {
			enemiesInRange[enemiesInRangeNum++] = actor;
		}
	}

	*outNum = enemiesInRangeNum;
	return enemiesInRange;
}

void startNextWave() {
	data->phase = PHASE_WAVE;
	data->wave++;

	int actorsToSpawnNum = 0;
	ActorType *actorsToSpawn = generateWave(data->wave, &actorsToSpawnNum);
	memcpy(data->actorsToSpawn, actorsToSpawn, sizeof(Actor) * actorsToSpawnNum);
	data->actorsToSpawnNum = actorsToSpawnNum;

	data->startingActorsToSpawnNum = data->actorsToSpawnNum;
}

ActorType *generateWave(int wave, int *outActorTypesNum) {
	*outActorTypesNum = 0;

	ActorType *actorsToSpawn = (ActorType *)frameMalloc(sizeof(ActorType) * ACTORS_MAX);
	int actorsToSpawnNum = 0;

	ActorType *possibleActors = (ActorType *)frameMalloc(sizeof(ActorType) * ACTOR_TYPES_MAX);
	int possibleActorsNum = 0;

	for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
		ActorTypeInfo *info = &core->actorTypeInfos[i];
		if (info->enemySpawnStartingWave != 0 && info->enemySpawnStartingWave <= wave) {
			possibleActors[possibleActorsNum++] = (ActorType)i;
		}
	}

	if (possibleActorsNum == 0) {
		logf("No possible actor types to spawn???\n");
		possibleActors[possibleActorsNum++] = ACTOR_ENEMY1;
	}

	int maxEnemies = powf(wave, 1.2);
#if 1
	float *actorTypePerc = (float *)zalloc(sizeof(float) * possibleActorsNum);

	float total = 0;
	for (int i = 0; i < possibleActorsNum; i++) {
		total += possibleActorsNum - i;
		actorTypePerc[i] = possibleActorsNum - i;
	}

	for (int i = 0; i < possibleActorsNum; i++) actorTypePerc[i] /= total;

	int enemiesLeft = maxEnemies;
	for (int i = 0; i < possibleActorsNum; i++) {
		int toGive = actorTypePerc[i] * maxEnemies;
		if (toGive == 0) toGive = 1;

		enemiesLeft -= toGive;
		ActorType actorType = possibleActors[i];
		for (int i = 0; i < toGive; i++) {
			actorsToSpawn[actorsToSpawnNum++] = actorType;
		}

		if (enemiesLeft < 0) break;
	}

	if (enemiesLeft > 0) actorsToSpawn[actorsToSpawnNum++] = possibleActors[0];
#else
	for (int i = 0; i < maxEnemies; i++) {
		float value = rndFloat(0, 1);
		value = tweenEase(value, QUAD_IN);
		int index = roundf(lerp(0, possibleActorsNum-1, value)); // Not perfect distribution
		actorsToSpawn[actorsToSpawnNum++] = possibleActors[index];
	}
#endif

	auto qsortActorsToSpawn = [](const void *a, const void *b)->int {
		ActorTypeInfo *infoA = &core->actorTypeInfos[*(ActorType *) a];
		ActorTypeInfo *infoB = &core->actorTypeInfos[*(ActorType *) b];
		return infoA->enemySpawnStartingWave - infoB->enemySpawnStartingWave;
	};

	qsort(actorsToSpawn, actorsToSpawnNum, sizeof(ActorType), qsortActorsToSpawn);

	if (wave == 15) {
		actorsToSpawn[actorsToSpawnNum++] = ACTOR_ENEMY8;
	}

	if (wave == 25) {
		actorsToSpawn[actorsToSpawnNum++] = ACTOR_ENEMY14;
	}

	*outActorTypesNum = actorsToSpawnNum;
	return actorsToSpawn;
}

Tri2 getAttackTri(Vec2 start, float range, float angle, float deviation) {
	Vec2 end0 = start + radToVec2(angle - deviation) * range;
	Vec2 end1 = start + radToVec2(angle + deviation) * range;
	Tri2 tri = makeTri2(start, end0, end1);
	return tri;
}

void addShotStat(Actor *actor) {
	Stats *actorAll = &actor->stats[0];
	Stats *actorWave = &actor->stats[data->wave];
	Stats *typeAll = &data->actorTypeStats[actor->type][0];
	Stats *typeWave = &data->actorTypeStats[actor->type][data->wave];
	actorAll->shots++;
	actorWave->shots++;
	typeAll->shots++;
	typeWave->shots++;
}

void addDamageStat(Actor *actor, float shieldAmount, float armorAmount, float hpAmount) {
	Stats *actorAll = &actor->stats[0];
	Stats *actorWave = &actor->stats[data->wave];
	Stats *typeAll = &data->actorTypeStats[actor->type][0];
	Stats *typeWave = &data->actorTypeStats[actor->type][data->wave];
	actorAll->shieldDamage += shieldAmount;
	actorWave->shieldDamage += shieldAmount;
	typeAll->shieldDamage += shieldAmount;
	typeWave->shieldDamage += shieldAmount;
	actorAll->armorDamage += armorAmount;
	actorWave->armorDamage += armorAmount;
	typeAll->armorDamage += armorAmount;
	typeWave->armorDamage += armorAmount;
	actorAll->hpDamage += hpAmount;
	actorWave->hpDamage += hpAmount;
	typeAll->hpDamage += hpAmount;
	typeWave->hpDamage += hpAmount;
}

void addInvestmentStat(Actor *actor, int amount) {
	Stats *actorAll = &actor->stats[0];
	Stats *actorWave = &actor->stats[data->wave];
	Stats *typeAll = &data->actorTypeStats[actor->type][0];
	Stats *typeWave = &data->actorTypeStats[actor->type][data->wave];
	actorAll->investment += amount;
	actorWave->investment += amount;
	typeAll->investment += amount;
	typeWave->investment += amount;
}

void saveState(char *path) {
	logf("Saving...\n");
	DataStream *stream = newDataStream();

	writeU32(stream, 24); // version
	writeFloat(stream, lcgSeed);
	writeString(stream, data->campaignName);
	writeFloat(stream, data->time);
	writeVec2(stream, data->cameraPosition);
	writeFloat(stream, data->cameraZoom);
	writeU32(stream, data->mapGenMode);
	writeU32(stream, data->actorsNum);
	for (int i = 0; i < data->actorsNum; i++) writeActor(stream, &data->actors[i]);
	writeU32(stream, data->nextActorId);
	writeU32(stream, data->chunksNum);
	for (int i = 0; i < data->chunksNum; i++) writeChunk(stream, &data->chunks[i]);
	writeU32(stream, data->prevTool);
	writeU32(stream, data->tool);
	writeFloat(stream, data->toolTime);

	writeU32(stream, data->actorToBuild);
	writeU32(stream, data->hp);
	writeU32(stream, data->money);
	writeFloat(stream, data->mana);
	writeFloat(stream, data->maxMana);
	writeU32(stream, data->phase);
	writeU32(stream, data->prevPhase);
	writeU32(stream, data->phaseTime);
	writeU32(stream, data->wave);

	writeU32(stream, data->startingActorsToSpawnNum);
	writeU32(stream, data->actorsToSpawnNum);
	for (int i = 0; i < data->actorsToSpawnNum; i++) writeU32(stream, data->actorsToSpawn[i]);
	writeFloat(stream, data->timeTillNextSpawn);
	writeFloat(stream, data->timeTillNextDot);
	writeU32(stream, data->dotQuadrant);

	writeU32(stream, data->selectedActorsNum);
	for (int i = 0; i < data->selectedActorsNum; i++) writeU32(stream, data->selectedActors[i]);

	writeU32(stream, data->ownedUpgradesNum);
	for (int i = 0; i < data->ownedUpgradesNum; i++) writeU32(stream, data->ownedUpgrades[i]);

	writeU32(stream, ACTOR_TYPES_MAX);
	writeU32(stream, data->actorTypeStatsEach);
	for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
		Stats *actorStats = data->actorTypeStats[i];
		for (int i = 0; i < data->actorTypeStatsEach; i++) {
			writeStats(stream, actorStats[i]);
		}
	}

	writeU8(stream, data->hasUsedPoison);
	writeU8(stream, data->hasUsedBurn);
	writeU8(stream, data->hasUsedBleed);

	writeDataStream(path, stream);
	destroyDataStream(stream);
}

void writeStats(DataStream *stream, Stats stats) {
	writeU32(stream, stats.investment);
	writeU32(stream, stats.shots);
	writeFloat(stream, stats.shieldDamage);
	writeFloat(stream, stats.armorDamage);
	writeFloat(stream, stats.hpDamage);
}

void writeActor(DataStream *stream, Actor *actor) {
	writeU32(stream, actor->type);
	writeU32(stream, actor->id);
	writeVec2(stream, actor->position);
	writeVec2(stream, actor->velo);
	writeVec2(stream, actor->accel);
	writeFloat(stream, actor->aimRads);
	writeU32(stream, actor->aimTarget);
	writeFloat(stream, actor->hp);
	writeFloat(stream, actor->armor);
	writeFloat(stream, actor->shield);
	writeFloat(stream, actor->timeTillNextShot);
	writeU8(stream, actor->markedForDeletion);

	writeU32(stream, actor->dotsNum);
	for (int i = 0; i < actor->dotsNum; i++) {
		Dot *dot = &actor->dots[i];
		writeU32(stream, dot->type);
		writeU32(stream, dot->src);
		writeU32(stream, dot->ticks);
	}
	writeFloat(stream, actor->slow);
	writeFloat(stream, actor->movementSpeed);

	writeU32(stream, actor->statsNum);
	for (int i = 0; i < actor->statsNum; i++) writeStats(stream, actor->stats[i]);

	writeU32(stream, actor->priority);
	writeU32(stream, actor->bulletTarget);
	writeVec2(stream, actor->bulletTargetPosition);
	writeU32(stream, actor->parentTower);

	writeU32(stream, actor->sawHitListNum);
	for (int i = 0; i < actor->sawHitListNum; i++) writeU32(stream, actor->sawHitList[i]);

	writeU32(stream, actor->amountPaid);

	writeU32(stream, actor->level);
	writeFloat(stream, actor->xp);

	writeFloat(stream, actor->time);
}

void writeChunk(DataStream *stream, Chunk *chunk) {
	writeVec2i(stream, chunk->position);
	writeRect(stream, chunk->rect);
	for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) writeTile(stream, chunk->tiles[i]);
	for (int i = 0; i < 4; i++) writeVec2i(stream, chunk->connections[i]);
	writeU32(stream, chunk->connectionsNum);
	writeU8(stream, chunk->visible);
}

void writeTile(DataStream *stream, Tile tile) {
	writeU32(stream, tile.type);
	writeVec2(stream, tile.flow);
	writeU32(stream, tile.costSoFar);
	writeU32(stream, tile.dijkstraValue);
	writeU8(stream, tile.elevation);
}

void loadState(char *path) {
	for (int i = 0; i < data->actorsNum; i++) deinitActor(&data->actors[i]);

	DataStream *stream = loadDataStream(path);
	if (!stream) {
		logf("No state at %s\n", path);
		return;
	}

	int version = readU32(stream);
	lcgSeed = readU32(stream);
	readStringInto(stream, data->campaignName, CAMPAIGN_NAME_MAX_LEN);
	data->time = readFloat(stream);
	data->cameraPosition = readVec2(stream);
	data->cameraZoom = readFloat(stream);
	if (version >= 21) data->mapGenMode = (MapGenMode)readU32(stream);
	data->actorsNum = readU32(stream);
	for (int i = 0; i < data->actorsNum; i++) readActor(stream, &data->actors[i], version);
	data->nextActorId = readU32(stream);
	data->chunksNum = readU32(stream);
	for (int i = 0; i < data->chunksNum; i++) readChunk(stream, &data->chunks[i], version);
	data->prevTool = (Tool)readU32(stream);
	data->tool = (Tool)readU32(stream);
	data->toolTime = readFloat(stream);
	data->actorToBuild = (ActorType)readU32(stream);
	data->hp = readU32(stream);
	data->money = readU32(stream);
	data->mana = readFloat(stream);
	data->maxMana = readFloat(stream);
	data->phase = (Phase)readU32(stream);
	data->prevPhase = (Phase)readU32(stream);
	data->phaseTime = readU32(stream);
	data->wave = readU32(stream);
	if (version >= 22) data->startingActorsToSpawnNum = readU32(stream);
	data->actorsToSpawnNum = readU32(stream);
	for (int i = 0; i < data->actorsToSpawnNum; i++) data->actorsToSpawn[i] = (ActorType)readU32(stream);
	data->timeTillNextSpawn = readFloat(stream);
	if (version >= 23) data->timeTillNextDot = readFloat(stream);
	if (version >= 23) data->dotQuadrant = readU32(stream);

	data->selectedActorsNum = readU32(stream);
	for (int i = 0; i < data->selectedActorsNum; i++) data->selectedActors[i] = readU32(stream);

	data->ownedUpgradesNum = readU32(stream);
	for (int i = 0; i < data->ownedUpgradesNum; i++) data->ownedUpgrades[i] = readU32(stream);

	int actorTypeStatsToLoad = readU32(stream);
	if (actorTypeStatsToLoad > ACTOR_TYPES_MAX) {
		logf("Trimming loaded actors stats from %d to %d\n", actorTypeStatsToLoad, ACTOR_TYPES_MAX);
		actorTypeStatsToLoad = ACTOR_TYPES_MAX;
	}
	data->actorTypeStatsEach = readU32(stream);
	for (int i = 0; i < actorTypeStatsToLoad; i++) {
		if (data->actorTypeStats[i]) {
			free(data->actorTypeStats[i]);
			data->actorTypeStats[i] = NULL;
		}
		data->actorTypeStats[i] = (Stats *)zalloc(sizeof(Stats)*data->actorTypeStatsEach);
		Stats *actorStats = data->actorTypeStats[i];
		for (int i = 0; i < data->actorTypeStatsEach; i++) {
			readStats(stream, &actorStats[i], version);
		}
	}

	if (version >= 24) {
		data->hasUsedPoison = readU8(stream);
		data->hasUsedBurn = readU8(stream);
		data->hasUsedBleed = readU8(stream);
	}

	destroyDataStream(stream);

	generateMapFields();
}

void readStats(DataStream *stream, Stats *stats, int version) {
	stats->investment = readU32(stream);
	stats->shots = readU32(stream);
	stats->shieldDamage = readFloat(stream);
	stats->armorDamage = readFloat(stream);
	stats->hpDamage = readFloat(stream);
}

void readActor(DataStream *stream, Actor *actor, int version) {
	actor->type = (ActorType)readU32(stream);
	initActor(actor);
	actor->id = readU32(stream);
	actor->position = readVec2(stream);
	actor->velo = readVec2(stream);
	actor->accel = readVec2(stream);
	actor->aimRads = readFloat(stream);
	actor->aimTarget = readU32(stream);
	actor->hp = readFloat(stream);
	actor->armor = readFloat(stream);
	actor->shield = readFloat(stream);
	actor->timeTillNextShot = readFloat(stream);
	actor->markedForDeletion = readU8(stream);

	actor->dotsMax = actor->dotsNum = readU32(stream);
	actor->dots = (Dot *)zalloc(sizeof(Dot) * actor->dotsMax);
	for (int i = 0; i < actor->dotsNum; i++) {
		Dot *dot = &actor->dots[i];
		dot->type = (DotType)readU32(stream);
		dot->src = readU32(stream);
		dot->ticks = readU32(stream);
	}
	actor->slow = readFloat(stream);
	actor->movementSpeed = readFloat(stream);

	actor->statsNum = readU32(stream);
	actor->stats = (Stats *)zalloc(sizeof(Stats) * actor->statsNum);
	for (int i = 0; i < actor->statsNum; i++) readStats(stream, &actor->stats[i], version);

	actor->priority = (Priority)readU32(stream);
	actor->bulletTarget = readU32(stream);
	actor->bulletTargetPosition = readVec2(stream);
	actor->parentTower = readU32(stream);

	actor->sawHitListNum = readU32(stream);
	for (int i = 0; i < actor->sawHitListNum; i++) actor->sawHitList[i] = readU32(stream);

	actor->amountPaid = readU32(stream);

	actor->level = readU32(stream);
	actor->xp = readFloat(stream);

	actor->time = readFloat(stream);
}

void readChunk(DataStream *stream, Chunk *chunk, int version) {
	chunk->position = readVec2i(stream);
	chunk->rect = readRect(stream);
	for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) chunk->tiles[i] = readTile(stream, version);
	for (int i = 0; i < 4; i++) chunk->connections[i] = readVec2i(stream);
	chunk->connectionsNum = readU32(stream);
	chunk->visible = readU8(stream);
}

Tile readTile(DataStream *stream, int version) {
	Tile tile = {};
	tile.type = (TileType)readU32(stream);
	tile.flow = readVec2(stream);
	tile.costSoFar = readU32(stream);
	tile.dijkstraValue = readU32(stream);
	tile.elevation = readU8(stream);
	return tile;
}

#endif
