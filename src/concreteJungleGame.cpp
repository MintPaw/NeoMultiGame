// Make it so allies leave if there's too many
// Figure out how to prevent units from bunching up by doors
// Smooth out movement, blocking, and animation transtions
// Figure out a better solution for passes

// We need healing
// Do a ghost that is the base speed
// Make running punch more like streak
// Make money counter add up
// Grenade enemy
// Guys that have less hitstun?
// Don't block backwards?

/*

Upgrades:
	- Chain lightning (5 max links, 4% chance, 5% damage, 40 distance)
		- Link chance + 2%
		- Link radius 5%
		- Link damage multiplier +10%

	- Magnet (Attract orbs from 30 distance away)
		- Attract from +10% away

	- Armor (10% chance of hyper armor)
		- +2% chance of hyper armor

	- Slow mo (Can I even do that? -_-)
	- Warp strike
	!- Automatic push blocking
	- Level up (+1 to every stat)
	- Bleed
	!- Dash
	- Mist coil: Fire a fireball at the cost of hp
	- Aphotic Shield: Create a shield the absorbs a certain amount of damage (armor?)
	- (passive) Curse of Avernus: Hitting an enemy 4 times within a specific amount of time will cause them to be slowed
	- Borrowed Time: For a short time, taking damage heals you (automatically activates at low hp?)
	- Acid Spray: Throw down some acid that slowly damages anyone who's standing in it.
	- Chemical Rage: Puts you into rage for a short time, gives hp regen, cooldown reduction, and movespeed (and attack speed?)
	- Cold Feet: Drop a orb that slow surrounding units
	!- (passive) Mana break: Attacks that land take a small percentage of enemy stamina
	- Magnetic field: Drop a field that give allies bonus attack speed
	- Spark wraith: Like a one-time widow mine
	- (passive) Counter spike: Gives a chance for a magical counter to being attacked
	!- Culling blade: An attack that instantly kills units below a certain amount of hp percent
	- (passive) Enfeeble: Attacking applies a debuff that lowers attack damage
	- Brain sap: An attack that heals you
	- (passive) Sticky napalm: A stacking slow that's applied with every attack
	- Flamebreak: You "hit the ground" and push everyone away
	- Firefly: For a short time, when you walk around, you drop fire or something else that does damage
	- Ally summons?
	- Primal roar: Stuns everyone in a "cone" in front of you
	!- (passive) Bloodrage: You gain attack speed as you lose hp
	- Blood rite: Put down a seal/bomb that goes off after ~3 seconds
	- Handgun: Limited ammo

*idle = 1
*move = 4
*run = 4
*jump = 3
*punch = 3
*punch2 = 3
*kick = 5
*air punch = 2
*air kick = 1
*upper cut = 3
*blocked = 1
*hitstun = 3
*air hitstun = 3
*knockdown = 3
*raising = 3
*running punch = 3
*running kick = 3
idleStanding (hands in pocket)
idleTalking (hand forward)
idleTalking
alerted
alertedPointing

*/

#define TEAMS_MAX 8
#define CITY_ROWS 8
#define CITY_COLS 8
#define CITY_START_INDEX 63

#define BLOCKING_STAMINA_THRESHOLD 5
#define SECS_PER_CITY_TICK 1
Vec3 UNIT_SIZE = v3(150, 150, 300);
#define ROOM_PREWARM_TIME 20

u32 teamColors[TEAMS_MAX] = {
	0xFFC5FF87,
	0xFFFF878B,
	0xFFBAFFF8,
	0xFF7C6EFF,
	0xFFFFC56E,
	0xFFFFF16E,
	0xFFFF6BF3,
	0xFF57000C,
};

enum BuffType {
	BUFF_NONE=0,
	BUFF_HEAVEN_STEP_GRAVITY=1,
	BUFF_BUDDHA_PALM_SLOW=2,
	BUFF_STICKY_NAPALM_STACK=3,
};
char *buffTypeStrings[] = {
	"None",
	"Heaven Step gravity",
	"Buddha Palm slow",
	"Sticky Napalm stack",
};

struct Buff {
	BuffType type;
	float time;
	float maxTime;
};

enum ActionType {
	ACTION_NONE=0,
	ACTION_LANDING=1, // Maybe
	ACTION_HITSTUN=2,
	ACTION_KNOCKDOWN=3,
	ACTION_BLOCKSTUN=4,
	ACTION_RAISING=5,
	ACTION_EXTRA_4=6,
	ACTION_EXTRA_5=7,
	ACTION_EXTRA_6=8,
	ACTION_EXTRA_7=9,
	ACTION_EXTRA_8=10,
	ACTION_PUNCH=11,
	ACTION_KICK=12,
	ACTION_AIR_PUNCH=13,
	ACTION_AIR_KICK=14,
	ACTION_UPPERCUT=15,
	ACTION_RUNNING_PUNCH=16,
	ACTION_RUNNING_KICK=17,
	ACTION_HEAVEN_STEP=18,
	ACTION_SHADOW_STEP=19,
	ACTION_BUDDHA_PALM=20,
	ACTION_DASH=21,
	ACTION_BRAIN_SAP=22,
	ACTION_CULLING_BLADE=23,
	ACTION_STICKY_NAPALM=24,

	ACTION_FORCED_IDLE=64,
	ACTION_FORCED_MOVE=65,
	ACTION_FORCED_LEAVE=66, // Not working yet
	ACTION_TYPES_MAX=128,
};

#define _F_AT_ALLOWED_ON_GROUND    (1 << 0)
#define _F_AT_ALLOWED_IN_AIR       (1 << 1)
struct ActionTypeInfo {
#define ACTION_NAME_MAX_LEN 32
	char name[ACTION_NAME_MAX_LEN];

	u32 flags;

	int startupFrames;
	int activeFrames;
	int recoveryFrames;
	int hitstunFrames;
	int blockstunFrames;
	char animationName[PATH_MAX_LEN];
	bool animationLoops;

#define HITBOXES_MAX 4
	AABB hitboxes[HITBOXES_MAX];
	int hitboxesNum;

	Vec3 hitVelo;
	Vec3 blockVelo;
	float damage;

	Vec3 thrust;
	int thrustFrame;

	float staminaUsage;

	BuffType buffToGet;
	float buffToGetTime;
	BuffType buffToGive;
	float buffToGiveTime;
};

struct Globals {
	ActionTypeInfo actionTypeInfos[ACTION_TYPES_MAX];
	Vec3 actorSpriteOffset;
	float actorSpriteScale;
	float actorSpriteScaleMultiplier;
	float movementPercDistanceWalkingRatio;
	float movementPercDistanceRunningRatio;
};

enum ItemType {
	ITEM_NONE,
	ITEM_MONEY,
	ITEM_HEALTH_PACK,
	ITEM_DAMAGE_BOOST,
	ITEM_HP_BOOST,
	ITEM_STAMINA_REGEN_BOOST,
	ITEM_MAX_STAMINA_BOOST,
	ITEM_MOVEMENT_SPEED_BOOST,
	ITEM_ATTACK_SPEED_BOOST,
	ITEM_MAGNET,
	// ITEM_MAGNET_RANGE_1,
	ITEM_HYPER_ARMOR, // Doesn't work??
	// ITEM_HYPER_ARMOR_ODDS_1,
	ITEM_HEAVEN_STEP,
	// ITEM_HEAVEN_LENGTH_1,
	ITEM_SHADOW_STEP,
	// ITEM_SHADOW_STEP_COST_1
	ITEM_BUDDHA_PALM,
	ITEM_BLOOD_RAGE,
	// ITEM_BLOOD_RAGE_AMP_1,
	ITEM_DASH,
	// ITEM_DASH_RANGE_1,
	ITEM_BRAIN_SAP,
	// ITEM_BRAIN_SAP_SPEED_1,
	ITEM_CULLING_BLADE,
	// ITEM_CULLING_BLADE_CUT_OFF_1,
	ITEM_STICKY_NAPALM,
	// ITEM_CULLING_BLADE_CUT_OFF_1,
	ITEM_TYPES_MAX,
};
enum ItemSlotType {
	ITEM_SLOT_GLOBAL,
	ITEM_SLOT_PASSIVE,
	ITEM_SLOT_ACTIVE,
};
struct ItemTypeInfo {
#define ITEM_NAME_MAX_LEN 32
	char name[ITEM_NAME_MAX_LEN];
	ItemSlotType slotType;
	ActionType actionType;
	float price;
	ItemType preReq;
	int maxAmountFromStore;
};
struct Item {
	ItemType type;
	int id;
	int amount;

	ItemTypeInfo *info;
};

struct Action {
	int id;
	ActionType type;
	ActionTypeInfo *info;

	float prevTime;
	float time;
	float customLength;
	Vec3 targetPosition;
};

enum ActorType {
	ACTOR_NONE=0,
	ACTOR_UNIT=1,
	ACTOR_GROUND=2,
	ACTOR_WALL=3,
	ACTOR_DUMMY=4,
	ACTOR_DOOR=5,
	ACTOR_UNIT_SPAWNER=6,
	ACTOR_ITEM=7,
	ACTOR_STORE=8,
	ACTORS_MAX,
};
char *actorTypeStrings[] = {
	"None",
	"Player",
	"Ground",
	"Wall",
	"Dummy",
	"Door",
	"Spawner",
	"Item",
	"Store",
};
struct ActorTypeInfo {
	bool isWall;
	bool canBeHit;
	bool hasPhysics;
};

enum AiState {
	AI_IDLE,
	AI_STAND_NEAR_TARGET,
	AI_APPROACH_FOR_ATTACH,
};

enum StatType {
	STAT_DAMAGE,
	STAT_HP,
	STAT_STAMINA_REGEN,
	STAT_MAX_STAMINA,
	STAT_MOVEMENT_SPEED,
	STAT_ATTACK_SPEED,
	STATS_MAX,
};
char *statStrings[] = {
	"Damage",
	"Hp",
	"Stamina regen",
	"Max stamina",
	"Movement speed",
	"Attack speed",
};
u32 statTypeColors[] = {
	0xFF690009,
	0xFF004F0C,
	0xFF6E81FF,
	0xFF0523EB,
	0xFF02ED26,
	0xFFFF3042,
};

enum AiType {
	AI_NORMAL,
	AI_DUMMY,
};

struct Style {
	int activeItem0;
	int activeItem1;
	int passiveItem;
};

struct Actor {
	ActorType type;
	int id;
#define ACTOR_NAME_MAX_LEN 64
	char name[ACTOR_NAME_MAX_LEN];

	Vec3 position;
	Vec3 size;

#define MAP_NAME_MAX_LEN 64
	char destMapName[MAP_NAME_MAX_LEN];
	float locked;

	int unitsToSpawn;

	/// Unserialized
	ActorTypeInfo *info;
	bool playerControlled;
	int team;
	bool facingLeft;
	bool markedForDeletion;

	float hp;
	float maxHp;

	float money;

	float stamina;
	float maxStamina;
	bool isBlocking;

#define ACTION_IDS_HIT_BY_MAX 8
	int actionIdsHitBy[ACTION_IDS_HIT_BY_MAX];

#define ACTIONS_MAX 8
	Action pastActions[ACTIONS_MAX];
	int pastActionsNum;
	Action actions[ACTIONS_MAX];
	int actionsNum;

#define BUFFS_ON_ACTOR_MAX 32
	Buff buffs[BUFFS_ON_ACTOR_MAX];
	int buffsNum;

	float stats[STATS_MAX];
	int level;

	Vec3 movementAccel;
	Vec3 accel;
	Vec3 velo;
	bool prevIsOnGround;
	bool isOnGround;

	float timeMoving;
	float timeNotMoving;
	float timeInAir;
	float timeWithoutAction;

	float movementPerc;

	Vec2 prevInputVec; // All this is just for running lmao
	float timeSinceLastLeftPress;
	float timeSinceLastRightPress;
	bool isRunningLeft;
	bool isRunningRight;

	AiType aiType;
	AiState prevAiState;
	AiState aiState;
	float aiStateTime;
	int aiTarget;
	Vec2 aiCurrentXyTarget;
	Vec2 aiCurrentXy;
	Vec2 aiCurrentXyOffset;
	float aiStateLength;
	float allianceCost;

	bool isLastExitedDoor;
	bool doorPlayerSpawnedOver;

	ItemType itemType;
	float itemAmount;

	Item *items;
	int itemsNum;
	int itemsMax;

#define STYLES_MAX 4
	Style styles[STYLES_MAX];
	int styleIndex;
};

struct Map {
	char name[MAP_NAME_MAX_LEN];
	bool isTemplatized;

#define ACTORS_MAX 512
	Actor actors[ACTORS_MAX];
	int actorsNum;
	int nextActorId;

	float baseAlliances[TEAMS_MAX];

	/// Unserialized (Maybe with player saves)
	float alliances[TEAMS_MAX];
	int fortifiedByTeam;
	float fortifiedPerc;
};

enum MapVisualization {
	MAP_VISUALIZATION_SLICES,
	MAP_VISUALIZATION_BARS,
	MAP_VISUALIZATION_LOCKED_IN,
	MAP_VISUALIZATION_MAX_ALLIANCE_SURROUNDING,
};
const char *mapVisualizationStrings[] = {
	"Slices",
	"Bars",
	"Locked in",
	"Max alliance surrounding",
};

struct DebugCube {
	AABB aabb;
	int color;
};

struct WorldChannel {
	int channelId;
	Vec3 position;
};

struct AnimationMarkerData {
	char animName[PATH_MAX_LEN];
#define ANIMATION_MARKER_MAX_LEN 64
	char markerName[ANIMATION_MARKER_MAX_LEN];
	int frame;
};

enum EffectType {
	EFFECT_ENEMY_DAMAGE,
	EFFECT_PLAYER_DAMAGE,
	EFFECT_BLOCK_DAMAGE,
	EFFECT_MONEY,
};
struct Effect {
	EffectType type;
	Vec3 position;
	float time;
	float value;

#define EFFECT_TEXT_MAX_LEN 16
	char text[EFFECT_TEXT_MAX_LEN];
};

enum ParticleType {
	PARTICLE_DUST,
	PARTICLE_BLOOD,
};
struct Particle {
	ParticleType type;
	Vec3 position;
	Vec3 velo;
	int tint;
	float time;
	float maxTime;
};

struct Game {
	Font *defaultFont;
	Font *particleFont;
	Font *simpleStatsFont;
	RenderTexture *gameTexture;
	RenderTexture *debugTexture;
	RenderTexture *mapTexture;

	Globals globals;
	bool inEditor;
	float timeScale;
	float prevTime;
	float time;
	Vec2 size;
	Vec2 mouse;
	Vec2 worldMouse;

	int hitPauseFrames;

	ActorTypeInfo actorTypeInfos[ACTORS_MAX];

#define MAPS_MAX 128
	Map maps[MAPS_MAX];
	int currentMapIndex;
	int nextMapIndex;
	float nextMap_t;
	float prevMapTime;
	float mapTime;

	Vec3 cameraTarget;
	Vec3 visualCameraTarget;
	Matrix3 isoMatrix3;
	Matrix3 cameraMatrix;

	int nextActionId;

	bool lookingAtMap;
	MapVisualization mapVisualization;
	float timeTillNextCityTick;
	bool cityInited;
	int cityTicks;
	float cityTime;

	int leftToBeatTillUnlock;
	float timeTillNextSpawn;

#define WORLD_SOUNDS_MAX CHANNELS_MAX
	WorldChannel worldChannels[WORLD_SOUNDS_MAX];
	int worldChannelsNum;

	ItemTypeInfo itemTypeInfos[ITEM_TYPES_MAX];

	bool inStore;
	bool ignoreStoreOverlap;
	float storeTime;
#define STORE_ITEMS_MAX 8
	Item storeItems[STORE_ITEMS_MAX];
	int storeItemsNum;

	bool inInventory;
	int draggingItemId;

	int nextItemId; //@playerSaveSerialize

	AnimationMarkerData *animationMarkerData;
	int animationMarkerDataNum;

#define EFFECTS_MAX 128
	Effect effects[EFFECTS_MAX];
	int effectsNum;

	Particle *particles;
	int particlesNum;
	int particlesMax;

	/// Editor/debug
	int selectedActorId;

	bool debugShowFrameTimes;
	bool debugAlwaysShowWireframes;
	bool debugDrawPlayerBox;
	bool debugDrawHitboxes;
	bool debugDrawActorStatus;
	bool debugDrawActorAction;
	bool debugDrawActorStats;
	bool debugDrawActorStatsSimple;
	bool debugDrawActorFacingDirection;
	bool debugDrawActorTargets;
	bool debugDrawBillboards;
	bool debugNeverTakeDamage;
	bool debugForceRestock;
	Map *editorSelectedCityMap;
	bool debugSkipPrewarm;

	/// 3D
	Vec2i cameraAngleDegrees;
#define DEBUG_CUBES_MAX 1024
	DebugCube debugCubes[DEBUG_CUBES_MAX];
	int debugCubesNum;

	Vec3 mouseRayPos;
	Vec3 mouseRayDir;
};
Game *game = NULL;

void runGame();
void updateGame();
void stepGame(bool lastStepOfFrame, float elapsed, float timeScale);
void updateStore(Actor *player, Actor *storeActor, float elapsed);
Map *getMapByName(char *mapName);
Map *getCityMapByCoords(int x, int y);
Vec2i getCoordsByCityMap(Map *map);
int getIndexByMap(Map *map);
Actor *createActor(Map *map, ActorType type);
void deleteActor(Map *map, Actor *actor);
Actor *getActor(int id);
Actor *getActorOfType(Map *map, ActorType type);
Actor *getRandomActorOfType(Map *map, ActorType type);
Actor *getActorByName(Map *map, char *actorName);
void removeActorById(Map *map, int id);
void removeActorByIndex(Map *map, int index);
Actor *findDoorWithDestMapName(Map *map, char *destMapName);

Action *addAction(Actor *actor, ActionType type);
Buff *addBuff(Actor *actor, BuffType type, float maxTime);
bool hasBuff(Actor *actor, BuffType type);
int getBuffCount(Actor *actor, BuffType type);
void initItem(Item *item, ItemType type, int amount);
Item *giveItem(Actor *actor, ItemType type, int amount);
void removeItem(Actor *actor, ItemType type, int amount);
Item *getItem(Actor *actor, int id);
int getStashedItemCount(Actor *actor, ItemType type);
int getEquippedItemCount(Actor *actor, ItemType type);
float getStatPoints(Actor *actor, StatType stat);
int getSurroundingAtMaxAlliance(Vec2i startingIndex, int team);
int getTeamWithMostAlliance(Map *map);

Rect getBounds(AABB aabb);
AABB getAABBAtPosition(Actor *actor, Vec3 position);
AABB getAABBFromSizePosition(Vec3 size, Vec3 position);
bool overlaps(Actor *actor0, Actor *actor1);
bool overlaps(Actor *actor, AABB aabb);
float distance(Actor *actor0, Actor *actor1);
AABB getAABB(Actor *actor);
AABB bringWithinBounds(AABB groundAABB, AABB aabb);
AABB bringWithinBounds(Map *map, AABB aabb);
void bringWithinBounds(Map *map, Actor *actor);
void drawAABB3d(AABB aabb, int color);
void drawAABB2d(AABB aabb, int lineThickness, int color);

int playWorldSound(char *path, Vec3 worldPosition);
Effect *createEffect(EffectType type, Vec3 position);
Particle *createParticle(ParticleType type);

void saveMap(Map *map, int mapFileIndex);
void loadMap(Map *map, int mapFileIndex);
void loadAndRefreshMaps();

void saveGlobals();
void loadGlobals();
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
#if !defined(FALLOW_INTERNAL) // This needs to be a macro
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#else
	if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/concreteJungle/concreteJungleGameAssets");
	if (directoryExists("C:/Users/I Am The Sun King/Dropbox")) strcpy(projectAssetDir, "C:/Users/I Am The Sun King/Dropbox/concreteJungle/concreteJungleGameAssets");
	// if (directoryExists("O:/Dropbox")) strcpy(projectAssetDir, "O:/Dropbox/ZooBound/zooBoundGameAssets");
