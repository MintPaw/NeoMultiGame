// Add a way to see your upgrades
// dps stats: per tower, per tower type (per wave/per game)
// Tower building ghost should be a CoreEvent
// GameData should contain presentedUpgrades

// Upgrade ideas:
// Saws go through X extra enemies
// Gaining the ability to slow down time more

#define FROST_FALL_DISTANCE 64

#define POISON_COLOR 0xFF6B4876
#define BURN_COLOR 0xFFDCAB2C
#define BLEED_COLOR 0xFF770000
#define SHIELD_COLOR 0xFF718691
#define HP_COLOR 0xFF00FF00
#define ARMOR_COLOR 0xFFFFD66E

#define XP_PER_SEC 10

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

	float hp;
	float armor;
	float shield;

	float timeTillNextShot;
	float timeSinceLastShot;
	bool markedForDeletion;

	float slow;
	float poison;
	float burn;
	float bleed;
	float movementSpeed;

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
	u8 height;
	float perlinValue;
};

#define TILE_SIZE 64
struct Chunk {
	Vec2i position;
	Rect rect;
#define CHUNK_SIZE 7
	Tile tiles[CHUNK_SIZE*CHUNK_SIZE];

	Vec2i connections[4];
	int connectionsNum;

	bool visible;
};

struct World {
#define ACTORS_MAX 65535
	Actor actors[ACTORS_MAX];
	int actorsNum;
	int nextActorId;

#define CHUNKS_MAX 512
	Chunk chunks[CHUNKS_MAX];
	int chunksNum;
};

enum Tool {
	TOOL_NONE,
	TOOL_BUILDING,
	TOOL_SELECTED,
};

enum UpgradeEffectType {
	UPGRADE_EFFECT_UNLOCK,
	UPGRADE_EFFECT_DAMAGE_MULTI,
	UPGRADE_EFFECT_RANGE_MULTI,
	UPGRADE_EFFECT_RPM_MULTI,
	UPGRADE_EFFECT_EXTRA_CARDS,
	UPGRADE_EFFECT_EXTRA_MONEY,
	UPGRADE_EFFECT_MANA_GAIN_MULTI,
	UPGRADE_EFFECT_TYPES_MAX,
};
// char *upgradeEffectTypeStrings[] = {
// 	"Unlock",
// 	"Damage",
// 	"Range",
// 	"Rpm",
// 	"Upgrade card",
// 	"Mana",
// };

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
	int prereqs[UPGRADE_PREREQS_MAX];
	int prereqsNum;
};

enum CoreEventType {
	CORE_EVENT_DAMAGE,
	CORE_EVENT_SHOW_GHOST,
};
struct CoreEvent {
	CoreEventType type;
	Vec2 position;
	float floatValue;

	float shieldValue;
	float armorValue;
	float hpValue;

	ActorType actorType;
};

enum EffectType {
	EFFECT_DEFAULT_CORE_EVENT,
};
struct Effect {
	EffectType type;
	Vec2 position;
	float time;
	float floatValue;

	CoreEvent coreEvent;
};

struct GameData {
	World *world;
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

	int wave;
	float waveTime;
	bool playingWave;

	ActorType actorsToSpawn[ACTORS_MAX];
	int actorsToSpawnNum;
	float timeTillNextSpawn;

#define SELECTED_ACTORS_MAX 2048
	int selectedActors[SELECTED_ACTORS_MAX];
	int selectedActorsNum;

#define UPGRADES_MAX 256
	int ownedUpgrades[UPGRADES_MAX];
	int ownedUpgradesNum;
};

struct Game {
	Font *defaultFont;

	bool inEditor;
	float timeScale;
	Vec2 size;
	Vec2 mouse;

	bool shouldReset;

	ActorTypeInfo actorTypeInfos[ACTOR_TYPES_MAX];
	int actorTypeCounts[ACTOR_TYPES_MAX];

	Upgrade upgrades[UPGRADES_MAX];
	int upgradesNum;
	int nextUpgradeId;

	float manaToGain;

	GameData data;

	bool is2d;

	int presentedUpgrades[UPGRADES_MAX];
	int presentedUpgradesNum;

#define CORE_EVENTS_MAX 1024
	CoreEvent coreEvents[CORE_EVENTS_MAX];
	int coreEventsNum;

#define EFFECTS_MAX 1024
	Effect effects[EFFECTS_MAX];
	int effectsNum;

	Matrix3 cameraMatrix;

	/// Editor/debug
	bool debugShowFrameTimes;
	bool debugShowDijkstraValues;
	bool debugShowFlowFieldValues;
	bool debugShowPerlinValues;
	bool debugDrawChunkLines;
	bool debugDrawTileLines;
	bool debugShowActorVelo;

	char debugNewSaveStateName[PATH_MAX_LEN];
};

Game *game = NULL;
GameData *data = NULL;

void runGame();
void updateGame();

bool isMouseClicked();
Vec2i getTileHovering();
bool isHoveringActor(Actor *actor);
#include "tower2GameCore.cpp"
void drawGame(float elapsed);
Effect *createEffect(EffectType type);

void updateAndDrawOverlay(float elapsed);

/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
#if !defined(FALLOW_INTERNAL) // This needs to be a macro
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#else
	// if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/ZooBound/zooBoundGameAssets");
	// if (directoryExists("O:/Dropbox")) strcpy(projectAssetDir, "O:/Dropbox/ZooBound/zooBoundGameAssets");
#endif
#endif

	initFileOperations();

	Vec2 res = v2(1600, 900);

	initPlatform(res.x, res.y, "tower2");
	platform->sleepWait = true;
	initAudio();
	initRenderer(res.x, res.y);
	initTextureSystem();
	initFonts();
	nguiInit();

	logf("Game+World is %.1fmb\n", (sizeof(World)+sizeof(Game)) / (float)(Megabytes(1)));

	platformUpdateLoop(updateGame);
}

