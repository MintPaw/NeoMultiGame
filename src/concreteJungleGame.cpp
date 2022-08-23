// Do npc awareness
// Shouldn't block if unaware
// Attacks that break block need to go through
// Running kick needs to launch further
// Make money fade away eventaully
// Make thrown weapons do damage
// Add weapon images while they're on the ground
// Make movement end combos
// Do tick marks on hp bars
// Weapon hitboxes, damage, and velos
// Smooth out movement, blocking, and animation transtions
// Make shops take less in-game time to travel to
// Better map and fortification graphics
// Glowing eyes?

// We need healing
// Do a ghost that is the base speed
// Make running punch more like streak
// Make money counter add up
// Grenade enemy
// Guys that have less hitstun?
// Don't block backwards?

/*

Anims:
dashStart
dashForward
armorGain
drinkPotionStart
drinkPotionFinish
drinkPotionFail
Fail drink potion
Stun snap
createWeapon
stashOnBack
stashOnHip
fireShotgun
openShotgun
loadShotgunShell
closeShotgun
useWeaponOil

Reanim:
walk
handsUpWalk
//run
jump
land
dashStart
dashForward
armorGain
drinkPotionStart
drinkPotionFinish
drinkPotionFail
startSnap(s)
createWeapon
stashOnBack
stashOnHip
fireShotgun
openShotgun
loadShotgunShell
closeShotgun
useWeaponOil

knifeAttaks(s)
swordWalk
swordRun
swordSlash(s)
swordJump
swordLand

throw
airThrow
toss

dodges
blocks(s)

knockdowns(s)/stands(s)
knockedup

wallBounce

crumple

idles

Ending items:
	- [L1] Estus
	- [R1] Dash (Parry if neutral?)
	- [L3?] Hyper armor
	- [R2] Statis attack
	- [Hold pickup] Create weapon (costs 1/2 of remaining hp)
	- [Down] Weapon stash?
	- [L2] Shotgun
	- [Left, Right, Up] Weapon oil?
		- Damage (sharpening)
		- Burning
		- Lightning
		- Speed (wind)
		- Unbreaking


Street/room types:
	- Snowy (slowed, slippery?)
	- Rainy (low visibility)
	- Desert/Hot (stamina regen reduction)
	- Windy (wind)
	- Dark
	- Gravity (high gravity)
	- Shopping
	- Blood moon (high speed, vampirism?)

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

#define GROUND_SPACING 0.1

#define FRAME_SUBSTEPS 1

#define STASIS_TIME_SCALE_AMOUNT 0.1
#define STASIS_STAMINA_PERC 0.95

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
	ACTION_UNUSED_1=21,
	ACTION_BRAIN_SAP=22,
	ACTION_CULLING_BLADE=23,
	ACTION_STICKY_NAPALM=24,
	ACTION_DASH_START=25,
	ACTION_DASH_FORWARD=26,
	ACTION_DRINK_POTION_START=27,
	ACTION_DRINK_POTION_FINISH=28,
	ACTION_DRINK_POTION_FAIL=29,
	ACTION_ARMOR_GAIN=30,
	ACTION_STASIS_GAIN=31,

	ACTION_PUNCH_2=40,

	ACTION_FORCED_IDLE=64,
	ACTION_FORCED_MOVE=65,
	ACTION_FORCED_LEAVE=66,
	ACTION_START_PICKUP=67,
	ACTION_END_PICKUP=68,
	ACTION_THROW=69,
	ACTION_AIR_THROW=70,
	ACTION_ATTACK1_SWORD=71,
	ACTION_ATTACK1_KNIFE=72,
	ACTION_AIR_ATTACK1_SWORD=73,
	ACTION_AIR_ATTACK1_KNIFE=74,
	ACTION_TOSS=75,
	ACTION_CREATE_ITEM=76,
	ACTION_STASH_ON_BACK=77,
	ACTION_STASH_ON_HIP=78,
	ACTION_TYPES_MAX=128,
};

struct Thruster {
	Vec3 accel;
	float maxTime;

	/// Unserialized
	float time;
};

struct ThrusterTrigger {
	int frame;
	bool requiresHit;
	Thruster thruster;
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

	float damage;
	float staminaUsage;

	BuffType buffToGet;
	float buffToGetTime;
	BuffType buffToGive;
	float buffToGiveTime;

#define THRUSTER_TRIGGERS_MAX 4
	ThrusterTrigger thrusterTriggers[THRUSTER_TRIGGERS_MAX];
	int thrusterTriggersNum;

	Thruster hitThruster;
	Thruster blockThruster;
};

struct Globals {
	ActionTypeInfo actionTypeInfos[ACTION_TYPES_MAX];
	Vec3 actorModelOffset;
	float actorModelScale;
	float movementPercDistanceWalkingRatio;
	float movementPercDistanceRunningRatio;
	float specularPower;
	float pixelizePower;
	float fxaaResolutionScale;
	float posterizeGamma;
	float posterizeNumColors;
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
	"Max hp",
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

enum ItemType {
	ITEM_NONE=0,
	ITEM_MONEY=1,
	ITEM_HEALTH_PACK=2,
	ITEM_DAMAGE_BOOST=3,
	ITEM_HP_BOOST=4,
	ITEM_STAMINA_REGEN_BOOST=5,
	ITEM_MAX_STAMINA_BOOST=6,
	ITEM_MOVEMENT_SPEED_BOOST=7,
	ITEM_ATTACK_SPEED_BOOST=8,
	ITEM_MAGNET=9,
	ITEM_HYPER_ARMOR=10, // Doesn't work??
	ITEM_HEAVEN_STEP=11,
	ITEM_SHADOW_STEP=12,
	ITEM_BUDDHA_PALM=13,
	ITEM_BLOOD_RAGE=14,
	ITEM_DASH=15,
	ITEM_BRAIN_SAP=16,
	ITEM_CULLING_BLADE=17,
	ITEM_STICKY_NAPALM=18,
	ITEM_FOOD_0=19,
	ITEM_FOOD_1=20,
	ITEM_FOOD_2=21,
	ITEM_FOOD_3=22,
	ITEM_FOOD_4=23,
	ITEM_FOOD_5=24,
	ITEM_FOOD_6=25,
	ITEM_FOOD_7=26,
	ITEM_FOOD_8=27,
	ITEM_SWORD=28,
	ITEM_KNIFE=29,
	ITEM_TYPES_MAX,
};
enum ItemSlotType {
	ITEM_SLOT_GLOBAL,
	ITEM_SLOT_PASSIVE,
	ITEM_SLOT_ACTIVE,
	ITEM_SLOT_CONSUMABLE,
	ITEM_SLOT_WEAPON,
};
struct ItemTypeInfo {
#define ITEM_NAME_MAX_LEN 32
	char name[ITEM_NAME_MAX_LEN];
	ItemSlotType slotType;
	ActionType actionType;
	float basePrice;
	ItemType preReq;
	int maxAmountFromStore;
	float extraHpFromConsume;

	float statsToGive[STATS_MAX];

	//@playerSaveSerialize
	int everBought;
};
struct Item {
	ItemType type;
	int id;
	int amount;

	float price;

	ItemTypeInfo *info;
};

struct Actor;
struct Action {
	int id;
	ActionType type;
	ActionTypeInfo *info;

	float prevTime;
	float time;
	float customLength;
	Vec3 targetPosition;
	int relatedActorId;

	bool didHitATarget;
};

enum ActorType {
	ACTOR_NONE=0,
	ACTOR_UNIT=1,
	ACTOR_GROUND=2,
	ACTOR_WALL=3,
	ACTOR_BED=4,
	ACTOR_DOOR=5,
	ACTOR_MODEL=6,
	ACTOR_ITEM=7,
	ACTOR_STORE=8,
	ACTOR_TYPES_MAX,
};
struct ActorTypeInfo {
#define ACTOR_TYPE_NAME_MAX_LEN 64
	char name[ACTOR_TYPE_NAME_MAX_LEN];
	bool isWall;
	bool canBeHit;
	bool hasPhysics;
	bool canBeCreatedInEdtior;
};

enum AiState {
	AI_IDLE,
	AI_STAND_NEAR_TARGET,
	AI_STAND_AT_ATTENTION,
	AI_APPROACH_FOR_ATTACK,
};
char *aiStateStrings[] = {
	"AI_IDLE",
	"AI_STAND_NEAR_TARGET",
	"AI_STAND_AT_ATTENTION",
	"AI_APPROACH_FOR_ATTACK",
};

enum AiType {
	AI_NORMAL,
};

enum StoreType {
	STORE_NONE,
	STORE_ATTACK,
	STORE_STAMINA,
	STORE_UTILITY,
	STORE_TYPES_MAX,
};

enum SpinState {
	SPIN_NONE=0,
	SPIN_STRAIGHT,
	SPIN_SPINNING,
};

struct Actor {
	ActorType type;
	int id;
#define ACTOR_NAME_MAX_LEN 64
	char name[ACTOR_NAME_MAX_LEN];

	Vec3 position;
	Vec3 size;
	Skeleton *skeleton;
	Matrix4 modelMatrix;

#define MAP_NAME_MAX_LEN 64
	char destMapName[MAP_NAME_MAX_LEN];
	StoreType destMapStoreType;
	float locked;

	char modelPath[PATH_MAX_LEN];

	/// Unserialized
	ActorTypeInfo *info;
	bool playerControlled;
	int team;
	bool facingLeft;
	float facingAngle;
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

	Vec2 prevMovementVec; // All this is just for running lmao
	float timeSinceLastLeftPress;
	float timeSinceLastRightPress;
	bool isRunningLeft;
	bool isRunningRight;

	AiType aiType;
	AiState prevAiState;
	AiState aiState;
	float aiStateTime;
	int aiTarget;
	Vec2 aiTargetPosition2;
	float aiStateLength;

	float allianceCost;

	bool isLastExitedDoor;
	bool doorPlayerSpawnedOver;

	ItemType itemType;
	float itemAmount;

	Item *items;
	int itemsNum;
	int itemsMax;

	int potionsLeft;
	bool doingSpotDodge;
	bool hasHyperArmor;
	bool hasStasisAttack;

	float stasisTimeLeft;

	Item heldItem;
	Item backItem;
	Item hipItem;

	float awareness;

#define THRUSTERS_MAX 8
	Thruster thrusters[THRUSTERS_MAX];
	int thrustersNum;

	SpinState spin;
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
	MAP_VISUALIZATION_TOTAL_ALLIANCE_SURROUNDING,
};
const char *mapVisualizationStrings[] = {
	"Slices",
	"Bars",
	"Locked in",
	"Max alliance surrounding",
	"Total alliance surrounding",
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
	float perc;
};

enum EffectType {
	EFFECT_ENEMY_DAMAGE,
	EFFECT_PLAYER_DAMAGE,
	EFFECT_BLOCK_DAMAGE,
	EFFECT_MONEY,
	EFFECT_STASIS_GAIN,
};
struct Effect {
	EffectType type;
	Vec3 position;
	float time;
	float value;

#define EFFECT_TEXT_MAX_LEN 16
	char text[EFFECT_TEXT_MAX_LEN];

	int actorId;
	int pulses;
};

enum ParticleType {
	PARTICLE_DUST,
	PARTICLE_STASIS_GAIN,
};
struct Particle {
	ParticleType type;
	Vec3 position;
	Vec2 size;
	Vec3 velo;
	int tint;
	float time;
	float maxTime;
	float delay;
};

struct DrawBillboardCall {
	Camera camera;
	Texture *texture;
	RenderTexture *renderTexture;
	Vec3 position;
	Vec2 size;
	int tint;
	float alpha;
	Rect source;
};

enum WorldElementType {
	WORLD_ELEMENT_CUBE,
	WORLD_ELEMENT_TRIANGLE,
	WORLD_ELEMENT_SPHERE,
	WORLD_ELEMENT_MODEL,
};
struct WorldElement {
	WorldElementType type;
	// union {
	Tri tri;
	Sphere sphere;
	AABB aabb;
	Model *model;
	// };
	int color;
	float alpha;

	Skeleton *skeleton;
	Matrix4 modelMatrix;
};

enum ScreenElementType {
	SCREEN_ELEMENT_RECT,
	SCREEN_ELEMENT_RECT_OUTLINE,
	SCREEN_ELEMENT_LINE,
	SCREEN_ELEMENT_TEXT,
	SCREEN_ELEMENT_TEXT_IN_RECT,
};
struct ScreenElement {
	ScreenElementType type;
	Rect rect;
	int color;

	Line2 line;
	int thickness;

	char *text;
	DrawTextProps drawTextProps;
};

struct StoreData {
	StoreType type;
	int relatedMapIndex;
	int relatedActorId;
#define STORE_ITEMS_MAX 8
	Item items[STORE_ITEMS_MAX];
	int itemsNum;
};

struct Game {
	Font *defaultFont;
	Font *particleFont;
	Font *simpleStatsFont;
	RenderTexture *gameTexture;
	RenderTexture *gameTextureFinal;
	RenderTexture *overlayTexture;
	RenderTexture *mapTexture;

	Shader *pixelizeShader;
	int pixelizePixelRatioLoc;

	Shader *fxaaShader;
	int fxaaResolutionLoc;

	Shader *posterizeShader;
	int posterizeGammaLoc;
	int posterizeNumColorsLoc;

	bool resizeDirty;

	Globals globals;
	bool inEditor;
	float timeScale;
	float prevTime;
	float time;
	Vec2 size;
	Vec2 mouse;

	int hitPauseFrames;
	bool lastStepOfFrame;
	int extraStepsFromSleep;

	ActorTypeInfo actorTypeInfos[ACTOR_TYPES_MAX];

#define MAPS_MAX 128
	Map maps[MAPS_MAX];
	int currentMapIndex;
	int nextMapIndex;
	StoreType nextMapStoreType;
	char lastMapName[MAP_NAME_MAX_LEN];
	float nextMap_t;
	float prevMapTime;
	float mapTime;

	bool alliancesControlled[TEAMS_MAX];

	Vec3 cameraTarget;
	Vec3 visualCameraTarget;
	Camera camera3d;

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
#define STORE_DATAS_MAX 128
	StoreData storeDatas[STORE_DATAS_MAX];
	int storeDatasNum;
	StoreData *currentStoreData;

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

	Vec3 mouseRayPos;
	Vec3 mouseRayDir;

#define BILLBOARDS_MAX 8192
	DrawBillboardCall billboards[BILLBOARDS_MAX];
	int billboardsNum;

#define SCREEN_ELEMENTS_MAX 2048
	ScreenElement screenElements[SCREEN_ELEMENTS_MAX];
	int screenElementsNum;

#define WORLD_ELEMENTS_MAX 2048
	WorldElement worldElements[WORLD_ELEMENTS_MAX];
	int worldElementsNum;

	/// Editor/debug
	int selectedActorId;

	bool debugShowFrameTimes;
	bool debugAlwaysShowWireframes;
	bool debugDrawHitboxes;
	bool debugDrawActorStatus;
	bool debugDrawActorAction;
	bool debugDrawActorStats;
	bool debugDrawActorStatsSimple;
	bool debugDrawUnitCombo;
	bool debugDrawUnitAiState;
	bool debugDrawActorTargets;
	bool debugDrawUnitBoxes;
	bool debugDrawUnitModels;
	bool debugDrawVision;
	bool debugDisablePostProcessing;
	bool debugNeverTakeDamage;
	bool debugNeverLoseStamina;
	bool debugAiNeverApproaches;
	bool debugForceRestock;
	Map *editorSelectedCityMap;
	bool debugShowPrewarm;
	bool debugSkipPrewarm;
	bool debugDrawPathing;
	bool debugDisableSkeletonFrameBlending;
	bool debugPaused;
	float debugTimeScale;

	/// 3D
	Vec2i cameraAngleDegrees;
};
Game *game = NULL;

void runGame();
void updateGame();
void stepGame(float elapsed);
void updateStore(Actor *player, Actor *storeActor, float elapsed);
Map *getMapByName(char *mapName);
Map *getCityMapByCoords(int x, int y);
Vec2i getCoordsByCityMap(Map *map);
int getIndexByMap(Map *map);
Actor *createActor(Map *map, ActorType type);
void deleteActor(Map *map, Actor *actor);
Actor *getActor(Map *map, int id);
Actor *getActorOfType(Map *map, ActorType type);
Actor *getRandomActorOfType(Map *map, ActorType type);
Actor *getActorByName(Map *map, char *actorName);
void removeActorById(Map *map, int id);
void removeActorByIndex(Map *map, int index);
Actor *findDoorWithDestMapName(Map *map, char *destMapName);

Actor *createUnit(Map *map, int team);

Action *addAction(Actor *actor, ActionType type);
Buff *addBuff(Actor *actor, BuffType type, float maxTime);
bool hasBuff(Actor *actor, BuffType type);
int getBuffCount(Actor *actor, BuffType type);
void initItem(Item *item, ItemType type, int amount);
Item *giveItem(Actor *actor, ItemType type, int amount);
void removeItem(Actor *actor, ItemType type, int amount);
Item *getItem(Actor *actor, int id);
int getStashedItemCount(Actor *actor, ItemType type);
float getStatPoints(Actor *actor, StatType stat);

int getSurroundingAtMaxAlliance(Vec2i startingIndex, int team);
int getTeamWithMostAlliance(Map *map);
float countSurroundingAlliance(Vec2i startingIndex, int team);

Rect getBounds(AABB aabb);
AABB getAABBAtPosition(Actor *actor, Vec3 position);
AABB getAABBFromSizePosition(Vec3 size, Vec3 position);
bool overlaps(Actor *actor0, Actor *actor1);
bool overlaps(Actor *actor, AABB aabb);
bool overlaps(Actor *actor, Sphere sphere);
float distance(Actor *actor0, Actor *actor1);
AABB getAABB(Actor *actor);
AABB bringWithinBounds(AABB groundAABB, AABB aabb);
AABB bringWithinBounds(Map *map, AABB aabb);
void bringWithinBounds(Map *map, Actor *actor);
Vec2 getScreenPoint(Vec3 position);

void pushAABB(AABB aabb, int color, float alpha=1);
void pushAABBOutline(AABB aabb, int lineThickness, int color);
void pushSphere(Sphere sphere, int color, float alpha=1);
void pushModel(Model *model, Matrix4 matrix, Skeleton *skeleton=NULL, int color=0xFFFFFFFF);
void pushBillboard(DrawBillboardCall billboard);
WorldElement *createWorldElement();
void pushTriangle(Vec3 vert0, Vec3 vert1, Vec3 vert2, int color);
ScreenElement *createScreenElement();
void pushScreenRect(Rect rect, int color);
void pushScreenRectOutline(Rect rect, int thickness, int color);
void pushScreenLine(Vec2 start, Vec2 end, int thickness, int color);
void pushScreenText(char *text, DrawTextProps drawTextProps);
void pushScreenTextInRect(char *text, DrawTextProps drawTextProps, Rect rect);

int playWorldSound(char *path, Vec3 worldPosition);
Effect *createEffect(EffectType type, Vec3 position);
Particle *createParticle(ParticleType type);
void applyThruster(Actor *actor, Thruster newThruster, bool flipX=false);
Matrix4 getBoneMatrix(Actor *actor, char *boneName);

void saveMap(Map *map, int mapFileIndex);
void loadMap(Map *map, int mapFileIndex);
void refreshMap(Map *map);

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
	nguiInit();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		logf("Game runtime memory is %dmb btw\n", sizeof(Game) / Megabytes(1));
		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 40);
		game->particleFont = createFont("assets/common/arial.ttf", 25);
		game->simpleStatsFont = createFont("assets/common/arial.ttf", 18);

		game->pixelizeShader = loadShader(NULL, "assets/common/shaders/raylib/glsl330/pixelizer.fs");
		game->pixelizePixelRatioLoc = getUniformLocation(game->pixelizeShader, "pixelRatio");

		game->fxaaShader = loadShader(NULL, "assets/common/shaders/raylib/glsl330/fxaa.fs");
		game->fxaaResolutionLoc = getUniformLocation(game->fxaaShader, "resolution");

		game->posterizeShader = loadShader(NULL, "assets/common/shaders/raylib/glsl330/posterization.fs");
		game->posterizeGammaLoc = getUniformLocation(game->posterizeShader, "gamma");
		game->posterizeNumColorsLoc = getUniformLocation(game->posterizeShader, "numColors");

		loadGlobals();

		{ /// Animation data
			char *str = (char *)readFile("assets/skeletons/unit.markers");
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
				char frameStr[32] = {};
				strncpy(frameStr, spacePtr1+1, spacePtr2 - spacePtr1 - 1);
				// marker->frame = atoi(frameStr);
				marker->perc = atof(frameStr);

				char *spacePtr3 = &line[strlen(line)-1];
				strncpy(marker->markerName, spacePtr2+1, spacePtr3 - spacePtr2);

				lineStart = lineEnd+1;
			}
		} ///

		{
			ActorTypeInfo *info;

			info = &game->actorTypeInfos[ACTOR_UNIT];
			strcpy(info->name, "Unit");
			info->canBeHit = true;
			info->hasPhysics = true;

			info = &game->actorTypeInfos[ACTOR_GROUND];
			strcpy(info->name, "Ground");
			info->isWall = true;

			info = &game->actorTypeInfos[ACTOR_WALL];
			strcpy(info->name, "Wall");
			info->canBeCreatedInEdtior = true;
			info->isWall = true;

			info = &game->actorTypeInfos[ACTOR_BED];
			info->canBeCreatedInEdtior = true;
			strcpy(info->name, "Bed");

			info = &game->actorTypeInfos[ACTOR_DOOR];
			info->canBeCreatedInEdtior = true;
			strcpy(info->name, "Door");

			info = &game->actorTypeInfos[ACTOR_MODEL];
			info->canBeCreatedInEdtior = true;
			strcpy(info->name, "Model");

			info = &game->actorTypeInfos[ACTOR_ITEM];
			strcpy(info->name, "Item");
			info->hasPhysics = true;

			info = &game->actorTypeInfos[ACTOR_STORE];
			info->canBeCreatedInEdtior = true;
			strcpy(info->name, "Store");
		}

		{
			ItemTypeInfo *info = NULL;

			for (int i = 0; i < ITEM_TYPES_MAX; i++) {
				info = &game->itemTypeInfos[i];
			}

			info = &game->itemTypeInfos[ITEM_NONE];
			strcpy(info->name, "none");

			info = &game->itemTypeInfos[ITEM_MONEY];
			strcpy(info->name, "money");

			info = &game->itemTypeInfos[ITEM_HEALTH_PACK];
			strcpy(info->name, "health pack (doesn't work)");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->basePrice = 10;

			info = &game->itemTypeInfos[ITEM_DAMAGE_BOOST];
			strcpy(info->name, "damage boost (doesn't work)");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->basePrice = 25;

			info = &game->itemTypeInfos[ITEM_HP_BOOST];
			strcpy(info->name, "hp boost (doesn't work)");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->basePrice = 25;

			info = &game->itemTypeInfos[ITEM_STAMINA_REGEN_BOOST];
			strcpy(info->name, "stamina regen boost (doesn't work)");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->basePrice = 25;

			info = &game->itemTypeInfos[ITEM_MAX_STAMINA_BOOST];
			strcpy(info->name, "max stamina boost (doesn't work)");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->basePrice = 25;

			info = &game->itemTypeInfos[ITEM_MOVEMENT_SPEED_BOOST];
			strcpy(info->name, "movement speed boost (doesn't work)");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->basePrice = 25;

			info = &game->itemTypeInfos[ITEM_ATTACK_SPEED_BOOST];
			strcpy(info->name, "attack speed boost (doesn't work)");
			info->slotType = ITEM_SLOT_GLOBAL;
			info->basePrice = 25;

			info = &game->itemTypeInfos[ITEM_MAGNET];
			strcpy(info->name, "magnet (doesn't work)");
			info->slotType = ITEM_SLOT_PASSIVE;
			info->basePrice = 30;

			info = &game->itemTypeInfos[ITEM_HYPER_ARMOR];
			strcpy(info->name, "hyper armor (doesn't work)");
			info->slotType = ITEM_SLOT_PASSIVE;
			info->basePrice = 40;

			info = &game->itemTypeInfos[ITEM_HEAVEN_STEP];
			strcpy(info->name, "heaven step (doesn't work)");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->basePrice = 50;
			info->actionType = ACTION_HEAVEN_STEP;

			info = &game->itemTypeInfos[ITEM_SHADOW_STEP];
			strcpy(info->name, "shadow step (doesn't work)");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->basePrice = 60;
			info->actionType = ACTION_SHADOW_STEP;

			info = &game->itemTypeInfos[ITEM_BUDDHA_PALM];
			strcpy(info->name, "buddha palm (doesn't work)");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->basePrice = 70;
			info->actionType = ACTION_BUDDHA_PALM;

			info = &game->itemTypeInfos[ITEM_BLOOD_RAGE];
			strcpy(info->name, "blood rage (doesn't work)");
			info->slotType = ITEM_SLOT_PASSIVE;
			info->basePrice = 80;

			info = &game->itemTypeInfos[ITEM_DASH];
			strcpy(info->name, "dash (doesn't work)");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->basePrice = 90;
			// info->actionType = ACTION_DASH;

			info = &game->itemTypeInfos[ITEM_BRAIN_SAP];
			strcpy(info->name, "brain sap (doesn't work)");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->basePrice = 100;
			info->actionType = ACTION_BRAIN_SAP;

			info = &game->itemTypeInfos[ITEM_CULLING_BLADE];
			strcpy(info->name, "culling blade (doesn't work)");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->basePrice = 110;
			info->actionType = ACTION_CULLING_BLADE;

			info = &game->itemTypeInfos[ITEM_STICKY_NAPALM];
			strcpy(info->name, "sticky napalm (doesn't work)");
			info->slotType = ITEM_SLOT_ACTIVE;
			info->basePrice = 120;
			info->actionType = ACTION_STICKY_NAPALM;

			info = &game->itemTypeInfos[ITEM_SWORD];
			strcpy(info->name, "Sword");
			info->slotType = ITEM_SLOT_WEAPON;
			info->basePrice = 0;

			info = &game->itemTypeInfos[ITEM_KNIFE];
			strcpy(info->name, "Knife");
			info->slotType = ITEM_SLOT_WEAPON;
			info->basePrice = 0;

			for (int i = 0; i < 9; i++) {
				ItemType type = (ItemType)(ITEM_FOOD_0 + i);
				info = &game->itemTypeInfos[type];
				sprintf(info->name, "food %d", i);
				info->slotType = ITEM_SLOT_CONSUMABLE;
				info->basePrice = 5;
				info->maxAmountFromStore = 3;
				info->extraHpFromConsume = 50;

				StoreType storeType = (StoreType)((i / 3)+1);
				int variantIndex = i % 3;
				if (storeType == STORE_ATTACK) {
					if (variantIndex == 0 || variantIndex == 2) info->statsToGive[STAT_DAMAGE]++;
					if (variantIndex == 1 || variantIndex == 2) info->statsToGive[STAT_ATTACK_SPEED]++;
				} else if (storeType == STORE_STAMINA) {
					if (variantIndex == 0 || variantIndex == 2) info->statsToGive[STAT_STAMINA_REGEN]++;
					if (variantIndex == 1 || variantIndex == 2) info->statsToGive[STAT_MAX_STAMINA]++;
				} else if (storeType == STORE_UTILITY) {
					if (variantIndex == 0 || variantIndex == 2) info->statsToGive[STAT_HP]++;
					if (variantIndex == 1 || variantIndex == 2) info->statsToGive[STAT_MOVEMENT_SPEED]++;
				}
			}
		}

		game->particlesMax = 128;
		game->particles = (Particle *)zalloc(sizeof(Particle) * game->particlesMax);

		game->debugTimeScale = 1;
		game->debugDrawUnitModels = true;
		game->debugDrawActorStatsSimple = true;

		game->cameraAngleDegrees.x = 75;
		game->cameraAngleDegrees.y = 3;

		maximizeWindow();
	}

	Globals *globals = &game->globals;

	if (keyJustPressed('0')) game->debugPaused = !game->debugPaused;
	if (keyJustPressed('-')) {
		game->debugTimeScale -= 0.1;
		logf("Time scale: %.2f\n", game->debugTimeScale);
	}
	if (keyJustPressed('=')) {
		game->debugTimeScale += 0.1;
		logf("Time scale: %.2f\n", game->debugTimeScale);
	}

	float timeScale = 1 * game->debugTimeScale;

	if (game->hitPauseFrames > 0) {
		game->hitPauseFrames--;
		timeScale = 0.0001;
	}

	if  (game->debugPaused) timeScale = 0.0001;

	float elapsed = platform->elapsed * timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	game->mouse = platform->mouse;

	{ /// Resizing
		Vec2 newSize = v2(platform->windowWidth, platform->windowHeight);
		if (!equal(game->size, newSize)) game->resizeDirty = true;

		if (game->resizeDirty) {
			game->resizeDirty = false;
			game->size = newSize;

			if (game->overlayTexture) destroyTexture(game->overlayTexture);
			game->overlayTexture = NULL;
			if (game->gameTexture) destroyTexture(game->gameTexture);
			game->gameTexture = NULL;
			if (game->gameTextureFinal) destroyTexture(game->gameTextureFinal);
			game->gameTextureFinal = NULL;
			if (game->mapTexture) destroyTexture(game->mapTexture);
			game->mapTexture = NULL;

			Vec2 pixelRatio = globals->pixelizePower * (1/game->size);
			setShaderUniform(game->pixelizeShader, game->pixelizePixelRatioLoc, &pixelRatio.x, SHADER_UNIFORM_VEC2, 1);

			Vec2 fxaaSize = game->size * globals->fxaaResolutionScale;
			setShaderUniform(game->fxaaShader, game->fxaaResolutionLoc, &fxaaSize.x, SHADER_UNIFORM_VEC2, 1);

			setShaderUniform(game->posterizeShader, game->posterizeGammaLoc, &globals->posterizeGamma, SHADER_UNIFORM_FLOAT, 1);
			setShaderUniform(game->posterizeShader, game->posterizeNumColorsLoc, &globals->posterizeNumColors, SHADER_UNIFORM_FLOAT, 1);
		}
	} ///

	if (!game->mapTexture) game->mapTexture = createRenderTexture(game->size.x, game->size.y);
	if (!game->overlayTexture) game->overlayTexture = createRenderTexture(game->size.x, game->size.y);
	if (!game->gameTexture) game->gameTexture = createRenderTexture(game->size.x, game->size.y);
	if (!game->gameTextureFinal) game->gameTextureFinal = createRenderTexture(game->size.x, game->size.y);
	pushTargetTexture(game->gameTexture);

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;

	int extraStepsThisFrame = MinNum(game->extraStepsFromSleep, 1000);
	game->extraStepsFromSleep -= extraStepsThisFrame;

	int steps = 1 + extraStepsThisFrame;

	if (!game->debugSkipPrewarm && game->mapTime < ROOM_PREWARM_TIME) {
		steps = 20;
	}

	for (int i = 0; i < steps; i++) {
		game->lastStepOfFrame = i == (steps-1);
		renderer->disabled = !game->lastStepOfFrame;
		fontSys->disabled = renderer->disabled;
		stepGame(elapsed);

		{ /// Draw 3d
			if (game->lastStepOfFrame) {
				clearRenderer();
				start3d(game->camera3d, game->size, -10000, 10000);

				Vec3 sunPosition = v3(104.000, -134.000, 66.000);

				renderer->lights[0].position.x = sunPosition.x;
				renderer->lights[0].position.y = sunPosition.y;
				renderer->lights[0].position.z = sunPosition.z;
				updateLightingShader(game->camera3d);

				getMouseRay(game->camera3d, game->mouse, &game->mouseRayPos, &game->mouseRayDir);

				for (int i = 0; i < game->worldElementsNum; i++) {
					WorldElement *element = &game->worldElements[i];

					if (element->type == WORLD_ELEMENT_CUBE) {
						drawAABB(element->aabb, element->color);
					} else if (element->type == WORLD_ELEMENT_TRIANGLE) {
						Vec2 uvs[3] = {};
						logf("drawTriangle is gone now\n");
						// drawTriangle(NULL, element->tri.verts, uvs);
					} else if (element->type == WORLD_ELEMENT_SPHERE) {
						drawSphere(element->sphere, element->color);
					} else if (element->type == WORLD_ELEMENT_MODEL) {

						Material material = createMaterial();
						material.shader = renderer->lightingAnimatedShader;
						material.values[Raylib::MATERIAL_MAP_DIFFUSE].color = hexToArgbFloat(element->color);
						material.values[Raylib::MATERIAL_MAP_SPECULAR].color = v4(0, globals->specularPower, 0, 0);
						replaceAllMaterials(element->model, material);
						drawModel(element->model, element->modelMatrix, element->skeleton, element->color);
					}
				}

				// {
				// 	Model *model = getModel("assets/models/mocapUnit/mocapUnit.model");
				// 	static Skeleton *skeleton = NULL;

				// 	if (!skeleton) {
				// 		skeleton = deriveSkeleton("assets/skeletons/mocaprig.skele");

				// 		SkeletonBlend *blend = createSkeletonBlend(skeleton, "main", SKELETON_BLEND_ANIMATION);
				// 		blend->animation = getAnimation(skeleton, "a1_remap");
				// 	}

				// 	updateSkeleton(skeleton, elapsed);

				// 	Matrix4 modelMatrix = mat4();
				// 	modelMatrix.SCALE(globals->actorModelScale*6);
				// 	drawModel(model, modelMatrix, skeleton);
				// }

				setDepthMask(false);
				for (int i = 0; i < game->billboardsNum; i++) {
					DrawBillboardCall *billboard = &game->billboards[i];
					if (isZero(billboard->camera.up)) billboard->camera = game->camera3d;

					int tint = billboard->tint;
					Vec4 tintVec = hexToArgbFloat(billboard->tint);
					tintVec.x *= billboard->alpha;
					tint = argbToHex(tintVec);

					if (billboard->texture) {
						drawBillboard(
							billboard->camera,
							billboard->texture,
							billboard->position,
							billboard->size,
							tint,
							billboard->source
						);
					} else {
						drawBillboard(
							billboard->camera,
							billboard->renderTexture,
							billboard->position,
							billboard->size,
							tint,
							billboard->source
						);
					}
				}
				setDepthMask(true);

				end3d();
			}
			game->worldElementsNum = 0;
			game->billboardsNum = 0;
		} ///
	}

	popTargetTexture(); // game->gameTexture

	clearRenderer();

	// Probably not needed
	pushTargetTexture(game->gameTextureFinal);
	clearRenderer();
	drawSimpleTexture(game->gameTexture);
	popTargetTexture();

	auto copyBackGameTexture = []() {
		pushTargetTexture(game->gameTexture);
		clearRenderer();
		drawSimpleTexture(game->gameTextureFinal);
		popTargetTexture();
	};

	if (!game->debugDisablePostProcessing) {
		if (globals->posterizeNumColors != 0) {
			copyBackGameTexture();

			pushTargetTexture(game->gameTextureFinal);
			clearRenderer();
			startShader(game->posterizeShader);

			drawSimpleTexture(game->gameTexture);

			endShader();
			popTargetTexture();
		}

		if (globals->fxaaResolutionScale != 0) {
			copyBackGameTexture();

			pushTargetTexture(game->gameTextureFinal);
			clearRenderer();
			startShader(game->fxaaShader);

			drawSimpleTexture(game->gameTexture);

			endShader();
			popTargetTexture();
		}

		if (globals->pixelizePower != 0) {
			copyBackGameTexture();

			pushTargetTexture(game->gameTextureFinal);
			startShader(game->pixelizeShader);
			drawSimpleTexture(game->gameTexture);
			endShader();
			popTargetTexture();
		}
	}

	{
		RenderTexture *texture = game->gameTextureFinal;
		Matrix3 matrix = mat3();
		matrix.SCALE(game->size);

		drawSimpleTexture(texture, matrix);
	}

	{
		RenderTexture *texture = game->overlayTexture;
		Matrix3 matrix = mat3();
		matrix.SCALE(game->size);

		drawSimpleTexture(texture, matrix);
	}

	{ /// Update screen elements
		for (int i = 0; i < game->screenElementsNum; i++) {
			ScreenElement *element = &game->screenElements[i];

			if (element->type == SCREEN_ELEMENT_RECT) {
				drawRect(element->rect, element->color);
			} else if (element->type == SCREEN_ELEMENT_RECT_OUTLINE) {
				drawRectOutline(element->rect, element->thickness, element->color);
			} else if (element->type == SCREEN_ELEMENT_LINE) {
				drawLine(element->line, element->thickness, element->color);
			} else if (element->type == SCREEN_ELEMENT_TEXT) {
				drawText(element->text, element->drawTextProps);
			} else if (element->type == SCREEN_ELEMENT_TEXT_IN_RECT) {
				drawTextInRect(element->text, element->drawTextProps, element->rect);
			}
		}
		game->screenElementsNum = 0;
	} ///

	float fadeOutPerc = game->nextMap_t;
	if (!game->debugSkipPrewarm && !game->debugShowPrewarm) {
		fadeOutPerc += clampMap(game->mapTime, ROOM_PREWARM_TIME, ROOM_PREWARM_TIME+0.25, 1, 0);
	}
	fadeOutPerc = Clamp01(fadeOutPerc);
	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, fadeOutPerc));

	{
		RenderTexture *texture = game->mapTexture;
		Matrix3 matrix = mat3();
		matrix.SCALE(game->size);

		drawSimpleTexture(texture, matrix);
	}

	ngui->mouse = game->mouse;
	nguiDraw(elapsed);

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) {
		char *str = frameSprintf("%.1fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
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

			Vec2 screenPos = getScreenPoint(worldChannel->position);

			Vec2 screenPerc = (game->size/2 - screenPos) / game->size*2;
			float distPerc = distance(v2(), screenPerc);
			float vol = clampMap(distPerc, 0, 2, 1, 0);
			float pan = screenPerc.x;
			channel->userVolume2 = vol;
			channel->pan = pan;
		}
	} ///

	guiDraw();
	drawOnScreenLog();
}

void stepGame(float elapsed) {
	if (elapsed > (1/60.0) + 0.001 && platform->frameCount % (60*5) == 0) logf("You shouldn't use high time steps...\n");
	Globals *globals = &game->globals;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	{ /// Init city (and maps)
		if (!game->cityInited) {
			game->cityInited = true;
			{ //loadAndRefreshMaps
				for (int i = 0; i < MAPS_MAX; i++) {
					Map *map = &game->maps[i];

					while (map->actorsNum > 0) {
						removeActorByIndex(map, 0);
					}

					loadMap(map, i);
					refreshMap(map);
				}
			}
			game->currentMapIndex = 0;
			game->mapTime = 0;
			game->timeTillNextCityTick = 0;
			game->cityTime = 0;
			game->cityTicks = 0;
			game->inStore = false;
			game->inInventory = false;
			game->lookingAtMap = false;

			memset(game->alliancesControlled, 0, sizeof(bool) * TEAMS_MAX);
			game->alliancesControlled[0] = true;
		}
	} ///

	{ /// Set up matrices
		Vec2 screenScale = v2(platform->windowSize) / v2(1920, 1080);
		float zoom = MinNum(screenScale.x, screenScale.y);

		game->visualCameraTarget = lerp(game->visualCameraTarget, game->cameraTarget, 0.5);
		if (distance(game->visualCameraTarget, game->cameraTarget) > 100) game->visualCameraTarget = game->cameraTarget;

		Matrix3 matrix = mat3();
		matrix.ROTATE(game->cameraAngleDegrees.y);
		matrix.ROTATE_X(game->cameraAngleDegrees.x);
		Vec3 cameraPos = (matrix * v3(0, 0, 1000)) + game->visualCameraTarget;

		game->camera3d.position = cameraPos;
		game->camera3d.target = game->visualCameraTarget;
		game->camera3d.up = v3(0, 0, 1);
		game->camera3d.orthoScale = zoom;
		game->camera3d.isOrtho = true;
	} ///

	bool inRoomPrewarm = false;
	{ /// Change map
		if (game->nextMapIndex != game->currentMapIndex) {
			bool changeMaps = false;
			if (!game->debugShowPrewarm) {
				if (game->nextMap_t <= 0) game->lookingAtMap = true;
			}
			game->nextMap_t += 0.1;
			if (game->nextMap_t > 1) changeMaps = true;

			if (changeMaps) {
				strcpy(game->lastMapName, game->maps[game->currentMapIndex].name);
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
					// Should factor into moveActor()? // You really should, because them itemsPtr fixup is really weird @incomplete
					Actor *newActor = createActor(destMap, ACTOR_UNIT);
					int id = newActor->id;
					Skeleton *skeleton = newActor->skeleton;
					Item *itemsPtr = newActor->items;
					memcpy(itemsPtr, player->items, sizeof(Item) * player->itemsNum);
					*newActor = *player;
					newActor->id = id;
					newActor->items = itemsPtr;
					newActor->skeleton = skeleton;
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

			if (actor->markedForDeletion) {
				removeActorByIndex(map, i);
				i--;
				continue;
			}

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
		// pushAABB(backWall, 0xFFFF0000);

		AABB frontWall = makeAABB();
		frontWall.min.x = groundAABB.min.x;
		frontWall.max.x = groundAABB.max.x;

		frontWall.min.y = groundAABB.min.y - wallThickness;
		frontWall.max.y = frontWall.min.y + wallThickness;

		frontWall.min.z = groundAABB.min.z;
		frontWall.max.z = frontWall.min.z + 1000;
		walls[wallsNum++] = frontWall;
		// pushAABB(frontWall, 0xFFFF0000);

		AABB leftWall = makeAABB();
		leftWall.min.x = groundAABB.min.x - wallThickness;
		leftWall.max.x = leftWall.min.x + wallThickness;

		leftWall.min.y = groundAABB.min.y;
		leftWall.max.y = groundAABB.max.y;

		leftWall.min.z = groundAABB.min.z;
		leftWall.max.z = leftWall.min.z + 1000;
		walls[wallsNum++] = leftWall;
		// pushAABB(leftWall, 0xFFFF0000);

		AABB rightWall = makeAABB();
		rightWall.min.x = groundAABB.max.x;
		rightWall.max.x = rightWall.min.x + wallThickness;

		rightWall.min.y = groundAABB.min.y;
		rightWall.max.y = groundAABB.max.y;

		rightWall.min.z = groundAABB.min.z;
		rightWall.max.z = rightWall.min.z + 1000;
		walls[wallsNum++] = rightWall;
		// pushAABB(rightWall, 0xFFFF0000);
	} ///

	if (game->lastStepOfFrame && game->inEditor) { /// Editor update
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::TreeNodeEx("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Always show wireframes", &game->debugAlwaysShowWireframes);
			ImGui::Checkbox("Draw hitboxes", &game->debugDrawHitboxes);
			ImGui::Checkbox("Draw actor status", &game->debugDrawActorStatus);
			ImGui::Checkbox("Draw actor action", &game->debugDrawActorAction);
			ImGui::Checkbox("Draw actor stats", &game->debugDrawActorStats);
			ImGui::Checkbox("Draw actor stats simple", &game->debugDrawActorStatsSimple);
			ImGui::Checkbox("Draw unit combo", &game->debugDrawUnitCombo);
			ImGui::Checkbox("Draw unit ai state", &game->debugDrawUnitAiState);
			ImGui::Checkbox("Draw actor targets", &game->debugDrawActorTargets);
			ImGui::Checkbox("Draw unit boxes", &game->debugDrawUnitBoxes);
			ImGui::Checkbox("Draw unit models", &game->debugDrawUnitModels);
			ImGui::Checkbox("Draw vision", &game->debugDrawVision);
			ImGui::Checkbox("Disable post processing", &game->debugDisablePostProcessing);
			ImGui::Checkbox("Show prewarm", &game->debugShowPrewarm);
			ImGui::Checkbox("Skip prewarm", &game->debugSkipPrewarm);
			ImGui::Checkbox("Draw pathing", &game->debugDrawPathing);
			ImGui::Checkbox("Disable skeleton frame blending", &game->debugDisableSkeletonFrameBlending);
			skeletonSys->disableFrameBlending = game->debugDisableSkeletonFrameBlending;

			ImGui::SliderInt("Degs1", &game->cameraAngleDegrees.x, 0, 90);
			ImGui::SliderInt("Degs2", &game->cameraAngleDegrees.y, -90, 90);
			if (ImGui::Button("Reset")) {
				game->cameraAngleDegrees.x = 75;
				game->cameraAngleDegrees.y = 3;
			}

			if (ImGui::Button("Spawn low max hp enemy")) {
				Actor *actor = createActor(map, ACTOR_UNIT);
				actor->stats[STAT_HP] = 2;
				actor->team = 1;
				actor->position = v3(0, 0, 10);
			}

			ImGui::SameLine();
			if (ImGui::Button("Spawn Dummy")) {
				Actor *actor = createUnit(map, 2);
				actor->stats[STAT_MAX_STAMINA] = 1;
				actor->stats[STAT_STAMINA_REGEN] = 1;
				actor->stats[STAT_HP] = 500;
				actor->stats[STAT_MOVEMENT_SPEED] = 1;
				actor->stats[STAT_ATTACK_SPEED] = 1;
				actor->hasHyperArmor = false;
				actor->position = v3(0, 0, 10);
			}

			ImGui::Text("Spawn unit from team:");
			for (int i = 0; i < TEAMS_MAX; i++) {
				if (ImGui::Button(frameSprintf("%d###teamSpawn%d", i, i))) {
					Actor *actor = createUnit(map, i);
					actor->stats[STAT_DAMAGE] = 10;
					actor->stats[STAT_HP] = 10;
					actor->stats[STAT_MAX_STAMINA] = 10;
					actor->stats[STAT_STAMINA_REGEN] = 10;
					actor->stats[STAT_MOVEMENT_SPEED] = 10;
					actor->stats[STAT_ATTACK_SPEED] = 10;
					actor->position = v3(0, 0, 10);
				}
				ImGui::SameLine();
			}
			ImGui::NewLine();

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

			if (ImGui::Button("Spawn sword")) {
				Actor *actor = createActor(map, ACTOR_ITEM);
				actor->itemType = ITEM_SWORD;
				actor->position = v3(0, 0, 10);
			}
			ImGui::SameLine();
			if (ImGui::Button("Spawn knife")) {
				Actor *actor = createActor(map, ACTOR_ITEM);
				actor->itemType = ITEM_KNIFE;
				actor->position = v3(0, 0, 10);
			}

			ImGui::Checkbox("Never take damage", &game->debugNeverTakeDamage);
			ImGui::Checkbox("Never lose stamina", &game->debugNeverLoseStamina);
			ImGui::Checkbox("AI never approaches", &game->debugAiNeverApproaches);

			ImGui::Text("alliancesControlled: ");
			ImGui::SameLine();
			for (int i = 0; i < TEAMS_MAX; i++) {
				guiPushStyleColor(ImGuiCol_FrameBg, lerpColor(teamColors[i], 0xFF000000, 0.5));
				ImGui::Checkbox(frameSprintf("###%d", i), &game->alliancesControlled[i]);
				guiPopStyleColor();
				ImGui::SameLine();
			}
			ImGui::NewLine();
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
							// "#cornerBuildings",
							// "#longRoom",
						};
						templateMap = getMapByName(choices[rndInt(0, ArrayLength(choices)-1)]);
						float totalBaseAlliance = 0;
						for (int i = 0; i < TEAMS_MAX; i++) totalBaseAlliance += map->baseAlliances[i];
						if (totalBaseAlliance >= 0.75) templateMap = getMapByName("#cornerBuildings");

						float baseAlliances[TEAMS_MAX];
						memcpy(baseAlliances, map->baseAlliances, sizeof(float) * TEAMS_MAX);
						*map = *templateMap;

						strcpy(map->name, frameSprintf("city%d-%d", x, y));
						map->isTemplatized = true;
						memcpy(map->baseAlliances, baseAlliances, sizeof(float) * TEAMS_MAX);

						if (streq(templateMap->name, "#cornerBuildings")) {
							Actor *leftBuilding = getActorByName(map, "leftBuilding");
							leftBuilding->size.x *= rndFloat(0.5, 1);
							leftBuilding->size.y *= rndFloat(0.5, 1);
							leftBuilding->position.x -= 2000;
							leftBuilding->position.y += 2000;
							bringWithinBounds(map, leftBuilding);

							Actor *rightBuilding = getActorByName(map, "rightBuilding");
							rightBuilding->size.x *= rndFloat(0.5, 1);
							rightBuilding->size.y *= rndFloat(0.5, 1);
							rightBuilding->position.x += 2000;
							rightBuilding->position.y += 2000;
							bringWithinBounds(map, rightBuilding);

							Actor *door = createActor(map, ACTOR_DOOR);
							if (rndPerc(0.5)) {
								door->size = v3(20, 300, 350);
								door->position.x = leftBuilding->position.x + leftBuilding->size.x/2 + door->size.x/2;
								door->position.y = leftBuilding->position.y;
								strcpy(door->destMapName, "shopLeft");
							} else {
								door->size = v3(300, 20, 350);
								door->position.x = rightBuilding->position.x;
								door->position.y = rightBuilding->position.y - rightBuilding->size.y/2 - door->size.y/2;
								strcpy(door->destMapName, "shopTop");
							}
							door->position.z = groundAABB.max.z;

							door->destMapStoreType = (StoreType)rndInt(1, STORE_TYPES_MAX-1);
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

						refreshMap(map);
					}
				}
				logf("Maps templatized\n");
			}
			if (ImGui::Button("Save all city maps")) {
				for (int y = 0; y < CITY_ROWS; y++) {
					for (int x = 0; x < CITY_COLS; x++) {
						int cityIndex = y * CITY_COLS + x;
						int mapIndex = cityIndex + CITY_START_INDEX;
						Map *map = &game->maps[mapIndex];
						saveMap(map, mapIndex);
					}
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Save all maps")) {
				for (int i = 0; i < MAPS_MAX; i++) {
					Map *map = &game->maps[i];
					saveMap(map, i);
				}
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

		if (ImGui::TreeNode("Globals")) {
			if (ImGui::Button("Save globals")) saveGlobals();
			ImGui::SameLine();
			if (ImGui::Button("Load globals")) loadGlobals();
			if (ImGui::TreeNode("Action type infos")) {
				if (ImGui::Button("Remove player actions")) player->actionsNum = 0;
				for (int i = 0; i < ACTION_TYPES_MAX; i++) {
					if (i >= ACTION_EXTRA_4 && i <= ACTION_EXTRA_8) continue;
					ActionTypeInfo *info = &globals->actionTypeInfos[i];
					if (ImGui::TreeNode(frameSprintf("%d: %s###%d", i, info->name, i))) {
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

						if (ImGui::TreeNode("Thrusters")) {
							ImGui::Text("Hit thruster");
							ImGui::InputFloat3("Accel###hitThrusterAccel", &info->hitThruster.accel.x);
							ImGui::InputFloat("Max time###hitThrusterMaxTime", &info->hitThruster.maxTime);
							ImGui::Separator();
							ImGui::Text("Block thruster");
							ImGui::InputFloat3("Accel###blockThrusterAccel", &info->blockThruster.accel.x);
							ImGui::InputFloat("Max time###blockThrusterMaxTime", &info->blockThruster.maxTime);
							ImGui::Separator();
							if (ImGui::TreeNode("Triggers")) {
								for (int i = 0; i < info->thrusterTriggersNum; i++) {
									ThrusterTrigger *trigger = &info->thrusterTriggers[i];
									ImGui::InputInt("Frame", &trigger->frame);
									ImGui::Checkbox("Requires hit", &trigger->requiresHit);

									Thruster *thruster = &trigger->thruster;
									ImGui::InputFloat3("Accel", &thruster->accel.x);
									ImGui::InputFloat("Max time", &thruster->maxTime);
									ImGui::Separator();
								}
								ImGui::InputInt("Thuster triggers num", &info->thrusterTriggersNum);
								info->thrusterTriggersNum = mathClamp(info->thrusterTriggersNum, 0, THRUSTER_TRIGGERS_MAX);
								ImGui::TreePop();
							}

							ImGui::TreePop();
						}
						ImGui::InputFloat("Damage", &info->damage);
						ImGui::InputFloat("Stamina usage", &info->staminaUsage);
						if (ImGui::TreeNode("Buffs")) {
							ImGui::Combo("Buff to get", (int *)&info->buffToGet, buffTypeStrings, ArrayLength(buffTypeStrings));
							if (info->buffToGet) ImGui::InputFloat("Buff to get time", &info->buffToGetTime, 0);
							ImGui::Combo("Buff to give", (int *)&info->buffToGive, buffTypeStrings, ArrayLength(buffTypeStrings));
							if (info->buffToGive) ImGui::InputFloat("Buff to give time", &info->buffToGiveTime, 0);
							ImGui::TreePop();
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

			ImGui::DragFloat3("Actor model offset", &globals->actorModelOffset.x);
			ImGui::DragFloat("Actor model scale", &globals->actorModelScale, 0.01);
			ImGui::DragFloat("Movement perc distance walking ratio", &globals->movementPercDistanceWalkingRatio, 0.01, 0, 0, "%.4f");
			ImGui::DragFloat("Movement perc distance running ratio", &globals->movementPercDistanceRunningRatio, 0.01, 0, 0, "%.4f");
			ImGui::Separator();

			ImGui::DragFloat("Specular power", &globals->specularPower, 0.01, 0, 0, "%.4f");
			if (ImGui::DragFloat("Pixelize power", &globals->pixelizePower, 0.01, 0, 0, "%.4f")) game->resizeDirty = true;
			if (ImGui::DragFloat("Fxaa resolution scale", &globals->fxaaResolutionScale, 0.01, 0, 0, "%.4f")) game->resizeDirty = true;
			if (ImGui::DragFloat("Posterize numColors", &globals->posterizeNumColors, 0.1, 0, 0, "%.4f")) game->resizeDirty = true;
			if (ImGui::DragFloat("Posterize gamma", &globals->posterizeGamma, 0.01, 0, 0, "%.4f")) game->resizeDirty = true;
			ImGui::TreePop();
		}
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth, 0), ImGuiCond_Always, ImVec2(1, 0));
		ImGui::Begin("Actors", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::BeginChild("ActorListChild", ImVec2(300, 200));
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			char *name = frameSprintf("%s###%d", actor->info->name, i);
			if (ImGui::Selectable(name, game->selectedActorId == actor->id)) {
				game->selectedActorId = actor->id;
			}
		}
		ImGui::EndChild();

		ImGui::Text("Create:");
		int buttonCount = 0;
		for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
			ActorTypeInfo *actorTypeInfo = &game->actorTypeInfos[i];
			if (!actorTypeInfo->canBeCreatedInEdtior) continue;
			buttonCount++;
			if (ImGui::Button(actorTypeInfo->name)) {
				Actor *actor = createActor(map, (ActorType)i);
				actor->position = game->cameraTarget;
				game->selectedActorId = actor->id;
			}
			if (buttonCount % 5 != 4) ImGui::SameLine();
		}
		ImGui::NewLine();

		ImGui::Separator();
		ImGui::Separator();

		Actor *actor = getActor(map, game->selectedActorId);
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

			ImGui::Text("Actor type: %s", actor->info->name);
			// ImGui::Combo("Actor type", (int *)&actor->type, actorTypeStrings, ArrayLength(actorTypeStrings));
			ImGui::InputText("Name", actor->name, ACTOR_NAME_MAX_LEN);
			ImGui::SameLine();
			ImGui::Text("Id: %d", actor->id);
			ImGui::DragFloat3("Position", &actor->position.x);
			ImGui::DragFloat3("Size", &actor->size.x);

			if (ImGui::Button("bringWithinBounds")) bringWithinBounds(map, actor);

			if (actor->type == ACTOR_UNIT) {
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

			if (actor->type == ACTOR_WALL || actor->type == ACTOR_MODEL) {
				ImGui::InputText("Model path", actor->modelPath, PATH_MAX_LEN);
				Model *model = getModel(frameSprintf("assets/models/%s", actor->modelPath));
				if (model) {
					ImGui::SameLine();
					if (ImGui::Button("Match size")) {
						actor->size = getSize(model) * globals->actorModelScale;
					}
				}
			}

			ImGui::Separator();
		}
		ImGui::End();

		bool selectedSomething = false;
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			Rect bounds = getBounds(getAABB(actor));

			if (platform->mouseJustUp && contains(bounds, game->mouse)) {
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

	Vec2 inputVec = v2();
	Vec2 movementVec = v2();
	bool jumpPressed, punchPressed, kickPressed;
	bool drinkPotionPressed, dashButtonPressed, armorGainButtonPressed, stasisGainButtonPressed, stashWeaponButtonPressed;
	bool pickupPressed;
	{ /// Update inputs
		jumpPressed = false;
		punchPressed = false;
		kickPressed = false;
		drinkPotionPressed = false;
		dashButtonPressed = false;
		armorGainButtonPressed = false;
		stasisGainButtonPressed = false;
		stashWeaponButtonPressed = false;
		pickupPressed = false;

		bool canInput = true;
		if (player->actionsNum > 0) canInput = false;
		if (inRoomPrewarm) canInput = false;
		if (game->inEditor) canInput = false;
		if (player->stamina <= 0) canInput = false;

		if (keyPressed('W') || keyPressed(KEY_UP)) inputVec.y++;
		if (keyPressed('S') || keyPressed(KEY_DOWN)) inputVec.y--;
		if (keyPressed('A') || keyPressed(KEY_LEFT)) inputVec.x--;
		if (keyPressed('D') || keyPressed(KEY_RIGHT)) inputVec.x++;
		Vec2 leftStick = joyLeftStick(0);
		inputVec.y += clampMap(leftStick.y, -0.2, -1, 0, 1); // This doesn't actually result in smooth movement
		inputVec.y -= clampMap(leftStick.y, 0.2, 1, 0, 1);
		inputVec.x += clampMap(leftStick.x, 0.2, 1, 0, 1);
		inputVec.x -= clampMap(leftStick.x, -0.2, -1, 0, 1);
		inputVec = inputVec.normalize();

		if (canInput) {
			movementVec = inputVec;
			if (keyJustPressed(' ') || joyButtonJustPressed(0, JOY_X)) jumpPressed = true;
			if (keyJustPressed('J') || joyButtonJustPressed(0, JOY_SQUARE)) punchPressed = true;
			if (keyJustPressed('K') || joyButtonJustPressed(0, JOY_TRIANGLE)) kickPressed = true;
			if (keyJustPressed('L') || joyButtonJustPressed(0, JOY_CIRCLE)) pickupPressed = true;
			if (game->alliancesControlled[0] && player->isOnGround && (keyJustPressed('Q') || joyButtonJustPressed(0, JOY_L1))) drinkPotionPressed = true;
			if (game->alliancesControlled[1] && player->isOnGround && (keyJustPressed('N') || joyButtonJustPressed(0, JOY_R1))) dashButtonPressed = true;
			if (game->alliancesControlled[2] && player->isOnGround && (keyJustPressed('B') || joyButtonJustPressed(0, JOY_L3))) armorGainButtonPressed = true;
			if (game->alliancesControlled[3] && player->isOnGround && (keyJustPressed('U') || joyButtonJustPressed(0, JOY_R2))) stasisGainButtonPressed = true;
			if (game->alliancesControlled[5] && player->isOnGround && (keyJustPressed('H') || joyButtonJustPressed(0, JOY_PAD_DOWN))) stashWeaponButtonPressed = true;
		}
	} ///

	for (int i = 0; i < map->actorsNum; i++) { /// Update actors
		Actor *actor = &map->actors[i];

		if (actor->prevIsOnGround != actor->isOnGround) { /// Landing on the ground
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
		} ///

		{ /// Update action
			if (actor->actionsNum > 0) {
				Action *action = &actor->actions[0];

				if (action->type != ACTION_BLOCKSTUN) actor->isBlocking = false;

				bool actionDone = false;
				bool canDoAction = true;

				if (action->time == 0) {
					float staminaCost = action->info->staminaUsage;
					if (action->type == ACTION_STASIS_GAIN) staminaCost = actor->maxStamina * STASIS_STAMINA_PERC;
					actor->stamina -= staminaCost;
					if (actor->stamina <= 0 && staminaCost) {
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

					for (int i = 0; i < action->info->thrusterTriggersNum; i++) {
						ThrusterTrigger *trigger = &action->info->thrusterTriggers[i];
						if (trigger->requiresHit && !action->didHitATarget) continue;
						if (trigger->frame == justHitFrame) applyThruster(actor, trigger->thruster, actor->facingLeft);
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
							if (game->debugDrawHitboxes) pushAABB(hitbox, 0xFFFF0000);
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
							if (otherActorActionType == ACTION_DASH_FORWARD) continue;

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

									action->didHitATarget = true;

									if (otherActor->hasHyperArmor) {
										playWorldSound("assets/audio/hyperArmorBreak.ogg", getCenter(otherActorAABB));
										otherActor->hasHyperArmor = false;
										continue;
									}

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

										Thruster blockThruster = action->info->blockThruster;
										if (isZero(blockThruster.accel)) {
											blockThruster = action->info->hitThruster;
											blockThruster.accel *= 0.25;
										}
										blockThruster.accel.z = 0;
										applyThruster(otherActor, blockThruster, actor->facingLeft);

										otherActor->actionsNum = 0;
										Action *newAction = addAction(otherActor, ACTION_BLOCKSTUN);
										newAction->customLength = action->info->blockstunFrames/60.0;
										if (newAction->customLength == 0) newAction->customLength = action->info->hitstunFrames/60.0;
									} else {
										particleColor = 0x80FF0000;
										particlesAmount = damage;

										int extraHitPauseFrames = 0;
										if (actor->playerControlled || otherActor->playerControlled) {
											extraHitPauseFrames += clampMap(action->info->damage/otherActor->maxHp, 0.1, 0.5, 10, 20);
										}

										{
											int hitSoundId = 0;
											if (damage >= 20) hitSoundId = playWorldSound("assets/audio/hit/4.ogg", getCenter(otherActorAABB));
											else if (damage >= 10) hitSoundId = playWorldSound("assets/audio/hit/3.ogg", getCenter(otherActorAABB));
											else if (damage >= 5) hitSoundId = playWorldSound("assets/audio/hit/2.ogg", getCenter(otherActorAABB));
											else hitSoundId = playWorldSound("assets/audio/hit/1.ogg", getCenter(otherActorAABB));

											Channel *channel = getChannel(hitSoundId);
											if (channel) {
												channel->delay = 1/60.0 * extraHitPauseFrames;
											}
										}
										game->hitPauseFrames += extraHitPauseFrames;
										otherActor->hp -= damage;

										if (actor->playerControlled) {
											game->extraStepsFromSleep = 0;
										}

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

										applyThruster(otherActor, action->info->hitThruster, actor->facingLeft);

										otherActor->actionsNum = 0;
										Action *newAction = addAction(otherActor, ACTION_HITSTUN);
										newAction->customLength = action->info->hitstunFrames/60.0;

										if (action->info->buffToGive != BUFF_NONE) addBuff(otherActor, action->info->buffToGive, action->info->buffToGiveTime);

										if (actor->hasStasisAttack) {
											actor->hasStasisAttack = false;
											otherActor->stasisTimeLeft = 3;
										}
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
				float actionTimeScale = getStatPoints(actor, STAT_ATTACK_SPEED)*0.1;
				if (usesCustomLength) {
					maxTime = action->customLength;
				} else {
					maxTime = (action->info->startupFrames + action->info->activeFrames + action->info->recoveryFrames) / 60.0;
				}

				if (action->type == ACTION_FORCED_MOVE) {
					if (game->debugDrawPathing) pushSphere(makeSphere(actor->position, 64), 0xFFFF0000);
					maxTime = 9999;

					Vec3 dir = normalize(action->targetPosition - actor->position);

					float baseSpeed = getStatPoints(actor, STAT_MOVEMENT_SPEED) * 0.2;
					Vec2 speed = v2(baseSpeed, baseSpeed);
					actor->movementAccel.x += dir.x * speed.x;
					actor->movementAccel.y += dir.y * speed.y;

					Vec3 targetPosition = action->targetPosition;
					if (game->debugDrawPathing) pushSphere(makeSphere(action->targetPosition, 64), 0xFFFF0000);

					AABB destAABB = getAABBAtPosition(actor, targetPosition);
					destAABB = bringWithinBounds(map, destAABB);
					targetPosition = getCenter(destAABB) + getSize(destAABB)*v3(0, 0, -0.5);
					if (game->debugDrawPathing) pushSphere(makeSphere(action->targetPosition, 64), 0xFFFF0000);

					if (distance(actor->position, targetPosition) < 20) actionDone = true;
				}

				if (action->type == ACTION_FORCED_LEAVE) {
					maxTime = 1;
				}

				if (action->type == ACTION_DASH_FORWARD) {
					if (actor->doingSpotDodge) {
						actionTimeScale *= 0.3;
					} else {
						float perc = action->time / maxTime;
						if (perc < 0.5) {
							Vec3 velo = v3(15, 0, 0);
							if (actor->facingLeft) velo.x *= -1;
							actor->velo += velo;
						}
					}
				}

				if (action->time >= maxTime) actionDone = true;
				if (actionDone) {
					if (actor->pastActionsNum > ACTIONS_MAX-1) {
						memmove(&actor->pastActions[0], &actor->pastActions[1], sizeof(Action) * ACTIONS_MAX-1);
						actor->pastActionsNum--;
					}
					actor->pastActions[actor->pastActionsNum++] = actor->actions[0];

					if (action->type == ACTION_UPPERCUT) {
						actor->pastActionsNum = 0;
					} else if (action->type == ACTION_KNOCKDOWN) {
						addAction(actor, ACTION_RAISING);
					} else if (action->type == ACTION_FORCED_LEAVE) {
						actor->markedForDeletion = true;
					} else if (action->type == ACTION_START_PICKUP) {
						Actor *itemActor = getActor(map, action->relatedActorId);
						if (itemActor) {
							initItem(&actor->heldItem, itemActor->itemType, 1);
							itemActor->markedForDeletion = true;
						}
					} else if (action->type == ACTION_THROW || action->type == ACTION_AIR_THROW) {
						Actor *itemActor = createActor(map, ACTOR_ITEM);
						itemActor->itemType = actor->heldItem.type;
						itemActor->position = actor->position;
						itemActor->position.z += actor->size.z * 0.75;
						itemActor->velo.z = 1;
						if (actor->facingLeft) {
							itemActor->velo.x = -40;
						} else {
							itemActor->velo.x = 40;
						}
						actor->heldItem.type = ITEM_NONE;
					} else if (action->type == ACTION_TOSS) {
						Actor *itemActor = createActor(map, ACTOR_ITEM);
						itemActor->itemType = actor->heldItem.type;
						itemActor->position = actor->position;
						itemActor->position.z += actor->size.z * 0.75;
						itemActor->velo.z = 5;
						if (actor->facingLeft) {
							itemActor->velo.x = 5;
						} else {
							itemActor->velo.x = -5;
						}
						actor->heldItem.type = ITEM_NONE;
					} else if (action->type == ACTION_DRINK_POTION_START) {
						if (actor->potionsLeft > 0) {
							actor->potionsLeft--;
							actor->hp += actor->maxHp*0.5;
							addAction(actor, ACTION_DRINK_POTION_FINISH);
						} else {
							addAction(actor, ACTION_DRINK_POTION_FAIL);
						}
					} else if (action->type == ACTION_DASH_START) {
						if (actor->playerControlled) {
							if (fabs(inputVec.x) > 0.75) {
								actor->facingLeft = false;
								if (inputVec.x < 0) actor->facingLeft = true;
								actor->doingSpotDodge = false;
							} else {
								actor->doingSpotDodge = true;
							}
							addAction(actor, ACTION_DASH_FORWARD);
						}
					} else if (action->type == ACTION_STASIS_GAIN) {
						actor->hasStasisAttack = true;
						Effect *effect = createEffect(EFFECT_STASIS_GAIN, v3());
						effect->actorId = actor->id;
						effect->pulses = 10;
					} else if (action->type == ACTION_STASH_ON_BACK) {
						Item temp = actor->backItem;
						actor->backItem = actor->heldItem;
						actor->heldItem = temp;
					} else if (action->type == ACTION_STASH_ON_HIP) {
						Item temp = actor->hipItem;
						actor->hipItem = actor->heldItem;
						actor->heldItem = temp;
					}

					arraySpliceIndex(actor->actions, actor->actionsNum, sizeof(Action), 0);
					actor->actionsNum--;
				}

				if (actor->hasStasisAttack) actionTimeScale *= 2;

				if (action->time < activeMin && action->time + elapsed*actionTimeScale > activeMax) actionTimeScale = 1;

				if (usesCustomLength) actionTimeScale = 1;
				if (action->type == ACTION_RAISING) actionTimeScale = 1;

				if (actor->stasisTimeLeft > 0) actionTimeScale *= STASIS_TIME_SCALE_AMOUNT;

				action->prevTime = action->time;
				action->time += elapsed * actionTimeScale;

				actor->timeWithoutAction = 0;
			} else { 
				actor->timeWithoutAction += elapsed;
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
			if (actor->stasisTimeLeft > 0) speed *= STASIS_TIME_SCALE_AMOUNT;

			actor->isBlocking = true;
			if (actor->stamina <= BLOCKING_STAMINA_THRESHOLD) actor->isBlocking = false;
			if (!actor->isOnGround) actor->isBlocking = false;

			if (actor->timeMoving > 0 || actor->timeWithoutAction > 0.2) actor->pastActionsNum = 0;

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
				if (!game->inEditor) {
					if (actor->isOnGround) {
						game->cameraTarget = actor->position;
						game->cameraTarget.z += platform->windowHeight*0.15;
					} else {
						if (fabs(actor->position.z - game->cameraTarget.z) < game->size.y*0.5) {
							game->cameraTarget.x = actor->position.x;
							game->cameraTarget.y = actor->position.y;
						} else {
							game->cameraTarget = actor->position;
							game->cameraTarget.z += platform->windowHeight*0.15;
						}
					}
				}

				{ // Figure out running
					if (movementVec.x < -0.5) {
						if (actor->prevMovementVec.x >= -0.5) {
							if (actor->timeSinceLastLeftPress < 0.25) {
								actor->isRunningLeft = true;
							}
							actor->timeSinceLastLeftPress = 0;
						}
					} else {
						actor->isRunningLeft = false;
					}

					if (movementVec.x > 0.5) {
						if (actor->prevMovementVec.x <= 0.5) {
							if (actor->timeSinceLastRightPress < 0.25) {
								actor->isRunningRight = true;
							}
							actor->timeSinceLastRightPress = 0;
						}
					} else {
						actor->isRunningRight = false;
					}
				}

				actor->movementAccel.x += movementVec.x * speed.x;
				actor->movementAccel.y += movementVec.y * speed.y;

				actor->prevMovementVec = movementVec;

				if (jumpPressed && actor->isOnGround) actor->velo.z += 20;

				if (punchPressed) {
					if (actor->isOnGround) {
						if (actor->heldItem.type == ITEM_NONE) {
							if (actor->isRunningLeft || actor->isRunningRight) {
								addAction(actor, ACTION_RUNNING_PUNCH);
							} else if (actor->pastActionsNum == 1 && actor->pastActions[0].type == ACTION_PUNCH) {
								addAction(actor, ACTION_PUNCH_2);
							} else if (actor->pastActionsNum == 2 && actor->pastActions[0].type == ACTION_PUNCH && actor->pastActions[1].type == ACTION_PUNCH_2) {
								addAction(actor, ACTION_UPPERCUT);
							} else {
								addAction(actor, ACTION_PUNCH);
							}
						} else if (actor->heldItem.type == ITEM_SWORD) {
							addAction(actor, ACTION_ATTACK1_SWORD);
						} else if (actor->heldItem.type == ITEM_KNIFE) {
							addAction(actor, ACTION_ATTACK1_KNIFE);
						}
					} else {
						if (actor->heldItem.type == ITEM_NONE) {
							addAction(actor, ACTION_AIR_PUNCH);
						} else if (actor->heldItem.type == ITEM_SWORD) {
							addAction(actor, ACTION_AIR_ATTACK1_SWORD);
						} else if (actor->heldItem.type == ITEM_KNIFE) {
							addAction(actor, ACTION_AIR_ATTACK1_KNIFE);
						}
					}
				}

				if (kickPressed) {
					if (actor->isOnGround) {
						if (actor->heldItem.type == ITEM_NONE) {
							bool didAttack = false;

							if (actor->isRunningLeft || actor->isRunningRight) {
								addAction(actor, ACTION_RUNNING_KICK);
								didAttack = true;
							}

							if (!didAttack) addAction(actor, ACTION_KICK);
						} else if (actor->heldItem.type == ITEM_SWORD) {
							addAction(actor, ACTION_THROW);
						} else if (actor->heldItem.type == ITEM_KNIFE) {
							addAction(actor, ACTION_THROW);
						}
					} else {
						if (actor->heldItem.type == ITEM_NONE) {
							addAction(actor, ACTION_AIR_KICK);
						} else if (actor->heldItem.type == ITEM_SWORD) {
							addAction(actor, ACTION_AIR_THROW);
						} else if (actor->heldItem.type == ITEM_KNIFE) {
							addAction(actor, ACTION_AIR_THROW);
						}
					}
				}

				if (pickupPressed) {
					// Maybe pickup logic should go here...
					if (actor->heldItem.type != ITEM_NONE) {
						addAction(actor, ACTION_TOSS);
					}
				}

				if (drinkPotionPressed) {
					addAction(actor, ACTION_DRINK_POTION_START);
				}

				if (dashButtonPressed) {
					addAction(actor, ACTION_DASH_START);
				}

				if (armorGainButtonPressed) {
					actor->hasHyperArmor = true;
					addAction(actor, ACTION_ARMOR_GAIN);
				}

				if (stasisGainButtonPressed) {
					if (actor->stamina / actor->maxStamina > STASIS_STAMINA_PERC) {
						addAction(actor, ACTION_STASIS_GAIN);
					} else {
						infof("Not enough stamina\n");
					}
				}

				if (stashWeaponButtonPressed) {
					auto doesItemTypeGoOnBack = [](ItemType type) {
						if (type == ITEM_SWORD) return true;
						if (type == ITEM_KNIFE) return false;
						return false;
					};

					ItemType typeToSwapFor = actor->backItem.type;
					if (typeToSwapFor == ITEM_NONE) typeToSwapFor = actor->hipItem.type;

					if (actor->heldItem.type != ITEM_NONE) {
						if (doesItemTypeGoOnBack(actor->heldItem.type)) {
							addAction(actor, ACTION_STASH_ON_BACK);
						} else { 
							addAction(actor, ACTION_STASH_ON_HIP);
						}

						if (typeToSwapFor != ITEM_NONE && doesItemTypeGoOnBack(actor->heldItem.type) != doesItemTypeGoOnBack(typeToSwapFor)) {
							if (doesItemTypeGoOnBack(typeToSwapFor)) {
								addAction(actor, ACTION_STASH_ON_BACK);
							} else { 
								addAction(actor, ACTION_STASH_ON_HIP);
							}
						}
					} else if (typeToSwapFor != ITEM_NONE) {
						if (doesItemTypeGoOnBack(typeToSwapFor)) {
							addAction(actor, ACTION_STASH_ON_BACK);
						} else { 
							addAction(actor, ACTION_STASH_ON_HIP);
						}
					}
				}

				actor->timeSinceLastLeftPress += elapsed;
				actor->timeSinceLastRightPress += elapsed;
			} else {
				{ /// Do enemy AI @todo factor this out stepAI()
					float aggression = 0.5;

					Sphere visionSphere;
					{ /// Vision
						visionSphere.position = actor->position + v3(0, 0, actor->size.z);
						visionSphere.position.x += actor->facingLeft ? -400 : 400;
						visionSphere.radius = 500;
					} ///

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

						bool canBePassivelyAggroed = false;
						if (!getActor(map, actor->aiTarget)) {
							if (actor->actionsNum == 0) canBePassivelyAggroed = true;
							if (actor->actionsNum == 1 && (
									actor->actions[0].type == ACTION_FORCED_MOVE ||
									actor->actions[0].type == ACTION_FORCED_IDLE)
							) canBePassivelyAggroed = true;
						}

						if (canBePassivelyAggroed) {
							if (game->debugDrawVision) pushSphere(visionSphere, 0xFFFF0000);
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
									if (!overlaps(otherActor, visionSphere)) continue;

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

						if (actor->actionsNum == 0) {
							Actor *target = getActor(map, actor->aiTarget);

							if (!target) actor->aiState = AI_IDLE;

							if (actor->prevAiState != actor->aiState) {
								actor->prevAiState = actor->aiState;
								actor->aiStateTime = 0;
							}

							Vec2 position2 = v2(actor->position);
							Vec2 targetPosition2 = v2();
							if (target) targetPosition2 = v2(target->position);

							float standAwayMin = 200;
							float standAwayMax = 300;
							float yVariance = 300;
							float stepBackMulti = 1;
							float maxDistTillChase = (standAwayMax + yVariance/2) * 1.25;

							if (actor->team == 1) {
								standAwayMin = 400;
								standAwayMax = 500;
							}

							if (actor->aiState == AI_IDLE) {
								if (target) actor->aiState = AI_STAND_NEAR_TARGET;

								if (actor->aiStateTime > 1 && actor->timeNotMoving > 2) { /// Make idle choice
									int alliedUnits = 0;
									int totalUnits = 0;
									for (int i = 0; i < map->actorsNum; i++) {
										Actor *otherActor = &map->actors[i];
										if (otherActor->type != ACTOR_UNIT) continue;
										totalUnits++;
										if (actor->team == otherActor->team) alliedUnits++;
									}
									float currentTeamPerc = (float)alliedUnits / (float)totalUnits;
									float extraPerc = currentTeamPerc - map->alliances[actor->team];

									bool shouldLeave = false;
									if (extraPerc > 0.01 && totalUnits > 2 && rndPerc(0.3)) shouldLeave = true;

									if (shouldLeave) {
										Actor **possilbeDoor = (Actor **)frameMalloc(sizeof(Actor *) * ACTORS_MAX);
										int possibleDoorNum = 0;
										for (int i = 0; i < map->actorsNum; i++) {
											Actor *door = &map->actors[i];
											if (door->type == ACTOR_DOOR && door->destMapStoreType == STORE_NONE) possilbeDoor[possibleDoorNum++] = door;
										}

										if (possibleDoorNum > 0) {
											Actor *door = possilbeDoor[rndInt(0, possibleDoorNum-1)];

											Action *action = addAction(actor, ACTION_FORCED_MOVE);
											action->targetPosition = door->position;

											addAction(actor, ACTION_FORCED_LEAVE);
										}

									} else {
										for (int i = 0; ; i++) {
											Vec3 newPos;
											newPos.x = rndFloat(groundAABB.min.x, groundAABB.max.x);
											newPos.y = rndFloat(groundAABB.min.y, groundAABB.max.y);
											newPos.z = groundAABB.max.z + 1; // This should probably be a ray cast eventually
											AABB newAABB = getAABBAtPosition(actor, newPos);

											bool canStand = true;
											if (!equal(newAABB, bringWithinBounds(map, newAABB))) canStand = false;
											for (int i = 0; i < map->actorsNum; i++) {
												Actor *otherActor = &map->actors[i];
												if (!otherActor->info->isWall) continue;

												if (overlaps(otherActor, newAABB)) {
													canStand = false;
													break;
												}
											}

											if (canStand) {
												Action *action = addAction(actor, ACTION_FORCED_MOVE);
												action->targetPosition = newPos;
												break;
											}

											if (i >= 100) {
												logf("Failed to find ai walk location\n");
												break;
											}
										}
									}
								} ///
							} else if (actor->aiState == AI_STAND_NEAR_TARGET) {

								if (actor->aiStateTime == 0) {
									float xDir = actor->position.x < target->position.x ? -1 : 1;
									Vec2 targetPositionOffset;
									targetPositionOffset.x = xDir * rndFloat(standAwayMin, standAwayMax);
									targetPositionOffset.y = rndFloat(-yVariance/2, yVariance/2);

									actor->aiStateLength = rndFloat(2, 4);

									actor->aiTargetPosition2 = targetPosition2 + targetPositionOffset;

									Vec2 position = actor->aiTargetPosition2;
									Vec3 size = actor->size;
									{ // bringActorPoisitionWithinBounds
										if (position.x < groundAABB.min.x + size.x/2) position.x = groundAABB.min.x + size.x/2;
										if (position.x > groundAABB.max.x - size.x/2) position.x = groundAABB.max.x - size.x/2;
										if (position.y < groundAABB.min.y + size.y/2) position.y = groundAABB.min.y + size.y/2;
										if (position.y > groundAABB.max.y - size.y/2) position.y = groundAABB.max.y - size.y/2;
									}

									actor->aiTargetPosition2 = position;
								}

								float dist = distance(actor->aiTargetPosition2, position2);
								if (distance(actor->aiTargetPosition2, position2) > 10) {
									float speedMulti = clampMap(dist, 50, 100, 0.2, 1);

									Vec2 dir = normalize(actor->aiTargetPosition2 - position2);
									actor->movementAccel += v3(dir * speed.x*speedMulti);
								} else {
									actor->aiState = AI_STAND_AT_ATTENTION;
								}
							} else if (actor->aiState == AI_STAND_AT_ATTENTION) {
								if (actor->aiStateTime == 0) {
									actor->aiStateLength = rndFloat(0.1, 6);

									bool shouldOpenWithDash = false;
									if (
										actor->team == 1 &&
										rndPerc(0.01) &&
										actor->stamina >= actor->maxStamina*0.3 &&
										fabs((targetPosition2.y - position2.y)) < 100 &&
										fabs((targetPosition2.x - position2.x)) > (standAwayMin+standAwayMax)/2
									) shouldOpenWithDash = true;
									if (shouldOpenWithDash) {
										addAction(actor, ACTION_DASH_START);
										addAction(actor, ACTION_DASH_FORWARD);
										actor->aiState = AI_APPROACH_FOR_ATTACK;
									}

									bool shouldOpenWithArmorGain = false;
									if (
										actor->team == 2 &&
										rndPerc(0.01) &&
										!actor->hasHyperArmor &&
										actor->stamina >= globals->actionTypeInfos[ACTION_ARMOR_GAIN].staminaUsage &&
										actor->stamina >= actor->maxStamina*0.3
									) shouldOpenWithArmorGain = true;
									if (shouldOpenWithArmorGain) {
										actor->hasHyperArmor = true;
										addAction(actor, ACTION_ARMOR_GAIN);
									}

									bool shouldOpenWithStasisGain = false;
									if (
										actor->team == 3 &&
										rndPerc(0.01) &&
										!actor->hasStasisAttack &&
										actor->stamina >= actor->maxStamina * STASIS_STAMINA_PERC
									) shouldOpenWithStasisGain = true;
									if (shouldOpenWithStasisGain) {
										addAction(actor, ACTION_STASIS_GAIN);
									}
								}

								float dist = distance(position2, targetPosition2);
								float tooFarDist = dist - standAwayMax;

								if (tooFarDist > 300) actor->aiState = AI_STAND_NEAR_TARGET;

#if 0 // Backstepping causes no blocking...
								float tooCloseDist = dist - standAwayMax;
								if (tooCloseDist < -standAwayMin*0.2) {
									float speedMulti = clampMap(tooCloseDist, -standAwayMin*0.2, -standAwayMin*0.8, 0.2, 0.9);

									Vec2 dir = normalize(targetPosition2 - position2);
									actor->movementAccel += v3(dir * -speed.x*speedMulti);
								}
#endif

								if (actor->aiStateTime > actor->aiStateLength && !game->debugAiNeverApproaches) {
									if (rndPerc(aggression)) {
										actor->aiState = AI_APPROACH_FOR_ATTACK;
									} else {
										actor->prevAiState = AI_IDLE; // Reset ai state
									}
								}
							} else if (actor->aiState == AI_APPROACH_FOR_ATTACK) {
								Vec2 dir = (targetPosition2 - position2).normalize();
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
										addAction(actor, ACTION_PUNCH_2);
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
					}
				} ///
			}

			{ /// Update facing
				bool canTurn = false;
				float turnSpeed = 0.3;

				if (actor->actionsNum == 0) {
					canTurn = true;
				} else {
					if (actor->actions[0].type == ACTION_FORCED_MOVE) canTurn = true;
					if (actor->actions[0].type == ACTION_RAISING) {
						turnSpeed *= 0.5;
						canTurn = true;
					}
				}

				if (canTurn) {
					if (actor->playerControlled) {
						if (actor->movementAccel.x < -0.1) actor->facingLeft = true;
						if (actor->movementAccel.x > 0.1) actor->facingLeft = false;
					} else {
						Actor *target = getActor(map, actor->aiTarget);
						if (target) {
							Vec3 dir = target->position - actor->position;
							actor->facingLeft = dir.x <= 0;
						} else {
							if (actor->movementAccel.x < -0.1) actor->facingLeft = true;
							if (actor->movementAccel.x > 0.1) actor->facingLeft = false;
						}
					}
				}

				float targetRads = M_PI/2 - 0.01;
				if (actor->facingLeft) targetRads *= -1;

				if (actor->actionsNum > 0) turnSpeed *= 1.25;

				float timeScale = elapsed / (1/60.0);
				turnSpeed *= timeScale;

				actor->facingAngle = lerpRad(actor->facingAngle, targetRads, turnSpeed);
			} ///

			{ /// Update items
				for (int i = 0; i < actor->itemsNum; i++) {
					Item *item = &actor->items[i];
					if (item->info->slotType == ITEM_SLOT_CONSUMABLE) {
						for (int i = 0; i < STATS_MAX; i++) {
							actor->stats[i] += item->info->statsToGive[i];
							actor->level += item->info->statsToGive[i];
						}
						actor->hp += item->info->extraHpFromConsume;
						removeItem(actor, item->type, 1);
						continue; // This removal is not bulletproof // (not frame perfect)
					}
				}
			} ///

			bool doRefill = false;
			if (actor->maxHp == 0) doRefill = true;
			actor->maxHp = getStatPoints(actor, STAT_HP) * 10;
			actor->maxStamina = getStatPoints(actor, STAT_MAX_STAMINA) * 10;
			if (doRefill) {
				actor->hp = actor->maxHp;
				actor->stamina = actor->maxStamina;
			}

			{ /// Set animation
				auto getMarkerFrame = [](char *animName, char *markerName)->float {
					for (int i = 0; i < game->animationMarkerDataNum; i++) {
						AnimationMarkerData *marker = &game->animationMarkerData[i];
						if (streq(marker->animName, animName) && streq(marker->markerName, markerName)) return marker->perc;
					}

					return 0;
				};

				bool usesFrameData = false;
				if (actor->actionsNum > 0) {
					usesFrameData = true;
					Action *action = &actor->actions[0];
					if (action->type == ACTION_FORCED_MOVE) usesFrameData = false;
					if (action->type == ACTION_FORCED_IDLE) usesFrameData = false;
					if (action->type == ACTION_FORCED_LEAVE) usesFrameData = false;
				}

				auto getActorAnimation = [](Actor *actor, char *name) {
					char *animSuffix = "";
					if (actor->heldItem.type == ITEM_SWORD) animSuffix = "_sword";
					if (actor->heldItem.type == ITEM_KNIFE) animSuffix = "_knife";

					if (!stringEndsWith(name, animSuffix)) name = frameSprintf("%s%s", name, animSuffix);

					Animation *anim = getAnimationOrEmpty(name);
					return anim;
				};

				char *animName = NULL;
				float animTime = 0;
				float animPercOverride = -1;
				bool animLoops = true;

				if (usesFrameData) {
					Action *action = &actor->actions[0];
					animName = action->info->animationName;
					animTime = action->time;
					animLoops = action->info->animationLoops;

					float actionStartupEndTime = action->info->startupFrames / 60.0;
					float actionActiveEndTime = actionStartupEndTime + (action->info->activeFrames / 60.0);
					float actionRecoveryEndTime = actionActiveEndTime + (action->info->recoveryFrames / 60.0);

					float animationStartupEndPerc = getMarkerFrame(action->info->animationName, "active");
					float animationActiveEndPerc = getMarkerFrame(action->info->animationName, "recovery");

					if (animationStartupEndPerc != 0 || animationActiveEndPerc != 0) {
						if (action->time < actionStartupEndTime) {
							animPercOverride = clampMap(action->time, 0, actionStartupEndTime, 0, animationStartupEndPerc);
						} else if (action->time < actionActiveEndTime) { 
							animPercOverride = clampMap(action->time, actionStartupEndTime, actionActiveEndTime, animationStartupEndPerc, animationActiveEndPerc);
						} else {
							animPercOverride = clampMap(action->time, actionActiveEndTime, actionRecoveryEndTime, animationActiveEndPerc, 1);
						}
					} else {
						float maxTime = (action->info->startupFrames + action->info->activeFrames + action->info->recoveryFrames)/60.0;
						if (maxTime) {
							animPercOverride = clampMap(action->time, 0, maxTime, 0, 1);
						} else {
							animTime = action->time;
						}
					}
				} else {
					if (actor->timeMoving) {
						if (actor->isRunningLeft || actor->isRunningRight) {
							animName = "run";
							animPercOverride = actor->movementPerc;
						} else {
							animName = "walk";
							animPercOverride = actor->movementPerc;
						}
					} else if (actor->timeInAir) {
						animName = "jump";
						animTime = actor->timeInAir * FRAME_SUBSTEPS * 2; // * 2 because it's slow...
						animLoops = false;
					} else {
						animName = "idle";
						animTime = actor->timeNotMoving * FRAME_SUBSTEPS;
					}
				}

				AABB aabb = getAABB(actor);
				if (actor->actionsNum > 0) {
					Action *action = &actor->actions[0];
					if (action->type == ACTION_BLOCKSTUN) { // Vibration
						float amount = clampMap(action->time, 0, action->customLength, 5, 0, QUAD_IN);
						if (platform->frameCount % 2) {
							aabb += v3(amount, 0, 0);
						} else {
							aabb -= v3(amount, 0, 0);
						}
					}
				}

				{ /// 3d
					actor->modelMatrix = mat4();
					actor->modelMatrix.TRANSLATE(getCenter(aabb) - v3(0, 0, getSize(aabb).z/2));
					actor->modelMatrix.TRANSLATE(globals->actorModelOffset);
					actor->modelMatrix.ROTATE_EULER(0, 0, actor->facingAngle);
					actor->modelMatrix.SCALE(globals->actorModelScale);

					char *altAnimName = NULL;
					if (actor->heldItem.type == ITEM_SWORD) altAnimName = frameSprintf("%s_sword", animName);
					if (actor->heldItem.type == ITEM_KNIFE) altAnimName = frameSprintf("%s_knife", animName);

					SkeletonAnimation *mainAnim = NULL;
					mainAnim = getAnimation(actor->skeleton, altAnimName);
					if (!mainAnim) mainAnim = getAnimation(actor->skeleton, animName);

					if (!mainAnim) {
						mainAnim = getAnimation(actor->skeleton, "idle");
						logf("Anim '%s'/'%s' not found\n", altAnimName, animName);
						pushAABB(aabb, teamColors[actor->team]);
					}

					{ // Blend trees
						Skeleton *skeleton = actor->skeleton;
						SkeletonBlend *mainBlend = getSkeletonBlend(actor->skeleton, "main");
						SkeletonBlend *nextBlend = getSkeletonBlend(actor->skeleton, "next");
						SkeletonBlend *fadeOutBlend = getSkeletonBlend(actor->skeleton, "fadeOut");
						float skeletonBlendTimeScale = elapsed / (1/60.0);

						SkeletonBlend *currentBlend = NULL;

						float weightChange = 0.1 * skeletonBlendTimeScale;

						nextBlend->weight = 0;

						currentBlend = mainBlend;
						if (mainBlend->animation != mainAnim) {
							for (int i = 0; i < skeleton->base->bonesNum; i++) {
								fadeOutBlend->poseXforms[i] = skeleton->currentPoseXforms[i];
								fadeOutBlend->controlMask[i] = 1;
							}
							fadeOutBlend->weight = 1;
						}

						fadeOutBlend->weight = Clamp01(fadeOutBlend->weight - weightChange);
						mainBlend->weight = 1 - fadeOutBlend->weight;

						currentBlend->animation = mainAnim;
						currentBlend->loops = animLoops;
						currentBlend->playing = false;
						currentBlend->time = animTime;
						if (animPercOverride != -1) {
							currentBlend->time = animPercOverride * (currentBlend->animation->frameCount / currentBlend->animation->frameRate);
						}
					}

					updateSkeleton(actor->skeleton, elapsed);
					if (game->debugDrawUnitModels) {
						int color = teamColors[actor->team];
						if (actor->stasisTimeLeft > 0) color = lerpColor(color, 0xFF0000FF, 0.5);
						pushModel(getModel("assets/models/unit.model"), actor->modelMatrix, actor->skeleton, color);

						Matrix4 heldItemMatrix = getBoneMatrix(actor, "weapon.r");
						Matrix4 backItemMatrix = getBoneMatrix(actor, "weaponBack");
						Matrix4 hipItemMatrix = getBoneMatrix(actor, "weaponBelt");

						if (actor->heldItem.type == ITEM_SWORD) {
							pushModel(getModel("assets/models/sword.model"), heldItemMatrix);
						} else if (actor->heldItem.type == ITEM_KNIFE) {
							pushModel(getModel("assets/models/knife.model"), heldItemMatrix);
						}

						if (actor->backItem.type == ITEM_SWORD) {
							pushModel(getModel("assets/models/sword.model"), backItemMatrix);
						} else if (actor->backItem.type == ITEM_KNIFE) {
							pushModel(getModel("assets/models/knife.model"), backItemMatrix);
						}

						if (actor->hipItem.type == ITEM_SWORD) {
							pushModel(getModel("assets/models/sword.model"), hipItemMatrix);
						} else if (actor->hipItem.type == ITEM_KNIFE) {
							pushModel(getModel("assets/models/knife.model"), hipItemMatrix);
						}
					}
				} ///

				if (game->debugDrawUnitBoxes) pushAABB(aabb, teamColors[actor->team]);
			} ///

			{ /// Draw overlay
				AABB aabb = getAABB(actor);
				Vec2 positionTop2 = getScreenPoint(actor->position + v3(0, 0, actor->size.z));

				bool hasHpBar = true;
				bool hasStaminaBar = true;

				Rect hpBgRect;
				if (hasHpBar) {
					hpBgRect = makeRect(0, 0, actor->size.x, 10);
					hpBgRect.x = positionTop2.x - hpBgRect.width/2;
					hpBgRect.y = positionTop2.y - hpBgRect.height/2;
					pushScreenRect(hpBgRect, 0xFFFF0000);

					Rect rect = hpBgRect;
					rect.width *= actor->hp/actor->maxHp;

					int color = 0xFF44AA44;
					if (actor->playerControlled) color = 0xFFF5EE2A;
					pushScreenRect(rect, color);
				}

				if (hasStaminaBar) {
					Rect staminaBgRect = hpBgRect;
					staminaBgRect.y += hpBgRect.height;
					pushScreenRect(staminaBgRect, 0xFF000070);

					Rect rect = staminaBgRect;
					rect.width *= fabs(actor->stamina/actor->maxStamina);
					pushScreenRect(rect, actor->stamina < 0 ? 0xFFF00000 : 0xFF0000F0);
				}

				if (game->debugDrawActorTargets) {
					Actor *target = getActor(map, actor->aiTarget);
					if (target) {
						Vec2 positionCenter2 = getScreenPoint(getCenter(aabb));

						AABB targetAABB = getAABB(target);
						Vec2 targetPositionCenter2 = getScreenPoint(getCenter(targetAABB));
						pushScreenLine(positionCenter2, targetPositionCenter2, 4, 0xFFFF0000);
					}
				}

				{ /// Text lines
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

					if (game->debugDrawUnitCombo && actor->type == ACTOR_UNIT) {
						for (int i = 0; i < actor->pastActionsNum; i++) {
							Action *action = &player->pastActions[i];
							textLines[textLinesNum++] = frameSprintf("%d: %s", i, action->info->name);
						}
					}

					if (game->debugDrawUnitAiState && actor->type == ACTOR_UNIT && !actor->playerControlled) {
						textLines[textLinesNum++] = frameSprintf("%s (%.1fs)", aiStateStrings[actor->aiState], actor->aiStateTime);
					}

					Vec2 positionTop2 = getScreenPoint(actor->position + v3(0, 0, actor->size.z));
					Vec2 cursor = positionTop2;
					for (int i = 0; i < textLinesNum; i++) {
						Vec2 size = getTextSize(game->defaultFont, textLines[i]);
						Vec2 position;
						position.x = cursor.x - size.x/2;
						position.y = cursor.y - size.y;

						Rect rect = inflate(makeRect(position, size), 5);
						pushScreenRect(rect, 0x80000000);
						DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFC0C0C0, position);
						pushScreenText(textLines[i], props);

						cursor.y -= size.y;
					}

					if (game->debugDrawActorStatsSimple) {
						char *statTextStrs[STATS_MAX];
						Vec2 statTextSizes[STATS_MAX];
						Vec2 size = v2();
						for (int i = 0; i < STATS_MAX; i++) {
							statTextStrs[i] = frameSprintf("%.0f", getStatPoints(actor, (StatType)i));
							statTextSizes[i] = getTextSize(game->simpleStatsFont, statTextStrs[i]);
							size.x += statTextSizes[i].x;
							size.y = MaxNum(size.y, statTextSizes[i].y);
						}
						Vec2 position = positionTop2;
						position.x -= size.x/2;
						position.y -= size.y;

						for (int i = 0; i < STATS_MAX; i++) {
							DrawTextProps props = newDrawTextProps(game->simpleStatsFont, statTypeColors[i], position);
							pushScreenText(statTextStrs[i], props);
							position.x += statTextSizes[i].x;
						}

						{
							Vec2 center = positionTop2;
							center.y -= 30;
							Rect levelRect = makeCenteredSquare(center, 32);
							DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
							pushScreenTextInRect(frameSprintf("%d", actor->level), props, levelRect);
						}
					}
				} ///
			} ///

			if (game->debugNeverLoseStamina && actor->playerControlled) actor->stamina = actor->maxStamina;
		} else if (actor->type == ACTOR_GROUND) {
			pushAABB(getAABB(actor), 0xFFE8C572);
		} else if (actor->type == ACTOR_WALL) {
			pushAABB(getAABB(actor), 0xFFFFFFFF);
		} else if (actor->type == ACTOR_MODEL) {
			Model *model = getModel(frameSprintf("assets/models/%s", actor->modelPath));
			if (model) {
				Matrix4 matrix = mat4();
				matrix.TRANSLATE(actor->position);
				matrix.SCALE(1/getSize(model->bounds));
				matrix.SCALE(actor->size);
				pushModel(model, matrix);
			} else {
				pushAABB(getAABB(actor), 0xFFFFFFFF);
			}
		} else if (actor->type == ACTOR_BED) {
			pushAABB(getAABB(actor), 0xFFA3FFF6);
			if (overlaps(actor, player)) {
				if (game->lastStepOfFrame) {
					int hoursChosen = 0;
					nguiStartWindow("SleepForWindow");

					if (nguiButton("8 hours")) hoursChosen = 8;
					if (nguiButton("4 hours")) hoursChosen = 4;
					if (nguiButton("2 hours")) hoursChosen = 2;
					if (nguiButton("1 hours")) hoursChosen = 1;

					if (hoursChosen) {
						float ticksPerSec = 1.0/SECS_PER_CITY_TICK;
						game->extraStepsFromSleep = 60 * 60 * 60 * hoursChosen;
						player->potionsLeft = 3;
					}

					nguiEndWindow();
				}
			}
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

			if (overlaps(actor, player)) {
				if (actor->locked < 0.9 && !actor->doorPlayerSpawnedOver && game->currentMapIndex == game->nextMapIndex) {
					char *nextMapName = actor->destMapName;
					if (streq(nextMapName, "!lastMap")) nextMapName = game->lastMapName;

					for (int i = 0; i < MAPS_MAX; i++) {
						Map *possibleMap = &game->maps[i];
						if (streq(possibleMap->name, nextMapName)) {
							game->nextMapIndex = i;

							game->currentStoreData = NULL;
							if (actor->destMapStoreType != STORE_NONE) {
								for (int i = 0; i < game->storeDatasNum; i++) {
									StoreData *data = &game->storeDatas[i];
									if (data->relatedMapIndex == game->currentMapIndex && data->relatedActorId == actor->id) {
										game->currentStoreData = data;
										break;
									}
								}
								if (!game->currentStoreData) {
									if (game->storeDatasNum > STORE_DATAS_MAX-1) {
										logf("Too many store datas!\n");
										game->storeDatasNum--;
									}
									StoreData *data = &game->storeDatas[game->storeDatasNum++];
									memset(data, 0, sizeof(StoreData));
									data->type = actor->destMapStoreType;
									data->relatedMapIndex = game->currentMapIndex;
									data->relatedActorId = actor->id;
									data->itemsNum = -1;

									game->currentStoreData = data;
								}
							}

							break;
						}
					}
				}
			} else {
				actor->doorPlayerSpawnedOver = false;
			}

			pushAABB(getAABB(actor), lerpColor(0xFF523501, 0xFF121212, actor->locked));
		} else if (actor->type == ACTOR_ITEM) {
			if (actor->isOnGround) {
				ItemTypeInfo *itemTypeInfo = &game->itemTypeInfos[actor->itemType];
				if (itemTypeInfo->slotType != ITEM_SLOT_WEAPON && overlaps(actor, player)) {
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

			Matrix4 matrix = mat4();
			matrix.TRANSLATE(getCenter(getAABB(actor)));
			matrix.SCALE(globals->actorModelScale);
			if (actor->itemType == ITEM_SWORD) pushModel(getModel("assets/models/sword.model"), matrix);
			else if (actor->itemType == ITEM_KNIFE) pushModel(getModel("assets/models/knife.model"), matrix);
			else if (actor->itemType == ITEM_MONEY) pushAABB(getAABB(actor), 0xFFFFFB94);
		} else if (actor->type == ACTOR_STORE) {
			bool overlappingStore = overlaps(actor, player);

			if (!game->inStore && overlappingStore && !game->ignoreStoreOverlap) {
				game->inStore = true;
				game->ignoreStoreOverlap = true;
			}

			if (!overlappingStore) game->ignoreStoreOverlap = false;
			pushAABB(getAABB(actor), 0xFF45E6E6);
		} else {
			pushAABB(getAABB(actor), lerpColor(0xFFFF00FF, 0x00FF00FF, timePhase(platform->time)));
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

		if (actor->info->hasPhysics) {
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

			if (actor->isOnGround) actor->accel += actor->movementAccel;
			if (!isZero(actor->movementAccel)) {
				actor->movementAccel = v3();
				actor->isBlocking = false;
			}

			float grav = 0.75;
			for (int i = 0; i < getBuffCount(actor, BUFF_HEAVEN_STEP_GRAVITY); i++) grav *= 0.5;

			actor->accel.z -= grav;

			for (int i = 0; i < actor->thrustersNum; i++) {
				Thruster *thruster = &actor->thrusters[i];
				if (thruster->maxTime == 0) thruster->maxTime = 1/60.0;

				actor->accel += thruster->accel;

				float thrusterTimeScale = 1;
				if (actor->stasisTimeLeft > 0) thrusterTimeScale *= STASIS_TIME_SCALE_AMOUNT;
				thruster->time += elapsed * thrusterTimeScale;
				if (thruster->time >= thruster->maxTime) {
					arraySpliceIndex(actor->thrusters, actor->thrustersNum, sizeof(Thruster), i);
					actor->thrustersNum--;
					i--;
					continue;
				}
			}

			Vec3 damping = v3();
			if (actor->isOnGround) {
				damping = v3(0.2, 0.2, 0);
			} else {
				damping = v3(0, 0, 0.02);
			}

			float physicsTimeScale = elapsed / (1/60.0);
			if (actor->stasisTimeLeft > 0) physicsTimeScale *= STASIS_TIME_SCALE_AMOUNT;

			actor->velo += (actor->accel - damping*actor->velo) * physicsTimeScale;
			actor->accel = v3();

			Vec3 positionOffset = actor->velo * physicsTimeScale;

			{ // AABB collision
				bool bouncedOffSideWall = false;

				AABB oldAABB = getAABB(actor);
				AABB newAABB = oldAABB + positionOffset;
				actor->isOnGround = false;
				for (int i = 0; i < wallsNum; i++) {
					AABB wallAABB = walls[i];
					if (!intersects(newAABB, wallAABB)) {
						Vec3 feetPoint = getCenter(newAABB) + getSize(newAABB)*v3(0, 0, -0.5);
						feetPoint.z -= GROUND_SPACING*2;
						if (contains(wallAABB, feetPoint)) actor->isOnGround = true;
						continue;
					}

					if (newAABB.min.z <= wallAABB.max.z && oldAABB.min.z > wallAABB.max.z) { // Bot
						float dist = wallAABB.max.z - newAABB.min.z;
						newAABB += v3(0, 0, dist+GROUND_SPACING);
						actor->velo.z = 0;
						actor->isOnGround = true;
					}

					// if (newAABB.min.z >= wallAABB.min.z && oldAABB.max.z < wallAABB.max.z) { // Top //@todo
					// 	float dist = wallAABB.min.z - newAABB.max.z;
					// 	newAABB += v3(0, 0, dist-0.1);
					// 	actor->velo.z = 0;
					// }

					float restitution = 0.5;

					if (newAABB.min.x <= wallAABB.max.x && oldAABB.min.x > wallAABB.max.x) { // left
						float dist = wallAABB.max.x - newAABB.min.x;
						newAABB += v3(dist+GROUND_SPACING, 0, 0);
						if (actor->velo.x < 0) {
							bouncedOffSideWall = true;
							actor->velo.x *= -restitution;
						}
					}

					if (newAABB.max.x >= wallAABB.min.x && oldAABB.max.x < wallAABB.min.x) { // right
						float dist = wallAABB.min.x - newAABB.max.x;
						newAABB += v3(dist-GROUND_SPACING, 0, 0);
						if (actor->velo.x > 0) {
							bouncedOffSideWall = true;
							actor->velo.x *= -restitution;
						}
					}

					if (newAABB.min.y <= wallAABB.max.y && oldAABB.min.y > wallAABB.max.y) {
						float dist = wallAABB.max.y - newAABB.min.y;
						newAABB += v3(0, dist+GROUND_SPACING, 0);
						if (actor->velo.y < 0) {
							bouncedOffSideWall = true;
							actor->velo.y *= -restitution;
						}
					}

					if (newAABB.max.y >= wallAABB.min.y && oldAABB.max.y < wallAABB.min.y) {
						float dist = wallAABB.min.y - newAABB.max.y;
						newAABB += v3(0, dist-GROUND_SPACING, 0);
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
		}

		if (actor->type == ACTOR_UNIT) { // Update movementPerc
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

		if (game->debugNeverTakeDamage && actor == player) player->hp = 100;
		if (game->inEditor) {
			AABB aabb = getAABB(actor);
			Rect bounds = getBounds(aabb);
			if (contains(bounds, game->mouse) || game->debugAlwaysShowWireframes) {
				pushAABBOutline(aabb, 2, 0xFFFFFFFF);
				pushScreenRectOutline(bounds, 1, 0xFF00FFFF);
			}

			if (game->selectedActorId == actor->id) {
				pushAABBOutline(aabb, 2, lerpColor(0xFFFFFF80, 0xFFFFFF00, secondPhase));

				if (game->inEditor) {
					Line3 lineX = makeLine3(actor->position, actor->position + v3(64, 0, 0));
					Line3 lineY = makeLine3(actor->position, actor->position + v3(0, 64, 0));
					Line3 lineZ = makeLine3(actor->position, actor->position + v3(0, 0, 64));
					pushScreenLine(getScreenPoint(lineX.start), getScreenPoint(lineX.end), 4, 0xFFFF0000);
					pushScreenLine(getScreenPoint(lineY.start), getScreenPoint(lineY.end), 4, 0xFF00FF00);
					pushScreenLine(getScreenPoint(lineZ.start), getScreenPoint(lineZ.end), 4, 0xFF0000FF);
				}
			}
		}

		if (actor->hp <= 0 && actor->maxHp > 0) {
			actor->markedForDeletion = true;
			if (actor->type == ACTOR_UNIT) {
				if (actor->playerControlled) {
					game->nextMapIndex = CITY_START_INDEX;
					logf("You died (-$%.2f)\n", player->money*0.5);
					player->money *= 0.5;
					player->hp = player->maxHp;
					player->markedForDeletion = false;
				} else {
					map->alliances[actor->team] -= actor->allianceCost;

					if (actor->team != player->team) {
						game->leftToBeatTillUnlock--;

						int moneyToGive = clampMap(actor->level, 1, 100, 1, 50);
						for (;;) {
							int chunkSize = rndInt(1, 3);
							if (chunkSize > moneyToGive) chunkSize = moneyToGive;

							Actor *money = createActor(map, ACTOR_ITEM);
							money->position = actor->position;
							money->itemType = ITEM_MONEY;
							money->itemAmount = chunkSize;
							money->velo.x = rndFloat(-1, 1);
							money->velo.y = rndFloat(-1, 1);
							money->velo.z = rndFloat(0.2, 1)*2;
							money->velo *= v3(7, 7, 12);

							moneyToGive -= chunkSize;
							if (moneyToGive <= 0) break;
						}
					}
				}
			}
		}
		actor->hp = mathClamp(actor->hp, 0, actor->maxHp);
		if (actor->actionsNum == 0) actor->stamina += getStatPoints(actor, STAT_STAMINA_REGEN) * 0.03 * (elapsed*60.0);
		actor->stamina = mathClamp(actor->stamina, -100, actor->maxStamina);

		actor->aiStateTime += elapsed;
		actor->stasisTimeLeft -= elapsed;
		if (actor->stasisTimeLeft < 0) actor->stasisTimeLeft = 0;

		if (actor->position.z < -10000) actor->position = v3(0, 0, 500);
	} ///

	{ /// Update pickup prompt
		Actor *promptItem = NULL;
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->type != ACTOR_ITEM) continue;

			ItemTypeInfo *itemTypeInfo = &game->itemTypeInfos[actor->itemType];
			if (itemTypeInfo->slotType != ITEM_SLOT_WEAPON) continue;

			if (player->heldItem.type == ITEM_NONE && overlaps(player, actor)) {
				promptItem = actor;
				break;
			}
		}

		if (promptItem) {
			Vec2 position2 = getScreenPoint(promptItem->position);
			Rect rect = makeCenteredSquare(position2, 64);
			pushScreenRect(rect, 0x80FF0000);

			if (pickupPressed) {
				Action *action = addAction(player, ACTION_START_PICKUP);
				action->relatedActorId = promptItem->id;
				addAction(player, ACTION_END_PICKUP);
			}
		}
	} ///

	{ /// Update sleeping
		// if (game->isSleeping) {
		// 	if (game->cityTicks == 0) {
		// 		game->isSleeping = false;
		// 	}
		// }
	} ///

	{ /// Update particles
		for (int i = 0; i < game->particlesNum; i++) {
			Particle *particle = &game->particles[i];
			bool complete = false;

			if (particle->delay > 0) {
				particle->delay -= elapsed; 
				continue;
			}

			particle->time += elapsed;
			if (particle->time > particle->maxTime) complete = true;

			float fadeInPerc = 0.10;
			float fadeOutPerc = 0.10;
			Vec3 accel = v3();

			if (particle->type == PARTICLE_DUST) {
			} else if (particle->type == PARTICLE_STASIS_GAIN) {
				fadeInPerc = 0.20;
				fadeOutPerc = 0.50;
				accel.z += 1;
			}

			float particleTimeScale = elapsed / (1/60.0);

			Vec3 damping = v3(0.05, 0.05, 0.05);
			particle->velo += (accel - damping*particle->velo) * particleTimeScale;

			particle->position += particle->velo * particleTimeScale;

			float alpha =
				clampMap(particle->time, 0, particle->maxTime*fadeInPerc, 0, 1)
				* clampMap(particle->time, particle->maxTime*(1-fadeOutPerc), particle->maxTime, 1, 0);

			DrawBillboardCall billboard = {};
			billboard.texture = renderer->circleTexture1024;
			billboard.position = particle->position;
			billboard.size = particle->size;
			billboard.tint = particle->tint;
			billboard.alpha = alpha;
			pushBillboard(billboard);

			if (complete) {
				game->particles[i] = game->particles[game->particlesNum-1];
				game->particlesNum--;
				i--;
				continue;
			}
		}
	} ///

	{ /// Update effects
		for (int i = 0; i < game->effectsNum; i++) {
			Effect *effect = &game->effects[i];

			bool complete = false;
			float maxTime = 1;
			Vec2 position2 = getScreenPoint(effect->position);

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
			} else if (effect->type == EFFECT_STASIS_GAIN) {
				maxTime = 0.1;
				char *mainBones[] = {
					"chest", "body", "head", 
					"hand.l", "arm1.l", "arm2.l", 
					"toes.l", "leg1.l", "leg2.l", 
					"hand.r", "arm1.r", "arm2.r", 
					"toes.r", "leg1.r", "leg2.r", 
				};

				Actor *actor = getActor(map, effect->actorId);
				if (effect->time == 0 && actor && actor->skeleton) {
					for (int i = 0; i < ArrayLength(mainBones); i++) {
						Matrix4 boneMatrix = getBoneMatrix(actor, mainBones[i]);
						Particle *particle = createParticle(PARTICLE_STASIS_GAIN);
						particle->position = boneMatrix * v3();
						particle->position.y -= 5;
						particle->size = v2(32, 32);
						particle->velo.x = rndFloat(-2, 2);
						particle->velo.z = rndFloat(-1, 1);
						particle->maxTime = rndFloat(0.1, 0.5);
						particle->tint = 0x80606090;
					}
				}
			}

			if (effectText) {
				Vec2 size = getTextSize(game->particleFont, effectText);
				Vec2 pos = position2 - size/2;

				float alpha =
					clampMap(effect->time, 0, maxTime*0.05, 0, 1)
					* clampMap(effect->time, maxTime*0.95, maxTime, 1, 0);

				pos.y += clampMap(effect->time, 0, maxTime*0.5, -20, 0, BOUNCE_OUT);

				Rect rect = makeRect(pos, size);

				pushScreenRect(inflate(rect, 2), alphaColor(0x30000000, alpha));
				pushScreenText(effectText, newDrawTextProps(game->particleFont, alphaColor(effectTextColor, alpha), pos));
			}

			effect->time += elapsed;
			if (effect->time > maxTime) {
				if (effect->pulses > 0) {
					effect->pulses--;
					effect->time = 0;
				} else {
					complete = true;
				}
			}

			if (complete) {
				arraySpliceIndex(game->effects, game->effectsNum, sizeof(Effect), i);
				game->effectsNum--;
				i--;
				continue;
			}
		}
	} ///

	pushTargetTexture(game->overlayTexture);
	clearRenderer();

	{ /// Update map
		game->timeTillNextCityTick -= elapsed;
		if (game->timeTillNextCityTick <= 0) {
			game->cityTicks++;
			game->timeTillNextCityTick = SECS_PER_CITY_TICK;
		}

		float spreadAmount = 0.0005;
		while (game->cityTicks > 0) {
			game->cityTicks--;

			float prevCityTime = game->cityTime;
			game->cityTime += SECS_PER_CITY_TICK;

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

			{ /// Update stores
				float prevMod = fmod(prevCityTime, 120);
				float currentMod = fmod(game->cityTime, 120);
				if (prevMod > currentMod || prevCityTime == 0 || game->debugForceRestock) {
					if (game->extraStepsFromSleep <= 0) logf("Stores have restocked\n");
					game->debugForceRestock = false;
					for (int i = 0; i < game->storeDatasNum; i++) {
						StoreData *data = &game->storeDatas[i];
						data->itemsNum = -1;
					}
				}
			} ///
		}

		{ /// Spawn enemies // Spawn npcs
			auto createNpcUnit = [](Map *map)->Actor *{
				int team = rndPick(map->alliances, TEAMS_MAX);
				Actor *actor = createUnit(map, team);

				actor->allianceCost = 0.05;

				float surroundingAlliance = countSurroundingAlliance(getCoordsByCityMap(map), actor->team);
				int extraPoints = clampMap(surroundingAlliance, 0, 8, 10, 50);
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

		{ /// Draw map
			pushTargetTexture(game->mapTexture);
			clearRenderer();
			if (keyJustPressed('M') || joyButtonJustPressed(0, JOY_SELECT)) game->lookingAtMap = !game->lookingAtMap;
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
						} else if (game->mapVisualization == MAP_VISUALIZATION_TOTAL_ALLIANCE_SURROUNDING) {
							Rect rect = makeRect(cursor, mapTileSize);

							Rect tile = rect;
							tile.width *= 0.5;
							tile.height *= 0.25;

							if (TEAMS_MAX != 8 && platform->frameCount % 60 == 0) logf("You have to change how MAP_VISUALIZATION_TOTAL_ALLIANCE_SURROUNDING works\n");
							for (int i = 0; i < TEAMS_MAX; i++) {
								DrawTextProps props = newDrawTextProps(game->defaultFont, teamColors[i]);
								drawTextInRect(frameSprintf("%.2f", countSurroundingAlliance(v2i(x, y), i)), props, tile);
								tile.y += tile.height;
								if (i == 3) {
									tile.x += tile.width;
									tile.y = rect.y;
								}
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

				if (game->lastStepOfFrame && game->inEditor) {
					Map *map = game->editorSelectedCityMap;
					if (map) {
						ImGui::Begin("Map data", NULL, ImGuiWindowFlags_AlwaysAutoResize);
						ImGui::Combo("Visual", (int *)&game->mapVisualization, mapVisualizationStrings, ArrayLength(mapVisualizationStrings));
						ImGui::Separator();

						if (ImGui::Button("Advance 60 seconds")) {
							game->cityTicks += 60.0/SECS_PER_CITY_TICK;
						}
						if (ImGui::Button("Advance 5 min")) {
							game->cityTicks += (60.0*5)/SECS_PER_CITY_TICK;
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
			if (keyPressed(KEY_CTRL) && keyJustPressed('N')) game->inInventory = !game->inInventory;
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

						Rect iconRect = makeRect(cursor, cellSize);
						drawItemIcon(item, inflatePerc(iconRect, -0.1));

						if (platform->mouseJustDown && contains(iconRect, game->mouse)) {
							logf("You clicked an item!\n");
						}

						cursor.x += iconRect.width;
						if (cursor.x + cellSize.x > itemContainer.x + itemContainer.width) {
							cursor.x = itemContainer.x;
							cursor.y += cellSize.y;
						}
					}
				}
			}
		} ///

		{ /// Corner text
			const int TEXT_LINES_MAX = 32;
			char *textLines[TEXT_LINES_MAX];
			int textLinesNum = 0;

			bool usingController = false;
			if (usingController) {
				textLines[textLinesNum++] = "Square - Punch";
				textLines[textLinesNum++] = "Triangle - Kick";
				textLines[textLinesNum++] = "Circle - Pickup";
				textLines[textLinesNum++] = "Select - Map";
				if (game->alliancesControlled[0]) textLines[textLinesNum++] = frameSprintf("L1 - Quaff potion (%d/3)", player->potionsLeft);
				if (game->alliancesControlled[1]) textLines[textLinesNum++] = "R1 - Dash";
				if (game->alliancesControlled[2]) textLines[textLinesNum++] = "L3 - Armor up";
				if (game->alliancesControlled[3]) textLines[textLinesNum++] = "R2 - Stasis";
				if (game->alliancesControlled[4]) textLines[textLinesNum++] = "Circle(hold) - Create weapon";
				if (game->alliancesControlled[5]) textLines[textLinesNum++] = "D-pad down - Stash weapon";
			} else {
				textLines[textLinesNum++] = "J - Punch";
				textLines[textLinesNum++] = "K - Kick";
				textLines[textLinesNum++] = "L - Pickup";
				textLines[textLinesNum++] = "M - Map";
				if (game->alliancesControlled[0]) textLines[textLinesNum++] = frameSprintf("Q - Quaff potion (%d/3)", player->potionsLeft);
				if (game->alliancesControlled[1]) textLines[textLinesNum++] = "N - Dash";
				if (game->alliancesControlled[2]) textLines[textLinesNum++] = "B - Armor up";
				if (game->alliancesControlled[3]) textLines[textLinesNum++] = "U - Stasis";
				if (game->alliancesControlled[4]) textLines[textLinesNum++] = "L(hold) - Create weapon";
				if (game->alliancesControlled[5]) textLines[textLinesNum++] = "H - Stash weapon";
			}

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
		} ///

	} /// 
	popTargetTexture(); // game->overlayTexture


	game->prevMapTime = game->mapTime;
	game->mapTime += elapsed;

	game->prevTime = game->time;
	game->time += elapsed;
}

void updateStore(Actor *player, Actor *storeActor, float elapsed) {
	Map *map = &game->maps[game->currentMapIndex];

	if (game->inStore) {
		StoreData *data = game->currentStoreData;
		if (!data) {
			logf("No store data!\n");
			game->inStore = false;
			return;
		}

		if (data->itemsNum == -1) {
			auto getRandomStoreItemType = [](StoreType storeType)->ItemType {
				Item *allItems = (Item *)frameMalloc(sizeof(Item) * ITEM_TYPES_MAX);
				int allItemsNum = 0;
				for (int i = 0; i < ITEM_TYPES_MAX; i++) {
					Item *item = &allItems[allItemsNum++];
					initItem(item, (ItemType)i, 1);
					item->amount = item->info->maxAmountFromStore - item->info->everBought;

					bool allowItem = true;
					if (i >= ITEM_FOOD_0 && i <= ITEM_FOOD_8) {
						if (storeType == STORE_ATTACK) {
							if (i < ITEM_FOOD_0 || i > ITEM_FOOD_2) allowItem = false;
						} else if (storeType == STORE_STAMINA) {
							if (i < ITEM_FOOD_3 || i > ITEM_FOOD_5) allowItem = false;
						} else if (storeType == STORE_UTILITY) {
							if (i < ITEM_FOOD_6 || i > ITEM_FOOD_8) allowItem = false;
						}
					}
					if (!allowItem) item->amount = 0;
				}

				for (int i = 0; i < game->storeDatasNum; i++) {
					StoreData *data = &game->storeDatas[i];
					for (int i = 0; i < data->itemsNum; i++) {
						Item *item = &data->items[i];

						allItems[item->type].amount -= item->amount;
					}
				}

				int *chances = (int *)frameMalloc(sizeof(int) * ITEM_TYPES_MAX);
				int totalChances = 0;
				for (int i = 0; i < allItemsNum; i++) {
					Item *item = &allItems[i];
					if (item->amount < 0) item->amount = 0;
					chances[i] += item->amount;
					totalChances += item->amount;
				}

				if (totalChances <= 0) return ITEM_NONE;

				ItemType chosenType = (ItemType)rndPick(chances, ITEM_TYPES_MAX);
				return chosenType;
			};

			data->itemsNum = 0;
			for (int i = 0; i < 8; i++) {
				ItemType type = getRandomStoreItemType(data->type);
				if (type == ITEM_NONE) continue;
				initItem(&data->items[data->itemsNum++], type, 1);
			}
		}

		Rect bgRect = makeCenteredRect(game->size/2, game->size*v2(0.8, 0.6));
		drawRect(bgRect, 0xFF222222);

		Rect innerRect = inflatePerc(bgRect, -0.2);

		for (int i = 0; i < data->itemsNum; i++) {
			Item *item = &data->items[i];

			Rect itemSlice = innerRect;
			itemSlice.width /= data->itemsNum;
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

			float price = item->info->basePrice;
			if (item->price) price = item->price;

			Rect priceRect = buyRect;
			priceRect.y -= priceRect.height + priceRect.height*0.2;
			priceRect.width = itemSlice.width * 0.9;
			priceRect.x = itemSlice.x + itemSlice.width/2 - priceRect.width/2;
			drawRect(priceRect, 0xFF111111);
			{
				DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFF909090);
				drawTextInRect(frameSprintf("Price $%.2f", price), props, priceRect);
			}

			Rect descRect = inflatePerc(itemSlice, v2(-0.1, -0.6));
			descRect.y = priceRect.y - descRect.height*1.05;
			drawRect(descRect, 0xFF111111);
			{
				char *descLines[16] = {};
				int descLinesNum = 0;
				int descTotal = 0;
				for (int i = 0; i < STATS_MAX; i++) {
					float toGive = item->info->statsToGive[i];
					if (toGive > 0) {
						descLines[descLinesNum++] = frameSprintf("+%.0f %s", toGive, statStrings[i]);
						descTotal += strlen(descLines[descLinesNum-1]);
					} else if (toGive < 0) {
						descLines[descLinesNum++] = frameSprintf("-%.0f %s", toGive, statStrings[i]);
						descTotal += strlen(descLines[descLinesNum-1]);
					}
				}
				if (item->info->extraHpFromConsume) descLines[descLinesNum++] = frameSprintf("+%.0f hp", item->info->extraHpFromConsume);

				char *desc = (char *)frameMalloc(descTotal + 128);
				for (int i = 0; i < descLinesNum; i++) {
					strcat(desc, descLines[i]);
					strcat(desc, "\n");
				}
				DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFF909090);
				drawTextInRect(desc, props, descRect, v2(0.5, 0));
			}

			Rect titleRect = inflatePerc(itemSlice, v2(-0.1, -0.8));
			titleRect.y = itemSlice.y + itemSlice.height * 0.03;
			drawRect(titleRect, 0xFF111111);
			{
				DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFF909090);
				drawTextInRect(item->info->name, props, titleRect, v2(0.5, 0));
			}

			if (contains(buyRect, game->mouse) && platform->mouseJustUp) {
				if (player->money >= price) {
					player->money -= price;
					item->info->everBought++;
					giveItem(player, item->type, item->amount); // Maybe giveItem should take an item pointer to copy the id?
					arraySpliceIndex(data->items, data->itemsNum, sizeof(Item), i);
					data->itemsNum--;
				} else {
					infof("Not enough cash\n");
				}
			}
		}

		Rect exitRect = getInnerRectOfSize(bgRect, getSize(bgRect)*0.07, v2(0.99, 0.99));
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
		actor->level = 30;
		actor->potionsLeft = 3;

		actor->skeleton = deriveSkeleton("assets/skeletons/unit.skele");
		createSkeletonBlend(actor->skeleton, "main", SKELETON_BLEND_ANIMATION);

		SkeletonBlend *nextBlend = createSkeletonBlend(actor->skeleton, "next", SKELETON_BLEND_ANIMATION);
		nextBlend->animation = getAnimation(actor->skeleton, "idle");

		SkeletonBlend *fadeOutBlend = createSkeletonBlend(actor->skeleton, "fadeOut", SKELETON_BLEND_MANUAL_BONES);
		fadeOutBlend->weight = 0;

	} else if (actor->type == ACTOR_ITEM) {
		actor->size = v3(50, 50, 50);
	}
	return actor;
}

void deleteActor(Map *map, Actor *actor) {
	removeActorById(map, actor->id);
}

Actor *getActor(Map *map, int id) {
	if (id == 0) return NULL;

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

	if (actor->items) free(actor->items);
	if (actor->skeleton) destroySkeleton(actor->skeleton);
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

Actor *createUnit(Map *map, int team) {
	Actor *actor = createActor(map, ACTOR_UNIT);
	actor->stats[STAT_DAMAGE] = 5;
	actor->stats[STAT_HP] = 5;
	actor->stats[STAT_MAX_STAMINA] = 5;
	actor->stats[STAT_STAMINA_REGEN] = 5;
	actor->stats[STAT_MOVEMENT_SPEED] = 5;
	actor->stats[STAT_ATTACK_SPEED] = 5;
	actor->level = 0;

	actor->team = team;
	if (actor->team == 2) actor->hasHyperArmor = rndPerc(0.9);
	if (actor->team == 3) actor->hasStasisAttack = rndPerc(0.9);
	return actor;
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

float getStatPoints(Actor *actor, StatType stat) {
	float value = actor->stats[stat];
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

float countSurroundingAlliance(Vec2i startingIndex, int team) {
	float alliance = 0;
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
		alliance += adjMap->alliances[team];
	}

	return alliance;
}

Rect getBounds(AABB aabb) {
	Rect bounds = makeRect();
	Vec3 points[8];
	toPoints(aabb, points);
	for (int i = 0; i < 8; i++) {
		Vec2 point = getScreenPoint(points[i]);
		bounds = insert(bounds, point);
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

bool overlaps(Actor *actor, Sphere sphere) {
	bool ret = overlaps(getAABB(actor), sphere);
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

Vec2 getScreenPoint(Vec3 position) {
	Vec2 position2 = worldSpaceTo2dNDC01(game->camera3d, position) * v2(platform->windowSize);
	return position2;
}

void pushAABB(AABB aabb, int color, float alpha) {
	WorldElement *element = createWorldElement();
	element->type = WORLD_ELEMENT_CUBE;
	element->aabb = aabb;
	element->color = color;
	element->alpha = alpha;
}

void pushAABBOutline(AABB aabb, int lineThickness, int color) {
	Vec3 points[24];
	AABBToLines(aabb, points);

	for (int i = 0; i < 12; i++) {
		Vec2 start = getScreenPoint(points[i*2 + 0]);
		Vec2 end = getScreenPoint(points[i*2 + 1]);
		pushScreenLine(start, end, lineThickness, color);
	}
}

void pushBillboard(DrawBillboardCall billboard) {
	if (game->billboardsNum > BILLBOARDS_MAX-1) {
		game->billboardsNum = BILLBOARDS_MAX-1;
		logf("Too many billboards!\n");
	}

	game->billboards[game->billboardsNum++] = billboard;
}

WorldElement *createWorldElement() {
	if (game->worldElementsNum > WORLD_ELEMENTS_MAX-1) {
		game->worldElementsNum = WORLD_ELEMENTS_MAX-1;
		logf("Too many world elements!\n");
	}

	WorldElement *element = &game->worldElements[game->worldElementsNum++];
	memset(element, 0, sizeof(WorldElement));
	element->alpha = 1;
	return element;
}

void pushTriangle(Vec3 vert0, Vec3 vert1, Vec3 vert2, int color) {
	WorldElement *element = createWorldElement();
	element->type = WORLD_ELEMENT_TRIANGLE;
	element->tri.verts[0] = vert0;
	element->tri.verts[1] = vert1;
	element->tri.verts[2] = vert2;
	element->color = color;
}

void pushSphere(Sphere sphere, int color, float alpha) {
	WorldElement *element = createWorldElement();
	element->type = WORLD_ELEMENT_SPHERE;
	element->sphere = sphere;
	element->color = color;
	element->alpha = alpha;
}

void pushModel(Model *model, Matrix4 matrix, Skeleton *skeleton, int color) {
	WorldElement *element = createWorldElement();
	element->type = WORLD_ELEMENT_MODEL;
	element->model = model;
	element->skeleton = skeleton;
	element->color = color;
	element->modelMatrix = matrix;
}

ScreenElement *createScreenElement() {
	if (game->screenElementsNum > SCREEN_ELEMENTS_MAX-1) {
		game->screenElementsNum = SCREEN_ELEMENTS_MAX-1;
		logf("Too many screen elements!\n");
	}

	ScreenElement *element = &game->screenElements[game->screenElementsNum++];
	memset(element, 0, sizeof(ScreenElement));
	return element;
}

void pushScreenRect(Rect rect, int color) {
	ScreenElement *element = createScreenElement();
	element->type = SCREEN_ELEMENT_RECT;
	element->rect = rect;
	element->color = color;
}

void pushScreenRectOutline(Rect rect, int thickness, int color) {
	ScreenElement *element = createScreenElement();
	element->type = SCREEN_ELEMENT_RECT_OUTLINE;
	element->rect = rect;
	element->color = color;
	element->thickness = thickness;
}

void pushScreenLine(Vec2 start, Vec2 end, int thickness, int color) {
	ScreenElement *element = createScreenElement();
	element->type = SCREEN_ELEMENT_LINE;
	element->line = makeLine2(start, end);
	element->color = color;
	element->thickness = thickness;
}

void pushScreenText(char *text, DrawTextProps drawTextProps) {
	ScreenElement *element = createScreenElement();
	element->type = SCREEN_ELEMENT_TEXT;
	element->text = text;
	element->drawTextProps = drawTextProps;
}

void pushScreenTextInRect(char *text, DrawTextProps drawTextProps, Rect rect) {
	ScreenElement *element = createScreenElement();
	element->type = SCREEN_ELEMENT_TEXT_IN_RECT;
	element->text = text;
	element->rect = rect;
	element->drawTextProps = drawTextProps;
}

int playWorldSound(char *path, Vec3 worldPosition) {
	if (!game->debugSkipPrewarm && game->mapTime < ROOM_PREWARM_TIME) return 0;

	Sound *sound = getSound(path);
	if (!sound) {
		logf("No sound called %s\n", path);
		return -1;
	}

	int id;
	{
		Channel *channel = playSound(sound);
		channel->userVolume = 0.2;
		id = channel->id;
	}
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
		game->particles = (Particle *)resizeArray(game->particles, sizeof(Particle), game->particlesNum, game->particlesMax*2);
		game->particlesMax *= 2;
	}

	Particle *particle = &game->particles[game->particlesNum++];
	memset(particle, 0, sizeof(Particle));
	particle->type = type;
	particle->tint = 0xFFFFFFFF;
	particle->size = v2(32, 32);
	return particle;
}

void applyThruster(Actor *actor, Thruster newThruster, bool flipX) {
	if (actor->thrustersNum > THRUSTERS_MAX) {
		logf("Too many thrusters\n");
		actor->thrustersNum--;
	}
	Thruster *thruster = &actor->thrusters[actor->thrustersNum++];
	*thruster = newThruster;
	if (flipX) thruster->accel.x *= -1;
	thruster->time = 0; // This shouldn't actually be needed
}

Matrix4 getBoneMatrix(Actor *actor, char *boneName) {
	int boneIndex = getBoneIndex(actor->skeleton, boneName);
	if (boneIndex == -1) {
		logf("Can't find bone %s\n", boneName);
		return mat4();
	}

	Bone *bone = &actor->skeleton->base->bones[boneIndex];
	Matrix4 matrix = actor->modelMatrix * actor->skeleton->meshTransforms[boneIndex] * bone->modelSpaceMatrix;
	return matrix;
}

void saveMap(Map *map, int mapFileIndex) {
	DataStream *stream = newDataStream();

	int mapVersion = 8;
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
		writeU32(stream, actor->destMapStoreType);
		writeU32(stream, 0);
		writeString(stream, actor->modelPath);
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
	map->isTemplatized = readU8(stream);

	map->actorsNum = readU32(stream);
	memset(map->actors, 0, sizeof(Actor) * ACTORS_MAX);
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		actor->type = (ActorType)readU32(stream);
		actor->info = &game->actorTypeInfos[actor->type];
		actor->id = readU32(stream);
		readStringInto(stream, actor->name, ACTOR_NAME_MAX_LEN);
		actor->position = readVec3(stream);
		actor->size = readVec3(stream);
		readStringInto(stream, actor->destMapName, MAP_NAME_MAX_LEN);
		actor->destMapStoreType = (StoreType)readU32(stream);
		readU32(stream);
		readStringInto(stream, actor->modelPath, MAP_NAME_MAX_LEN);
	}

	map->nextActorId = readU32(stream);

	for (int i = 0; i < TEAMS_MAX; i++) map->baseAlliances[i] = readFloat(stream);

	destroyDataStream(stream);
}

void refreshMap(Map *map) {
	memcpy(map->alliances, map->baseAlliances, sizeof(float) * TEAMS_MAX);
	for (int i = 0; i < TEAMS_MAX; i++) {
		if (map->alliances[i] >= 1) {
			map->fortifiedByTeam = i;
			map->fortifiedPerc = 1;
		}
	}
}

void saveGlobals() {
	Globals *globals = &game->globals;

	DataStream *stream = newDataStream();

	int globalsVersion = 18;
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
		writeVec3(stream, v3()); // @todo remove
		writeVec3(stream, v3()); // @todo remove
		writeFloat(stream, info->damage);
		writeVec3(stream, v3()); //@todo remove
		writeU32(stream, 0); //@todo remove
		writeFloat(stream, info->staminaUsage);
		writeU32(stream, info->buffToGet);
		writeFloat(stream, info->buffToGetTime);
		writeU32(stream, info->buffToGive);
		writeFloat(stream, info->buffToGiveTime);

		writeU32(stream, info->thrusterTriggersNum);
		for (int i = 0; i < info->thrusterTriggersNum; i++) {
			ThrusterTrigger *trigger = &info->thrusterTriggers[i];
			writeU32(stream, trigger->frame);
			writeU8(stream, trigger->requiresHit);

			Thruster *thruster = &trigger->thruster;
			writeVec3(stream, thruster->accel);
			writeFloat(stream, thruster->maxTime);
		}

		writeVec3(stream, info->hitThruster.accel);
		writeFloat(stream, info->hitThruster.maxTime);
		writeVec3(stream, info->blockThruster.accel);
		writeFloat(stream, info->blockThruster.maxTime);
	}

	writeVec3(stream, v3());
	writeFloat(stream, 0);
	writeFloat(stream, 0);
	writeVec3(stream, globals->actorModelOffset);
	writeFloat(stream, globals->actorModelScale);
	writeFloat(stream, globals->movementPercDistanceWalkingRatio);
	writeFloat(stream, globals->movementPercDistanceRunningRatio);

	writeFloat(stream, globals->specularPower);
	writeFloat(stream, globals->pixelizePower);
	writeFloat(stream, globals->fxaaResolutionScale);
	writeFloat(stream, globals->posterizeGamma);
	writeFloat(stream, globals->posterizeNumColors);

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
		readVec3(stream); // @todo remove
		readVec3(stream); // @todo remove
		info->damage = readFloat(stream);
		readVec3(stream); // @todo remove
		readU32(stream); // @todo remove
		info->staminaUsage = readFloat(stream);
		info->buffToGet = (BuffType)readU32(stream);
		info->buffToGetTime = readFloat(stream);
		info->buffToGive = (BuffType)readU32(stream);
		info->buffToGiveTime = readFloat(stream);

		info->thrusterTriggersNum = readU32(stream);
		for (int i = 0; i < info->thrusterTriggersNum; i++) {
			ThrusterTrigger *trigger = &info->thrusterTriggers[i];
			memset(trigger, 0, sizeof(ThrusterTrigger));
			trigger->frame = readU32(stream);
			trigger->requiresHit = readU8(stream);

			Thruster *thruster = &trigger->thruster;
			thruster->accel = readVec3(stream);
			thruster->maxTime = readFloat(stream);
		}

		info->hitThruster.accel = readVec3(stream);
		info->hitThruster.maxTime = readFloat(stream);
		info->blockThruster.accel = readVec3(stream);
		info->blockThruster.maxTime = readFloat(stream);
	}

	readVec3(stream);
	readFloat(stream);
	readFloat(stream);
	globals->actorModelOffset = readVec3(stream);
	globals->actorModelScale = readFloat(stream);
	globals->movementPercDistanceWalkingRatio = readFloat(stream);
	globals->movementPercDistanceRunningRatio = readFloat(stream);
	globals->specularPower = readFloat(stream);
	globals->pixelizePower = readFloat(stream);
	globals->fxaaResolutionScale = readFloat(stream);
	globals->posterizeGamma = readFloat(stream);
	globals->posterizeNumColors = readFloat(stream);

	destroyDataStream(stream);
}