#endif
#endif

	initFileOperations();

	Vec2 res = v2(1600, 900);

	initPlatform(res.x, res.y, "Concrete Jungle");
	platform->sleepWait = true;
	initAudio();
	usesAlphaDiscard = true;
	initRenderer(res.x, res.y);
	initTextureSystem();
	initFonts();
	initAnimations();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		logf("Game runtime memory is %dmb btw\n", sizeof(Game) / Megabytes(1));
		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 40);
		game->particleFont = createFont("assets/common/arial.ttf", 25);
		game->simpleStatsFont = createFont("assets/common/arial.ttf", 18);

		animSys->loopsByDefault = true;
		animSys->frameRate = 60;

		bool reprocessSheets = false;
		for (int i = 0; i < assetPathsNum; i++) {
			char *path = assetPaths[i];
			if (strstr(path, "/needsReprocess.dummy")) {
				reprocessSheets = true;
				deleteFile(path);
			}
		}
		if (reprocessSheets) removeDirectory("assets/sheets");

		if (fileExists("assets/sheets/sheetData.bin")) {
			loadSpriteSheet("assets/sheets/sheetData.bin");
		} else {
			packSpriteSheet("assets/frames");

			if (!directoryExists("assets/sheets")) createDirectory("assets/sheets");
			saveSpriteSheets("assets/sheets");
		}

		loadGlobals();

		{ /// Animation data
			char *str = (char *)readFile("assets/frames/Unit/params.vars");
			game->animationMarkerData = (AnimationMarkerData *)zalloc(sizeof(AnimationMarkerData) * countChar(str, '\n'));

			char *lineStart = str;
			char *line = frameMalloc(512);
			for (;;) {
				char *lineEnd = strchr(lineStart, '\n');
				if (!lineEnd) break;

				int len = lineEnd - lineStart;
				strncpy(line, lineStart, len);
				line[len-1] = 0;

				AnimationMarkerData *marker = &game->animationMarkerData[game->animationMarkerDataNum++];
				char *spacePtr1 = strchr(line, ' ');
				strncpy(marker->animName, line, spacePtr1 - line);
				marker->animName[spacePtr1 - line] = 0;

				char *spacePtr2 = strchr(spacePtr1+1, ' ');
				char frameStr[8] = {};
				strncpy(frameStr, spacePtr1+1, spacePtr2 - spacePtr1 - 1);
				marker->frame = atoi(frameStr);

				char *spacePtr3 = &line[strlen(line)-1];
				strncpy(marker->markerName, spacePtr2+1, spacePtr3 - spacePtr2);

				lineStart = lineEnd+1;
			}
		} ///

		{
			ActorTypeInfo *info;

			info = &game->actorTypeInfos[ACTOR_UNIT];
			info->canBeHit = true;
			info->hasPhysics = true;

			info = &game->actorTypeInfos[ACTOR_GROUND];
			info->isWall = true;

			info = &game->actorTypeInfos[ACTOR_WALL];
			info->isWall = true;

			info = &game->actorTypeInfos[ACTOR_DUMMY];
			info->canBeHit = true;
			info->hasPhysics = true;

			info = &game->actorTypeInfos[ACTOR_DOOR];

			info = &game->actorTypeInfos[ACTOR_UNIT_SPAWNER];

			info = &game->actorTypeInfos[ACTOR_ITEM];
			info->hasPhysics = true;

			info = &game->actorTypeInfos[ACTOR_STORE];
		}

		{
			ItemTypeInfo *info = NULL;

			for (int i = 0; i < ITEM_TYPES_MAX; i++) {
				info = &game->itemTypeInfos[i];
				info->maxAmountFromStore = 1;
			}

			info = &game->itemTypeInfos[ITEM_NONE];
			strcpy(info->name, "none");
			info->maxAmountFromStore = 0;

			info = &game->itemTypeInfos[ITEM_MONEY];
			strcpy(info->name, "xp");
			info->maxAmountFromStore = 0;

			info = &game->itemTypeInfos[ITEM_HEALTH_PACK];
			strcpy(info->name, "health pack");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->price = 10;
			info->maxAmountFromStore = 10;

			info = &game->itemTypeInfos[ITEM_DAMAGE_BOOST];
			strcpy(info->name, "damage boost");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->price = 25;
			info->maxAmountFromStore = 5;

			info = &game->itemTypeInfos[ITEM_HP_BOOST];
			strcpy(info->name, "hp boost");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->price = 25;
			info->maxAmountFromStore = 5;

			info = &game->itemTypeInfos[ITEM_STAMINA_REGEN_BOOST];
			strcpy(info->name, "stamina regen boost");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->price = 25;
			info->maxAmountFromStore = 5;

			info = &game->itemTypeInfos[ITEM_MAX_STAMINA_BOOST];
			strcpy(info->name, "max stamina boost");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->price = 25;
			info->maxAmountFromStore = 5;

			info = &game->itemTypeInfos[ITEM_MOVEMENT_SPEED_BOOST];
			strcpy(info->name, "movement speed boost");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->price = 25;
			info->maxAmountFromStore = 5;

			info = &game->itemTypeInfos[ITEM_ATTACK_SPEED_BOOST];
			strcpy(info->name, "attack speed boost");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->price = 25;
			info->maxAmountFromStore = 5;

			info = &game->itemTypeInfos[ITEM_MAGNET];
			strcpy(info->name, "magnet");
			info->slotType = ITEM_SLOT_PASSIVE;
			info->price = 30;

			info = &game->itemTypeInfos[ITEM_HYPER_ARMOR];
			strcpy(info->name, "hyper armor");
			info->slotType = ITEM_SLOT_PASSIVE;
			info->price = 40;

			info = &game->itemTypeInfos[ITEM_HEAVEN_STEP];
			strcpy(info->name, "heaven step");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->price = 50;
			info->actionType = ACTION_HEAVEN_STEP;

			info = &game->itemTypeInfos[ITEM_SHADOW_STEP];
			strcpy(info->name, "shadow step");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->price = 60;
			info->actionType = ACTION_SHADOW_STEP;

			info = &game->itemTypeInfos[ITEM_BUDDHA_PALM];
			strcpy(info->name, "buddha palm");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->price = 70;
			info->actionType = ACTION_BUDDHA_PALM;

			info = &game->itemTypeInfos[ITEM_BLOOD_RAGE];
			strcpy(info->name, "blood rage");
			info->slotType = ITEM_SLOT_PASSIVE;
			info->price = 80;

			info = &game->itemTypeInfos[ITEM_DASH];
			strcpy(info->name, "dash");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->price = 90;
			info->actionType = ACTION_DASH;

			info = &game->itemTypeInfos[ITEM_BRAIN_SAP];
			strcpy(info->name, "brain sap");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->price = 100;
			info->actionType = ACTION_BRAIN_SAP;

			info = &game->itemTypeInfos[ITEM_CULLING_BLADE];
			strcpy(info->name, "culling blade");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->price = 110;
			info->actionType = ACTION_CULLING_BLADE;

			info = &game->itemTypeInfos[ITEM_STICKY_NAPALM];
			strcpy(info->name, "sticky napalm");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->price = 120;
			info->actionType = ACTION_STICKY_NAPALM;
		}

		game->particlesMax = 128;
		game->particles = (Particle *)zalloc(sizeof(Particle) * game->particlesMax);

		game->timeScale = 1;
		// game->debugDrawPlayerBox = true;
		game->debugDrawBillboards = true;
		game->debugDrawHitboxes = true;
		game->debugDrawActorStatsSimple = true;

		game->cameraAngleDegrees.x = 75;
		game->cameraAngleDegrees.y = 3;

		maximizeWindow();
	}

	Globals *globals = &game->globals;

	float timeScale = game->timeScale;

	if (game->hitPauseFrames > 0) {
		game->hitPauseFrames--;
		timeScale = 0.0001;
	}

	float elapsed = platform->elapsed * timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	game->mouse = platform->mouse;

	{ /// Resizing
		Vec2 newSize = v2(platform->windowWidth, platform->windowHeight);
		if (!equal(game->size, newSize)) {
			game->size = newSize;

			if (game->gameTexture) destroyTexture(game->gameTexture);
			game->gameTexture = NULL;
			if (game->debugTexture) destroyTexture(game->debugTexture);
			game->debugTexture = NULL;
			if (game->mapTexture) destroyTexture(game->mapTexture);
			game->mapTexture = NULL;
		}
	} ///

	if (!game->gameTexture) game->gameTexture = createRenderTexture(game->size.x, game->size.y);
	if (!game->mapTexture) game->mapTexture = createRenderTexture(game->size.x, game->size.y);

	if (!game->debugTexture) game->debugTexture = createRenderTexture(game->size.x, game->size.y);
	pushTargetTexture(game->debugTexture);
	clearRenderer();
	popTargetTexture();

	pushTargetTexture(game->gameTexture);
	clearRenderer(0xFF101010);

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;

	bool shouldDraw3d = true;

	int steps = 1;

	if (!game->debugSkipPrewarm && game->mapTime < ROOM_PREWARM_TIME) {
		steps = 20;
	}

	for (int i = 0; i < steps; i++) {
		bool lastStepOfFrame = i == (steps-1);
		stepGame(lastStepOfFrame, elapsed, timeScale);
	}

	{ /// Update world channels
		for (int i = 0; i < game->worldChannelsNum; i++) {
			WorldChannel *worldChannel = &game->worldChannels[i];
			Channel *channel = getChannel(worldChannel->channelId);
			if (!channel) {
				arraySpliceIndex(game->worldChannels, game->worldChannelsNum, sizeof(WorldChannel), i);
				game->worldChannelsNum--;
				i--;
				continue;
			}

			Vec2 screenPos = game->cameraMatrix * v2(game->isoMatrix3 * worldChannel->position);
			Vec2 screenPerc = (game->size/2 - screenPos) / game->size*2;
			float distPerc = distance(v2(), screenPerc);
			float vol = clampMap(distPerc, 0, 2, 1, 0);
			float pan = screenPerc.x;
			channel->userVolume = vol;
			channel->pan = pan;
		}
	} ///

	popTargetTexture(); // game->gameTexture

	clearRenderer();

	{
		RenderTexture *texture = game->gameTexture;
		Matrix3 matrix = mat3();
		matrix.SCALE(game->size);

		drawSimpleTexture(texture, matrix);
	}

	{ /// Draw 3d
		if (shouldDraw3d) {
			Matrix3 matrix = mat3();
			matrix.ROTATE(game->cameraAngleDegrees.y);
			matrix.ROTATE_X(game->cameraAngleDegrees.x);

			Vec3 cameraPos = (matrix * v3(0, 0, 1000)) + game->visualCameraTarget;

			Camera camera = {};
			camera.position = cameraPos;
			camera.target = game->visualCameraTarget;
			camera.up = v3(0, 0, 1);
			camera.fovy = 10;
			camera.isOrtho = true;

			start3d(camera, game->size, -10000, 10000);

#if 1
			Vec3 sunPosition = v3(104.000, -134.000, 66.000);
#else
			static Vec3 sunPosition = v3(104.000, -134.000, 66.000);
			ImGui::DragFloat3("sunPosition", &sunPosition.x, 1);
#endif
			renderer->lights[0].position.x = sunPosition.x;
			renderer->lights[0].position.y = sunPosition.y;
			renderer->lights[0].position.z = sunPosition.z;
			updateLightingShader(camera);

			getMouseRay(camera, game->mouse, &game->mouseRayPos, &game->mouseRayDir);

			{ // Really draw 3d
				Map *map = &game->maps[game->currentMapIndex];

				Actor **actors = (Actor **)frameMalloc(sizeof(Actor *) * map->actorsNum);
				int actorsNum = map->actorsNum;
				for (int i = 0; i < map->actorsNum; i++) actors[i] = &map->actors[i];

				auto qsortActors = [](const void *ptrA, const void *ptrB)->int {
					Actor *actorA = *(Actor **)ptrA;
					Actor *actorB = *(Actor **)ptrB;
					if (actorA->position.z < actorB->position.z) {
						return 1;
					} else if (actorA->position.z > actorB->position.z) {
						return -1;
					} else {
						return 0;
					}

				};
				// qsort(actors, actorsNum, sizeof(Actor *), qsortActors);

				for (int pass = 0; pass < 2; pass++) {
					if (pass == 1) {
						startShader(renderer->lightingShader);
						for (int i = 0; i < game->debugCubesNum; i++) {
							DebugCube *cube = &game->debugCubes[i];
							drawAABB(cube->aabb, cube->color);
						}
						endShader();
					}

					for (int i = 0; i < actorsNum; i++) {
						Actor *actor = actors[i];
						AABB aabb = getAABB(actor);

						int boxColor = 0xFFFFFFFF;
						bool showBox = false;
						float boxHeightPerc = 1;

						if (actor->type == ACTOR_UNIT) {
							if (game->debugDrawPlayerBox) showBox = true;
							boxColor = teamColors[actor->team];

							if (actor->actionsNum > 0 && actor->actions[0].type == ACTION_KNOCKDOWN) boxHeightPerc *= 0.5;

							if (game->debugDrawBillboards) {
								auto getMarkerFrame = [](char *animName, char *markerName)->int {
									for (int i = 0; i < game->animationMarkerDataNum; i++) {
										AnimationMarkerData *marker = &game->animationMarkerData[i];
										if (streq(marker->animName, animName) && streq(marker->markerName, markerName)) return marker->frame;
									}

									return 0;
								};

								Animation *anim = NULL;
								float animTime;
								int animFrameOverride = -1;
								if (actor->actionsNum > 0 &&
									actor->actions[0].type != ACTION_FORCED_MOVE &&
									actor->actions[0].type != ACTION_FORCED_IDLE &&
									actor->actions[0].type != ACTION_FORCED_LEAVE
								) {
									Action *action = &actor->actions[0];
									anim = getAnimation(frameSprintf("Unit/%s", action->info->animationName));
									animTime = action->time;
									if (anim) anim->loops = action->info->animationLoops;

									float actionStartupEndTime = action->info->startupFrames / 60.0;
									float actionActiveEndTime = actionStartupEndTime + (action->info->activeFrames / 60.0);
									float actionRecoveryEndTime = actionActiveEndTime + (action->info->recoveryFrames / 60.0);

									int animationStartupEndFrame = getMarkerFrame(action->info->animationName, "active");
									int animationActiveEndFrame = getMarkerFrame(action->info->animationName, "recovery");
									int animationRecoveryEndFrame = 0;
									if (anim) animationRecoveryEndFrame = anim->framesNum-1;

									if (animationStartupEndFrame != 0 || animationActiveEndFrame != 0) {
										if (action->time < actionStartupEndTime) {
											animFrameOverride = clampMap(action->time, 0, actionStartupEndTime, 0, animationStartupEndFrame);
										} else if (action->time < actionActiveEndTime) { 
											animFrameOverride = clampMap(action->time, actionStartupEndTime, actionActiveEndTime, animationStartupEndFrame, animationActiveEndFrame);
										} else {
											animFrameOverride = clampMap(action->time, actionActiveEndTime, actionRecoveryEndTime, animationActiveEndFrame, animationRecoveryEndFrame);
										}
									}
								} else {
									if (actor->timeMoving) {
										if (actor->isRunningLeft || actor->isRunningRight) {
											anim = getAnimation("Unit/run");
											animFrameOverride = anim->framesNum * actor->movementPerc;
										} else {
											anim = getAnimation("Unit/walk");
											animFrameOverride = anim->framesNum * actor->movementPerc;
										}
									} else if (actor->timeNotMoving) {
										anim = getAnimation("Unit/idle");
										animTime = actor->timeNotMoving;
									} else if (actor->timeInAir) {
										anim = getAnimation("Unit/jump");
										animTime = actor->timeInAir;
										anim->loops = false;
									} else {
										anim = getAnimation("Unit/idle");
									}
								}

								Frame *frame = NULL;

								if (anim) {
									// logf("%s (%f)\n", anim->name, animTime);
									frame = getAnimFrameAtSecond(anim, animTime);
									if (animFrameOverride != -1) frame = anim->frames[animFrameOverride];
								}

								bool flipped = false;
								float scale = globals->actorSpriteScale * globals->actorSpriteScaleMultiplier;
								Vec3 position = getCenter(aabb) + globals->actorSpriteOffset;
								if (actor->facingLeft) flipped = true;
								if (frame) {
									Rect source = {};
									source.width = frame->width;
									source.height = frame->height;
									source.x = frame->srcX;
									source.y = frame->texture->height - source.height - frame->srcY;

									Vec2 size = v2(frame->width, frame->height);
									if (flipped) size.x *= -1;
									size *= scale;

									if (actor->actionsNum > 0) {
										Action *action = &actor->actions[0];
										if (action->type == ACTION_BLOCKSTUN) { // Vibration
											float amount = clampMap(action->time, 0, action->customLength, 5, 0, QUAD_IN);
											if (platform->frameCount % 2) {
												position.x += amount;
											} else {
												position.x -= amount;
											}
										}
									}

									// position.x += frame->destOffX;
									// position.y += frame->destOffY;

									if (pass == 1) drawBillboard(camera, frame->texture, position, size, boxColor, source);
								} else {
									if (pass == 0) showBox = true;
								}
							}
						} else if (actor->type == ACTOR_GROUND) {
							showBox = true;
							boxColor = 0xFFE8C572;
						} else if (actor->type == ACTOR_DUMMY) {
							showBox = true;
							boxColor = 0xFFFF878B;
						} else if (actor->type == ACTOR_DOOR) {
							showBox = true;
							boxColor = lerpColor(0xFF523501, 0xFF121212, actor->locked);
						} else if (actor->type == ACTOR_UNIT_SPAWNER) {
							if (game->inEditor) showBox = true;
						} else if (actor->type == ACTOR_ITEM) {
							showBox = true;
							boxColor = lerpColor(0xFFFFD86B, 0xFFFFFFFF, 0.5);
						} else if (actor->type == ACTOR_STORE) {
							showBox = true;
							boxColor = 0xFF45E6E6;
						} else {
							showBox = true;
						}

						if (showBox) drawAABB3d(aabb, boxColor);
					}
				}

				{ /// Render particles
					for (int i = 0; i < game->particlesNum; i++) {
						Particle *particle = &game->particles[i];

						float fadeInPerc = 0.10;
						float fadeOutPerc = 0.10;

						if (particle->type == PARTICLE_DUST) {
						} else if (particle->type == PARTICLE_BLOOD) {
						}

						float alpha =
							clampMap(particle->time, 0, particle->maxTime*fadeInPerc, 0, 1)
							* clampMap(particle->time, particle->maxTime*(1-fadeOutPerc), particle->maxTime, 1, 0);

						{
							RenderTexture *texture = renderer->circleTexture;
							Vec2 size = v2(32, 32);
							Rect source = makeRect(texture);

							int tint = particle->tint;
							Vec4 tintVec = hexToArgbFloat(particle->tint);
							tintVec *= alpha;

							tint = argbToHex(tintVec);

							// tint = 0x80808080; //@todo Figure out why this means 50% alpha (circleTexture or billboards aren't premultiplied?)

							drawBillboard(camera, texture, particle->position, size, tint, source);
						}
					}
				} ///
			}
			game->debugCubesNum = 0;

			end3d();
		}
	} ///

	{
		RenderTexture *texture = game->debugTexture;
		Matrix3 matrix = mat3();
		matrix.SCALE(game->size);

		drawSimpleTexture(texture, matrix);
	}

	float fadeOutPerc = game->nextMap_t;
	if (!game->debugSkipPrewarm) fadeOutPerc += clampMap(game->mapTime, ROOM_PREWARM_TIME, ROOM_PREWARM_TIME+0.25, 1, 0);
	fadeOutPerc = Clamp01(fadeOutPerc);
	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, fadeOutPerc));

	{
		RenderTexture *texture = game->mapTexture;
		Matrix3 matrix = mat3();
		matrix.SCALE(game->size);

		drawSimpleTexture(texture, matrix);
	}

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) {
		char *str = frameSprintf("%.1fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
	}

	guiDraw();
	drawOnScreenLog();
}