void updateGame() {
	bool isFirstStart = false;
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		data = &game->data;
		game->defaultFont = createFont("assets/common/arial.ttf", 80);

		// if (ArrayLength(upgradeEffectTypeStrings) != UPGRADE_EFFECT_TYPES_MAX) Panic("Upgrade type string mismatch\n");

		{ /// Setup actor type infos
			for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
				ActorTypeInfo *info = &game->actorTypeInfos[i];
				sprintf(info->name, "Actor %d", i);
			}

			for (int i = ACTOR_BALLISTA; i <= ACTOR_PARTICLE_CANNON; i++) {
				ActorTypeInfo *info = &game->actorTypeInfos[i];
				info->isTower = true;
			}

			for (int i = ACTOR_ENEMY1; i <= ACTOR_ENEMY64; i++) {
				ActorTypeInfo *info = &game->actorTypeInfos[i];
				info->isEnemy = true;
			}

			ActorTypeInfo *info = NULL;

			info = &game->actorTypeInfos[ACTOR_BALLISTA];
			strncpy(info->name, "Ballista", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 5;
			info->hpDamageMulti = 10;
			info->armorDamageMulti = 5;
			info->shieldDamageMulti = 5;
			info->baseRange = 5 * TILE_SIZE;
			info->rpm = 20;
			info->mana = 0;
			info->price = 10;
			info->priceMulti = 15;

			info = &game->actorTypeInfos[ACTOR_MORTAR_TOWER];
			strncpy(info->name, "Mortar", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 10;
			info->hpDamageMulti = 10;
			info->armorDamageMulti = 15;
			info->shieldDamageMulti = 5;
			info->baseRange = 10 * TILE_SIZE;
			info->rpm = 10;
			info->mana = 0;
			info->price = 200;
			info->priceMulti = 75;

			info = &game->actorTypeInfos[ACTOR_TESLA_COIL];
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

			info = &game->actorTypeInfos[ACTOR_FROST_KEEP];
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

			info = &game->actorTypeInfos[ACTOR_FLAME_THROWER];
			strncpy(info->name, "Flame Thrower", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 5;
			info->hpDamageMulti = 6;
			info->armorDamageMulti = 9;
			info->shieldDamageMulti = 3;
			info->baseRange = 4 * TILE_SIZE;
			info->rpm = 60;
			info->mana = 1;
			info->price = 300;
			info->priceMulti = 75;

			info = &game->actorTypeInfos[ACTOR_POISON_SPRAYER];
			strncpy(info->name, "Poison Sprayer", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 5;
			info->hpDamageMulti = 6;
			info->armorDamageMulti = 3;
			info->shieldDamageMulti = 9;
			info->baseRange = 4 * TILE_SIZE;
			info->rpm = 60;
			info->mana = 1;
			info->price = 300;
			info->priceMulti = 75;

			info = &game->actorTypeInfos[ACTOR_SHREDDER];
			strncpy(info->name, "Shredder", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 10;
			info->hpDamageMulti = 20;
			info->armorDamageMulti = 10;
			info->shieldDamageMulti = 10;
			info->baseRange = 5 * TILE_SIZE;
			info->rpm = 5;
			info->price = 500;
			info->priceMulti = 100;

			info = &game->actorTypeInfos[ACTOR_ENCAMPENT];
			strncpy(info->name, "Encampent", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 20;
			info->hpDamageMulti = 10;
			info->armorDamageMulti = 15;
			info->shieldDamageMulti = 5;
			info->baseRange = 2 * TILE_SIZE;
			info->rpm = 5;
			info->price = 500;
			info->priceMulti = 100;

			info = &game->actorTypeInfos[ACTOR_LOOKOUT];
			strncpy(info->name, "Lookout", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 1;
			info->hpDamageMulti = 2;
			info->armorDamageMulti = 1;
			info->shieldDamageMulti = 3;
			info->baseRange = 8 * TILE_SIZE;
			info->rpm = 0;
			info->price = 500;
			info->priceMulti = 100;

			info = &game->actorTypeInfos[ACTOR_RADAR];
			strncpy(info->name, "Radar", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 20;
			info->hpDamageMulti = 20;
			info->armorDamageMulti = 10;
			info->shieldDamageMulti = 10;
			info->baseRange = 30 * TILE_SIZE;
			info->rpm = 700;
			info->price = 1000;
			info->priceMulti = 250;

			info = &game->actorTypeInfos[ACTOR_OBELISK];
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

			info = &game->actorTypeInfos[ACTOR_PARTICLE_CANNON];
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

			info = &game->actorTypeInfos[ACTOR_MANA_SIPHON];
			strncpy(info->name, "Mana Siphon", ACTOR_TYPE_NAME_MAX_LEN);
			info->price = 100;
			info->priceMulti = 10;

			info = &game->actorTypeInfos[ACTOR_ENEMY1];
			info->enemySpawnStartingWave = 1;
			info->movementSpeed = 2;
			info->maxHp = 100;

			info = &game->actorTypeInfos[ACTOR_ENEMY2];
			info->enemySpawnStartingWave = 3;
			info->movementSpeed = 2;
			info->maxHp = 300;

			info = &game->actorTypeInfos[ACTOR_ENEMY3];
			info->enemySpawnStartingWave = 5;
			info->movementSpeed = 1.75;
			info->maxHp = 400;
			info->maxArmor = 200;

			info = &game->actorTypeInfos[ACTOR_ENEMY4];
			info->enemySpawnStartingWave = 7;
			info->movementSpeed = 1.75;
			info->maxHp = 800;
			info->hpGainPerSec = 25;

			info = &game->actorTypeInfos[ACTOR_ENEMY5];
			info->enemySpawnStartingWave = 9;
			info->movementSpeed = 1.75;
			info->maxHp = 400;
			info->maxArmor = 600;

			info = &game->actorTypeInfos[ACTOR_ENEMY6];
			info->enemySpawnStartingWave = 11;
			info->movementSpeed = 1;
			info->maxHp = 300;
			info->maxArmor = 1500;

			info = &game->actorTypeInfos[ACTOR_ENEMY7];
			info->enemySpawnStartingWave = 13;
			info->movementSpeed = 1.25;
			info->maxHp = 2000;

			info = &game->actorTypeInfos[ACTOR_ENEMY8];
			info->enemySpawnStartingWave = 15;
			info->movementSpeed = 1;
			info->maxHp = 20000;

			info = &game->actorTypeInfos[ACTOR_ARROW];
			info->bulletSpeed = 20;

			info = &game->actorTypeInfos[ACTOR_MORTAR];
			info->bulletSpeed = 0.5;
			info->baseRange = 2 * TILE_SIZE;
		} ///

		{ /// Setup upgrades
			auto createUpgrade = []() {
				if (game->upgradesNum > UPGRADES_MAX-1) Panic("Too many upgrades");

				Upgrade *upgrade = &game->upgrades[game->upgradesNum++];
				memset(upgrade, 0, sizeof(Upgrade));
				upgrade->id = ++game->nextUpgradeId;
				return upgrade;
			};

			Upgrade *upgrade = NULL;

			ActorType actorsCouldUpgrade[] = {
				ACTOR_BALLISTA, ACTOR_MORTAR_TOWER, ACTOR_TESLA_COIL, ACTOR_FROST_KEEP, ACTOR_FLAME_THROWER, ACTOR_POISON_SPRAYER, ACTOR_SHREDDER,
			};
			// ACTOR_ENCAMPENT, ACTOR_LOOKOUT, ACTOR_RADAR, ACTOR_OBELISK, ACTOR_PARTICLE_CANNON,

			game->upgradesNum = 0;
			game->nextUpgradeId = 0;
			for (int i = 0; i < ArrayLength(actorsCouldUpgrade); i++) {
				ActorType actorType = actorsCouldUpgrade[i];

				Upgrade *unlockUpgrade = NULL;
				if (actorType != ACTOR_BALLISTA) {
					Upgrade *upgrade = createUpgrade();
					UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
					effect->type = UPGRADE_EFFECT_UNLOCK;
					effect->actorType = actorType;
					unlockUpgrade = upgrade;
				}

				Upgrade *prevUpgrade = NULL;

				prevUpgrade = unlockUpgrade;
				for (int i = 0; i < 3; i++) {
					Upgrade *upgrade = createUpgrade();
					UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
					effect->type = UPGRADE_EFFECT_DAMAGE_MULTI;
					effect->actorType = actorType;
					effect->value = 1 + (0.1 * (i+1));
					if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
					prevUpgrade = upgrade;
				}

				prevUpgrade = unlockUpgrade;
				for (int i = 0; i < 3; i++) {
					Upgrade *upgrade = createUpgrade();
					UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
					effect->type = UPGRADE_EFFECT_RANGE_MULTI;
					effect->actorType = actorType;
					effect->value = 1 + (0.1 * (i+1));
					if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
					prevUpgrade = upgrade;
				}

				prevUpgrade = unlockUpgrade;
				for (int i = 0; i < 3; i++) {
					Upgrade *upgrade = createUpgrade();
					UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
					effect->type = UPGRADE_EFFECT_RPM_MULTI;
					effect->actorType = actorType;
					effect->value = 1 + (0.1 * (i+1));
					if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
					prevUpgrade = upgrade;
				}
			}

			{
				Upgrade *prevUpgrade = NULL;
				for (int i = 0; i < 3; i++) {
					Upgrade *upgrade = createUpgrade();
					UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
					effect->type = UPGRADE_EFFECT_EXTRA_CARDS;
					effect->value = 1;
					if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
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
					if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
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
					if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
					prevUpgrade = upgrade;
				}
			}

			{
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_UNLOCK;
				effect->actorType = ACTOR_MANA_SIPHON;
			}
		} ///

		game->timeScale = 1;
		game->is2d = false;

		maximizeWindow();

		rndInt(0, 3); // Burn an rnd seed???
		isFirstStart = true;
	}

	if (game->shouldReset) {
		game->shouldReset = false;

		World *world = data->world;
		for (int i = 0; i < world->actorsNum; i++) deinitActor(&world->actors[i]);
		free(world);
		data->world = NULL;
		memset(data, 0, sizeof(GameData));
	}

	if (!data->world) {
		data->world = (World *)zalloc(sizeof(World));
		World *world = data->world;

		{ /// Generate map
			Chunk **chunksCouldExpand = (Chunk **)frameMalloc(sizeof(Chunk) * CHUNKS_MAX);
			int chunksCouldExpandNum = 0;

			Chunk *chunk = createChunk(v2i(0, 0));

			int maxChunks = 45;

			for (;;) {
				if (world->chunksNum > maxChunks-1) {
					logf("Done.\n");
					break;
				}

				if (chunksCouldExpandNum == 0) {
					Chunk *randomChunk = &world->chunks[rndInt(0, world->chunksNum-1)];
					chunksCouldExpand[chunksCouldExpandNum++] = randomChunk;
					continue;
				}

				int expandIndex = rndInt(0, chunksCouldExpandNum-1);
				Chunk *chunkToExpand = chunksCouldExpand[expandIndex];

				Vec2i possiblePositions[4];
				int possiblePositionsNum = 0;
				possiblePositions[possiblePositionsNum++] = chunkToExpand->position + v2i(-1, 0);
				possiblePositions[possiblePositionsNum++] = chunkToExpand->position + v2i(1, 0);
				possiblePositions[possiblePositionsNum++] = chunkToExpand->position + v2i(0, -1);
				possiblePositions[possiblePositionsNum++] = chunkToExpand->position + v2i(0, 1);
				for (int i = 0; i < possiblePositionsNum; i++) {
					Vec2i possiblePosition = possiblePositions[i];
					if (getChunkAt(possiblePosition)) {
						arraySpliceIndex(possiblePositions, possiblePositionsNum, sizeof(Vec2i), i);
						possiblePositionsNum--;
						i--;
						continue;
					}
				}

				if (possiblePositionsNum > 0) {
					int chosenIndex = rndInt(0, possiblePositionsNum-1);
					Vec2i position = possiblePositions[chosenIndex];
					Chunk *newChunk = createChunk(position);
					newChunk->connections[newChunk->connectionsNum++] = chunkToExpand->position;
					chunkToExpand->connections[chunkToExpand->connectionsNum++] = newChunk->position;

					chunksCouldExpand[chunksCouldExpandNum++] = newChunk;
				}

				arraySpliceIndex(chunksCouldExpand, chunksCouldExpandNum, sizeof(Vec2i), expandIndex);
				chunksCouldExpandNum--;
			}

			for (int i = 0; i < world->chunksNum; i++) {
				Chunk *chunk = &world->chunks[i];

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
					tile->height = 0;
				}
			}

			world->chunks[0].visible = true;

			for (int i = 0; i < world->chunksNum; i++) {
				Chunk *chunk = &world->chunks[i];
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
		} ///

		data->hp = 10;
		data->money = 1000;
		data->mana = 100;
		data->maxMana = 100;
		generateMapFields();

		data->cameraZoom = 1;

		if (isFirstStart) loadState("assets/states/autosave.save_state");
	}

	game->size = v2(platform->windowSize);

	ngui->mouse = platform->mouse;
	ngui->screenSize = game->size;

	int stepsToTake = 1;
	float elapsed = platform->elapsed;

	if (game->timeScale > 1) {
		stepsToTake = game->timeScale;
	} else if (game->timeScale < 1) {
		elapsed *= game->timeScale;
	}

	clearRenderer();

	for (int i = 0; i < stepsToTake; i++) {
		stepGame(elapsed);
		drawGame(elapsed);
	}

	updateAndDrawOverlay(elapsed*stepsToTake);

	guiDraw();
}

bool isMouseClicked() {
	bool ret = platform->mouseJustDown;
	if (ngui->mouseHoveringThisFrame) ret = false;
	if (ngui->mouseHoveringLastFrame) ret = false;
	if (ngui->mouseJustDownThisFrame) ret = false;
	if (data->prevTool != data->tool) ret = false;
	if (data->toolTime < 0.05) ret = false;
	return ret;
}

Vec2i getTileHovering() {
	return worldToTile(game->mouse);
}

bool isHoveringActor(Actor *actor) {
	return contains(getRect(actor), game->mouse);
}

void drawGame(float elapsed) {
	World *world = data->world;

	{ /// Iterate CoreEvents
		for (int i = 0; i < game->coreEventsNum; i++) {
			CoreEvent *event = &game->coreEvents[i];
			if (event->type == CORE_EVENT_DAMAGE || event->type == CORE_EVENT_SHOW_GHOST) {
				Effect *effect = createEffect(EFFECT_DEFAULT_CORE_EVENT);
				effect->coreEvent = *event;
				effect->position = event->position;
			}
		}
	} ///

	if (game->is2d) {
		Rect screenRect = {};
		{ /// Setup camera
			game->cameraMatrix = mat3();
			game->cameraMatrix.TRANSLATE(game->size/2);
			game->cameraMatrix.SCALE(data->cameraZoom);
			game->cameraMatrix.TRANSLATE(-data->cameraPosition);

			game->mouse = game->cameraMatrix.invert() * platform->mouse;

			screenRect.width = game->size.x / data->cameraZoom;
			screenRect.height = game->size.y / data->cameraZoom;
			screenRect.x = data->cameraPosition.x - screenRect.width/2;
			screenRect.y = data->cameraPosition.y - screenRect.height/2;
		} ///

		pushCamera2d(game->cameraMatrix);

		{ /// Draw map
			for (int i = 0; i < world->chunksNum; i++) {
				Chunk *chunk = &world->chunks[i];
				if (!overlaps(screenRect, chunk->rect)) continue;
				if (!chunk->visible) continue;

				for (int y = 0; y < CHUNK_SIZE; y++) {
					for (int x = 0; x < CHUNK_SIZE; x++) {
						int tileIndex = y*CHUNK_SIZE + x;
						Rect rect = {};
						rect.x = x*TILE_SIZE + chunk->rect.x;
						rect.y = y*TILE_SIZE + chunk->rect.y;
						rect.width = TILE_SIZE;
						rect.height = TILE_SIZE;
						Tile *tile = &chunk->tiles[tileIndex];

						int color = 0x00000000;

						if (tile->type == TILE_HOME) color = 0xFFFFF333;
						if (tile->type == TILE_GROUND) color = 0xFF017301;
						if (tile->type == TILE_ROAD) color = 0xFF966F02;

						float heightShadePerc = clampMap(tile->height, 0, 3, 0, 0.25);
						// float heightShadePerc = clampMap(tile->height, 0, 255, 0, 1);
						color = lerpColor(color, 0xFF000000, heightShadePerc);

						drawRect(rect, color);

						if (game->debugShowDijkstraValues) {
							DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
							drawTextInRect(frameSprintf("%d", tile->dijkstraValue), props, rect);
						}

						if (game->debugShowFlowFieldValues) {
							if (!isZero(tile->flow)) {
								Vec2 start = getCenter(rect);
								Vec2 end = start + tile->flow*TILE_SIZE/2;
								drawLine(start, end, 4, 0xFFFF0000);
							}
						}

						if (game->debugShowPerlinValues) {
							DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
							drawTextInRect(frameSprintf("%.02f", tile->perlinValue), props, rect);
						}

						if (game->debugDrawTileLines) drawRectOutline(rect, 4, 0xA0FFFFFF);
					}
				}

				if (game->debugDrawChunkLines) drawRectOutline(chunk->rect, 8, 0xA0FFFFFF);
			}
		} ///

		{ /// Draw actors
			for (int i = 0; i < world->actorsNum; i++) {
				Actor *actor = &world->actors[i];
				ActorTypeInfo *info = &game->actorTypeInfos[actor->type];

				Chunk *chunk = worldToChunk(actor->position);
				if (chunk && !chunk->visible) continue;

				bool isSelected = false;
				for (int i = 0; i < data->selectedActorsNum; i++) {
					if (data->selectedActors[i] == actor->id) {
						isSelected = true;
						break;
					}
				}

				if (isSelected) {
					drawRect(inflatePerc(getRect(actor), 0.2), 0xFFEAF82A);

					if (info->isTower) {
						Circle range = makeCircle(actor->position, getRange(actor, worldToTile(actor->position)));
						drawCircle(range, 0x80FF0000);
					}
				}

				Rect rect = getRect(actor);

				if (actor->type == ACTOR_BALLISTA) {
					drawRect(rect, 0xFF800000);

					Line2 line;
					line.start = getCenter(rect);
					line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
					drawLine(line, 4, 0xFFFF0000);
				} else if (actor->type == ACTOR_MORTAR_TOWER) {
					drawRect(rect, 0xFF525252);
				} else if (actor->type == ACTOR_TESLA_COIL) {
					float perc = clampMap(actor->timeSinceLastShot, 0, 0.5, 0.5, 0);
					int sparkColor = setAofArgb(0xFFB8FFFA, perc*255.0);

					Circle circle = makeCircle(actor->position, getRange(actor, worldToTile(actor->position)));
					drawCircle(circle, sparkColor);

					drawCircle(makeCircle(getCenter(rect), rect.width/2), 0xFFA0A0F0);
				} else if (actor->type == ACTOR_FROST_KEEP) {
					drawRect(rect, 0xFFE3F0F5);
				} else if (actor->type == ACTOR_FLAME_THROWER) {
					drawRect(rect, lerpColor(BURN_COLOR, 0xFF000000, 0.75));

					Line2 line;
					line.start = getCenter(rect);
					line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
					drawLine(line, 12, 0xFF000000);

					float range = getRange(actor, worldToTile(actor->position));
					Tri2 tri = getAttackTri(actor->position, range, actor->aimRads, toRad(15));

					float perc = clampMap(actor->timeSinceLastShot, 0, 0.5, 0.5, 0);
					int color = setAofArgb(BURN_COLOR, perc*255.0);
					drawLine(tri.verts[0], tri.verts[1], 5, color);
					drawLine(tri.verts[1], tri.verts[2], 5, color);
					drawLine(tri.verts[2], tri.verts[0], 5, color);
				} else if (actor->type == ACTOR_POISON_SPRAYER) {
					drawRect(rect, lerpColor(POISON_COLOR, 0xFF000000, 0.75));

					Line2 line;
					line.start = getCenter(rect);
					line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
					drawLine(line, 12, 0xFF000000);

					float range = getRange(actor, worldToTile(actor->position));
					Tri2 tri = getAttackTri(actor->position, range, actor->aimRads, toRad(15));

					float perc = clampMap(actor->timeSinceLastShot, 0, 0.5, 0.5, 0);
					int color = setAofArgb(POISON_COLOR, perc*255.0);
					drawLine(tri.verts[0], tri.verts[1], 5, color);
					drawLine(tri.verts[1], tri.verts[2], 5, color);
					drawLine(tri.verts[2], tri.verts[0], 5, color);
				} else if (actor->type == ACTOR_SHREDDER) {
					drawRect(rect, 0xFF800000);

					Line2 line;
					line.start = getCenter(rect);
					line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
					line.start = line.end - radToVec2(actor->aimRads)*(TILE_SIZE);
					drawLine(line, 4, 0xFFFF0000);
				} else if (actor->type == ACTOR_MANA_SIPHON) {
					drawRect(rect, lerpColor(0xFFA4CCC8, 0xFF000000, 0.25));
				} else if (actor->type == ACTOR_MANA_CRYSTAL) {
					drawRect(rect, 0xFFA4B0CC);
				} else if (actor->type >= ACTOR_ENEMY1 && actor->type <= ACTOR_ENEMY64) {
					drawRect(rect, 0xFF008000);

					Rect vitalityRect = rect;
					{
						vitalityRect.height = 4;
						vitalityRect.y = rect.y - vitalityRect.height - 4;
						float totalPoints = info->maxHp + info->maxArmor + info->maxShield;

						float maxHpPerc = info->maxHp / totalPoints;
						Rect hpRect = vitalityRect;
						hpRect.width *= maxHpPerc;
						hpRect.width *= actor->hp / info->maxHp;
						drawRect(hpRect, 0xFF00FF00);

						float maxArmorPerc = info->maxArmor / totalPoints;
						Rect armorRect = vitalityRect;
						armorRect.x += maxHpPerc * vitalityRect.width;
						armorRect.width *= maxArmorPerc;
						armorRect.width *= actor->armor / info->maxArmor;
						drawRect(armorRect, 0xFFFFD66E);

						float maxShieldPerc = info->maxShield / totalPoints;
						Rect shieldRect = vitalityRect;
						shieldRect.x += (maxHpPerc + maxArmorPerc) * vitalityRect.width;
						shieldRect.width *= maxShieldPerc;
						shieldRect.width *= actor->shield / info->maxShield;
						drawRect(shieldRect, 0xFF718691);
					}

					if (actor->slow > 0) {
						Rect slowRect = vitalityRect;
						slowRect.y -= slowRect.height + 4;
						slowRect.width *= clampMap(actor->slow, 0, 100, 0, 1, QUINT_OUT);
						drawRect(slowRect, 0xFF01335C);
					}

					if (actor->poison) {
						Rect textRect = getRect(actor);
						textRect.x -= textRect.width;
						textRect.y -= textRect.height;
						DrawTextProps props = newDrawTextProps(game->defaultFont, POISON_COLOR);
						drawTextInRect(frameSprintf("%.0f", actor->poison), props, textRect);
					}

					if (actor->burn) {
						Rect textRect = getRect(actor);
						textRect.y -= textRect.height;
						DrawTextProps props = newDrawTextProps(game->defaultFont, BURN_COLOR);
						drawTextInRect(frameSprintf("%.0f", actor->burn), props, textRect);
					}

					if (actor->bleed) {
						Rect textRect = getRect(actor);
						textRect.x += textRect.width;
						textRect.y -= textRect.height;
						DrawTextProps props = newDrawTextProps(game->defaultFont, BLEED_COLOR);
						drawTextInRect(frameSprintf("%.0f", actor->bleed), props, textRect);
					}
				} else if (actor->type == ACTOR_ARROW) {
					Rect bulletRect = makeCenteredSquare(actor->position, 8);
					drawRect(bulletRect, 0xFFFF0000);
				} else if (actor->type == ACTOR_MORTAR) {
					float delayTime = info->bulletSpeed;
					float explodeRange = info->baseRange;
					if (actor->time < delayTime) {
						float ghostPerc = clampMap(actor->time, 0, delayTime, 0.75, 1);
						drawCircle(actor->position, explodeRange*ghostPerc, 0x80900000);
					}

					if (actor->time >= delayTime) {
						Circle circle = makeCircle(actor->position, explodeRange);
						drawCircle(circle, 0xFFFFFFFF);
					}
				} else if (actor->type == ACTOR_FROST) {
					drawRect(rect, 0x80FFFFFF);
				} else if (actor->type == ACTOR_SAW) {
					Rect bulletRect = makeCenteredSquare(actor->position, 8);
					drawRect(bulletRect, 0xFFFF0000);
				} else {
					drawRect(rect, 0xFFFF00FF);
				}

				if (info->isTower) {
					Rect levelNumberRect = makeCenteredSquare(v2(), game->size.y*0.03);
					levelNumberRect.x = rect.x + rect.width/2 - levelNumberRect.width/2;
					levelNumberRect.y = rect.y - levelNumberRect.height - game->size.y*0.01;

					DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
					drawTextInRect(frameSprintf("%d", actor->level), props, levelNumberRect);

					if (actor->level < getMaxLevel(actor->type)) {
						Rect xpRect = rect;
						xpRect.height = game->size.y*0.005;
						xpRect.y -= xpRect.height + game->size.y*0.005;
						drawRect(xpRect, 0x80FFEF94);

						float maxXp = maxXpPerLevels[actor->level];
						xpRect.width *= actor->xp / maxXp;
						drawRect(xpRect, 0xFFFFEF94);
					}
				}

				if (getInfo(actor)->isEnemy) {
					if (isHoveringActor(actor)) {
						Rect textRect = makeCenteredRect(actor->position, game->size*v2(0.02, 0.02));
						DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
						drawTextInRect(frameSprintf(
								"%s\n%.0f/%.0f\n%.0f/%.0f\n%.0f/%.0f\n",
								getInfo(actor)->name,
								actor->shield,
								getInfo(actor)->maxShield,
								actor->armor,
								getInfo(actor)->maxArmor,
								actor->hp,
								getInfo(actor)->maxHp
						), props, textRect);
					}
				}

				if (game->debugShowActorVelo) {
					DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
					drawTextInRect(frameSprintf("%.01f\n%.01f", actor->velo.x, actor->velo.y), props, rect);
					// Vec2 start = actor->position;
					// Vec2 end = actor->position + normalize(actor->velo)*(TILE_SIZE/2);
					// drawLine(start, end, 5, 0xFFFF0000);
				}
			}
		} ///

		{ /// Update effects
			for (int i = 0; i < game->effectsNum; i++) {
				Effect *effect = &game->effects[i];

				bool complete = false;
				float maxTime = 1;

				if (effect->type == EFFECT_DEFAULT_CORE_EVENT) {
					CoreEvent *event = &effect->coreEvent;
					if (event->type == CORE_EVENT_DAMAGE) {
						float perc = effect->time / maxTime;

						Rect textRect = makeCenteredRect(effect->position, game->size*v2(0.01, 0.01));

						DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);

						if (effect->coreEvent.shieldValue != 0) {
							props.color = SHIELD_COLOR;
							drawTextInRect(frameSprintf("-%.0f", effect->coreEvent.shieldValue), props, textRect);
							textRect.y += textRect.height;
						}

						if (effect->coreEvent.armorValue != 0) {
							props.color = ARMOR_COLOR;
							drawTextInRect(frameSprintf("-%.0f", effect->coreEvent.armorValue), props, textRect);
							textRect.y += textRect.height;
						}

						if (effect->coreEvent.hpValue != 0) {
							props.color = HP_COLOR;
							drawTextInRect(frameSprintf("-%.0f", effect->coreEvent.hpValue), props, textRect);
							textRect.y += textRect.height;
						}
					} else if (event->type == CORE_EVENT_SHOW_GHOST) {
						complete = true;
						Vec2i tilePosition = worldToTile(event->position);
						Rect tileRect = tileToWorldRect(tilePosition);
						drawRect(tileRect, lerpColor(0x80000088, 0xFF000088, timePhase(data->time*2)));

						Circle range = makeCircle(getCenter(tileRect), getRange(event->actorType, tilePosition));
						drawCircle(range, 0x80FF0000);
					}
				}

				effect->time += elapsed;
				if (effect->time > maxTime) complete = true;

				if (complete) {
					arraySpliceIndex(game->effects, game->effectsNum, sizeof(Effect), i);
					game->effectsNum--;
					i--;
					continue;
				}
			}
		} ///

		{ /// Show explore buttons
			if (!data->playingWave && !game->presentedUpgradesNum && data->tool == TOOL_NONE) {
				for (int i = 0; i < world->chunksNum; i++) {
					Chunk *chunk = &world->chunks[i];
					if (!chunk->visible) continue;

					for (int i = 0; i < chunk->connectionsNum; i++) {
						Chunk *newChunk = getChunkAt(chunk->connections[i]);
						Assert(newChunk);

						if (newChunk->visible) continue;

						Vec2 middle = (getCenter(chunk->rect) + getCenter(newChunk->rect)) / 2;
						Rect exploreRect = makeCenteredRect(middle, v2(300, 128));

						if (contains(exploreRect, game->mouse)) {
							exploreRect = inflatePerc(exploreRect, 0.1);
							if (isMouseClicked()) {
								newChunk->visible = true;
								startNextWave();
							}
						}

						drawRectOutline(exploreRect, 4, 0xFFCCCCCC);

						DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
						drawTextInRect("Explore here", props, inflatePerc(exploreRect, -0.1));
					}

					bool isPortal = false; //@copyPastedIsPortal
					if (chunk->connectionsNum == 1 && !isZero(chunk->position)) isPortal = true;
					if (isPortal) {
						Rect portalTileRect = tileToWorldRect(chunkTileToWorldTile(chunk, v2i(CHUNK_SIZE/2, CHUNK_SIZE/2)));
						drawRect(portalTileRect, 0x80FF0000);
					}
				}
			}
		} ///

		popCamera2d();
	} else {
		Pass *pass = createPass();
		pushPass(pass);

		pass->camera.position = v3(0, 0, 10);
		pass->camera.target = v3(0, 0, 0);
		pass->camera.up = v3(0, 1, 0);
		pass->camera.fovy = 59;
		pass->camera.isOrtho = false;
		// pass->camera.orthoScale = 10;

		{
			Vec3 offset = v3();

			Tri tri = {};
			tri.verts[0] = v3(0, 0, 0) + offset;
			tri.verts[1] = v3(0, 1, 0) + offset;
			tri.verts[2] = v3(1, 1, 0) + offset;

			PassCmd *cmd = createPassCmd(pass);
			cmd->type = PASS_CMD_TRI;
			cmd->verts[0] = tri.verts[0];
			cmd->verts[1] = tri.verts[1];
			cmd->verts[2] = tri.verts[2];

			cmd->colors[0] = 0xFFFF0000;
			cmd->colors[1] = 0xFFFF0000;
			cmd->colors[2] = 0xFFFF0000;
		}

		{
			Mesh *cubeMesh = getMesh("assets/common/models/Cube.Cube.mesh");
			Matrix4 matrix = mat4();
			matrix.ROTATE_EULER(0, platform->time, 0);
			passMesh(cubeMesh, matrix, 0xFFFF0000);
		}

		popPass();

		start3d(pass->camera, game->size, 0.01, 1000);
		Raylib::rlDisableBackfaceCulling();

		for (int i = 0; i < pass->cmdsNum; i++) {
			PassCmd *cmd = &pass->cmds[i];
			if (cmd->type == PASS_CMD_TRI) {
				Raylib::rlCheckRenderBatchLimit(3);

				// Raylib::rlSetTexture(textureId);
				Raylib::rlBegin(RL_TRIANGLES);

				for (int i = 0; i < 3; i++) {
					int a, r, g, b;
					hexToArgb(cmd->colors[i], &a, &r, &g, &b);

					Raylib::rlColor4ub(r, g, b, a);
					Raylib::rlTexCoord2f(cmd->uvs[i].x, cmd->uvs[i].y);
					Raylib::rlVertex3f(cmd->verts[i].x, cmd->verts[i].y, cmd->verts[i].z);
				}

				Raylib::rlEnd();
			} else if (cmd->type == PASS_CMD_MESH) {
				Material material = createMaterial();
				material.values[Raylib::MATERIAL_MAP_DIFFUSE].color = hexToArgbFloat(cmd->meshTint);
				drawMesh(cmd->mesh, cmd->meshMatrix, NULL, material);
			}
		}
		end3d();

		destroyPass(pass);
	}
}

Effect *createEffect(EffectType type) {
	if (game->effectsNum > EFFECTS_MAX-1) {
		logf("Too many effects!\n");
		game->effectsNum--;
	}

	Effect *effect = &game->effects[game->effectsNum++];
	memset(effect, 0, sizeof(Effect));
	effect->type = type;
	return effect;
}

void updateAndDrawOverlay(float elapsed) {
	World *world = data->world;

	{ /// Editor
		if (keyPressed(KEY_CTRL) && keyJustPressed('R')) game->is2d = !game->is2d;

		if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
		if (game->inEditor) {
			ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Checkbox("Show Dijkstra values", &game->debugShowDijkstraValues);
			ImGui::Checkbox("Show Flow Field values", &game->debugShowFlowFieldValues);
			ImGui::Checkbox("Show perlin values", &game->debugShowPerlinValues);
			ImGui::Checkbox("Draw chunk lines", &game->debugDrawChunkLines);
			ImGui::Checkbox("Draw tile lines", &game->debugDrawTileLines);
			ImGui::Checkbox("Show actor velo", &game->debugShowActorVelo);
			ImGui::Checkbox("Is 2d", &game->is2d);
			if (ImGui::Button("Explore all")) {
				for (int i = 0; i < world->chunksNum; i++) {
					Chunk *chunk = &world->chunks[i];
					chunk->visible = true;
				}
			}
			ImGui::InputInt("Money", &data->money);
			ImGui::InputFloat("Mana", &data->mana);
			ImGui::InputFloat("MaxMana", &data->maxMana);
			ImGui::InputInt("Wave", &data->wave);
			ImGui::SameLine();
			if (ImGui::Button("Next wave")) startNextWave();

			if (ImGui::Button("Add 1000 random ballistas")) {
				int added = 0;
				int iters = 0;
				for (;;) {
					if (added > 1000) break;
					if (iters >= 10000) {
						logf("Failed after 10000...\n");
						break;
					}

					Chunk *chunk = &world->chunks[rndInt(0, world->chunksNum-1)];
					if (!chunk->visible) continue;
					Vec2i chunkTilePos;
					chunkTilePos.x = rndInt(0, CHUNK_SIZE-1);
					chunkTilePos.y = rndInt(0, CHUNK_SIZE-1);
					Tile *tile = &chunk->tiles[chunkTilePos.y * CHUNK_SIZE + chunkTilePos.x];
					Vec2i tilePos = chunkTileToWorldTile(chunk, chunkTilePos);

					bool canBuild = true;
					if (tile->type != TILE_GROUND) canBuild = false;

					for (int i = 0; i < world->actorsNum; i++) {
						Actor *other = &world->actors[i];
						if (equal(worldToTile(other->position), tilePos)) canBuild = false;
					}

					if (canBuild) {
						Actor *newTower = createActor(ACTOR_BALLISTA);
						newTower->position = tileToWorld(tilePos);
						added++;
					}

					iters++;
				}
			}

			ImGui::Separator();

			ImGui::InputText("New save state name", game->debugNewSaveStateName, PATH_MAX_LEN);
			ImGui::SameLine();
			if (ImGui::Button("Save")) {
				saveState(frameSprintf("assets/states/%s.save_state", game->debugNewSaveStateName));
				game->debugNewSaveStateName[0] = 0;
				refreshAssetPaths();
			}

			int pathCount = 0;
			for (int i = 0; i < assetPathsNum; i++) {
				char *path = assetPaths[i];
				if (strstr(path, "assets/states/")) {
					char *name = frameStringClone(path) + strlen("assets/states/");
					char *dotPos = strchr(name, '.');
					if (dotPos) *dotPos = 0;
					if (ImGui::Button(name)) {
						loadState(path);
					}
					if ((pathCount+1) % 5 != 0) ImGui::SameLine();
					pathCount++;
				}
			}

			ImGui::NewLine();
			ImGui::Separator();
			if (ImGui::Button("Reset game")) game->shouldReset = true;
			ImGui::InputText("Campaign name", data->campaignName, CAMPAIGN_NAME_MAX_LEN);

			ImGui::End();
		}
	} ///

	{ /// Get input and move camera
		Vec2 moveDir = v2();
		if (keyPressed('W')) moveDir.y--;
		if (keyPressed('S')) moveDir.y++;
		if (keyPressed('A')) moveDir.x--;
		if (keyPressed('D')) moveDir.x++;

		data->cameraZoom += platform->mouseWheel * 0.1;
		data->cameraZoom = mathClamp(data->cameraZoom, 0.1, 20);

		data->cameraPosition += normalize(moveDir) * 20 / data->cameraZoom;

		for (int i = 1; i <= 9; i++) {
			if (keyPressed(KEY_ALT) && keyJustReleased('0' + i)) {
				loadState(frameSprintf("assets/states/%d.save_state", i));
			}
		}

		if (keyJustPressed('-')) {
			game->timeScale *= 0.5;
			logf("Time scale: %.3f\n", game->timeScale);
		}
		if (keyJustPressed('=')) {
			game->timeScale *= 2;
			logf("Time scale: %.3f\n", game->timeScale);
		}
	} ///

	{ /// Update tool related ui
		if (data->tool == TOOL_NONE) {
			nguiStartWindow("Tools window", game->size*v2(0.5, 1), v2(0.5, 1));
			nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 4);

			ActorType *typesCanBuy = (ActorType *)frameMalloc(sizeof(ActorType) * ACTOR_TYPES_MAX);
			int typesCanBuyNum = 0;
			typesCanBuy[typesCanBuyNum++] = ACTOR_BALLISTA;
			if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK, ACTOR_MORTAR_TOWER)) typesCanBuy[typesCanBuyNum++] = ACTOR_MORTAR_TOWER;
			if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK, ACTOR_TESLA_COIL)) typesCanBuy[typesCanBuyNum++] = ACTOR_TESLA_COIL;
			if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK, ACTOR_FROST_KEEP)) typesCanBuy[typesCanBuyNum++] = ACTOR_FROST_KEEP;
			if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK, ACTOR_FLAME_THROWER)) typesCanBuy[typesCanBuyNum++] = ACTOR_FLAME_THROWER;
			if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK, ACTOR_POISON_SPRAYER)) typesCanBuy[typesCanBuyNum++] = ACTOR_POISON_SPRAYER;
			if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK, ACTOR_SHREDDER)) typesCanBuy[typesCanBuyNum++] = ACTOR_SHREDDER;
			if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK, ACTOR_MANA_SIPHON)) typesCanBuy[typesCanBuyNum++] = ACTOR_MANA_SIPHON;

			for (int i = 0; i < typesCanBuyNum; i++) {
				ActorType actorType = typesCanBuy[i];
				ActorTypeInfo *info = &game->actorTypeInfos[actorType];
				float price = info->price + info->priceMulti*game->actorTypeCounts[actorType];
				char *label = frameSprintf("%s $%.0f\n", info->name, price);
				if (nguiButton(label)) {
					data->tool = TOOL_BUILDING;
					data->actorToBuild = actorType;
				}
			}

			nguiPopStyleVar(NGUI_STYLE_ELEMENTS_IN_ROW);
			nguiEndWindow();
		} else if (data->tool == TOOL_BUILDING) {
			// Nothing...
		} else if (data->tool == TOOL_SELECTED) {
			nguiStartWindow("Selected window", game->size*v2(0.5, 1), v2(0.5, 1));
			nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 4);

			if (nguiButton("Sell")) {
				for (int i = 0; i < data->selectedActorsNum; i++) {
					Actor *actor = getActor(data->selectedActors[i]);
					ActorTypeInfo *info = &game->actorTypeInfos[actor->type];

					actor->markedForDeletion = true;
					data->money += actor->amountPaid;
					data->money += info->price + ((game->actorTypeCounts[actor->type]-1) * info->priceMulti);

					arraySpliceIndex(data->selectedActors, data->selectedActorsNum, sizeof(int), i);
					data->selectedActorsNum--;
					i--;
					continue;
				}
			}

			if (data->selectedActorsNum == 1) {
				Actor *actor = getActor(data->selectedActors[0]);
				if (nguiButton(frameSprintf("Priority: %s", priorityStrings[actor->priority]))) {
					actor->priority = (Priority)((int)actor->priority + 1);
					if (actor->priority > PRIORITIES_MAX-1) {
						actor->priority = (Priority)0;
					}
				}
			}

			nguiPopStyleVar(NGUI_STYLE_ELEMENTS_IN_ROW);
			nguiEndWindow();

			if (isMouseClicked()) data->selectedActorsNum = 0;

			if (data->selectedActorsNum == 0) data->tool = TOOL_NONE;
		}
	} ///

	if (game->presentedUpgradesNum > 0) {
		nguiStartWindow("Upgrade window", v2(0, platform->windowHeight/2), v2(0, 0.5));
		for (int i = 0; i < game->presentedUpgradesNum; i++) {
			Upgrade *upgrade = getUpgrade(game->presentedUpgrades[i]);
			char *label = "";
			for (int i = 0; i < upgrade->effectsNum; i++) {
				UpgradeEffect *effect = &upgrade->effects[i];
				ActorTypeInfo *info = &game->actorTypeInfos[effect->actorType];

				char *line = "";
				if (effect->type == UPGRADE_EFFECT_UNLOCK) {
					line = frameSprintf("Unlock %s", info->name);
				} else if (effect->type == UPGRADE_EFFECT_DAMAGE_MULTI) {
					line = frameSprintf("%s damage %.0f%%", info->name, effect->value*100.0);
				} else if (effect->type == UPGRADE_EFFECT_RANGE_MULTI) {
					line = frameSprintf("%s range %.0f%%", info->name, effect->value*100.0);
				} else if (effect->type == UPGRADE_EFFECT_RPM_MULTI) {
					line = frameSprintf("%s rpm %.0f%%", info->name, effect->value*100.0);
				} else if (effect->type == UPGRADE_EFFECT_EXTRA_CARDS) {
					line = frameSprintf("Get %.0f extra upgrade card choice(s)", effect->value);
				} else if (effect->type == UPGRADE_EFFECT_EXTRA_MONEY) {
					line = frameSprintf("Gain an extra %.0f money per kill", effect->value);
				} else if (effect->type == UPGRADE_EFFECT_MANA_GAIN_MULTI) {
					line = frameSprintf("%.0f%% mana gain", effect->value*100.0);
				} else {
					line = frameSprintf("Unlabeled effect %d", effect->type);
				}
				if (i != 0) label = frameSprintf("%s\n", label);
				label = frameSprintf("%s%s", label, line);
			}
			if (nguiButton(label)) {
				data->ownedUpgrades[data->ownedUpgradesNum++] = upgrade->id;
				game->presentedUpgradesNum = 0;

				if (data->hp > 0) {
					// if (fileExists("assets/states/autosave.save_state")) {
					// 	if (fileExists("assets/states/prevAutosave.save_state")) deleteFile("assets/states/prevAutosave.save_state");
					// 	copyFile("assets/states/autosave.save_state", "assets/states/prevAutosave.save_state");
					// }
					saveState("assets/states/autosave.save_state");
					copyFile("assets/states/autosave.save_state", frameSprintf("assets/states/%s_autosave_%d.save_state", data->campaignName, data->wave));
				}
			}
		}
		nguiEndWindow();
	}

	{
		Rect rect = makeRect(0, 0, 350, 100);
		DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
		drawTextInRect(frameSprintf(
			"Wave: %d\nHp: %d\nMoney $%d\nMana: %.1f/%.1f (+%.1f/s)",
			data->wave,
			data->hp,
			data->money,
			data->mana,
			data->maxMana,
			game->manaToGain/elapsed
		), props, rect);
	}

	nguiDraw(elapsed);

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) drawText(game->defaultFont, frameSprintf("%.1fms", platform->frameTimeAvg), v2(300, 0), 0xFF808080);

	drawOnScreenLog();
}

//@consider Frost keep has a square range
//@consider Everything but the frost keep scale mana usage by base damage?