void stepGame(bool lastStepOfFrame, float elapsed, float timeScale) {
	Globals *globals = &game->globals;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	auto getActionType = [](Actor *actor, int actionIndex)->ActionType {
		Globals *globals = &game->globals;
		Style *style = &actor->styles[actor->styleIndex];

		Item *item = NULL;
		if (actionIndex == 0) item = getItem(actor, style->activeItem0);
		if (actionIndex == 1) item = getItem(actor, style->activeItem1);
		if (!item) return ACTION_NONE;
		Assert(item->info);

		ActionTypeInfo *atInfo = &globals->actionTypeInfos[item->info->actionType];
		if (actor->isOnGround && !(atInfo->flags & _F_AT_ALLOWED_ON_GROUND)) return ACTION_NONE;
		if (!actor->isOnGround && !(atInfo->flags & _F_AT_ALLOWED_IN_AIR)) return ACTION_NONE;
		return item->info->actionType;
	};

	{ /// Init city (and maps)
		if (!game->cityInited) {
			game->cityInited = true;
			loadAndRefreshMaps();
			game->currentMapIndex = 0;
			game->mapTime = 0;
			game->timeTillNextCityTick = 0;
			game->cityTime = 0;
			game->cityTicks = 0;
			game->inStore = false;
			game->inInventory = false;
			game->lookingAtMap = false;
		}
	} ///

	{ /// Set up matrices
		game->visualCameraTarget = lerp(game->visualCameraTarget, game->cameraTarget, 0.5);
		if (distance(game->visualCameraTarget, game->cameraTarget) > 100) game->visualCameraTarget = game->cameraTarget;

		game->cameraMatrix = mat3(); // More like cameraInvMatrix?
		game->cameraMatrix.TRANSLATE(game->size/2);
		game->cameraMatrix.TRANSLATE(-v2(game->isoMatrix3 * game->visualCameraTarget));
		pushCamera2d(game->cameraMatrix);
		game->worldMouse = game->cameraMatrix.invert() * game->mouse;

		Matrix3 matrix = mat3();
		matrix.ROTATE(game->cameraAngleDegrees.y);
		matrix.ROTATE_X(game->cameraAngleDegrees.x);

		{ // Fix my 2d coordinate system
			matrix.SCALE(1, -1);
			matrix = matrix.transpose();
		}

		game->isoMatrix3 = matrix;
	} ///

	bool inRoomPrewarm = false;
	{ /// Change map
		if (game->nextMapIndex != game->currentMapIndex) {
			bool changeMaps = false;
			if (game->nextMap_t <= 0) game->lookingAtMap = true;
			game->nextMap_t += 0.1;
			if (game->nextMap_t > 1) changeMaps = true;

			if (changeMaps) {
				Map *srcMap = &game->maps[game->currentMapIndex];
				Map *destMap = &game->maps[game->nextMapIndex];
				// infof("%s\n", destMap->name);

				Actor *player = NULL;
				int playerSrcIndex = 0;
				for (int i = 0; srcMap->actorsNum; i++) {
					Actor *actor = &srcMap->actors[i];
					if (actor->type == ACTOR_UNIT && actor->playerControlled) {
						playerSrcIndex = i;
						player = actor;
						break;
					}
				}

				if (player) { // There always has to be a player???
					Actor *newActor = createActor(destMap, ACTOR_UNIT); // Should factor into moveActor()? // You really should, because them itemsPtr fixup is really weird
					int id = newActor->id;
					Item *itemsPtr = newActor->items;
					memcpy(itemsPtr, player->items, sizeof(Item) * player->itemsNum);
					*newActor = *player;
					newActor->id = id;
					newActor->items = itemsPtr;
					removeActorByIndex(srcMap, playerSrcIndex);

					Vec3 playerSpawnPos = v3();
					Actor *door = findDoorWithDestMapName(destMap, srcMap->name);
					if (!door) door = findDoorWithDestMapName(destMap, NULL);

					if (door) {
						playerSpawnPos = door->position + v3(0, 0, 1);
						door->doorPlayerSpawnedOver = true;
						door->isLastExitedDoor = true;
					}

					newActor->position = playerSpawnPos;

					bringWithinBounds(destMap, newActor);
					player = newActor;
				}

				for (int i = 0; i < srcMap->actorsNum; i++) {
					Actor *actor = &srcMap->actors[i];
					if (actor->type == ACTOR_UNIT && !actor->playerControlled) {
						deleteActor(srcMap, actor);
						i--;
						continue;
					}

					if (actor->type == ACTOR_DOOR) actor->isLastExitedDoor = false;
				}

				game->currentMapIndex = game->nextMapIndex;
				game->mapTime = 0;

				Map *map = &game->maps[game->currentMapIndex];

				int lockAmount = 0;
				{ //@copyPastedGetLockAmount
					float totalOtherAlliance = 0;
					float largestOtherAlliance = 0;
					for (int i = 0; i < TEAMS_MAX; i++) {
						if (i != player->team) {
							totalOtherAlliance += map->alliances[i];
							if (largestOtherAlliance < map->alliances[i]) largestOtherAlliance = map->alliances[i];
						}
					}
					if (totalOtherAlliance > 0.3 && map->alliances[player->team] < 0.8) {
						lockAmount = clampMap(largestOtherAlliance, 0.3, 1, 1, 15);
					}
				}
				game->leftToBeatTillUnlock = lockAmount;
			}
		} else {
			float prevNextMap_t = game->nextMap_t;
			game->nextMap_t -= 0.1;
			if (game->debugSkipPrewarm && prevNextMap_t > 0 && game->nextMap_t <= 0) game->lookingAtMap = false;
		}
		game->nextMap_t = Clamp01(game->nextMap_t);

		if (game->mapTime < ROOM_PREWARM_TIME) inRoomPrewarm = true;
		if (game->debugSkipPrewarm) inRoomPrewarm = false;

		if (game->prevMapTime < ROOM_PREWARM_TIME && game->mapTime >= ROOM_PREWARM_TIME) game->lookingAtMap = false;
	} ///

	Map *map = &game->maps[game->currentMapIndex];

	Actor *player = NULL;
	Actor *ground = NULL;
	AABB groundAABB;
	AABB *walls = (AABB *)frameMalloc(sizeof(AABB) * (ACTORS_MAX+4)); // +4 because of edge walls
	int wallsNum = 0;
	{ /// Initial iteration
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			// actor->info = &game->actorTypeInfos[actor->type]; // I don't think I need this anymore
			if (actor->type == ACTOR_UNIT && actor->playerControlled) {
				if (player) logf("Multiple players!?\n");
				player = actor;
			} else if (actor->type == ACTOR_GROUND) {
				if (ground) logf("Multiple grounds!?\n");
				ground = actor;
			}

			if (actor->info->isWall) walls[wallsNum++] = getAABB(actor);
		}

		if (!player) {
			player = createActor(map, ACTOR_UNIT);
			player->playerControlled = true;

			player->position = v3(0, 0, 2);
			logf("Player created\n");
		}

		if (!ground) {
			ground = createActor(map, ACTOR_GROUND);
			ground->position = v3(0, 0, -100);
			ground->size = v3(2000, 1800, 100);
			logf("Ground created\n");
		}

		groundAABB = getAABB(ground);
	} ///

	{ /// Add extra walls outside ground
		float wallThickness = 20;

		AABB backWall = makeAABB();
		backWall.min.x = groundAABB.min.x;
		backWall.max.x = groundAABB.max.x;

		backWall.min.y = groundAABB.max.y;
		backWall.max.y = backWall.min.y + wallThickness;

		backWall.min.z = groundAABB.min.z;
		backWall.max.z = backWall.min.z + 1000;
		walls[wallsNum++] = backWall;
		// drawAABB3d(backWall, 0xFFFF0000);

		AABB frontWall = makeAABB();
		frontWall.min.x = groundAABB.min.x;
		frontWall.max.x = groundAABB.max.x;

		frontWall.min.y = groundAABB.min.y - wallThickness;
		frontWall.max.y = frontWall.min.y + wallThickness;

		frontWall.min.z = groundAABB.min.z;
		frontWall.max.z = frontWall.min.z + 1000;
		walls[wallsNum++] = frontWall;
		// drawAABB3d(frontWall, 0xFFFF0000);

		AABB leftWall = makeAABB();
		leftWall.min.x = groundAABB.min.x - wallThickness;
		leftWall.max.x = leftWall.min.x + wallThickness;

		leftWall.min.y = groundAABB.min.y;
		leftWall.max.y = groundAABB.max.y;

		leftWall.min.z = groundAABB.min.z;
		leftWall.max.z = leftWall.min.z + 1000;
		walls[wallsNum++] = leftWall;
		// drawAABB3d(leftWall, 0xFFFF0000);

		AABB rightWall = makeAABB();
		rightWall.min.x = groundAABB.max.x;
		rightWall.max.x = rightWall.min.x + wallThickness;

		rightWall.min.y = groundAABB.min.y;
		rightWall.max.y = groundAABB.max.y;

		rightWall.min.z = groundAABB.min.z;
		rightWall.max.z = rightWall.min.z + 1000;
		walls[wallsNum++] = rightWall;
		// drawAABB3d(rightWall, 0xFFFF0000);
	} ///

	if (lastStepOfFrame && game->inEditor) { /// Editor update
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::TreeNodeEx("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Always show wireframes", &game->debugAlwaysShowWireframes);
			ImGui::Checkbox("Show player box", &game->debugDrawPlayerBox);
			ImGui::Checkbox("Draw hitboxes", &game->debugDrawHitboxes);
			ImGui::Checkbox("Draw actor status", &game->debugDrawActorStatus);
			ImGui::Checkbox("Draw actor action", &game->debugDrawActorAction);
			ImGui::Checkbox("Draw actor stats", &game->debugDrawActorStats);
			ImGui::Checkbox("Draw actor stats simple", &game->debugDrawActorStatsSimple);
			ImGui::Checkbox("Draw actor facing directions", &game->debugDrawActorFacingDirection);
			ImGui::Checkbox("Draw actor targets", &game->debugDrawActorTargets);
			ImGui::Checkbox("Draw billboards", &game->debugDrawBillboards);
			ImGui::Checkbox("Skip prewarm", &game->debugSkipPrewarm);

			ImGui::SliderInt("Degs1", &game->cameraAngleDegrees.x, 0, 90);
			ImGui::SliderInt("Degs2", &game->cameraAngleDegrees.y, -90, 90);

			ImGui::Text("timeSinceLastLeftPress: %f", player->timeSinceLastLeftPress);
			ImGui::Text("timeSinceLastRightPress: %f", player->timeSinceLastRightPress);
			ImGui::Text("isRunningLeft: %d", player->isRunningLeft);
			ImGui::Text("isRunningRight: %d", player->isRunningRight);

			if (ImGui::Button("Spawn enemy")) {
				Actor *actor = createActor(map, ACTOR_UNIT);
				actor->team = 1;
				actor->position = v3(0, 0, 10);
			}
			ImGui::SameLine();
			if (ImGui::Button("Spawn dummy")) {
				Actor *actor = createActor(map, ACTOR_UNIT);
				actor->team = 1;
				actor->position = v3(0, 0, 10);
				actor->aiType = AI_DUMMY;
			}

			if (ImGui::Button("Kill enemies")) {
				for (int i = 0; i < map->actorsNum; i++) {
					Actor *actor = &map->actors[i];
					if (actor->type == ACTOR_UNIT && actor->team != player->team) actor->hp = 0;
				}
			}

			if (ImGui::Button("Give all items")) {
				for (int i = 1; i < ITEM_TYPES_MAX; i++) {
					giveItem(player, (ItemType)i, 1);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Give $100")) {
				player->money += 100;
			}

			if (ImGui::Button("Force restock")) {
				game->debugForceRestock = true;
			}

			if (ImGui::Button("Set all hps to 1")) {
				for (int i = 0; i < map->actorsNum; i++) {
					Actor *actor = &map->actors[i];
					if (actor->type == ACTOR_UNIT) actor->hp = 1;
				}
			}

			ImGui::Checkbox("Never take damage", &game->debugNeverTakeDamage);
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("City", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button("Reapply templates to all")) {
				if (CITY_START_INDEX + (CITY_ROWS * CITY_COLS) > MAPS_MAX-1) Panic("Not enough maps...\n");
				for (int y = 0; y < CITY_ROWS; y++) {
					for (int x = 0; x < CITY_COLS; x++) {
						int cityIndex = y * CITY_COLS + x;
						int mapIndex = cityIndex + CITY_START_INDEX;
						Map *map = &game->maps[mapIndex];
						if (!map->isTemplatized) continue;

						Map *templateMap = NULL;
						char *choices[] = {
							"#basicTemplate",
							"#cornerBuildings",
							"#longRoom",
						};
						templateMap = getMapByName(choices[rndInt(0, ArrayLength(choices)-1)]);

						float baseAlliances[TEAMS_MAX];
						memcpy(baseAlliances, map->baseAlliances, sizeof(float) * TEAMS_MAX);
						*map = *templateMap;

						strcpy(map->name, frameSprintf("city%d-%d", x, y));
						map->isTemplatized = true;
						memcpy(map->baseAlliances, baseAlliances, sizeof(float) * TEAMS_MAX);

						if (streq(templateMap->name, "#cornerBuildings")) {
							Actor *building = NULL;

							building = getActorByName(map, "leftBuilding");
							building->size.x *= rndFloat(0.5, 1);
							building->size.y *= rndFloat(0.5, 1);
							building->position.x -= 2000;
							building->position.y += 2000;
							bringWithinBounds(map, building);

							building = getActorByName(map, "rightBuilding");
							building->size.x *= rndFloat(0.5, 1);
							building->size.y *= rndFloat(0.5, 1);
							building->position.x += 2000;
							building->position.y += 2000;
							bringWithinBounds(map, building);
						}

						{ /// Fix doors
							Actor *door;
							door = getActorByName(map, "leftDoor");
							if (x > 0) {
								strcpy(door->destMapName, frameSprintf("city%d-%d", x-1, y));
							} else {
								deleteActor(map, door);
							}

							door = getActorByName(map, "rightDoor");
							if (x < CITY_COLS-1) {
								strcpy(door->destMapName, frameSprintf("city%d-%d", x+1, y));
							} else {
								deleteActor(map, door);
							}

							door = getActorByName(map, "topDoor");
							if (y > 0) {
								strcpy(door->destMapName, frameSprintf("city%d-%d", x, y-1));
							} else {
								deleteActor(map, door);
							}

							door = getActorByName(map, "bottomDoor");
							if (y < CITY_ROWS-1) {
								strcpy(door->destMapName, frameSprintf("city%d-%d", x, y+1));
							} else {
								deleteActor(map, door);
							}
						} ///

						saveMap(map, mapIndex);
					}
				}
				logf("Maps templatized\n");
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Map", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button("Change map")) ImGui::OpenPopup("changeMapPopup");
			if (ImGui::BeginPopup("changeMapPopup")) {
				for (int i = 0; i < MAPS_MAX; i++) {
					Map *map = &game->maps[i];
					ImGui::Text("%d: %s", i, map->name);
					// if (ImGui::ImageButton((ImTextureID)(intptr_t)map->thumbnail->id, ImVec2(map->thumbnail->width, map->thumbnail->height), ImVec2(0, 1), ImVec2(1, 0)))
					if (ImGui::Button(frameSprintf("%d: %s", i, map->name))) {
						if (game->currentMapIndex != i) {
							game->nextMapIndex = i;
							ImGui::CloseCurrentPopup();
						}
					}
				}
				ImGui::EndPopup();
			}

			if (keyPressed('S') && ImGui::Button("Save all maps")) {
				for (int i = 0; i < MAPS_MAX; i++) saveMap(&game->maps[i], i);
			}
			ImGui::Checkbox("Templatized", &map->isTemplatized);
			if (!map->isTemplatized) {
				if (ImGui::Button("Save map")) saveMap(map, game->currentMapIndex);
				ImGui::SameLine();
				if (ImGui::Button("Load map")) {
					loadMap(map, game->currentMapIndex);
					// game->nextMapIndex = game->currentMapIndex;
				}
			}
			ImGui::InputText("Name", map->name, MAP_NAME_MAX_LEN);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Past actions")) {
			for (int i = 0; i < player->pastActionsNum; i++) {
				Action *action = &player->pastActions[i];
				ImGui::Text("%d: %s", i, action->info->name);
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Globals")) {
			if (ImGui::Button("Save globals")) saveGlobals();
			ImGui::SameLine();
			if (ImGui::Button("Load globals")) loadGlobals();
			if (ImGui::TreeNode("Action type infos")) {
				if (ImGui::Button("Remove player actions")) player->actionsNum = 0;
				for (int i = 0; i < ACTION_TYPES_MAX; i++) {
					if (i >= ACTION_EXTRA_4 && i <= ACTION_EXTRA_8) continue;
					ActionTypeInfo *info = &globals->actionTypeInfos[i];
					if (ImGui::TreeNode(frameSprintf("%s###%d", info->name, i))) {
						guiPushStyleColor(ImGuiCol_FrameBg, lerpColor(0xFF000000|stringHash32(info->name), 0xFF000000, 0.5));

						ImGui::InputText("Name", info->name, ACTION_NAME_MAX_LEN);
						ImGui::CheckboxFlags("Allowed on ground", &info->flags, _F_AT_ALLOWED_ON_GROUND);
						ImGui::SameLine();
						ImGui::CheckboxFlags("Allowed in air", &info->flags, _F_AT_ALLOWED_IN_AIR);
						if (ImGui::TreeNode("Frame data")) {
							ImGui::InputInt("Startup frames", &info->startupFrames);
							ImGui::InputInt("Active frames", &info->activeFrames);
							ImGui::InputInt("Recovery frames", &info->recoveryFrames);
							ImGui::InputInt("Hitstun frames", &info->hitstunFrames);
							ImGui::InputInt("Blockstun frames", &info->blockstunFrames);
							ImGui::Text("Adv: %d/%d", info->hitstunFrames - info->recoveryFrames, info->blockstunFrames - info->recoveryFrames);
							ImGui::TreePop();
						}
						ImGui::PushItemWidth(100);
						ImGui::InputText("Animation name", info->animationName, PATH_MAX_LEN);
						ImGui::PopItemWidth();
						ImGui::SameLine();
						ImGui::Checkbox("Loops", &info->animationLoops);

						for (int i = 0; i < info->hitboxesNum; i++) {
							ImGui::DragFloat3(frameSprintf("Hitbox %d min", i), &info->hitboxes[i].min.x);
							// {
							// 	ImGui::SameLine();
							// 	Vec3 size = getSize(info->hitboxes[i]);
							// 	ImGui::Text("(%.0fx%.0f px in 2d space)", size.x, size.z);
							// }
							ImGui::DragFloat3(frameSprintf("Hitbox %d max", i), &info->hitboxes[i].max.x);
						}
						ImGui::InputInt("Hitboxes num", &info->hitboxesNum);
						info->hitboxesNum = mathClamp(info->hitboxesNum, 0, HITBOXES_MAX);

						if (ImGui::TreeNode("Velos")) {
							ImGui::DragFloat3("Hit velo", &info->hitVelo.x);
							ImGui::DragFloat3("Block velo", &info->blockVelo.x);
							ImGui::DragFloat3("Thrust", &info->thrust.x);
							ImGui::InputInt("Thrust frame", &info->thrustFrame);
							ImGui::TreePop();
						}
						ImGui::InputFloat("Damage", &info->damage);
						ImGui::InputFloat("Stamina usage", &info->staminaUsage);
						if (ImGui::TreeNode("Buffs")) {
							ImGui::Combo("Buff to get", (int *)&info->buffToGet, buffTypeStrings, ArrayLength(buffTypeStrings));
							if (info->buffToGet) ImGui::InputFloat("Buff to get time", &info->buffToGetTime, 0);
							ImGui::Combo("Buff to give", (int *)&info->buffToGive, buffTypeStrings, ArrayLength(buffTypeStrings));
							if (info->buffToGive) ImGui::InputFloat("Buff to give time", &info->buffToGiveTime, 0);
						}

						if (ImGui::Button("Do action")) addAction(player, (ActionType)i);

						if (keyPressed(KEY_SHIFT) && keyPressed(KEY_CTRL) && ImGui::Button("Copy from punch")) {
							char *name = frameStringClone(info->name);
							*info = globals->actionTypeInfos[ACTION_PUNCH];
							strcpy(info->name, name);
						}

						guiPopStyleColor();
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			ImGui::DragFloat3("Actor sprite offset", &globals->actorSpriteOffset.x);
			ImGui::DragFloat("Actor sprite scale", &globals->actorSpriteScale, 0.01);
			ImGui::DragFloat("Actor sprite scale multiplier", &globals->actorSpriteScaleMultiplier, 0.01);
			ImGui::DragFloat("Movement perc distance walking ratio", &globals->movementPercDistanceWalkingRatio, 0.01);
			ImGui::DragFloat("Movement perc distance running ratio", &globals->movementPercDistanceRunningRatio, 0.01);
			ImGui::TreePop();
		}
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth, 0), ImGuiCond_Always, ImVec2(1, 0));
		ImGui::Begin("Actors", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::BeginChild("ActorListChild", ImVec2(300, 200));
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			char *name = frameSprintf("%s###%d", actorTypeStrings[actor->type], i);
			if (ImGui::Selectable(name, game->selectedActorId == actor->id)) {
				game->selectedActorId = actor->id;
			}
		}
		ImGui::EndChild();

		ImGui::Text("Create:");
		if (ImGui::Button("Spawn point")) {
			Actor *actor = createActor(map, ACTOR_UNIT_SPAWNER);
			actor->position = game->cameraTarget;
			game->selectedActorId = actor->id;
		}

		ImGui::SameLine();
		if (ImGui::Button("Wall")) {
			Actor *actor = createActor(map, ACTOR_WALL);
			actor->position = game->cameraTarget;
			game->selectedActorId = actor->id;
		}

		ImGui::SameLine();
		if (ImGui::Button("Door")) {
			Actor *actor = createActor(map, ACTOR_DOOR);
			actor->position = game->cameraTarget;
			game->selectedActorId = actor->id;
		}

		ImGui::SameLine();
		if (ImGui::Button("Store")) {
			Actor *actor = createActor(map, ACTOR_STORE);
			actor->position = game->cameraTarget;
			game->selectedActorId = actor->id;
		}

		ImGui::Separator();
		ImGui::Separator();

		Actor *actor = getActor(game->selectedActorId);
		if (actor) {
			if (ImGui::Button("Delete actor")) actor->markedForDeletion = true;
			if (ImGui::Button("Duplicate actor")) {
				Actor *newActor = createActor(map, actor->type);
				int id = newActor->id;
				*newActor = *actor;
				newActor->id = id;
				game->selectedActorId = newActor->id;
				logf("Dupped\n");
			}

			ImGui::Combo("Actor type", (int *)&actor->type, actorTypeStrings, ArrayLength(actorTypeStrings));
			// ImGui::InputText("Name", actor->name, ACTOR_NAME_MAX_LEN);
			// ImGui::SameLine();
			ImGui::Text("Id: %d", actor->id);
			ImGui::InputText("Name", actor->name, ACTOR_NAME_MAX_LEN);
			ImGui::DragFloat3("Position", &actor->position.x);
			ImGui::DragFloat3("Size", &actor->size.x);

			if (actor->type == ACTOR_UNIT || actor->type == ACTOR_DUMMY) {
				ImGui::InputInt("Team", &actor->team);
				if (ImGui::TreeNode("Stats")) {
					for (int i = 0; i < STATS_MAX; i++) {
						ImGui::InputFloat(statStrings[i], &actor->stats[i], 1);
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode(frameSprintf("Items (%d)###itemTree", actor->itemsNum))) {
					char **itemTypeStrings = (char **)frameMalloc(sizeof(char *) * ITEM_TYPES_MAX);
					for (int i = 0; i < ITEM_TYPES_MAX; i++) itemTypeStrings[i] = frameStringClone(game->itemTypeInfos[i].name);

					for (int i = 0; i < actor->itemsNum; i++) {
						Item *item = &actor->items[i];
						ImGui::PushID(i);
						if (ImGui::Combo("Type", (int *)&item->type, itemTypeStrings, ITEM_TYPES_MAX)) {
							item->info = &game->itemTypeInfos[item->type];
						}
						ImGui::InputInt("Amount", &item->amount);
						ImGui::PopID();
					}
					if (ImGui::Button("Give item")) {
						giveItem(actor, ITEM_NONE, 1);
					}
					ImGui::TreePop();
				}
			}

			if (actor->type == ACTOR_DOOR) {
				ImGui::InputText("Destination map", actor->destMapName, MAP_NAME_MAX_LEN);
			}

			if (actor->type == ACTOR_UNIT_SPAWNER) {
				ImGui::InputInt("Unit to spawn", &actor->unitsToSpawn);
			}

			if (ImGui::Button("bringWithinBounds")) bringWithinBounds(map, actor);

			ImGui::Separator();
		}
		ImGui::End();

		bool selectedSomething = false;
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			AABB aabb = getAABB(actor);
			Rect bounds = getBounds(aabb);

			if (platform->mouseJustUp && contains(bounds, game->worldMouse)) {
				game->selectedActorId = actor->id;
				selectedSomething = true;
			}
		}

		if (platform->mouseJustUp && !selectedSomething) {
			game->selectedActorId = 0;
		}

		float scrollSpeed = 10;
		if (keyPressed(KEY_LEFT) || keyPressed('A')) game->cameraTarget.x -= scrollSpeed;
		if (keyPressed(KEY_RIGHT) || keyPressed('D')) game->cameraTarget.x += scrollSpeed;
		if (keyPressed(KEY_UP) || keyPressed('W')) game->cameraTarget.y += scrollSpeed;
		if (keyPressed(KEY_DOWN) || keyPressed('S')) game->cameraTarget.y -= scrollSpeed;
	} /// 

	for (int i = 0; i < map->actorsNum; i++) { /// Update actors
		Actor *actor = &map->actors[i];
		if (actor->prevIsOnGround != actor->isOnGround) { // Landing on the ground
			actor->prevIsOnGround = actor->isOnGround;
			if (actor->isOnGround) {
				if (actor->actionsNum > 0) {
					Action *action = &actor->actions[0];
					if (action->type == ACTION_HITSTUN) {
						float timeLeft = action->customLength - action->time;
						actor->actionsNum = 0;
						Action *knockdownAction = addAction(actor, ACTION_KNOCKDOWN);
						knockdownAction->customLength = timeLeft;
						playWorldSound("assets/audio/fallThump.ogg", getCenter(getAABB(actor)));
					} else if (action->type == ACTION_KNOCKDOWN) {
						// Nothing...
					} else if (action->info->activeFrames) { // Cancel attacks upon landing, but not other actions
						actor->actionsNum = 0;
					}
				}
			}
		}

		bool canInput = true;
		{ /// Update action
			if (actor->actionsNum > 0) {
				Action *action = &actor->actions[0];
				canInput = false;
				if (action->type != ACTION_BLOCKSTUN) actor->isBlocking = false;

				bool actionDone = false;
				bool canDoAction = true;

				if (action->time == 0) {
					actor->stamina -= action->info->staminaUsage;
					if (actor->stamina <= 0 && action->info->staminaUsage) {
						canDoAction = false;
						actionDone = true;
					}

					if (action->info->buffToGet != BUFF_NONE) {
						addBuff(actor, action->info->buffToGet, action->info->buffToGetTime);
					}
				}

				float activeMin = action->info->startupFrames / 60.0;
				float activeMax = activeMin + (action->info->activeFrames / 60.0);

				bool inActiveTime = true;
				if (canDoAction) {
					int justHitFrame = -1;
					{ // Compute justHitFrame
						int currentFrame = action->time * 60.0;
						int prevFrame = action->prevTime * 60.0;
						if (currentFrame == prevFrame+1) {
							justHitFrame = currentFrame;
						}
						if (action->time == 0) justHitFrame = 0;
					}

					if (justHitFrame == action->info->thrustFrame) {
						Vec3 velo = action->info->thrust;
						if (actor->facingLeft) velo.x *= -1;
						actor->velo += velo;
					}

					if (justHitFrame == action->info->startupFrames+1) {
						if (action->type == ACTION_SHADOW_STEP) {
							Actor *closestActor = NULL;
							float closestDist = 0;
							for (int i = 0; i < map->actorsNum; i++) {
								Actor *otherActor = &map->actors[i];
								bool onCorrectSide = false;
								if (otherActor->position.x < actor->position.x && actor->facingLeft) onCorrectSide = true;
								if (otherActor->position.x > actor->position.x && !actor->facingLeft) onCorrectSide = true;
								if (!onCorrectSide) continue;
								if (otherActor->team == actor->team) continue;
								float dist = distance(otherActor->position, actor->position);
								if (!closestActor || dist < closestDist) {
									closestActor = otherActor;
									closestDist = dist;
								}
							}

							if (closestActor) {
								Vec3 dir = normalize(closestActor->position - actor->position);
								float edgeDist = closestDist - actor->size.x/2 - closestActor->size.x/2;
								Vec3 newPosition = actor->position + (dir*(edgeDist*0.99));
								AABB newAABB = getAABBAtPosition(actor, newPosition);

								bool onWall = false;
								for (int i = 0; i < wallsNum; i++) {
									AABB wallAABB = walls[i];
									if (intersects(wallAABB, newAABB)) {
										onWall = true;
										break;
									}
								}

								if (!onWall) {
									actor->position = newPosition;
								}
							}
						}
					}

					if (action->info->activeFrames && action->time >= activeMin && action->time <= activeMax) {
						inActiveTime = true;
						AABB actorAABB = getAABB(actor);
						Vec3 actorCenter = getCenter(actorAABB);

						if (action->prevTime < activeMin) playWorldSound("assets/audio/attack.ogg", actorCenter);

						AABB worldSpaceHitboxs[HITBOXES_MAX];
						int worldSpaceHitboxesNum = 0;
						for (int i = 0; i < action->info->hitboxesNum; i++) {
							AABB hitbox = action->info->hitboxes[i];
							if (actor->facingLeft) {
								Vec3 center = getCenter(hitbox);
								hitbox -= v3(center.x, 0, 0)*2;
							}
							hitbox += actorCenter;
							if (game->debugDrawHitboxes) drawAABB3d(hitbox, 0xFFFF0000);
							worldSpaceHitboxs[worldSpaceHitboxesNum++] = hitbox;
						}

						for (int i = 0; i < map->actorsNum; i++) {
							Actor *otherActor = &map->actors[i];
							if (!otherActor->info->canBeHit) continue;
							if (otherActor->team == actor->team) continue;
							if (otherActor->playerControlled && inRoomPrewarm) continue;
							ActionType otherActorActionType = ACTION_NONE;
							if (otherActor->actionsNum != 0) otherActorActionType = otherActor->actions[0].type;

							if (otherActorActionType == ACTION_KNOCKDOWN) continue;

							bool alreadyBeenHit = false;
							for (int i = 0; i < ACTION_IDS_HIT_BY_MAX; i++) {
								if (otherActor->actionIdsHitBy[i] == action->id) alreadyBeenHit = true;
							}
							if (alreadyBeenHit) continue;

							bool hitboxUsed = false;
							AABB otherActorAABB = getAABB(otherActor);
							for (int i = 0; i < worldSpaceHitboxesNum; i++) {
								AABB hitbox = worldSpaceHitboxs[i];
								if (intersects(otherActorAABB, hitbox)) {

									memmove(&otherActor->actionIdsHitBy[1], &otherActor->actionIdsHitBy[0], sizeof(int) * (ACTION_IDS_HIT_BY_MAX-1));
									otherActor->actionIdsHitBy[0] = action->id;

									float damage = getStatPoints(actor, STAT_DAMAGE) * action->info->damage;

									int particleColor;
									int particlesAmount = 0;

									if (otherActor->isBlocking) {
										playWorldSound("assets/audio/block.ogg", getCenter(otherActorAABB));
										particleColor = 0x80606060;
										particlesAmount = damage;

										otherActor->stamina -= damage;
										if (otherActor->stamina < 0) otherActor->stamina -= damage;

										Effect *effect = createEffect(EFFECT_BLOCK_DAMAGE, otherActor->position + v3(0, 0, otherActor->size.z*0.5));
										effect->value = damage;

										Vec3 blockVelo = action->info->blockVelo;
										if (isZero(blockVelo)) blockVelo = action->info->hitVelo * 0.25;
										if (actor->facingLeft) blockVelo.x *= -1;
										otherActor->velo += blockVelo;

										otherActor->actionsNum = 0;
										Action *newAction = addAction(otherActor, ACTION_BLOCKSTUN);
										newAction->customLength = action->info->blockstunFrames/60.0;
										if (newAction->customLength == 0) newAction->customLength = action->info->hitstunFrames/60.0;
									} else {
										particleColor = 0x80FF0000;
										particlesAmount = damage;

										if (damage >= 20) playWorldSound("assets/audio/hit/4.ogg", getCenter(otherActorAABB));
										else if (damage >= 10) playWorldSound("assets/audio/hit/3.ogg", getCenter(otherActorAABB));
										else if (damage >= 5) playWorldSound("assets/audio/hit/2.ogg", getCenter(otherActorAABB));
										else playWorldSound("assets/audio/hit/1.ogg", getCenter(otherActorAABB));

										if (actor->playerControlled || otherActor->playerControlled) game->hitPauseFrames = action->info->damage;
										otherActor->hp -= damage;

										Effect *effect = createEffect(
											otherActor == player ? EFFECT_PLAYER_DAMAGE: EFFECT_ENEMY_DAMAGE,
											otherActor->position + v3(0, 0, otherActor->size.z*0.5)
										);
										effect->value = damage;

										if (action->type == ACTION_BRAIN_SAP) {
											actor->hp += damage;
										} else if (action->type == ACTION_CULLING_BLADE) {
											float cullPerc = 0.5;
											if (otherActor->hp/otherActor->maxHp < cullPerc) otherActor->hp = 0;
										} else if (action->type == ACTION_STICKY_NAPALM) {
											addBuff(otherActor, BUFF_STICKY_NAPALM_STACK, 5);
										}

										Vec3 hitVelo = action->info->hitVelo;
										if (actor->facingLeft) hitVelo.x *= -1;
										otherActor->velo += hitVelo;

										otherActor->actionsNum = 0;
										Action *newAction = addAction(otherActor, ACTION_HITSTUN);
										newAction->customLength = action->info->hitstunFrames/60.0;

										if (action->info->buffToGive != BUFF_NONE) addBuff(otherActor, action->info->buffToGive, action->info->buffToGiveTime);
									}

									for (int i = 0; i < particlesAmount; i++) {
										Particle *particle = createParticle(PARTICLE_DUST);
										particle->position = getCenter(getAABB(otherActor));
										particle->position.y -= 0.01*i;
										particle->velo.x = rndFloat(0, 10);
										if (otherActor->position.x < actor->position.x) particle->velo.x *= -1;
										particle->velo.z = rndFloat(-5, 5);
										particle->maxTime = rndFloat(0.1, 0.25);
										particle->tint = particleColor;
									}

									hitboxUsed = true;
									break;
								}
							}

							if (hitboxUsed) break;
						}
					}
				}

				bool usesCustomLength = false;
				if (
					action->type == ACTION_HITSTUN ||
					action->type == ACTION_KNOCKDOWN ||
					action->type == ACTION_BLOCKSTUN ||
					action->type == ACTION_FORCED_IDLE ||
					action->type == ACTION_FORCED_MOVE || 
					action->type == ACTION_FORCED_LEAVE
				) usesCustomLength = true;

				float maxTime;
				if (usesCustomLength) {
					maxTime = action->customLength;
				} else {
					maxTime = (action->info->startupFrames + action->info->activeFrames + action->info->recoveryFrames) / 60.0;
				}

				if (action->type == ACTION_FORCED_MOVE) {
					maxTime = 9999;

					Vec3 dir = normalize(action->targetPosition - actor->position);

					float baseSpeed = getStatPoints(actor, STAT_MOVEMENT_SPEED) * 0.2;
					Vec2 speed = v2(baseSpeed, baseSpeed);
					actor->movementAccel.x += dir.x * speed.x;
					actor->movementAccel.y += dir.y * speed.y;

					if (distance(actor->position, action->targetPosition) < 20) actionDone = true;
				}

				if (action->time >= maxTime) actionDone = true;
				if (actionDone) {
					if (actor->pastActionsNum > ACTIONS_MAX-1) {
						memmove(&actor->pastActions[0], &actor->pastActions[1], sizeof(Action) * ACTIONS_MAX-1);
						actor->pastActionsNum--;
					} 
					actor->pastActions[actor->pastActionsNum++] = actor->actions[0];

					if (action->type == ACTION_KNOCKDOWN) addAction(actor, ACTION_RAISING);

					arraySpliceIndex(actor->actions, actor->actionsNum, sizeof(Action), 0);
					actor->actionsNum--;
				}

				float actionTimeScale = getStatPoints(actor, STAT_ATTACK_SPEED)*0.1;
				if (getEquippedItemCount(actor, ITEM_BLOOD_RAGE)) actionTimeScale *= clampMap(actor->hp/actor->maxHp, 0.5, 0.2, 1, 2);
				if (action->time < activeMin && action->time + elapsed*actionTimeScale > activeMax) actionTimeScale = 1;

				action->prevTime = action->time;
				action->time += elapsed * actionTimeScale;

				actor->timeWithoutAction = 0;
			} else { 
				actor->timeWithoutAction += elapsed;
				if (actor->timeWithoutAction > 0.5) actor->pastActionsNum = 0;
			}
		} ///

		{ /// Update buffs
			for (int i = 0; i < actor->buffsNum; i++) {
				Buff *buff = &actor->buffs[i];
				bool buffDone = false;

				if (buff->time > buff->maxTime) buffDone = true;

				buff->time += elapsed;

				if (buffDone) {
					arraySpliceIndex(actor->buffs, actor->buffsNum, sizeof(Buff), i);
					actor->buffsNum--;
					i--;
				}
			}
		} ///

		if (actor->type == ACTOR_UNIT) {
			float baseSpeed = getStatPoints(actor, STAT_MOVEMENT_SPEED) * 0.2;
			Vec2 speed = v2(baseSpeed, baseSpeed);
			if (actor->isRunningLeft || actor->isRunningRight) speed *= 2;
			for (int i = 0; i < getBuffCount(actor, BUFF_BUDDHA_PALM_SLOW); i++) speed *= 0.5;
			for (int i = 0; i < getBuffCount(actor, BUFF_STICKY_NAPALM_STACK); i++) speed *= 0.8;
			actor->isBlocking = true;
			if (actor->stamina <= BLOCKING_STAMINA_THRESHOLD) actor->isBlocking = false;

			if (actor->isRunningLeft || actor->isRunningRight) {
				if (fmod(game->time, 0.2) < fmod(game->prevTime, 0.2)) {
					Particle *particle = createParticle(PARTICLE_DUST);
					particle->position = actor->position + v3(0, 0, 5);
					particle->position.y -= 0.01*i;
					particle->velo.x = rndFloat(0, 5);
					if (actor->facingLeft) particle->velo.x *= -1;
					particle->velo.z = rndFloat(2, 3);
					particle->maxTime = rndFloat(0.5, 1);
					particle->tint = 0xFFDEA404;
				}
			}

			if (actor->playerControlled) {
				if (!game->inEditor) game->cameraTarget = actor->position;

				Vec2 inputVec = v2();
				bool jumpPressed = false;
				bool punchPressed = false;
				bool kickPressed = false;
				bool special1Pressed = false;
				bool special2Pressed = false;
				bool changeStyle1Pressed = false;
				bool changeStyle2Pressed = false;
				bool changeStyle3Pressed = false;
				bool changeStyle4Pressed = false;
				if (!game->inEditor && !inRoomPrewarm && canInput && actor->stamina > 0) {
					if (keyPressed('W') || keyPressed(KEY_UP)) inputVec.y++;
					if (keyPressed('S') || keyPressed(KEY_DOWN)) inputVec.y--;
					if (keyPressed('A') || keyPressed(KEY_LEFT)) inputVec.x--;
					if (keyPressed('D') || keyPressed(KEY_RIGHT)) inputVec.x++;
					inputVec = inputVec.normalize();
					if (keyJustPressed(' ')) jumpPressed = true;
					if (keyJustPressed('J')) punchPressed = true;
					if (keyJustPressed('K')) kickPressed = true;
					if (keyJustPressed('U')) special1Pressed = true;
					if (keyJustPressed('I')) special2Pressed = true;
					if (keyJustPressed('1')) changeStyle1Pressed = true;
					if (keyJustPressed('2')) changeStyle2Pressed = true;
					if (keyJustPressed('3')) changeStyle3Pressed = true;
					if (keyJustPressed('4')) changeStyle4Pressed = true;
				}

				{ // Figure out running
					if (inputVec.x < -0.5) {
						if (actor->prevInputVec.x >= -0.5) {
							if (actor->timeSinceLastLeftPress < 0.25) {
								actor->isRunningLeft = true;
							}
							actor->timeSinceLastLeftPress = 0;
						}
					} else {
						actor->isRunningLeft = false;
					}

					if (inputVec.x > 0.5) {
						if (actor->prevInputVec.x <= 0.5) {
							if (actor->timeSinceLastRightPress < 0.25) {
								actor->isRunningRight = true;
							}
							actor->timeSinceLastRightPress = 0;
						}
					} else {
						actor->isRunningRight = false;
					}
				}

				actor->movementAccel.x += inputVec.x * speed.x;
				actor->movementAccel.y += inputVec.y * speed.y;

				actor->prevInputVec = inputVec;

				if (jumpPressed && actor->isOnGround) actor->movementAccel.z += 20;

				if (punchPressed) {
					if (actor->isOnGround) {
						bool didAttack = false;

						if (actor->isRunningLeft || actor->isRunningRight) {
							addAction(actor, ACTION_RUNNING_PUNCH);
							didAttack = true;
						}

						if (!didAttack && actor->pastActionsNum >= 2) {
							bool arePunches = true;
							for (int i = actor->pastActionsNum-2; i < actor->pastActionsNum; i++) {
								Action *action = &actor->pastActions[i];
								if (action->type != ACTION_PUNCH) arePunches = false;
							}

							if (arePunches) {
								addAction(actor, ACTION_UPPERCUT);
								didAttack = true;
							}
						}
						if (!didAttack) addAction(actor, ACTION_PUNCH);
					} else {
						addAction(actor, ACTION_AIR_PUNCH);
					}
				}

				if (kickPressed) {
					if (actor->isOnGround) {
						bool didAttack = false;

						if (actor->isRunningLeft || actor->isRunningRight) {
							addAction(actor, ACTION_RUNNING_KICK);
							didAttack = true;
						}

						if (!didAttack) addAction(actor, ACTION_KICK);
					} else {
						addAction(actor, ACTION_AIR_KICK);
					}
				}

				if (special1Pressed) {
					ActionType actionType = getActionType(actor, 0);
					if (actionType != ACTION_NONE) addAction(actor, actionType);
				}

				if (special2Pressed) {
					ActionType actionType = getActionType(actor, 1);
					if (actionType != ACTION_NONE) addAction(actor, actionType);
				}

				if (changeStyle1Pressed) {
					infof("Style 1\n");
					actor->styleIndex = 0;
				}

				if (changeStyle2Pressed) {
					infof("Style 2\n");
					actor->styleIndex = 1;
				}

				if (changeStyle3Pressed) {
					infof("Style 3\n");
					actor->styleIndex = 2;
				}

				if (changeStyle4Pressed) {
					infof("Style 4\n");
					actor->styleIndex = 3;
				}

				actor->timeSinceLastLeftPress += elapsed;
				actor->timeSinceLastRightPress += elapsed;
			} else {
				{ /// Do enemy AI @todo factor this out
					float aggression = 0.5;

					if (actor->aiType == AI_NORMAL) {
						auto getAttackers = [](Actor *src, int *attackersNumOut)->int * {
							Map *map = &game->maps[game->currentMapIndex];

							int *ids = (int *)frameMalloc(sizeof(int) * ACTORS_MAX);
							int idsNum = 0;
							for (int i = 0; i < map->actorsNum; i++) {
								Actor *otherActor = &map->actors[i];
								if (otherActor->aiTarget == src->id) ids[idsNum++] = otherActor->id;
							}

							*attackersNumOut = idsNum;
							return ids;
						};

						if (actor->actionsNum == 0) { // This is the same as canInput
							Actor *target = getActor(actor->aiTarget);
							if (!target) { // Find target
								int bestOtherId = 0;

								int currentAttackersNum;
								int *currentAttackers = getAttackers(actor, &currentAttackersNum);
								if (currentAttackersNum > 0) {
									bestOtherId = currentAttackers[0];
								}

								if (!bestOtherId) {
									int lowestOtherAttackers = 0;
									for (int i = 0; i < map->actorsNum; i++) {
										Actor *otherActor = &map->actors[i];
										if (otherActor->type != ACTOR_UNIT) continue;
										if (otherActor->team == actor->team) continue;
										if (otherActor->playerControlled && inRoomPrewarm) continue;

										int otherAttackersNum;
										int *otherAttackers = getAttackers(otherActor, &otherAttackersNum);

										if (!bestOtherId || otherAttackersNum < lowestOtherAttackers) {
											bestOtherId = otherActor->id;
											lowestOtherAttackers = otherAttackersNum;
										}
									}
								}

								if (bestOtherId) actor->aiTarget = bestOtherId;
							}

							if (!target) {
								if (actor->aiState == AI_STAND_NEAR_TARGET) actor->aiState = AI_IDLE;
								if (actor->aiState == AI_APPROACH_FOR_ATTACH) actor->aiState = AI_IDLE;
							}

							if (actor->prevAiState != actor->aiState) {
								actor->prevAiState = actor->aiState;
								actor->aiStateTime = 0;
							}

							Vec2 xyPosition = v2(actor->position);
							Vec2 xyTarget = v2();
							if (target) xyTarget = v2(target->position);

							if (actor->aiState == AI_IDLE) {
								if (target) actor->aiState = AI_STAND_NEAR_TARGET;
							} else if (actor->aiState == AI_STAND_NEAR_TARGET) {
								if (actor->aiStateTime == 0) {
									float dist = rndFloat(100, 200);
									actor->aiStateLength = clampMap(dist, 0, 300, 0.1, 4);

									Vec2 dir = normalize(v2(rndFloat(-1, 1), rndFloat(-1, 1)));
									actor->aiCurrentXyOffset = (dir*2 - 1) * dist;
									if (actor->position.x < target->position.x) actor->aiCurrentXyOffset -= 200;
									else actor->aiCurrentXyOffset += 200;
								}

								if (actor->aiStateTime == 0 || distance(actor->aiCurrentXyTarget, xyTarget) > 300) {
									actor->aiCurrentXyTarget = xyTarget;
									actor->aiCurrentXy = actor->aiCurrentXyTarget + actor->aiCurrentXyOffset;

									Vec2 position = actor->aiCurrentXy;
									Vec3 size = actor->size;
									{ // bringActorPoisitionWithinBounds
										if (position.x < groundAABB.min.x + size.x/2) position.x = groundAABB.min.x + size.x/2;
										if (position.x > groundAABB.max.x - size.x/2) position.x = groundAABB.max.x - size.x/2;
										if (position.y < groundAABB.min.y + size.y/2) position.y = groundAABB.min.y + size.y/2;
										if (position.y > groundAABB.max.y - size.y/2) position.y = groundAABB.max.y - size.y/2;
									}

									actor->aiCurrentXy = position;
								}

								float dist = distance(actor->aiCurrentXy, xyPosition);
								if (distance(actor->aiCurrentXy, xyPosition) > 10) {
									float speedMulti = clampMap(dist, 50, 100, 0.2, 1);

									Vec2 dir = normalize(actor->aiCurrentXy - xyPosition);
									actor->movementAccel += v3(dir * speed.x*speedMulti);
								}

								if (actor->aiStateTime > actor->aiStateLength) {
									if (rndPerc(aggression)) {
										actor->aiState = AI_APPROACH_FOR_ATTACH;
									} else {
										actor->prevAiState = AI_IDLE; // Reset ai state
									}
								}
							} else if (actor->aiState == AI_APPROACH_FOR_ATTACH) {
								Vec2 xyDest = xyTarget;

								Vec2 dir = (xyDest - xyPosition).normalize();
								actor->movementAccel += v3(dir * speed);

								if (distance(actor, target) < 100 && fabs(actor->position.y - target->position.y) < 100) {
									const int AI_CHOICE_1 = 0;
									const int AI_CHOICE_2 = 1;
									const int AI_CHOICE_3 = 2;
									const int AI_CHOICES_MAX = 3;
									float choiceBuckets[AI_CHOICES_MAX] = {};
									choiceBuckets[AI_CHOICE_1] += 1;
									choiceBuckets[AI_CHOICE_2] += 1;
									choiceBuckets[AI_CHOICE_3] += 0.5;

									int choice = rndPick(choiceBuckets, AI_CHOICES_MAX);
									if (choice == AI_CHOICE_1) {
										addAction(actor, ACTION_PUNCH);
										addAction(actor, ACTION_PUNCH);
										addAction(actor, ACTION_UPPERCUT);
									} else if (choice == AI_CHOICE_2) {
										addAction(actor, ACTION_PUNCH);
										addAction(actor, ACTION_KICK);
									} else if (choice == AI_CHOICE_3) {
										addAction(actor, ACTION_KICK);
									}
									actor->aiState = AI_STAND_NEAR_TARGET;
								}
							}
						}
					} else if (actor->aiType == AI_DUMMY) {
					}
				} ///
			}

			bool doRefill = false;
			if (actor->maxHp == 0) doRefill = true;
			actor->maxHp = getStatPoints(actor, STAT_HP) * 10;
			actor->maxStamina = getStatPoints(actor, STAT_MAX_STAMINA) * 10;
			if (doRefill) {
				actor->hp = actor->maxHp;
				actor->stamina = actor->maxStamina;
			}
		} else if (actor->type == ACTOR_GROUND) {
		} else if (actor->type == ACTOR_WALL) {
		} else if (actor->type == ACTOR_DUMMY) {
			actor->team = 1;
		} else if (actor->type == ACTOR_DOOR) {
			if (game->mapTime == 0) {
				actor->locked = 0;
			}

			if (game->leftToBeatTillUnlock > 0) {
				if (game->mapTime > ROOM_PREWARM_TIME) actor->locked += 0.003;
				if (game->mapTime == 0 && !actor->doorPlayerSpawnedOver) actor->locked = 1;
			} else {
				actor->locked -= 0.05;
			}
			actor->locked = Clamp01(actor->locked);

			if (actor->locked < 0.9 && overlaps(actor, player)) {
				if (!actor->doorPlayerSpawnedOver) {
					for (int i = 0; i < MAPS_MAX; i++) {
						Map *possibleMap = &game->maps[i];
						if (streq(possibleMap->name, actor->destMapName)) {
							game->nextMapIndex = i;
							break;
						}
					}
				}
			} else {
				actor->doorPlayerSpawnedOver = false;
			}
		} else if (actor->type == ACTOR_ITEM) {
			if (actor->isOnGround) {
				if (getEquippedItemCount(player, ITEM_MAGNET) > 0) {
					if (distance(player, actor) < 100) {
						Vec3 dir = normalize(getCenter(getAABB(player)) - getCenter(getAABB(actor)));
						actor->accel += dir * 5;
					}
				}

				if (overlaps(actor, player)) {
					if (actor->itemType == ITEM_MONEY) {
						player->money += actor->itemAmount;
						actor->markedForDeletion = true;

						Effect *effect = createEffect(EFFECT_MONEY, player->position + v3(0, 0, player->size.z*0.5));
						effect->value = actor->itemAmount;
					} else {
						if (giveItem(player, actor->itemType, actor->itemAmount)) {
							actor->markedForDeletion = true;
						} else {
							if (platform->frameCount % 60 == 0) infof("Too many items to pick up %s\n", game->itemTypeInfos[actor->itemType].name);
						}
					}
				}
			}
		} else if (actor->type == ACTOR_STORE) {
			bool overlappingStore = overlaps(actor, player);

			if (!game->inStore && overlappingStore && !game->ignoreStoreOverlap) {
				game->inStore = true;
				game->ignoreStoreOverlap = true;
			}

			if (!overlappingStore) game->ignoreStoreOverlap = false;
		}

		if (actor->isOnGround) {
			actor->timeInAir = 0;
			if (actor->movementAccel.length() > 0.1) { // This has to happen before physics update
				actor->timeMoving += elapsed;
				actor->timeNotMoving = 0;
			} else {
				actor->timeMoving = 0;
				actor->timeNotMoving += elapsed;
			}
		} else {
			actor->timeInAir += elapsed;
			actor->timeMoving = 0;
			actor->timeNotMoving = 0;
		}

		Vec3 oldPosition = actor->position;
		if (actor->info->hasPhysics && timeScale > 0.1) {
			{ // Bump other actors
				for (int i = 0; i < map->actorsNum; i++) {
					Actor *otherActor = &map->actors[i];
					if (actor->playerControlled) break;
					if (otherActor->playerControlled) continue;
					if (!otherActor->info->hasPhysics) continue;
					if (actor == otherActor) continue;

					float dist = distance(actor->position, otherActor->position);
					if (actor->size.x != actor->size.y) logf("Phyics with non uniform scale actor\n");
					dist -= actor->size.x/2;
					dist -= otherActor->size.x/2;
					if (dist <= 0) {
						Vec3 dir = otherActor->position - actor->position;
						float force = clampMap(dist, -50, 0, 0.01, 0);
						actor->accel -= dir * force;
						otherActor->accel += dir * force;
					}
				}
			}

			if (actor->movementAccel.x < -0.1) actor->facingLeft = true;
			if (actor->movementAccel.x > 0.1) actor->facingLeft = false;
			if (actor->isOnGround) actor->accel += actor->movementAccel;
			if (!isZero(actor->movementAccel)) {
				actor->movementAccel = v3();
				actor->isBlocking = false;
			}

			float grav = 1;
			for (int i = 0; i < getBuffCount(actor, BUFF_HEAVEN_STEP_GRAVITY); i++) grav *= 0.5;

			actor->accel.z -= grav;

			actor->velo += actor->accel;
			actor->accel = v3();

			{ // AABB collision
				bool bouncedOffSideWall = false;

				AABB oldAABB = getAABB(actor);
				AABB newAABB = oldAABB + actor->velo;
				actor->isOnGround = false;
				for (int i = 0; i < wallsNum; i++) {
					AABB wallAABB = walls[i];
					if (!intersects(newAABB, wallAABB)) continue;

					if (newAABB.min.z <= wallAABB.max.z && oldAABB.min.z > wallAABB.max.z) { // Bot
						float dist = wallAABB.max.z - newAABB.min.z;
						newAABB += v3(0, 0, dist+0.1);
						actor->velo.z = 0;
						actor->isOnGround = true;
					}

					// if (newAABB.min.z >= wallAABB.min.z && oldAABB.max.z < wallAABB.max.z) { // Top
					// 	float dist = wallAABB.min.z - newAABB.max.z;
					// 	newAABB += v3(0, 0, dist-0.1);
					// 	actor->velo.z = 0;
					// }

					float restitution = 0.5;

					if (newAABB.min.x <= wallAABB.max.x && oldAABB.min.x > wallAABB.max.x) { // left
						float dist = wallAABB.max.x - newAABB.min.x;
						newAABB += v3(dist+0.1, 0, 0);
						if (actor->velo.x < 0) {
							bouncedOffSideWall = true;
							actor->velo.x *= -restitution;
						}
					}

					if (newAABB.max.x >= wallAABB.min.x && oldAABB.max.x < wallAABB.min.x) { // right
						float dist = wallAABB.min.x - newAABB.max.x;
						newAABB += v3(dist-0.1, 0, 0);
						if (actor->velo.x > 0) {
							bouncedOffSideWall = true;
							actor->velo.x *= -restitution;
						}
					}

					if (newAABB.min.y <= wallAABB.max.y && oldAABB.min.y > wallAABB.max.y) {
						float dist = wallAABB.max.y - newAABB.min.y;
						newAABB += v3(0, dist+0.1, 0);
						if (actor->velo.y < 0) {
							bouncedOffSideWall = true;
							actor->velo.y *= -restitution;
						}
					}

					if (newAABB.max.y >= wallAABB.min.y && oldAABB.max.y < wallAABB.min.y) {
						float dist = wallAABB.min.y - newAABB.max.y;
						newAABB += v3(0, dist-0.1, 0);
						if (actor->velo.y > 0) {
							bouncedOffSideWall = true;
							actor->velo.y *= -restitution;
						}
					}
				}

				Vec3 newPos = getCenter(newAABB);
				newPos.z -= getSize(newAABB).z/2;
				actor->position = newPos;

				if (bouncedOffSideWall) {
					if (actor->type == ACTOR_UNIT && actor->actionsNum > 0 && actor->actions[0].type == ACTION_HITSTUN) {
						playWorldSound("assets/audio/wallThump.ogg", getCenter(newAABB));
					}
				}
			}

			if (actor->isOnGround) {
				actor->velo.x *= 0.8;
				actor->velo.y *= 0.8;
			}
			actor->velo.z *= 0.98;
		}

		if (actor->type == ACTOR_UNIT) { //We have to flip around ai actors here because physics flips them if they're moving slightly backwards
			Actor *target = getActor(actor->aiTarget);
			if (target) {
				Vec3 dir = target->position - actor->position;
				if (dir.x <= 0) {
					actor->facingLeft = true;
				} else {
					actor->facingLeft = false;
				}
			}
		}

		if (actor->type == ACTOR_UNIT) {
			float distanceCovered = distance(oldPosition, actor->position);
			Vec3 dir = normalize(actor->position - oldPosition);
			float movementPerc = distanceCovered;
			if (actor->isRunningLeft || actor->isRunningRight) movementPerc *= globals->movementPercDistanceRunningRatio;
			else movementPerc *= globals->movementPercDistanceWalkingRatio;
			if ((dir.x > 0 && actor->facingLeft) || (dir.x < 0 && !actor->facingLeft)) movementPerc *= -1;
			actor->movementPerc += movementPerc;
			
			while (actor->movementPerc < 0) actor->movementPerc++;
			while (actor->movementPerc > 1) actor->movementPerc--;

			if (actor->timeMoving == 0) actor->movementPerc = 0;
		}

		if (actor->hp < 20 && getEquippedItemCount(actor, ITEM_HEALTH_PACK) > 0) {
			infof("Health pack used\n");
			actor->hp += 100;
			removeItem(actor, ITEM_HEALTH_PACK, 1);
		}
		if (game->debugNeverTakeDamage && actor == player) player->hp = 100;
		if (actor->hp <= 0 && actor->maxHp > 0) {
			actor->markedForDeletion = true;
			if (actor->type == ACTOR_UNIT) {
				if (actor->playerControlled) {
					game->nextMapIndex = CITY_START_INDEX;
					logf("You died (-$%.2f)\n", player->money*0.5);
					player->money *= 0.5;
				} else {
					map->alliances[actor->team] -= actor->allianceCost;
					if (actor->team != player->team) game->leftToBeatTillUnlock--;
				}
			}
		}
		actor->hp = mathClamp(actor->hp, 0, actor->maxHp);
		actor->stamina += getStatPoints(actor, STAT_STAMINA_REGEN) * 0.03;
		actor->stamina = mathClamp(actor->stamina, -100, actor->maxStamina);

		actor->aiStateTime += elapsed;
		if (actor->position.z < -10000) actor->position = v3(0, 0, 500);
	} ///

	{ /// Update particles
		for (int i = 0; i < game->particlesNum; i++) {
			Particle *particle = &game->particles[i];
			bool complete = false;

			particle->time += elapsed;
			if (particle->time > particle->maxTime) complete = true;

			// particle->velo.z -= 0.98; // grav

			particle->position += particle->velo;
			particle->velo *= 0.95;

			if (complete) {
				game->particles[i] = game->particles[game->particlesNum-1];
				game->particlesNum--;
				i--;
				continue;
			}
		}
	}

	pushTargetTexture(game->debugTexture);
	{ /// Draw actors (debug/overlay)
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];

			AABB aabb = getAABB(actor);
			Rect bounds = getBounds(aabb);

			Vec2 position2 = v2(game->isoMatrix3 * actor->position);
			Vec2 positionCenter2 = v2(game->isoMatrix3 * getCenter(aabb));
			Vec2 positionTop2 = v2(game->isoMatrix3 * (actor->position + v3(0, 0, actor->size.z)));

			// drawRectOutline(bounds, 2, 0xFFFF0000);

			bool hasHpBar = false;
			bool hasStaminaBar = false;
			if (actor->type == ACTOR_UNIT) hasHpBar = hasStaminaBar = true;
			Rect hpBgRect;
			if (hasHpBar) {
				hpBgRect = makeRect(0, 0, actor->size.x, 10);
				hpBgRect.x = positionTop2.x - hpBgRect.width/2;
				hpBgRect.y = positionTop2.y - hpBgRect.height/2;
				drawRect(hpBgRect, 0xFFFF0000);

				Rect rect = hpBgRect;
				rect.width *= actor->hp/actor->maxHp;

				int color = 0xFF44AA44;
				if (actor->playerControlled) color = 0xFFF5EE2A;
				drawRect(rect, color);
			}

			if (hasStaminaBar) {
				Rect staminaBgRect = hpBgRect;
				staminaBgRect.y += hpBgRect.height;
				drawRect(staminaBgRect, 0xFF000070);

				Rect rect = staminaBgRect;
				rect.width *= actor->stamina/actor->maxStamina;
				drawRect(rect, actor->stamina < 0 ? 0xFFF00000 : 0xFF0000F0);
			}

			if (game->debugDrawActorTargets) {
				Actor *target = getActor(actor->aiTarget);
				if (target) {
					AABB targetAABB = getAABB(target);
					Vec2 targetPositionCenter2 = v2(game->isoMatrix3 * getCenter(targetAABB));
					drawLine(positionCenter2, targetPositionCenter2, 4, 0xFFFF0000);
				}
			}

			if (game->inEditor) {
				if (contains(bounds, game->worldMouse) || game->debugAlwaysShowWireframes) drawAABB2d(aabb, 2, 0xFFFFFFFF);

				if (game->selectedActorId == actor->id) {
					drawAABB2d(aabb, 2, lerpColor(0xFFFFFF80, 0xFFFFFF00, secondPhase));

					if (game->inEditor) {
						Line3 lineX = makeLine3(actor->position, actor->position + v3(64, 0, 0));
						Line3 lineY = makeLine3(actor->position, actor->position + v3(0, 64, 0));
						Line3 lineZ = makeLine3(actor->position, actor->position + v3(0, 0, 64));
						drawLine(v2(game->isoMatrix3 * lineX.start), v2(game->isoMatrix3 * lineX.end), 4, 0xFFFF0000);
						drawLine(v2(game->isoMatrix3 * lineY.start), v2(game->isoMatrix3 * lineY.end), 4, 0xFF00FF00);
						drawLine(v2(game->isoMatrix3 * lineZ.start), v2(game->isoMatrix3 * lineZ.end), 4, 0xFF0000FF);
					}
				}
			}

			{ // Status lines
				if (actor->type == ACTOR_UNIT || actor->type == ACTOR_DUMMY) {
					const int TEXT_LINES_MAX = 8;
					char *textLines[TEXT_LINES_MAX];
					int textLinesNum = 0;

					if (game->debugDrawActorStatus) {
						if (actor->timeInAir > 0) textLines[textLinesNum++] = frameSprintf("In air (%.1f)", actor->timeInAir);
						if (actor->timeMoving > 0) textLines[textLinesNum++] = frameSprintf("Moving (%.1f)", actor->timeMoving);
						if (actor->timeNotMoving > 0) textLines[textLinesNum++] = frameSprintf("Not moving (%.1f)", actor->timeNotMoving);
					}

					if (game->debugDrawActorAction) {
						if (actor->actionsNum > 0) {
							Action *action = &actor->actions[0];
							textLines[textLinesNum++] = frameSprintf("%s (%.1fs)", action->info->name, action->time);
						}
					}

					if (game->debugDrawActorStats) {
						textLines[textLinesNum++] = frameSprintf(
							"atk:%.0f hp:%.0f sGen:%.0f\nmaxS:%.0f move:%.0f aSpeed:%.0f",
							getStatPoints(actor, STAT_DAMAGE),
							getStatPoints(actor, STAT_HP),
							getStatPoints(actor, STAT_STAMINA_REGEN),
							getStatPoints(actor, STAT_MAX_STAMINA),
							getStatPoints(actor, STAT_MOVEMENT_SPEED),
							getStatPoints(actor, STAT_ATTACK_SPEED)
						);
					}

					if (game->debugDrawActorStatsSimple) {
						textLines[textLinesNum++] = "SIMPLE_STATS";
						// textLines[textLinesNum++] = frameSprintf(
						// 	"%.0f %.0f %.0f %.0f %.0f %.0f",
						// 	getStatPoints(actor, STAT_DAMAGE),
						// 	getStatPoints(actor, STAT_HP),
						// 	getStatPoints(actor, STAT_STAMINA_REGEN),
						// 	getStatPoints(actor, STAT_MAX_STAMINA),
						// 	getStatPoints(actor, STAT_MOVEMENT_SPEED),
						// 	getStatPoints(actor, STAT_ATTACK_SPEED)
						// );
					}

					if (game->debugDrawActorFacingDirection) {
						textLines[textLinesNum++] = frameSprintf("%s", actor->facingLeft ? "Facing left" : "Facing right");
					}

					Vec2 cursor = positionTop2;
					for (int i = 0; i < textLinesNum; i++) {
						if (streq(textLines[i], "SIMPLE_STATS")) {
							char *statTextStrs[STATS_MAX];
							Vec2 statTextSizes[STATS_MAX];
							Vec2 size = v2();
							for (int i = 0; i < STATS_MAX; i++) {
								statTextStrs[i] = frameSprintf("%.0f", getStatPoints(actor, (StatType)i));
								statTextSizes[i] = getTextSize(game->simpleStatsFont, statTextStrs[i]);
								size.x += statTextSizes[i].x;
								size.y = MaxNum(size.y, statTextSizes[i].y);
							}
							Vec2 position;
							position.x = cursor.x - size.x/2;
							position.y = cursor.y - size.y;

							for (int i = 0; i < STATS_MAX; i++) {
								drawText(game->simpleStatsFont, statTextStrs[i], position, statTypeColors[i]);
								position.x += statTextSizes[i].x;
							}

							cursor.y -= size.y;
						} else {
							Vec2 size = getTextSize(game->defaultFont, textLines[i]);
							Vec2 position;
							position.x = cursor.x - size.x/2;
							position.y = cursor.y - size.y;

							Rect rect = inflate(makeRect(position, size), 5);
							drawRect(rect, 0x80000000);
							drawText(game->defaultFont, textLines[i], position, 0xFFC0C0C0);

							cursor.y -= size.y;
						}
					}
				}
			}

			{ // Status icons
				if (actor->type == ACTOR_UNIT || actor->type == ACTOR_DUMMY) {
					struct Icon {
						Texture *texture;
						bool flipped;
					};

					const int ICONS_MAX = 128;
					Icon *icons = (Icon *)zalloc(sizeof(Icon) * ICONS_MAX);
					int iconsNum = 0;

					for (int i = 0; i < actor->buffsNum; i++) {
						Buff *buff = &actor->buffs[i];

						if (buff->type == BUFF_HEAVEN_STEP_GRAVITY) {
							Icon *icon = &icons[iconsNum++];
							icon->texture = getTexture("assets/images/statusIcons/heavenStep.png");
						} else if (buff->type == BUFF_BUDDHA_PALM_SLOW) {
							Icon *icon = &icons[iconsNum++];
							icon->texture = getTexture("assets/images/statusIcons/buddhaPalm.png");
						} else {
							Icon *icon = &icons[iconsNum++];
							icon->texture = renderer->whiteTexture;
						}

					}

					Vec2 cursor = position2 - v2(0, 50);
					for (int i = 0; i < iconsNum; i++) {
						Icon *icon = &icons[i];
						Texture *texture = icon->texture;

						Rect rect = makeCenteredSquare(cursor, 64);
						Matrix3 mat = mat3();
						mat.TRANSLATE(rect.x, rect.y);
						mat.SCALE(rect.width, rect.height);
						Vec2 uv0 = v2(0, 0);
						Vec2 uv1 = v2(1, 1);

						if (icon->flipped) {
							uv0 = v2(1, 0);
							uv1 = v2(0, 1);
						}

						drawRect(rect, 0x40000000);
						drawSimpleTexture(icon->texture, mat, uv0, uv1);

						cursor.y -= rect.height + 8;
					}
				}
			}

		}
	} ///

	{ /// Update effects
		for (int i = 0; i < game->effectsNum; i++) {
			Effect *effect = &game->effects[i];

			bool complete = false;
			float maxTime = 1;
			Vec2 position2 = v2(game->isoMatrix3 * effect->position);

			char *effectText = NULL;
			int effectTextColor = 0;
			if (effect->type == EFFECT_ENEMY_DAMAGE) {
				effectText = frameSprintf("-%.0f", effect->value);
				effectTextColor = 0xFFFF8000;
			} else if (effect->type == EFFECT_PLAYER_DAMAGE) {
				effectText = frameSprintf("-%.0f", effect->value);
				effectTextColor = 0xFFFF0000;
			} else if (effect->type == EFFECT_BLOCK_DAMAGE) {
				effectText = frameSprintf("-%.0f", effect->value);
				effectTextColor = 0xFF0000FF;
			} else if (effect->type == EFFECT_MONEY) {
				effectText = frameSprintf("+$%.2f", effect->value);
				effectTextColor = 0xFF00FF00;
			}

			if (effectText) {
				Vec2 size = getTextSize(game->particleFont, effectText);
				Vec2 pos = position2 - size/2;

				float alpha =
					clampMap(effect->time, 0, maxTime*0.05, 0, 1)
					* clampMap(effect->time, maxTime*0.95, maxTime, 1, 0);

				pos.y += clampMap(effect->time, 0, maxTime*0.5, -20, 0, BOUNCE_OUT);

				Rect rect = makeRect(pos, size);

				pushAlpha(alpha);
				drawRect(inflate(rect, 2), 0x30000000);
				drawText(game->particleFont, effectText, pos, effectTextColor);
				popAlpha();
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

	{ /// Update map
		game->timeTillNextCityTick -= elapsed;
		float prevCityTime = game->cityTime;
		game->cityTime += elapsed;
		if (game->timeTillNextCityTick <= 0) {
			// logf("Tick %d %f\n", game->cityTicks, game->cityTime); //@todo Maybe make this so time controls ticks
			game->cityTicks++;
			game->timeTillNextCityTick = SECS_PER_CITY_TICK;
		}

		float spreadAmount = 0.0005;
		while (game->cityTicks > 0) {
			game->cityTicks--;

			for (int y = 0; y < CITY_ROWS; y++) {
				for (int x = 0; x < CITY_COLS; x++) {
					Map *map = getCityMapByCoords(x, y);

					float totalAlliance = 0;
					int teamsPresent = 0;

					float highestTeamAlliance = 0;
					for (int i = 0; i < TEAMS_MAX; i++) {
						totalAlliance += map->alliances[i];
						if (map->alliances[i] > 0) teamsPresent++;
						if (highestTeamAlliance < map->alliances[i]) highestTeamAlliance = map->alliances[i];
					}

					for (int i = 0; i < TEAMS_MAX; i++) {
						float normalizedAlliance = map->alliances[i] / totalAlliance;
						if (totalAlliance == 0) normalizedAlliance = 0;

						// map->alliances[i] += clampMap(normalizedAlliance, 0, 1, 0, 0.0001); // Self growth

						float currentSpreadAmount = 0;
						currentSpreadAmount += clampMap(normalizedAlliance, 0, 1, 0, spreadAmount);

						int strongestTeam = getTeamWithMostAlliance(map);
						int surroundingAtMaxCount = 0;
						if (i == strongestTeam && map->alliances[i] > 0.9) {
							surroundingAtMaxCount = getSurroundingAtMaxAlliance(v2i(x, y), i);
							if (map->fortifiedPerc < 0.1 && surroundingAtMaxCount >= 8) {
								map->fortifiedByTeam = i;
								map->fortifiedPerc += 0.001;
							}
						}

						if (map->fortifiedByTeam == i) {
							if (map->fortifiedPerc > 0) {
								float fortLerp = 0.01;
								// if (map->fortifiedPerc < map->alliances[i]) fortLerp = 0.01;

								map->fortifiedPerc = lerp(map->fortifiedPerc, normalizedAlliance, fortLerp);
								map->alliances[i] += clampMap(map->fortifiedPerc, 0, 1, 0, 0.001);
							}
							map->fortifiedPerc = Clamp01(map->fortifiedPerc);
							currentSpreadAmount *= clampMap(map->fortifiedPerc, 0, 1, 1, 1.5);
						}

						{ // Spread alliance
							Map *possibleMaps[8] = {};
							int possibleMapsNum = 0;
							Map *adjMap;

							// adjMap = getCityMapByCoords(x-1, y-1);
							// if (adjMap) possibleMaps[possibleMapsNum++] = adjMap;

							// adjMap = getCityMapByCoords(x+1, y+1);
							// if (adjMap) possibleMaps[possibleMapsNum++] = adjMap;

							// adjMap = getCityMapByCoords(x-1, y+1);
							// if (adjMap) possibleMaps[possibleMapsNum++] = adjMap;

							// adjMap = getCityMapByCoords(x+1, y-1);
							// if (adjMap) possibleMaps[possibleMapsNum++] = adjMap;

							adjMap = getCityMapByCoords(x, y-1);
							if (adjMap) possibleMaps[possibleMapsNum++] = adjMap;

							adjMap = getCityMapByCoords(x, y+1);
							if (adjMap) possibleMaps[possibleMapsNum++] = adjMap;

							adjMap = getCityMapByCoords(x-1, y);
							if (adjMap) possibleMaps[possibleMapsNum++] = adjMap;

							adjMap = getCityMapByCoords(x+1, y);
							if (adjMap) possibleMaps[possibleMapsNum++] = adjMap;

							int team = i;
							for (int i = 0; i < possibleMapsNum; i++) {
								Map *map = possibleMaps[i];
								if (map->alliances[i] >= 1) {
									arraySpliceIndex(possibleMaps, possibleMapsNum, sizeof(Map *), i);
									possibleMapsNum--;
									i--;
									continue;
								}
							}

							if (possibleMapsNum > 0) {
								int choice = rndInt(0, possibleMapsNum-1);
								if (x == 6 && y == 1) {
									int k=5;
									// logf("[%d, %d] %d\n", 0, possibleMapsNum-1, choice);
								}

								possibleMaps[choice]->alliances[i] += currentSpreadAmount * 8;
							}
						}
					}

					for (int i = 0; i < TEAMS_MAX; i++) {
						float lossToGive = highestTeamAlliance - 1;
						if (teamsPresent > 1 && lossToGive > 0) {
							map->alliances[i] -= lossToGive*2;
						}
						map->alliances[i] = Clamp01(map->alliances[i]); // Probably not needed
					}

				}
			}
		}

		{ /// Update maps (stores)
			float prevMod = fmod(prevCityTime, 120);
			float currentMod = fmod(game->cityTime, 120);
			if (prevMod > currentMod || prevCityTime == 0 || game->debugForceRestock) {
				logf("Stores have restocked\n");
				game->debugForceRestock = false;

				Item *possibleItems = (Item *)frameMalloc(sizeof(Item) * ITEM_TYPES_MAX);
				int possibleItemsNum = 0;
				for (int i = 0; i < ITEM_TYPES_MAX; i++) {
					ItemTypeInfo *info = &game->itemTypeInfos[i];

					if (getStashedItemCount(player, (ItemType)i) >= info->maxAmountFromStore) continue;
					//@todo prereqs

					initItem(&possibleItems[possibleItemsNum], (ItemType)i, 1);
					possibleItemsNum++;
				}

				game->storeItemsNum = 0;
				for (int i = 0; i < 3; i++) {
					if (possibleItemsNum <= 0) break;

					int itemIndex = rndInt(0, possibleItemsNum-1);
					game->storeItems[game->storeItemsNum] = possibleItems[itemIndex];
					game->storeItemsNum++;
					arraySpliceIndex(possibleItems, possibleItemsNum, sizeof(Item), itemIndex);
					possibleItemsNum--;
				}

				// for (int i = 0; i < MAPS_MAX; i++) {
				// 	Map *map = &game->maps[i];
				// 	for (int i = 0; i < map->actorsNum; i++) {
				// 		Actor *actor = &map->actors[i];
				// 		if (actor->type == ACTOR_STORE) {
				// 			actor->itemsNum = 0;
				// 		}
				// 	}
				// }
			}
		} ///

		{ /// Spawn enemies // Spawn npcs
			auto createNpcUnit = [](Map *map)->Actor *{
				Actor *actor = createActor(map, ACTOR_UNIT);
				actor->stats[STAT_DAMAGE] = 5;
				actor->stats[STAT_HP] = 5;
				actor->stats[STAT_MAX_STAMINA] = 5;
				actor->stats[STAT_STAMINA_REGEN] = 5;
				actor->stats[STAT_MOVEMENT_SPEED] = 5;
				actor->stats[STAT_ATTACK_SPEED] = 5;

				actor->team = rndPick(map->alliances, TEAMS_MAX);
				Vec2 allianceMinMax;
				allianceMinMax.x = 0.05;
				allianceMinMax.y = clampMap(map->alliances[actor->team], 0, 1, 0.05, 0.2);
				actor->allianceCost = rndFloat(allianceMinMax.x, allianceMinMax.y);

				int extraPoints = clampMap(actor->allianceCost, allianceMinMax.x, allianceMinMax.y, 10, 50);
				for (int i = 0; i < extraPoints; i++) {
					actor->stats[rndInt(0, STATS_MAX-1)]++;
					actor->level++;
				}

				return actor;
			};

			int currentNpcs = 0;
			for (int i = 0; i < map->actorsNum; i++) {
				Actor *actor = &map->actors[i];
				if (actor->type == ACTOR_UNIT && !actor->playerControlled) currentNpcs++;
			}

			float totalAlliance = 0;
			for (int i = 0; i < TEAMS_MAX; i++) totalAlliance += map->alliances[i];
			int maxNpcs = clampMap(totalAlliance, 0, 2, 2, 20);
			if (totalAlliance <= 0.01) maxNpcs = 0;

			// int startingNpcs = maxNpcs * 0.8;
			int startingNpcs = 0; // This is disabled because I think prewarming is better
			if (game->mapTime == 0 && startingNpcs > 0) {
				auto generatePoissonPoints = [](AABB surface, Vec2 cellSize, int *outPointsNum)->Vec3 *{
					Vec3 surfaceSize = getSize(surface);

					int cellsWide = surfaceSize.x / cellSize.x;
					int cellsHigh = surfaceSize.y / cellSize.y;

					Vec3 *points = (Vec3 *)frameMalloc(sizeof(Vec3) * (cellsWide * cellsHigh));
					int pointsNum = 0;
					for (int y = 0; y < cellsHigh; y++) {
						for (int x = 0; x < cellsWide; x++) {
							Rect cell = makeRect(v2(), cellSize);
							cell.x = surface.min.x + (x * cellSize.x);
							cell.y = surface.min.y + (y * cellSize.y);
							Vec2 pos;
							pos.x = rndFloat(cell.x, cell.x+cell.width);
							pos.y = rndFloat(cell.y, cell.y+cell.height);

							points[pointsNum++] = v3(pos, surface.max.z + 1);
						}
					}

					*outPointsNum = pointsNum;
					return points;
				};

				auto removeInvalidSpawnPoints = [](Map *map, Vec3 *points, int pointsNum)->int {
					for (int i = 0; i < pointsNum; i++) {
						Vec3 point = points[i];
						AABB aabb = getAABBFromSizePosition(UNIT_SIZE, point);
						bool shouldRemove = false;
						Actor *ground = getActorOfType(map, ACTOR_GROUND);
						if (!equal(aabb, bringWithinBounds(getAABB(ground), aabb))) shouldRemove = true;
						for (int i = 0; i < map->actorsNum; i++) {
							Actor *actor = &map->actors[i];
							if (overlaps(actor, aabb)) {
								shouldRemove = true;
								break;
							}
						}

						if (shouldRemove) {
							arraySpliceIndex(points, pointsNum, sizeof(Vec3), i);
							pointsNum--;
							i--;
							continue;
						}
					}

					return pointsNum;
				};

				Vec2 cellSize = v2(getSize(groundAABB)) / 2;

				Vec3 *points;
				int pointsNum;
				for (int i = 0; ; i++) {
					if (i > 100) {
						logf("Failed 100 times to spawn units...\n");
						pointsNum = 0;
						break;
					}

					points = generatePoissonPoints(groundAABB, cellSize, &pointsNum);
					pointsNum = removeInvalidSpawnPoints(map, points, pointsNum);

					if (pointsNum >= startingNpcs) {
						break;
					} else {
						cellSize.x *= 0.01;
						cellSize.y *= 0.01;
					}
				}

				if (pointsNum > 0) {
					for (int i = 0; i < startingNpcs; i++) {
						int pointIndex = rndInt(0, pointsNum-1);
						Vec3 point = points[pointIndex];
						arraySpliceIndex(points, pointsNum, sizeof(Vec3), pointIndex);
						pointsNum--;

						Actor *newActor = createNpcUnit(map);
						newActor->position = point;
					}
				}
			}

			game->timeTillNextSpawn -= elapsed;
			float fullPerc = currentNpcs / (float)maxNpcs;
			if (game->timeTillNextSpawn < 0 && fullPerc < 1) {
				game->timeTillNextSpawn = clampMap(fullPerc, 0, 1, 0, 10);

				Actor *randomDoor = getRandomActorOfType(map, ACTOR_DOOR);
				if (randomDoor) {
					Actor *newActor = createNpcUnit(map);

					int team = newActor->team;
					float bestDoorAlliance = 0;
					Actor *bestDoor = NULL;
					for (int i = 0; i < map->actorsNum; i++) {
						Actor *actor = &map->actors[i];
						if (actor->type != ACTOR_DOOR) continue;
						Map *destMap = getMapByName(actor->destMapName);
						if (!bestDoor || destMap->alliances[team] > bestDoorAlliance) {
							bestDoor = actor;
							bestDoorAlliance = destMap->alliances[team];
						}
					}

					AABB doorAABB = getAABB(bestDoor);
					newActor->position.x = rndFloat(doorAABB.min.x, doorAABB.max.x);
					newActor->position.y = rndFloat(doorAABB.min.y, doorAABB.max.y);
					newActor->position.z = doorAABB.min.z;
					if (newActor->position.z < getAABB(ground).max.z) newActor->position.z = getAABB(ground).max.z + 1;

					bringWithinBounds(map, newActor);
				}
			}
		} ///
	} ///

	{ /// Draw hud
		pushCamera2d(renderer->currentCameraMatrix.invert());

		{ /// Draw map
			pushTargetTexture(game->mapTexture);
			clearRenderer();
			if (keyJustPressed('M')) game->lookingAtMap = !game->lookingAtMap;
			if (game->lookingAtMap) {
				Vec2 mapTileSize = v2(100, 100);
				Vec2 totalSize = v2(CITY_COLS, CITY_ROWS) * mapTileSize;

				Vec2 startCursor = game->size/2 - totalSize/2;
				Vec2 cursor = startCursor;
				for (int y = 0; y < CITY_ROWS; y++) {
					for (int x = 0; x < CITY_COLS; x++) {
						Map *map = getCityMapByCoords(x, y);

						Rect rect = makeRect(cursor, mapTileSize);
						drawRect(rect, 0xA0000000);

						float totalAlliance = 0;
						for (int i = 0; i < TEAMS_MAX; i++) totalAlliance += map->alliances[i];
						if (totalAlliance < 1) totalAlliance = 1;

						if (game->mapVisualization == MAP_VISUALIZATION_SLICES) {
							Vec2 sliceCursor = cursor;
							for (int i = 0; i < TEAMS_MAX; i++) {
								float slicePerc = map->alliances[i] / totalAlliance;
								Rect sliceRect = makeRect(sliceCursor, mapTileSize*v2(1, slicePerc));
								drawRect(sliceRect, teamColors[i]);
								sliceCursor.y += sliceRect.height;
							}
						} else if (game->mapVisualization == MAP_VISUALIZATION_BARS) {
							for (int i = 0; i < TEAMS_MAX; i++) {
								Rect bar = makeRect(cursor, mapTileSize);
								bar.width *= 1.0/TEAMS_MAX;
								bar.x += bar.width * i;

								float oldHeight = bar.height;
								bar.height *= Clamp01(map->alliances[i]);
								bar.y += oldHeight - bar.height;
								drawRect(bar, teamColors[i]);
							}
						} else if (game->mapVisualization == MAP_VISUALIZATION_LOCKED_IN) {
							int lockAmount = 0;
							{ //@copyPastedGetLockAmount
								float totalOtherAlliance = 0;
								float largestOtherAlliance = 0;
								for (int i = 0; i < TEAMS_MAX; i++) {
									if (i != player->team) {
										totalOtherAlliance += map->alliances[i];
										if (largestOtherAlliance < map->alliances[i]) largestOtherAlliance = map->alliances[i];
									}
								}
								if (totalOtherAlliance > 0.3 && map->alliances[player->team] < 0.8) {
									lockAmount = clampMap(largestOtherAlliance, 0.3, 1, 1, 15);
								}
							}

							Rect rect = makeRect(cursor, mapTileSize);
							if (lockAmount) {
								drawRect(rect, 0xFF800000);

								DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
								drawTextInRect(frameSprintf("%d", lockAmount), props, inflatePerc(rect, -0.1));
							} else {
								drawRect(rect, 0xFF008000);
							}
						} else if (game->mapVisualization == MAP_VISUALIZATION_MAX_ALLIANCE_SURROUNDING) {
							Rect rect = makeRect(cursor, mapTileSize);
							DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);

							int team = getTeamWithMostAlliance(map);
							if (map->alliances[team] > 0.9) {
								int count = getSurroundingAtMaxAlliance(v2i(x, y), team);
								drawRect(rect, lerpColor(0xFF00FF00, 0xFFFF0000, (float)count/(TEAMS_MAX-1)));
								drawTextInRect(frameSprintf("%d", count), props, inflatePerc(rect, -0.1));
							}
						}

						if (map == &game->maps[game->currentMapIndex]) {
							drawRect(rect, lerpColor(0x60FFFF00, 0xC0FFFF00, timePhase(game->time)));
						}

						if (game->nextMapIndex != game->currentMapIndex && map == &game->maps[game->nextMapIndex]) {
							drawRect(rect, lerpColor(0x60FFFFFF, 0xC0FFFFFF, timePhase(game->time * 5)));
						}

						drawRectOutline(rect, 5, 0xFF000000);

						if (map->fortifiedPerc > 0.1) {
							DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFF444444);
							drawTextInRect(frameSprintf("%.0f%% fortied", map->fortifiedPerc*100), props, inflatePerc(rect, -0.1));
						}

						if (game->inEditor) {
							if (contains(rect, game->mouse)) {
								drawRectOutline(rect, 4, 0xFFFF0000);
								if (platform->mouseJustDown) game->editorSelectedCityMap = map;
							}
						}

						cursor.x += rect.width;
						if (x == CITY_COLS-1) {
							cursor.x = startCursor.x;
							cursor.y += rect.height;
						}
					}
				}

				if (lastStepOfFrame && game->inEditor) {
					Map *map = game->editorSelectedCityMap;
					if (map) {
						ImGui::Begin("Map data", NULL, ImGuiWindowFlags_AlwaysAutoResize);
						ImGui::Combo("Visual", (int *)&game->mapVisualization, mapVisualizationStrings, ArrayLength(mapVisualizationStrings));
						ImGui::Separator();

						if (ImGui::Button("Advance 60 seconds")) {
							game->cityTicks += 60.0/SECS_PER_CITY_TICK;
							game->cityTime += 60;
						}
						if (ImGui::Button("Advance 5 min")) {
							game->cityTicks += (60.0*5)/SECS_PER_CITY_TICK;
							game->cityTime += 60*5;
						}
						ImGui::Separator();

						Vec2i coords = getCoordsByCityMap(map);
						ImGui::Text("%d, %d", coords.x, coords.y);
						if (ImGui::Button("Go")) game->nextMapIndex = getIndexByMap(map);
						if (ImGui::TreeNode("Base alliances")) {
							if (ImGui::Button("Copy from current")) memcpy(map->baseAlliances, map->alliances, sizeof(float) * TEAMS_MAX);
							for (int i = 0; i < TEAMS_MAX; i++) {
								guiPushStyleColor(ImGuiCol_FrameBg, lerpColor(teamColors[i], 0xFF000000, 0.5));
								ImGui::SliderFloat(frameSprintf("Alliance %d", i), &map->baseAlliances[i], 0, 1);
								guiPopStyleColor();
							}
							ImGui::TreePop();
						}

						if (ImGui::TreeNode("Current alliances")) {
							for (int i = 0; i < TEAMS_MAX; i++) {
								ImGui::PushID(i);
								guiPushStyleColor(ImGuiCol_FrameBg, lerpColor(teamColors[i], 0xFF000000, 0.5));
								ImGui::SliderFloat(frameSprintf("Alliance %d", i), &map->alliances[i], 0, 1);
								ImGui::SameLine();
								if (ImGui::Button("+10%")) {
									map->alliances[i] += 0.1;
								}
								guiPopStyleColor();
								ImGui::PopID();
							}
							ImGui::TreePop();
						}
						ImGui::End();
					}
				}
			}

			popTargetTexture(); // game->mapTexture
		} ///

		{ /// Update inventory
			if (keyJustPressed('N')) game->inInventory = !game->inInventory;
			if (game->inInventory) {
				auto drawItemIcon = [](Item *item, Rect rect) {
					drawRect(rect, 0xFF444444);

					DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFCCCCCC);
					drawTextInRect(game->itemTypeInfos[item->type].name, props, rect);
				};

				Rect bg;
				bg.width = game->size.x * 0.8;
				bg.height = game->size.y * 0.8;
				bg.x = game->size.x/2 - bg.width/2;
				bg.y = game->size.y/2 - bg.height/2;
				drawRect(bg, 0xDD555555);

				{
					Rect itemContainer = bg;
					itemContainer.width *= 0.5;
					itemContainer = inflatePerc(itemContainer, -0.1);
					drawRect(itemContainer, 0xFF222222);

					int cols = 6;
					Vec2 cellSize = v2(itemContainer.width/8, itemContainer.width/8);
					Vec2 cursor = getPosition(itemContainer);
					for (int i = 0; i < player->itemsNum; i++) {
						Item *item = &player->items[i];

						bool inUse = false;
						for (int i = 0; i < STYLES_MAX; i++) {
							Style *style = &player->styles[i];
							for (int i = 0; i < 3; i++) {
								int *slotPtr = NULL;
								if (i == 0) slotPtr = &style->passiveItem;
								if (i == 1) slotPtr = &style->activeItem0;
								if (i == 2) slotPtr = &style->activeItem1;

								if (item->id == *slotPtr) {
									inUse = true;
									break;
								}
							}
						}

						if (game->draggingItemId == item->id || inUse) pushAlpha(0.5);

						Rect iconRect = makeRect(cursor, cellSize);
						drawItemIcon(item, inflatePerc(iconRect, -0.1));

						if (game->draggingItemId == item->id || inUse) popAlpha();

						if (platform->mouseJustDown && contains(iconRect, game->mouse) && !game->draggingItemId && !inUse) {
							game->draggingItemId = item->id;
						}

						cursor.x += iconRect.width;
						if (cursor.x + cellSize.x > itemContainer.x + itemContainer.width) {
							cursor.x = itemContainer.x;
							cursor.y += cellSize.y;
						}
					}
				}

				bool itemDropped = game->draggingItemId && !platform->mouseDown;
				Rect draggingRect = makeCenteredSquare(game->mouse, game->size.y * 0.05);

				{
					Rect stylesContainer = bg;
					stylesContainer.width *= 0.5;
					stylesContainer.x += stylesContainer.width;
					stylesContainer = inflatePerc(stylesContainer, -0.1);
					drawRect(stylesContainer, 0xFF444444);

					int stylesMax = 4;
					for (int i = 0; i < stylesMax; i++) {
						Style *style = &player->styles[i];

						Rect styleContainer = stylesContainer;
						styleContainer.height /= stylesMax;
						styleContainer.y += styleContainer.height * i;
						styleContainer = inflatePerc(styleContainer, -0.1);
						drawRect(styleContainer, 0xFF333333);

						int slotsMax = 3;
						for (int i = 0; i < slotsMax; i++) {
							bool isActiveSlot = false;
							int *slotPtr = NULL;
							if (i == 0) {
								slotPtr = &style->passiveItem;
							} else if (i == 1) {
								slotPtr = &style->activeItem0;
								isActiveSlot = true;
							} else if (i == 2) {
								slotPtr = &style->activeItem1;
								isActiveSlot = true;
							} else {
								logf("Invalid slot\n");
							}

							Rect slotRect = styleContainer;
							slotRect.width /= slotsMax;
							slotRect.x += slotRect.width * i;
							slotRect = inflatePerc(slotRect, -0.2);
							Item *item = getItem(player, *slotPtr);
							if (item) {
								drawItemIcon(item, slotRect);
								Circle xCircle = makeCircle(v2(slotRect.x + slotRect.width, slotRect.y), game->size.y*0.02);
								drawCircle(xCircle, 0xFFFF0000);
								if (platform->mouseJustUp && contains(xCircle, game->mouse)) *slotPtr = 0;
							} else {
								int color = 0xFF111111;
								if (!isActiveSlot) color = lerpColor(color, 0xFF0000FF, 0.1);
								drawRect(slotRect, color);
							}

							bool canDropItemHere = false;
							Item *draggingItem = getItem(player, game->draggingItemId);
							if (draggingItem && isActiveSlot && draggingItem->info->slotType == ITEM_SLOT_ACTIVE) canDropItemHere = true;
							if (draggingItem && !isActiveSlot && draggingItem->info->slotType == ITEM_SLOT_PASSIVE) canDropItemHere = true;

							if (itemDropped && overlaps(slotRect, draggingRect)) {
								if (canDropItemHere) {
									*slotPtr = game->draggingItemId;
									game->draggingItemId = 0; // Drop instantly to avoid putting in multiple slots on the same frame
								} else {
									logf("Can't put item in this slot...\n");
								}
							}
						}
					}
				}

				if (game->draggingItemId) {
					Item *item = getItem(player, game->draggingItemId);
					drawItemIcon(item, draggingRect);
					if (!platform->mouseDown) game->draggingItemId = 0;
				}
			}
		} ///

		{ /// Corner text
			const int TEXT_LINES_MAX = 32;
			char *textLines[TEXT_LINES_MAX];
			int textLinesNum = 0;

			textLines[textLinesNum++] = "J - Punch";
			textLines[textLinesNum++] = "K - Kick";

			Style *style = &player->styles[player->styleIndex];
			Item *item0 = getItem(player, style->activeItem0);
			Item *item1 = getItem(player, style->activeItem1);
			ActionTypeInfo *info0 = &globals->actionTypeInfos[item0 ? item0->info->actionType : ACTION_NONE];
			ActionTypeInfo *info1 = &globals->actionTypeInfos[item1 ? item1->info->actionType : ACTION_NONE];

			textLines[textLinesNum++] = frameSprintf("U - %s", info0->name);
			textLines[textLinesNum++] = frameSprintf("I - %s", info1->name);

			Vec2 cursor = v2(3, game->size.y);
			for (int i = textLinesNum-1; i >= 0; i--) {
				Vec2 size = getTextSize(game->defaultFont, textLines[i]);
				Vec2 position;
				position.x = cursor.x;
				position.y = cursor.y - size.y;

				Rect rect = makeRect(position, size);
				drawRect(rect, 0x80000000);
				drawText(game->defaultFont, textLines[i], position, 0xFFC0C0C0);

				cursor.y -= size.y;
			}
		} ///

		{ /// Money
			Vec2 cursor = v2();
			char *str = frameSprintf("Money: $%.2f", player->money);
			Vec2 size = drawText(game->defaultFont, str, cursor, 0xFFEEEEEE);
			cursor.y += size.y + 5;
			if (game->leftToBeatTillUnlock > 0) {
				str = frameSprintf("%d left", game->leftToBeatTillUnlock);
				drawText(game->defaultFont, str, cursor, 0xFFEEEEEE);
			}
		} ///

		{ /// Store
			updateStore(player, NULL, elapsed);
		} ///

		{ /// Time
			int h, m, s;
			secsToHMS(&h, &m, &s, game->cityTime);
			Rect rect;
			rect.width = game->size.x * 0.1;
			rect.height = game->size.y * 0.05;
			rect.x = game->size.x/2 - rect.width/2;
			rect.y = game->size.y * 0.01;

			DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFCCCCCC);
			if (h > 0) {
				drawTextInRect(frameSprintf("%02d:%02d:%02d", h, m, s), props, rect);
			} else {
				drawTextInRect(frameSprintf("%02d:%02d", m, s), props, rect);
			}
		}

		popCamera2d();
	} ///
	popTargetTexture(); // game->debugTexture

	for (int i = 0; i < map->actorsNum; i++) { /// Removed marked actors
		Actor *actor = &map->actors[i];
		if (actor->markedForDeletion) {
			if (actor->type == ACTOR_UNIT && actor->team != player->team) {
				int xpToGive = actor->level;
				for (;;) {
					int chunkSize = rndInt(1, 3);
					if (chunkSize > xpToGive) chunkSize = xpToGive;

					Actor *xp = createActor(map, ACTOR_ITEM);
					xp->position = actor->position;
					xp->itemType = ITEM_MONEY;
					xp->itemAmount = chunkSize;
					xp->velo.x = rndFloat(-1, 1);
					xp->velo.y = rndFloat(-1, 1);
					xp->velo.z = rndFloat(0.2, 1)*2;
					xp->velo *= v3(7, 7, 12);

					xpToGive -= chunkSize;
					if (xpToGive <= 0) break;
				}
			}
			removeActorByIndex(map, i);
			i--;
			continue;
		}
	} ///

	popCamera2d();

	game->prevMapTime = game->mapTime;
	game->mapTime += elapsed;

	game->prevTime = game->time;
	game->time += elapsed;
}

void updateStore(Actor *player, Actor *storeActor, float elapsed) {
	Map *map = &game->maps[game->currentMapIndex];

	if (game->inStore) {
		Rect bgRect = makeCenteredRect(game->size/2, game->size*v2(0.8, 0.6));
		drawRect(bgRect, 0xFF222222);

		Rect innerRect = inflatePerc(bgRect, -0.2);

		for (int i = 0; i < game->storeItemsNum; i++) {
			Item *item = &game->storeItems[i];

			Rect itemSlice = innerRect;
			itemSlice.width /= game->storeItemsNum;
			itemSlice.x += i * itemSlice.width;
			drawRect(itemSlice, lerpColor(0xFF000000, 0xFF111111, timePhase(game->time*0.2 + (i*0.1))));

			Rect buyRect = inflatePerc(itemSlice, v2(-0.7, -0.9));
			buyRect.y = itemSlice.y + itemSlice.height - buyRect.height - buyRect.height*0.2;
			drawRect(buyRect, 0xFF111111);
			{
				DrawTextProps props = newDrawTextProps();
				props.font = game->defaultFont;
				props.color = 0xFF909090;
				drawTextInRect("Buy", props, buyRect);
			}

			Rect priceRect = buyRect;
			priceRect.y -= priceRect.height + priceRect.height*0.2;
			priceRect.width = itemSlice.width * 0.9;
			priceRect.x = itemSlice.x + itemSlice.width/2 - priceRect.width/2;
			drawRect(priceRect, 0xFF111111);
			{
				DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFF909090);
				drawTextInRect(frameSprintf("Price $%.0f", item->info->price), props, priceRect);
			}

			Rect descRect = inflatePerc(itemSlice, v2(-0.1, -0.6));
			descRect.y = priceRect.y - descRect.height*1.05;
			drawRect(descRect, 0xFF111111);
			{
				DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFF909090);
				drawTextInRect(item->info->name, props, descRect, v2(0.5, 0));
			}

			if (contains(buyRect, game->mouse) && platform->mouseJustUp) {
				if (player->money >= item->info->price) {
					player->money -= item->info->price;
					giveItem(player, item->type, item->amount); // Maybe giveItem should take an item pointer to copy the id?
					arraySpliceIndex(game->storeItems, game->storeItemsNum, sizeof(Item), i);
					game->storeItemsNum--;
				} else {
					infof("Not enough cash\n");
				}
			}
		}

		Rect exitRect = getCenteredRectOfSize(bgRect, getSize(bgRect)*0.07, v2(0.99, 0.99));
		drawRect(exitRect, 0xFF990000);
		drawTextInRect("Exit", newDrawTextProps(game->defaultFont, 0xFFEEEEEE), exitRect);
		if (contains(exitRect, game->mouse) && platform->mouseJustUp) {
			game->inStore = false;
		}

		game->storeTime += elapsed;
	} else {
		game->storeTime = 0;
	}
}

Map *getMapByName(char *mapName) {
	for (int i = 0; i < MAPS_MAX; i++) {
		Map *map = &game->maps[i];
		if (streq(map->name, mapName)) return map;
	}

	return NULL;
}

Map *getCityMapByCoords(int x, int y) {
	if (x > CITY_COLS-1) return NULL;
	if (y > CITY_ROWS-1) return NULL;
	if (x < 0) return NULL;
	if (y < 0) return NULL;

	int cityIndex = y * CITY_COLS + x;
	int mapIndex = cityIndex + CITY_START_INDEX;
	Map *map = &game->maps[mapIndex];
	return map;
}

Vec2i getCoordsByCityMap(Map *map) {
	int mapIndex = -1;
	for (int i = 0; i < MAPS_MAX; i++) {
		if (&game->maps[i] == map) {
			mapIndex = i;
			break;
		}
	}
	if (mapIndex == -1) return v2i(-1, -1);
	if (mapIndex < CITY_START_INDEX) return v2i(-1, -1);
	mapIndex -= CITY_START_INDEX;
	int x = mapIndex % CITY_COLS;
	int y = mapIndex / CITY_COLS;
	return v2i(x, y);
}

int getIndexByMap(Map *map) {
	for (int i = 0; i < MAPS_MAX; i++) {
		if (&game->maps[i] == map) {
			return i;
		}
	}

	return -1;
}

Actor *createActor(Map *map, ActorType type) {
	if (map->actorsNum > ACTORS_MAX-1) {
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->type == ACTOR_ITEM) {
				logf("Too many items, removing an item\n");
				removeActorByIndex(map, i);
				break;
			}
		}
	}
	if (map->actorsNum > ACTORS_MAX-1) {
		logf("Too many actors, removing actor 0\n");
		removeActorByIndex(map, 0);
	}
	Actor *actor = &map->actors[map->actorsNum++];
	memset(actor, 0, sizeof(Actor));
	actor->id = ++map->nextActorId;
	actor->type = type;
	actor->info = &game->actorTypeInfos[actor->type];
	actor->size = v3(100, 100, 100);
	if (actor->type == ACTOR_UNIT) {
		actor->size = UNIT_SIZE;

		actor->itemsMax = 128;
		actor->items = (Item *)zalloc(sizeof(Item) * actor->itemsMax);

		actor->stats[STAT_DAMAGE] = 10;
		actor->stats[STAT_HP] = 10;
		actor->stats[STAT_MAX_STAMINA] = 10;
		actor->stats[STAT_STAMINA_REGEN] = 10;
		actor->stats[STAT_MOVEMENT_SPEED] = 10;
		actor->stats[STAT_ATTACK_SPEED] = 10;
		actor->level = 10;
	} else if (actor->type == ACTOR_ITEM) {
		actor->size = v3(50, 50, 50);
	}
	return actor;
}

void deleteActor(Map *map, Actor *actor) {
	for (int i = 0; i < map->actorsNum; i++) {
		if (&map->actors[i] == actor) {
			arraySpliceIndex(map->actors, map->actorsNum, sizeof(Actor), i);
			map->actorsNum--;
			i--;
			return;
		}
	}

	logf("Couldn't delete actor, because it's not on this map...\n");
};

Actor *getActor(int id) {
	if (id == 0) return NULL;

	Map *map = &game->maps[game->currentMapIndex];
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->id == id) return actor;
	}
	return NULL;
}

Actor *getActorOfType(Map *map, ActorType type) {
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->type == type) return actor;
	}
	return NULL;
}

Actor *getRandomActorOfType(Map *map, ActorType type) {
	Actor **possible = (Actor **)frameMalloc(sizeof(Actor *) * ACTORS_MAX);
	int possibleNum = 0;
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->type == type) possible[possibleNum++] = actor;
	}

	if (possibleNum == 0) return NULL;
	return possible[rndInt(0, possibleNum-1)];
}

Actor *getActorByName(Map *map, char *actorName) {
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (streq(actor->name, actorName)) return actor;
	}
	return NULL;
}

void removeActorById(Map *map, int id) {
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->id == id) {
			removeActorByIndex(map, i);
			return;
		}
	}

	logf("Tried to remove non-existent actor with id %d\n", id);
}

void removeActorByIndex(Map *map, int index) {
	Actor *actor = &map->actors[index];
	if (actor->type == ACTOR_UNIT) {
		free(actor->items);
		actor->items = NULL;
	}
	arraySpliceIndex(map->actors, map->actorsNum, sizeof(Actor), index);
	map->actorsNum--;
}

Actor *findDoorWithDestMapName(Map *map, char *destMapName) {
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->type == ACTOR_DOOR) {
			if (destMapName == NULL || streq(actor->destMapName, destMapName)) {
				return actor;
			}
		}
	}

	return NULL;
}

Action *addAction(Actor *actor, ActionType type) {
	Globals *globals = &game->globals;

	Action *action = NULL;
	if (actor->actionsNum > ACTIONS_MAX-1) {
		logf("Too many actions!\n");
		action = &actor->actions[ACTIONS_MAX-1];
	}

	action = &actor->actions[actor->actionsNum++];
	memset(action, 0, sizeof(Action));
	action->id = ++game->nextActionId;
	action->type = type;
	action->info = &globals->actionTypeInfos[action->type];
	return action;
}

Buff *addBuff(Actor *actor, BuffType type, float maxTime) {
	if (actor->buffsNum > BUFFS_ON_ACTOR_MAX-1) {
		logf("Too many buffs!\n");
		actor->buffsNum--;
	}

	Buff *buff = &actor->buffs[actor->buffsNum++];
	memset(buff, 0, sizeof(Buff));
	buff->type = type;
	buff->maxTime = maxTime;
	return buff;
}

bool hasBuff(Actor *actor, BuffType type) {
	for (int i = 0; i < actor->buffsNum; i++) {
		Buff *buff = &actor->buffs[i];
		if (buff->type == type) return true;
	}
	return false;
}

int getBuffCount(Actor *actor, BuffType type) {
	int count = 0;

	for (int i = 0; i < actor->buffsNum; i++) {
		Buff *buff = &actor->buffs[i];
		if (buff->type == type) count++;
	}
	return count;
}

void initItem(Item *item, ItemType type, int amount) {
	memset(item, 0, sizeof(Item));
	item->id = ++game->nextItemId;
	item->type = type;
	item->info = &game->itemTypeInfos[type];
	item->amount = amount;
}

Item *giveItem(Actor *actor, ItemType type, int amount) {
	if (actor->itemsNum > actor->itemsMax-1) {
		logf("Too many items\n");
		return NULL;
	}

	Item *item = &actor->items[actor->itemsNum++];
	memset(item, 0, sizeof(Item));
	item->id = ++game->nextItemId;
	item->type = type;
	item->info = &game->itemTypeInfos[type];
	item->amount = amount;
	return item;
}

void removeItem(Actor *actor, ItemType type, int amount) {
	for (int i = 0; i < actor->itemsNum; i++) {
		Item *item = &actor->items[i];
		if (amount <= 0) break;
		if (item->type == type) {
			int amountToRemove = amount;
			if (amountToRemove > item->amount) amountToRemove = item->amount;
			item->amount -= amount;
			amountToRemove -= amount;
			if (item->amount <= 0) {
				arraySpliceIndex(actor->items, actor->itemsNum, sizeof(Item), i);
				actor->itemsNum--;
				i--;
				continue;
			}
		}
	}
}

Item *getItem(Actor *actor, int id) {
	for (int i = 0; i < actor->itemsNum; i++) {
		Item *item = &actor->items[i];
		if (item->id == id) return item;
	}
	return NULL;
}

int getStashedItemCount(Actor *actor, ItemType type) {
	int count = 0;
	for (int i = 0; i < actor->itemsNum; i++) {
		Item *item = &actor->items[i];
		if (item->type == type) count += item->amount;
	}
	return count;
}

int getEquippedItemCount(Actor *actor, ItemType type) {
	int count = 0;
	Style *style = &actor->styles[actor->styleIndex];

	Item *item = NULL;

	item = getItem(actor, style->activeItem0);
	if (item && item->type == type) count += item->amount;

	item = getItem(actor, style->activeItem1);
	if (item && item->type == type) count += item->amount;

	item = getItem(actor, style->passiveItem);
	if (item && item->type == type) count += item->amount;

	return count;
}

float getStatPoints(Actor *actor, StatType stat) {
	float value = actor->stats[stat];
	if (stat == STAT_DAMAGE) {
		value += getStashedItemCount(actor, ITEM_DAMAGE_BOOST);
	} else if (stat == STAT_HP) {
		value += getStashedItemCount(actor, ITEM_HP_BOOST);
	} else if (stat == STAT_STAMINA_REGEN) {
		value += getStashedItemCount(actor, ITEM_STAMINA_REGEN_BOOST);
	} else if (stat == STAT_MAX_STAMINA) {
		value += getStashedItemCount(actor, ITEM_MAX_STAMINA_BOOST);
	} else if (stat == STAT_MOVEMENT_SPEED) {
		value += getStashedItemCount(actor, ITEM_MOVEMENT_SPEED_BOOST);
	} else if (stat == STAT_ATTACK_SPEED) {
		value += getStashedItemCount(actor, ITEM_ATTACK_SPEED_BOOST);
	}
	return value;
}

int getSurroundingAtMaxAlliance(Vec2i startingIndex, int team) {
	int count = 0;
	for (int i = 0; i < 8; i++) {
		Vec2 offset = v2();
		if (i == 0) offset = v2(-1, -1);
		if (i == 1) offset = v2(-1, 0);
		if (i == 2) offset = v2(-1, 1);
		if (i == 3) offset = v2(0, -1);
		if (i == 4) offset = v2(0, 1);
		if (i == 5) offset = v2(1, -1);
		if (i == 6) offset = v2(1, 0);
		if (i == 7) offset = v2(1, 1);
		Map *adjMap = getCityMapByCoords(startingIndex.x+offset.x, startingIndex.y+offset.y);
		if (!adjMap) continue;
		if (adjMap->alliances[team] >= 0.99) count++;
	}

	return count;
}

int getTeamWithMostAlliance(Map *map) {
	int highestTeam = 0;
	for (int i = 0; i < TEAMS_MAX; i++) {
		if (map->alliances[i] > map->alliances[highestTeam]) highestTeam = i;
	}
	return highestTeam;
}

Rect getBounds(AABB aabb) {
	Rect bounds = makeRect();
	Vec3 points[8];
	toPoints(aabb, points);
	for (int i = 0; i < 8; i++) {
		Vec3 point = game->isoMatrix3 * points[i];
		bounds = insert(bounds, v2(point));
	}

	return bounds;
}

AABB getAABBFromSizePosition(Vec3 size, Vec3 position) {
	AABB aabb = makeAABB(position - size/2, position + size/2);
	aabb.min.z += size.z/2;
	aabb.max.z += size.z/2;
	return aabb;
}

AABB getAABBAtPosition(Actor *actor, Vec3 position) {
	return getAABBFromSizePosition(actor->size, position);
}

bool overlaps(Actor *actor0, Actor *actor1) {
	AABB aabb0 = getAABB(actor0);
	AABB aabb1 = getAABB(actor1);
	bool ret = intersects(aabb0, aabb1);
	return ret;
}

bool overlaps(Actor *actor, AABB aabb) {
	bool ret = intersects(getAABB(actor), aabb);
	return ret;
}

float distance(Actor *actor0, Actor *actor1) {
	AABB aabb0 = getAABB(actor0);
	AABB aabb1 = getAABB(actor1);
	float ret = distance(aabb0, aabb1);
	return ret;
}

AABB getAABB(Actor *actor) {
	return getAABBAtPosition(actor, actor->position);
}

AABB bringWithinBounds(AABB groundAABB, AABB aabb) {
	if (aabb.min.x < groundAABB.min.x) aabb += v3(groundAABB.min.x - aabb.min.x + 1, 0, 0);
	if (aabb.max.x > groundAABB.max.x) aabb -= v3(aabb.max.x - groundAABB.max.x + 1, 0, 0);
	if (aabb.min.y < groundAABB.min.y) aabb += v3(0, groundAABB.min.y - aabb.min.y + 1, 0);
	if (aabb.max.y > groundAABB.max.y) aabb -= v3(0, aabb.max.y - groundAABB.max.y + 1, 0);
	return aabb;
}

AABB bringWithinBounds(Map *map, AABB aabb) {
	Actor *ground = getActorOfType(map, ACTOR_GROUND);
	if (!ground) {
		logf("No ground??? (bringWithinBounds)\n");
		return aabb;
	}

	return bringWithinBounds(getAABB(ground), aabb);
}

void bringWithinBounds(Map *map, Actor *actor) {
	AABB aabb = bringWithinBounds(map, getAABB(actor));

	Vec3 newPos = getCenter(aabb);
	newPos.z -= getSize(aabb).z/2;
	actor->position = newPos;
}

void drawAABB3d(AABB aabb, int color) {
	if (game->debugCubesNum > DEBUG_CUBES_MAX-1) {
		logf("Too many debug cubes\n");
		return;
	}

	DebugCube *cube = &game->debugCubes[game->debugCubesNum++];
	memset(cube, 0, sizeof(DebugCube));
	cube->aabb = aabb;
	cube->color = color;
}

void drawAABB2d(AABB aabb, int lineThickness, int color) {
	Vec3 points[24];
	AABBToLines(aabb, points);

	for (int i = 0; i < 12; i++) {
		Vec3 start = game->isoMatrix3 * points[i*2 + 0];
		Vec3 end = game->isoMatrix3 * points[i*2 + 1];
		drawLine(v2(start), v2(end), lineThickness, color);
	}
}

int playWorldSound(char *path, Vec3 worldPosition) {
	if (!game->debugSkipPrewarm && game->mapTime < ROOM_PREWARM_TIME) return 0;

	Sound *sound = getSound(path);
	if (!sound) {
		logf("No sound called %s\n", path);
		return -1;
	}

	int id = playSound(sound)->id;
	WorldChannel *worldChannel = &game->worldChannels[game->worldChannelsNum++];
	memset(worldChannel, 0, sizeof(WorldChannel));
	worldChannel->channelId = id;
	worldChannel->position = worldPosition;
	return id;
}

Effect *createEffect(EffectType type, Vec3 position) {
	if (game->effectsNum > EFFECTS_MAX-1) {
		game->effectsNum = EFFECTS_MAX-1;
		logf("Too many effects!\n");
	}

	Effect *effect = &game->effects[game->effectsNum++];
	memset(effect, 0, sizeof(Effect));
	effect->type = type;
	effect->position = position;
	return effect;
}

Particle *createParticle(ParticleType type) {
	if (game->particlesNum > game->particlesMax-1) {
		game->particlesNum = game->particlesMax-1;
		logf("Too many particles! (should self expand)\n");
	}

	Particle *particle = &game->particles[game->particlesNum++];
	memset(particle, 0, sizeof(Particle));
	particle->type = type;
	particle->tint = 0xFFFFFFFF;
	return particle;
}

void saveMap(Map *map, int mapFileIndex) {
	DataStream *stream = newDataStream();

	int mapVersion = 6;
	writeU32(stream, mapVersion);

	writeString(stream, map->name);
	writeU8(stream, map->isTemplatized);

	int actorsToSave = 0;
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		bool willSave = true;
		if (actor->type == ACTOR_UNIT) willSave = false;
		// if (actor->type == ACTOR_ENEMY) willSave = false;
		// if (actor->type == ACTOR_ITEM) willSave = false;
		if (willSave) actorsToSave++;
	}

	writeU32(stream, actorsToSave);
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		bool willSave = true;
		if (actor->type == ACTOR_UNIT) willSave = false;
		// if (actor->type == ACTOR_ENEMY) willSave = false;
		// if (actor->type == ACTOR_ITEM) willSave = false;
		if (!willSave) continue;

		writeU32(stream, actor->type);
		writeU32(stream, actor->id);
		writeString(stream, actor->name);
		writeVec3(stream, actor->position);
		writeVec3(stream, actor->size);
		writeString(stream, actor->destMapName);
		writeU32(stream, actor->unitsToSpawn);
	}

	writeU32(stream, map->nextActorId);

	for (int i = 0; i < TEAMS_MAX; i++) writeFloat(stream, map->baseAlliances[i]);

	writeDataStream(frameSprintf("assets/maps/map%d.bin", mapFileIndex), stream);
	destroyDataStream(stream);
}

void loadMap(Map *map, int mapFileIndex) {
	memset(map, 0, sizeof(Map));

	DataStream *stream = loadDataStream(frameSprintf("assets/maps/map%d.bin", mapFileIndex));
	if (!stream) {
		logf("No map at index %d\n", mapFileIndex);
		return;
	}

	int mapVersion = readU32(stream);

	readStringInto(stream, map->name, MAP_NAME_MAX_LEN);
	if (mapVersion >= 5) map->isTemplatized = readU8(stream);

	map->actorsNum = readU32(stream);
	memset(map->actors, 0, sizeof(Actor) * ACTORS_MAX);
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		actor->type = (ActorType)readU32(stream);
		actor->info = &game->actorTypeInfos[actor->type];
		actor->id = readU32(stream);
		if (mapVersion >= 4) readStringInto(stream, actor->name, ACTOR_NAME_MAX_LEN);
		actor->position = readVec3(stream);
		actor->size = readVec3(stream);
		if (mapVersion >= 2) readStringInto(stream, actor->destMapName, MAP_NAME_MAX_LEN);
		if (mapVersion >= 3) actor->unitsToSpawn = readU32(stream);
	}

	map->nextActorId = readU32(stream);

	if (mapVersion >= 6) {
		for (int i = 0; i < TEAMS_MAX; i++) map->baseAlliances[i] = readFloat(stream);
	}

	destroyDataStream(stream);
}

void loadAndRefreshMaps() {
	for (int i = 0; i < MAPS_MAX; i++) {
		Map *map = &game->maps[i];

		for (int i = 0; map->actorsNum; i++) { // Free actor memory
			Actor *actor = &map->actors[i];
			if (actor->items) {
				free(actor->items);
				actor->items = NULL;
			}
		}

		loadMap(map, i);

		memcpy(map->alliances, map->baseAlliances, sizeof(float) * TEAMS_MAX);
		for (int i = 0; i < TEAMS_MAX; i++) {
			if (map->alliances[i] >= 1) {
				map->fortifiedByTeam = i;
				map->fortifiedPerc = 1;
			}
		}
	}
}

void saveGlobals() {
	Globals *globals = &game->globals;

	DataStream *stream = newDataStream();

	int globalsVersion = 11;
	writeU32(stream, globalsVersion);

	for (int i = 0; i < ACTION_TYPES_MAX; i++) {
		ActionTypeInfo *info = &globals->actionTypeInfos[i];
		writeString(stream, info->name);
		writeU32(stream, info->flags);
		writeU32(stream, info->startupFrames);
		writeU32(stream, info->activeFrames);
		writeU32(stream, info->recoveryFrames);
		writeU32(stream, info->hitstunFrames);
		writeU32(stream, info->blockstunFrames);
		writeString(stream, info->animationName);
		writeU8(stream, info->animationLoops);
		for (int i = 0; i < HITBOXES_MAX; i++) writeAABB(stream, info->hitboxes[i]);
		writeU32(stream, info->hitboxesNum);
		writeVec3(stream, info->hitVelo);
		writeVec3(stream, info->blockVelo);
		writeFloat(stream, info->damage);
		writeVec3(stream, info->thrust);
		writeU32(stream, info->thrustFrame);
		writeFloat(stream, info->staminaUsage);
		writeU32(stream, info->buffToGet);
		writeFloat(stream, info->buffToGetTime);
		writeU32(stream, info->buffToGive);
		writeFloat(stream, info->buffToGiveTime);
	}

	writeVec3(stream, globals->actorSpriteOffset);
	writeFloat(stream, globals->actorSpriteScale);
	writeFloat(stream, globals->actorSpriteScaleMultiplier);
	writeFloat(stream, globals->movementPercDistanceWalkingRatio);
	writeFloat(stream, globals->movementPercDistanceRunningRatio);

	writeDataStream("assets/info/globals.bin", stream);
	destroyDataStream(stream);
	logf("Globals saved\n");
}

void loadGlobals() {
	Globals *globals = &game->globals;

	memset(globals, 0, sizeof(Globals));

	DataStream *stream = loadDataStream("assets/info/globals.bin");
	if (!stream) return;

	int globalsVersion = readU32(stream);

	for (int i = 0; i < ACTION_TYPES_MAX; i++) {
		ActionTypeInfo *info = &globals->actionTypeInfos[i];
		readStringInto(stream, info->name, ACTION_NAME_MAX_LEN);
		info->flags = readU32(stream);
		info->startupFrames = readU32(stream);
		info->activeFrames = readU32(stream);
		info->recoveryFrames = readU32(stream);
		info->hitstunFrames = readU32(stream);
		info->blockstunFrames = readU32(stream);
		readStringInto(stream, info->animationName, PATH_MAX_LEN);
		info->animationLoops = readU8(stream);
		for (int i = 0; i < HITBOXES_MAX; i++) info->hitboxes[i] = readAABB(stream);
		info->hitboxesNum = readU32(stream);
		info->hitVelo = readVec3(stream);
		info->blockVelo = readVec3(stream);
		info->damage = readFloat(stream);
		info->thrust = readVec3(stream);
		info->thrustFrame = readU32(stream);
		info->staminaUsage = readFloat(stream);
		info->buffToGet = (BuffType)readU32(stream);
		info->buffToGetTime = readFloat(stream);
		info->buffToGive = (BuffType)readU32(stream);
		info->buffToGiveTime = readFloat(stream);
	}

	globals->actorSpriteOffset = readVec3(stream);
	globals->actorSpriteScale = readFloat(stream);
	if (globalsVersion >= 10) globals->actorSpriteScaleMultiplier = readFloat(stream);
	if (globalsVersion >= 11) globals->movementPercDistanceWalkingRatio = readFloat(stream);
	if (globalsVersion >= 11) globals->movementPercDistanceRunningRatio = readFloat(stream);

	destroyDataStream(stream);
}
