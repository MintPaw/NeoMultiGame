#if defined(__EMSCRIPTEN__)
#define RTS_AUTO_NONE
#else

#define RTS_AUTO_SINGLE
// #define RTS_AUTO_LOCAL_MULTI
// #define RTS_AUTO_JOIN_LOBBY_1
// #define RTS_AUTO_NONE

#endif

#if defined(RTS_AUTO_SINGLE)
bool flagShouldInstantlyConnect = false;
bool flagShouldInstantlySinglePlayer = true;
#endif

#if defined(RTS_AUTO_LOCAL_MULTI)
bool flagShouldInstantlyConnect = true;
bool flagShouldInstantlySinglePlayer = false;
#endif

#if defined(RTS_AUTO_JOIN_LOBBY_1)
bool flagShouldInstantlyConnect = true;
bool flagShouldInstantlySinglePlayer = false;
#endif

#if defined(RTS_AUTO_NONE)
bool flagShouldInstantlyConnect = false;
bool flagShouldInstantlySinglePlayer = false;
#endif

const int TILES_UNDER_UNIT_MAX = 64;
#define MIN_MINERAL_DISTANCE 200

#include "rtsCommon2.cpp"
typedef Vec2t<u16> Vec2Tile;
#define v2Tile(x, y) v2t<u16>((x), (y))

enum Direction {
	DIR_NONE=0,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP,
	DIR_DOWN,
};

enum UnitType {
	UNIT_NONE=0,
	UNIT_PROBE=1,
	UNIT_NEXUS=2,
	UNIT_MINERALS=3,

	UNIT_PYLON=4,
	UNIT_GATEWAY=5,
	UNIT_VEHICLE_PLANT=6,
	UNIT_PLASMA_GENERATOR=7,
	UNIT_PLASMA_BATTERY=8,
	UNIT_POWER_STATION=9,
	UNIT_ARMORY=10,
	UNIT_LAB=11,
	UNIT_SATCOM=12,
	UNIT_AUTO_GUN=13,
	UNIT_WATCH_TOWER=14,
	UNIT_AIR_PAD=15,

	UNIT_ZEALOT=16,
	UNIT_STALKER=17,
	UNIT_PANTHER=18,
	UNIT_HEMINE=19,
	UNIT_REPULSE_MINE=20,
	UNIT_SPIDER_DEPLOYER=21,
	UNIT_SPIDER_MINE=22,
	UNIT_JACK_RABBIT=23,
	UNIT_FLAME_ANTELOPE=24,
	UNIT_BEAVER=25,
	UNIT_WIDOW_TANK=26,
	UNIT_WARRIOR=27,
	UNIT_JACKAL=28,
	UNIT_FENNEC=29,
	UNIT_PYTHON=30,
	UNIT_COCKATIEL=31,
	UNIT_SUGAR_GLIDER=32,
	UNIT_FALCON=33,
	UNIT_OWL=34,
	UNIT_UNIT_B=35,
	UNIT_MINT_ZEALOT=36,
	UNIT_UNIT_D=37,
	UNIT_UNIT_E=38,
	UNIT_UNIT_F=39,
	UNIT_UNIT_G=40,
	UNIT_UNIT_H=41,
	UNIT_UNIT_I=42,
	UNIT_EMPTY_1=43,
	UNIT_EMPTY_2=44,
	UNIT_EMPTY_3=45,
	UNIT_EMPTY_4=46,
	UNIT_EMPTY_5=47,
	UNIT_EMPTY_6=48,
	UNIT_EMPTY_7=49,
	UNIT_EMPTY_8=50,
	UNIT_EMPTY_9=51,
	UNIT_EMPTY_10=52,
	UNIT_EMPTY_11=53,
	UNIT_EMPTY_12=54,
	UNIT_EMPTY_13=55,
	UNIT_EMPTY_14=56,
	UNIT_EMPTY_15=57,
	UNIT_EMPTY_16=58,
	UNIT_EMPTY_17=59,
	UNIT_EMPTY_18=60,
	UNIT_EMPTY_19=61,
	UNIT_EMPTY_20=62,
	UNIT_EMPTY_21=63,
	UNIT_EMPTY_22=64,
	UNIT_EMPTY_23=65,
	UNIT_EMPTY_24=66,
	UNIT_EMPTY_25=67,
	UNIT_EMPTY_26=68,
	UNIT_EMPTY_27=69,
	UNIT_EMPTY_28=70,
	UNIT_EMPTY_29=71,
	UNIT_EMPTY_30=72,
	UNIT_EMPTY_31=73,
	UNIT_EMPTY_32=74,
	UNIT_EMPTY_33=75,
	UNIT_EMPTY_34=76,
	UNIT_EMPTY_35=77,
	UNIT_EMPTY_36=78,
	UNIT_EMPTY_37=79,
	UNIT_EMPTY_38=80,
	UNIT_EMPTY_39=81,
	UNIT_EMPTY_40=82,
	UNIT_EMPTY_41=83,
	UNIT_EMPTY_42=84,
	UNIT_EMPTY_43=85,
	UNIT_EMPTY_44=86,
	UNIT_EMPTY_45=87,
	UNIT_EMPTY_46=88,
	UNIT_EMPTY_47=89,
	UNIT_EMPTY_48=90,
	UNIT_EMPTY_49=91,
	UNIT_EMPTY_50=92,
	UNIT_EMPTY_51=93,
	UNIT_EMPTY_52=94,
	UNIT_EMPTY_53=95,
	UNIT_EMPTY_54=96,
	UNIT_EMPTY_55=97,
	UNIT_EMPTY_56=98,
	UNIT_EMPTY_57=99,
	UNIT_EMPTY_58=100,
	UNIT_EMPTY_59=101,
	UNIT_EMPTY_60=102,
	UNIT_EMPTY_61=103,
	UNIT_EMPTY_62=104,
	UNIT_EMPTY_63=105,
	UNIT_EMPTY_64=106,
	UNIT_END,
};
const char *unitTypeStrings[] = {
	"None",
	"Probe",
	"Nexus",
	"Minerals",

	"Pylon", 
	"Gateway", 
	"VehiclePlant", 
	"PlasmaGenerator", 
	"PlasmaBattery", 
	"PowerStation", 
	"Armory", 
	"Lab", 
	"Satcom", 
	"AutoGun", 
	"WatchTower", 
	"AirPad", 

	"Zealot", 
	"Stalker", 
	"Panther", 
	"Hemine", 
	"RepulseMine", 
	"SpiderDeployer", 
	"SpiderMine", 
	"JackRabbit", 
	"FlameAntelope", 
	"Beaver", 
	"WidowTank", 
	"Warrior", 
	"Jackal",
	"Fennec",
	"Python",
	"Cockatiel",
	"SugarGlider",
	"Falcon",
	"Owl",
	"UNIT_UNIT_B",
	"MintZealot",
	"UNIT_UNIT_D",
	"UNIT_UNIT_E",
	"UNIT_UNIT_F",
	"UNIT_UNIT_G",
	"UNIT_UNIT_H",
	"UNIT_UNIT_I",
	"UNIT_EMPTY_1",
	"UNIT_EMPTY_2",
	"UNIT_EMPTY_3",
	"UNIT_EMPTY_4",
	"UNIT_EMPTY_5",
	"UNIT_EMPTY_6",
	"UNIT_EMPTY_7",
	"UNIT_EMPTY_8",
	"UNIT_EMPTY_9",
	"UNIT_EMPTY_10",
	"UNIT_EMPTY_11",
	"UNIT_EMPTY_12",
	"UNIT_EMPTY_13",
	"UNIT_EMPTY_14",
	"UNIT_EMPTY_15",
	"UNIT_EMPTY_16",
	"UNIT_EMPTY_17",
	"UNIT_EMPTY_18",
	"UNIT_EMPTY_19",
	"UNIT_EMPTY_20",
	"UNIT_EMPTY_21",
	"UNIT_EMPTY_22",
	"UNIT_EMPTY_23",
	"UNIT_EMPTY_24",
	"UNIT_EMPTY_25",
	"UNIT_EMPTY_26",
	"UNIT_EMPTY_27",
	"UNIT_EMPTY_28",
	"UNIT_EMPTY_29",
	"UNIT_EMPTY_30",
	"UNIT_EMPTY_31",
	"UNIT_EMPTY_32",
	"UNIT_EMPTY_33",
	"UNIT_EMPTY_34",
	"UNIT_EMPTY_35",
	"UNIT_EMPTY_36",
	"UNIT_EMPTY_37",
	"UNIT_EMPTY_38",
	"UNIT_EMPTY_39",
	"UNIT_EMPTY_40",
	"UNIT_EMPTY_41",
	"UNIT_EMPTY_42",
	"UNIT_EMPTY_43",
	"UNIT_EMPTY_44",
	"UNIT_EMPTY_45",
	"UNIT_EMPTY_46",
	"UNIT_EMPTY_47",
	"UNIT_EMPTY_48",
	"UNIT_EMPTY_49",
	"UNIT_EMPTY_50",
	"UNIT_EMPTY_51",
	"UNIT_EMPTY_52",
	"UNIT_EMPTY_53",
	"UNIT_EMPTY_54",
	"UNIT_EMPTY_55",
	"UNIT_EMPTY_56",
	"UNIT_EMPTY_57",
	"UNIT_EMPTY_58",
	"UNIT_EMPTY_59",
	"UNIT_EMPTY_60",
	"UNIT_EMPTY_61",
	"UNIT_EMPTY_62",
	"UNIT_EMPTY_63",
	"UNIT_EMPTY_64",
};

enum ActionType {
	ACTION_NONE,
	ACTION_IDLE2,
	ACTION_MOVE,
	ACTION_STOP,
	ACTION_ATTACK,
	ACTION_ATTACK_MOVE,
	ACTION_GATHER,
	ACTION_PLACE_UNIT,
	ACTION_CONSTRUCT,
	ACTION_EXPLODE,
	ACTION_AREA_SCAN,
	ACTION_SCAN_DART,
	ACTION_THROW_FIRE_BOMB,
	ACTION_DEATH,
};
const char *actionTypeStrings[] = {
	"None",
	"Idle2",
	"Move",
	"Stop",
	"Attack",
	"AttackMove",
	"Gather",
	"PlaceUnit",
	"Construct",
	"Explode",
	"AreaScan",
	"ScanDart",
	"FireBomb",
	"Death",
};

struct Action {
	ActionType type;
	Vec2 position;
	UnitType unitType;
	bool forceTarget;
	int targetId;
	int mineralPrice;
	int populationPrice;
	int mineralsToGain;
};

struct FlowField {
	char *data;
	int *dijkstraData;
	Vec2 targetTile;
	int size;
};

enum UnitState {
	UNIT_STATE_NORMAL,
	UNIT_STATE_CREATING,

	UNIT_STATE_GOING_TO_GATHER,
	UNIT_STATE_GATHERING,
	UNIT_STATE_RETURNING_RESOURCES,
	UNIT_STATE_ATTACK_APPROACH,
	UNIT_STATE_PRE_ATTACK,
	UNIT_STATE_POST_ATTACK,
};

struct UnitInfo;

struct Unit {
	UnitInfo *info;
	UnitType type;
	int id;
	int playerNumber;
	int teamNumber;
	float hp;
	bool canRemove;
	int slow;
	float heat;
	int mineralAmount; // UNIT_MINERALS only
	Vec2 position;
	Vec2 visualPosition;
	float rotation;
	Vec2 velo;
	Vec2 accel;
	Vec2 movementFacingVelo;

#define PREV_POSITIONS_MAX 2
	Vec2 prevPositions[PREV_POSITIONS_MAX];
	float speed;

	float visualRotation;
	char lastRadChar;
	float timeSinceLastRadChar;

	Rect rect;

	UnitState lastState;
	UnitState state;
	float stateTime;
	float attackTime;
	float postAttackCooldown;
	float existTime;
	float idleTime;
	float nextIdle2Timer;

#define BUILDING_QUEUE_MAX 5 //@incomplete Enforce this later
#define ACTIONS_MAX 64
	ActionType lastActionType;
	Action actions[ACTIONS_MAX];
	int actionsNum;
	float actionTime;
	float uniqueActionTime;
#define GATHERING_TIMER_MAX 2
	float gatheringTimer;
	Vec2 lastGatherPosition;
	bool firstFrameOfAction;
	int damagedBlinkAmount;

	Vec2 navGoal;
	Vec2 tileNavGoal;
	Vec2 seekTarget;
	FlowField *flowField;
	bool atLocation;
	bool unmovable;

	Vec2 rallyPoint;

#define UNITS_MINING_FROM_THIS_MAX 3
	int unitsMiningFromThis[UNITS_MINING_FROM_THIS_MAX];
	int unitsMiningFromThisNum;

	float timeSinceAttackAlert;
	bool selected;

	int scanDartLightId;
	Vec2 scanDartPosition;
	float scanDartRads;
	float scanDartLastShotTime;
	bool scanDartHitWall;

	bool givingTrueVision;
	bool connectedToPower;

	int lightId;
};

enum BulletType {
	BULLET_GENERIC_PROJECTILE,
	BULLET_HIT_SCAN,
};
const char *bulletTypeStrings[] = {
	"Generic projectile",
	"Hit scan",
};
struct Bullet {
	BulletType type;

	int srcId;
	int destId;

	Vec2 position;
	Vec2 destPosition;
	float speed;
	float damage;
	float slowDamage;
	float heatDamage;
};

enum OrderType {
	ORDER_NONE=0,
	ORDER_OPEN_BUILD_MENU,
	ORDER_OPEN_BEAVER_MENU,
	ORDER_PLACE,
	ORDER_CONSTRUCT,

	ORDER_ATTACK,
	ORDER_STOP,
	ORDER_HALT,
	ORDER_PATROL,

	ORDER_EXPLODE,
	ORDER_DEPLOY_SPIDERS,

	ORDER_AREA_SCAN,
	ORDER_SCAN_DART,
	ORDER_THROW_FIRE_BOMB,
};
char *orderTypeStrings[] = {
	"ORDER_NONE",
	"ORDER_OPEN_BUILD_MENU",
	"ORDER_OPEN_BEAVER_MENU",
	"ORDER_PLACE",
	"ORDER_CONSTRUCT",

	"ORDER_ATTACK",
	"ORDER_STOP",
	"ORDER_HALT",
	"ORDER_PATROL",

	"Explode",
	"Deploy spiders",
	"Area scan",
	"Scan dart",
	"Fire bomb",
};
struct Order {
	OrderType type;
	char key;

	UnitType unitType;
};
struct UnitInfo {
#define UNIT_NAME_MAX_LEN 32
	char name[UNIT_NAME_MAX_LEN];

#define FORMAL_NAME_MAX_LEN 32
	char formalName[FORMAL_NAME_MAX_LEN];

	BulletType bulletType;
	int size;
	int speed;
	int range;
	float maxHp;
	float damage;
	float slowDamage;
	float heatDamage;
	float creationTime;

	bool isBuilding;
	int mineralPrice;
	int population;
	float bulletSpeed;
	int visionRadius;
	int aggroRadius;

	float preAttackDelay;
	float postAttackDelay;

	Rect spriteTransform;
	float resMulti;
	bool stealth;
	bool noGroundAttack;
	bool noAirAttack;
	bool flying;
	bool noBoids;
	bool alwaysAbleToBoxSelect;
	bool hideRally;

	Vec2 idle2Timer;
	float idle2Length;
	float deathLength;

	float walkSpeedMulti;

#define UNIT_DESCRIPTION_MAX_LEN 256
	char description[UNIT_DESCRIPTION_MAX_LEN];

	/// Unserialized

#define ORDER_MAX 32
	Order orders[ORDER_MAX];
	int ordersNum;
};

enum PropGenerationType {
	PROP_GENERATION_CIRCLE=0,
	PROP_GENERATION_POISSON_CHEAT=1,
};
const char *propGenerationTypeStrings[] = {
	"Circle",
	"Poisson cheat",
};

enum MapPropType {
	MAP_PROP_IMAGE=0,
	MAP_PROP_UNIT_SPAWN=1,
};
const char *mapPropTypeStrings[] = {
	"Image",
	"Unit spawn",
};
struct MapProp {
	int id;
#define MAP_PROP_NAME_MAX_LEN 32
	char name[MAP_PROP_NAME_MAX_LEN];
	MapPropType type;
	int parent;
	bool coopOnly;
	bool nonCoopOnly;

	UnitType unitType;
	int playerNumber;

	char imagePath[PATH_MAX_LEN];
	Vec2 position;
	Vec2 size;
	float rotation;
	int tint;
	float transparency;
	bool generatesCollision;
	bool editorOnly;

	int modColorShiftColor;
	Vec2 modColorShiftPower;
	Vec2 modRotationBump;
	Vec2 modTransparencyBump;
	Vec2 modScaleBump;
	int modSeed;
	bool childrenInheritMods;

	int generatesProps;
	PropGenerationType generationType;
	float generationRadius;

	int emitsLight;

	/// Unserialized
	float time;
	bool wasGenerated;
	Matrix3 matrix;
	Matrix3 screenSpaceMatrix;

	bool beenModdedByParent;
	float colorShiftPower;
	float rotationBump;
	float transparencyBump;
	float scaleBump;
	float timeTillNextEmit;
	Vec2 center;

	int lightIds[TEAMS_PER_MATCH_MAX];
};
struct Map {
#define MAP_NAME_MAX_LEN 32
	char name[MAP_NAME_MAX_LEN];

	float minLight;
	float radianceMulti;
	int backgroundColor;
	float perlinFreq;
	Vec2 perlinScroll;
	int perlinColor;

	float overlayScale;

#define MAP_PROPS_MAX 4096
	MapProp props[MAP_PROPS_MAX];
	int propsNum;
	int nextPropId;

#define MAP_WIDTH_MAX 256
#define MAP_HEIGHT_MAX 256
	int collisionData[MAP_WIDTH_MAX * MAP_HEIGHT_MAX];

	int width;
	int height;

	Vec3 sunPosition;

	bool enableCoop;
	bool enable1v1;
};

enum EditorMode {
	EDITOR_NONE,
	EDITOR_NORMAL,
	EDITOR_COLLISION,
};
enum GameState {
	GAME_STATE_LOADING,
	GAME_STATE_LOG_IN,
	GAME_STATE_MAIN_MENU,
	GAME_STATE_CONFIGURE_SERVER,
	GAME_STATE_LOBBY_PENDING,
	GAME_STATE_LOBBY_JOIN_PENDING,
	GAME_STATE_LOBBY,
	GAME_STATE_IN_QUEUE,
	GAME_STATE_REPLAY_BROWSER,
	GAME_STATE_PLAYING,
	GAME_STATE_MATCH_SUMMERY,
};
enum InterfaceMode {
	INTERFACE_NORMAL,
	INTERFACE_PICKING_BUILDING,
	INTERFACE_PLACING_BUILDING,
	INTERFACE_ATTACK_MOVE,
};

enum ServerConnectionState {
	SERVER_DISCONNECTED=0,
	SERVER_CONNECTING,
	SERVER_CONNECTED,
	SERVER_IN_QUEUE,
	SERVER_IN_MATCH,
};

enum EffectType {
	EFFECT_NONE=0,
	EFFECT_UNIT_SELECTED,
	EFFECT_UNIT_TARGETED,
	EFFECT_ISSUED_MOVE,
	EFFECT_ISSUED_ATTACK_MOVE,
	EFFECT_UNIT_DEATH,
	EFFECT_START_UNIT_CONSTRUCTION,
	EFFECT_UNIT_FULLY_CONSTRUCTED,
	EFFECT_HEMINE_EXPLODE,
};
struct Effect {
	EffectType type;
	Vec2 position;
	int unitId;

	float time;
};

struct Options {
};

#define UNITS_MAX 1024
// #define UNITS_MAX 1024
struct ControlGroup {
	int ids[UNITS_MAX];
	int idsNum;
};

enum AlertType {
	ALERT_NONE,
	ALERT_TEST_ALERT,
	ALERT_TEST_ALERT_2,
	ALERT_FOUND_AN_ENEMY_BASE,
	ALERT_UNIT_UNDER_ATTACK,
	ALERT_BUILDING_UNDER_ATTACK,
	ALERT_PROBES_UNDER_ATTACK,
	ALERT_OUT_OF_MINERALS,
	ALERT_HEMINE_EXPLODE,
	ALERT_DEFEND_YOUR_BASE,
	ALERT_DESTROY_EVERYTHING,
	ALERT_SAY,
	ALERT_SAY_DIALOG,
};
const char *alertTypeStrings[] = {
	"ALERT_NONE", 
	"ALERT_TEST_ALERT", 
	"ALERT_TEST_ALERT_2", 
	"ALERT_FOUND_AN_ENEMY_BASE", 
	"ALERT_UNIT_UNDER_ATTACK", 
	"ALERT_BUILDING_UNDER_ATTACK", 
	"ALERT_PROBES_UNDER_ATTACK", 
	"ALERT_OUT_OF_MINERALS", 
	"ALERT_HEMINE_EXPLODE", 
	"ALERT_DEFEND_YOUR_BASE", 
	"ALERT_DESTROY_EVERYTHING", 
	"ALERT_SAY_DIALOG", 
};

struct Dialog {
	AlertType alertType;
#define DIALOG_NAME_MAX_LEN 64
	char name[DIALOG_NAME_MAX_LEN];
#define DIALOG_TEXT_MAX_LEN 512
	char text[DIALOG_TEXT_MAX_LEN];

	UnitType unitType;
};

struct Alert {
#define ALERT_CUSTOM_MSG_MAX_LEN 128
	char customMsg[ALERT_CUSTOM_MSG_MAX_LEN];
	AlertType type;
	int playerNumber;

	Vec2 position;
	int unitId;
	float time;

	bool visualDone;
	bool isOnlyOne;
	char *frameStr;

	Dialog *dialog;
};

struct FogUnit {
	bool onScreen;
	Frame *frame;
	Rect rect;
	Matrix3 matrix;
	Matrix3 uvMatrix;
	RenderProps props;
};

enum PlayMode {
	PLAY_NONE,
	PLAY_1V1_ONLINE,
	PLAY_REPLAY,
	PLAY_SINGLE_PLAYER,
	PLAY_MISSION,
};

struct Globals {
	float spiderMineLifeTime;
	int spiderMineDeployCount;
	bool doFxaa;
};

struct EditorConfig {
	bool onlyShowOrphanMapProps;
	bool showParent;
	bool hideGeneratedFromScreen;
	bool hideGeneratedFromList;
	bool compressSpriteSheets;
	bool editorOnlyFlash;
	bool noShadowsInEditor;
};

enum PingChartType {
	PING_CHART_LOGIN_UDP_SEND,
	PING_CHART_LOGIN_UDP_RECV,
	PING_CHART_LOGIN_TCP_SEND,
	PING_CHART_LOGIN_TCP_RECV,
	PING_CHART_SESSION_UDP_SEND,
	PING_CHART_SESSION_UDP_RECV,
	PING_CHART_SESSION_TCP_RECV,
	PING_CHART_SESSION_TCP_SEND,
	PING_CHART_END,
};
struct PingChart {
#define PING_CHART_TIMES_MAX 120
	float times[PING_CHART_TIMES_MAX];
	int nextTime;
	float lastTime;
	NanoTime lastNanoTime;

	float min;
	float average;
	float max;
	float highest;
	char *title;
};

struct SpatialBin {
	Rect region;
	Unit *units[UNITS_MAX];
	int unitsNum;
};

struct FireBomb {
	Vec2 start;
	Vec2 end;
	Vec2 degMinMax;
	float time;
};

struct FirePuddle {
	Vec2 worldPosition;
	float worldRadius;

	Vec2Tile *tiles;
	int tilesNum;
	float time;
};

struct Sim {
#define FIRE_BOMBS_MAX 256
	FireBomb fireBombs[FIRE_BOMBS_MAX];
	int fireBombsNum;

#define FIRE_PUDDLES_MAX 256
	FirePuddle firePuddles[FIRE_PUDDLES_MAX];
	int firePuddlesNum;
};

struct LightCasterRef {
	int id;
	int teamNumber;
	Vec2Tile position;
	int radius;
	int value;
	bool isFlying;
};

struct Game {
	bool debugSkipGamePass;
	bool debugSkipWorldPass;
	bool debugShowDijkstraGrid;
	bool debugShowFlowField;
	bool debugShowPathfindingData;
	bool debugShowLightNumbers;
	bool debugHideMinimap;
	bool debugDisableFogOfWar;
	bool debugAlwaysShowCollisionData;
	bool debugShowBuildGrid;
	bool debugShowAttackRange;
	Rect debugViewport;
	float _debugPacketDropRate;
	float _debugPacketDropRateIn;
	int _debugSenderLag;
	int debugGpuStress;
	bool debugShowUnitInfo;
	Texture *debugNormalTexture;
	bool debugLiveUpdatePerlin;
	bool debugShowVisionRefs;
	int debugFrameIter;

	char debugReplayFileName[PATH_MAX_LEN];

	Globals globals;

	char prevMusic[PATH_MAX_LEN];
	char currentMusic[PATH_MAX_LEN];
	int musicChannel;
	float musicFadePerc;

	/// Menu
	ServerConfig serverConfig;

	char serverIp[PATH_MAX_LEN];
	int serverPort;
	char **replayPaths;
	int replayPathsNum;
#define MAPS_MAX 64
	char *mapPaths[MAPS_MAX];
	int mapPathsNum;
	Map maps[MAPS_MAX];

	Lobby lobby;
	char lobbyHash[LOBBY_HASH_MAX_LEN];
	Map lobbySelectedMap;

	PlayMode playMode;
	Vec2 playerBaseStarts[PLAYERS_PER_MATCH_MAX];
	bool replayPaused;

	GameCmd *gameCmdHistory;
	int gameCmdHistoryNum;
	int gameCmdHistoryMax;

	GameCmd *gameCmdsReplay;
	int gameCmdsReplayNum;
	int gameCmdsReplayMax;

	GameCmd gameCmds[GAME_CMDS_MAX];
	int gameCmdsNum;

#define TEXTURE_STORE_MAX 128
	char textureStoreNames[TEXTURE_STORE_MAX][PATH_MAX_LEN];
	int textureStoreNamesNum;
	Texture *textureStore[TEXTURE_STORE_MAX];
	int textureStoreNum;

// #define SOUND_STORE_MAX 128
// 	char soundStoreNames[SOUND_STORE_MAX][PATH_MAX_LEN];
// 	int soundStoreNamesNum;
// 	Sound *soundStore[SOUND_STORE_MAX];
// 	int soundStoreNum;

	/// Editor
	EditorMode editorMode;
	int selectedPropId;
	Vec2 propDragPivot;
	bool draggingProp;
	bool addingCollision;
	bool attaching;
	int mapEditorWidth;
	int mapEditorHeight;

	EditorConfig editorConfig;
	void *unitInfoEditor;
	void *particleEditorData;

	/// Game
	Texture *tileTexture;
	Texture *tempTexture;
	Texture *tempTexture64;
	Texture *tempTexture256;
	Texture *debugTexture;
	Texture *statusCardTexture;
	Texture *circle64;
	Font *defaultFont;
	Font *smallOrderFont;
	Font *bigOrderFont;
	Font *smallFont;
	Font *smallerFont;
	Font *hugeFont;
	Font *bigFont;

	float width;
	float height;
	float scale;
	float prevScale;

	Texture *minimapTexture;
	bool draggingMinimap;

	char currentMapPath[PATH_MAX_LEN];
	Map map;
	bool doGenerators;
	Texture *mapBg;
	float mapBgRenderScale;

	int framesPerTurn;
	Vec2 cameraTarget;
	Vec2 visualCameraTarget;
	float zoom;
	float visualZoom;
	int wave;
	int prevWave;
	float prevWaveTime;
	float waveTime;

	Vec2 camBoundsMin;
	Vec2 camBoundsMax;
	float forceZoom;

	GameState state;
	bool wonGame;

	/// Networking
	int clientId;
	int matchId;
	float timeTillRetryConnection;

	ThreadSafeQueue *_loginPacketsOut;
	ThreadSafeQueue *_loginPacketsIn;
	ThreadSafeQueue *sessionPacketsIn;
	ThreadSafeQueue *sessionPacketsOut;

	Socket *_udpLoginSocket;
	Socket *_tcpLoginSocket;
	volatile bool _shouldBeLoggedIn;
	volatile bool _supportsUdp;
	Socket *udpSessionSocket;
	sockaddr_storage udpSessionAddr;
	Socket *tcpSessionSocket;
	ServerConnectionState serverConnectionState;

	sockaddr_storage _loginServerAddr;

#define PING_CHARTS_MAX (PING_CHART_END)
	PingChart pingCharts[PING_CHARTS_MAX];

	volatile int _confirmedTurn;
	volatile float _timeSinceLastContactedLoginServer;
	float timeSinceLastSessionServerContact;
	float timeSinceConfirmTurnUpdate;

	/// /Networking

	Texture *gameTexture;
	float gameSpeed;
	float time;

	int playerNumber;
	int teamNumber;
	int npcPlayerNumber;
	bool firstFrameOfMatch;
	bool inGameMenu;
	float timeoutTimeLeft;

	UnitType unitTypeCurrentlyPlacing;
	Rect unitCurrentlyPlacingRect;
	InterfaceMode interfaceMode;

	Vec2 mouseDownPosition;
	Rect boxSelectRect;

	int matchRndSeed;
	int turn;
	int framesThisTurn;
	bool firstFrameOfTurn;
	bool missingTurnData;
	bool turnEnded;
	bool isDesynced;

#define BULLETS_MAX 128
	Bullet bullets[BULLETS_MAX];
	int bulletsNum;

#define UNIT_INFOS_MAX ((UNIT_END)-1) // I think this should not have the -1    1/28/2021
	UnitInfo unitInfos[UNIT_INFOS_MAX];

	Unit units[UNITS_MAX];
	int unitsNum;
	int nextUnitId;

#define CONTROL_GROUPS_MAX 10
	ControlGroup controlGroups[PLAYERS_PER_MATCH_MAX][CONTROL_GROUPS_MAX];
	ControlGroup localControlGroups[CONTROL_GROUPS_MAX];

	ControlGroup selectedUnits[PLAYERS_PER_MATCH_MAX];
	ControlGroup localSelectedUnits;

	int lastControlGroupSelected;
	float timeSinceLastControlGroupSelected;

	int minerals[PLAYERS_PER_MATCH_MAX];
	float visualMineral[PLAYERS_PER_MATCH_MAX];
	int population[PLAYERS_PER_MATCH_MAX];
	int startingBaseIds[TEAMS_PER_MATCH_MAX];

	int *collisionData;
	bool collisionHasChanged;

#define LIGHT_CASTER_REFS_MAX ((UNITS_MAX) + 1024)
	LightCasterRef lightCasterRefs[LIGHT_CASTER_REFS_MAX];
	int lightCasterRefsNum;
	int lightCasterRefNextId;
	int *visionRefs[PLAYERS_PER_MATCH_MAX];
	int *visionLightingRefs[PLAYERS_PER_MATCH_MAX];

#define FLOW_FIELDS_MAX 32
	FlowField flowFields[FLOW_FIELDS_MAX];

#define EFFECTS_MAX 512
	Effect effects[EFFECTS_MAX];
	int effectsNum;

	Options options;
	char currentWorldHash;

	struct UnitPastData {
		int id;
		Vec2 position;
	};

	struct PastTurnData {
		UnitPastData unitDatas[UNITS_MAX];
		int unitDatasNum;
	};

#define PAST_TURN_DATA_MAX 3
	PastTurnData pastTurnData[PAST_TURN_DATA_MAX];

#define ALERTS_MAX 8
	Alert alerts[ALERTS_MAX];
	int alertsNum;
	bool onlyOneAlert;

#define BUILDINGS_PER_PLAYER_MAX 256
	FogUnit fogUnits[BUILDINGS_PER_PLAYER_MAX];
	int fogUnitsNum;

	int populationCounts[PLAYERS_PER_MATCH_MAX];
	int negOnePopulationCount;

#define DIALOGS_MAX 128
	Dialog dialogs[DIALOGS_MAX];

	Texture *visionBitTexture;
	Texture *lightMap;
	float *lightFloatData;

#define WORLD_CHANNELS_MAX CHANNELS_MAX
	int worldChannels[WORLD_CHANNELS_MAX];
	int worldChannelsNum;

	Matrix3 visualCameraMatrix;
	float orderWheelBigMode;
	float orderWheelBigModeTarget;

#define BINS_MAX 256
	SpatialBin spatialBins[BINS_MAX];
	int spatialBinsNum;

	char *tooltip;

	Sim sim;
};

void runGame();
void updateGame();

void saveMap();

GameCmd *createGameCmd();
Action *createAction(Unit *unit, bool queues);
Action *injectAction(Unit *unit, int afterIndex);

Unit *getUnit(int id);

MapProp *getMapProp(int id);
MapProp *getMapPropByName(const char *name);
Matrix3 getMapPropMatrix(MapProp *mapProp);
MapProp *duplicateMapProp(MapProp *toDup, bool wasGenerated);
void deleteMapProp(MapProp *toDelete);

Unit *createUnit(int playerNumber, int teamNumber, UnitType type);
bool hasPopFor(int playerNumber, UnitType type);
void dealDamage(Unit *dest, Unit *src, float amount, Vec2 damagePosition=v2());

Bullet *createBullet(Unit *src, Unit *dest);

bool hasLineOfSight(Vec2Tile start, Vec2Tile goal, bool forVision);
bool hasLineOfSightThick(int *tiles, int tilesWide, int tilesHigh, Vec2Tile start, Vec2Tile goal, int thickness);
Vec2 getClosestReachableNode(int *tiles, int tilesWide, int tilesHigh, Vec2 start, Vec2 goal);
void endPathfinding(Unit *unit);
void stepPathfinding(Unit *unit, Vec2 navGoal);

void createRightClickAction(Unit *unit, Vec2 position, bool queues);
void processClickSelect(int playerNumber, int unitId, bool appends, bool local);
void processSpliceSelection(int playerNumber, int unitIndex, bool local);
void processTimeWarpBoxSelect(int playerNumber, Rect box, bool appends, UnitType onlyType, int turnsBack, bool local);
void doPassiveAggro(Unit *unit);

Unit *getClosestUnit(ControlGroup *group, Vec2 position, UnitType type, ActionType filterActionType=ACTION_NONE);
Unit *getFirstUnit(ControlGroup *group);
void removeFromControlGroup(ControlGroup *group, int unitId);
void removeFromAllControlGroups(int unitId, bool local);
void addToControlGroup(ControlGroup *group, int id);
void replaceControlGroup(ControlGroup *dest, ControlGroup *src);

void generateDijkstraGrid(int *tiles, int tilesWide, int tilesHigh, Vec2 start, int size, int *outGrid);
void generateFlowField(int *tiles, int tilesWide, int tilesHigh, Vec2 start, int *dijkstraGrid, char *outGrid);
int canSeeUnit(int teamNumber, Unit *unit);
int canSeeTile(int teamNumber, Vec2Tile tile);
int canSeeTile(int teamNumber, int tileIndex);
int canSeeTile(int teamNumber, u8 x, u8 y);
int canSeeRect(int teamNumber, Rect rect);
int getTilesUnderUnitType(UnitType type, Rect rect, Vec2Tile *tiles, int tilesMax);
int getTilesUnderRect(Rect rect, Vec2Tile *tiles, int tilesMax);
int getTilesUnderCircle(Vec2 position, float radius, Vec2Tile *tiles, int tilesMax);
int getOutlineTilesUnderRect(Rect rect, Vec2Tile *tiles, int tilesMax);
Unit **getUnitsInCircle(Vec2 position, float range, int *unitsNum);
void recountPopulation();

void drawGhostImage(UnitType unitType, Vec2 position, float angleDeg);

Rect getUnitRect(UnitType type, Vec2 position);
Vec2 worldToTile(Vec2 point);
Vec2 tileToWorld(Vec2 tile);
Vec2 tileToWorld(Vec2Tile tile);
Vec2Tile worldToVec2Tile(Vec2 point);
Rect tileToWorldRect(Vec2 tile);
Rect tileToWorldRect(Vec2Tile tile);
char radToChar(float rad, bool *shouldFlip);
float charToRad(char rad);

char degToChar(float deg);
float charToDeg(char ch);

int createLight(int teamNumber);
void destroyLight(int lightId);
void propagateLight(int lightId, Vec2Tile position, int radius, int value);
LightCasterRef *getLightCasterRef(int lightId);
void modifyVisionRefs(int teamNumber, Vec2Tile position, int radius, int value, bool isFlying);

Effect *createEffect(EffectType type, Vec2 position);
Effect *createEffect(EffectType type, int unitId);
Alert *createAlert(AlertType type, Unit *unit, const char *customMsg=NULL);
Alert *createAlert(AlertType type, int playerNumber, Vec2 position, const char *customMsg=NULL);

int playWorldSound(const char *name, Vec2 position);
int playGameSound(const char *name);
int playGameMusic(const char *name);
void stepPingChart(PingChartType type);
MsgPacket createMsgPacket(NetMsgType msgType);

void updateLoginThread(void *threadParam);
void updateSim();

void loadSprites();
void packOtherUnitInfo();

void gameCleanup();

struct BitGrid {
	bool *data;
	int size;
};

BitGrid *createBitGrid(int size);
void setBit(BitGrid *bitGrid, int index, bool value);
bool getBit(BitGrid *bitGrid, int index);
void destroyBitGrid(BitGrid *bitGrid);
BitGrid *createBitGrid(int size) {
	BitGrid *bitGrid = (BitGrid *)frameMalloc(sizeof(BitGrid));
	bitGrid->size = size;
	bitGrid->data = (bool *)frameMalloc(sizeof(bool) * bitGrid->size);
	return bitGrid;
}

void setBit(BitGrid *bitGrid, int index, bool value) {
	if (index > bitGrid->size) logf("Bit grid too big\n");
	bitGrid->data[index] = value;
}

bool getBit(BitGrid *bitGrid, int index) {
	if (index > bitGrid->size) logf("Bit grid too big\n");
	return bitGrid->data[index];
}

// void destroyBitGrid(BitGrid *bitGrid) {
// 	free(bitGrid->data);
// 	free(bitGrid);
// }

/// FUNCTIONS ^

Game *game = NULL;
int tileSize = 32;
unsigned int playerColors[] = {0x880000FF, 0x88FF0000};
unsigned int playerMinimapColors[] = {0xFF4444FF, 0xFFFF0000};
int mineralAmountMax = 2000;

void runGame() {
	// Assert(PAST_TURN_DATA_MAX == turnDelay);
	startingFrameMemory = Megabytes(20);

#if defined(_WIN32)
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameA(hModule, exeDir, PATH_MAX_LEN);

	char *lastSlash = strrchr(exeDir, '\\');
	if (!lastSlash) Panic("No last slash found in exe path");
	*lastSlash = 0;

#if defined(FALLOW_INTERNAL) // This needs to be a macro
		if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/rtsGame/rtsClientAssets");
		if (directoryExists("C:/Users/Dog_Spacey/Dropbox")) strcpy(projectAssetDir, "C:/Users/Dog_Spacey/Dropbox/rtsGame/rtsClientAssets");
		if (directoryExists("C:/Users/i am the sun king/Dropbox")) strcpy(projectAssetDir, "C:/Users/i am the sun king/Dropbox/rtsGame/rtsClientAssets");
		if (directoryExists("/media/sf_Dropbox")) strcpy(projectAssetDir, "/media/sf_Dropbox/rtsGame/rtsClientAssets");
#else
		snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#endif

#endif

	initFileOperations();

#if defined(FALLOW_INTERNAL) // This needs to be a macro
	useGuiIni = true;
#endif

	initPlatform(1280, 720, "rtsClient");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	initTextureSystem();
	initFonts();

	bool good = initNetworking();
	if (!good) Panic("Failed to init networking");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	// {
	// 	static NanoTime nanoTime;

	// 	NanoTime now = getNanoTime();
	// 	float ms = getMsPassed(&nanoTime, &now);
	// 	nanoTime = now;

	// 	logf("%f passed\n", ms);
	// }

	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		logf("Game is %.2fmb btw\n", (float)sizeof(Game)/(float)Megabytes(1));
		logf("Unit is %.2fmb btw\n", (float)sizeof(Unit)/(float)Megabytes(1));
		platform->atExitCallback = gameCleanup;

		for (int i = 0; i < 50; i++) logf("Remember pushCamera2d() has changed.\n");

		{
			RegMem(MapProp, id);
			RegMem(MapProp, name);
			RegMem(MapProp, type);
			RegMem(MapProp, parent);
			RegMem(MapProp, coopOnly);
			RegMem(MapProp, nonCoopOnly);
			RegMem(MapProp, unitType);
			RegMem(MapProp, playerNumber);
			RegMem(MapProp, imagePath);
			RegMem(MapProp, position);
			RegMem(MapProp, size);
			RegMem(MapProp, rotation);
			RegMem(MapProp, tint);
			RegMem(MapProp, transparency);
			RegMem(MapProp, generatesCollision);
			RegMem(MapProp, editorOnly);
			RegMem(MapProp, modColorShiftPower);
			RegMem(MapProp, modColorShiftColor);
			RegMem(MapProp, modRotationBump);
			RegMem(MapProp, modTransparencyBump);
			RegMem(MapProp, modScaleBump);
			RegMem(MapProp, modSeed);
			RegMem(MapProp, childrenInheritMods);
			RegMem(MapProp, generatesProps);
			RegMem(MapProp, generationType);
			RegMem(MapProp, generationRadius);
			RegMem(MapProp, emitsLight);

			RegMem(Map, name);
			RegMem(Map, minLight);
			RegMem(Map, radianceMulti);
			RegMem(Map, backgroundColor);
			RegMem(Map, perlinFreq);
			RegMem(Map, perlinScroll);
			RegMem(Map, perlinColor);
			RegMem(Map, overlayScale);
			RegMem(Map, propsNum);
			RegMem(Map, nextPropId);
			RegMem(Map, collisionData);
			RegMem(Map, width);
			RegMem(Map, height);
			RegMem(Map, sunPosition);
			RegMem(Map, enableCoop);
			RegMem(Map, enable1v1);

			RegMem(UnitInfo, name);
			RegMem(UnitInfo, formalName);
			RegMem(UnitInfo, bulletType);
			RegMem(UnitInfo, size);
			RegMem(UnitInfo, speed);
			RegMem(UnitInfo, range);
			RegMem(UnitInfo, maxHp);
			RegMem(UnitInfo, damage);
			RegMem(UnitInfo, slowDamage);
			RegMem(UnitInfo, heatDamage);
			RegMem(UnitInfo, isBuilding);
			RegMem(UnitInfo, mineralPrice);
			RegMem(UnitInfo, population);
			RegMem(UnitInfo, bulletSpeed);
			RegMem(UnitInfo, visionRadius);
			RegMem(UnitInfo, aggroRadius);
			RegMem(UnitInfo, preAttackDelay);
			RegMem(UnitInfo, postAttackDelay);
			RegMem(UnitInfo, creationTime);
			RegMem(UnitInfo, spriteTransform);
			RegMem(UnitInfo, resMulti);
			RegMem(UnitInfo, stealth);
			RegMem(UnitInfo, noGroundAttack);
			RegMem(UnitInfo, noAirAttack);
			RegMem(UnitInfo, flying);
			RegMem(UnitInfo, noBoids);
			RegMem(UnitInfo, alwaysAbleToBoxSelect);
			RegMem(UnitInfo, hideRally);
			RegMem(UnitInfo, idle2Timer);
			RegMem(UnitInfo, idle2Length);
			RegMem(UnitInfo, deathLength);
			RegMem(UnitInfo, walkSpeedMulti);
			RegMem(UnitInfo, description);

			RegMem(Globals, spiderMineLifeTime);
			RegMem(Globals, spiderMineDeployCount);
			RegMem(Globals, doFxaa);

			RegMem(EditorConfig, onlyShowOrphanMapProps);
			RegMem(EditorConfig, showParent);
			RegMem(EditorConfig, hideGeneratedFromScreen);
			RegMem(EditorConfig, hideGeneratedFromList);
			RegMem(EditorConfig, compressSpriteSheets);
			RegMem(EditorConfig, editorOnlyFlash);
			RegMem(EditorConfig, noShadowsInEditor);

			RegMem(Dialog, alertType);
			RegMem(Dialog, name);
			RegMem(Dialog, text);
			RegMem(Dialog, unitType);

			regMemGameCmd();
		}

		loadStruct("EditorConfig", "assets/info/editorConfig.txt", &game->editorConfig);

		if (platform->isInternalVersion) {
#if !defined(__EMSCRIPTEN__)
			for (int i = 0; i < assetPathsNum; i++) {
				char *path = assetPaths[i];
				if (strContains(path, ".albedo")) {
					char *finalPath = frameMalloc(PATH_MAX_LEN);
					strcpy(finalPath, path);
					char *lastDot = strrchr(finalPath, '.');
					*lastDot = 0;
					strcat(finalPath, ".png");

					if (!fileExists(finalPath) || isFirstPathNewer(path, finalPath)) {
						logf("Generating normals for %s\n", path);
						logf("RIP generate2dNormals\n", path);
						// generate2dNormals(path);
					}
				}
			}
#endif
		}

		game->tileTexture = createTexture(32, 32);
		game->tempTexture = createTexture(renderer->width, renderer->height);
		game->tempTexture64 = createTexture(64, 64);
		game->tempTexture256 = createTexture(256, 256);
		if (platform->isDebugVersion) game->debugTexture = createTexture(renderer->width, renderer->height);

		{
			game->circle64 = createTexture(64, 64);
			pushTargetTexture(game->circle64);
			pushCamera2d(mat3());
			// clearRenderer(0xFFFF0000);
			drawCircle(v2(32, 32), 32, 0xFFFFFFFF);
			popCamera2d();
			popTargetTexture();
		}

		game->defaultFont = createFont("assets/fonts/arial.ttf", 24);
		game->smallOrderFont = createFont("assets/fonts/arial.ttf", 18);
		game->bigOrderFont = createFont("assets/fonts/arial.ttf", 32);
		game->smallFont = createFont("assets/fonts/arial.ttf", 12);
		game->smallerFont = createFont("assets/fonts/arial.ttf", 10);
		game->bigFont = createFont("assets/fonts/arial.ttf", 32);
		game->hugeFont = createFont("assets/fonts/arial.ttf", 64);
		game->gameSpeed = 1;
		if (!platform->isInternalVersion) {
			flagShouldInstantlyConnect = false;
			flagShouldInstantlySinglePlayer = false;
		}
		game->gameCmdHistoryMax = 10 * 60 * 60 / minFramesPerTurn * GAME_CMDS_MAX;
		game->gameCmdHistory = (GameCmd *)zalloc(game->gameCmdHistoryMax * sizeof(GameCmd));

		strcpy(game->currentMapPath, "assets/maps/desert");
		if (platform->isInternalVersion) strcpy(game->currentMapPath, "assets/maps/test/simpleFight");
		// if (platform->isInternalVersion) strcpy(game->currentMapPath, "assets/maps/TinyTest");
		// if (platform->isInternalVersion) strcpy(game->currentMapPath, "assets/maps/hugeFight");
		// if (platform->isInternalVersion) strcpy(game->currentMapPath, "assets/maps/mission0");

		if (platform->isDebugVersion) {
			strcpy(game->serverIp, "127.0.0.1");
		} else {
			strcpy(game->serverIp, "184.72.9.156");
		}
		game->serverPort = 54029;

#if defined(__EMSCRIPTEN__)
		game->_supportsUdp = false;
		game->serverPort = 54031; // This is dumb but works
		strcpy(game->serverIp, "184.72.9.156");
#else
		game->_supportsUdp = true;
#endif
		// game->_supportsUdp = false; logf("//@delete\n");

		loadStructArray("UnitInfo", "assets/info/unitInfo.txt", game->unitInfos, UNIT_INFOS_MAX, sizeof(UnitInfo));
		loadStruct("Globals", "assets/info/globals.txt", &game->globals);
		// loadStruct("EditorConfig", "assets/info/editorConfig.txt", &game->editorConfig); // This happens earlier
		{ /// Unit info
			UnitInfo *info;

			info = &game->unitInfos[UNIT_PROBE];
			info->orders[0].type = ORDER_OPEN_BUILD_MENU;
			info->orders[0].key = 'B';
			info->ordersNum = 1;

			info = &game->unitInfos[UNIT_NEXUS];
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_PROBE;
			info->orders[info->ordersNum++].key = 'E';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_BEAVER;
			info->orders[info->ordersNum++].key = 'B';

			info = &game->unitInfos[UNIT_MINERALS];

			info = &game->unitInfos[UNIT_PYLON];

			info = &game->unitInfos[UNIT_GATEWAY];
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_ZEALOT;
			info->orders[info->ordersNum++].key = 'Z';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_STALKER;
			info->orders[info->ordersNum++].key = 'S';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_JACK_RABBIT;
			info->orders[info->ordersNum++].key = 'R';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_FLAME_ANTELOPE;
			info->orders[info->ordersNum++].key = 'F';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_WARRIOR;
			info->orders[info->ordersNum++].key = 'W';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_JACKAL;
			info->orders[info->ordersNum++].key = 'A';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_MINT_ZEALOT;
			info->orders[info->ordersNum++].key = 'M';

			info = &game->unitInfos[UNIT_BEAVER];
			info->orders[0].type = ORDER_OPEN_BEAVER_MENU;
			info->orders[0].key = 'B';
			info->ordersNum = 1;


			info = &game->unitInfos[UNIT_VEHICLE_PLANT];
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_PANTHER;
			info->orders[info->ordersNum++].key = 'A';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_WIDOW_TANK;
			info->orders[info->ordersNum++].key = 'W';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_FENNEC;
			info->orders[info->ordersNum++].key = 'F';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_PYTHON;
			info->orders[info->ordersNum++].key = 'T';

			info = &game->unitInfos[UNIT_AIR_PAD];
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_COCKATIEL;
			info->orders[info->ordersNum++].key = 'C';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_SUGAR_GLIDER;
			info->orders[info->ordersNum++].key = 'S';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_FALCON;
			info->orders[info->ordersNum++].key = 'F';

			info = &game->unitInfos[UNIT_ZEALOT];

			info = &game->unitInfos[UNIT_STALKER];

			info = &game->unitInfos[UNIT_PANTHER];
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_HEMINE;
			info->orders[info->ordersNum++].key = 'E';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_REPULSE_MINE;
			info->orders[info->ordersNum++].key = 'R';
			info->orders[info->ordersNum].type = ORDER_CONSTRUCT;
			info->orders[info->ordersNum].unitType = UNIT_SPIDER_DEPLOYER;
			info->orders[info->ordersNum++].key = 'Q';

			info = &game->unitInfos[UNIT_HEMINE];
			info->orders[info->ordersNum].type = ORDER_EXPLODE;
			info->orders[info->ordersNum++].key = 'E';

			info = &game->unitInfos[UNIT_REPULSE_MINE];

			info = &game->unitInfos[UNIT_SPIDER_DEPLOYER];
			info->orders[info->ordersNum].type = ORDER_DEPLOY_SPIDERS;
			info->orders[info->ordersNum++].key = 'Q';

			info = &game->unitInfos[UNIT_JACK_RABBIT];
			info->orders[info->ordersNum].type = ORDER_SCAN_DART;
			info->orders[info->ordersNum++].key = 'Q';
			info->orders[info->ordersNum].type = ORDER_AREA_SCAN;
			info->orders[info->ordersNum++].key = 'W';

			info = &game->unitInfos[UNIT_FLAME_ANTELOPE];
			info->orders[info->ordersNum].type = ORDER_THROW_FIRE_BOMB;
			info->orders[info->ordersNum++].key = 'Q';

			for (int i = 0; i < UNIT_INFOS_MAX; i++) {
				UnitInfo *info = &game->unitInfos[i];
				if (info->idle2Timer.isZero()) info->idle2Timer = v2(30, 30);
				if (info->idle2Length == 0) info->idle2Length = 1;
				if (info->deathLength == 0) info->deathLength = 1;
				if (info->walkSpeedMulti == 0) info->walkSpeedMulti = 1;
				if (info->resMulti == 0) info->resMulti = 1;
				info->resMulti = 1; //@incomplete Decide how this works

				if (!info->formalName[0]) {
					strncpy(info->formalName, unitTypeStrings[i], FORMAL_NAME_MAX_LEN);
				}

				if (!info->isBuilding) {
					Order *order = NULL;

					order = &info->orders[info->ordersNum++];
					order->type = ORDER_ATTACK;
					order->key = 'A';

					order = &info->orders[info->ordersNum++];
					order->type = ORDER_STOP;
					order->key = 'S';

					order = &info->orders[info->ordersNum++];
					order->type = ORDER_HALT;
					order->key = 'H';

					order = &info->orders[info->ordersNum++];
					order->type = ORDER_PATROL;
					order->key = 'P';
				}
			}
		}

		pushTargetTexture(game->tileTexture);
		clearRenderer(0);
		drawRect(0, 0, game->tileTexture->width, game->tileTexture->height, 0xFFFF0000);
		popTargetTexture();

		game->_loginPacketsIn = createThreadSafeQueue(sizeof(MsgPacket), 256, false);
		game->_loginPacketsOut = createThreadSafeQueue(sizeof(MsgPacket), 256, false);

		audio->masterVolume = 1;
		game->scale = 1;
		// {
		// 	int size = 0;
		// 	void *data = readFile("assets/info/aZip.zip", &size);
		// 	Zip *zip = openZip((unsigned char *)data, size);
		// 	if (!zip) logf("No zip\n");
		// }

		WriteFence();

		createThread(updateLoginThread);

		logf("Game inited, took %f\n", getMsPassed(mainNano));
	}

	game->width = platform->windowWidth * game->scale;
	game->height = platform->windowHeight * game->scale;
	bool scaleChanged = false;
	if (game->prevScale != game->scale) {
		game->prevScale = game->scale;
		scaleChanged = true;

		if (game->minimapTexture) {
			destroyTexture(game->minimapTexture);
			game->minimapTexture = NULL;
		}

		if (game->statusCardTexture) {
			destroyTexture(game->statusCardTexture);
			game->statusCardTexture = NULL;
		}
	}

	if (game->debugFrameIter != 0) logf("Frame iters: %d\n", game->debugFrameIter);
	game->debugFrameIter = 0;

	Lobby *lobby = &game->lobby;
	LobbyMember *selfMember = NULL;
	for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
		if (lobby->members[i].clientId == game->clientId) {
			selfMember = &lobby->members[i];
			break;
		}
	}

	for (;;) {
		MsgPacket packet;
		bool good = threadSafeQueueShift(game->_loginPacketsIn, &packet);
		if (!good) break;
		NetMsg *msg = &packet.msg;

		if (msg) {
			game->_timeSinceLastContactedLoginServer = 0;
			if (msg->type == NET_MSG_StC_CONNECTED) {
				game->clientId = msg->clientId;
				logf("My client id is %d\n", game->clientId);
				game->serverConnectionState = SERVER_CONNECTED;
			} else if (msg->type == NET_MSG_StC_MATCH_START) {
				game->playerNumber = msg->matchStart.playerNumber;

				if (lobby->clientIsConnected) {
					selfMember->inGame = true;
					selfMember->isReady = false;
					game->playMode = PLAY_1V1_ONLINE;
				} else {
					game->teamNumber = game->playerNumber; 
					strcpy(game->currentMapPath, msg->matchStart.mapName);
				}

				game->framesPerTurn = msg->matchStart.framesPerTurn; 
				logf("Given player number %d (team %d)\n", game->playerNumber, game->teamNumber);
				game->matchId = msg->matchId;
				game->state = GAME_STATE_PLAYING;
				game->firstFrameOfMatch = true;
				logf("Match started, I am player %d (team %d)\n", game->playerNumber, game->teamNumber);
				// logf("Client %d %d\n", matchUdpListenerPort, matchTcpListenerPort);
				game->serverConnectionState = SERVER_IN_MATCH;

				if (game->sessionPacketsOut) destroyThreadSafeQueue(game->sessionPacketsOut);
				game->sessionPacketsOut = createThreadSafeQueue(sizeof(MsgPacket), 256, false);

				if (game->sessionPacketsIn) destroyThreadSafeQueue(game->sessionPacketsIn);
				game->sessionPacketsIn = createThreadSafeQueue(sizeof(MsgPacket), 256, false);

				if (game->_supportsUdp) { /// Create udp session socket
					if (game->udpSessionSocket) {
						socketClose(game->udpSessionSocket);
						game->udpSessionSocket = NULL;
					}
					game->udpSessionSocket = createUdpSocket(sizeof(NetMsg));

					game->udpSessionAddr = createSockaddr(game->serverIp, msg->matchStart.matchUdpListenerPort);

					if (!game->udpSessionSocket) {
						logf("Failed to create udp socket for session\n");
						logLastOSError();
						// Panic("Can't continue without udp socket");
					}

					if (!socketSetNonblock(game->udpSessionSocket)) {
						logf("Failed to make udp socket nonblocking for session\n");
						logLastOSError();
						Panic("Can't continue without nonblocking socket for session");
					}

					if (!socketBind(game->udpSessionSocket, createSockaddr(0, 0))) {
						logf("Failed to make bind udp socket for session\n");
						logLastOSError();
					}

					logf("Client session bound to: ");
					printSocketInfo(game->udpSessionSocket);
				} else {
					if (game->tcpSessionSocket) {
						socketClose(game->tcpSessionSocket);
						game->tcpSessionSocket = NULL;
					}
					game->tcpSessionSocket = createTcpSocket(sizeof(NetMsg));

					if (!game->tcpSessionSocket) {
						logf("Failed to create tcp socket for session\n");
						logLastOSError();
						Panic("Can't continue without tcp socket");
					}

					// logf("Trying %d\n", msg->matchStart.matchTcpListenerPort);
					if (!socketConnect(game->tcpSessionSocket, createSockaddr(game->serverIp, msg->matchStart.matchTcpListenerPort))) {
						logf("Failed to connect tcp socket for session\n");
						logLastOSError();
						Panic("Can't continue without connected tcp socket");
					}

					if (!socketSetNonblock(game->tcpSessionSocket)) {
						logf("Failed to make tcp socket nonblocking for session\n");
						logLastOSError();
						Panic("Can't continue without nonblocking socket for session");
					}
				}

				if (msg->matchStart.sameIp) {
					if (game->playerNumber == 0) {
						maximizeWindow();
					} else {
						minimizeWindow();
					}
				}

			} else if (msg->type == NET_MSG_StC_TRANSMIT_CONFIG) {
				game->serverConfig = msg->transmitConfig.serverConfig;
			} else if (msg->type == NET_MSG_StC_LOBBY_CREATED) {
				strcpy(game->lobbyHash, msg->joinLobby.lobbyHash);
				strcpy(lobby->hash, game->lobbyHash);

				MsgPacket packet = createMsgPacket(NET_MSG_CtS_JOIN_LOBBY);
				strcpy(packet.msg.joinLobby.lobbyHash, game->lobbyHash);
				if (!threadSafeQueuePush(game->_loginPacketsOut, &packet)) logf("_loginPacketsOut is full\n");
				game->state = GAME_STATE_LOBBY_JOIN_PENDING;
			} else if (msg->type == NET_MSG_StC_LOBBY_JOINED) {
				if (game->state != GAME_STATE_LOBBY) {
					lobby->clientIsConnected = true;
					game->state = GAME_STATE_LOBBY;
				}

				lobby->leaderClientId = msg->lobbyJoined.leaderClientId;
				logf("Leader is id %d\n", lobby->leaderClientId);

				LobbyMember *newMember = &msg->lobbyJoined.member;
				memcpy(&lobby->members[newMember->slot], newMember, sizeof(LobbyMember));
			} else if (msg->type == NET_MSG_StC_SOMEONE_IS_READY) {
				for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
					LobbyMember *member = &lobby->members[i];
					if (member->clientId == msg->someoneIsReady.member.clientId) {
						memcpy(member, &msg->someoneIsReady.member, sizeof(LobbyMember));
						break;
					}
				}
			} else if (msg->type == NET_MSG_CHANGE_LOBBY_CONFIG) {
				memcpy(&lobby->serverConfig, &msg->changeLobbyConfig.newServerConfig, sizeof(ServerConfig));
			}
		}
	}

	float timeScale = game->gameSpeed;
	float elapsed = platform->elapsed * timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	platform->mouse.x += platform->relativeMouse.x;
	platform->mouse.y += platform->relativeMouse.y;

	{ /// 2d camera
		if (game->forceZoom) game->zoom = game->forceZoom;
#if 1
		game->visualZoom = lerp(game->visualZoom, game->zoom, 0.1);
		game->visualCameraTarget = lerp(game->visualCameraTarget, game->cameraTarget, 0.1);
#else
		game->visualZoom = game->zoom;
		game->visualCameraTarget = game->cameraTarget;
#endif

		if (game->visualCameraTarget.distance(game->cameraTarget) > 1000) game->visualCameraTarget = game->cameraTarget;

		float camScale = 1.0/game->visualZoom / game->scale;
		Matrix3 mat = mat3();
		mat.TRANSLATE(game->visualCameraTarget.x, game->visualCameraTarget.y);
		mat.SCALE(camScale, camScale);
		mat.TRANSLATE(-game->width/2.0, -game->height/2.0);
		mat = mat.invert();
		game->visualCameraMatrix = mat;
		pushCamera2d(game->visualCameraMatrix);
	}

	// clearRenderer(0xFF64462A);
	// drawRect(0, 0, 64, 64, 0xFFFF0000); // The first line

	Map *map = &game->map;
	clearRenderer(map->backgroundColor);

	if (game->state == GAME_STATE_LOADING) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Loading...", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Loading...");
		ImGui::End();
		if (remoteZipsLoading == 0) game->state = GAME_STATE_LOG_IN;
	} else if (game->state == GAME_STATE_LOG_IN) {
		drawRect(100, 100, 128, 128, 0xFFFFFFFF);
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Log in", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Built %s %s", __DATE__, __TIME__);

		if (game->serverConnectionState == SERVER_DISCONNECTED) {
			if (platform->isDebugVersion) {
				if (ImGui::Button("Local server")) strcpy(game->serverIp, "127.0.0.1");
				ImGui::SameLine();
				if (ImGui::Button("Online server")) strcpy(game->serverIp, "184.72.9.156");
			}

#if !defined(__EMSCRIPTEN__)
			ImGui::InputText("Server ip", game->serverIp, PATH_MAX_LEN);
			ImGui::Checkbox("Supports udp", (bool *)&game->_supportsUdp);
#endif

			if (ImGui::Button("Connect") || flagShouldInstantlyConnect) {
				game->_loginServerAddr = createSockaddr(game->serverIp, game->serverPort);

				int tcpPortAdd = 0;
#if defined(__EMSCRIPTEN__)
				tcpPortAdd = 2;
#endif
				game->serverConnectionState = SERVER_CONNECTING;
				game->timeTillRetryConnection = 0;

				WriteFence();
				ReadFence();
				game->_shouldBeLoggedIn = true;
			}

			ImGui::Separator();

			if (platform->isDebugVersion && ImGui::Button("Play replay")) {
				game->state = GAME_STATE_REPLAY_BROWSER;
			}

			if (game->mapPathsNum == 0) {
				int pathsNum;
				char **paths = getDirectoryList("assets/maps", &pathsNum);
				for (int i = 0; i < pathsNum; i++) {
					char *path = paths[i];
					if (!strContains(path, "Props")) {
						loadStruct("Map", path, &game->maps[game->mapPathsNum]);

						*strrchr(path, '.') = 0;

						game->mapPaths[game->mapPathsNum] = stringClone(path);
						game->mapPathsNum++;
					}

					free(path);
				}
				free(paths);
			}

			if (ImGui::BeginCombo("Map", game->currentMapPath, ImGuiComboFlags_None)) {
				for (int i = 0; i < game->mapPathsNum; i++) {

					if (ImGui::Selectable(game->mapPaths[i], streq(game->mapPaths[i], game->currentMapPath))) {
						strcpy(game->currentMapPath, game->mapPaths[i]);
					}
				}
				ImGui::EndCombo();
			}
			ImGui::SameLine();
			if (ImGui::Button("Single player, no server") || flagShouldInstantlySinglePlayer) {
				flagShouldInstantlySinglePlayer = false;
				game->state = GAME_STATE_PLAYING;
				game->playMode = PLAY_SINGLE_PLAYER;
				game->framesPerTurn = 6;
				game->firstFrameOfMatch = true;
				game->playerNumber = 0;
				game->teamNumber = 0;
				maximizeWindow();
				logf("Single player match started\n");
			}

			// for (int i = 0; i < 3; i++) {
			// 	if (ImGui::Button(frameSprintf("Mission %d", i))) {
			// 		flagShouldInstantlySinglePlayer = false;
			// 		strcpy(game->currentMapPath, frameSprintf("assets/maps/mission%d", i));
			// 		game->state = GAME_STATE_PLAYING;
			// 		game->playMode = PLAY_MISSION;
			// 		game->framesPerTurn = 6;
			// 		game->firstFrameOfMatch = true;
			// 		game->playerNumber = 0;
			// 		game->teamNumber = 0;
			// 		maximizeWindow();
			// 		logf("Single player match started\n");
			// 	}
			// }

			if (ImGui::Button("Combat Test")) {
				flagShouldInstantlySinglePlayer = false;
				strcpy(game->currentMapPath, "assets/maps/CombatTest");
				game->state = GAME_STATE_PLAYING;
				game->playMode = PLAY_MISSION;
				game->framesPerTurn = 6;
				game->firstFrameOfMatch = true;
				game->playerNumber = 0;
				game->teamNumber = 0;
				maximizeWindow();
				logf("Single player match started\n");
			}

			ImGui::SliderFloat("game->scale", &game->scale, 0.01, 3);
		}

		if (game->serverConnectionState == SERVER_CONNECTING) {
			game->timeTillRetryConnection -= elapsed;
			if (game->timeTillRetryConnection <= 0) {
				game->timeTillRetryConnection = 3;

				{ /// Garbage
					MsgPacket packet = {};
					packet.msg.type = NET_MSG_GARBAGE;
					packet.msg.clientId = game->clientId;
					bool good = threadSafeQueuePush(game->_loginPacketsOut, &packet);
					if (!good) logf("_loginPacketsOut is full\n");
				}

#if !defined(__EMSCRIPTEN__)
				{ /// Connection message
					MsgPacket packet = {};
					packet.msg.type = game->_supportsUdp ? NET_MSG_CtS_UDP_CONNECT : NET_MSG_CtS_TCP_CONNECT;
					packet.msg.clientId = game->clientId;
					bool good = threadSafeQueuePush(game->_loginPacketsOut, &packet);
					if (!good) logf("_loginPacketsOut is full\n");
				}
#endif
			}

			ImGui::Text("Server connecting (retrying in %.1f)", game->timeTillRetryConnection);
			if (ImGui::Button("Cancel")) {
				flagShouldInstantlyConnect = false;
				flagShouldInstantlySinglePlayer = false;
				game->serverConnectionState = SERVER_DISCONNECTED;
			}
		} else if (game->serverConnectionState == SERVER_CONNECTED) {
			game->state = GAME_STATE_MAIN_MENU;
		}

#if defined(_WIN32)
		// if (ImGui::Button("Self-update")) {
		// 	if (netSys->curl) {

		// 		char *url = frameSprintf("http://184.72.9.156/rtsGameClient.zip");
		// 		curl_easy_setopt(netSys->curl, CURLOPT_URL, url);
		// 		CURLcode res = curl_easy_perform(netSys->curl);
		// 		curl_easy_cleanup(netSys->curl);
		// 	} else {
		// 		logf("Can't update because no cURL\n");
		// 	}
		// }
#endif

		ImGui::End();

	} else if (game->state == GAME_STATE_MAIN_MENU) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Main Menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		if (game->serverConnectionState == SERVER_CONNECTED) {
			bool doButton = false;

#if defined(RTS_AUTO_LOCAL_MULTI)
			if (flagShouldInstantlyConnect) doButton = true;
#endif

			if (ImGui::Button("Enter queue") || doButton) {
				doButton = false;
				flagShouldInstantlyConnect = false;

				MsgPacket packet = {};
				packet.msg.clientId = game->clientId;
				packet.msg.type = NET_MSG_CtS_QUEUE;
				bool good = threadSafeQueuePush(game->_loginPacketsOut, &packet);
				if (!good) logf("_loginPacketsOut is full\n");

				logf("Waiting for game (clientId: %d)\n", game->clientId);
				game->state = GAME_STATE_IN_QUEUE;
			}

			if (platform->isInternalVersion) {
				if (ImGui::Button("Create lobby")) {
					MsgPacket packet = createMsgPacket(NET_MSG_CtS_CREATE_LOBBY);
					if (!threadSafeQueuePush(game->_loginPacketsOut, &packet)) logf("_loginPacketsOut is full\n");
					memset(lobby, 0, sizeof(Lobby));
					game->state = GAME_STATE_LOBBY_PENDING;
				}

				ImGui::InputText("Lobby hash", game->lobbyHash, LOBBY_HASH_MAX_LEN);
				ImGui::SameLine();

#if defined(RTS_AUTO_JOIN_LOBBY_1)
			if (flagShouldInstantlyConnect) {
				doButton = true;
				strcpy(game->lobbyHash, "1");
			}
#endif
				if (ImGui::Button("Join lobby") || doButton) {
					doButton = false;
					flagShouldInstantlyConnect = false;

					MsgPacket packet = createMsgPacket(NET_MSG_CtS_JOIN_LOBBY);
					strcpy(packet.msg.joinLobby.lobbyHash, game->lobbyHash);
					if (!threadSafeQueuePush(game->_loginPacketsOut, &packet)) logf("_loginPacketsOut is full\n");
					memset(lobby, 0, sizeof(Lobby));
					strcpy(lobby->hash, game->lobbyHash);
					game->state = GAME_STATE_LOBBY_JOIN_PENDING;
				}
			}

			if (ImGui::Button("Configure server")) {
				game->serverConfig.isValid = false;

				MsgPacket packet = {};
				packet.msg.clientId = game->clientId;
				packet.msg.type = NET_MSG_CtS_REQUEST_CONFIG;
				bool good = threadSafeQueuePush(game->_loginPacketsOut, &packet);
				if (!good) logf("_loginPacketsOut is full\n");

				game->state = GAME_STATE_CONFIGURE_SERVER;
			}
		}

		ImGui::End();
	} else if (game->state == GAME_STATE_CONFIGURE_SERVER) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Configuring server", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ServerConfig *config = &game->serverConfig;
		if (config->isValid) {
			ImGui::InputText("Map name", config->mapName, PATH_MAX_LEN);

			if (ImGui::Button("Submit")) {
				MsgPacket packet = {};
				packet.msg.clientId = game->clientId;
				packet.msg.type = NET_MSG_CtS_TRANSMIT_CONFIG;
				packet.msg.transmitConfig.serverConfig = game->serverConfig;
				bool good = threadSafeQueuePush(game->_loginPacketsOut, &packet);
				if (!good) logf("_loginPacketsOut is full\n");

				game->state = GAME_STATE_MAIN_MENU;
			}
			if (ImGui::Button("Cancel")) {
				game->state = GAME_STATE_MAIN_MENU;
			}
		} else {
			ImGui::Text("Waiting for config");
		}
		ImGui::End();
	} else if (game->state == GAME_STATE_IN_QUEUE) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("In queue", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Waiting for another player...");

		ImGui::End();
		game->playMode = PLAY_1V1_ONLINE;
	} else if (game->state == GAME_STATE_REPLAY_BROWSER) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Replay browser", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		if (!game->replayPaths) {
			game->replayPaths = getDirectoryList("assets/replays", &game->replayPathsNum);
		}

		for (int i = 0; i < game->replayPathsNum; i++) {
			if (ImGui::Button(game->replayPaths[i])) {
				game->gameCmdsReplayMax = game->gameCmdHistoryMax;
				if (game->gameCmdsReplay) free(game->gameCmdsReplay);
				game->gameCmdsReplay = (GameCmd *)zalloc(game->gameCmdsReplayMax * sizeof(GameCmd));
				loadStructArray("GameCmd", game->replayPaths[i], game->gameCmdsReplay, game->gameCmdsReplayMax, sizeof(GameCmd));

				for (int i = 0; i < game->gameCmdsReplayMax; i++) {
					GameCmd *gameCmd = &game->gameCmdsReplay[i];
					if (gameCmd->type == GAME_CMD_NONE) {
						game->gameCmdsReplayNum = i;
						break;
					}
				}

				char *path = game->replayPaths[i];
				char *playerPtr = strstr(path, "player");
				playerPtr += strlen("player");
				if (*playerPtr == '0') game->playerNumber = 0;
				if (*playerPtr == '1') game->playerNumber = 1;
				Panic("Replys don't work (teamNumber)");
				logf("playerPtr: %c\n", *playerPtr);

				game->state = GAME_STATE_PLAYING;
				game->playMode = PLAY_REPLAY;
				game->replayPaused = true;
				game->firstFrameOfMatch = true;
			}
		}

		ImGui::End();
	} else if (game->state == GAME_STATE_MATCH_SUMMERY) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Match summery", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (game->wonGame) {
			ImGui::Text("You won!");
		} else {
			ImGui::Text("You lost!");
		}

		if (ImGui::Button("Back")) {
			if (game->playMode == PLAY_1V1_ONLINE) {
				if (lobby->clientIsConnected) {
					selfMember->inGame = false;
					game->state = GAME_STATE_LOBBY;
				} else {
					game->state = GAME_STATE_MAIN_MENU;
				}
			} else {
				game->state = GAME_STATE_LOG_IN;
			}
		}
		ImGui::End();
	} else if (game->state == GAME_STATE_LOBBY_PENDING) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Creating...", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("The lobby is being created");
		ImGui::End();
	} else if (game->state == GAME_STATE_LOBBY_JOIN_PENDING) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Joining...", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Joining lobby %s...", game->lobbyHash);
		ImGui::End();
	} else if (game->state == GAME_STATE_LOBBY) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Lobby", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		Lobby *lobby = &game->lobby;
		Map *lobbyMap = &game->lobbySelectedMap;
		ImGui::Text("Lobby code: %s\n", game->lobbyHash);
		ImGui::Separator();
		for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
			LobbyMember *member = &lobby->members[i];
			if (member->clientId == 0) continue;

			ImGui::PushID(i+32);

			if (member->clientId == game->clientId) {
				ImGui::PushStyleColor(ImGuiCol_Text, lerpColor(0xFF888888, 0xFF00FFFF, timePhase(platform->time, 5)));
			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, 0xFF888888);
			}
			ImGui::Text(
				"%s Client %d|%s|%s",
				member->clientId == lobby->leaderClientId ? "(Leader)" : "        ",
				member->clientId,
				member->isReady ? "Ready" : "Not ready",
				member->dead?"Dead":"Alive"
			);

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_SliderGrab, playerColors[member->teamNumber]);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, lerpColor(playerColors[member->teamNumber], 0xFFFFFFFF, 0.1));
			ImGui::SliderInt("Team number", &member->teamNumber, 0, 1);
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			ImGui::PopStyleColor();

			ImGui::PopID();
		}
		ImGui::Separator();

		for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
			LobbyMember *member = &lobby->members[i];
			if (member->clientId == game->clientId) {
				game->teamNumber = member->teamNumber;
				break;
			}
		}

		bool isLeader = game->clientId == lobby->leaderClientId;
		if (!isLeader) {
			ImGui::PushStyleColor(ImGuiCol_Text, 0xFF0000FF);
			ImGui::Text("You are not the leader");
			ImGui::PopStyleColor();
		}

		ServerConfig *serverConfig = &lobby->serverConfig;
		// if (serverConfig->mapName[0] == 0) strcpy(serverConfig->mapName, "assets/maps/smallTest");
		if (serverConfig->mapName[0] == 0) {
			char *chosenPath = NULL;
			for (int i = 0; i < game->mapPathsNum; i++) {
				Map *possibleMap = &game->maps[i];
				if (serverConfig->lobbyMode == LOBBY_MODE_1V1 && !possibleMap->enable1v1) continue;
				if (serverConfig->lobbyMode == LOBBY_MODE_COOP && !possibleMap->enableCoop) continue;
				chosenPath = game->mapPaths[i];
			}

			if (chosenPath) {
				strcpy(serverConfig->mapName, chosenPath);
			} else {
				strcpy(serverConfig->mapName, "assets/maps/mission1");
			}
		}

		if (!streq(game->currentMapPath, serverConfig->mapName)) {
			strcpy(game->currentMapPath, serverConfig->mapName);
			memset(lobbyMap, 0, sizeof(Map));
			loadStruct("Map", frameSprintf("%s.txt", game->currentMapPath), lobbyMap);
		}

		if (isLeader) {
			if (ImGui::Combo("Lobby mode", (int *)&serverConfig->lobbyMode, lobbyModeStrings, ArrayLength(lobbyModeStrings))) {
				serverConfig->mapName[0] = 0;
			}

			ImGui::SameLine();
			if (ImGui::BeginCombo("Map", serverConfig->mapName, ImGuiComboFlags_None)) {
				for (int i = 0; i < game->mapPathsNum; i++) {
					Map *possibleMap = &game->maps[i];
					if (serverConfig->lobbyMode == LOBBY_MODE_1V1 && !possibleMap->enable1v1) continue;
					if (serverConfig->lobbyMode == LOBBY_MODE_COOP && !possibleMap->enableCoop) continue;

					if (ImGui::Selectable(game->mapPaths[i], streq(game->mapPaths[i], serverConfig->mapName))) {
						strcpy(serverConfig->mapName, game->mapPaths[i]);
					}
				}
				ImGui::EndCombo();
			}
		} else {
			ImGui::Text("Lobby mode: %s", lobbyModeStrings[serverConfig->lobbyMode]);
			ImGui::Text("Map: %s", serverConfig->mapName);
		}

		ImGui::Text("MapProps: %d", lobbyMap->propsNum);
		ImGui::Text("Size: %dx%d", lobbyMap->width, lobbyMap->height);

		ImGui::PushStyleColor(ImGuiCol_Text, 0xFF0000FF);
		if (serverConfig->lobbyMode == LOBBY_MODE_COOP) {
			if (selfMember) selfMember->teamNumber = 0;
		} else if (serverConfig->lobbyMode == LOBBY_MODE_1V1) {
			LobbyMember *opposingMember = NULL;

			for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
				LobbyMember *member = &lobby->members[i];
				if (member->clientId == 0) continue;
				if (member->clientId == game->clientId) continue;

				if (!opposingMember) {
					opposingMember = member;
					continue;
				}

				if (!opposingMember->isReady && member->isReady) opposingMember = member;
			}

			if (selfMember && opposingMember) {
				if (selfMember->slot < opposingMember->slot) {
					selfMember->teamNumber = 0;
				} else {
					selfMember->teamNumber = 1;
				}
			}
		} else if (serverConfig->lobbyMode == LOBBY_MODE_ANY) {
			// Nothing...
		} else {
			logf("Bad\n");
		}
		ImGui::PopStyleColor();

		ImGui::Separator();

		if (ImGui::Button("Ready")) {
			selfMember->isReady = !selfMember->isReady;
		}

		ImGui::End();
	} else if (game->state == GAME_STATE_PLAYING) {
		updateSim();
	}

	// if (game->state == GAME_STATE_PLAYING) {
	// 	strcpy(game->currentMusic, "assets/audio/music/game/0.ogg");
	// } else {
	// 	strcpy(game->currentMusic, "assets/audio/music/menu/0.ogg");
	// }

	if (lobby->clientIsConnected) {
		if (platform->frameCount % 16 == 0) {
			if (platform->frameCount % 32 == 0 && game->clientId == lobby->leaderClientId) {
				MsgPacket packet = createMsgPacket(NET_MSG_CHANGE_LOBBY_CONFIG);
				strcpy(packet.msg.iAmReady.lobbyHash, lobby->hash);
				memcpy(&packet.msg.changeLobbyConfig.newServerConfig, &lobby->serverConfig, sizeof(ServerConfig));
				if (!threadSafeQueuePush(game->_loginPacketsOut, &packet)) logf("_loginPacketsOut is full\n");
			} else {
				MsgPacket packet = createMsgPacket(NET_MSG_CtS_I_AM_READY);
				strcpy(packet.msg.iAmReady.lobbyHash, lobby->hash);
				memcpy(&packet.msg.iAmReady.member, selfMember, sizeof(LobbyMember));
				if (!threadSafeQueuePush(game->_loginPacketsOut, &packet)) logf("_loginPacketsOut is full\n");
			}
		}
	}

	if (game->serverConnectionState != SERVER_DISCONNECTED && game->_timeSinceLastContactedLoginServer >= 1) {
		game->_timeSinceLastContactedLoginServer = 0;
		{ /// Garbage
			MsgPacket packet = {};
			packet.msg.type = NET_MSG_GARBAGE;
			packet.msg.clientId = game->clientId;
			bool good = threadSafeQueuePush(game->_loginPacketsOut, &packet);
			if (!good) logf("_loginPacketsOut is full\n");
		}
	}

	int myLastHash = -1;
	int otherLastHash = -1;
	{ /// Detect desync
		int myLastHashTurn = -1;
		for (int i = game->gameCmdsNum-1; i >= 0; i--) {
			GameCmd *gameCmd = &game->gameCmds[i];
			if (gameCmd->type == GAME_CMD_END_TURN && gameCmd->playerNumber == game->playerNumber) {
				myLastHash = gameCmd->worldHash;
				myLastHashTurn = gameCmd->turnToExecute;
				break;
			}
		}

		if (myLastHashTurn != -1) {
			for (int i = game->gameCmdsNum-1; i >= 0; i--) {
				GameCmd *gameCmd = &game->gameCmds[i];
				if (
					gameCmd->type == GAME_CMD_END_TURN &&
					gameCmd->playerNumber != game->playerNumber &&
					gameCmd->turnToExecute == myLastHashTurn
				) {
					otherLastHash = gameCmd->worldHash;
					break;
				}
			}
		}

		if (myLastHash != -1 && otherLastHash != -1 && myLastHash != otherLastHash) {
			if (!game->isDesynced && game->playMode == PLAY_1V1_ONLINE) {
				time_t currentDateTime = time(0);
				tm *now = localtime(&currentDateTime);
				char *name = frameSprintf(
					"assets/replays/desync p%d %d%d%d %d%d%d.txt",
					game->playerNumber,
					now->tm_mon+1,
					now->tm_mday,
					now->tm_year+1900,
					now->tm_hour,
					now->tm_min,
					now->tm_sec
				);

				logf("Client(%d): Desync occured, saving log...\n", game->playerNumber);
				saveStructArray("GameCmd", name, game->gameCmdHistory, game->gameCmdHistoryNum, sizeof(GameCmd));
			}
			game->isDesynced = true;
		}
	}

	{ /// Debug
		{ /// Debug info
			pushCamera2d(mat3());
			char *extraStr = NULL;

			if (game->missingTurnData) extraStr = frameSprintf("[red]No connection???[]");
			if (game->isDesynced) extraStr = frameSprintf("[red]Desynced!!![]");

#define DEBUG_STR_MAX_LEN 1024
			char *str = frameMalloc(DEBUG_STR_MAX_LEN);
			str[0] = 0;

			float avgFrameTime = 0;
			for (int i = 0; i < platform->frameTimesMax; i++) avgFrameTime += platform->frameTimes[i];
			avgFrameTime /= (float)platform->frameTimesMax;
			strcat(str, frameSprintf("Frame time: %.2f (%.1f fps)\nNet frame delay: %d\n", avgFrameTime, 1.0/(avgFrameTime/1000.0), game->framesPerTurn));

			for (int i = 0; i < PING_CHARTS_MAX; i++) {
				PingChart *chart = &game->pingCharts[i];
				if (!chart->title) {
					if (i == PING_CHART_LOGIN_UDP_SEND) chart->title = mallocSprintf("Login upd send");
					if (i == PING_CHART_LOGIN_UDP_RECV) chart->title = mallocSprintf("Login upd recv");
					if (i == PING_CHART_LOGIN_TCP_SEND) chart->title = mallocSprintf("Login tcp send");
					if (i == PING_CHART_LOGIN_TCP_RECV) chart->title = mallocSprintf("Login tcp recv");
					if (i == PING_CHART_SESSION_UDP_SEND) chart->title = mallocSprintf("Session udp send");
					if (i == PING_CHART_SESSION_UDP_RECV) chart->title = mallocSprintf("Session udp recv");
					if (i == PING_CHART_SESSION_TCP_RECV) chart->title = mallocSprintf("Session tcp recv");
					if (i == PING_CHART_SESSION_TCP_SEND) chart->title = mallocSprintf("Session tcp send");
				}

				chart->average = 0;
				chart->highest = 0;
				chart->min = 99999;
				chart->max = -99999;
				for (int i = 0; i < PING_CHART_TIMES_MAX; i++) {
					float value = chart->times[i];
					chart->average += value;
					if (value > chart->highest) chart->highest = value;
					if (value < chart->min) chart->min = value;
					if (value > chart->max) chart->max = value;
				}
				chart->average /= (float)PING_CHART_TIMES_MAX;

				char *line = frameSprintf("%s: %.1f\n", chart->title, chart->average*1000.0);

				strcat(str, line);
			}

			if (extraStr) strcat(str, extraStr);
			Vec2 size = getTextSize(game->smallFont, str);
			Vec2 pos = v2();
			drawText(game->smallFont, str, pos, 0xFFFFFFFF);
			popCamera2d();

			Rect rect = makeRect(pos, size);
			if (rect.contains(platform->mouse)) {
				ImGui::BeginTooltip();

				for (int i = 0; i < PING_CHARTS_MAX; i++) {
					PingChart *chart = &game->pingCharts[i];

					ImGui::PlotLines(
						frameSprintf("###%s", chart->title),
						(float *)chart->times,
						PING_CHART_TIMES_MAX,
						chart->nextTime,
						frameSprintf("%s (%.0f/%.0f/%.0f)", chart->title, chart->min*1000.0, chart->average*1000.0, chart->max*1000.0),
						0,
						chart->highest * 1.5,
						ImVec2(0,80)
					);
				}
				ImGui::EndTooltip();
			}
		}
	}

	if (game->debugShowDijkstraGrid) {
		Unit *unit = getFirstUnit(&game->localSelectedUnits);
		if (unit) {
			FlowField *field = unit->flowField;

			if (field) {
				for (int y = 0; y < map->height; y++) {
					for (int x = 0; x < map->width; x++) {
						Vec2 tile = v2(x, y);
						Rect rect = tileToWorldRect(tile);
						int index = field->dijkstraData[y * map->width + x];
						if (index == -1) {
							drawRect(rect, 0xFF0000FF);
							continue;
						}
						if (index == 0) continue;
						// logf("%d\n", index);
						drawRect(rect, argbToHex(255, 255, (index*5) % 255, 0));
					}
				}
			}
		}
	}

	if (game->debugShowFlowField) {
		Unit *unit = getFirstUnit(&game->localSelectedUnits);
		if (unit) {
			FlowField *field = unit->flowField;

			if (field) {
				for (int y = 0; y < map->height; y++) {
					for (int x = 0; x < map->width; x++) {
						char dir = field->data[y * map->width + x];
						if (dir == 0) continue;
						if (dir == -1) {
							drawRect(tileToWorldRect(v2(x, y)), 0xFFFF0000);
							continue;
						}
						if (dir == -2) {
							drawRect(tileToWorldRect(v2(x, y)), 0xFF00FF00);
							continue;
						}
						Vec2 start = tileToWorld(v2(x, y));
						Vec2 end;
						float deg = charToDeg(dir);
						float rad = toRad(deg);
						end.x = start.x + cos(rad) * 10;
						end.y = start.y + sin(rad) * 10;
						drawLine(start, end, 2, 0xFF00FFFF);
					}
				}
			}
		}
	}

	processBatchDraws();
	guiDraw();

	{
		pushCamera2d(mat3());

		// if (game->debugTexture) {
		// 	RenderProps props = newRenderProps();
		// 	drawTexture(game->debugTexture, props);
		// }

		drawOnScreenLog();

		if (platform->waitingForPointerLock) {
			Rect rect = {};
			rect.width = platform->windowWidth;
			rect.height = platform->windowHeight;
			drawRect(rect, 0x88000000);

			char *str = "[waves]Click the screen to play";
			Vec2 size = getTextSize(game->hugeFont, str);
			drawText(game->hugeFont, str, rect.center() - size/2.0, 0xFFFFFFFF);
		} else {
			Texture *cursorTexture = getTexture("assets/images/ui/cursor1.png");
			if (cursorTexture) {
				RenderProps props = newRenderProps();
				props.matrix.TRANSLATE(platform->mouse.x - cursorTexture->width/2, platform->mouse.y - cursorTexture->height/2);
				drawTexture(cursorTexture, props);
			}

			Rect mouseRect = makeRect(0, 0, 4, 4);
			mouseRect.x = platform->mouse.x - mouseRect.width/2;
			mouseRect.y = platform->mouse.y - mouseRect.height/2;
			drawRect(mouseRect, 0xFFFF0000);
		}

		// {
		// 	Texture *texture = getTexture("assets/images/fade128.png");
		// 	RenderProps props = newRenderProps();
		// 	props.x = 200;
		// 	props.y = 200;
		// 	drawTexture(texture, props);
		// }

		// {
		// 	Texture *texture = getTexture("assets/images/fade128.png");
		// 	RenderProps props = newRenderProps();
		// 	Matrix3 mat;
		// 	mat.setIdentity();
		// 	mat = mat.translate(200, 200);
		// 	mat = mat.translate(0, lerp(0, 128, secondPhase));
		// 	mat = mat.rotate(90);
		// 	props.matrix = &mat;
		// 	drawTexture(texture, props);
		// }

		popCamera2d();
	}

	{ /// Update sound
		if (!streq(game->prevMusic, game->currentMusic)) {
			Channel *channel = getChannel(game->musicChannel);
			if (!channel) {
				game->musicFadePerc = 0;
				game->musicChannel = playGameMusic(game->currentMusic);
				Channel *channel = getChannel(game->musicChannel);
				channel->userVolume = game->musicFadePerc;
				strcpy(game->prevMusic, game->currentMusic);
			} else {
				game->musicFadePerc -= 0.02;
				channel->userVolume = game->musicFadePerc;
				if (game->musicFadePerc <= 0) stopChannel(channel);
			}
		} else {
			game->musicFadePerc += 0.02;
			Clamp01(game->musicFadePerc);

			Channel *channel = getChannel(game->musicChannel);
			if (channel) {
				channel->userVolume = game->musicFadePerc;
			}
		}
	}

	if (game->tooltip) { /// Tooltip
		pushCamera2d(mat3());
		char *str = game->tooltip;
		float maxWidth = game->width * 0.15;

		Vec2 size = getTextSize(game->defaultFont, str, maxWidth);
		Vec2 pos = platform->mouse;

		Rect rect = makeRect(pos, size);
		if (rect.x + rect.width > platform->windowWidth) rect.x = platform->windowWidth - rect.width;
		if (rect.x < 0) rect.x = 0;
		if (rect.y + rect.height > platform->windowHeight) rect.y = platform->windowHeight - rect.height;
		if (rect.y < 0) rect.y = 0;
		pos = rect.pos();

		drawRect(rect, 0x88000000);
		drawText(game->defaultFont, str, pos, 0xFFFFFFFF, maxWidth);

		game->tooltip = NULL;
		popCamera2d();
	}

	game->time += elapsed;
	game->_timeSinceLastContactedLoginServer += elapsed;

	popCamera2d();
}

void updateSim() {
	if (game->gameTexture) {
		if (game->gameTexture->width != (int)game->width || game->gameTexture->height != (int)game->height) {
			destroyTexture(game->gameTexture);
			game->gameTexture = NULL;
		}
	}

	if (!game->gameTexture) {
		float scale = 0.5;
		game->gameTexture = createTexture(game->width, game->height);
	}

	if (!game->debugSkipGamePass) {
		pushTargetTexture(game->gameTexture);
		clearRenderer();
	}

	bool gamePaused = false;
	if (game->playMode == PLAY_REPLAY && game->replayPaused && game->framesThisTurn == 1) gamePaused = true;
	if (game->missingTurnData) gamePaused = true;

	float timeScale = game->gameSpeed;
	float elapsed = platform->elapsed * timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	bool shouldWinGame = false;
	bool shouldLoseGame = false;
	bool shouldDisconnect = false;

	// if (game->timeoutTimeLeft > 0) {
	// 	game->timeoutTimeLeft -= elapsed;
	// 	gamePaused = true;
	// }

	auto regenMapDeps = []()->void {
		Map *map = &game->map;
		for (int i = 0; i < FLOW_FIELDS_MAX; i++) {
			FlowField *field = &game->flowFields[i];
			if (field->data) free(field->data);
			if (field->dijkstraData) free(field->dijkstraData);
			memset(field, 0, sizeof(FlowField));
			field->data = (char *)zalloc(map->width * map->height * sizeof(char));
			field->dijkstraData = (int *)zalloc(map->width * map->height * sizeof(int));
		}

		if (game->collisionData) free(game->collisionData);
		game->collisionData = (int *)malloc(sizeof(int) * map->width * map->height);

		for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
			if (game->visionRefs[i]) free(game->visionRefs[i]);
			game->visionRefs[i] = (int *)zalloc(sizeof(int) * map->width * map->height);

			if (game->visionLightingRefs[i]) free(game->visionLightingRefs[i]);
			game->visionLightingRefs[i] = (int *)zalloc(sizeof(int) * map->width * map->height);
		}
	};

	Map *map = &game->map;
	Sim *sim = &game->sim;
	Globals *globals = &game->globals;
	Lobby *lobby = &game->lobby;
	bool doFirstFrameMapPropsStuff = false;
	if (game->firstFrameOfMatch) {
		memset(sim, 0, sizeof(Sim));
		game->matchRndSeed = 0;
		game->firstFrameOfMatch = false;
		game->doGenerators = true;
		doFirstFrameMapPropsStuff = true;
		game->missingTurnData = false;
		game->isDesynced = false;
		game->turnEnded = false;
		game->nextUnitId = 0;
		game->currentWorldHash = 0;
		game->_confirmedTurn = 0;
		game->_timeSinceLastContactedLoginServer = 0;
		game->timeSinceLastSessionServerContact = 0;
		game->timeSinceConfirmTurnUpdate = 0;

		game->wave = 0;
		game->prevWave = -1;
		game->npcPlayerNumber = -1;
		game->camBoundsMin = v2();
		game->camBoundsMax = v2();
		game->forceZoom = 0;

		memset(game->playerBaseStarts, 0, sizeof(Vec2) * PLAYERS_PER_MATCH_MAX);

		memset(game->gameCmds, 0, GAME_CMDS_MAX * sizeof(GameCmd));
		game->gameCmdsNum = 0;

		memset(game->gameCmdHistory, 0, game->gameCmdHistoryMax * sizeof(GameCmd));
		game->gameCmdHistoryNum = 0;

		memset(game->units, 0, UNITS_MAX * sizeof(Unit));
		game->unitsNum = 0;

		memset(game->bullets, 0, BULLETS_MAX * sizeof(Bullet));
		game->bulletsNum = 0;

		memset(game->effects, 0, EFFECTS_MAX * sizeof(Effect));
		game->effectsNum = 0;

		memset(game->fogUnits, 0, sizeof(FogUnit) * BUILDINGS_PER_PLAYER_MAX);
		game->fogUnitsNum = 0;

		game->alertsNum = 0;
		game->lightCasterRefsNum = 0;

		game->turn = 0;
		game->framesThisTurn = 0;
		game->inGameMenu = false;

		memset(game->controlGroups, 0, PLAYERS_PER_MATCH_MAX * CONTROL_GROUPS_MAX * sizeof(ControlGroup));
		memset(game->localControlGroups, 0, CONTROL_GROUPS_MAX * sizeof(ControlGroup));
		memset(game->selectedUnits, 0, PLAYERS_PER_MATCH_MAX * sizeof(ControlGroup));
		memset(&game->localSelectedUnits, 0, sizeof(ControlGroup));

		memset(game->pastTurnData, 0, PAST_TURN_DATA_MAX * sizeof(Game::PastTurnData));

		for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
			game->minerals[i] = 50;
			game->population[i] = 0;
			recountPopulation();
		}

		loadStructArray("Dialog", "assets/info/dialogs.txt", game->dialogs, DIALOGS_MAX, sizeof(Dialog));

		game->zoom = 1;
		platform->mouse.x = platform->windowWidth/2;
		platform->mouse.y = platform->windowHeight/2;

		{
			Map *currentMap = &game->map;
			char *mapFileName = frameSprintf("%s.txt", game->currentMapPath);
			char *mapPropsFileName = frameSprintf("%sProps.txt", game->currentMapPath);
			loadStructArray("MapProp", mapPropsFileName, currentMap->props, MAP_PROPS_MAX, sizeof(MapProp));
			loadStruct("Map", mapFileName, currentMap);

			game->mapEditorWidth = currentMap->width;
			game->mapEditorHeight = currentMap->height;
			if (map->minLight == 0) map->minLight = 0.85;
			if (map->radianceMulti == 0) map->radianceMulti = 1;
		}
		Map *map = &game->map;

		regenMapDeps();

		game->lightFloatData = (float *)zalloc(map->width*map->height*sizeof(float));

		if (game->playMode != PLAY_REPLAY) {
			GameCmd *gameCmd = createGameCmd();
			if (gameCmd) {
				gameCmd->type = GAME_CMD_END_TURN;
				gameCmd->turnToExecute = 0;
			}

			gameCmd = createGameCmd();
			if (gameCmd) {
				gameCmd->type = GAME_CMD_END_TURN;
				gameCmd->turnToExecute = 1;
			}
		}
		if (game->playMode == PLAY_SINGLE_PLAYER || game->playMode == PLAY_MISSION) {
			GameCmd *gameCmd = createGameCmd();
			if (gameCmd) {
				gameCmd->playerNumber = 1;
				gameCmd->clientId = -1;
				gameCmd->type = GAME_CMD_END_TURN;
				gameCmd->turnToExecute = 0;
			}

			gameCmd = createGameCmd();
			if (gameCmd) {
				gameCmd->playerNumber = 1;
				gameCmd->clientId = -1;
				gameCmd->type = GAME_CMD_END_TURN;
				gameCmd->turnToExecute = 1;
			}
		}

		game->firstFrameOfTurn = true;
	}

	bool isCoop = false;
	if (lobby->clientIsConnected) {
		for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
			LobbyMember *member = &lobby->members[i];
			if (member->clientId == 0) continue;
			if (member->clientId == game->clientId) continue;
			if (member->teamNumber == game->teamNumber) {
				isCoop = true;
				break;
			}
		}
	}

	if (game->playMode == PLAY_1V1_ONLINE) {
		Socket *sessionSocket = NULL;

		if (game->_supportsUdp) {
			sessionSocket = game->udpSessionSocket;
		} else {
			sessionSocket = game->tcpSessionSocket;
		}

		{ /// Read from session socket
			for (;;) {
				SocketlingStatus status = socketReceive(sessionSocket);
				if (status == SOCKETLING_ZERO_RECEIVE) {
					logf("The session server was disconnected\n");
					// exit(0);
				} else if (status == SOCKETLING_CONNECTION_RESET) {
					logf("The session server was destroyed\n");
					// exit(0);
				} else if (status == SOCKETLING_WOULD_BLOCK) {
					// Nothing...
				} else if (status == SOCKETLING_PENDING) {
					// Nothing...
				} else if (status == SOCKETLING_ERROR) {
					logLastOSError();
				} else if (status == SOCKETLING_GOOD) {
					if (sessionSocket == game->udpSessionSocket) stepPingChart(PING_CHART_SESSION_UDP_RECV);
					else stepPingChart(PING_CHART_SESSION_TCP_RECV);

					MsgPacket packet = {};
					memcpy(&packet.msg, sessionSocket->receiveBuffer, sessionSocket->packetSize);

					if (!threadSafeQueuePush(game->sessionPacketsIn, &packet)) {
						logf("Session incoming queue is overloaded\n");
					}
				} else {
					logf("Unknown socketling status %d\n", status);
				}

				if (status != SOCKETLING_GOOD) break;
			}
		}

		{ /// Sent to session socket
			for (;;) {
				MsgPacket packet;
				if (!threadSafeQueueShift(game->sessionPacketsOut, &packet)) {
					break;
				}
				NetMsg msg = packet.msg;
				// if (msg.type == NET_MSG_CtS_TURN_DATA) logf("Player %d sending %d game commands\n", game->playerNumber, msg.turnData.gameCmdsNum);

				if (game->_debugPacketDropRate != 0) {
					if (rndPerc(game->_debugPacketDropRate)) {
						logf("Packet dropped\n");
						continue;
					}
				}

				SocketlingStatus status = socketSend(sessionSocket, &msg, game->udpSessionAddr);
				if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
					if (!threadSafeQueuePush(game->sessionPacketsOut, &packet)) {
						logf("sessionPacketsOut is full upon repush\n");
					}
				} else if (status == SOCKETLING_ERROR) {
					logLastOSError();
				} else if (status == SOCKETLING_GOOD) {
					if (sessionSocket == game->udpSessionSocket) stepPingChart(PING_CHART_SESSION_UDP_SEND);
					else stepPingChart(PING_CHART_SESSION_TCP_SEND);
				} else {
					logf("Unknown socketling status %d\n", status);
				}
			}
		}

		for (;;) {
			MsgPacket packet;
			bool good = threadSafeQueueShift(game->sessionPacketsIn, &packet);
			if (!good) break;
			NetMsg *msg = &packet.msg;

			if (msg) {
				if (msg->type == NET_MSG_StC_TURN_DATA) {
					game->timeSinceLastSessionServerContact = 0;
					for (int i = 0; i < msg->turnData.gameCmdsNum; i++) {
						GameCmd *gameCmd = &msg->turnData.gameCmds[i];
						if (gameCmd->playerNumber == game->playerNumber) logf("Got own cmd\n");
						int diff = gameCmd->turnToExecute - game->_confirmedTurn;
						if (diff <= 0) continue;
						if (diff > 1) continue;
						game->gameCmds[game->gameCmdsNum++] = *gameCmd;

						if (gameCmd->type == GAME_CMD_END_TURN) {
							if (diff > 1) logf("Client is closing turns in the future\n");
							game->timeSinceConfirmTurnUpdate = 0;
							game->_confirmedTurn = gameCmd->turnToExecute;
						}
					}
				}
			}
		}
	}

	{ /// Checking turn
		game->missingTurnData = false;
		if (game->framesThisTurn >= game->framesPerTurn) {
			if (!game->turnEnded) {
				if (game->playMode != PLAY_REPLAY) {
					GameCmd *gameCmd = createGameCmd();
					if (!gameCmd) Panic("Couldn't append GAME_CMD_END_TURN (impossible)");
					gameCmd->type = GAME_CMD_END_TURN;
				}

				if (game->playMode == PLAY_SINGLE_PLAYER || game->playMode == PLAY_MISSION) {
					GameCmd *gameCmd = createGameCmd();
					if (!gameCmd) Panic("Couldn't append GAME_CMD_END_TURN (impossible (one player))");
					gameCmd->playerNumber = 1;
					gameCmd->clientId = -1;
					gameCmd->type = GAME_CMD_END_TURN;
					game->_confirmedTurn = gameCmd->turnToExecute;
				}

				game->turnEnded = true;
				// logf("Turn ended %d\n", game->turn);
			}

			if (game->_confirmedTurn >= game->turn+1) {
				game->framesThisTurn = 0;
				game->turn++;
				game->firstFrameOfTurn = true;
				game->turnEnded = false;

				int hash = 0;
				for (int i = 0; i < game->unitsNum; i++) {
					Unit *unit = &game->units[i];
					hash += unit->position.x;
					hash += unit->position.y;
					hash += unit->rallyPoint.x;
					hash += unit->rallyPoint.y;
					hash += unit->actionTime;
					hash += unit->actionsNum;
				}
				game->currentWorldHash = hash;

				{ /// Save past turn data
					memmove(&game->pastTurnData[1], &game->pastTurnData[0], (PAST_TURN_DATA_MAX-1) * sizeof(Game::PastTurnData));

					Game::PastTurnData *currentTurnData = &game->pastTurnData[0];
					currentTurnData->unitDatasNum = 0;

					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						Game::UnitPastData *unitPastData = &currentTurnData->unitDatas[currentTurnData->unitDatasNum++];

						unitPastData->id = unit->id;
						unitPastData->position = unit->visualPosition;
					}
				}

				{ /// Execute GameCmds
					for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
						int currentPlayerNumber = i;
						for (int i = 0; i < game->gameCmdsNum; i++) {
							GameCmd *gameCmd = &game->gameCmds[i];
							if (gameCmd->turnToExecute != game->turn) continue;
							if (gameCmd->playerNumber != currentPlayerNumber) continue;

							memcpy(&game->gameCmdHistory[game->gameCmdHistoryNum], gameCmd, sizeof(GameCmd));
							game->gameCmdHistoryNum++;

							if (gameCmd->type == GAME_CMD_CLICK_SELECT) {
								processClickSelect(gameCmd->playerNumber, gameCmd->targetId, gameCmd->queues, false);
							} else if (gameCmd->type == GAME_CMD_SPLICE_SELECTION) {
								processSpliceSelection(gameCmd->playerNumber, gameCmd->targetId, false);
							} else if (gameCmd->type == GAME_CMD_BOX_SELECT) {
								processTimeWarpBoxSelect(gameCmd->playerNumber, makeRect(gameCmd->position, gameCmd->size), gameCmd->queues, (UnitType)gameCmd->unitType, 2, false);
							} else if (gameCmd->type == GAME_CMD_CONSTRUCT_UNIT) {
								UnitInfo *info = &game->unitInfos[gameCmd->unitType];
								bool canBuild = true;
								if (game->minerals[gameCmd->playerNumber] < info->mineralPrice) {
									if (game->playerNumber == gameCmd->playerNumber) infof("Not enough minerals\n");
									canBuild = false;
								}

								if (!hasPopFor(gameCmd->playerNumber, (UnitType)gameCmd->unitType)) canBuild = false;

								ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
								Unit *firstUnit = getFirstUnit(group);
								Unit *lowestActionsUnit = NULL;

								for (int i = 0; i < group->idsNum; i++) {
									Unit *unit = getUnit(group->ids[i]);
									if (!unit) continue;
									if (unit->state == UNIT_STATE_CREATING) continue;
									if (!unit->connectedToPower && unit->info->isBuilding) continue;
									if (unit->type != firstUnit->type) continue;

									if (!lowestActionsUnit) lowestActionsUnit = unit;

									if (lowestActionsUnit->actionsNum > unit->actionsNum) {
										lowestActionsUnit = unit;
									}
								}

								if (!lowestActionsUnit) {
									// if (game->playerNumber == gameCmd->playerNumber) infof("Unit died\n"); // Or no power
									canBuild = false;
								}

								if (canBuild && lowestActionsUnit->actionsNum > BUILDING_QUEUE_MAX-1) {
									if (game->playerNumber == gameCmd->playerNumber) infof("Construction queue is full\n");
									canBuild = false;
								}

								if (canBuild) {
									if (canSeeUnit(game->teamNumber, lowestActionsUnit)) createEffect(EFFECT_START_UNIT_CONSTRUCTION, lowestActionsUnit->position);

									bool queues = gameCmd->queues;
									if (lowestActionsUnit->info->isBuilding) queues = true;
									Action *action = createAction(lowestActionsUnit, queues);
									if (action) {
										game->minerals[gameCmd->playerNumber] -= info->mineralPrice;
										action->type = ACTION_CONSTRUCT;
										action->unitType = (UnitType)gameCmd->unitType;
										action->mineralPrice = info->mineralPrice;
										action->populationPrice = info->population;
									}
								}
							} else if (gameCmd->type == GAME_CMD_PLACE_UNIT) {
								UnitInfo *info = &game->unitInfos[gameCmd->unitType];

								bool canBuild = true;
								if (game->minerals[gameCmd->playerNumber] < info->mineralPrice) {
									if (game->playerNumber == gameCmd->playerNumber) infof("Not enough minerals\n");
									canBuild = false;
								}

								if (!hasPopFor(gameCmd->playerNumber, (UnitType)gameCmd->unitType)) canBuild = false;

								Unit *unit = getUnit(gameCmd->targetId);
								if (!unit) {
									if (game->playerNumber == gameCmd->playerNumber) infof("Unit died\n");
									canBuild = false;
								}

								if (canBuild) {
									Rect rect = makeRect(gameCmd->position, gameCmd->size);
									Vec2 moveTo = rect.getClosestPoint(unit->position);

									// Action *action = createAction(unit, gameCmd->queues);
									// if (action) {
									// 	action->type = ACTION_MOVE;
									// 	action->position = moveTo;
									// }

									Action *action = createAction(unit, gameCmd->queues);
									if (action) {
										game->minerals[gameCmd->playerNumber] -= info->mineralPrice;
										action->type = ACTION_PLACE_UNIT;
										action->position = rect.center();
										action->unitType = (UnitType)gameCmd->unitType;
										action->mineralPrice = info->mineralPrice;
										// action->populationPrice = info->population; // This will be UNIT_STATE_CREATING when processed, so it should count in the queue either
									}
								}

							} else if (gameCmd->type == GAME_CMD_ATTACK_MOVE) {
								ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
								for (int i = 0; i < group->idsNum; i++) {
									Unit *unit = getUnit(group->ids[i]);
									if (!unit) continue;
									if (unit->info->isBuilding) continue;
									Action *action = createAction(unit, gameCmd->queues);
									if (action) {
										action->type = ACTION_ATTACK_MOVE;
										action->position = gameCmd->position;
									}
								}
							} else if (gameCmd->type == GAME_CMD_RIGHT_CLICK_UNIT) {
								ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
								Unit *clickedUnit = getUnit(gameCmd->targetId);
								if (clickedUnit) {
									for (int i = 0; i < group->idsNum; i++) {
										Unit *unit = getUnit(group->ids[i]);
										if (!unit) continue;
										createRightClickAction(unit, clickedUnit->position, gameCmd->queues);
									}
								}

							} else if (gameCmd->type == GAME_CMD_RIGHT_CLICK) {
								ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
								for (int i = 0; i < group->idsNum; i++) {
									Unit *unit = getUnit(group->ids[i]);
									if (!unit) continue;
									createRightClickAction(unit, gameCmd->position, gameCmd->queues);
								}

							} else if (gameCmd->type == GAME_CMD_CREATE_CONTROL_GROUP) {
								ControlGroup *group = &game->controlGroups[gameCmd->playerNumber][gameCmd->targetId];
								group->idsNum = 0;
								ControlGroup *srcGroup = &game->selectedUnits[gameCmd->playerNumber];
								for (int i = 0; i < srcGroup->idsNum; i++) {
									int id = srcGroup->ids[i];
									removeFromAllControlGroups(id, false);
									addToControlGroup(group, id);
								}
							} else if (gameCmd->type == GAME_CMD_ADD_TO_CONTROL_GROUP) {
								ControlGroup *group = &game->controlGroups[gameCmd->playerNumber][gameCmd->targetId];
								ControlGroup *srcGroup = &game->selectedUnits[gameCmd->playerNumber];
								for (int i = 0; i < srcGroup->idsNum; i++) {
									int id = srcGroup->ids[i];
									removeFromAllControlGroups(id, false);
									addToControlGroup(group, id);
								}
							} else if (gameCmd->type == GAME_CMD_SELECT_CONTROL_GROUP) {
								ControlGroup *destGroup = &game->selectedUnits[gameCmd->playerNumber];
								ControlGroup *srcGroup = &game->controlGroups[gameCmd->playerNumber][gameCmd->targetId];
								replaceControlGroup(destGroup, srcGroup);
							} else if (gameCmd->type == GAME_CMD_EXPLODE) {
								ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
								for (int i = 0; i < group->idsNum; i++) {
									Unit *mine = getUnit(group->ids[i]);
									if (!mine) continue;
									if (mine->type != UNIT_HEMINE && mine->type != UNIT_SPIDER_DEPLOYER) {
										continue;
									}
									Action *action = createAction(mine, false);
									if (action) {
										action->type = ACTION_EXPLODE;
									}
								}
							} else if (gameCmd->type == GAME_CMD_GENERIC_ORDER) {
								if (gameCmd->orderType == ORDER_AREA_SCAN) {
									ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
									for (int i = 0; i < group->idsNum; i++) {
										Unit *unit = getUnit(group->ids[i]);
										if (!unit) continue;
										if (unit->type != UNIT_JACK_RABBIT) continue;

										Action *action = createAction(unit, gameCmd->queues);
										if (action) action->type = ACTION_AREA_SCAN;
									}
								} else if (gameCmd->orderType == ORDER_SCAN_DART) {
									ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
									Unit *unit = getClosestUnit(group, gameCmd->position, UNIT_JACK_RABBIT);
									if (unit) {
										Action *action = createAction(unit, gameCmd->queues);
										if (action) {
											action->type = ACTION_SCAN_DART;
											action->position = gameCmd->position;
										}
										break;
									}
								} else if (gameCmd->orderType == ORDER_THROW_FIRE_BOMB) {
									ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
									Unit *unit = getClosestUnit(group, gameCmd->position, UNIT_FLAME_ANTELOPE, ACTION_THROW_FIRE_BOMB);
									if (unit) {
										Action *action = createAction(unit, gameCmd->queues);
										if (action) {
											action->type = ACTION_THROW_FIRE_BOMB;
											action->position = gameCmd->position;
										}
									}
								} else if (gameCmd->orderType == ORDER_STOP) {
									ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
									for (int i = 0; i < group->idsNum; i++) {
										Unit *unit = getUnit(group->ids[i]);
										if (!unit) continue;
										Action *action = createAction(unit, gameCmd->queues);
										if (action) {
											action->type = ACTION_STOP;
										}
									}
								}
							} else if (gameCmd->type == GAME_CMD_GAIN_MINERALS) {
								game->minerals[gameCmd->playerNumber] += gameCmd->targetId;
							} else if (gameCmd->type == GAME_CMD_CHANGE_GAME_SPEED) {
								game->gameSpeed = gameCmd->floatValue;
							} else if (gameCmd->type == GAME_CMD_RESIGN) {
								if (gameCmd->playerNumber == game->playerNumber) {
									shouldLoseGame = true;
								} else {
									shouldWinGame = true;
								}
							} else if (gameCmd->type == GAME_CMD_TIMEOUT) {
								game->timeoutTimeLeft = gameCmd->targetId;
							} else if (gameCmd->type == GAME_CMD_SPLICE_ACTION) {
								Unit *unit = getFirstUnit(&game->selectedUnits[gameCmd->playerNumber]);
								if (unit) {
									int actionNumber = gameCmd->targetId;
									if (actionNumber >= unit->actionsNum) continue;

									Action *action = &unit->actions[actionNumber];

									game->minerals[unit->playerNumber] += action->mineralPrice; //@copyPastedRefund
									if (actionNumber == 0) unit->firstFrameOfAction = true;
									arraySpliceIndex(unit->actions, ACTIONS_MAX, sizeof(Action), i);
									unit->actionsNum--;
								}
							} else if (gameCmd->type == GAME_CMD_ESC) {
								ControlGroup *group = &game->selectedUnits[gameCmd->playerNumber];
								Unit *mostProducingBuilding = NULL;
								for (int i = 0; i < group->idsNum; i++) {
									Unit *unit = getUnit(group->ids[i]);
									if (!unit) continue;
									if (unit->info->isBuilding && unit->actionsNum > 0) {
										if (!mostProducingBuilding || unit->actionsNum > mostProducingBuilding->actionsNum) {
											mostProducingBuilding = unit;
										}
									}
								}

								if (mostProducingBuilding) {
									Action *action = &mostProducingBuilding->actions[mostProducingBuilding->actionsNum-1];
									game->minerals[mostProducingBuilding->playerNumber] += action->mineralPrice; //@copyPastedRefund
									mostProducingBuilding->actionsNum--;
								} else {
									for (int i = 0; i < group->idsNum; i++) {
										Unit *unit = getUnit(group->ids[i]);
										if (!unit) continue;
										if (unit->state == UNIT_STATE_CREATING) {
											game->minerals[unit->playerNumber] += (float)unit->info->mineralPrice * 0.75;
											unit->hp = 0;
											unit->canRemove = true;
										}
									}
								}
							}

							if (game->gameCmdHistoryNum > game->gameCmdHistoryMax-1) {
								logf("Game is too long\n");
							}
						}
					}

					for (int i = 0; i < game->gameCmdsNum; i++) {
						GameCmd *gameCmd = &game->gameCmds[i];
						if (gameCmd->turnToExecute <= game->turn-2) {
							arraySplice(game->gameCmds, sizeof(GameCmd) * GAME_CMDS_MAX, sizeof(GameCmd), gameCmd);
							game->gameCmdsNum--;
							i--;
							continue;
						}
					}
				}

			} else {
				game->missingTurnData = true;
				gamePaused = true;
			}
		}
	}

	if (game->playMode == PLAY_REPLAY) {
		ImGui::Begin("Replay", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Checkbox("game->replayPaused", &game->replayPaused);
		ImGui::End();

		if (game->firstFrameOfTurn && !gamePaused) {
			for (int i = 0; i < game->gameCmdsReplayNum; i++) {
				GameCmd *replayCmd = &game->gameCmdsReplay[i];
				bool shouldAdd = false;

				if (game->turn == 0) {
					if (replayCmd->turnToExecute == 0 || replayCmd->turnToExecute == 1) shouldAdd = true;
				} else {
					if (replayCmd->turnToExecute == game->turn+1) shouldAdd = true;
				}

				if (shouldAdd) {
					game->gameCmds[game->gameCmdsNum++] = *replayCmd;
					game->_confirmedTurn = replayCmd->turnToExecute;
				}
			}
		}
	} else if (game->playMode == PLAY_1V1_ONLINE) {
		if (platform->frameCount % sendEveryXFrames == 0 || sendEveryXFrames <= 1) { /// Send GameCmds
			MsgPacket packet = {};
			packet.msg.clientId = game->clientId;
			packet.msg.matchId = game->matchId;
			packet.msg.type = NET_MSG_CtS_TURN_DATA;

			packet.msg.turnData.confirmedTurn = game->_confirmedTurn;
			NetMsg *msg = &packet.msg;
			for (int i = 0; i < game->gameCmdsNum; i++) {
				GameCmd *localGameCmd = &game->gameCmds[i];
				if (localGameCmd->playerNumber != game->playerNumber) continue;
				if (localGameCmd->turnToExecute == game->turn+turnDelay && !game->turnEnded) continue;
				if (localGameCmd->turnToExecute > game->turn+turnDelay) continue;
				if (msg->turnData.gameCmdsNum < GAME_CMDS_MAX) {
					// if (localGameCmd->turnToExecute == 0) logf("Packing turn 0 game cmd\n");
					msg->turnData.gameCmds[msg->turnData.gameCmdsNum++] = *localGameCmd;
				} else {
					Panic("Trying to send too many GameCmds to the server");
				}
			}

			// logf("Turn has passed (%f)\n", platform->time);
			// if (game->playerNumber == 0) logf("queuing data for turn %d\n", packet.msg.turnData.turn);

			// bool good = threadSafeQueuePush(game->_loginPacketsOut, &packet);
			// if (!good) logf("_loginPacketsOut is full\n");

			bool good = threadSafeQueuePush(game->sessionPacketsOut, &packet);
			if (!good) logf("sessionPacketsOut is full\n");
		}
	}

	int ticks = game->framesPerTurn*game->turn + game->framesThisTurn;
	// renderer->defaultSunPosition = map->sunPosition;

	float uiScale = game->scale;
	if (!game->minimapTexture) game->minimapTexture = createTexture(256 * uiScale, 256 * uiScale);
	Rect minimapRect;
	minimapRect.width = game->minimapTexture->width;
	minimapRect.height = game->minimapTexture->height;
	minimapRect.x = 0;
	minimapRect.y = game->height - minimapRect.height;
	bool isHoveringMinimap = minimapRect.contains(platform->mouse);

	Vec2 minimapHoveringWorldPos = platform->mouse;
	minimapHoveringWorldPos.x -= minimapRect.x;
	minimapHoveringWorldPos.y -= minimapRect.y;
	minimapHoveringWorldPos.x /= minimapRect.width;
	minimapHoveringWorldPos.y /= minimapRect.height;
	minimapHoveringWorldPos.x *= map->width * tileSize;
	minimapHoveringWorldPos.y *= map->height * tileSize;

	if (!game->statusCardTexture) game->statusCardTexture = createTexture(750*uiScale, 220*uiScale);
	Rect statusCardRect;
	statusCardRect.width = game->statusCardTexture->width;
	statusCardRect.height = game->statusCardTexture->height;
	statusCardRect.x = (256 + 40) * uiScale;
	statusCardRect.y = game->height - statusCardRect.height;
	bool isHoveringStatus = statusCardRect.contains(platform->mouse);

	Rect wheelCornerRect = makeRect(0, 0, 230*uiScale, 230*uiScale);
	wheelCornerRect.x = statusCardRect.x + statusCardRect.width;
	wheelCornerRect.y = statusCardRect.y;
	bool isHoveringCornerWheel = wheelCornerRect.contains(platform->mouse);

	bool mouseJustDown = false;
	bool mouseJustUp = false;
	bool rightMouseJustDown = false;
	bool rightMouseJustUp = false;
	bool mouseDown = false;

	if (keyJustPressed(KEY_BACKTICK)) {
		if (game->editorMode == EDITOR_NONE) {
			game->editorMode = EDITOR_NORMAL;
		} else {
			game->editorMode = EDITOR_NONE;
		}
	}

	// platform->disableGui = false;
	// if (game->editorMode == EDITOR_NONE) platform->disableGui = true;
	// if (game->inGameMenu) platform->disableGui = false;

	mouseJustDown = platform->mouseJustDown;
	mouseJustUp = platform->mouseJustUp;
	rightMouseJustDown = platform->rightMouseJustDown;
	rightMouseJustUp = platform->rightMouseJustUp;
	mouseDown = platform->mouseDown;

	if (platform->hoveringGui) {
		mouseJustUp = false;
		mouseJustDown = false;
		rightMouseJustDown = false;
	}

	if (keyJustPressed(KEY_BACKSPACE) && game->editorMode == EDITOR_NONE) {
		game->inGameMenu = !game->inGameMenu;
	}

#if defined(__EMSCRIPTEN__)
	platform->useRelativeMouse = true;
#else
	platform->useRelativeMouse = platform->windowHasFocus;
#endif

	if (game->inGameMenu) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Game menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::Button("Resign")) {
			GameCmd *gameCmd = createGameCmd();
			gameCmd->type = GAME_CMD_RESIGN;
		}

		if (ImGui::Button("Timeout (30sec.)")) {
			GameCmd *gameCmd = createGameCmd();
			gameCmd->type = GAME_CMD_TIMEOUT;
			gameCmd->targetId = 30;
		}

		ImGui::Separator();

		if (ImGui::Button("Return to game")) {
			game->inGameMenu = false;
			platform->mouse.x = platform->windowWidth/2;
			platform->mouse.y = platform->windowHeight/2;
		}

		ImGui::End();

		platform->useRelativeMouse = false;
	}

	if (platform->useRelativeMouse) {
		float scrollSpeed = 30.0 * 1.0/game->zoom;

		if (platform->mouse.x <= 0) {
			platform->mouse.x = 0;
			game->cameraTarget.x -= scrollSpeed;
		}

		if (platform->mouse.x >= platform->windowWidth-1) {
			platform->mouse.x = platform->windowWidth-1;
			game->cameraTarget.x += scrollSpeed;
		}

		if (platform->mouse.y <= 0) {
			platform->mouse.y = 0;
			game->cameraTarget.y -= scrollSpeed;
		}

		if (platform->mouse.y >= platform->windowHeight-1) {
			platform->mouse.y = platform->windowHeight-1;
			game->cameraTarget.y += scrollSpeed;
		}

		Vec2 camBoundsMin = v2();
		Vec2 camBoundsMax;
		camBoundsMax.x = map->width * tileSize;
		camBoundsMax.y = map->height * tileSize;

		if (!game->camBoundsMin.isZero()) camBoundsMin = game->camBoundsMin;
		if (!game->camBoundsMax.isZero()) camBoundsMax = game->camBoundsMax;

		Vec2 camSize;
		camSize.x = (float)game->width/2.0 / (float)game->zoom;
		camSize.y = (float)game->height/2.0 / (float)game->zoom;
		camSize = camSize / game->scale;

		Vec2 cameraMin;
		cameraMin.x = camBoundsMin.x + camSize.x;
		cameraMin.y = camBoundsMin.y + camSize.y;

		Vec2 cameraMax;
		cameraMax.x = camBoundsMax.x - camSize.x;
		cameraMax.y = camBoundsMax.y - camSize.y;

		if (game->editorMode == EDITOR_NONE) {
			if (game->cameraTarget.x > cameraMax.x) game->cameraTarget.x = cameraMax.x;
			if (game->cameraTarget.x < cameraMin.x) game->cameraTarget.x = cameraMin.x;
			if (game->cameraTarget.y > cameraMax.y) game->cameraTarget.y = cameraMax.y;
			if (game->cameraTarget.y < cameraMin.y) game->cameraTarget.y = cameraMin.y;
		}

		game->visualCameraTarget = game->cameraTarget;

		if (platform->mouseWheel != 0 && !platform->hoveringGui && !isHoveringStatus && !isHoveringCornerWheel) {
			if (platform->mouseWheel > 0) {
				float zoomChange = 1.1;
				game->zoom *= zoomChange;
			}

			if (platform->mouseWheel < 0) {
				float zoomChange = 0.9;
				game->zoom *= zoomChange;
				if (game->zoom > 1) {
					if (!platform->isInternalVersion && game->editorMode == EDITOR_NORMAL) game->zoom = 1;
				}
			}
			logf("Zoom: %f\n", game->zoom);
		}
	}

	Vec2 mousePos = platform->mouse * (game->scale);
	mousePos = renderer->camera2d.invert().multiply(mousePos);

	for (int i = 0; i < map->propsNum; i++) {
		MapProp *mapProp = &map->props[i];
		mapProp->matrix = getMapPropMatrix(mapProp);

		if (mapProp->type == MAP_PROP_UNIT_SPAWN) {
			UnitInfo *info = &game->unitInfos[mapProp->unitType];
			mapProp->size.x = info->size;
			mapProp->size.y = info->size;
		}

		Matrix3 mat = mapProp->matrix;

		mat.SCALE(mapProp->size.x, mapProp->size.y);

		mapProp->screenSpaceMatrix = mat;
		mapProp->center = mapProp->screenSpaceMatrix.multiply(v2(0.5, 0.5));

		if (mapProp->modSeed == 0) mapProp->modSeed = time(NULL);
	}

	if (doFirstFrameMapPropsStuff) {
		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];

			if (mapProp->coopOnly && !isCoop) continue;
			if (mapProp->nonCoopOnly && isCoop) continue;

			if (mapProp->type == MAP_PROP_UNIT_SPAWN) {
				int teamNumber = -1;

				if (lobby->clientIsConnected) {
					for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
						LobbyMember *member = &lobby->members[i];
						if (member->clientId == 0) continue;
						if (member->clientId == game->clientId) teamNumber = member->teamNumber;
					}
				} else {
					teamNumber = mapProp->playerNumber;
				}

				Unit *unit = createUnit(mapProp->playerNumber, teamNumber, mapProp->unitType);
				if (unit) {
					unit->position = mapProp->screenSpaceMatrix.multiply(v2(0.5, 0.5));
					bool isOddSize = (unit->info->size/tileSize) % 2 == 1;

					if (isOddSize) {
						unit->position.x -= tileSize/2;
						unit->position.y -= tileSize/2;
					}
					unit->position.x = roundToNearest(unit->position.x, tileSize);
					unit->position.y = roundToNearest(unit->position.y, tileSize);

					if (isOddSize) {
						unit->position.x += tileSize/2;
						unit->position.y += tileSize/2;
					}

				} else {
					logf("Player %d creates %s\n", mapProp->playerNumber, unitTypeStrings[mapProp->unitType]);
					logf("Very bad %d\n", __LINE__);
				}

				if (unit->type == UNIT_NEXUS) {
					game->playerBaseStarts[unit->playerNumber] = unit->position;
					game->startingBaseIds[unit->teamNumber] = unit->id;
					if (unit->playerNumber == game->playerNumber) {
						game->cameraTarget = unit->position;
					} else {
						createAlert(ALERT_FOUND_AN_ENEMY_BASE, game->playerNumber, unit->position);
					}
				}
			}
		}

		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->type == UNIT_PROBE) {
				Action *action = createAction(unit, false);
				if (action) {
					action->type = ACTION_GATHER;
				}
			}

		}
	}

	{ /// Map bg
		if (game->mapBgRenderScale == 0) game->mapBgRenderScale = 1;

		Vec2 mapBgSize = v2((float)(map->width*tileSize) * game->mapBgRenderScale, (float)(map->height*tileSize) * game->mapBgRenderScale);
		if (game->mapBg) {
			if (game->mapBg->width != mapBgSize.x || game->mapBg->height != mapBgSize.y || doFirstFrameMapPropsStuff || game->debugLiveUpdatePerlin) {
				destroyTexture(game->mapBg);
				game->mapBg = NULL;
			}
		}

		if (!game->mapBg) {
			game->mapBg = createTexture(mapBgSize.x, mapBgSize.y);
			Texture *texture = game->mapBg;
			pushTargetTexture(texture);
			pushCamera2d(mat3());
			clearRenderer();

			Matrix3 uvMatrix = mat3();
			uvMatrix.SCALE(map->perlinFreq, map->perlinFreq);
			uvMatrix.TRANSLATE(map->perlinScroll.x, map->perlinScroll.y);

			Texture *white = renderer->whiteTexture;
			RenderProps props = newRenderProps();
			props.flags |= _F_PERLIN;
			props.matrix.SCALE((float)texture->width/(float)white->width, (float)texture->height / (float)white->height);
			props.uvMatrix = uvMatrix;
			props.colorShift = map->perlinColor;
			drawTexture(white, props);

			popCamera2d();
			popTargetTexture();
		}

		{
			pushCamera2d(mat3());
			Texture *texture = game->mapBg;

			Vec2 verts[4];
			verts[0] = v2(0, 0);
			verts[1] = v2(1, 0);
			verts[2] = v2(1, 1);
			verts[3] = v2(0, 1);

			Vec2 uvs[4];
			uvs[0] = v2(0, 0);
			uvs[1] = v2(1, 0);
			uvs[2] = v2(1, 1);
			uvs[3] = v2(0, 1);

			u16 inds[6] = {0, 2, 1, 0, 3, 2};

			Matrix3 mat = mat3();
			mat = mat.multiply(renderer->baseMatrix2d);
			mat = mat.multiply(game->visualCameraMatrix);
			mat.SCALE(texture->width, texture->height);
			mat.SCALE(1.0/game->mapBgRenderScale, 1.0/game->mapBgRenderScale);

			Matrix3 uvMat = mat3();
			uvMat.TRANSLATE(0, 1);
			uvMat.SCALE(1, -1);

			// glDisable(GL_BLEND);
			draw2dMesh(verts, uvs, inds, 6, mat, game->mapBg);
			// glEnable(GL_BLEND);
			popCamera2d();
		}
	}

	if (game->doGenerators) {
		game->doGenerators = false;

		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];
			if (mapProp->wasGenerated) {
				arraySpliceIndex(map->props, map->propsNum, sizeof(MapProp), i);
				map->propsNum--;
				i--;
				continue;
			}
		}

		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];

			char *imageDir = frameMalloc(PATH_MAX_LEN);
			strcpy(imageDir, mapProp->imagePath);
			char *lastSlash = strrchr(imageDir, '/');
			if (lastSlash) *(lastSlash+1) = 0;

			pushRndSeed(mapProp->modSeed);

			int poissonCols = 1;
			int poissonRows = 1;
			if (mapProp->generatesProps) {
				poissonCols = sqrt(mapProp->generatesProps);
				poissonRows = sqrt(mapProp->generatesProps);
			}
			int poissonCellSize = map->width * tileSize / poissonRows;
			for (int i = 0; i < mapProp->generatesProps; i++) {
				MapProp *gennedMapProp = duplicateMapProp(mapProp, true);
				if (!gennedMapProp) continue;

				u32 oldSeed = popRndSeed();
				char *newImagePath = resolveFuzzyPath(imageDir, NULL, FUZZY_PNG);
				pushRndSeed(oldSeed);
				strcpy(gennedMapProp->imagePath, newImagePath);
				gennedMapProp->parent = mapProp->id;
				gennedMapProp->modSeed = rndInt(0, lcgM/2);

				Texture *texture = getTexture(newImagePath);
				gennedMapProp->size.x = texture->width;
				gennedMapProp->size.y = texture->height;

				if (mapProp->generationType == PROP_GENERATION_CIRCLE) {
				float rads = rndFloat(0, 2.0*M_PI);
				float dist = rndFloat(0, mapProp->generationRadius);
				gennedMapProp->position.x = cos(rads)*dist;
				gennedMapProp->position.y = sin(rads)*dist;
				} else if (mapProp->generationType == PROP_GENERATION_POISSON_CHEAT) {
					Rect rect;
					rect.x = (i % poissonCols) * poissonCellSize - mapProp->position.x;
					rect.y = (i / poissonCols) * poissonCellSize - mapProp->position.y;
					rect.width = poissonCellSize;
					rect.height = poissonCellSize;
					gennedMapProp->position.x = rndFloat(rect.x, rect.x+rect.width);
					gennedMapProp->position.y = rndFloat(rect.y, rect.y+rect.height);
				}
			}
			popRndSeed();
		}
	}

	{
		for (int i = 0; i < map->propsNum; i++) map->props[i].beenModdedByParent = false;

		for (int i = 0; i < map->propsNum; i++) {
			MapProp *moddedMapProp = &map->props[i];

			MapProp *propsLeft[MAP_PROPS_MAX];
			int propsLeftNum = 0;
			for (int i = 0; i < map->propsNum; i++) if (map->props[i].id != moddedMapProp->id) propsLeft[propsLeftNum++] = &map->props[i];

			int toChange[MAP_PROPS_MAX];
			int toChangeNum = 0;
			toChange[toChangeNum++] = moddedMapProp->id;

			for (;;) {
				if (!moddedMapProp->childrenInheritMods) break;
				bool addedSomething = false;

				for (int i = 0; i < propsLeftNum; i++) {
					MapProp *mapProp = propsLeft[i];
					bool shouldSplicePropsLeft = false;
					for (int i = 0; i < toChangeNum; i++) {
						if (mapProp->id == toChange[i] || mapProp->parent == toChange[i]) {
							toChange[toChangeNum++] = mapProp->id;
							addedSomething = true;
							shouldSplicePropsLeft = true;
							break;
						}
					}

					if (shouldSplicePropsLeft) {
						arraySpliceIndex(propsLeft, MAP_PROPS_MAX, sizeof(MapProp *), i);
						i--;
						propsLeftNum--;
						continue;
					}
				}

				if (!addedSomething) break;
			}

			// logf("Changing %d props\n", toChangeNum);
			pushRndSeed(moddedMapProp->modSeed);
			for (int i = 0; i < toChangeNum; i++) {
				MapProp *mapProp = getMapProp(toChange[i]);
				if (!mapProp) {
					if (game->turn < 4) logf("Map prop %d tried to change %d, but it doesn't exist\n", moddedMapProp->id, toChange[i]);
					continue;
				}
				if (mapProp->beenModdedByParent) continue;

				mapProp->colorShiftPower = rndFloat(moddedMapProp->modColorShiftPower.x, moddedMapProp->modColorShiftPower.y);
				mapProp->rotationBump = rndFloat(moddedMapProp->modRotationBump.x, moddedMapProp->modRotationBump.y);
				mapProp->transparencyBump = rndFloat(moddedMapProp->modTransparencyBump.x, moddedMapProp->modTransparencyBump.y);
				mapProp->scaleBump = rndFloat(moddedMapProp->modScaleBump.x, moddedMapProp->modScaleBump.y);
				if (mapProp != moddedMapProp) mapProp->beenModdedByParent = true;

			}
			popRndSeed();
		}
	}

	for (int i = 0; i < map->propsNum; i++) {
		MapProp *mapProp = &map->props[i];
		if (mapProp->editorOnly && game->editorMode == EDITOR_NONE) continue;
		if (mapProp->wasGenerated && game->editorConfig.hideGeneratedFromScreen) continue;

		if (mapProp->type == MAP_PROP_IMAGE) {
			char *imagePath = NULL;
			if (mapProp->imagePath[0] == 0) {
				imagePath = mapProp->imagePath;
			} else if (stringStartsWith(mapProp->imagePath, "assets/")) {
				imagePath = mapProp->imagePath;
			} else {
				imagePath = frameSprintf("assets/images/mapProps/%s", mapProp->imagePath);
			}

			Texture *texture = getTexture(imagePath);
			if (texture) {
				RenderProps props = newRenderProps();

				Matrix3 mat = mapProp->screenSpaceMatrix;

				// Vec2 point = mat.multiply(v2(0.5, 0.5));
				// drawCircle(point, 3, 0xFFFF0000);

				mat.SCALE(1.0/texture->width, 1.0/texture->height);
				mat.SCALE(1 + mapProp->scaleBump, 1 + mapProp->scaleBump);

				mat.TRANSLATE(0.5, 0.5);
				mat.ROTATE(-mapProp->rotationBump);
				mat.TRANSLATE(-0.5, -0.5);

				props.matrix = mat;
				if (game->editorConfig.editorOnlyFlash && mapProp->editorOnly) {
					props.tint = lerpColor(mapProp->tint, 0xFFFF0000, secondPhase);
				} else {
					props.tint = mapProp->tint;
				}
				props.colorShift = lerpColor(0xFFFFFFFF, mapProp->modColorShiftColor, mapProp->colorShiftPower);
				props.alpha = 1.0 - (mapProp->transparency + mapProp->transparencyBump);
				// props.flags |= _F_FXAA;
				drawTexture(texture, props);

				{
					// char *str = frameSprintf("%f\n", mapProp->scaleBump);
					// Vec2 size = getTextSize(game->defaultFont, str);
					// drawText(game->defaultFont, str, mapProp->screenSpaceMatrix.multiply(v2(0.5, 0.5)) - size/2);
				}
			}
		}
	}

	for (int i = 0; i < map->propsNum; i++) {
		MapProp *mapProp = &map->props[i];
		if (mapProp->wasGenerated && mapProp->time > 0) {
			Vec2 propCenter = mapProp->screenSpaceMatrix.multiply(v2(0.5, 0.5));
			Vec2 propTile = worldToTile(propCenter);

			bool shouldDelete = false;
			if (
				propTile.x <= 0 || propTile.x >= map->width-1 ||
				propTile.y <= 0 || propTile.y >= map->height-1
			) shouldDelete = true;

			int centerTileIndex = propTile.y * map->width + propTile.x;
			if (!shouldDelete && game->collisionData[centerTileIndex] != 0) shouldDelete = true;

			for (int i = 0; i < map->propsNum; i++) {
				MapProp *otherMapProp = &map->props[i];
				if (otherMapProp == mapProp) continue;

#if 1 //@todo This was flipped the other way
				Rect myRect = mapProp->screenSpaceMatrix.multiply(makeRect(0, 0, 1, 1));
				Rect otherRect = otherMapProp->screenSpaceMatrix.multiply(makeRect(0, 0, 1, 1));

				if (myRect.intersects(otherRect)) {
					shouldDelete = true;
					break;
				}
#else
				Vec2 myTri1[3] = {
					mapProp->screenSpaceMatrix.multiply(v2(0.0, 0.0)),
					mapProp->screenSpaceMatrix.multiply(v2(1.0, 0.0)),
					mapProp->screenSpaceMatrix.multiply(v2(0.0, 1.0)),
				};

				Vec2 myTri2[3] = {
					mapProp->screenSpaceMatrix.multiply(v2(1.0, 0.0)),
					mapProp->screenSpaceMatrix.multiply(v2(1.0, 1.0)),
					mapProp->screenSpaceMatrix.multiply(v2(0.0, 1.0)),
				};

				Vec2 otherTri1[3] = {
					otherMapProp->screenSpaceMatrix.multiply(v2(0.0, 0.0)),
					otherMapProp->screenSpaceMatrix.multiply(v2(1.0, 0.0)),
					otherMapProp->screenSpaceMatrix.multiply(v2(0.0, 1.0)),
				};

				Vec2 otherTri2[3] = {
					otherMapProp->screenSpaceMatrix.multiply(v2(1.0, 0.0)),
					otherMapProp->screenSpaceMatrix.multiply(v2(1.0, 1.0)),
					otherMapProp->screenSpaceMatrix.multiply(v2(0.0, 1.0)),
				};


				if (triTri2D(myTri1, otherTri1)) shouldDelete = true;
				if (triTri2D(myTri2, otherTri1)) shouldDelete = true;
				if (triTri2D(myTri1, otherTri2)) shouldDelete = true;
				if (triTri2D(myTri2, otherTri2)) shouldDelete = true;
#endif
			}

			if (shouldDelete) {
				deleteMapProp(mapProp);
				i--;
				continue;
			}
		}

		mapProp->time += elapsed;
	}

	if (game->editorMode != EDITOR_NONE && game->editorConfig.showParent) {
		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];
			if (mapProp->wasGenerated && (game->editorConfig.hideGeneratedFromScreen || game->editorConfig.hideGeneratedFromList)) continue;

			if (game->editorConfig.showParent) {
				Vec2 point = v2(0, 0);

				MapProp *parent = getMapProp(mapProp->parent);
				if (parent) {
					Vec2 start = mapProp->screenSpaceMatrix.multiply(v2(0.5, 0.5));
					Vec2 end = parent->screenSpaceMatrix.multiply(v2(0.5, 0.5));
					drawLine(start, end, 2, 0xFF00FF00);
					// Do something
				}
			}
		}
	}

	bool noMinimapClick = false;
	bool noBoxSelect = false;

	/// ---------------------------------------------- About to figure out unit stuff
	memcpy(game->collisionData, map->collisionData, sizeof(int) * map->width * map->height);

	struct RepulsionCircle {
		Vec2 position;
		float radius;
		int createdByUnitId;
	};

	RepulsionCircle *repulsionCircles = (RepulsionCircle *)frameMalloc(UNITS_MAX * sizeof(RepulsionCircle));
	int repulsionCirclesNum = 0;

	for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
		game->population[i] = 0;
		game->visualMineral[i] = lerp(game->visualMineral[i], game->minerals[i], 0.25);
	}

	BitGrid *baseBuildGrids[PLAYERS_PER_MATCH_MAX];
	for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
		baseBuildGrids[i] = createBitGrid(map->width * map->height);
	}

	BitGrid *buildGrids[PLAYERS_PER_MATCH_MAX];
	for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
		buildGrids[i] = createBitGrid(map->width * map->height);
	}

	for (int i = 0; i < map->propsNum; i++) {
		MapProp *mapProp = &map->props[i];
		if (mapProp->generatesCollision && mapProp->type == MAP_PROP_IMAGE) {
			Vec2 verts[4];
			verts[0] = v2(0, 0);
			verts[1] = v2(1, 0);
			verts[2] = v2(1, 1);
			verts[3] = v2(0, 1);

			Vec2 min = v2(999999, 999999);
			Vec2 max = v2(-999999, -999999);
			for (int i = 0; i < 4; i++) {
				verts[i] = mapProp->screenSpaceMatrix.multiply(verts[i]);
				if (verts[i].x < min.x) min.x = verts[i].x;
				if (verts[i].y < min.y) min.y = verts[i].y;
				if (verts[i].x > max.x) max.x = verts[i].x;
				if (verts[i].y > max.y) max.y = verts[i].y;
			}

			Vec2Tile minTile = worldToVec2Tile(min);
			if (minTile.x < 0) minTile.x = 0;
			if (minTile.y < 0) minTile.y = 0;
			if (minTile.x > map->width-1) minTile.x = map->width-1;
			if (minTile.y > map->height-1) minTile.y = map->height-1;

			Vec2Tile maxTile = worldToVec2Tile(max);
			if (maxTile.x < 0) maxTile.x = 0;
			if (maxTile.y < 0) maxTile.y = 0;
			if (maxTile.x > map->width-1) maxTile.x = map->width-1;
			if (maxTile.y > map->height-1) maxTile.y = map->height-1;

			for (int y = minTile.y; y <= maxTile.y; y++) {
				for (int x = minTile.x; x <= maxTile.x; x++) {
					Vec2 point = tileToWorld(v2(x, y));

					if (
						pointInTriangle(point, verts[0], verts[1], verts[2]) ||
						pointInTriangle(point, verts[0], verts[2], verts[3])
					) {
						game->collisionData[y * map->width + x] = 2;
					}
				}
			}

			// drawCircle(min, 5, 0xFFFF0000);
			// drawCircle(max, 5, 0xFF0000FF);
		}
	}

	{ /// Build spatial bins
		int binsWide = sqrt(BINS_MAX);
		int binsHigh = sqrt(BINS_MAX);

		float binWidth = map->width*tileSize / binsWide;
		float binHeight = map->height*tileSize / binsHigh;

		game->spatialBinsNum = 0;
		for (int y = 0; y < binsHigh; y++) {
			for (int x = 0; x < binsWide; x++) {
				SpatialBin *bin = &game->spatialBins[game->spatialBinsNum++];
				bin->unitsNum = 0;
				bin->region.x = x * binWidth;
				bin->region.y = y * binHeight;
				bin->region.width = binWidth;
				bin->region.height = binHeight;
			}
		}

		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];

			Vec2 minPoint = unit->rect.pos();
			Vec2 maxPoint = minPoint + unit->rect.size();
			int minBinX = minPoint.x / binWidth;
			int minBinY = minPoint.y / binHeight;
			int maxBinX = ceilf(maxPoint.x / binWidth);
			int maxBinY = ceilf(maxPoint.y / binHeight);
			if (minBinX < 0) minBinX = 0;
			if (minBinX > binsWide) minBinX = binsWide;

			if (maxBinX < 0) maxBinX = 0;
			if (maxBinX > binsWide) maxBinX = binsWide;

			if (minBinY < 0) minBinY = 0;
			if (minBinY > binsHigh) minBinY = binsHigh;

			if (maxBinY < 0) maxBinY = 0;
			if (maxBinY > binsHigh) maxBinY = binsHigh;

			for (int y = minBinY; y < maxBinY; y++) {
				for (int x = minBinX; x < maxBinX; x++) {
					int binIndex = y * binsWide + x;
					SpatialBin *bin = &game->spatialBins[binIndex];
					if (bin->region.intersects(unit->rect)) bin->units[bin->unitsNum++] = unit;
				}
			}
		}
	}

	bool resetFlowFields = false;
	if (game->collisionHasChanged) resetFlowFields = true;
	game->collisionHasChanged = false;
	int buildingsPerTeam[TEAMS_PER_MATCH_MAX] = {};
	for (int i = 0; i < game->unitsNum; i++) { /// First unit loop of the frame
		Unit *unit = &game->units[i];
		unit->connectedToPower = false;
		if (resetFlowFields) unit->flowField = NULL;

		unit->rect = getUnitRect(unit->type, unit->position);

		if (unit->teamNumber != -1) {
			if (unit->info->isBuilding) buildingsPerTeam[unit->teamNumber]++;
		}

		if (unit->type == UNIT_NEXUS && unit->rallyPoint.isZero()) {
			Unit *closestMinerals = NULL;
			for (int i = 0; i < game->unitsNum; i++) {
				Unit *otherUnit = &game->units[i];
				if (otherUnit->type != UNIT_MINERALS) continue;

				if (!closestMinerals) {
					closestMinerals = otherUnit;
					continue;
				}

				if (otherUnit->position.distance(unit->position) < closestMinerals->position.distance(unit->position)) {
					closestMinerals = otherUnit;
				}
			}

			if (closestMinerals) {
				unit->rallyPoint = closestMinerals->position;
			}
		}

		int visionRadius = unit->info->visionRadius;
		if (unit->state == UNIT_STATE_CREATING) visionRadius = 2;

		unit->givingTrueVision = false;
		if (unit->actionsNum > 0 && unit->actions[0].type == ACTION_AREA_SCAN) {
			unit->givingTrueVision = true;
			visionRadius = 0;

			float perc = unit->actionTime / 5.0;
			if (perc > 1) perc = 1;
			int scanRadius = roundf(lerp(1, 20, perc));
			visionRadius = scanRadius;
		}

		if (unit->teamNumber > -1) {
			if (unit->lightId) propagateLight(unit->lightId, worldToVec2Tile(unit->position), visionRadius, 1);
		}

		bool countsForPopulation = true;
		if (unit->state == UNIT_STATE_CREATING) countsForPopulation = false;
		if (unit->playerNumber == -1) countsForPopulation = false;
		if (unit->info->isBuilding) countsForPopulation = false;

		if (countsForPopulation) game->population[unit->playerNumber] += unit->info->population;
		for (int i = 0; i < unit->actionsNum; i++) {
			Action *action = &unit->actions[i];
			game->population[unit->playerNumber] += action->populationPrice;
		}

		bool countsForCollision = true;
		if (!unit->info->isBuilding) countsForCollision = false;
		if (unit->info->noBoids) countsForCollision = false;

		if (countsForCollision) {
			Vec2Tile tiles[TILES_UNDER_UNIT_MAX];

			int tilesNum = getTilesUnderRect(unit->rect, tiles, TILES_UNDER_UNIT_MAX);
			// if (game->playerNumber == 0) logf("Tiles for %s: \n", unitTypeStrings[unit->type]);
			for (int i = 0; i < tilesNum; i++) {
				Vec2Tile tile = tiles[i];
				// if (game->playerNumber == 0) tile.print("tile");

				int tileIndex = tile.y * map->width + tile.x;
				game->collisionData[tileIndex] = 1;
			}
		}

		unit->unmovable = false;
		if (unit->state == UNIT_STATE_PRE_ATTACK || unit->state == UNIT_STATE_POST_ATTACK) {
			unit->unmovable = true;
		}

		if (unit->actionsNum > 0) {
			Action *action = &unit->actions[0];
			if (action->type == ACTION_PLACE_UNIT) {
				float newUnitSize = (float)game->unitInfos[action->unitType].size;
				if (unit->position.distance(action->position) < newUnitSize/2.0 + tileSize + tileSize*5) {
					RepulsionCircle *circle = &repulsionCircles[repulsionCirclesNum++];
					circle->position = action->position;
					circle->radius = newUnitSize/2.0 * 2.0;
				}
			}

			if (action->type == ACTION_MOVE && game->playerNumber == unit->playerNumber) { // Hilights ACTION_MOVEs
				Vec2 pos0 = unit->position;
				for (int i = 0; i < unit->actionsNum; i++) {
					Action *action = &unit->actions[i];
					if (action->type != ACTION_MOVE) break;

					Vec2 pos1 = action->position;
					drawLine(pos0, pos1, 2, 0x8888FF00);
					pos0 = pos1;
				}
			}

			if (action->type == ACTION_STOP) {
				unit->unmovable = true;
			}
		}

		if (unit->hp < 0) {
			bool shouldDoDeathAction = false;
			if (unit->actionsNum == 0) shouldDoDeathAction = true;
			if (unit->actionsNum > 0 && unit->actions[0].type != ACTION_DEATH) shouldDoDeathAction = true;

			if (shouldDoDeathAction) {
				Action *action = createAction(unit, false);
				if (action) {
					action->type = ACTION_DEATH;
				} else {
					logf("Unit failed to die\n");
				}
			}
		}

		if (game->debugShowUnitInfo) {
			ControlGroup *group = &game->localSelectedUnits;
			for (int i = 0; i < group->idsNum; i++) {
				if (group->ids[i] == unit->id) {
					ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
					ImGui::Begin("Unit debug info", NULL, ImGuiWindowFlags_AlwaysAutoResize);
					char *str = frameSprintf(
						"Position: %f %f\n"
						"Rally point: %f %f\n"
						"Action time: %f\n"
						"Unique action time: %f\n"
						"Actions num: %d (%s)\n"
						"Attack time: %f\n"
						"Slow: %d\n"
						"timeSinceAttackAlert: %f\n",
						unit->position.x, unit->position.y,
						unit->rallyPoint.x, unit->rallyPoint.y,
						unit->actionTime,
						unit->uniqueActionTime,
						unit->actionsNum, unit->actionsNum > 0 ? actionTypeStrings[unit->actions[0].type] : "none",
						unit->attackTime,
						unit->slow,
						unit->timeSinceAttackAlert
					);
					ImGui::Text(str);
					ImGui::End();
				}
			}
		}
	}

	{ /// Light emission
		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];
			if (mapProp->emitsLight > 0) {
				for (int i = 0; i < TEAMS_PER_MATCH_MAX; i++) {
					if (!mapProp->lightIds[i]) mapProp->lightIds[i] = createLight(i);
					propagateLight(mapProp->lightIds[i], worldToVec2Tile(mapProp->center), mapProp->emitsLight, 1);
				}
			}
		}
	}

	recountPopulation();

	{  /// Vision is calculated, now do the build grid
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];

			Vec2Tile tiles[TILES_UNDER_UNIT_MAX];

			int tilesNum = getTilesUnderRect(unit->rect, tiles, TILES_UNDER_UNIT_MAX);
			// if (game->playerNumber == 0) logf("Tiles for %s: \n", unitTypeStrings[unit->type]);
			for (int i = 0; i < tilesNum; i++) {
				Vec2Tile tile = tiles[i];
				// if (game->playerNumber == 0) tile.print("tile");

				int tileIndex = tile.y * map->width + tile.x;
				if (unit->teamNumber == -1) {
					setBit(buildGrids[0], tileIndex, true);
					setBit(buildGrids[1], tileIndex, true);
				} else {
					if (unit->info->isBuilding) {
						if (canSeeTile(0, tileIndex)) setBit(buildGrids[0], tileIndex, true);
						if (canSeeTile(1, tileIndex)) setBit(buildGrids[1], tileIndex, true);
					} else {
						int teamToLookAtTheVisionGridOf = unit->teamNumber == 0 ? 1 : 0;
						bool canSee = canSeeTile(teamToLookAtTheVisionGridOf, tileIndex);
						if (canSee) setBit(buildGrids[teamToLookAtTheVisionGridOf], tileIndex, true);
					}
				}
			}

			if (unit->type == UNIT_MINERALS) {
				//@incomplete Consider only fog units and visible units
				int radius = 2; // make a global
				Vec2Tile tilePosition = worldToVec2Tile(unit->position);
				int minX = tilePosition.x - radius;
				int maxX = tilePosition.x + radius+1;
				int minY = tilePosition.y - radius;
				int maxY = tilePosition.y + radius+1;

				if (minX < 0) minX = 0;
				if (maxX > map->width-1) maxX = map->width-1;
				if (minY < 0) minY = 0;
				if (maxY > map->height-1) maxY = map->height-1;

				Circle circle = makeCircle(v2(tilePosition.x, tilePosition.y), (float)radius+0.5);
				for (int y = minY; y < maxY; y++) {
					for (int x = minX; x < maxX; x++) {
						if (!contains(circle, v2(x, y))) continue;

						int gridIndex = y * map->width + x;
						setBit(baseBuildGrids[0], gridIndex, true);
						setBit(baseBuildGrids[1], gridIndex, true);
					}
				}
			}
		}
	}

	for (int i = 0; i < map->width * map->height; i++) {
		int collValue = game->collisionData[i];
		if (collValue == 0) continue;

		int tileIndex = i;
		for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
			if (collValue == 1 && !canSeeTile(i, tileIndex)) continue;
			setBit(buildGrids[i], tileIndex, true);
			if (getBit(buildGrids[i], tileIndex)) setBit(baseBuildGrids[i], tileIndex, 1);
		}
	}

	/// -------------------------------------- Done figuring out unit stuff

	if (game->editorMode == EDITOR_NONE) {
		if (keyJustReleased(KEY_SHIFT) && game->interfaceMode == INTERFACE_ATTACK_MOVE)  game->interfaceMode = INTERFACE_NORMAL;

		if (mouseJustDown && !isHoveringStatus && !isHoveringCornerWheel) {
			// {{{
			bool shouldProcessClick = false;
			Vec2 clickPosition = v2();

			if (isHoveringMinimap) {
				if (game->interfaceMode == INTERFACE_ATTACK_MOVE) {
					shouldProcessClick = true;
					clickPosition = minimapHoveringWorldPos;
				}
			} else {
				shouldProcessClick = true;
				clickPosition = mousePos;
				if (game->interfaceMode == INTERFACE_NORMAL || game->interfaceMode == INTERFACE_PICKING_BUILDING) {
					game->mouseDownPosition = mousePos;
				}
			}

			if (shouldProcessClick) {
				if (game->interfaceMode == INTERFACE_PLACING_BUILDING) {
					UnitInfo *info = &game->unitInfos[game->unitTypeCurrentlyPlacing];
					bool canBuild = true;

					if (game->minerals[game->playerNumber] < info->mineralPrice) {
						infof("Not enough minerals\n");
						canBuild = false;
					}

					if (!hasPopFor(game->playerNumber, game->unitTypeCurrentlyPlacing)) canBuild = false;

					Vec2Tile tiles[TILES_UNDER_UNIT_MAX];
					int tilesNum = getTilesUnderRect(game->unitCurrentlyPlacingRect, tiles, TILES_UNDER_UNIT_MAX);
					for (int i = 0; i < tilesNum; i++) {
						Vec2Tile tile = tiles[i];
						int tileIndex = tile.y * map->width + tile.x;
						bool offGrid = tile.x < 0 || tile.x > map->width-1 || tile.y < 0 || tile.y > map->height-1;

						BitGrid *grid;
						if (game->unitTypeCurrentlyPlacing == UNIT_NEXUS) grid = baseBuildGrids[game->teamNumber];
						else grid = buildGrids[game->teamNumber];

						if (offGrid || getBit(grid, tileIndex)) {
							infof("Can't build on top of terrain\n");
							canBuild = false;
							break;
						}
					}

					if (canBuild) {
						ControlGroup *group = &game->localSelectedUnits;
						Unit *firstUnit = getFirstUnit(group);
						Unit *lowestActionsUnit = NULL;

						for (int i = 0; i < group->idsNum; i++) {
							Unit *unit = getUnit(group->ids[i]);
							if (!unit) continue;
							if (unit->type != firstUnit->type) continue;

							if (!lowestActionsUnit) lowestActionsUnit = unit;
							if (lowestActionsUnit->actionsNum > unit->actionsNum) lowestActionsUnit = unit;
						}

						if (lowestActionsUnit) {
							GameCmd *gameCmd = createGameCmd();
							if (gameCmd) {
								gameCmd->type = GAME_CMD_PLACE_UNIT;
								gameCmd->targetId = lowestActionsUnit->id;
								gameCmd->unitType = game->unitTypeCurrentlyPlacing;
								gameCmd->position = v2(game->unitCurrentlyPlacingRect.x, game->unitCurrentlyPlacingRect.y);
								gameCmd->size = v2(game->unitCurrentlyPlacingRect.width, game->unitCurrentlyPlacingRect.height);
								if (keyPressed(KEY_SHIFT)) gameCmd->queues = true;
							}
						}

						if (!keyPressed(KEY_SHIFT)) {
							game->interfaceMode = INTERFACE_NORMAL;
							game->unitTypeCurrentlyPlacing = UNIT_NONE;
							game->orderWheelBigMode = 0;
						}
					}
				} else if (game->interfaceMode == INTERFACE_ATTACK_MOVE) {
					GameCmd *gameCmd = createGameCmd();
					if (gameCmd) {
						gameCmd->type = GAME_CMD_ATTACK_MOVE;
						gameCmd->position = isHoveringMinimap ? minimapHoveringWorldPos : mousePos;
						if (keyPressed(KEY_SHIFT)) gameCmd->queues = true;

						createEffect(EFFECT_ISSUED_ATTACK_MOVE, gameCmd->position);
					}

					if (!keyPressed(KEY_SHIFT)) game->interfaceMode = INTERFACE_NORMAL;
					noMinimapClick = true;
				}
			}
			// }}}
		}

		if (mouseDown) {
			// {{{
			if (!game->mouseDownPosition.isZero()) {
				Rect *rect = &game->boxSelectRect;
				rect->x = game->mouseDownPosition.x;
				rect->y = game->mouseDownPosition.y;

				{ //@makePositive
					float width = mousePos.x - rect->x;
					float height = mousePos.y - rect->y;

					if (width < 0) {
						rect->x += width;
						width *= -1;
					}

					if (height < 0) {
						rect->y += height;
						height *= -1;
					}

					rect->width = width;
					rect->height = height;
				}

				drawRect(game->boxSelectRect, 0x8800FF00);
			}
			// }}}
		}

		if (mouseJustUp) {
			// {{{
			if (game->interfaceMode == INTERFACE_NORMAL || game->interfaceMode == INTERFACE_PICKING_BUILDING) {
				if (!game->mouseDownPosition.isZero()) {

					float boxArea = game->boxSelectRect.width * game->boxSelectRect.height;
					if (boxArea < 16 && !keyPressed(KEY_CTRL)) {
						bool didClick = false;
						for (int i = 0; i < game->unitsNum; i++) {
							Unit *unit = &game->units[i];
							Rect unitRect = getUnitRect(unit->type, unit->visualPosition);
							if (unitRect.contains(mousePos)) {
								if (unit->playerNumber != game->playerNumber) continue;
								didClick = true;
								GameCmd *gameCmd = createGameCmd();
								if (gameCmd) {
									gameCmd->type = GAME_CMD_CLICK_SELECT;
									gameCmd->targetId = unit->id;
									if (keyPressed(KEY_SHIFT)) gameCmd->queues = true;
									createEffect(EFFECT_UNIT_SELECTED, unit->id);

									processClickSelect(gameCmd->playerNumber, gameCmd->targetId, gameCmd->queues, true);
									game->interfaceMode = INTERFACE_NORMAL;
								}

								break;
							}
						}

						if (!didClick) {
							GameCmd *gameCmd = createGameCmd();
							if (gameCmd) {
								gameCmd->type = GAME_CMD_CLICK_SELECT;
								gameCmd->targetId = 0;
								processClickSelect(gameCmd->playerNumber, gameCmd->targetId, gameCmd->queues, true);
								game->interfaceMode = INTERFACE_NORMAL;
							}
						}

					} else {
						GameCmd *gameCmd = createGameCmd();

						if (gameCmd) {
							gameCmd->type = GAME_CMD_BOX_SELECT;
							if (keyPressed(KEY_CTRL)) {
								for (int i = 0; i < game->unitsNum; i++) {
									Unit *unit = &game->units[i];
									Rect unitRect = getUnitRect(unit->type, unit->visualPosition);
									if (unitRect.contains(mousePos)) gameCmd->unitType = (int)unit->type;
								}
								// renderer->screenRect = topCamera.invert().multiply(makeRect(0, 0, platform->windowWidth, platform->windowHeight));
								Rect rect = renderer->screenRect;
								gameCmd->position.x = rect.x;
								gameCmd->position.y = rect.y;
								gameCmd->size.x = rect.width;
								gameCmd->size.y = rect.height;
							} else {
								gameCmd->position.x = game->boxSelectRect.x;
								gameCmd->position.y = game->boxSelectRect.y;
								gameCmd->size.x = game->boxSelectRect.width;
								gameCmd->size.y = game->boxSelectRect.height;
							}

							if (keyPressed(KEY_SHIFT)) gameCmd->queues = true; // This actually means add to the current selection

							Rect boxRect = makeRect(gameCmd->position, gameCmd->size);
							processTimeWarpBoxSelect(gameCmd->playerNumber, boxRect, gameCmd->queues, (UnitType)gameCmd->unitType, 0, true);
							game->interfaceMode = INTERFACE_NORMAL;
						}
					}
				}

				game->mouseDownPosition = v2();
			}
			// }}}
		}

		if (rightMouseJustDown && !isHoveringStatus && !isHoveringCornerWheel) {
			// {{{ 
			if (game->interfaceMode == INTERFACE_PLACING_BUILDING) {
				game->interfaceMode = INTERFACE_NORMAL;
			} else {
				Unit *firstUnit = getFirstUnit(&game->localSelectedUnits);
				bool alreadyDidClick = false;
				if (firstUnit) {
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *other = &game->units[i];
						if (other->playerNumber == firstUnit->playerNumber) continue;
						if (!canSeeUnit(firstUnit->teamNumber, other)) continue;

						if (other->rect.contains(mousePos)) {
							GameCmd *gameCmd = createGameCmd();
							if (gameCmd) {
								gameCmd->type = GAME_CMD_RIGHT_CLICK_UNIT;
								gameCmd->targetId = other->id;
								if (keyPressed(KEY_SHIFT)) gameCmd->queues = true;

								createEffect(EFFECT_UNIT_TARGETED, other->id);
								alreadyDidClick = true;
								break;
							}
						}
					}

					if (!alreadyDidClick) {
						GameCmd *gameCmd = createGameCmd();
						if (gameCmd) {
							gameCmd->type = GAME_CMD_RIGHT_CLICK;
							gameCmd->position = isHoveringMinimap ? minimapHoveringWorldPos : mousePos;
							if (keyPressed(KEY_SHIFT)) gameCmd->queues = true;

							createEffect(EFFECT_ISSUED_MOVE, gameCmd->position);
						}

						if (game->interfaceMode == INTERFACE_PLACING_BUILDING) game->interfaceMode = INTERFACE_NORMAL;
					}
				}
			}
			// }}}
		}

		game->timeSinceLastControlGroupSelected += elapsed;
		if (game->playMode != PLAY_REPLAY) {
			// {{{
			int numberThatWasPressed = -1;
			for (int i = 0; i < 10; i++) if (keyJustPressed('0'+i)) numberThatWasPressed = i;
			if (numberThatWasPressed != -1) {
				if (keyPressed(KEY_CTRL)) {
					GameCmd *gameCmd = createGameCmd();
					if (gameCmd) {
						gameCmd->type = GAME_CMD_CREATE_CONTROL_GROUP;
						gameCmd->targetId = numberThatWasPressed;

						ControlGroup *group = &game->localControlGroups[gameCmd->targetId];
						ControlGroup *srcGroup = &game->localSelectedUnits;
						group->idsNum = 0;
						for (int i = 0; i < srcGroup->idsNum; i++) {
							int id = srcGroup->ids[i];
							removeFromAllControlGroups(id, true);
							addToControlGroup(group, id);
						}
					}
				} else {
					GameCmd *gameCmd = createGameCmd();
					if (gameCmd) {
						gameCmd->targetId = numberThatWasPressed;

						if (keyPressed(KEY_SHIFT)) {
							gameCmd->type = GAME_CMD_ADD_TO_CONTROL_GROUP;
							ControlGroup *group = &game->localControlGroups[gameCmd->targetId];
							ControlGroup *srcGroup = &game->localSelectedUnits;
							for (int i = 0; i < srcGroup->idsNum; i++) {
								int id = srcGroup->ids[i];
								removeFromAllControlGroups(id, true);
								addToControlGroup(group, id);
							}
						} else {
							gameCmd->type = GAME_CMD_SELECT_CONTROL_GROUP;
							ControlGroup *destGroup = &game->localSelectedUnits;
							ControlGroup *srcGroup = &game->localControlGroups[gameCmd->targetId];
							replaceControlGroup(destGroup, srcGroup);

							for (int i = 0; i < destGroup->idsNum; i++) {
								Unit *unit = getUnit(destGroup->ids[i]);
								if (!unit) continue;
								createEffect(EFFECT_UNIT_SELECTED, unit->id);
							}

							if (game->lastControlGroupSelected == numberThatWasPressed && game->timeSinceLastControlGroupSelected <= 0.2) {
								Vec2 cameraCenterWorldPos;
								cameraCenterWorldPos = game->cameraTarget;

								Unit *closestUnit = NULL;
								for (int i = 0; i < destGroup->idsNum; i++) {
									Unit *unit = getUnit(destGroup->ids[i]);
									if (!unit) continue;
									if (!closestUnit) {
										closestUnit = unit;
										continue;
									}

									if (cameraCenterWorldPos.distance(unit->position) < cameraCenterWorldPos.distance(closestUnit->position)) {
										closestUnit = unit;
									}
								}

								if (closestUnit) {
									game->cameraTarget.x = closestUnit->position.x;
									game->cameraTarget.y = closestUnit->position.y;
								}
							}
							game->lastControlGroupSelected = numberThatWasPressed;
							game->timeSinceLastControlGroupSelected = 0;
						}
					}
				}
			}

			if (keyJustPressed(KEY_ESC)) {
				GameCmd *gameCmd = createGameCmd();
				if (gameCmd) {
					gameCmd->type = GAME_CMD_ESC;
				}
			}
			// }}}
		}
	}

	for (int i = 0; i < game->fogUnitsNum; i++) {
		FogUnit *fogUnit = &game->fogUnits[i];
		bool canSee = canSeeRect(game->teamNumber, fogUnit->rect.inflate(-tileSize/2));
		if (canSee) {
			arraySpliceIndex(game->fogUnits, BUILDINGS_PER_PLAYER_MAX, sizeof(FogUnit), i);
			game->fogUnitsNum--;
			i--;
			continue;
		}
	}

	if (game->interfaceMode == INTERFACE_PLACING_BUILDING) {
		UnitInfo *info = &game->unitInfos[game->unitTypeCurrentlyPlacing];

		{
			Rect rect;
			rect.x = 0;
			rect.y = 0;
			rect.width = info->size;
			rect.height = info->size;

			rect.x = roundToNearest(mousePos.x - rect.width/2, tileSize);
			rect.y = roundToNearest(mousePos.y - rect.height/2, tileSize);
			game->unitCurrentlyPlacingRect = rect;
		}

		drawGhostImage(game->unitTypeCurrentlyPlacing, game->unitCurrentlyPlacingRect.center(), 45);
		drawRectOutline(game->unitCurrentlyPlacingRect, 2, 0x88008800);

		Vec2 mouseTile = worldToTile(mousePos);
		int rectSize = 4;
		int minX = mouseTile.x - rectSize;
		int maxX = mouseTile.x + rectSize;
		int minY = mouseTile.y - rectSize;
		int maxY = mouseTile.y + rectSize;

		for (int y = minY; y < maxY; y++) {
			for (int x = minX; x < maxX; x++) {
				bool offGrid = x < 0 || x > map->width-1 || y < 0 || y > map->height-1;
				int tileIndex = y * map->width + x;
				Rect rect = tileToWorldRect(v2(x, y));

				BitGrid *grid;
				if (game->unitTypeCurrentlyPlacing == UNIT_NEXUS) grid = baseBuildGrids[game->teamNumber];
				else grid = buildGrids[game->teamNumber];

				if (offGrid || getBit(grid, tileIndex)) {
					drawRect(rect, 0x88FF0000);
				} else {
					drawRect(rect, 0x88FFFFFF);
				}
			}
		}
	}

	{
		u8 *visionBitmapData = (u8 *)frameMalloc(map->width*map->height*4);
		u8 *lightMapData = (u8 *)frameMalloc(map->width*map->height*4);
		for (int y = 0; y < map->height; y++) {
			for (int x = 0; x < map->width; x++) {
				int index = y * map->width + x;
				if (game->teamNumber > 1 || game->teamNumber < 0) logf("Given team number %d\n", game->teamNumber);
				int vision = canSeeTile(game->teamNumber, index);
				int pixelIndex = (map->height - y - 1) * map->width + x;
				visionBitmapData[pixelIndex * 4 + 0] = 255;
				visionBitmapData[pixelIndex * 4 + 1] = 255;
				visionBitmapData[pixelIndex * 4 + 2] = 255;
				visionBitmapData[pixelIndex * 4 + 3] = vision ? 255 : 0;

				int visionLightingRef = game->visionLightingRefs[game->teamNumber][index];
				float minLight = map->minLight;
				float nextValue = minLight;
				if (visionLightingRef) nextValue = 1.0 - (visionLightingRef*map->radianceMulti / 100.0);
				if (nextValue > minLight) nextValue = minLight;
				// float nextValue = clampMap(vision, 10, 0, 0.1, 0.3);
				float currentValue = game->lightFloatData[pixelIndex];
				float newValue = lerp(currentValue, nextValue, 0.05);

				game->lightFloatData[pixelIndex] = newValue;
			}
		}

		// renderer->cameraRect = topCamera.invert().multiply(renderer->currentViewport);
		Rect bounds = renderer->cameraRect.inflate(tileSize*4);
		if (bounds.x < 0) bounds.x = 0;
		if (bounds.y < 0) bounds.y = 0;
		if (bounds.x+bounds.width > map->width*tileSize) bounds.width = map->width*tileSize - bounds.x;
		if (bounds.y+bounds.height > map->height*tileSize) bounds.height = map->height*tileSize - bounds.y;
		Vec2Tile min = worldToVec2Tile(bounds.pos());
		Vec2Tile max = worldToVec2Tile(bounds.pos() + bounds.size());

		for (int y = min.y; y < max.y; y++) {
			for (int x = min.x; x < max.x; x++) {
				int tileIndex = (map->height - y - 1) * map->width + x;
				float floatValue = game->lightFloatData[tileIndex];

// 				if (floatValue > 0) {
// 					Vec2 uv;
// 					uv.x = (float)x / 256.0;
// 					uv.y = (float)y / 256.0;
// 					float scroll = platform->time * 0.1;
// 					float noise = perlin2d(uv.x+scroll, uv.y+scroll, 10, 3);
// 					noise = noise * 2.0 - 1.0;
// 					float contrib = 1.0 - floatValue;
// 					floatValue += noise*contrib;
// 					floatValue = Clamp01(floatValue);
// 				}

				int tempY = map->height - y - 1;
				int pixelIndex = tempY * map->width + x;

				floatValue = Clamp01(floatValue);
				lightMapData[pixelIndex * 4 + 0] = 0;
				lightMapData[pixelIndex * 4 + 1] = 0;
				lightMapData[pixelIndex * 4 + 2] = 0;
				lightMapData[pixelIndex * 4 + 3] = floatValue * 255.0;

				// lightMapData[pixelIndex * 4 + 0] = 255;
				// lightMapData[pixelIndex * 4 + 1] = 255;
				// lightMapData[pixelIndex * 4 + 2] = 255;
				// lightMapData[pixelIndex * 4 + 3] = floatValue * 255.0;
			}
		}

		if (game->lightMap) destroyTexture(game->lightMap);
		game->lightMap = createTexture(map->width, map->height, lightMapData);

		if (game->visionBitTexture) destroyTexture(game->visionBitTexture);
		game->visionBitTexture = createTexture(map->width, map->height, visionBitmapData);

		if (game->debugShowVisionRefs) {
			for (int y = 0; y < map->height; y++) {
				for (int x = 0; x < map->width; x++) {
					int refIndex = y * map->width + x;
					if (game->visionRefs[game->playerNumber][refIndex] != 0) {
						Rect rect = tileToWorldRect(v2(x, y));
						drawRect(rect, 0xFF0000FF);
					}
				}
			}
		}
	}

	Matrix3 maskMatrix = mat3();
	maskMatrix.SCALE((map->width*tileSize/game->visionBitTexture->width), (map->height*tileSize/game->visionBitTexture->height));
	// maskMatrix.SCALE(1.0/(map->width*tileSize/game->visionBitTexture->width), 1.0/(map->height*tileSize/game->visionBitTexture->height));
	// maskMatrix.SCALE(1.0/tileSize, 1.0/tileSize);

	{ /// Draw attack range
		ControlGroup *group = &game->localSelectedUnits;
		for (int i = 0; i < group->idsNum; i++) {
			Unit *unit = getUnit(group->ids[i]);
			if (!unit) continue;
			drawCircle(unit->position, unit->info->range, 0x88FF0000);

			if (
				unit->type == UNIT_HEMINE ||
				unit->type == UNIT_SPIDER_DEPLOYER
			) {
				drawCircle(unit->position, unit->info->aggroRadius*tileSize, 0x88FF0000);
			}

			if (unit->actionsNum > 0) {
				Action *action = &unit->actions[0];
				if (action->type == ACTION_ATTACK) {
					Unit *target = getUnit(action->targetId);
					if (target) {
						drawLine(unit->position, target->position, 2, 0xFF888800);
					}
				}
			}
		}
	}

	if (!gamePaused) {
		if (game->playMode == PLAY_1V1_ONLINE) {
			for (int i = 0; i < TEAMS_PER_MATCH_MAX; i++) {
				if (buildingsPerTeam[i] <= 0) {
					if (i == game->teamNumber) {
						shouldLoseGame = true;
					} else {
						shouldWinGame = true;
					}
				}
			}
		}

		for (int i = 0; i < game->unitsNum; i++) { /// Real unit update loop (main unit)
			Unit *unit = &game->units[i];
			if (unit->existTime == 0) {
				unit->existTime += elapsed;
				continue;
			}
			unit->existTime += elapsed;
			unit->selected = false;

			UnitInfo *info = &game->unitInfos[unit->type];

			unit->stateTime += elapsed;
			if (unit->state != unit->lastState) {
				unit->lastState = unit->state;
				unit->stateTime = 0;
			}

			if (unit->actionsNum == 0) {
				unit->firstFrameOfAction = true;
			}

			if (unit->firstFrameOfAction) {
				endPathfinding(unit);
				unit->atLocation = false;
				unit->actionTime = 0;
				if (unit->actionsNum > 0 && unit->lastActionType != unit->actions[0].type) unit->uniqueActionTime = 0;

				if (unit->state != UNIT_STATE_CREATING) {
					unit->state = UNIT_STATE_NORMAL;
				}
			}

			if (unit->actionsNum > 0) {
				Action *action = &unit->actions[0];
				unit->lastActionType = action->type;
				unit->idleTime = 0;

				bool actionComplete = false;
				if (action->type == ACTION_MOVE) {
					if (unit->atLocation) {
						actionComplete = true;
					} else {
						stepPathfinding(unit, action->position);
					}
				} else if (action->type == ACTION_ATTACK_MOVE) {
					if (unit->atLocation) {
						actionComplete = true;
					} else {
						stepPathfinding(unit, action->position);
					}

				} else if (action->type == ACTION_PLACE_UNIT) {
					Rect buildingRect = getUnitRect(action->unitType, action->position);
					Vec2Tile tiles[TILES_UNDER_UNIT_MAX];
					int tilesNum = getTilesUnderRect(buildingRect, tiles, TILES_UNDER_UNIT_MAX);

					Rect outlineRect = buildingRect.inflate(tileSize*2);
					Vec2Tile outlineTiles[TILES_UNDER_UNIT_MAX];
					int outlineTilesNum = getOutlineTilesUnderRect(outlineRect, outlineTiles, TILES_UNDER_UNIT_MAX);

					Vec2Tile closestOutlineTile = {};
					for (int i = 0; i < outlineTilesNum; i++) {
						Vec2Tile tile = outlineTiles[i];
						int tileIndex = tile.y * map->width + tile.x;
						if (game->collisionData[tileIndex] != 0) continue;

						if (closestOutlineTile.isZero() || tileToWorld(tile).distance(unit->position) < tileToWorld(closestOutlineTile).distance(unit->position)) {
							closestOutlineTile = tile;
						}
					}

					bool canBuild = true;

					if (closestOutlineTile.isZero()) {
						if (unit->playerNumber == game->playerNumber) infof("Nowhere to stand to place building\n");
						canBuild = false;
						actionComplete = true;
					}

					if (!hasPopFor(unit->playerNumber, action->unitType)) {
						canBuild = false;
						actionComplete = true;
					}

					if (canBuild) {
						stepPathfinding(unit, tileToWorld(closestOutlineTile));
						if (!unit->atLocation) canBuild = false;
					}

					if (canBuild) {
						for (int i = 0; i < game->unitsNum; i++) {
							Unit *possibleBlockingUnit = &game->units[i];
							if (buildingRect.intersects(possibleBlockingUnit->rect)) {
								canBuild = false;

								if (possibleBlockingUnit->playerNumber != unit->playerNumber) { //@todo Also if a friendly unit was stopped/halted
									infof("Building was blocked\n");
									canBuild = false;
									actionComplete = true;
								} else {
									if (possibleBlockingUnit->actionsNum == 0) {
										Vec2Tile closestOutlineTile = {};
										for (int i = 0; i < outlineTilesNum; i++) {
											Vec2Tile tile = outlineTiles[i];
											int tileIndex = tile.y * map->width + tile.x;
											if (game->collisionData[tileIndex] != 0) continue;

											if (
												closestOutlineTile.isZero() ||
												tileToWorld(tile).distance(possibleBlockingUnit->position) < tileToWorld(closestOutlineTile).distance(possibleBlockingUnit->position)
											) {
												closestOutlineTile = tile;
											}
										}

										if (closestOutlineTile.isZero()) {
											if (game->playerNumber == unit->playerNumber) infof("Nowhere for ally to stand to place building\n");
											canBuild = false;
											actionComplete = true;
											break;
										}

										Action *action = createAction(possibleBlockingUnit, false);
										if (action) {
											action->type = ACTION_MOVE;
											action->position = tileToWorld(closestOutlineTile);
										}
									}
								}
							}
						}
					}

					{
						for (int i = 0; i < tilesNum; i++) {
							Vec2Tile tile = tiles[i];
							bool offGrid = tile.x < 0 || tile.x > map->width-1 || tile.y < 0 || tile.y > map->height-1;
							int tileIndex = tile.y * map->width + tile.x;

							BitGrid *grid;
							if (action->unitType == UNIT_NEXUS) grid = baseBuildGrids[unit->teamNumber];
							else grid = buildGrids[unit->teamNumber];

							if (offGrid || getBit(grid, tileIndex)) {
								if (game->playerNumber == unit->playerNumber) infof("Can't build on top of terrain\n");
								canBuild = false;
								actionComplete = true;
								break;
							}
						}
					}

					if (unit->playerNumber == game->playerNumber) { /// Drawing stuff
						Vec2 destPos = tileToWorld(closestOutlineTile);
						drawRectOutline(buildingRect, 2, 0x8800FF00);
						// drawCircleOutline(destPos, 40, 0x8800FF00, 1);
						drawCircle(destPos, 10, 0x8800FF00);

						drawLine(unit->position, destPos, 2, 0x8800FF00);
						drawLine(destPos, buildingRect.center(), 2, 0x8800FF00);
					}

					if (canBuild) {
						Unit *newUnit = createUnit(unit->playerNumber, unit->teamNumber, action->unitType);
						if (newUnit) {
							newUnit->position = action->position;
							newUnit->state = UNIT_STATE_CREATING;
						} else {
							logf("Failed to place building for a weird reason line: %d\n", __LINE__);
						}
						actionComplete = true;
					}
				} else if (action->type == ACTION_ATTACK) {
					if (unit->firstFrameOfAction) unit->state = UNIT_STATE_ATTACK_APPROACH;
					unit->attackTime += elapsed;
					Unit *target = getUnit(action->targetId);
					if (target && !canSeeUnit(unit->teamNumber, target)) target = NULL;

					if (target) {
						Vec2 closestPoint = target->rect.getClosestPoint(unit->position);
						bool inRange = unit->position.distance(closestPoint) < unit->info->range;

						if (unit->state == UNIT_STATE_ATTACK_APPROACH) {
							if (inRange) {
								unit->state = UNIT_STATE_PRE_ATTACK;
								endPathfinding(unit);
							} else {
								unit->attackTime -= elapsed;
								unit->attackTime += elapsed * (unit->speed / unit->info->speed);
								stepPathfinding(unit, target->position);
							}
						} else if (unit->state == UNIT_STATE_PRE_ATTACK) {
							if (unit->attackTime >= unit->info->preAttackDelay) {
								if (unit->postAttackCooldown <= 0) {
									int channelId = playWorldSound(frameSprintf("assets/audio/units/%s/attack.ogg", unit->info->formalName), unit->position);
									Channel *channel = getChannel(channelId);
									if (channel) {
										channel->sound->tweakVolume = 0.5;
									}

									Bullet *bullet = createBullet(unit, target);
									bullet->type = unit->info->bulletType;
									bullet->speed = unit->info->bulletSpeed;
									bullet->damage = unit->info->damage;
									bullet->slowDamage = unit->info->slowDamage;
									bullet->heatDamage = unit->info->heatDamage;

									unit->state = UNIT_STATE_POST_ATTACK;
									unit->attackTime = 0;
									unit->postAttackCooldown = unit->info->postAttackDelay;
								}
							}
						} else if (unit->state == UNIT_STATE_POST_ATTACK) {
							if (unit->attackTime >= unit->info->postAttackDelay) {
								if (!action->forceTarget) {
									doPassiveAggro(unit);
									target = getUnit(action->targetId);
									if (!target) actionComplete = true;
								}

								if (unit->postAttackCooldown <= 0 && inRange) {
									unit->state = UNIT_STATE_PRE_ATTACK;
								} else {
									unit->state = UNIT_STATE_ATTACK_APPROACH;
								}
								unit->attackTime = 0;
							}
						}
					} else {
						actionComplete = true;
					}
				} else if (action->type == ACTION_CONSTRUCT) {
					if (unit->actionTime >= game->unitInfos[action->unitType].creationTime) {
						if (unit->rallyPoint.isZero()) {
							unit->rallyPoint.x = unit->rect.x + unit->rect.width/2;
							unit->rallyPoint.y = unit->rect.y + unit->rect.height + tileSize;
						}

						if (unit->type == UNIT_PANTHER) unit->rallyPoint = unit->position;

						Rect outlineRect = unit->rect.inflate(game->unitInfos[action->unitType].size * 2);
						Vec2Tile outlineTiles[TILES_UNDER_UNIT_MAX];
						int outlineTilesNum = getOutlineTilesUnderRect(outlineRect, outlineTiles, TILES_UNDER_UNIT_MAX);

						Vec2Tile closestOutlineTile = {};
						for (int i = 0; i < outlineTilesNum; i++) {
							Vec2Tile tile = outlineTiles[i];
							int tileIndex = tile.y * map->width + tile.x;
							if (game->collisionData[tileIndex] != 0) continue;

							if (closestOutlineTile.isZero() || tileToWorld(tile).distance(unit->rallyPoint) < tileToWorld(closestOutlineTile).distance(unit->rallyPoint)) {
								closestOutlineTile = tile;
							}
						}

						bool canBuild = true;
						if (closestOutlineTile.isZero()) canBuild = false;

						if (canBuild) {
							Vec2 spawnPoint = tileToWorld(closestOutlineTile);

							game->population[unit->playerNumber] -= action->populationPrice; //@todo createUnitFromAction
							Unit *newUnit = createUnit(unit->playerNumber, unit->teamNumber, action->unitType);
							if (newUnit) {
								newUnit->position.x = spawnPoint.x;
								newUnit->position.y = spawnPoint.y;

								createEffect(EFFECT_UNIT_FULLY_CONSTRUCTED, unit->position);

								createRightClickAction(newUnit, unit->rallyPoint, false);
							} else {
								logf("Failed to spawn unit for a weird reason line: %d\n", __LINE__);
							}

							actionComplete = true;
						}
					} else {
						UnitType unitType = action->unitType;
						UnitInfo *info = &game->unitInfos[action->unitType];
					}
				} else if (action->type == ACTION_GATHER) {
					if (unit->firstFrameOfAction) {
						unit->state = UNIT_STATE_GOING_TO_GATHER;
						if (unit->lastGatherPosition.isZero()) unit->lastGatherPosition = unit->position;
					}

					int amountCanCarry = 5;

					if (unit->state == UNIT_STATE_GOING_TO_GATHER) {
						bool alreadyMining = false;

						Unit *currentMinerals = getUnit(action->targetId);
						if (currentMinerals) {
							unit->lastGatherPosition = currentMinerals->position;
							for (int i = 0; i < currentMinerals->unitsMiningFromThisNum; i++) {
								if (currentMinerals->unitsMiningFromThis[i] == unit->id) {
									alreadyMining = true;
									break;
								}
							}

							if (!alreadyMining) {
								for (int i = 0; i < UNITS_MINING_FROM_THIS_MAX; i++) {
									if (currentMinerals->unitsMiningFromThisNum < UNITS_MINING_FROM_THIS_MAX) {
										currentMinerals->unitsMiningFromThis[currentMinerals->unitsMiningFromThisNum++] = unit->id;
										alreadyMining = true;
										break;
									}
								}
							}
						}

						const int MINERALS_NEAR_BY_MAX = 16;
						Unit *mineralsNearBy[MINERALS_NEAR_BY_MAX];
						int mineralsNearByNum = 0;

						Vec2 nearByPoints[MINERALS_NEAR_BY_MAX];
						int nearByPointsNum = 0;
						nearByPoints[nearByPointsNum++] = unit->lastGatherPosition;

						if (!alreadyMining) {
							for (;;) {
								bool addedSomething = false;
								for (int i = 0; i < game->unitsNum; i++) {
									Unit *possibleMinerals = &game->units[i];
									if (possibleMinerals->type != UNIT_MINERALS) continue;
									if (mineralsNearByNum > MINERALS_NEAR_BY_MAX-1) {
										logf("Nearby minerals list too long\n");
										continue;
									}

									bool shouldAdd = false;

									for (int i = 0; i < nearByPointsNum; i++) {
										if (possibleMinerals->position.distance(nearByPoints[i]) <= MIN_MINERAL_DISTANCE) {
											shouldAdd = true;
										}
									}

									if (shouldAdd) {
										for (int i = 0; i < mineralsNearByNum; i++) {
											if (mineralsNearBy[i] == possibleMinerals) {
												shouldAdd = false;
												break;
											}
										}
									}

									if (shouldAdd) {
										mineralsNearBy[mineralsNearByNum++] = possibleMinerals;
										if (nearByPointsNum < MINERALS_NEAR_BY_MAX-1) nearByPoints[nearByPointsNum++] = possibleMinerals->position;
										addedSomething = true;
									}
								}

								if (!addedSomething) break;
							}

							action->targetId = -1;
							for (int i = 0; i < mineralsNearByNum; i++) {
								Unit *minerals = mineralsNearBy[i];

								bool canMine = false;
								for (int i = 0; i < minerals->unitsMiningFromThisNum; i++) {
									if (minerals->unitsMiningFromThis[i] == unit->id) {
										canMine = true;
										break;
									}
								}

								if (!canMine) {
									if (minerals->unitsMiningFromThisNum < UNITS_MINING_FROM_THIS_MAX) {
										minerals->unitsMiningFromThis[minerals->unitsMiningFromThisNum++] = unit->id;
										canMine = true;
									}
								}

								if (canMine) {
									action->targetId = minerals->id;
									break;
								}
							}
						}

						Unit *minerals = getUnit(action->targetId);
						if (minerals) {
							if (unit->atLocation) {
								unit->state = UNIT_STATE_GATHERING;
								unit->gatheringTimer = GATHERING_TIMER_MAX;
							} else {
								stepPathfinding(unit, minerals->position);
							}
						} else {
							if (game->turn > 0) createAlert(ALERT_OUT_OF_MINERALS, unit);
							actionComplete = true;
						}
					} else if (unit->state == UNIT_STATE_GATHERING) {
						unit->gatheringTimer -= elapsed;
						if (unit->gatheringTimer <= 0) {
							Unit *minerals = getUnit(action->targetId);
							if (minerals) {
								minerals->mineralAmount -= amountCanCarry;
								if (minerals->mineralAmount <= 0) {
									dealDamage(minerals, unit, minerals->hp);
								}
							}
							unit->state = UNIT_STATE_RETURNING_RESOURCES;
							unit->atLocation = false;
							endPathfinding(unit);
						}
					} else if (unit->state == UNIT_STATE_RETURNING_RESOURCES) {
						Unit *closestBase = NULL;
						for (int i = 0; i < game->unitsNum; i++) {
							Unit *base = &game->units[i];
							if (base->type != UNIT_NEXUS) continue;
							if (base->teamNumber != unit->teamNumber) continue;
							if (base->state == UNIT_STATE_CREATING) continue; 
							//@incomplete Check if base is actually reachable

							if (!closestBase) {
								closestBase = base;
								continue;
							}

							float baseDist = unit->position.distance(base->position);
							float closestBaseDist = unit->position.distance(closestBase->position);
							if (baseDist < closestBaseDist) closestBase = base;
						}

						if (closestBase) {
							if (unit->atLocation) {
								if (unit->actionsNum == 1) {
									game->minerals[unit->playerNumber] += amountCanCarry;
									unit->state = UNIT_STATE_GOING_TO_GATHER;
									unit->atLocation = false;
									endPathfinding(unit);
								} else {
									actionComplete = true;
								}
							} else {
								stepPathfinding(unit, closestBase->position);
							}
						} else {
							actionComplete = true;
						}
					}
				} else if (action->type == ACTION_EXPLODE) {
					if (unit->type == UNIT_HEMINE) {
						if (!renderer->screenRect.contains(unit->position)) {
							createAlert(ALERT_HEMINE_EXPLODE, 0, unit->position);
							createAlert(ALERT_HEMINE_EXPLODE, 1, unit->position);
						}
						for (int i = 0; i < game->unitsNum; i++) {
							Unit *unitToDamage = &game->units[i];
							if (unitToDamage->playerNumber == -1) continue;
							float dist = unitToDamage->position.distance(unit->position);
							if (dist > unit->info->range) continue;

							dealDamage(unitToDamage, unit, unit->info->damage, unitToDamage->position);
						}
					} else if (unit->type == UNIT_SPIDER_DEPLOYER) {
						for (int i = 0; i < globals->spiderMineDeployCount; i++) {
							Unit *newUnit = createUnit(unit->playerNumber, unit->teamNumber, UNIT_SPIDER_MINE);
							if (newUnit) {
								newUnit->position = unit->position;
							}
						}
					}

					unit->hp = 0;
					unit->canRemove = true;
					actionComplete = true;
				} else if (action->type == ACTION_AREA_SCAN) {
				} else if (action->type == ACTION_SCAN_DART) {
					if (unit->scanDartPosition.isZero()) {
						unit->scanDartPosition = unit->position;
						unit->scanDartHitWall = false;
						unit->scanDartRads = radsBetween(unit->position, action->position);
						unit->scanDartLastShotTime = 0;
						unit->scanDartLightId = createLight(unit->teamNumber);
					}
					actionComplete = true;
				} else if (action->type == ACTION_THROW_FIRE_BOMB) {
					if (sim->fireBombsNum < FIRE_BOMBS_MAX) {
						float range = 8*32;

						FireBomb *fireBomb = &sim->fireBombs[sim->fireBombsNum++];

						memset(fireBomb, 0, sizeof(FireBomb));
						fireBomb->start = unit->position;

						fireBomb->degMinMax.x = rndFloat(0, 360);
						fireBomb->degMinMax.y = fireBomb->degMinMax.x + (rndFloat(90, 180) * (rndBool() ? -1 : 1));

						float rads = radsBetween(unit->position, action->position);
						fireBomb->end.x = unit->position.x + cos(rads)*range;
						fireBomb->end.y = unit->position.y + sin(rads)*range;
					}
					actionComplete = true;
				} else if (action->type == ACTION_STOP) {
				} else if (action->type == ACTION_IDLE2) {
					actionComplete = true;
				} else if (action->type == ACTION_DEATH) {
					if (unit->firstFrameOfAction) {
						unit->canRemove = true;
					}

					float length = unit->info->deathLength;
					if (length == 0) length = 0.00001;
					if (unit->actionTime > length) {
						unit->canRemove = true;
						actionComplete = true;
					}
				}

				unit->firstFrameOfAction = false;
				if (actionComplete) {
					arraySplice(unit->actions, sizeof(Action) * ACTIONS_MAX, sizeof(Action), action);
					unit->actionsNum--;
					unit->firstFrameOfAction = true;
				}

				unit->actionTime += elapsed;
				unit->uniqueActionTime += elapsed;
			} else {
				unit->idleTime += elapsed;
			}

			if (unit->teamNumber == game->teamNumber) {
				for (int i = 0; i < unit->actionsNum; i++) {
					Action *action = &unit->actions[i];
					if (action->type == ACTION_PLACE_UNIT) {
						drawGhostImage(action->unitType, action->position, 45);
					}
				}
			}

			bool canBeAggroedPassively = true;
			if (unit->info->isBuilding) canBeAggroedPassively = false;
			if (unit->actionsNum > 0 && unit->actions[0].type != ACTION_ATTACK_MOVE) canBeAggroedPassively = false;
			if (canBeAggroedPassively) doPassiveAggro(unit);

			if (unit->state == UNIT_STATE_CREATING) {
				if (unit->stateTime > unit->info->creationTime) {
					unit->state = UNIT_STATE_NORMAL;
				}
			}

			unit->timeSinceAttackAlert += elapsed;
			unit->postAttackCooldown -= elapsed;
			if (renderer->screenRect.contains(unit->position)) unit->timeSinceAttackAlert = 0;

			if (unit->type == UNIT_MINERALS) {
				for (int i = 0; i < unit->unitsMiningFromThisNum; i++) {
					Unit *workerUnit = getUnit(unit->unitsMiningFromThis[i]);
					bool shouldSplice = true;

					if (workerUnit && workerUnit->actionsNum > 0 && workerUnit->actions[0].type == ACTION_GATHER && workerUnit->actions[0].targetId == unit->id) {
						shouldSplice = false;
					}

					if (shouldSplice) {
						arraySpliceIndex(unit->unitsMiningFromThis, unit->unitsMiningFromThisNum, sizeof(int), i);
						unit->unitsMiningFromThisNum--;
						i--;
						continue;
					}
				}

			}

			if (unit->type == UNIT_HEMINE || unit->type == UNIT_SPIDER_DEPLOYER) {
				for (int i = 0; i < game->unitsNum; i++) {
					Unit *other = &game->units[i];
					if (other->teamNumber == unit->teamNumber) continue;
					if (other->info->isBuilding) continue;
					if (unit->position.distance(other->position) < unit->info->aggroRadius*tileSize) {
						Action *action = createAction(unit, false);
						if (action) {
							action->type = ACTION_EXPLODE;
						}
					}
				}
			}

			if (unit->type == UNIT_SPIDER_MINE) {
				if (unit->existTime >= globals->spiderMineLifeTime) {
					unit->hp = 0;
					unit->canRemove = true;
				}
			}

			if (unit->slow > 0) unit->slow--;

			if (unit->heat > 200) unit->heat = 200;
			if (unit->heat > 0 && ticks % 4 == 0) unit->heat--;
			if (unit->heat > 100) {
				dealDamage(unit, NULL, 2.0 / 60.0);
			}

			float heatDistance = 64;
			int closeUnitsNum = 0;
			Unit **closeUnits = getUnitsInCircle(unit->position, heatDistance, &closeUnitsNum);
			for (int i = 0; i < closeUnitsNum; i++) {
				Unit *otherUnit = closeUnits[i];
				if (otherUnit == unit) continue;
				Vec2 midPoint = (otherUnit->position + unit->position) / 2;

				Vec2 unitPoint = unit->rect.getClosestPoint(midPoint);
				Vec2 otherUnitPoint = otherUnit->rect.getClosestPoint(midPoint);

				float dist = unitPoint.distance(otherUnitPoint);
				float perc = 1.0 - (dist / heatDistance);
				if (unit->heat > 100) otherUnit->heat += 0.5 * perc;
			}

			if (!unit->scanDartPosition.isZero()) {
				float maxTime = 5;
				float perc = unit->scanDartLastShotTime / maxTime;

				float speed = 10;
				float radius = 5;

				Vec2 velo;
				velo.x = cos(unit->scanDartRads) * speed;
				velo.y = sin(unit->scanDartRads) * speed;

				Vec2 newPos = unit->scanDartPosition + velo;
				Vec2Tile newTilePos = worldToVec2Tile(newPos);
				int newTileIndex = newTilePos.y * map->width + newTilePos.x;

				if (game->collisionData[newTileIndex] != 0) unit->scanDartHitWall = true;

				if (unit->scanDartHitWall) newPos = unit->scanDartPosition;

				propagateLight(unit->scanDartLightId, worldToVec2Tile(unit->scanDartPosition), radius, 1);
				unit->scanDartPosition = newPos;
				drawCircle(unit->scanDartPosition, 3, lerpColor(0xFFFF0000, 0xFF000000, timePhase(game->time, 2)));

				unit->scanDartLastShotTime += elapsed;

				if (perc >= 1) {
					destroyLight(unit->scanDartLightId);
					unit->scanDartPosition = v2();
				}
			}

			for (int i = 0; i < sim->firePuddlesNum; i++) {
				FirePuddle *puddle = &sim->firePuddles[i];
				logf("Fire puddle code is commented!\n");
				// if (unit->rect.containsCircle(puddle->worldPosition, puddle->worldRadius)) {
				// 	for (int i = 0; i < puddle->tilesNum; i++) {
				// 		Vec2Tile tile = puddle->tiles[i];
				// 		Rect rect = tileToWorldRect(tile);
				// 		if (unit->rect.intersects(rect)) {
				// 			unit->heat += 0.1;
				// 		}
				// 	}
				// }
			}

			/// Boids
			if (!unit->info->isBuilding && !unit->info->noBoids) {
				int boidNearUnitsNum = 0;
				Unit **boidNearUnits = getUnitsInCircle(unit->position, 128, &boidNearUnitsNum);
				for (int i = 0; i < boidNearUnitsNum; i++) {
					Unit *otherUnit = boidNearUnits[i];
					bool shouldSplice = false;

					if (unit->info->flying != otherUnit->info->flying) shouldSplice = true;

					if (shouldSplice) {
						arraySpliceIndex(boidNearUnits, boidNearUnitsNum, sizeof(Unit *), i);
						i--;
						boidNearUnitsNum--;
						continue;
					}
				}

				unit->accel = v2();

				float maxForce = 10;
				float maxAccel = 10;
				float maxSpeed = unit->info->speed;

				Vec2 seekForce = v2();
				if (!unit->seekTarget.isZero()) {
					Vec2 direction;
					direction.x = unit->seekTarget.x - unit->position.x;
					direction.y = unit->seekTarget.y - unit->position.y;
					direction = direction.normalize();

					if (direction.length() > 0) {
						seekForce = direction.multiply(maxForce);
					}
				}

				Vec2 flowFieldForce = v2();
				if (unit->flowField) {
					FlowField *field = unit->flowField;
					Vec2 unitTopRightWorldPos;
					unitTopRightWorldPos.x = unit->position.x - unit->info->size/2;
					unitTopRightWorldPos.y = unit->position.y - unit->info->size/2;

					Vec2 unitFloatTile;
					unitFloatTile.x = unit->position.x / (float)tileSize;
					unitFloatTile.y = unit->position.y / (float)tileSize;

					Vec2 floor = worldToTile(unitTopRightWorldPos);
					// var floor = agent.position.floor(); //Top left Coordinate of the 4

					//The 4 weights we'll interpolate, see http://en.wikipedia.org/wiki/File:Bilininterp.png for the coordinates
					int f00i = floor.y * map->width + floor.x;
					int f01i = (floor.y + 1) * map->width + (floor.x);
					int f10i = (floor.y) * map->width + (floor.x + 1);
					int f11i = (floor.y + 1) * map->width + (floor.x + 1);

					char f00c = field->data[f00i];
					char f01c = field->data[f01i];
					char f10c = field->data[f10i];
					char f11c = field->data[f11i];

					Vec2 f00 = v2();
					Vec2 f01 = v2();
					Vec2 f10 = v2();
					Vec2 f11 = v2();

					if (f00c != -1) f00 = degToVec2(charToDeg(f00c));
					if (f01c != -1) f01 = degToVec2(charToDeg(f01c));
					if (f10c != -1) f10 = degToVec2(charToDeg(f10c));
					if (f11c != -1) f11 = degToVec2(charToDeg(f11c));

					// drawRect(tileToWorldRect(v2(f00i % map->width, f00i / map->width)), 0x88FFFF00);
					// drawRect(tileToWorldRect(v2(f01i % map->width, f01i / map->width)), 0x88FFFF00);
					// drawRect(tileToWorldRect(v2(f10i % map->width, f10i / map->width)), 0x88FFFF00);
					// drawRect(tileToWorldRect(v2(f11i % map->width, f11i / map->width)), 0x88FFFF00);

					//Do the x interpolations
					float xWeight = unitFloatTile.x - floor.x;

					// var top = f00.mul(1 - xWeight).plus(f10.mul(xWeight));
					Vec2 top;
					top.x = (f00.x * (1.0 - xWeight)) + (f10.x * xWeight);
					top.y = (f00.y * (1.0 - xWeight)) + (f10.y * xWeight);

					// var bottom = f01.mul(1 - xWeight).plus(f11.mul(xWeight));
					Vec2 bottom;
					bottom.x = (f01.x * (1.0 - xWeight)) + (f11.x * xWeight);
					bottom.y = (f01.y * (1.0 - xWeight)) + (f11.y * xWeight);

					//Do the y interpolation
					// var yWeight = agent.position.y - floor.y;
					float yWeight = unitFloatTile.y - floor.y;

					//This is now the direction we want to be travelling in (needs to be normalized)
					// var direction = top.mul(1 - yWeight).plus(bottom.mul(yWeight)).normalize();
					Vec2 direction;
					direction.x = (top.x * (1.0 - yWeight)) + (bottom.x * yWeight);
					direction.y = (top.y * (1.0 - yWeight)) + (bottom.y * yWeight);
					direction = direction.normalize();

					//If we are centered on a grid square with no vector this will happen
					if (direction.length() != 0) {
						flowFieldForce = direction.multiply(maxForce);
					}
					// if (direction.length() != 0) {
					// 	Vec2 force = direction.multiply(maxSpeed);
					// 	force = force.subtract(unit->velo);
					// 	if (force.length() > maxForce) force = force.normalize().multiply(maxForce);
					// 	flowFieldForce = force;
					// }
				}

				Vec2 seperationForce = v2();
				{
					float seperationAmount = unit->info->size;
					if (unit->state == UNIT_STATE_GOING_TO_GATHER) seperationAmount = 4;
					if (unit->state == UNIT_STATE_GATHERING) seperationAmount = 4;
					if (unit->state == UNIT_STATE_RETURNING_RESOURCES) seperationAmount = 4;

					Vec2 force = v2();
					int count = 0;

					for (int i = 0; i < boidNearUnitsNum; i++) {
						Unit *otherUnit = boidNearUnits[i];
						if (otherUnit->info->noBoids) continue;
						float d = unit->position.distance(otherUnit->position);

						if (d > 0 && d < seperationAmount) {
							Vec2 diff = unit->position.subtract(otherUnit->position);
							diff = diff.normalize();
							diff = diff.divide(d);
							force = force + diff;
							count++;
						}
					}

					if (count > 0) force = force.divide(count);
					force = force.normalize();

					if (force.length() > 0) {
						seperationForce = force.multiply(maxForce);
					}
				}

				bool doAlignment = true;
				if (unit->state == UNIT_STATE_GOING_TO_GATHER) doAlignment = false;
				if (unit->state == UNIT_STATE_GATHERING) doAlignment = false;
				if (unit->state == UNIT_STATE_RETURNING_RESOURCES) doAlignment = false;

				Vec2 alignmentForce = v2();
				float minVeloForAlignment = 5;
				if (doAlignment) {
					if (unit->velo.length() >= minVeloForAlignment) {
						float neighborDist = 64;
						Vec2 force = v2();
						int count = 0;

						for (int i = 0; i < boidNearUnitsNum; i++) {
							Unit *otherUnit = boidNearUnits[i];
							if (otherUnit->info->noBoids) continue;
							if (otherUnit->velo.length() < minVeloForAlignment) continue;
							float d = unit->position.distance(otherUnit->position);
							if (d > 0 && d < neighborDist) {
								force = force + otherUnit->velo;
								count++;
							}
						}

						if (count > 0) force = force.divide(count);
						force = force.normalize();

						if (force.length() > 0) {
							force = force.multiply(maxSpeed);
							force = force.subtract(unit->velo);
							if (force.length() > maxForce) force = force.normalize().multiply(maxForce);

							alignmentForce = force;
						}
					}
				}

				Vec2 wallRejectionForce = v2();
				if (!unit->info->flying) {
					Vec2 currentTile = worldToTile(unit->position);
					Vec2 allDesired = v2();
					for (int i = 0; i < 8; i++) {
						Vec2 neighbor;
						if (i == 0) neighbor = v2(currentTile.x-1, currentTile.y);
						if (i == 1) neighbor = v2(currentTile.x+1, currentTile.y);
						if (i == 2) neighbor = v2(currentTile.x, currentTile.y-1);
						if (i == 3) neighbor = v2(currentTile.x, currentTile.y+1);
						if (i == 4) neighbor = v2(currentTile.x-1, currentTile.y-1);
						if (i == 5) neighbor = v2(currentTile.x+1, currentTile.y+1);
						if (i == 6) neighbor = v2(currentTile.x+1, currentTile.y-1);
						if (i == 7) neighbor = v2(currentTile.x-1, currentTile.y+1);

						int neighborIndex = neighbor.y * map->width + neighbor.x;
						if (neighbor.x < 0 || neighbor.x > map->width-1) continue;
						if (neighbor.y < 0 || neighbor.y > map->height-1) continue;

						if (game->collisionData[neighborIndex] != 0) {
							Vec2 neighborWorldSpace = tileToWorld(neighbor);

							float distMin = tileSize * 1.25;
							float dist = unit->position.distance(neighborWorldSpace);
							if (dist < distMin) {
								Vec2 direction = neighborWorldSpace - unit->position;
								direction = direction.normalize();

								float perc = 1.0 - dist/(float)tileSize;

								if (perc < 0) perc *= -1;
								allDesired = allDesired + direction.multiply(perc * maxForce);
							}
						}
					}

					if (allDesired.length() != 0) {
						Vec2 force = allDesired;
						force = unit->velo - force;

						if (force.length() > maxForce) force = force.normalize().multiply(maxForce);
						wallRejectionForce = force;
					}
				}

				Vec2 eWallRejectionForce = v2();
				if (!unit->info->flying) {
					Rect rect = unit->rect;
					rect = rect.inflate(-unit->rect.width/2);

					Vec2 tl = v2(rect.x, rect.y);
					Vec2 tr = v2(rect.x+rect.width, rect.y);
					Vec2 bl = v2(rect.x, rect.y+rect.height);
					Vec2 br = v2(rect.x+rect.width, rect.y+rect.height);

					tl = worldToTile(tl);
					tr = worldToTile(tr);
					bl = worldToTile(bl);
					br = worldToTile(br);

					Vec2 force = v2();

					Vec2 *toCorrect[4];
					toCorrect[0] = &tl;
					toCorrect[1] = &tr;
					toCorrect[2] = &bl;
					toCorrect[3] = &br;
					for (int i = 0; i < 4; i++) {
						Vec2 *pt = toCorrect[i];
						if (pt->x < 0) pt->x = 0;
						if (pt->x > map->width-1) pt->x = map->width-1;
						if (pt->y < 0) pt->y = 0;
						if (pt->y > map->height-1) pt->y = map->height-1;
					}

					if (game->collisionData[(int)(tl.y * map->width + tl.x)] != 0) force = force + v2(1, 1);
					if (game->collisionData[(int)(tr.y * map->width + tr.x)] != 0) force = force + v2(-1, 1);
					if (game->collisionData[(int)(bl.y * map->width + bl.x)] != 0) force = force + v2(1, -1);
					if (game->collisionData[(int)(br.y * map->width + br.x)] != 0) force = force + v2(-1, -1);

					if (force.length() != 0) eWallRejectionForce = force.normalize().multiply(maxForce);
				}

				Vec2 repulsionForce = v2();
				{
					for (int i = 0; i < repulsionCirclesNum; i++) {
						RepulsionCircle *circle = &repulsionCircles[i];
						Unit *unitCreating = getUnit(circle->createdByUnitId);
						if (!unitCreating) continue;
						if (unitCreating->playerNumber == unit->playerNumber) continue;

						if (contains(makeCircle(circle->position, circle->radius), unit->position)) {
							Vec2 direction;
							direction.x = unit->position.x - circle->position.x;
							direction.y = unit->position.y - circle->position.y;
							direction = direction.normalize();

							float dist = unit->position.distance(circle->position);
							float perc = 1.0 - dist/circle->radius;

							repulsionForce = repulsionForce + direction.multiply(perc * maxForce);
						}
					}
				}

				Vec2 facingForces = v2();
				facingForces = facingForces + seekForce;
				facingForces = facingForces + flowFieldForce;
				facingForces = facingForces + alignmentForce;
				facingForces = facingForces + repulsionForce*0.5;
				facingForces = facingForces + seperationForce*1.5;
				facingForces = facingForces + wallRejectionForce*3;
				facingForces = facingForces + eWallRejectionForce*3;
				unit->movementFacingVelo = facingForces.normalize();

				Vec2 nonFacingForces = v2();

				Vec2 totalForce = facingForces + nonFacingForces;

				unit->accel = unit->accel + totalForce;
				if (unit->accel.length() > maxAccel) unit->accel = unit->accel.normalize().multiply(maxAccel);

				unit->velo = unit->velo + unit->accel;
				if (unit->velo.length() > maxSpeed) unit->velo = unit->velo.normalize().multiply(maxSpeed);

				unit->velo = unit->velo.multiply(0.75);
				if (unit->slow > 0) {
					float slowPerc = (float)unit->slow / 100.0;
					if (slowPerc > 1) slowPerc = 1;
					slowPerc *= 0.5;

					unit->velo = unit->velo * (1.0 - slowPerc);
				}

				if (game->debugShowPathfindingData) {
					float visualScale = 30;
					drawLine(unit->position, unit->position + totalForce.multiply(visualScale), 4, 0xFFFF00FF);
					drawLine(unit->position, unit->position + seekForce.multiply(visualScale), 2, 0xFF00FF00);
					drawLine(unit->position, unit->position + flowFieldForce.multiply(visualScale), 2, 0xFF0000FF);
					drawLine(unit->position, unit->position + repulsionForce.multiply(visualScale), 2, 0xFFFFFFFF);
					drawLine(unit->position, unit->position + alignmentForce.multiply(visualScale), 2, 0xFF00FFFF);
					drawLine(unit->position, unit->position + seperationForce.multiply(visualScale), 2, 0xFFFF0000);
					drawLine(unit->position, unit->position + wallRejectionForce.multiply(visualScale), 2, 0xFFFFFF00);
					drawLine(unit->position, unit->position + eWallRejectionForce.multiply(visualScale), 2, lerpColor(0x00FF0000, 0xFFFF0000, secondPhase));
				}

				if (unit->unmovable) unit->velo = v2();

				arraySpliceIndex(unit->prevPositions, PREV_POSITIONS_MAX, sizeof(Vec2), 0);
				unit->position = unit->position + unit->velo;

				unit->seekTarget = v2();

				float distLeft = unit->position.distance(tileToWorld(unit->tileNavGoal));
				if (distLeft < unit->velo.length() || distLeft <= tileSize) {
					unit->atLocation = true;
					endPathfinding(unit);
				}
			}

			unit->prevPositions[PREV_POSITIONS_MAX-1] = unit->visualPosition;

			if (unit->visualPosition.isZero()) unit->visualPosition = unit->position;
			unit->visualPosition = lerp(unit->visualPosition, unit->position, 0.1);

			Vec2 prevPositionAvg = v2();
			for (int i = 0; i < PREV_POSITIONS_MAX; i++) {
				prevPositionAvg = prevPositionAvg + unit->prevPositions[i];
			}
			prevPositionAvg = prevPositionAvg * (1.0/(float)PREV_POSITIONS_MAX);
			unit->speed = unit->visualPosition.distance(prevPositionAvg);
		}
	}

	renderer->defaultMaskTexture = game->visionBitTexture;
	renderer->defaultMaskMatrix = maskMatrix;

	if (game->debugDisableFogOfWar) {
		renderer->defaultMaskTexture = NULL;
	}

	for (int i = 0; i < game->unitsNum; i++) { /// Unit draw loop
		Unit *unit = &game->units[i];

		if (unit->teamNumber != game->teamNumber && !canSeeUnit(game->teamNumber, unit) && !game->debugDisableFogOfWar) continue;

		if (unit->movementFacingVelo.length() >= 0.5) unit->rotation = unit->movementFacingVelo.toDeg();

		if (unit->actionsNum > 0) {
			Action *action = &unit->actions[0];
			Unit *target = getUnit(action->targetId);
			if (action->type == ACTION_ATTACK && target) {
				unit->rotation = toDeg(radsBetween(unit->position, target->position));
			}
		}

		if (!unit->info->isBuilding) {
			unit->visualRotation = lerpDegrees(unit->visualRotation, unit->rotation, 0.1);
		} else {
			unit->rotation = 45;
			unit->visualRotation = 45;
		}

		drawRect(unit->rect, playerColors[unit->teamNumber] | 0xFF000000);
		if (unit->info->isBuilding && unit->teamNumber != game->teamNumber && unit->teamNumber != -1 && canSeeUnit(game->teamNumber, unit)) {
			if (game->fogUnitsNum < BUILDINGS_PER_PLAYER_MAX-1) {
#if 0 //@todo fog units
				FogUnit *fogUnit = &game->fogUnits[game->fogUnitsNum++];
				memset(fogUnit, 0, sizeof(FogUnit));
				fogUnit->rect = unit->rect;
				fogUnit->frame = frame;
				fogUnit->matrix = mat;
				fogUnit->uvMatrix = uvMat;
				fogUnit->props = props;
				fogUnit->onScreen = true;
#endif
			} else {
				logf("Too many fogUnits\n");
			}
		}

		if (unit->selected) {
			Rect rect = makeCenteredSquare(unit->visualPosition, unit->info->size);
			drawRectOutline(rect, 1, 0x8800FF00);

			if (unit->info->isBuilding && !unit->info->hideRally) {
				if (!unit->rallyPoint.isZero()) {
					Rect rect = makeRect(0, 0, 8, 8);
					rect.x = unit->rallyPoint.x - rect.width/2;
					rect.y = unit->rallyPoint.y - rect.height/2;
					drawRect(rect, 0xFF00FF00);

					drawLine(unit->position, unit->rallyPoint, 2, 0xFF00FF00);
				}
			}
		}

		if (unit->hp != unit->info->maxHp && unit->hp > 0) {
			Rect unitRect = getUnitRect(unit->type, unit->visualPosition);
			Rect hpRect = makeRect(0, 0, 32, 4);
			hpRect.x = unitRect.x + unitRect.width/2 - hpRect.width/2;
			hpRect.y = unitRect.y - hpRect.height - 4;

			Rect hpBarRect = hpRect.inflate(v2(1, 1));

			drawRect(hpBarRect, 0xFF004400);

			hpRect.width *= unit->hp / unit->info->maxHp;
			drawRect(hpRect, 0xFF00FF00);
		}

		if (unit->heat > 0) {
			Rect heatRect = makeRect(0, 0, 8, 64);
			heatRect.x = unit->rect.x + unit->rect.width + 4;
			heatRect.y = unit->rect.y;

			drawRect(heatRect.inflate(2), 0x88FF0000);

			float origHeight = heatRect.height;

			int barColor = 0xFFFFFF00;
			if (unit->heat >= 100) {
				heatRect.height *= (float)(unit->heat - 100)/100.0;
				barColor = lerpColor(0xFFFFFF00, 0xFFFF0000, timePhase(game->time, 10));
			} else {
				heatRect.height *= (float)unit->heat/100.0;
			}

			heatRect.y += origHeight - heatRect.height;
			drawRect(heatRect, barColor);
		}

		if (unit->state == UNIT_STATE_CREATING) {
			Rect buildingTimeBar = makeRect(0, 0, 32, 4);
			buildingTimeBar.x = unit->rect.x + unit->rect.width/2 - buildingTimeBar.width/2;
			buildingTimeBar.y = unit->rect.y + 4;

			Rect buildingTimeBarBg = buildingTimeBar;

			buildingTimeBar.width *= unit->stateTime / unit->info->creationTime;

			drawRect(buildingTimeBarBg, 0xFF2222FF);
			drawRect(buildingTimeBar, 0xFF6666FF);
		}

		if (unit->actionsNum > 0) {
			Action *action = &unit->actions[0];
			if (canSeeUnit(game->teamNumber, unit)) {
				if (action->type == ACTION_CONSTRUCT) {
					Rect constructionTimeBar = makeRect(0, 0, 4, 32);
					constructionTimeBar.x = unit->rect.x - constructionTimeBar.width - 4;
					constructionTimeBar.y = unit->rect.y + unit->rect.height/2 - constructionTimeBar.height/2;

					Rect constructionTimeBarBg = constructionTimeBar;

					constructionTimeBar.height *= 1 - (unit->actionTime / game->unitInfos[action->unitType].creationTime);

					drawRect(constructionTimeBarBg, 0xFF2222FF);
					drawRect(constructionTimeBar, 0xFF9999FF);
				}
			}
		}
	}

	for (int i = 0; i < game->bulletsNum; i++) {
		if (gamePaused) break;

		Bullet *bullet = &game->bullets[i];

		Unit *dest = getUnit(bullet->destId);
		if (dest) {
			bullet->destPosition = dest->position;
		}

		bool shouldHit = false;
		if (bullet->type == BULLET_GENERIC_PROJECTILE) {
			float speed = bullet->speed * timeScale;
			if (bullet->position.distance(bullet->destPosition) < speed) {
				shouldHit = true; 
			} else {
				float rad = radsBetween(bullet->position, bullet->destPosition);
				bullet->position.x += cos(rad) * speed;
				bullet->position.y += sin(rad) * speed;

				Rect rect = makeRect(0, 0, 4, 4);
				rect.x = bullet->position.x - rect.width/2;
				rect.y = bullet->position.y - rect.height/2;
				drawRect(rect, 0xFFFF0000);
			}
		} else if (bullet->type == BULLET_HIT_SCAN) {
			shouldHit = true;
		} else {
			logf("Unknown bullet type\n");
		}

		if (shouldHit) {
			if (dest) {
				bool doGenericEmit = true;

				dealDamage(dest, getUnit(bullet->srcId), bullet->damage, bullet->position);
				dest->slow += bullet->slowDamage;

				int oldHeat = dest->heat;
				dest->heat += bullet->heatDamage;
				if (dest->heat > 100 && oldHeat < 100) dest->heat = 110;
			}

			arraySplice(game->bullets, sizeof(Bullet) * BULLETS_MAX, sizeof(Bullet), bullet);
			game->bulletsNum--;
			i--;
			continue;
		}
	}

	for (int i = 0; i < sim->fireBombsNum; i++) {
		FireBomb *fireBomb = &sim->fireBombs[i];
		float airTime = 0.5;
		float perc = fireBomb->time / airTime;

		if (!gamePaused) {
			fireBomb->time += elapsed;

			if (fireBomb->time > airTime) {
				if (sim->firePuddlesNum < FIRE_PUDDLES_MAX-1) {
					FirePuddle *puddle = &sim->firePuddles[sim->firePuddlesNum++];
					memset(puddle, 0, sizeof(FirePuddle));

					int radius = 6;

					puddle->worldPosition = fireBomb->end;
					puddle->worldRadius = radius * tileSize;

					Vec2Tile position = worldToVec2Tile(puddle->worldPosition);

					int minX = position.x - radius;
					int maxX = position.x + radius+1;
					int minY = position.y - radius;
					int maxY = position.y + radius+1;

					if (minX < 0) minX = 0;
					if (maxX > map->width-1) maxX = map->width-1;
					if (minY < 0) minY = 0;
					if (maxY > map->height-1) maxY = map->height-1;

					int tilesMax = radius*4 * radius*4;
					puddle->tiles = (Vec2Tile *)malloc(tilesMax * sizeof(Vec2Tile));

					Circle circle = makeCircle(v2(position.x, position.y), radius);
					for (int y = minY; y < maxY; y++) {
						for (int x = minX; x < maxX; x++) {
							if (!contains(circle, v2(x, y))) continue;
							Vec2Tile tile = v2Tile(x, y);
							if (hasLineOfSight(position, tile, true)) {
								puddle->tiles[puddle->tilesNum++] = tile;
							}
						}
					}
				}

				arraySpliceIndex(sim->fireBombs, sim->fireBombsNum, sizeof(FireBomb), i);
				i--;
				sim->fireBombsNum--;
				continue;
			}
		}

		Vec2 pos = lerp(fireBomb->start, fireBomb->end, perc);

		Vec2 hookPos;
		hookPos.x = (fireBomb->start.x + fireBomb->end.x) / 2;
		hookPos.y = (fireBomb->start.y + fireBomb->end.y) / 2;
		hookPos = hookPos - (v2(50, 100)*10000.0);

		float hookPerc = sin(perc*M_PI)/(4.0 * 10000.0);
		pos = lerp(pos, hookPos, hookPerc);

		drawCircle(pos, 10, 0xFF0000FF);
	}

	for (int i = 0; i < sim->firePuddlesNum; i++) {
		FirePuddle *puddle = &sim->firePuddles[i];
		float maxTime = 5;
		float perc = puddle->time / maxTime;

		if (!gamePaused) {
			puddle->time += elapsed;

			if (perc > 1) {
				free(puddle->tiles);
				arraySpliceIndex(sim->firePuddles, sim->firePuddlesNum, sizeof(FirePuddle), i);
				i--;
				sim->firePuddlesNum--;
				continue;
			}
		}

	}

	for (int i = 0; i < game->effectsNum; i++) {
		if (gamePaused) break;
		Effect *effect = &game->effects[i];

		float maxTime = 0.5;
		if (effect->type == EFFECT_UNIT_SELECTED) maxTime = 0.1;
		if (effect->type == EFFECT_HEMINE_EXPLODE) maxTime = 1;

		Vec2 position = effect->position;
		Unit *unit = NULL;
		if (effect->unitId) {
			unit = getUnit(effect->unitId);
			if (unit) position = unit->position;
		}

		float perc = effect->time/maxTime;
		bool effectDone = effect->time >= maxTime;

		if (effect->type == EFFECT_UNIT_TARGETED) {
			if (unit) {
				float unitWidth = unit->info->size;
				Rect rect = makeCenteredSquare(position, lerp(unitWidth * 2, unitWidth, perc));
				drawRectOutline(rect, 2, 0x88FF0000);
			}
		} else if (effect->type == EFFECT_UNIT_SELECTED) {
			if (unit) {
				float unitWidth = unit->info->size;
				Rect rect = makeCenteredSquare(position, lerp(unitWidth * 2, unitWidth, perc));
				drawRectOutline(rect, 2, 0x8800FF00);
			}
		} else if (effect->type == EFFECT_ISSUED_MOVE) {
			Rect rect = makeCenteredSquare(position, lerp(0, 32, perc));
			drawRect(rect, lerpColor(0xFF00FF00, 0x0000FF00, perc));
		} else if (effect->type == EFFECT_ISSUED_ATTACK_MOVE) {
			Rect rect = makeCenteredSquare(position, lerp(0, 32, perc));
			drawRect(rect, lerpColor(0xFFFF0000, 0x00FF0000, perc));
		} else if (effect->type == EFFECT_UNIT_DEATH) {
			for (int i = 0; i < 10; i++) {
				Rect rect = makeCenteredSquare(position + v2(rndFloat(-32, 32), rndFloat(-32, 32)), lerp(32, 8, perc));
				drawRect(rect, lerpColor(0xFFFF0000, 0x00FF0000, perc));
			}
		} else if (effect->type == EFFECT_START_UNIT_CONSTRUCTION) {
			Rect rect = makeCenteredSquare(position, lerp(0, 128, perc));
			drawRect(rect, lerpColor(0xFF0000FF, 0x000000FF, perc));
		} else if (effect->type == EFFECT_UNIT_FULLY_CONSTRUCTED) {
			Rect rect = makeCenteredSquare(position, lerp(0, 128, perc));
			drawRect(rect, lerpColor(0xFF00FF00, 0x0000FF00, perc));
		} else if (effect->type == EFFECT_HEMINE_EXPLODE) {
			UnitInfo *info = &game->unitInfos[UNIT_HEMINE];
			for (int i = 0; i < 100; i++) {
				float deg = rndFloat(0, 360);
				Vec2 off;
				off.x = cos(toRad(deg));
				off.y = sin(toRad(deg));
				off = off * rndFloat(0, info->range);

				drawCircle(
					position + off,
					lerp(16, 4, perc),
					lerpColor(0xFFFF0000, 0x0000FFFF, perc)
				);
			}
		} else {
			logf("Invalid effect\n");
		}

		if (effectDone) {
			arraySpliceIndex(game->effects, EFFECTS_MAX, sizeof(Effect), i);
			game->effectsNum--;
			i--;
			continue;
		}

		effect->time += elapsed;
	}

	for (int i = 0; i < repulsionCirclesNum; i++) {
		RepulsionCircle *circle = &repulsionCircles[i];
		Rect rect = makeCenteredSquare(circle->position, circle->radius);
		if (canSeeRect(game->teamNumber, rect)) {
			drawCircle(circle->position, circle->radius, 0x88FF00FF);
		}
	}

	renderer->defaultMaskTexture = NULL;

	for (int i = 0; i < game->unitsNum; i++) { /// Unit ui draw loop
		Unit *unit = &game->units[i];

		if (unit->teamNumber != game->teamNumber && !canSeeUnit(game->teamNumber, unit)) continue;

		Vec2 unitTile = worldToTile(unit->position);
		int unitTileIndex = unitTile.y * map->width + unitTile.x;
		if (unit->type == UNIT_MINERALS && canSeeTile(game->teamNumber, unitTileIndex)) {
			char *str = frameSprintf("%d/%d", unit->unitsMiningFromThisNum, UNITS_MINING_FROM_THIS_MAX);
			Vec2 size = getTextSize(fontSys->defaultFont, str);

			Vec2 pos;
			pos.x = unit->position.x - size.x/2;
			pos.y = unit->position.y - unit->info->size/2 - size.y - 8;

			drawText(fontSys->defaultFont, str, pos, 0xFFFFFFFF);

			if (game->editorMode != EDITOR_NONE) {
				drawCircle(unit->position, MIN_MINERAL_DISTANCE, 0x88F88F8F);
			}
		}
	}

	for (int i = 0; i < game->fogUnitsNum; i++) {
		FogUnit *fogUnit = &game->fogUnits[i];
		if (fogUnit->onScreen) continue;

		RenderProps props = fogUnit->props;
		props.tint = lerpColor(fogUnit->props.tint, 0xFF000000, 0.25);

		drawTexture(fogUnit->frame->texture, props);
	}

	{ /// Draw shadow map
		pushCamera2d(mat3());
		processBatchDraws();

		{
			Texture *texture = game->lightMap;

			Vec2 verts[4];
			verts[0] = v2(0, 0);
			verts[1] = v2(1, 0);
			verts[2] = v2(1, 1);
			verts[3] = v2(0, 1);

			Vec2 uvs[4];
			uvs[0] = v2(0, 0);
			uvs[1] = v2(1, 0);
			uvs[2] = v2(1, 1);
			uvs[3] = v2(0, 1);

			u16 inds[6] = {0, 2, 1, 0, 3, 2};

			Matrix3 mat = mat3();
			mat = mat.multiply(renderer->baseMatrix2d);
			mat = mat.multiply(game->visualCameraMatrix);
			mat.SCALE(texture->width, texture->height);
			mat.SCALE((float)(map->width*tileSize)/(float)texture->width, (float)(map->height*tileSize)/(float)texture->height);

			Matrix3 uvMat = mat3();
			uvMat.TRANSLATE(0, 1);
			uvMat.SCALE(1, -1);

			draw2dMesh(verts, uvs, inds, 6, mat, texture);
		}

		// RenderProps props = newRenderProps();
		// Matrix3 mat;
		// mat.setIdentity();
		// mat = mat.multiply(game->visualCameraMatrix);
		// mat = mat.scale(map->width*tileSize/game->visionBitTexture->width, map->height*tileSize/game->visionBitTexture->height);
		// props.matrix = &mat;
		// props.smooth = false;
		// props.flags |= _F_BLUR13;
		// if (keyPressed(' ')) props.flags |= _F_INVERSE_SRGB;
		bool doShadows = true;
		if (game->editorConfig.noShadowsInEditor && game->editorMode != EDITOR_NONE) doShadows = false;
		// if (doShadows) drawTexture(game->lightMap, props);

		popCamera2d();
	}

	if (game->debugShowLightNumbers) {
		for (int y = 0; y < map->height; y++) {
			for (int x = 0; x < map->width; x++) {
				int vision = game->visionRefs[game->playerNumber][y * map->width + x];
				if (vision == 0) continue;
				Rect rect = tileToWorldRect(v2(x, y));
				// drawRect(rect, 0x88FFFFFF);

				float visionFloat = game->visionLightingRefs[game->playerNumber][y * map->width + x];
				char *str = frameSprintf("%d\n%.0f", vision, visionFloat);
				Font *font = game->smallFont;
				Vec2 size = getTextSize(font, str);
				Vec2 pos;
				pos.x = rect.x + rect.width/2.0 - size.x/2.0;
				pos.y = rect.y + rect.height/2.0 - size.y/2.0;
				drawText(font, str, pos, 0xFFFFFFFF);
				// drawTextInRect(game->smallFont, "abc", rect); //@incomplete I want
			}
		}
	}

	if (map->overlayScale) { /// Overlay effect
		pushCamera2d(mat3());

		Texture *texture = getTexture("assets/images/white1.png");

		Matrix3 vertMat = mat3();
		vertMat.SCALE((float)game->width, (float)game->height);

		Matrix3 uvMat = mat3();
		uvMat.SCALE(1.0/map->overlayScale, 1.0/map->overlayScale);
		uvMat = uvMat.multiply(game->visualCameraMatrix.invert());
		uvMat = uvMat.multiply(vertMat);

		RenderProps props = newRenderProps();
		props.matrix = vertMat;
		props.uvMatrix = uvMat;
		props.flags |= _F_SANDSTORM;
		drawTexture(texture, props);
		popCamera2d();
	}

	{ /// Network check
		if (game->playMode == PLAY_1V1_ONLINE) {
			if (game->timeSinceLastSessionServerContact >= 1 && platform->frameCount % 60 == 0) {
				float timeLeft = maxDisconnectTime - game->timeSinceLastSessionServerContact;
				char *messageStr = frameSprintf("Lost connection, retrying... (%.0f)\n", timeLeft);
				logf(messageStr);
				if (timeLeft <= 0) {
					shouldLoseGame = true;
					shouldDisconnect = true;
				}
			} else if (game->timeSinceConfirmTurnUpdate >= 1 && platform->frameCount % 60 == 0) {
				// float timeLeft = maxDisconnectTime - game->timeSinceConfirmTurnUpdate;
				// char *messageStr = frameSprintf("Opponent not responding, waiting... (%.0f)\n", timeLeft);
				// logf(messageStr);
				// if (timeLeft <= 0) shouldWinGame = true;
			}
		}

		if (game->timeoutTimeLeft > 0 && platform->frameCount % 60 == 0) {
			logf("Timeout (%.0f...)\n", game->timeoutTimeLeft);
		}
	}

	{ /// Player card
		pushCamera2d(mat3());
		Font *font = game->defaultFont;

		{
			Rect rect = makeRect(0, 0, 148 * uiScale, 96 * uiScale);
			rect.x = game->width - rect.width;
			rect.y = 0;
			drawRect(rect, 0xCCFFFFFF);
		}

		Rect mineralTextRect;
		{
			char *str = NULL;
			if (fabs(game->visualMineral[game->playerNumber] - game->minerals[game->playerNumber]) > 0.5) {
				str = frameSprintf("[s]%d", (int)roundf(game->visualMineral[game->playerNumber]));
			} else {
				str = frameSprintf("%d", (int)roundf(game->visualMineral[game->playerNumber]));
			}
			Vec2 size = getTextSize(font, str);
			Vec2 pos;
			pos.x = game->width - size.x - 8;
			pos.y = 0;

			mineralTextRect = makeRect(pos, size);
			drawText(font, str, pos);
		}

		{
			Texture *texture = getTexture("assets/images/ui/hud/minerals.png");
			if (texture) {
				Rect rect;
				rect.width = 32 * uiScale;
				rect.height = 32 * uiScale;
				rect.x = mineralTextRect.x - rect.width - 8;
				rect.y = mineralTextRect.y + mineralTextRect.height/2 - rect.height/2;
				RenderProps props = newRenderProps(texture, rect);
				drawTexture(texture, props);
			}
		}

		Rect populationTextRect;
		{
			char *str = frameSprintf("%d/%d", game->population[game->playerNumber], UNITS_MAX/2);
			Vec2 size = getTextSize(font, str);
			Vec2 pos;
			pos.x = game->width - size.x - 8;
			pos.y = mineralTextRect.y + mineralTextRect.height + 8;
			drawText(font, str, pos);

			populationTextRect = makeRect(pos, size);
		}

		{
			Texture *texture = getTexture("assets/images/ui/hud/population.png");
			if (texture) {
				Rect rect;
				rect.width = 32 * uiScale;
				rect.height = 32 * uiScale;
				rect.x = populationTextRect.x - rect.width - 8;
				rect.y = populationTextRect.y + populationTextRect.height/2 - rect.height/2;
				RenderProps props = newRenderProps(texture, rect);
				drawTexture(texture, props);
			}
		}

		popCamera2d();
	}

	{ /// Status card
		pushTargetTexture(game->statusCardTexture);
		pushCamera2d(mat3());

		clearRenderer(0xFF000000);

		Vec2 mousePos;
		mousePos.x = platform->mouse.x - statusCardRect.x;
		mousePos.y = platform->mouse.y - statusCardRect.y;

// 		Rect mouseRect = makeRect(0, 0, 8, 8);
// 		mouseRect.x = mousePos.x - mouseRect.width/2;
// 		mouseRect.y = mousePos.y - mouseRect.height/2;
// 		drawRect(mouseRect, 0xFF00FF00);

		{ /// Control group tabs
			float pad = 4;
			Rect controlGroupsRect;
			controlGroupsRect.x = pad;
			controlGroupsRect.y = pad;
			controlGroupsRect.width = statusCardRect.width - pad*2;
			controlGroupsRect.height = 32;

			for (int i = 0; i < CONTROL_GROUPS_MAX; i++) {
				int groupNum = (i + 1) % CONTROL_GROUPS_MAX;

				ControlGroup *group = &game->controlGroups[game->playerNumber][groupNum];
				Rect groupRect;
				groupRect.width = (float)controlGroupsRect.width / (float)CONTROL_GROUPS_MAX;
				groupRect.height = controlGroupsRect.height;
				groupRect.x = groupRect.width * i;
				groupRect.y = 0;

				// if (groupRect.contains(mousePos) && mouseJustDown) {
				// }

				groupRect = groupRect.inflate(-5);

				if (groupNum == game->lastControlGroupSelected) {
					groupRect.y += clampMap(game->timeSinceLastControlGroupSelected, 0, 0.15, 20, 0, QUAD_OUT);
				}
				float timeSinceLastControlGroupSelected;

				drawRect(groupRect, 0xFF666666);

				if (group->idsNum != 0) { /// Group ids number
					char *str = frameSprintf("%d", group->idsNum);
					Vec2 size = getTextSize(game->defaultFont, str);
					Vec2 pos;
					pos.x = groupRect.x + (groupRect.width/2.0)/2.0 - size.x/2.0;
					pos.y = groupRect.y + groupRect.height/2.0 - size.y/2.0;
					drawText(game->defaultFont, str, pos, 0xFFFFFFFF);
				}

				{ /// Unit icon
					Unit *firstUnit = getFirstUnit(group);
					if (firstUnit) {
						Texture *texture = getTexture(frameSprintf("assets/images/unitIcons/%s.png", firstUnit->info->formalName));
						Rect rect = makeRect(0, 0, 16, 16);

						rect.x = groupRect.x + (groupRect.width/2.0)/2.0 - rect.width/2.0;
						rect.x += groupRect.width/2.0;
						rect.y = groupRect.y + groupRect.height/2.0 - rect.height/2.0;

						{
							Texture *backgroundTexture = getTexture("assets/images/ui/alerts/alertBg.png");
							if (backgroundTexture) {
								RenderProps props = newRenderProps(backgroundTexture, rect.inflate(8));

								float perc = lerp(0, 1, game->time);
								while (perc > 1) perc -= 1;

								Matrix3 uvMatrix = mat3();
								uvMatrix.SCALE(0.5, 0.5);
								uvMatrix.TRANSLATE(perc, perc);
								props.uvMatrix = uvMatrix;
								props.tint = 0x88000000;
								drawTexture(backgroundTexture, props);
							}
						}

						RenderProps props = newRenderProps(texture, rect);
						// props.flags |= _F_FXAA;
						drawTexture(texture, props);
					}
				}

				if (group->idsNum != 0) { /// Control group number
					char *str = frameSprintf("%d", groupNum);
					Vec2 size = getTextSize(game->smallFont, str);
					Vec2 pos;
					pos.x = groupRect.x + groupRect.width/2.0 - size.x/2.0;
					pos.y = groupRect.y + groupRect.height - size.y/2.0;
					drawText(game->smallFont, str, pos, 0xFFFFFFFF);
				}
			}
		}

		ControlGroup *group = &game->localSelectedUnits;
		if (group->idsNum == 0) {
			// Nothing
		} else if (group->idsNum == 1) {
			Unit *unit = getFirstUnit(group);
			Assert(unit);

			Rect iconRect = makeRect(0, 0, 128 * uiScale, 128 * uiScale);
			iconRect.x = statusCardRect.width * 0.05; 
			iconRect.y = statusCardRect.height/2 - iconRect.height/2;
			Texture *texture = getTexture(frameSprintf("assets/images/unitIcons/%s.png", unit->info->formalName));
			if (texture) {
				RenderProps props = newRenderProps(texture, iconRect);
				drawTexture(texture, props);
			}

			char *textStr = frameSprintf("Health: %d/%d", (int)unit->hp, (int)unit->info->maxHp);
			Vec2 textSize = getTextSize(fontSys->defaultFont, textStr);

			Vec2 textPosition;
			textPosition.x = iconRect.x + iconRect.width/2 - textSize.x/2;
			textPosition.y = iconRect.y + iconRect.height + 5;
			drawText(fontSys->defaultFont, textStr, textPosition, 0xFF00FF00);

			if (unit->info->isBuilding) {
				if (unit->actionsNum > 0) {
					Action *action = &unit->actions[0];

					Rect buildingQueueRects[BUILDING_QUEUE_MAX];

					for (int i = 0; i < BUILDING_QUEUE_MAX; i++) {
						Rect rect = makeRect(0, 0, 64 * uiScale, 64 * uiScale);
						rect.x = iconRect.x + iconRect.width + ((rect.width + 16) * i) + 64;
						rect.y = statusCardRect.height - rect.height - 64;
						buildingQueueRects[i] = rect;

						if (unit->actionsNum > i) {
							Action *action = &unit->actions[i];
							if (action->type == ACTION_CONSTRUCT) {
								UnitInfo *unitInfoConstructing = &game->unitInfos[action->unitType];

								Texture *texture = getTexture(frameSprintf("assets/images/unitIcons/%s.png", unitInfoConstructing->formalName));
								if (texture) {
									RenderProps props = newRenderProps(texture, rect);
									if (i == 0) props.tint = lerpColor(0x00FFFF00, 0x80FFFF00, secondPhase);
									drawTexture(texture, props);
								} else {
									logf("Missing icon for %s\n", unit->info->formalName);
								}
							} else {
								drawRect(rect, 0xFF440000);
							}

							if (rect.contains(mousePos) && mouseJustUp) {
								GameCmd *gameCmd = createGameCmd();
								if (gameCmd) {
									gameCmd->type = GAME_CMD_SPLICE_ACTION;
									gameCmd->targetId = i;
								}
							}
						} else {
							drawRect(rect, 0xFF444444);
						}
					}

					{
						Rect firstRect = buildingQueueRects[0];
						Rect lastRect = buildingQueueRects[BUILDING_QUEUE_MAX-1];

						Rect constructionStatusBar;
						constructionStatusBar.width = lastRect.x + lastRect.width - firstRect.x;
						constructionStatusBar.height = 8;
						constructionStatusBar.x = firstRect.x;
						constructionStatusBar.y = firstRect.y + firstRect.height + 16;

						Rect constructionStatusBarBg = constructionStatusBar;

						constructionStatusBar.width *= 1 - (unit->actionTime / game->unitInfos[action->unitType].creationTime);

						drawRect(constructionStatusBarBg, 0xFF000088);
						drawRect(constructionStatusBar, 0xFF0000FF);
					}
				}
			} else {
				/// Single unit status
			}
		} else {
			for (int i = 0; i < group->idsNum; i++) {
				Unit *unit = getUnit(group->ids[i]);
				if (!unit) continue;
				Rect unitRect = makeRect(0, 0, 64 * uiScale, 64 * uiScale);

				int maxCols = statusCardRect.width / unitRect.width;

				int row = i % maxCols;
				int col = i / maxCols;

				unitRect.x = row * (unitRect.width + 4);
				unitRect.y = col * (unitRect.height + 4) + 32 * uiScale; //@magicNumber 32

				Texture *texture = getTexture(frameSprintf("assets/images/unitIcons/%s.png", unit->info->formalName));
				if (texture) {
					RenderProps props = newRenderProps(texture, unitRect);
					drawTexture(texture, props);
				}

				if (unit->info->isBuilding) {
					for (int i = 0; i < BUILDING_QUEUE_MAX; i++) {
						Rect rect = makeRect(0, 0, 8, 8);

						rect.x = unitRect.x + unitRect.width - rect.width - (rect.width + 1)*i;
						rect.y = unitRect.y + unitRect.height - rect.height;

						int color = 0xFF444444;
						if (unit->actionsNum > (BUILDING_QUEUE_MAX-1 - i)) color = 0xFF888888;

						drawRect(rect, color);
					}
				}

				if (unitRect.contains(mousePos) && mouseJustUp) {
					if (keyPressed(KEY_SHIFT)) {
						GameCmd *gameCmd = createGameCmd();
						if (gameCmd) {
							gameCmd->type = GAME_CMD_SPLICE_SELECTION;
							gameCmd->targetId = i;

							processSpliceSelection(gameCmd->playerNumber, gameCmd->targetId, true);
							game->interfaceMode = INTERFACE_NORMAL;
						}
					} else {
						GameCmd *gameCmd = createGameCmd();
						if (gameCmd) {
							gameCmd->type = GAME_CMD_CLICK_SELECT;
							gameCmd->targetId = unit->id;
							createEffect(EFFECT_UNIT_SELECTED, unit->id);

							processClickSelect(gameCmd->playerNumber, gameCmd->targetId, gameCmd->queues, true);
							game->interfaceMode = INTERFACE_NORMAL;
						}
					}
				}
			}
		}

		popTargetTexture();
		popCamera2d();

		{
			pushCamera2d(mat3());
			RenderProps props = newRenderProps();
			props.matrix.TRANSLATE(statusCardRect.x, statusCardRect.y);
			drawTexture(game->statusCardTexture, props);
			popCamera2d();
		}
	}

	Rect clockRect = {};
	{ /// Clock
		pushCamera2d(mat3());

		int totalSec = game->turn * game->framesPerTurn * (1.0/60.0);
		int min = totalSec / 60;
		int sec = totalSec % 60;
		char *clockStr = frameSprintf("%02d:%02d", min, sec);

		Vec2 size = getTextSize(fontSys->defaultFont, clockStr);
		Vec2 pos;
		pos.x = minimapRect.x;
		pos.y = minimapRect.y - size.y - 2;
		drawText(fontSys->defaultFont, clockStr, pos, 0xFFFFFFFF);

		clockRect.x = pos.x;
		clockRect.y = pos.y;
		clockRect.width = 128;
		clockRect.height = 64;

		popCamera2d();
	}

	{ /// Alerts
		for (int i = 0; i < game->alertsNum; i++) {
			Alert *alert = &game->alerts[i];
			if (alert->playerNumber != game->playerNumber) continue;
			if (alert->visualDone && !alert->isOnlyOne) continue;

			if (renderer->cameraRect.contains(alert->position)) {
				Texture *texture = getTexture("assets/images/ui/alerts/inGame/indicator.png");
				if (texture) {
					Matrix3 mat = mat3();
					mat.TRANSLATE(alert->position.x, alert->position.y);
					mat.SCALE(1, 1);
					mat.TRANSLATE(-texture->width/2.0, -texture->height/2.0);

					RenderProps props = newRenderProps();
					props.matrix = mat;
					props.alpha = lerp(0, 0.5, secondPhase);
					drawTexture(texture, props);
				}
			} else {
				// Vec2 position = renderer->cameraRect.getClosestPoint(alert->position);
				// drawCircle(position, 20, 0xFF0000FF);
			}
		}

		pushCamera2d(mat3());
		Rect alertPanelRect = makeRect(0, 0, 256 * uiScale, 256 * uiScale);
		alertPanelRect.x = 0;
		alertPanelRect.y = clockRect.y - alertPanelRect.height - 5;
		drawRect(alertPanelRect, 0x88000000);

		int alertCount = 0;
		for (int i = game->alertsNum-1; i >= 0; i--) {
			Alert *alert = &game->alerts[i];
			if (alert->playerNumber != game->playerNumber) continue;

			char *str = NULL;
			Texture *iconTexture = NULL;
			if (alert->type == ALERT_TEST_ALERT) {
				str = frameSprintf("This is a test alert");
			} else if (alert->type == ALERT_TEST_ALERT_2) {
				str = frameSprintf("This is a test alert2");
			} else if (alert->type == ALERT_FOUND_AN_ENEMY_BASE) {
				str = frameSprintf("Found an enemy base!");
			} else if (alert->type == ALERT_UNIT_UNDER_ATTACK) {
				str = frameSprintf("Forces are in combat!");
			} else if (alert->type == ALERT_BUILDING_UNDER_ATTACK) {
				str = frameSprintf("Buildings are under attack!");
			} else if (alert->type == ALERT_PROBES_UNDER_ATTACK) {
				str = frameSprintf("Workers are under attack!");
			} else if (alert->type == ALERT_OUT_OF_MINERALS) {
				str = frameSprintf("Out of minerals");
				iconTexture = getTexture("assets/images/ui/alerts/outOfMinerals.png");
			} else if (alert->type == ALERT_HEMINE_EXPLODE) {
				str = frameSprintf("Boom!");
				iconTexture = getTexture("assets/images/ui/alerts/boom.png");
			} else if (alert->type == ALERT_DEFEND_YOUR_BASE) {
				str = frameSprintf("Defend your base!");
			} else if (alert->type == ALERT_DESTROY_EVERYTHING) {
				str = frameSprintf("Destroy everything on the map!");
			} else if (alert->type == ALERT_SAY) {
				str = frameSprintf("ALERT_SAY should have a customMsg");
			} else if (alert->type == ALERT_SAY_DIALOG) {
				str = frameSprintf("ALERT_SAY_DIALOG should have a dialog");
			} else {
				str = frameSprintf("This alert has no text!");
			}

			if (alert->customMsg[0]) str = alert->customMsg;
			if (alert->dialog) str = alert->dialog->text;

			Rect alertRect = makeRect(alertPanelRect.x, 0, alertPanelRect.width, alertPanelRect.height*0.15);
			alertRect.y = alertPanelRect.y + alertPanelRect.height - alertRect.height - alertRect.height*alertCount++;
			if (alert->time < 2) {
				drawRect(alertRect, lerpColor(0xFF444444, 0xFFFF4444, timePhase(alert->time*3)));
			} else {
				drawRect(alertRect, 0xFF444444);
			}

			if (mouseJustUp && alertRect.contains(platform->mouse)) {
				game->cameraTarget = alert->position;

				Unit *unit = getUnit(alert->unitId);
				if (unit) {
					GameCmd *gameCmd = createGameCmd();
					if (gameCmd) {
						gameCmd->type = GAME_CMD_CLICK_SELECT;
						gameCmd->targetId = unit->id;
						createEffect(EFFECT_UNIT_SELECTED, unit->id);

						processClickSelect(gameCmd->playerNumber, gameCmd->targetId, gameCmd->queues, true);
						game->interfaceMode = INTERFACE_NORMAL;
					}
				}
			}

			Vec2 textSize = getTextSize(game->smallFont, str);
			Vec2 textPos;
			textPos.x = alertRect.x + alertRect.width/2 - textSize.x/2;
			textPos.y = alertRect.y + alertRect.height/2 - textSize.y/2;
			drawText(game->smallFont, str, textPos, 0xFFEEEEEE);
			alert->frameStr = str;

			Rect icon = makeRect(0, 0, 32*uiScale, 32*uiScale);
			icon.x = alertRect.x;
			icon.y = alertRect.y;

			Texture *backgroundTexture = getTexture("assets/images/ui/alerts/alertBg.png");
			if (backgroundTexture) {
				RenderProps props = newRenderProps();
				props.matrix.TRANSLATE(icon.x, icon.y);
				props.matrix.SCALE(uiScale, uiScale);

				float perc = lerp(0, 1, game->time);
				while (perc > 1) perc -= 1;

				Matrix3 uvMatrix = mat3();
				uvMatrix.SCALE(0.5, 0.5);
				uvMatrix.TRANSLATE(perc, perc);
				props.uvMatrix = uvMatrix;
				drawTexture(backgroundTexture, props);
			}

			if (iconTexture) {
				RenderProps props = newRenderProps(iconTexture, icon);
				drawTexture(iconTexture, props);
			}

			alert->time += elapsed;
		}

		{ /// Most recent alert
			Alert *alert = &game->alerts[game->alertsNum-1];
			float bigDisplayTime = 5;
			float perc = alert->time / bigDisplayTime;
			if (alert->isOnlyOne && perc > 0.5) perc = 0.5;
			if (alert->type != ALERT_NONE && perc < 1) {
				float alpha = 1;
				if (perc < 0.1) alpha = clampMap(perc, 0, 0.1, 0, 1);
				else if (perc > 0.9) alpha = clampMap(perc, 0.9, 1, 1, 0);

				float pad = 16;
				Rect rect = makeRect(0, 0, 512 * uiScale, 128 * uiScale);
				rect.x = statusCardRect.x + pad;
				rect.y = statusCardRect.y - rect.height - pad;

				auto changeByte = [](u32 value, int byteIndex, u8 newByte)->u32 {
					u8 bytes[4];
					bytes[0] = (value >> 24) & 0xFF;
					bytes[1] = (value >> 16) & 0xFF;
					bytes[2] = (value >> 8) & 0xFF;
					bytes[3] = (value     ) & 0xFF;

					bytes[byteIndex] = newByte;

					return ((bytes[0] & 0xff) << 24) + ((bytes[1] & 0xff) << 16) + ((bytes[2] & 0xff) << 8) + (bytes[3] & 0xff);
				};

				rect = rect.inflate(16.0 * timePhase(game->time, 2));
				drawRect(rect, changeByte(0xFF666666, 0, 255.0*alpha));

				Rect iconRect = makeRect(0, 0, rect.height, rect.height);
				iconRect.x = rect.x;
				iconRect.y = rect.y;

				drawRect(iconRect, changeByte(0xFF006600, 0, 255.0*alpha));

				if (alert->frameStr) {
					Vec2 size = getTextSize(game->defaultFont, alert->frameStr, 256);
					Vec2 pos;
					pos.x = iconRect.x + iconRect.width + pad;
					pos.y = iconRect.y;

					drawText(game->defaultFont, alert->frameStr, pos, changeByte(0xFF000000, 0, 255.0*alpha), 256);
				}
			}
		}

		popCamera2d();
	}

	if (!game->debugHideMinimap) { /// Minimap card
		Rect minimapCameraRect = renderer->cameraRect;

		if (mouseJustDown && isHoveringMinimap && !noMinimapClick) {
			game->draggingMinimap = true;
		}
		if (!mouseDown) game->draggingMinimap = false;

		if (mouseDown && game->draggingMinimap) {
			game->cameraTarget.x = minimapHoveringWorldPos.x;
			game->cameraTarget.y = minimapHoveringWorldPos.y;
		}

		pushTargetTexture(game->minimapTexture);
		pushCamera2d(mat3());

		clearRenderer(0xFF000000);
		// drawRect(0, 0, game->minimapTexture->width, game->minimapTexture->height, 0xFF000000);

		Vec2 minimapWorldScale;
		minimapWorldScale.x = (float)game->minimapTexture->width / (float)(map->width * tileSize);
		minimapWorldScale.y = (float)game->minimapTexture->height / (float)(map->height * tileSize);

		minimapCameraRect.x *= minimapWorldScale.x;
		minimapCameraRect.y *= minimapWorldScale.y;
		minimapCameraRect.width *= minimapWorldScale.x;
		minimapCameraRect.height *= minimapWorldScale.y;
		drawRectOutline(minimapCameraRect, 2, 0x88FFFF00);

		float minimapTileScale = (float)game->minimapTexture->width / (float)map->width;
		for (int y = 0; y < map->height; y++) {
			for (int x = 0; x < map->width; x++) {
				Rect rect;
				rect.width = minimapTileScale;
				rect.height = minimapTileScale;
				rect.x = x * minimapTileScale;
				rect.y = y * minimapTileScale;

				int collisionDataIndex = y * map->width + x;
				if (map->collisionData[collisionDataIndex] == 1) drawRect(rect, 0xFFFFFFFF);
				if (game->collisionData[collisionDataIndex] == 2) drawRect(rect, 0xFFFFFFFF);
			}
		}

		for (int i = 0; i < game->fogUnitsNum; i++) {
			FogUnit *fogUnit = &game->fogUnits[i];
			if (fogUnit->onScreen) {
				fogUnit->onScreen = false;
				continue;
			}

			Vec2Tile tiles[TILES_UNDER_UNIT_MAX];
			int tilesNum = getTilesUnderRect(fogUnit->rect, tiles, TILES_UNDER_UNIT_MAX);

			Rect rect = fogUnit->rect;
			rect.x *= minimapWorldScale.x;
			rect.y *= minimapWorldScale.y;
			rect.width *= minimapWorldScale.x;
			rect.height *= minimapWorldScale.y;

			int color = playerMinimapColors[game->teamNumber==0 ? 1 : 0];
			drawRect(rect, color);
		}

		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];

			if (!canSeeUnit(game->teamNumber, unit)) continue;

			int color;
			if (unit->teamNumber == -1) {
				color = 0xFF888888;
			} else {
				color = playerMinimapColors[unit->teamNumber];
			}

			if (unit->damagedBlinkAmount > 0) {
				unit->damagedBlinkAmount--;
				if (platform->frameCount % 6 < 3) color = 0xFF00FF00;
			}

			Rect rect = unit->rect;
			rect.x *= minimapWorldScale.x;
			rect.y *= minimapWorldScale.y;
			rect.width *= minimapWorldScale.x;
			rect.height *= minimapWorldScale.y;

			drawRect(rect, color);
		}

		for (int i = 0; i < game->alertsNum; i++) {
			Alert *alert = &game->alerts[i];
			if (alert->visualDone && !alert->isOnlyOne) continue;
			Vec2 position = alert->position * minimapWorldScale;
			bool drawDefaultIndicator = false;

			if (alert->playerNumber != game->playerNumber) continue;
			if (alert->position.isZero()) continue;

			if (
				alert->type == ALERT_UNIT_UNDER_ATTACK ||
				alert->type == ALERT_BUILDING_UNDER_ATTACK ||
				alert->type == ALERT_PROBES_UNDER_ATTACK
			) {
				if (alert->time == 0) {
					playGameSound("assets/audio/underAttack.ogg");
				}

				float alertMaxTime = 3;
				if (alert->time > alertMaxTime) alert->visualDone = true;

				float perc = alert->time/alertMaxTime;

				Texture *bgTexture = getTexture("assets/images/ui/alerts/minimap/alertBg.png");
				if (bgTexture) {
					float scale = clampMap(alert->time, 0, 1, 2, 0.5, BACK_OUT);
					scale *= uiScale;
					Matrix3 mat = mat3();
					mat.TRANSLATE(position.x, position.y);
					mat.SCALE(scale, scale);
					mat.TRANSLATE(-bgTexture->width/2.0, -bgTexture->height/2.0);

					RenderProps props = newRenderProps();
					props.matrix = mat;
					if (perc < 0.5) props.alpha = clampMap(alert->time, 0, 0.5, 0, 1, LINEAR);
					else props.alpha = clampMap(alert->time, alertMaxTime-1, alertMaxTime, 1, 0, LINEAR);
					props.colorShift = lerpColor(0xFFFF0000, 0xFFFFFF00, perc);
					drawTexture(bgTexture, props);
				}

				Texture *exclaimTexture = getTexture("assets/images/ui/alerts/minimap/alertExclaim.png");
				if (exclaimTexture) {
					float scale = clampMap(alert->time, 0.5, 1, 1.25, 0.25, BOUNCE_OUT);
					scale *= lerp(1, 1.2, timePhase(alert->time));
					Matrix3 mat = mat3();
					mat.TRANSLATE(position.x, position.y + 5);
					mat.SCALE(scale, scale);
					mat.ROTATE(clampMap(alert->time, 0.5, 1, -(270), -(360+15), BACK_OUT));
					mat.TRANSLATE(-exclaimTexture->width/2.0, -exclaimTexture->height/2.0);

					RenderProps props = newRenderProps();
					if (perc < 0.5) props.alpha = clampMap(alert->time, 0.5, 1, 0, 1, LINEAR);
					else props.alpha = clampMap(alert->time, alertMaxTime-1, alertMaxTime, 1, 0, LINEAR);
					props.matrix = mat;
					props.colorShift = lerpColor(0xFFFF0000, 0xFFFFFF00, timePhase(alert->time, 5));
					drawTexture(exclaimTexture, props);
				}
			} else if (alert->type == ALERT_FOUND_AN_ENEMY_BASE) {
				if (alert->time == 0) {
					// playGameSound("assets/audio/underAttack.ogg");
				}

				if (alert->time > 3) alert->visualDone = true;

				Texture *texture = getTexture("assets/images/ui/alerts/minimap/baseIndicator.png");
				if (texture) {
					RenderProps props = newRenderProps();
					props.matrix.TRANSLATE(position.x - texture->width/2, position.y - texture->height/2);
					props.colorShift = playerColors[!alert->playerNumber];
					props.alpha = lerp(0, 1, secondPhase);
					drawTexture(texture, props);
				}
			} else if (alert->type == ALERT_TEST_ALERT) {
				if (alert->time > 3) alert->visualDone = true;
				drawDefaultIndicator = true;
			} else if (alert->type == ALERT_TEST_ALERT_2) {
				if (alert->time > 3) alert->visualDone = true;
				drawDefaultIndicator = true;
			} else if (alert->type == ALERT_OUT_OF_MINERALS) {
				if (alert->time > 3) alert->visualDone = true;

				Texture *texture = getTexture("assets/images/ui/alerts/minimap/baseIndicator.png");
				if (texture) {
					RenderProps props = newRenderProps();
					props.matrix.TRANSLATE(position.x - texture->width/2, position.y - texture->height/2);
					props.colorShift = playerColors[!alert->playerNumber];
					props.alpha = lerp(0, 1, secondPhase);
					drawTexture(texture, props);
				}
			} else if (alert->type == ALERT_HEMINE_EXPLODE) {
				if (alert->time > 3) alert->visualDone = true;
				drawDefaultIndicator = true;
			} else if (alert->type == ALERT_DEFEND_YOUR_BASE) {
				if (alert->time > 3) alert->visualDone = true;
				drawDefaultIndicator = true;
			} else if (alert->type == ALERT_DESTROY_EVERYTHING) {
				if (alert->time > 3) alert->visualDone = true;
				drawDefaultIndicator = true;
			} else if (alert->type == ALERT_SAY) {
				if (alert->time > 3) alert->visualDone = true;
				drawDefaultIndicator = true;
			} else {
				logf("Unknown alert %d\n", alert->type);
			}

			if (drawDefaultIndicator && !position.isZero()) {
				Texture *texture = getTexture("assets/images/ui/alerts/minimap/baseIndicator.png");
				if (texture) {
					RenderProps props = newRenderProps();
					props.matrix.TRANSLATE(position.x - texture->width/2, position.y - texture->height/2);
					props.alpha = lerp(0, 1, secondPhase);
					drawTexture(texture, props);
				}
			}
		}

		popCamera2d();
		popTargetTexture();

		{
			pushCamera2d(mat3());

			RenderProps props = newRenderProps();

			props.matrix.TRANSLATE(minimapRect.x, minimapRect.y);
			drawTexture(game->minimapTexture, props);

			popCamera2d();
		}
	}

	{ /// Game menu button
		pushCamera2d(mat3());
		Rect rect = makeRect(0, 0, 64 * uiScale, 32 * uiScale);
		rect.x = game->width - rect.width;
		rect.y = game->height*0.75 - rect.height;

		Texture *texture = getTexture("assets/images/ui/gameMenuButton.png");
		if (texture) {
			RenderProps props = newRenderProps(texture, rect);
			drawTexture(texture, props);
		} else {
			drawRect(rect, 0xFFFF0000);
		}

		if (mouseJustUp && rect.contains(platform->mouse)) {
			game->inGameMenu = !game->inGameMenu;
		}
		popCamera2d();
	}

	Order probeBuildOrders[ORDER_MAX];
	int probeBuildOrdersNum = 0;
	{
		Order order;
		order.type = ORDER_PLACE;
		order.unitType = UNIT_NEXUS;
		order.key = 'N';
		probeBuildOrders[probeBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_PYLON;
		order.key = 'E';
		probeBuildOrders[probeBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_GATEWAY;
		order.key = 'G';
		probeBuildOrders[probeBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_VEHICLE_PLANT;
		order.key = 'V';
		probeBuildOrders[probeBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_ARMORY;
		order.key = 'R';
		probeBuildOrders[probeBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_LAB;
		order.key = 'L';
		probeBuildOrders[probeBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_AIR_PAD;
		order.key = 'A';
		probeBuildOrders[probeBuildOrdersNum++] = order;
	}

	Order beaverBuildOrders[ORDER_MAX];
	int beaverBuildOrdersNum = 0;
	{
		Order order;
		order.type = ORDER_PLACE;
		order.unitType = UNIT_PLASMA_GENERATOR;
		order.key = 'G';
		beaverBuildOrders[beaverBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_PLASMA_BATTERY;
		order.key = 'B';
		beaverBuildOrders[beaverBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_POWER_STATION;
		order.key = 'P';
		beaverBuildOrders[beaverBuildOrdersNum++] = order;

		order.type = ORDER_PLACE;
		order.unitType = UNIT_SATCOM;
		order.key = 'S';
		beaverBuildOrders[beaverBuildOrdersNum++] = order;
	}

	float commandCardWidth = 256;
	float cardHeight = 256 + 40;

	{ /// Orders wheel
		ControlGroup *group = &game->localSelectedUnits;
		Unit *firstUnit = getFirstUnit(group);
		if (firstUnit && firstUnit->state != UNIT_STATE_CREATING) {
			if (
				game->interfaceMode == INTERFACE_NORMAL ||
				game->interfaceMode == INTERFACE_PICKING_BUILDING
			) {
				Order *orders = firstUnit->info->orders;
				int ordersNum = firstUnit->info->ordersNum;

				if (firstUnit->type == UNIT_PROBE && game->interfaceMode == INTERFACE_PICKING_BUILDING) {
					orders = probeBuildOrders;
					ordersNum = probeBuildOrdersNum;
				}

				if (firstUnit->type == UNIT_BEAVER && game->interfaceMode == INTERFACE_PICKING_BUILDING) {
					orders = beaverBuildOrders;
					ordersNum = beaverBuildOrdersNum;
				}

				pushCamera2d(mat3());
				game->orderWheelBigModeTarget = 0;
				if (game->interfaceMode == INTERFACE_PICKING_BUILDING) game->orderWheelBigModeTarget = 1;
				game->orderWheelBigMode = lerp(game->orderWheelBigMode, game->orderWheelBigModeTarget, 0.3);
				game->orderWheelBigMode = 0;
				game->orderWheelBigModeTarget = 0;

				float uiRatio = (game->width / game->height) / (1920.0 / 1080.0);
				uiRatio *= game->scale;
				float bigMode = game->orderWheelBigMode;

				for (int i = 0; i < ordersNum; i++) {
					Order *order = &orders[i];

					Vec2 bigCircleCenter;
					float bigCircleRadius;
					{
						float seperationDeg = 360.0/ordersNum;

						Vec2 screenCenter;
						screenCenter.x = game->width/2;
						screenCenter.y = game->height/2;
						float circleDistance = 300 * uiRatio;
						bigCircleRadius = 100 * uiRatio;
						bigCircleCenter = screenCenter + degToVec2(seperationDeg*i - 90)*circleDistance;
					}

					Vec2 smallCircleCenter;
					float smallCircleRadius;
					{
						smallCircleRadius = 32 * uiRatio;

						int cols = 3;
						int col = i % cols;
						int row = i / cols;
						float chunkSize = wheelCornerRect.width/(float)cols;
						smallCircleCenter.x = (float)col * chunkSize + chunkSize/2 + wheelCornerRect.x;
						smallCircleCenter.y = (float)row * chunkSize + chunkSize/2 + wheelCornerRect.y;
					}

					float circleRadius = smallCircleRadius*(1.0-bigMode) + bigCircleRadius*bigMode;
					Vec2 circleCenter = smallCircleCenter*(1.0-bigMode) + bigCircleCenter*bigMode;

					Rect rect = makeCenteredSquare(circleCenter, circleRadius*2);

					bool pressingOrder = false;
					bool hoveringOrder = false;
					// drawRect(rect, 0x88FF0000);
					if (rect.contains(platform->mouse)) hoveringOrder = true;
					if (mouseJustUp && hoveringOrder) pressingOrder = true;
					if (keyJustPressed(order->key)) pressingOrder = true;

					Texture *iconTexture = NULL;
					char *title = NULL;
					char *desc = NULL;
					int mineralPrice = -1;
					int population = -1;
					if (order->type == ORDER_CONSTRUCT || order->type == ORDER_PLACE) {
						iconTexture = getTexture(frameSprintf("assets/images/unitIcons/%s.png", game->unitInfos[order->unitType].formalName));
						title = game->unitInfos[order->unitType].name;
						mineralPrice = game->unitInfos[order->unitType].mineralPrice;
						population = game->unitInfos[order->unitType].population;
						desc = game->unitInfos[order->unitType].description;
					} else if (order->type == ORDER_OPEN_BUILD_MENU) {
						title = frameSprintf("Build");
					} else if (order->type == ORDER_ATTACK) {
						title = frameSprintf("Attack");
					} else if (order->type == ORDER_STOP) {
						title = frameSprintf("Stop");
					} else if (order->type == ORDER_PATROL) {
						title = frameSprintf("Patrol");
					} else if (order->type == ORDER_EXPLODE) {
						title = frameSprintf("Explode");
					} else {
						title = orderTypeStrings[order->type];
					}

					if (iconTexture) {
						Texture *maskTexture = getTexture("assets/images/circle64.png");

						Matrix3 maskMat = mat3();
						Vec2 circleScale = v2();
						maskMat.TRANSLATE(circleCenter.x, circleCenter.y);
						maskMat.SCALE((circleRadius*2)/(float)game->circle64->width, (circleRadius*2)/(float)game->circle64->height);
						maskMat.TRANSLATE(-(float)game->circle64->width/2, -(float)game->circle64->height/2);

						{
							Texture *backgroundTexture = getTexture("assets/images/ui/alerts/alertBg.png");
							if (backgroundTexture) {
								RenderProps props = newRenderProps(backgroundTexture, rect);

								float perc = lerp(0, 1, game->time);
								while (perc > 1) perc -= 1;

								Matrix3 uvMatrix = mat3();
								uvMatrix.SCALE(0.5, 0.5);
								uvMatrix.TRANSLATE(perc, perc);

								props.uvMatrix = uvMatrix;
								props.tint = 0x88000000;
								// props.maskTexture = maskTexture;
								// props.maskMatrix = &maskMat;
								// props.flags |= _F_CIRCLE;
								drawTexture(backgroundTexture, props);
							}
						}

						Matrix3 mat = mat3();
						mat.TRANSLATE(circleCenter.x, circleCenter.y);
						mat.SCALE((circleRadius*2) / (float)iconTexture->width, (circleRadius*2) / (float)iconTexture->height);
						mat.TRANSLATE(-iconTexture->width/2, -iconTexture->height/2);

						RenderProps props = newRenderProps();
						props.matrix = mat;
						// props.maskTexture = maskTexture;
						// props.maskMatrix = &maskMat;
						// props.flags |= _F_CIRCLE;
						drawTexture(iconTexture, props);
					} else {
						drawCircle(circleCenter, circleRadius, 0xFFCCCCCC);
					}

					if (title) {
						if (game->orderWheelBigModeTarget == 1 && order->key) {
							title = frameSprintf("(%c)%s", order->key, title);
						}

						Rect titleRect;
						for (int i = 0; i < 2; i++) {
							int maxTextWidth = circleRadius*2;

							Font *bigFont = game->bigOrderFont;
							Vec2 bigSize = getTextSize(bigFont, title, maxTextWidth);
							Vec2 bigPos = rect.pos() + rect.size()*v2(0.5, 0.3) - bigSize/2;

							Font *smallFont = game->smallOrderFont;
							Vec2 smallSize = getTextSize(smallFont, title, maxTextWidth);
							Vec2 smallPos = rect.pos() + rect.size()*v2(0.5, 0.5) - smallSize/2.0;

							if (game->orderWheelBigModeTarget == 1) titleRect = makeRect(bigPos, bigSize);
							else titleRect = makeRect(smallPos, smallSize);

							int color = 0xFF000000;
							if (i == 0) {
								// defaultTextDrawFlags |= _F_BLUR13;
								color = 0xFFFFFFFF;
							}

							float bigAlpha = bigMode;
							float smallAlpha = 1.0-bigMode;
							int bigColor = (color & 0x00FFFFFF) | ((int)(255.0 * bigAlpha)<<24);
							int smallColor = (color & 0x00FFFFFF) | ((int)(255.0 * smallAlpha)<<24);
							drawText(bigFont, title, bigPos, bigColor, maxTextWidth);
							drawText(smallFont, title, smallPos, smallColor, maxTextWidth);
							// defaultTextDrawFlags = 0;
						}

						Rect subRect = titleRect;
						subRect.width /= 2;
						subRect.y += subRect.height;
						subRect.height = 160 * uiScale;

						float cellWidth = 100 * uiScale;
						float toAdd = cellWidth - subRect.width;
						subRect.x -= toAdd;
						subRect.width += toAdd;

						bool showSubtitles = false;
						if (game->orderWheelBigModeTarget == 1) showSubtitles = true;
						if (game->orderWheelBigModeTarget == 0 && hoveringOrder) {
							showSubtitles = true;
							subRect.x = platform->mouse.x;
							subRect.y = platform->mouse.y;

							if (subRect.x + subRect.width*2 > game->width) subRect.x -= subRect.width*2;
							if (subRect.y + subRect.height > game->height) subRect.y -= subRect.height;
						}

						if (showSubtitles) {
							float originalSubRectHeight = subRect.height;
							subRect.height = 64*uiScale;
							if (mineralPrice != -1) {
								drawRect(subRect, 0xDD000000);
								Rect rect = subRect;
								rect.width /= 2;
								rect = makeCenteredSquare(rect.center(), 32*uiScale);
								Texture *texture = getTexture("assets/images/ui/hud/minerals.png");
								if (texture) {
									RenderProps props = newRenderProps(texture, rect);
									drawTexture(texture, props);
								}

								rect = subRect;
								rect.width /= 2;
								rect.x += rect.width;
								Vec2 center = rect.center();

								{
									char *str = frameSprintf("%d", mineralPrice);
									Vec2 size = getTextSize(game->defaultFont, str);
									Vec2 pos = center - size/2;
									drawText(game->defaultFont, str, pos, 0xFFFFFFFF);
								}
							}

							subRect.x += subRect.width;
							if (population != -1) {
								drawRect(subRect, 0xDD000000);

								Rect rect = subRect;
								rect.width /= 2;
								rect = makeCenteredSquare(rect.center(), 32);
								Texture *texture = getTexture("assets/images/ui/hud/population.png");
								if (texture) {
									RenderProps props = newRenderProps(texture, rect);
									drawTexture(texture, props);
								}

								rect = subRect;
								rect.width /= 2;
								rect.x += rect.width;
								Vec2 center = rect.center();

								{
									char *str = frameSprintf("%d", population);
									Vec2 size = getTextSize(game->defaultFont, str);
									Vec2 pos = center - size/2;
									drawText(game->defaultFont, str, pos, 0xFFFFFFFF);
								}
							}

							if (desc && desc[0]) {
								subRect.x -= subRect.width;
								subRect.y += subRect.height;
								subRect.height = originalSubRectHeight - subRect.height;
								subRect.width *= 2;
								drawRect(subRect, 0xDD440000);

								float pad = 10;
								float maxWidth = subRect.width - pad*2;
								char *str = desc;
								Vec2 size = getTextSize(game->defaultFont, desc, maxWidth);
								Vec2 pos = subRect.pos() + pad;
								drawText(game->defaultFont, str, pos, 0xFFFFFFFF, maxWidth);
							}
						}

						if (game->orderWheelBigModeTarget == 0) {
							char str[2] = {order->key, 0};
							Vec2 size = getTextSize(game->defaultFont, str);
							Vec2 angle = degToVec2(90 + 45 + 15);
							Vec2 pos = circleCenter + angle * (circleRadius - 10);
							drawCircle(pos+size/2, size.x, 0xEE000000);
							drawText(game->defaultFont, str, pos, 0xFFFFFFFF);
						}
					}

					if (pressingOrder) {
						UnitType unitToBuild = UNIT_NONE;
						UnitType unitToConstruct = UNIT_NONE;

						if (order->type == ORDER_OPEN_BUILD_MENU) {
							game->interfaceMode = INTERFACE_PICKING_BUILDING;
					 } else if (order->type == ORDER_OPEN_BEAVER_MENU) {
							game->interfaceMode = INTERFACE_PICKING_BUILDING;
						} else if (order->type == ORDER_PLACE) {
							unitToBuild = order->unitType;
						} else if (order->type == ORDER_CONSTRUCT) {
							unitToConstruct = order->unitType;
						} else if (order->type == ORDER_ATTACK) {
							game->interfaceMode = INTERFACE_ATTACK_MOVE;
						} else if (order->type == ORDER_HALT) {
							logf("Not handling halt yet\n");
						} else if (order->type == ORDER_PATROL) {
							logf("Not handling patrol yet\n");
						} else if (order->type == ORDER_EXPLODE) {
							GameCmd *gameCmd = createGameCmd();
							if (gameCmd) {
								gameCmd->type = GAME_CMD_EXPLODE;
							}
						} else if (order->type == ORDER_DEPLOY_SPIDERS) {
							GameCmd *gameCmd = createGameCmd();
							if (gameCmd) {
								gameCmd->type = GAME_CMD_EXPLODE;
							}
						} else if (
							order->type == ORDER_AREA_SCAN ||
							order->type == ORDER_SCAN_DART ||
							order->type == ORDER_THROW_FIRE_BOMB ||
							order->type == ORDER_STOP
						) {
							GameCmd *gameCmd = createGameCmd();
							if (gameCmd) {
								if (keyPressed(KEY_SHIFT)) gameCmd->queues = true;
								gameCmd->type = GAME_CMD_GENERIC_ORDER;
								gameCmd->orderType = order->type;
								gameCmd->position = mousePos;
							}
						} else {
							logf("Unknown order %s\n", orderTypeStrings[order->type]);
						}

						if (unitToBuild != UNIT_NONE) {
							UnitInfo *info = &game->unitInfos[unitToBuild];
							bool canBuild = true;

							if (game->minerals[game->playerNumber] < info->mineralPrice) {
								infof("Not enough minerals\n");
								canBuild = false;
							}

							if (!hasPopFor(game->playerNumber, unitToBuild)) canBuild = false;

							if (canBuild) {
								game->interfaceMode = INTERFACE_PLACING_BUILDING;
								game->unitTypeCurrentlyPlacing = unitToBuild;
							}
						}

						if (unitToConstruct != UNIT_NONE) {
							UnitInfo *info = &game->unitInfos[unitToConstruct];
							bool canBuild = true;

							if (game->minerals[game->playerNumber] < info->mineralPrice) {
								infof("Not enough minerals\n");
								canBuild = false;
							}

							if (!hasPopFor(game->playerNumber, unitToConstruct)) canBuild = false;

							if (canBuild) {
								GameCmd *gameCmd = createGameCmd();
								if (gameCmd) {
									gameCmd->type = GAME_CMD_CONSTRUCT_UNIT;
									gameCmd->unitType = unitToConstruct;
									if (keyPressed(KEY_SHIFT)) gameCmd->queues = true;
								}
							}
						}
					}
				}
				popCamera2d();
			}
		}
	}

	/// Handle unit death
	for (int i = 0; i < game->unitsNum; i++) {
		if (gamePaused) break;

		Unit *unit = &game->units[i];
		UnitInfo *info = &game->unitInfos[unit->type];

		if (unit->hp <= 0 && unit->canRemove) {
			destroyLight(unit->lightId);

			for (int i = 0; i < unit->actionsNum; i++) { //@copyPastedRefund
				Action *action = &unit->actions[i];
				game->minerals[unit->playerNumber] += action->mineralPrice;
			}

			removeFromAllControlGroups(unit->id, true);
			removeFromAllControlGroups(unit->id, false);
			removeFromControlGroup(&game->selectedUnits[game->playerNumber], unit->id);
			removeFromControlGroup(&game->localSelectedUnits, unit->id);

#if 0
			{ /// Paint body decal
				pushTargetTexture(game->mapBg);
				pushCamera2d(mat3());
				bool shouldFlip;
				char radChar = radToChar(toRad(unit->visualRotation), &shouldFlip);
				Animation *anim = getAnimation(frameSprintf("%s/Death1A%d", unit->info->formalName, radChar));
				if (anim) {
					Matrix3 mat;
					Matrix3 uvMat;
					RenderProps props;
					Frame *frame = getAnimFrameAtPercent(anim, 1);
					getFrameMatrixAndProps(frame, unit->type, unit->position, shouldFlip, &mat, &uvMat, &props);

					props.colorShift = 0xFFAAAAAA;
					drawTexture(frame->texture, props);
				} else {
					logf("No anim\n");
				}
				popCamera2d();
				popTargetTexture();
			}
#endif

			if (unit->info->isBuilding) game->collisionHasChanged = true;
			arraySplice(game->units, sizeof(Unit) * UNITS_MAX, sizeof(Unit), unit);
			game->unitsNum--;
			i--;
			continue;
		}
	}

	if (game->editorMode == EDITOR_NORMAL) {
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth, 0), ImGuiCond_Always, ImVec2(1, 0));
		ImGui::Begin("MapProps", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("MapProps:");
		ImGui::BeginChild("Map props", ImVec2(0, 200), false, 0); 
		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];
			if (game->editorConfig.onlyShowOrphanMapProps && mapProp->parent != 0) continue;
			if (game->editorConfig.hideGeneratedFromList && mapProp->wasGenerated) continue;
			ImGui::PushID(i);

			char *name = frameMalloc(PATH_MAX_LEN);
			if (mapProp->type == MAP_PROP_IMAGE) strcpy(name, mapProp->imagePath);
			else if (mapProp->type == MAP_PROP_UNIT_SPAWN) strcpy(name, game->unitInfos[mapProp->unitType].formalName);
			else strcpy(name, "???");

			if (ImGui::Selectable(name, game->selectedPropId == mapProp->id)) {
				game->selectedPropId = mapProp->id;
			}
			ImGui::PopID();
		}
		ImGui::EndChild();

		if (ImGui::Button("Create mapProp")) {
			if (map->propsNum < MAP_PROPS_MAX) {
				MapProp *mapProp = &map->props[map->propsNum++];
				mapProp->id = ++map->nextPropId;
				mapProp->size = v2(100, 100);
				game->selectedPropId = mapProp->id;
			} else {
				logf("Too many map props\n");
			}
		}

		MapProp *selectedProp = NULL;
		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];
			if (mapProp->id == game->selectedPropId) {
				selectedProp = mapProp;
				break;
			}
		}

		if (selectedProp) {
			int selectedIndex = -1;
			for (int i = 0; i < map->propsNum; i++) {
				MapProp *prop = &map->props[i];
				if (selectedProp->id == prop->id) {
					selectedIndex = i;
					break;
				}
			}

			MapProp *newParent = NULL;

			if (ImGui::Button("Attach")) {
				game->attaching = !game->attaching;
			}

			ImGui::SameLine();
			if (ImGui::Button("Detach")) {
				MapProp *parent = getMapProp(selectedProp->parent);
				if (parent) {
					Vec2 oldPos = selectedProp->position;
					selectedProp->position.x += parent->matrix.data[6];
					selectedProp->position.y += parent->matrix.data[7];
					selectedProp->position = selectedProp->position.rotate(v2(parent->matrix.data[6], parent->matrix.data[7]), parent->matrix.getRotationDeg());
					selectedProp->rotation -= parent->matrix.getRotationDeg();
				}

				selectedProp->parent = 0; 
			}
			if (game->attaching) { // It's bad that this is in ImGui code
				for (int i = 0; i < map->propsNum; i++) {
					MapProp *mapProp = &map->props[i];
					if (makeRect(0, 0, 1, 1).contains(mapProp->screenSpaceMatrix.invert().multiply(mousePos))) {
						pushTargetTexture(game->tempTexture);
						pushCamera2d(mat3());

						clearRenderer();
						drawRect(0, 0, 1, 1, lerpColor(0x88AA00AA, 0xFFAA00AA, secondPhase));

						popTargetTexture();
						popCamera2d();

						RenderProps props = newRenderProps();
						props.matrix = mapProp->screenSpaceMatrix;
						drawTexture(game->tempTexture, props);
					}

					Vec2 mapPropSpaceMousePos = mapProp->screenSpaceMatrix.invert().multiply(mousePos);
					if (mouseJustDown && makeRect(0, 0, 1, 1).contains(mapPropSpaceMousePos)) {
						bool canParent = true;

						MapProp *head = mapProp;
						for (;;) {
							if (head->id == selectedProp->id) {
								logf("This would cause a circular dependency\n");
								canParent = false;
								game->attaching = false;
								break;
							}

							head = getMapProp(head->parent);
							if (!head) break;
						}

						if (canParent) {
							newParent = mapProp;
							break;
						}
					}
				}
			}

			if (ImGui::TreeNode("Extra")) {
				static int attachToId;
				ImGui::PushItemWidth(100);
				ImGui::InputInt("###attachToId", &attachToId);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				if (ImGui::Button("...attach to")) {
					MapProp *possibleNewParent = getMapProp(attachToId);
					if (possibleNewParent) {
						newParent = possibleNewParent;
					} else {
						logf("No map prop with id %d\n", attachToId);
					}
				}

				static int newPlayerNumber = 0;
				ImGui::PushItemWidth(100);
				ImGui::InputInt("###newPlayerNumber", &newPlayerNumber);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				if (ImGui::Button("...new player number")) {
					MapProp *propsLeft[MAP_PROPS_MAX];
					int propsLeftNum = 0;
					for (int i = 0; i < map->propsNum; i++) if (map->props[i].id != selectedProp->id) propsLeft[propsLeftNum++] = &map->props[i];

					int toChange[MAP_PROPS_MAX];
					int toChangeNum = 0;
					toChange[toChangeNum++] = selectedProp->id;

					for (;;) {
						bool addedSomething = false;

						for (int i = 0; i < propsLeftNum; i++) {
							MapProp *mapProp = propsLeft[i];
							bool shouldSplicePropsLeft = false;
							for (int i = 0; i < toChangeNum; i++) {
								if (mapProp->id == toChange[i] || mapProp->parent == toChange[i]) {
									toChange[toChangeNum++] = mapProp->id;
									addedSomething = true;
									shouldSplicePropsLeft = true;
									break;
								}
							}

							if (shouldSplicePropsLeft) {
								arraySpliceIndex(propsLeft, MAP_PROPS_MAX, sizeof(MapProp *), i);
								i--;
								propsLeftNum--;
								continue;
							}
						}

						if (!addedSomething) break;
					}

					for (int i = 0; i < toChangeNum; i++) {
						MapProp *mapProp = getMapProp(toChange[i]);
						if (mapProp->type != MAP_PROP_UNIT_SPAWN) continue;
						logf("Set player %d on prop %d\n", newPlayerNumber, mapProp->id);
						mapProp->playerNumber = newPlayerNumber;
					}
				}

				if (ImGui::Button("Re roll child modSeeds")) {
					std::function<void(MapProp *)> reRollModSeeds;

					reRollModSeeds = [map, &reRollModSeeds](MapProp *mapProp)->void {
						mapProp->modSeed = rndInt(0, lcgM/2);

						for (int i = 0; i < map->propsNum; i++) {
							if (map->props[i].parent == mapProp->id) {
								reRollModSeeds(&map->props[i]);
							}
						}
					};

					pushRndSeed(time(NULL)+platform->frameCount);
					reRollModSeeds(selectedProp);
					popRndSeed();
				}

				ImGui::TreePop();
			}

			if (newParent) {
				Matrix3 newMatrix = newParent->matrix;
				newMatrix = newMatrix.invert().multiply(selectedProp->matrix.data);
				Vec2 newPos;
				newPos.x = newMatrix.data[6];
				newPos.y = newMatrix.data[7];
				float parentRotation = newParent->matrix.getRotationDeg();

				selectedProp->parent = newParent->id;

				selectedProp->position = newPos;
				if (selectedProp->type == MAP_PROP_UNIT_SPAWN) {
					selectedProp->position.x += selectedProp->size.x/2;
					selectedProp->position.y += selectedProp->size.y/2;
				}

				selectedProp->rotation += parentRotation;
				game->attaching = false;
			}

			if (ImGui::Button("Duplicate")) {
				duplicateMapProp(selectedProp, false);
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				deleteMapProp(selectedProp);
				game->selectedPropId = 0;
				selectedProp = NULL;
			}

			if (ImGui::Button("Move up")) {
				if (selectedIndex > 0) ArraySwap(map->props, selectedIndex, selectedIndex-1);
			}

			ImGui::SameLine();
			if (ImGui::Button("Move down")) {
				if (selectedIndex < map->propsNum-1) ArraySwap(map->props, selectedIndex, selectedIndex+1);
			}
		}

		ImGui::Separator();

		if (selectedProp) {
			ImGui::Text("Id: %d %s", selectedProp->id, selectedProp->wasGenerated ? "(genned)" : "");
			ImGui::InputText("Name", selectedProp->name, MAP_PROP_NAME_MAX_LEN);
			ImGui::Combo("Type", (int *)&selectedProp->type, mapPropTypeStrings, ArrayLength(mapPropTypeStrings));

			ImGui::Separator();
			if (selectedProp->type == MAP_PROP_IMAGE) {
				ImGui::InputText("Image path", selectedProp->imagePath, PATH_MAX_LEN);
				ImGui::DragFloat2("Position", &selectedProp->position.x);
				ImGui::DragFloat2("Size", &selectedProp->size.x, 1);
				ImGui::DragFloat("Rotation", &selectedProp->rotation);
				ImGui::SliderFloat("Transparency", &selectedProp->transparency, 0, 1);
				guiInputArgb("Tint", &selectedProp->tint);
				ImGui::Checkbox("Generates collision", &selectedProp->generatesCollision);
				ImGui::Checkbox("Editor only", &selectedProp->editorOnly);
				ImGui::DragInt("Emits light", &selectedProp->emitsLight);

				ImGui::Separator();

				guiInputArgb("modColorShiftColor", &selectedProp->modColorShiftColor);
				ImGui::SliderFloat2("modColorShiftPower", &selectedProp->modColorShiftPower.x, 0, 1);
				ImGui::SliderFloat2("modRotationBump", &selectedProp->modRotationBump.x, -45, 45);
				ImGui::SliderFloat2("modTransparencyBump", &selectedProp->modTransparencyBump.x, 0, 1);
				ImGui::SliderFloat2("modScaleBump", &selectedProp->modScaleBump.x, -1, 1);
				ImGui::Checkbox("childrenInheritMods", &selectedProp->childrenInheritMods);
				ImGui::DragInt("modSeed", &selectedProp->modSeed);

				ImGui::Separator();

				game->doGenerators |= ImGui::Combo("Generation type", (int *)&selectedProp->generationType, propGenerationTypeStrings, ArrayLength(propGenerationTypeStrings));
				game->doGenerators |= ImGui::SliderInt("generatesProps", &selectedProp->generatesProps, 0, 1024);

				if (selectedProp->generationType == PROP_GENERATION_CIRCLE) {
					game->doGenerators |= ImGui::SliderFloat("generationRadius", &selectedProp->generationRadius, 0, 1024);
				} else if (selectedProp->generationType == PROP_GENERATION_POISSON_CHEAT) {
					game->doGenerators |= ImGui::SliderFloat("Generation count", &selectedProp->generationRadius, 0, 1024);
				}

				if (ImGui::Button("Regenerate")) game->doGenerators = true;

			} else if (selectedProp->type == MAP_PROP_UNIT_SPAWN) {
				ImGui::DragFloat2("Position", &selectedProp->position.x);
				if (ImGui::BeginCombo("UnitType", unitTypeStrings[selectedProp->unitType], ImGuiComboFlags_None)) {
					for (int i = 0; i < UNIT_INFOS_MAX; i++) {
						// if (strContains(unitTypeStrings[i], "UNIT_")) continue;

						if (ImGui::Selectable(unitTypeStrings[i], selectedProp->unitType == i)) {
							selectedProp->unitType = (UnitType)i;
						}
					}
					ImGui::EndCombo();
				}
				ImGui::InputInt("Player number", &selectedProp->playerNumber);
				ImGui::Checkbox("Coop only", &selectedProp->coopOnly);
				ImGui::Checkbox("Non Coop only", &selectedProp->nonCoopOnly);
			} else {
				logf("Unknown map prop type %d\n", selectedProp->type);
			}
			ImGui::Separator();

			ImGui::InputInt("Parent", &selectedProp->parent);
		}

		ImGui::End();

		float nextX = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
		ImGui::Begin("Map", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::InputText("Name", map->name, MAP_NAME_MAX_LEN);

		ImGui::Separator();
		ImGui::Checkbox("Enable coop", &map->enableCoop);
		ImGui::Checkbox("Enable 1v1", &map->enable1v1);
		ImGui::Separator();

		ImGui::SliderFloat("Radiance multi", &map->radianceMulti, 0, 2);
		ImGui::SliderFloat("Min light", &map->minLight, 0, 1);
		guiInputArgb("Background color", &map->backgroundColor);

		ImGui::PushItemWidth(100);
		ImGui::SliderInt("Width", &game->mapEditorWidth, 0, MAP_WIDTH_MAX);
		ImGui::SameLine();
		ImGui::SliderInt("Height", &game->mapEditorHeight, 0, MAP_HEIGHT_MAX);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Resize")) {
			int oldWidth = map->width;
			int oldHeight = map->height;
			map->width = game->mapEditorWidth;
			map->height = game->mapEditorHeight;

			int *newCollisionData = (int *)frameMalloc(MAP_WIDTH_MAX * MAP_HEIGHT_MAX * sizeof(int));

			for (int y = 0; y < oldHeight; y++) {
				for (int x = 0; x < oldWidth; x++) {
					int newIndex = y * map->width + x;
					if (newIndex > (map->width-1) * (map->height-1) || newIndex < 0) continue;

					int oldIndex = y * oldWidth + x;

					if (map->collisionData[oldIndex] != 0) newCollisionData[newIndex] = 1;
				}
			}

			memcpy(map->collisionData, newCollisionData, MAP_WIDTH_MAX * MAP_HEIGHT_MAX * sizeof(int));
			saveMap();
			regenMapDeps(); //@todo Either make this work, or don't
		}

		ImGui::Separator();
		if (ImGui::Button("Edit collision")) {
			game->editorMode = EDITOR_COLLISION;
		}

		ImGui::Separator();

		ImGui::SliderFloat("Perlin freq", &map->perlinFreq, 0.1, 300);
		ImGui::DragFloat2("Perlin scroll", &map->perlinScroll.x);
		guiInputRgb("Perlin color", &map->perlinColor);
		ImGui::Checkbox("Live update", &game->debugLiveUpdatePerlin);

		ImGui::DragFloat("Overlay scale", &map->overlayScale);

		ImGui::Separator();
		ImGui::DragFloat3("Sun position", &map->sunPosition.x);

		if (ImGui::Button("Save map")) saveMap();

		ImGui::SameLine();
		if (ImGui::Button("Load map")) {
			Map *currentMap = &game->map;
			char *mapFileName = frameSprintf("%s.txt", game->currentMapPath);
			char *mapPropsFileName = frameSprintf("%sProps.txt", game->currentMapPath);
			loadStructArray("MapProp", mapPropsFileName, currentMap->props, MAP_PROPS_MAX, sizeof(MapProp));
			loadStruct("Map", mapFileName, currentMap);
		}

		nextX += ImGui::GetWindowWidth();
		ImGui::End();

		{
			ImGui::SetNextWindowPos(ImVec2(nextX, 0), ImGuiCond_Always);

			ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::TreeNode("ImGui demo")) {
				ImGui::ShowDemoWindow();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("GPU")) {
				ImGui::Text("Max texture image units: %d", renderer->maxTextureUnits);
				ImGui::Text("Draw calls per frame: %d", renderer->drawCallsLastFrame);

				ImGui::Text("prevPushesCulled: %d", renderer->prevPushesCulled);
				ImGui::Text("prevPushesProcessed: %d", renderer->prevPushesProcessed);

				if (game->debugViewport.width == 0 && game->debugViewport.height == 0) game->debugViewport = renderer->currentViewport;
				ImGui::DragFloat4("Opengl viewport", &game->debugViewport.x);
				setViewport(game->debugViewport.x, game->debugViewport.y, game->debugViewport.width, game->debugViewport.height);

				if (ImGui::TreeNode("Circle")) {
					pushCamera2d(mat3());
					Rect rect = makeRect(500, 500, 64, 64);
					drawRect(rect, 0xFFFF0000);

					static Vec2 pos = v2(500, 500);
					static float radius = 64;
					static int color = 0xFF00FF00;
					static float thickness = 0;

					ImGui::Begin("Circle", NULL, ImGuiWindowFlags_AlwaysAutoResize);

					ImGui::DragFloat2("pos", &pos.x);
					ImGui::DragFloat("radius", &radius);
					guiInputArgb("color", &color);
					ImGui::SliderFloat("thickness", &thickness, 0, 1);
					ImGui::End();

					drawCircle(rect.center(), radius, 0xFF0000FF);
					// drawCircleOutline(rect.center(), radius, color, thickness);
					popCamera2d();
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Memory")) {
				ImGui::Text("Total chunks ever allocated: %ld", memSys->currentChunkId);
				ImGui::Text("Currently allocted chunks: %d/%d", memSys->activeChunksNum, memSys->activeChunksMax);
				ImGui::Text("Currently empty chunks: %d/%d", memSys->emptyChunksNum, memSys->emptyChunksMax);
				ImGui::Separator();
				ImGui::Text("Total mem allocated: %0.2fmb", (float)memSys->lastFrameTotal / (float)Megabytes(1));
				ImGui::Text("All time allocated: %0.2fmb", (float)memSys->allTime / (float)Megabytes(1));
				ImGui::Text("Frame memory max: %0.2fmb", (float)memSys->frameMemoryMax / (float)Megabytes(1));
				ImGui::TreePop();
			}

			ImGui::Text("playerNumber: %d (team %d), clientId: %d\n", game->playerNumber, game->teamNumber, game->clientId);
			ImGui::Text("turn: %d | _confirmedTurn: %d\nframesThisTurn: %d", game->turn, game->_confirmedTurn, game->framesThisTurn);
			ImGui::Text("Focused: %d\n", platform->windowHasFocus);
			if (ImGui::TreeNode("GameCmdHistory")) {
				inspectGameCmds(game->gameCmdHistory, game->gameCmdHistoryNum);
				ImGui::Separator();
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("GameCmd replay viewer")) {
				ImGui::InputText("Replay file", game->debugReplayFileName, PATH_MAX_LEN);

				if (ImGui::Button("View")) {
					game->gameCmdsReplayMax = game->gameCmdHistoryMax;

					if (game->gameCmdsReplay) free(game->gameCmdsReplay);
					game->gameCmdsReplay = (GameCmd *)zalloc(game->gameCmdsReplayMax * sizeof(GameCmd));
					loadStructArray("GameCmd", game->debugReplayFileName, game->gameCmdsReplay, game->gameCmdsReplayMax, sizeof(GameCmd));

					for (int i = 0; i < game->gameCmdHistoryMax; i++) {
						GameCmd *gameCmd = &game->gameCmdsReplay[i];
						if (gameCmd->type == GAME_CMD_NONE) {
							game->gameCmdsReplayNum = i;
							break;
						}
					}
				}

				ImGui::Separator();
				inspectGameCmds(game->gameCmdsReplay, game->gameCmdsReplayNum);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Local GameCmds")) {
				inspectGameCmds(game->gameCmds, game->gameCmdsNum);
				ImGui::Separator();
				ImGui::TreePop();
			}

			// ImGui::Separator();

			if (ImGui::TreeNode("Log")) {
				ImGui::BeginChild("scrolling", ImVec2(0, 150), false, ImGuiWindowFlags_HorizontalScrollbar);
				for (int i = 0; i < LOGF_BUFFERS_MAX; i++) {
					LogfBuffer *logBuf = &platform->logs[LOGF_BUFFERS_MAX-1 - i];
					if (!logBuf->buffer[0]) continue;
					ImGui::Text(logBuf->buffer);
				}
				ImGui::EndChild();
				ImGui::TreePop();
			}

			if (ImGui::Button("Gain 1000 minerals")) {
				GameCmd *gameCmd = createGameCmd();
				gameCmd->type = GAME_CMD_GAIN_MINERALS;
				gameCmd->targetId = 1000;
			}

			ImGui::SameLine();
			if (ImGui::Button("1x")) {
				GameCmd *gameCmd = createGameCmd();
				gameCmd->type = GAME_CMD_CHANGE_GAME_SPEED;
				gameCmd->floatValue = 1;
			}

			ImGui::SameLine();
			if (ImGui::Button("2x")) {
				GameCmd *gameCmd = createGameCmd();
				gameCmd->type = GAME_CMD_CHANGE_GAME_SPEED;
				gameCmd->floatValue = 2;
			}

			ImGui::SameLine();
			if (ImGui::Button("10x")) {
				GameCmd *gameCmd = createGameCmd();
				gameCmd->type = GAME_CMD_CHANGE_GAME_SPEED;
				gameCmd->floatValue = 10;
			}

			ImGui::SameLine();
			if (ImGui::Button("50x")) {
				GameCmd *gameCmd = createGameCmd();
				gameCmd->type = GAME_CMD_CHANGE_GAME_SPEED;
				gameCmd->floatValue = 50;
			}

			if (ImGui::TreeNode("Misc")) {
				if (ImGui::Button("Desync")) game->units[0].position.x += 1;
				ImGui::SameLine();
				if (ImGui::Button("Crash")) Panic("Crashed");

				if (ImGui::Button("waveTime+=10")) {
					game->waveTime += 10;
					logf("Wave time: %f\n", game->waveTime);
				}

				ImGui::SameLine();
				if (ImGui::Button("Kill all npcs")) {
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						if (unit->playerNumber == game->npcPlayerNumber) {
							unit->hp = 0;
							unit->canRemove = true;
						}
					}
				}

				ImGui::SameLine();
				if (ImGui::Button("A move l/r")) {
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						if (unit->info->isBuilding) continue;
						if (unit->type == UNIT_PROBE) continue;

						Vec2 target = {};
						if (unit->teamNumber == 0) target = v2((map->width-2) * tileSize, unit->position.y);
						if (unit->teamNumber == 1) target = v2(1 * tileSize, unit->position.y);

						Action *action = createAction(unit, false);
						if (action) {
							action->type = ACTION_ATTACK_MOVE;
							action->position = target;
						}
					}
				}

				if (ImGui::Button("Test alert")) {
					Alert *alert = createAlert(ALERT_TEST_ALERT, game->playerNumber, renderer->screenRect.center());
				}

				ImGui::SameLine();
				if (ImGui::Button("Test alert 2")) {
					Alert *alert = createAlert(ALERT_TEST_ALERT_2, game->playerNumber, renderer->screenRect.center());
				}

				if (ImGui::Button("camZoom=1")) game->zoom = 1;
				ImGui::SameLine();
				if (ImGui::Button("camZoom=2")) game->zoom = 2;
				ImGui::SameLine();
				if (ImGui::Button("camZoom=3")) game->zoom = 3;
				ImGui::SameLine();
				if (ImGui::Button("camZoom=5")) game->zoom = 5;

				ImGui::SliderFloat("game->scale", &game->scale, 0.01, 3);
				ImGui::SliderFloat("game->mapBgRenderScale", &game->mapBgRenderScale, 0.01, 3);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Visual toggles")) {
				ImGui::Checkbox("Dijkstra", &game->debugShowDijkstraGrid);
				ImGui::SameLine();
				ImGui::Checkbox("Flow", &game->debugShowFlowField);
				ImGui::SameLine();
				ImGui::Checkbox("Path", &game->debugShowPathfindingData);
				ImGui::SameLine();
				ImGui::Checkbox("Coll", &game->debugAlwaysShowCollisionData);
				ImGui::SameLine();
				ImGui::Checkbox("Build grid", &game->debugShowBuildGrid);
				ImGui::SameLine();
				ImGui::Checkbox("Light", &game->debugShowLightNumbers);
				ImGui::SameLine();
				ImGui::Checkbox("No map", &game->debugHideMinimap);
				ImGui::Separator();

				ImGui::Checkbox("No fog", &game->debugDisableFogOfWar);
				ImGui::SameLine();
				ImGui::Checkbox("A range", &game->debugShowAttackRange);
				ImGui::SameLine();
				ImGui::Checkbox("Unit info", &game->debugShowUnitInfo);
				ImGui::SameLine();
				ImGui::Checkbox("Vision refs", &game->debugShowVisionRefs);
				ImGui::Separator();

				ImGui::Checkbox("No game pass", &game->debugSkipGamePass);
				ImGui::SameLine();
				ImGui::Checkbox("No world pass", &game->debugSkipWorldPass);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Sliders")) {
				ImGui::SliderFloat("game->_debugPacketDropRate", &game->_debugPacketDropRate, 0, 1);
				ImGui::SliderFloat("game->_debugPacketDropRateIn", &game->_debugPacketDropRateIn, 0, 1);
				ImGui::SliderInt("game->_debugSenderLag", &game->_debugSenderLag, 0, 1000);
				ImGui::SliderInt("game->sendEveryXFrames", &sendEveryXFrames, 0, 24);
				ImGui::SliderInt("game->debugGpuStress", &game->debugGpuStress, 0, 100000);
				ImGui::TreePop();
			}

			nextX += ImGui::GetWindowWidth();
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(nextX, 0), ImGuiCond_Always);
			ImGui::Begin("Tools", NULL, ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::TreeNode("Experimental")) {
				WorldProps *world = defaultWorld;

				if (ImGui::Button("Single texture") || ImGui::IsItemActive()) {
					processBatchDraws();
					clearRenderer();
					pushCamera2d(mat3());
					Texture *texture = getTexture("assets/images/white.png");
					RenderProps props = newRenderProps();

					Matrix3 mat = mat3();
					mat.TRANSLATE(256, 256);
					mat.SCALE(10, 10);
					// mat = mat.rotate(10, 10);
					mat.print("mat");
					props.matrix = mat;
					drawTexture(texture, props);

					popCamera2d();
					processBatchDraws();
				}

				if (ImGui::Button("1/1")) {
					changeSpriteSheetQuality("assets/images/spriteSheets/full", 1);
					loadSpriteSheetPngs("assets/images/spriteSheets/full/sheet");
				}
				ImGui::SameLine();
				if (ImGui::Button("1/2")) {
					changeSpriteSheetQuality("assets/images/spriteSheets/half", 2);
					loadSpriteSheetPngs("assets/images/spriteSheets/half/sheet");
				}
				ImGui::SameLine();
				if (ImGui::Button("1/3")) {
					changeSpriteSheetQuality("assets/images/spriteSheets/third", 3);
					loadSpriteSheetPngs("assets/images/spriteSheets/third/sheet");
				}
				ImGui::SameLine();
				if (ImGui::Button("1/4")) {
					changeSpriteSheetQuality("assets/images/spriteSheets/quart", 4);
					loadSpriteSheetPngs("assets/images/spriteSheets/quart/sheet");
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Editor config")) {
				if (ImGui::Button("Save")) saveStruct("EditorConfig", "assets/info/editorConfig.txt", &game->editorConfig);
				ImGui::SameLine();
				if (ImGui::Button("Load")) loadStruct("EditorConfig", "assets/info/editorConfig.txt", &game->editorConfig);

				ImGui::Checkbox("Compress sprite sheets", &game->editorConfig.compressSpriteSheets);
				ImGui::Checkbox("Editor only flash", &game->editorConfig.editorOnlyFlash);
				ImGui::Checkbox("No shadows in editor", &game->editorConfig.noShadowsInEditor);

				ImGui::Checkbox("Only orphans", &game->editorConfig.onlyShowOrphanMapProps);
				ImGui::Checkbox("Show parent", &game->editorConfig.showParent);

				ImGui::Text("Hide generated ");
				ImGui::SameLine();
				ImGui::Checkbox("From screen", &game->editorConfig.hideGeneratedFromScreen);
				ImGui::SameLine();
				ImGui::Checkbox("From list", &game->editorConfig.hideGeneratedFromList);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Unit info editor")) {
				if (ImGui::Button("Save")) saveStructArray("UnitInfo", "assets/info/unitInfo.txt", game->unitInfos, UNIT_INFOS_MAX, sizeof(UnitInfo));
				ImGui::SameLine();
				if (ImGui::Button("Load")) loadStructArray("UnitInfo", "assets/info/unitInfo.txt", game->unitInfos, UNIT_INFOS_MAX, sizeof(UnitInfo));


				static bool skippingEmpty = true;
				ImGui::Checkbox("Skip empty", &skippingEmpty);

				if (ImGui::TreeNode("Misc")) {
					static int cloneSrc = 0;
					static int cloneDest = 0;
					ImGui::Combo("cloneSrc", &cloneSrc, unitTypeStrings, ArrayLength(unitTypeStrings));
					ImGui::Combo("cloneDest", &cloneDest, unitTypeStrings, ArrayLength(unitTypeStrings));

					if (ImGui::Button("Clone")) {
						memcpy(&game->unitInfos[cloneDest], &game->unitInfos[cloneSrc], sizeof(UnitInfo));
					}

					ImGui::TreePop();
				}

				ImGui::BeginChild("Unit info editor child", ImVec2(1024, 600), false, ImGuiWindowFlags_HorizontalScrollbar);
				for (int i = 0; i < UNIT_INFOS_MAX; i++) {
					UnitInfo *info = &game->unitInfos[i];

					if (strContains(unitTypeStrings[i], "UNIT_") && skippingEmpty) continue;

					ImGui::BeginChild(frameSprintf("Unit###%d", i), ImVec2(400, 600), false, ImGuiWindowFlags_AlwaysAutoResize);

					ImGui::Text("Enum name: %s", unitTypeStrings[i]);
					ImGui::InputText("Name", info->name, UNIT_NAME_MAX_LEN);
					ImGui::InputText("formalName", info->formalName, FORMAL_NAME_MAX_LEN);
					ImGui::SliderInt("Size", &info->size, 32, 128);
					ImGui::SliderInt("Speed", &info->speed, 0, 10);
					ImGui::SliderInt("Range", &info->range, 0, 128);

					ImGui::Checkbox("Is building", &info->isBuilding);
					ImGui::SliderInt("Vision radius", &info->visionRadius, 0, 20);

					ImGui::Checkbox("Stealth", &info->stealth);
					ImGui::Checkbox("Flying", &info->flying);
					ImGui::Checkbox("No boids", &info->noBoids);

					if (ImGui::TreeNode("Production")) {
						ImGui::SliderInt("Mineral price", &info->mineralPrice, 0, 1000);
						ImGui::SliderInt("Population", &info->population, 0, 10);
						ImGui::SliderFloat("Creation time", &info->creationTime, 0, 120);
						ImGui::InputText("Description", info->description, UNIT_DESCRIPTION_MAX_LEN);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Combat")) {
						ImGui::Checkbox("noGroundAttack", &info->noGroundAttack);
						ImGui::Checkbox("noAirAttack", &info->noAirAttack);
						ImGui::SliderFloat("Max hp", &info->maxHp, 1, 100);
						ImGui::Combo("Bullet type", (int *)&info->bulletType, bulletTypeStrings, ArrayLength(bulletTypeStrings));
						ImGui::SliderFloat("Damage", &info->damage, 0, 100);
						ImGui::SliderFloat("Slow damage", &info->slowDamage, 0, 100);
						ImGui::SliderFloat("Heat damage", &info->heatDamage, 0, 100);
						ImGui::SliderFloat("Bullet speed", &info->bulletSpeed, 0, 10);
						ImGui::SliderInt("Aggro radius", &info->aggroRadius, 0, 20);
						ImGui::SliderFloat("Pre attack delay", &info->preAttackDelay, 0, 1);
						ImGui::SliderFloat("Post attack delay", &info->postAttackDelay, 0, 1);
						ImGui::TreePop();
					}

					ImGui::Separator();
					if (info->isBuilding) {
						ImGui::Checkbox("Always able to box select", &info->alwaysAbleToBoxSelect);
						ImGui::Checkbox("Hide rally", &info->hideRally);
					}

					ImGui::EndChild();

					ImGui::SameLine();
				}
				ImGui::NewLine();

				ImGui::EndChild();
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Globals")) {
				if (ImGui::Button("Save")) saveStruct("Globals", "assets/info/globals.txt", globals);
				ImGui::SameLine();
				if (ImGui::Button("Load")) loadStruct("Globals", "assets/info/globals.txt", globals);

				ImGui::Checkbox("Do fxaa", &globals->doFxaa);

				ImGui::SliderFloat("spiderMineLifeTime", &globals->spiderMineLifeTime, 0, 32);
				ImGui::SliderInt("spiderMineDeployCount", &globals->spiderMineDeployCount, 0, 32);

				ImGui::TreePop();
			}

			ImGui::End();
		}

		if (selectedProp) {
			MapProp *mapProp = selectedProp;

			Vec2 mapPropSpaceMousePos = mapProp->screenSpaceMatrix.invert().multiply(mousePos);
			if (makeRect(0, 0, 1, 1).contains(mapPropSpaceMousePos) && mouseJustDown) {
				game->propDragPivot = mousePos.subtract(mapProp->position);
				game->draggingProp = true;

				MapProp *parent = getMapProp(mapProp->parent);
				if (parent) game->propDragPivot = (parent->matrix.invert().multiply(mousePos)).subtract(mapProp->position); // This is weirder than it looks
			}

			if (game->draggingProp) {
				Vec2 localMousePos = mousePos;

				MapProp *parent = getMapProp(mapProp->parent);
				if (parent) localMousePos = parent->matrix.invert().multiply(localMousePos);
				mapProp->position = localMousePos.subtract(game->propDragPivot);
			}

			if (!mouseDown) {
				game->draggingProp = false;
			}
		}

		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];
			if (mapProp->wasGenerated && (game->editorConfig.hideGeneratedFromScreen || game->editorConfig.hideGeneratedFromList)) continue;

			Vec2 mapPropSpaceMousePos = mapProp->screenSpaceMatrix.invert().multiply(mousePos);
			if (makeRect(0, 0, 1, 1).contains(mapPropSpaceMousePos) && mouseJustDown && !game->draggingProp && !game->attaching) {
				game->selectedPropId = mapProp->id;
			}
		}
	} else if (game->editorMode == EDITOR_COLLISION) {
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
		ImGui::Begin("Collision editing", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::Button("Add")) game->addingCollision = true;
		ImGui::SameLine();
		if (ImGui::Button("Remove")) game->addingCollision = false;
		ImGui::SameLine();
		if (ImGui::Button("Back")) game->editorMode = EDITOR_NORMAL;
		ImGui::End();
	}

	/// Collision editor
	if (game->editorMode != EDITOR_NONE || game->debugAlwaysShowCollisionData) {
		for (int y = 0; y < map->height; y++) {
			for (int x = 0; x < map->width; x++) {
				Rect rect = tileToWorldRect(v2(x, y));

				int tileIndex = y * map->width + x;

				int color = 0;
				if (game->collisionData[tileIndex] == 1) color = 0x88FF0000;
				if (game->collisionData[tileIndex] == 2) color = 0x880000FF;

				if (game->editorMode == EDITOR_COLLISION) {
					if (rect.contains(mousePos)) {
						if (mouseDown && !platform->hoveringGui && !isHoveringStatus && !isHoveringMinimap && !isHoveringCornerWheel) {
							if (game->addingCollision) {
								map->collisionData[tileIndex] = 1;
							} else {
								map->collisionData[tileIndex] = 0;
							}
						}

						color = 0xFFFFFFFF;
					}
				}

				if (color) drawRect(rect, color);
			}
		}

		for (int y = 0; y < map->height; y++) {
			Vec2 start;
			start.x = 0;
			start.y = y * tileSize;

			Vec2 end;
			end.x = map->width * tileSize;
			end.y = y * tileSize;
			drawLine(start, end, 3, 0x88FFFFFF);
		}

		for (int x = 0; x < map->width; x++) {
			Vec2 start;
			start.x = x * tileSize;
			start.y = 0;

			Vec2 end;
			end.x = x * tileSize;
			end.y = map->height * tileSize;
			drawLine(start, end, 3, 0x44AAAAAA);
		}
	}

	if (game->editorMode != EDITOR_NONE) {
		for (int i = 0; i < map->propsNum; i++) {
			MapProp *mapProp = &map->props[i];
			if (mapProp->type == MAP_PROP_UNIT_SPAWN) {

				Matrix3 mat = game->visualCameraMatrix.multiply(mapProp->screenSpaceMatrix);
				pushCamera2d(mat);
				Rect rect = makeRect(0, 0, 1, 1);
				drawRect(rect, 0x88000000);
				popCamera2d();

				char *str = frameSprintf("%s", unitTypeStrings[mapProp->unitType]);
				Vec2 size = getTextSize(game->smallerFont, str);
				Vec2 pos = mapProp->center - size/2.0f;
				drawText(game->smallerFont, str, pos, 0xFFFFFFFF);
			}

			if (mapProp->id == game->selectedPropId) {
				pushTargetTexture(game->tempTexture);
				pushCamera2d(mat3());

				clearRenderer();
				drawRect(0, 0, 1, 1, lerpColor(0x88AAAA00, 0xCCAA00AA, secondPhase));

				popTargetTexture();
				popCamera2d();

				RenderProps props = newRenderProps();
				props.matrix = mapProp->screenSpaceMatrix;
				drawTexture(game->tempTexture, props);
			}
		}
	}

	if (game->debugGpuStress) {
		pushCamera2d(mat3());
		for (int i = 0; i < game->debugGpuStress; i++) {
			drawRect(makeRect(
					rndFloat(0, game->width),
					rndFloat(0, game->height),
					rndFloat(-game->width, game->width),
					rndFloat(-game->height, game->height)),
				0x88FF0000
			);
		}
		popCamera2d();
	}

	if (!gamePaused) {
		game->firstFrameOfTurn = false;
		game->framesThisTurn++;
	}

	game->timeSinceLastSessionServerContact += elapsed;
	game->timeSinceConfirmTurnUpdate += elapsed;

	if (shouldLoseGame || shouldWinGame) {
		if (game->playMode == PLAY_1V1_ONLINE) {
			game->missingTurnData = false;
#if 0
			MsgPacket packet = {};
			packet.msg.type = NET_MSG_CtS_DISCONNECT_FROM_MATCH;
			packet.msg.matchId = game->matchId;
			packet.msg.clientId = game->clientId;
			bool good = threadSafeQueuePush(game->sessionPacketsOut, &packet);
			if (!good) logf("sessionPacketsOut is full\n");
#endif
			if (game->udpSessionSocket) {
				socketClose(game->udpSessionSocket);
				game->udpSessionSocket = NULL;
			}
			if (game->tcpSessionSocket) {
				socketClose(game->tcpSessionSocket);
				game->tcpSessionSocket = NULL;
			}
		}
		game->matchId = 0;

		platform->useRelativeMouse = false;

		if (game->playMode != PLAY_REPLAY) {
			time_t currentDateTime = time(0);
			tm *now = localtime(&currentDateTime);
			char *name = frameSprintf(
				"assets/replays/replay player%d %d-%d-%d %d-%d-%d.txt",
				game->playerNumber,
				now->tm_mon+1,
				now->tm_mday,
				now->tm_year+1900,
				now->tm_hour,
				now->tm_min,
				now->tm_sec
			);

#if !defined(__EMSCRIPTEN__)
			saveStructArray("GameCmd", name, game->gameCmdHistory, game->gameCmdHistoryNum, sizeof(GameCmd));
#endif
		}

		if (game->playMode == PLAY_SINGLE_PLAYER || game->playMode == PLAY_MISSION) {
			game->state = GAME_STATE_LOG_IN;
			game->serverConnectionState = SERVER_DISCONNECTED;
			game->clientId = 0;
		} else if (game->playMode == PLAY_REPLAY) {
			game->state = GAME_STATE_LOG_IN;
			game->serverConnectionState = SERVER_DISCONNECTED;
			game->clientId = 0;
		} else if (game->playMode == PLAY_1V1_ONLINE) {
			if (shouldDisconnect) {
				if (game->_udpLoginSocket) {
					socketClose(game->_udpLoginSocket);
					game->_udpLoginSocket = NULL;
				}
				if (game->_tcpLoginSocket) {
					socketClose(game->_tcpLoginSocket);
					game->_tcpLoginSocket = NULL;
				}
				game->state = GAME_STATE_LOG_IN;
				game->serverConnectionState = SERVER_DISCONNECTED;
				game->clientId = 0;
			} else {
				game->wonGame = shouldWinGame;
				game->state = GAME_STATE_MATCH_SUMMERY;
				game->serverConnectionState = SERVER_CONNECTED;
			}
		}
	}

	for (int i = 0; i < game->worldChannelsNum; i++) {
		Channel *channel = getChannel(game->worldChannels[i]);
		if (!channel) {
			arraySpliceIndex(game->worldChannels, WORLD_CHANNELS_MAX, sizeof(int), i);
			i--;
			game->worldChannelsNum--;
			continue;
		}

		float dist = renderer->screenRect.center().distance(channel->position);
		float volPerc = 1.0 - dist/renderer->screenRect.width;

		if (volPerc < 0) volPerc = 0;
		channel->userVolume = volPerc;

		float pan = (channel->position.x - renderer->screenRect.center().x) / renderer->screenRect.width;
		pan = mathClamp(pan, -0.75, 0.75);
		channel->pan = pan;
	}


	if (game->debugShowBuildGrid) {
		for (int y = 0; y < map->height; y++) {
			for (int x = 0; x < map->width; x++) {
				int index = y * map->width + x;
				if (getBit(buildGrids[0], index)) {
					Rect rect = tileToWorldRect(v2(x, y));
					drawRect(rect, 0x880000FF);
				}
				if (getBit(buildGrids[1], index)) {
					Rect rect = tileToWorldRect(v2(x, y));
					drawRect(rect, 0x88FF0000);
				}
			}
		}
	}

	{
		// Rect rect = renderer->cameraRect;
		// rect = rect.inflate(v2(-rect.width/2.0, -rect.height/2.0));
		// pushCamera2d(mat3());
		// drawRect(rect, 0x99FFFFFF);
		// popCamera2d();
	}

	for (int i = 0; i < 10; i++) {
		Texture *texture = getTexture("assets/images/white1.png");
		RenderProps props = newRenderProps();
		Vec2 pos;
		Vec2 scale;
		pos.x = i * (64+5);
		pos.y = -128;
		scale.x = 64;
		scale.y = 64;
		props.matrix.TRANSLATE(pos);
		props.matrix.SCALE(scale);

		u8 col = 255.0*(i/10.0);
		props.tint = argbToHex(255, col, col, col);
		if (keyPressed(' ')) props.flags |= _F_INVERSE_SRGB;
		drawTexture(texture, props);

		Rect rect = makeRect(pos.x, pos.y, scale.x, scale.y);
		rect.y -= 80;
		drawRect(rect, argbToHex(255, col, col, col));
	}

	if (renderer->currentTargetTexture == game->gameTexture) {
		popTargetTexture();
		{
			pushCamera2d(mat3());

#if 1
			RenderProps props = newRenderProps();
			props.matrix.SCALE(1.0/game->scale);
			drawTexture(game->gameTexture, props);
#else
			Vec2 verts[4];
			verts[0] = v2(0, 0);
			verts[1] = v2(1, 0);
			verts[2] = v2(1, 1);
			verts[3] = v2(0, 1);
			for (int i = 0; i < 4; i++) {
				verts[i].x = verts[i].x * 2 - 1;
				verts[i].y = verts[i].y * 2 - 1;
			}

			Vec2 uvs[4];
			uvs[0] = v2(0, 0);
			uvs[1] = v2(1, 0);
			uvs[2] = v2(1, 1);
			uvs[3] = v2(0, 1);

			u16 inds[6] = {0, 2, 1, 0, 3, 2};

			Matrix3 mat = mat3();

			Matrix3 uvMat = mat3();
			// glEnable(GL_FRAMEBUFFER_SRGB);
			draw2dMesh(verts, uvs, inds, 6, mat, game->gameTexture);
			// glDisable(GL_FRAMEBUFFER_SRGB);
#endif
			popCamera2d();
		}
	} else {
		logf("Why would this happen?\n");
	}
}

void loadSprites() {
	// int zipDataNum;
	// u8 *zipData = (u8 *)readFile("assets/testZip.zip", &zipDataNum);
	// Zip *zip = openZip((unsigned char *)zipData, zipDataNum);
	// extractZip(zip);
	// closeZip(zip);
	// free(zipData);
	// exit(0);

	if (fileExists("assets/unitFramesFull.txt")) {
		loadCachedSpriteSheetData("assets/unitFramesFull.txt");

		logf("Gpu vendor: %s\n", renderer->vendor);
#if defined(__EMSCRIPTEN__)
		changeSpriteSheetQuality("assets/images/spriteSheets/quart", 4);
		bool good = loadSpriteSheetPngs("assets/images/spriteSheets/quart/sheet");
#else
		bool good = loadSpriteSheetPngs("assets/images/spriteSheets/full/sheet");
#endif

		if (!good) {
			if (!platform->isInternalVersion) Panic("Can't load sprite sheets!\n");
			logf("Failed to load sprite sheets, repacking...\n");
			packSpriteSheet("c:/binFrames/rtsUnits", "assets/unitFramesFull.txt", "assets/images/spriteSheets/full/sheet", game->editorConfig.compressSpriteSheets);
			packOtherUnitInfo();
		}
	} else {
		if (!platform->isInternalVersion) Panic("Sprite sheets missing!");
		packSpriteSheet("c:/binFrames/rtsUnits", "assets/unitFramesFull.txt", "assets/images/spriteSheets/full/sheet", game->editorConfig.compressSpriteSheets);
		packOtherUnitInfo();
	}
}

void packOtherUnitInfo() {
	int pathsNum;
	char **paths = getDirectoryList("c:/binFrames/rtsUnits", &pathsNum);
	for (int i = 0; i < pathsNum; i++) {
		if (strContains(paths[i], "params.vars")) {
			char *vars = (char *)readFile(paths[i]);

			char *lineEnd = strchr(vars, '\n');
			int lineLen = lineEnd - vars;
			char *line = frameMalloc(lineLen * sizeof(char));
			strncpy(line, vars, lineLen);

			char *entryStart = line;
			UnitType currentUnitType = UNIT_NONE;
			for (int i = 0; ; i++) {
				char *entryEnd = strchr(entryStart, ':');
				bool shouldBreak = false;
				if (!entryEnd) {
					entryEnd = entryStart + strlen(entryStart) - 1;
					shouldBreak = true;
				}
				const int entryMax = 64;
				char entry[entryMax] = {};
				strncpy(entry, entryStart, entryEnd - entryStart);

				if (i == 0) {
					for (int i = 0; i < ArrayLength(unitTypeStrings); i++) {
						if (streq(unitTypeStrings[i], entry)) {
							currentUnitType = (UnitType)i;
							break;
						}
					}

					if (currentUnitType == UNIT_NONE) logf("Couldn't find unit %s\n", entry);
				} else if (i == 1) {
					float resMulti = atof(entry);
					UnitInfo *info = &game->unitInfos[currentUnitType];
					// info->resMulti = resMulti;
				}
				entryStart = entryEnd+1;

				if (shouldBreak) break;
			}
		}

		free(paths[i]);
	}
	free(paths);
}

void saveMap() {
	Map *currentMap = &game->map;
	for (int i = 0; i < currentMap->propsNum; i++) {
		MapProp *mapProp = &currentMap->props[i];

		for (int i = 0; i < strlen(mapProp->imagePath); i++) {
			if (mapProp->imagePath[i] == '\\') mapProp->imagePath[i] = '/';
		}

		if (mapProp->id == 0) mapProp->id = ++currentMap->nextPropId;

		if (mapProp->wasGenerated) {
			arraySpliceIndex(currentMap->props, currentMap->propsNum, sizeof(MapProp), i);
			currentMap->propsNum--;
			i--;
			continue;
		}
	}

	game->doGenerators = true;

	char *mapFileName = frameSprintf("%s.txt", game->currentMapPath);
	char *mapPropsFileName = frameSprintf("%sProps.txt", game->currentMapPath);
	saveStructArray("MapProp", mapPropsFileName, currentMap->props, currentMap->propsNum, sizeof(MapProp));
	saveStruct("Map", mapFileName, currentMap);
}

Alert *createAlert(AlertType type, Unit *unit, const char *customMsg) {
	if (
		type == ALERT_UNIT_UNDER_ATTACK ||
		type == ALERT_BUILDING_UNDER_ATTACK ||
		type == ALERT_PROBES_UNDER_ATTACK
	) {
		if (unit->timeSinceAttackAlert < 10.0) return NULL;
	}

	Alert *alert = createAlert(type, unit->playerNumber, unit->position, customMsg);
	alert->unitId = unit->id;
	return alert;
}

Alert *createAlert(AlertType type, int playerNumber, Vec2 position, const char *customMsg) {
	if (game->alertsNum > ALERTS_MAX-1) {
		arraySpliceIndex(game->alerts, ALERTS_MAX, sizeof(Alert), 0);
		game->alertsNum--;
	}

	if (game->onlyOneAlert) game->alertsNum = 0;

	Alert *alert = &game->alerts[game->alertsNum++];
	memset(alert, 0, sizeof(Alert));
	alert->type = type;
	alert->playerNumber = playerNumber;
	alert->position = position;
	if (game->onlyOneAlert) alert->isOnlyOne = true;
	if (customMsg) strncpy(alert->customMsg, customMsg, ALERT_CUSTOM_MSG_MAX_LEN-1);

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->playerNumber != game->playerNumber) continue;

		if (unit->position.distance(alert->position) < 512) {
			if (
				type == ALERT_UNIT_UNDER_ATTACK ||
				type == ALERT_BUILDING_UNDER_ATTACK ||
				type == ALERT_PROBES_UNDER_ATTACK
			) {
				unit->timeSinceAttackAlert = 0;
			}
		}
	}

	for (int i = 0; i < DIALOGS_MAX; i++) {
		Dialog *dialog = &game->dialogs[i];
		if (dialog->alertType == type) {
			char *path = frameSprintf("assets/audio/generated/%s.ogg", alertTypeStrings[dialog->alertType]);
			playGameSound(path);
		}
	}
	return alert;
}

Unit **getUnitsInCircle(Vec2 position, float range, int *unitsNum) {
	if (game->spatialBinsNum == 0) {
		*unitsNum = 0;
		return NULL;
	}

	Unit *tempUnits[UNITS_MAX];
	int tempUnitsNum = 0;
#if 1
	float binWidth = game->spatialBins[0].region.width;
	float binHeight = game->spatialBins[0].region.height;
	int binsWide = sqrt(BINS_MAX);
	int binsHigh = sqrt(BINS_MAX);

	Vec2 minPoint = position - v2(range, range);
	Vec2 maxPoint = position + v2(range, range);
	int minBinX = minPoint.x / binWidth;
	int minBinY = minPoint.y / binHeight;
	int maxBinX = ceilf(maxPoint.x / binWidth);
	int maxBinY = ceilf(maxPoint.y / binHeight);
	if (minBinX < 0) minBinX = 0;
	if (minBinX > binsWide) minBinX = binsWide;

	if (maxBinX < 0) maxBinX = 0;
	if (maxBinX > binsWide) maxBinX = binsWide;

	if (minBinY < 0) minBinY = 0;
	if (minBinY > binsHigh) minBinY = binsHigh;

	if (maxBinY < 0) maxBinY = 0;
	if (maxBinY > binsHigh) maxBinY = binsHigh;

	for (int y = minBinY; y < maxBinY; y++) {
		for (int x = minBinX; x < maxBinX; x++) {
			int binIndex = y * binsWide + x;
			SpatialBin *bin = &game->spatialBins[binIndex];
			for (int i = 0; i < bin->unitsNum; i++) {
				Unit *unit = bin->units[i];
				if (contains(unit->rect, makeCircle(position, range))) {
					tempUnits[tempUnitsNum++] = unit;
				}
			}
		}
	}

	// for (int i = 0; i < game->spatialBinsNum; i++) {
	// 	SpatialBin *bin = &game->spatialBins[i];
	// 	if (bin->region.containsCircle(position, range)) {
	// 		for (int i = 0; i < bin->unitsNum; i++) {
	// 			Unit *unit = bin->units[i];
	// 			if (unit->rect.containsCircle(position, range)) {
	// 				tempUnits[tempUnitsNum++] = unit;
	// 			}
	// 		}
	// 	}
	// }
#else
	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->rect.containsCircle(position, range)) tempUnits[tempUnitsNum++] = unit;
	}

#endif
	Unit **units = (Unit **)frameMalloc(tempUnitsNum * sizeof(Unit *));
	memcpy(units, tempUnits, tempUnitsNum * sizeof(Unit *));
	*unitsNum = tempUnitsNum;
	return units;
}

GameCmd *createGameCmd() {
	if (game->gameCmdsNum > GAME_CMDS_MAX-2) { // Leave an extra space for GAME_CMD_END_TURN
		logf("Too many GameCmds!\n");
		return NULL;
	}

	int turnToExecute = game->turn+turnDelay;

	if (game->turnEnded) {
		// return NULL;
		turnToExecute++;
	}

	GameCmd *gameCmd = &game->gameCmds[game->gameCmdsNum++];
	memset(gameCmd, 0, sizeof(GameCmd));
	gameCmd->clientId = game->clientId;
	gameCmd->playerNumber = game->playerNumber;
	gameCmd->turnToExecute = turnToExecute;
	gameCmd->worldHash = game->currentWorldHash;
	return gameCmd;
}

Action *createAction(Unit *unit, bool queues) {
	if (unit->actionsNum > ACTIONS_MAX-1) {
		logf("Too many Actions!\n");
		return NULL;
	}

	if (unit->actionsNum > 0 && unit->actions[0].type == ACTION_DEATH) {
		return NULL;
	}

	// if (unit->actionsNum > 0) {
	// 	Action *action = &unit->actions[0];
	// 	if (action->type == ACTION_AREA_SCAN) {
	// 		queues = true;
	// 	}
	// }

	Action *action = NULL;

	if (queues) {
		action = &unit->actions[unit->actionsNum++];
	} else {
		for (int i = 0; i < unit->actionsNum; i++) { //@copyPastedRefund
			Action *action = &unit->actions[i];
			game->minerals[unit->playerNumber] += action->mineralPrice;
		}

		action = &unit->actions[0];
		unit->actionsNum = 1;
		unit->firstFrameOfAction = true;
		unit->actionTime = 0;
	}

	memset(action, 0, sizeof(Action));
	return action;
}

Action *injectAction(Unit *unit, int afterIndex) {
	if (unit->actionsNum > ACTIONS_MAX-1) {
		logf("Too many Actions!\n");
		return NULL;
	}

	memmove(&unit->actions[afterIndex+2], &unit->actions[afterIndex+1], (unit->actionsNum - afterIndex - 1) * sizeof(Action));

	Action *action = &unit->actions[afterIndex+1];
	memset(action, 0, sizeof(Action));

	unit->actionsNum++;
	return action;
}

MapProp *getMapPropByName(const char *name) {
	if (!name) return NULL;

	Map *map = &game->map;
	for (int i = 0; i < map->propsNum; i++) {
		MapProp *mapProp = &map->props[i];
		if (streq(mapProp->name, name)) return mapProp;
	}

	return NULL;
}

MapProp *getMapProp(int id) {
	if (id == 0) return NULL;

	Map *map = &game->map;
	for (int i = 0; i < map->propsNum; i++) {
		MapProp *mapProp = &map->props[i];
		if (mapProp->id == id) return mapProp;
	}

	return NULL;
}

Matrix3 getMapPropMatrix(MapProp *mapProp) {
	Matrix3 parentMat;

	MapProp *parent = getMapProp(mapProp->parent);
	if (parent) {
		if (parent->id != mapProp->id) {
			parentMat = getMapPropMatrix(parent);
		} else {
			mapProp->id = 0;
			parentMat = mat3();
		}
	} else {
		parentMat = mat3();
	}

	Matrix3 mat = mat3();
	mat = mat.multiply(parentMat.data);
	if (mapProp->type == MAP_PROP_IMAGE) {
		mat.TRANSLATE(mapProp->position.x, mapProp->position.y);
		mat.ROTATE(-mapProp->rotation);
	} else if (mapProp->type == MAP_PROP_UNIT_SPAWN) {
		UnitInfo *info = &game->unitInfos[mapProp->unitType];
		mat.TRANSLATE(mapProp->position.x - info->size/2, mapProp->position.y - info->size/2);
	}

	return mat;
}


void deleteMapProp(MapProp *toDelete) {
	Map *map = &game->map;
	for (int i = 0; i < map->propsNum; i++) {
		MapProp *mapProp = &map->props[i];
		if (mapProp->parent == toDelete->id) {
			deleteMapProp(mapProp);
			i--;
			continue;
		}
	}

	arraySplice(map->props, sizeof(MapProp) * MAP_PROPS_MAX, sizeof(MapProp), toDelete);
	map->propsNum--;
}
MapProp *duplicateMapProp(MapProp *toDup, bool wasGenerated) {
	if (toDup->wasGenerated) return NULL;

	Map *map = &game->map;
	if (map->propsNum > MAP_PROPS_MAX-1) {
		logf("Too many map props\n");
		return NULL;
	}

	MapProp *dupProp = &map->props[map->propsNum++];
	memcpy(dupProp, toDup, sizeof(MapProp));
	dupProp->id = ++map->nextPropId;
	dupProp->time = 0;
	if (wasGenerated) {
		dupProp->wasGenerated = true;
		dupProp->childrenInheritMods = false;
		dupProp->generatesProps = 0;
	}

	for (int i = 0; i < map->propsNum; i++) {
		MapProp *mapProp = &map->props[i];
		if (mapProp->parent == toDup->id) {
			MapProp *newChild = duplicateMapProp(mapProp, wasGenerated);
			if (!newChild) continue;
			newChild->parent = dupProp->id;
		}
	}

	return dupProp;
}

Unit *getUnit(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->id == id) return unit;
	}

	return NULL;
}

Unit *createUnit(int playerNumber, int teamNumber, UnitType type) {
	if (!hasPopFor(playerNumber, type)) return NULL;

	Unit *unit = &game->units[game->unitsNum++];
	memset(unit, 0, sizeof(Unit));
	unit->id = ++game->nextUnitId;
	unit->type = type;
	unit->playerNumber = playerNumber;
	unit->teamNumber = teamNumber;

	UnitInfo *info = &game->unitInfos[unit->type];
	unit->info = info;
	unit->hp = info->maxHp;
	unit->timeSinceAttackAlert = 999;

	if (unit->type == UNIT_MINERALS) unit->mineralAmount = mineralAmountMax;

	if (unit->info->isBuilding) game->collisionHasChanged = true;
	recountPopulation();

	unit->lightId = createLight(unit->teamNumber);
	if (info->flying) {
		LightCasterRef *casterRef = getLightCasterRef(unit->lightId);
		casterRef->isFlying = true;
	}

	return unit;
}

bool hasPopFor(int playerNumber, UnitType type) {
	bool showError = game->playerNumber == playerNumber;
	if (playerNumber == 2) showError = true;
	if (playerNumber == -1) showError = true;

	int requiredPop = game->unitInfos[type].population;
	if (playerNumber == -1) {
		if (game->negOnePopulationCount + requiredPop > UNITS_MAX/2) {
			logf("negOne is at max population!\n");
		}
	} else {
		if (game->populationCounts[playerNumber] + requiredPop > UNITS_MAX/2) {
			if (showError) logf("You are at max population!\n");
			return false;
		}
	}

	return true;
}

void dealDamage(Unit *dest, Unit *src, float amount, Vec2 damagePosition) {
	dest->damagedBlinkAmount = amount * 6;
	dest->hp -= amount;

	if (amount > 0 && game->teamNumber == dest->teamNumber) {
		AlertType alertType = ALERT_NONE;
		if (dest->info->isBuilding) alertType = ALERT_BUILDING_UNDER_ATTACK;
		if (dest->type == UNIT_PROBE) alertType = ALERT_PROBES_UNDER_ATTACK;
		else alertType = ALERT_UNIT_UNDER_ATTACK;

		Assert(alertType != ALERT_NONE);
		createAlert(alertType, dest);
	}
}

Bullet *createBullet(Unit *src, Unit *dest) {
	Bullet *bullet = &game->bullets[game->bulletsNum++];

	bullet->srcId = src->id;
	bullet->destId = dest->id;
	bullet->position = src->position;

	return bullet;
}

bool hasLineOfSight(Vec2Tile start, Vec2Tile goal, bool forVision) {
	Map *map = &game->map;

	u8 x0 = start.x;
	u8 y0 = start.y;
	u8 x1 = goal.x;
	u8 y1 = goal.y;

	u8 dx = abs(x1-x0);
	u8 sx = x0<x1 ? 1 : -1;
	u8 dy = abs(y1-y0);
	u8 sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2;
	int e2;

	for (;;) {
		if (forVision) {
			if (map->collisionData[y0 * map->width + x0] != 0) return false;
			if (game->collisionData[y0 * map->width + x0] == 2) return false;
		} else {
			if (game->collisionData[y0 * map->width + x0] != 0) return false;
		}

		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}

	return true;
}

bool hasLineOfSightThick(int *tiles, int tilesWide, int tilesHigh, Vec2Tile start, Vec2Tile goal, int thickness) {
	u8 x0 = start.x;
	u8 y0 = start.y;
	u8 x1 = goal.x;
	u8 y1 = goal.y;

	u8 dx = abs(x1-x0);
	u8 sx = x0<x1 ? 1 : -1;
	u8 dy = abs(y1-y0);
	u8 sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2;
	int e2;

	for (;;) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < thickness; j++) {
				u8 nx = x0;
				u8 ny = y0;

				if (i == 0) ny -= j;
				if (i == 1) nx += j;
				if (i == 2) ny += j;
				if (i == 3) nx -= j;

				if (nx < 0 || nx > tilesWide-1) continue;
				if (ny < 0 || ny > tilesHigh-1) continue;

				if (tiles[ny * tilesWide + nx] != 0) return false;
			}
		}

		if (tiles[y0 * tilesWide + x0] == 1) return false;

		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}

	return true;
}

Vec2 getClosestReachableNode(int *tiles, int tilesWide, int tilesHigh, Vec2 start, Vec2 goal) {
	NanoTime startTime;
	getNanoTime(&startTime);

	int vec2Size = sizeof(Vec2Tile);
	Vec2Tile realStart = v2Tile(start.x, start.y);
	Vec2Tile realGoal = v2Tile(goal.x, goal.y);

	Allocator queueAllocator = {};
	queueAllocator.type = ALLOCATOR_FRAME;
	Queue *frontier = createQueue(vec2Size, tilesWide*tilesHigh, &queueAllocator);

	bool good = queuePush(frontier, &realStart);
	if (!good) logf("queue is full\n");

	Vec2Tile closest = v2Tile(0, 0);
	float closestDist = 99999999.0f;

	bool *visited = (bool *)frameMalloc(tilesWide*tilesHigh*sizeof(bool));

	while (frontier->length > 0) {
		Vec2Tile current;
		bool good = queueShift(frontier, &current);
		if (!good) logf("queue is empty\n");

		if (current.equal(realGoal)) {
			closest = realGoal;
			break;
		}

		for (int i = 0; i < 4; i++) {
			Vec2Tile neighbor;

			if (i == 0 && current.x == 0) continue;
			if (i == 1 && current.x >= tilesWide-1) continue;
			if (i == 2 && current.y == 0) continue;
			if (i == 3 && current.y >= tilesHigh-1) continue;

			if (i == 0) neighbor = v2Tile(current.x-1, current.y);
			if (i == 1) neighbor = v2Tile(current.x+1, current.y);
			if (i == 2) neighbor = v2Tile(current.x, current.y-1);
			if (i == 3) neighbor = v2Tile(current.x, current.y+1);

			int neighborIndex = neighbor.y * tilesWide + neighbor.x;
			if (tiles[neighborIndex] == 1) continue;

			bool wasVisited = visited[neighborIndex];
			if (!wasVisited) {
				bool good = queuePush(frontier, &neighbor);
				if (!good) logf("queue is full\n");
				bool value = true;
				visited[neighborIndex] = true;

				if (closest.isZero()) {
					closest = neighbor;
				} else {
					float neighborDist = neighbor.distance(realGoal);
					if (neighborDist < closestDist) {
						closest = neighbor;
						closestDist = neighborDist;
					}
				}
			}
		}
	}

	destroyQueue(frontier);

	NanoTime endTime;
	getNanoTime(&endTime);
	float ms = getMsPassed(&startTime, &endTime);
	// if (game->playerNumber == 0) logf("This took %fms\n", ms);

	return v2(closest.x, closest.y);
}

void generateFlowField(int *tiles, int tilesWide, int tilesHigh, Vec2 start, int *dijkstraGrid, char *outGrid) {
	NanoTime startTime;
	getNanoTime(&startTime);

	memset(outGrid, 0, tilesWide * tilesHigh * sizeof(char));
	for (int y = 0; y < tilesHigh; y++) {
		for (int x = 0; x < tilesWide; x++) {
			Vec2 current = v2(x, y);
			int dijkstraValue = dijkstraGrid[y * tilesWide + x];

			//Obstacles have no flow value
			if (dijkstraValue == -1 || dijkstraValue == 0) {
				outGrid[y * tilesWide + x] = -1;
				continue;
			}

			bool canUseDiagonals = true;

			for (int i = 0; i < 4; i++) {
				Vec2 neighbor;
				if (i == 0) neighbor = v2(current.x-1, current.y);
				if (i == 1) neighbor = v2(current.x+1, current.y);
				if (i == 2) neighbor = v2(current.x, current.y-1);
				if (i == 3) neighbor = v2(current.x, current.y+1);

				if (neighbor.x < 0 || neighbor.x > tilesWide-1) continue;
				if (neighbor.y < 0 || neighbor.y > tilesHigh-1) continue;

				int neighborCollisionIndex = neighbor.y * tilesWide + neighbor.x;
				int neighborDijkstraValue = dijkstraGrid[neighborCollisionIndex];
				if (neighborDijkstraValue == -1) {
					canUseDiagonals = false;
					break;
				}
			}

			//Go through all neighbours and find the one with the lowest distance
			Vec2 min = v2();
			float minDist = 2;

			for (int i = 0; i < 8; i++) {
				if (!canUseDiagonals && i >= 4) continue;
				Vec2 neighbor;
				if (i == 0) neighbor = v2(current.x-1, current.y);
				if (i == 1) neighbor = v2(current.x+1, current.y);
				if (i == 2) neighbor = v2(current.x, current.y-1);
				if (i == 3) neighbor = v2(current.x, current.y+1);
				if (i == 4) neighbor = v2(current.x-1, current.y-1);
				if (i == 5) neighbor = v2(current.x+1, current.y+1);
				if (i == 6) neighbor = v2(current.x+1, current.y-1);
				if (i == 7) neighbor = v2(current.x-1, current.y+1);

				if (neighbor.x < 0 || neighbor.x > tilesWide-1) continue;
				if (neighbor.y < 0 || neighbor.y > tilesHigh-1) continue;

				int neighborCollisionIndex = neighbor.y * tilesWide + neighbor.x;
				int neighborDijkstraValue = dijkstraGrid[neighborCollisionIndex];
				if (neighborDijkstraValue == -1) continue;
				int dist = neighborDijkstraValue - dijkstraValue;

				if (dist < minDist) {
					min = neighbor;
					minDist = dist;
				}
			}

			//If we found a valid neighbour, point in its direction
			if (!min.isZero()) {
				float rad = radsBetween(current, min);
				char charDirection = degToChar(toDeg(rad));
				outGrid[y * tilesWide + x] = charDirection;
			} else {
				outGrid[y * tilesWide + x] = -2;
			}
		}
	}

	NanoTime endTime;
	getNanoTime(&endTime);
	float ms = getMsPassed(&startTime, &endTime);
	// if (game->playerNumber == 0) logf("This took %fms\n", ms);
}

char radToChar(float rad, bool *shouldFlip) {
	while (rad < 0) rad += 2*M_PI;
	while (rad > 2*M_PI) rad -= 2*M_PI;
	int result = (int)(8.0 * (rad+M_PI/2) / M_PI + 0.5) % 16;

	if (shouldFlip) {
		*shouldFlip = false;
		if (result > 8) {
			int dist = result - 8;
			result -= dist*2;
			*shouldFlip = true;
		}
	}

	return result;
}

float charToRad(char rad) {
	return 0;
	// return (int)(4.0 * rad / M_PI + 0.5) % 8;
}

char degToChar(float deg) {
	deg = roundf(deg);
	char ch = -1;
	if (deg == 0 || deg == 360) ch = 1;
	else if (deg == 45 || deg == -315) ch = 2;
	else if (deg == 90 || deg == -270) ch = 3;
	else if (deg == 135 || deg == -225) ch = 4;
	else if (deg == 180 || deg == -180) ch = 5;
	else if (deg == 225 || deg == -135) ch = 6;
	else if (deg == 270 || deg == -90) ch = 7;
	else if (deg == 315 || deg == -45) ch = 8;
	else logf("Unused deg: %f\n", deg);
	return ch;
}

float charToDeg(char ch) {
	float deg = -1;
	if (ch == 1) deg = 0;
	else if (ch == 2) deg = 45;
	else if (ch == 3) deg = 90;
	else if (ch == 4) deg = 135;
	else if (ch == 5) deg = 180;
	else if (ch == 6) deg = 225;
	else if (ch == 7) deg = 270;
	else if (ch == 8) deg = 315;
	else logf("Unused char: %d\n", ch);

	return deg;
}

void generateDijkstraGrid(int *tiles, int tilesWide, int tilesHigh, Vec2 start, int size, int *outGrid) {
	Vec2Tile realStart = v2Tile(start.x, start.y);
	int vec2Size = sizeof(Vec2Tile);

	Allocator priorityQueueAllocator = {};
	priorityQueueAllocator.type = ALLOCATOR_FRAME;
	PriorityQueue *frontier = createPriorityQueue(vec2Size, &priorityQueueAllocator);
	priorityQueuePush(frontier, &realStart, 0);

	Allocator hashMapAllocator = {};
	hashMapAllocator.type = ALLOCATOR_FRAME;
	HashMap *costSoFar = createHashMap(vec2Size, sizeof(int), 8192, &hashMapAllocator);

	int zero = 0;
	hashMapSet(costSoFar, &realStart, realStart.getFloatHashAsInt(), &zero);

	int startIndex = realStart.y * tilesWide + realStart.x;
	outGrid[startIndex] = 1;

	while (frontier->length > 0) {
		Vec2Tile current;
		bool good = priorityQueueShift(frontier, &current);
		if (!good) logf("queue is empty\n");

		int currentCollisionIndex = current.y * tilesWide + current.x;

		if (tiles[currentCollisionIndex] != 0) {
			outGrid[currentCollisionIndex] = -1;
			continue;
		}

		for (int i = 0; i < 4; i++) {
			Vec2Tile neighbor;
			if (i == 0) neighbor = v2Tile(current.x-1, current.y);
			if (i == 1) neighbor = v2Tile(current.x+1, current.y);
			if (i == 2) neighbor = v2Tile(current.x, current.y-1);
			if (i == 3) neighbor = v2Tile(current.x, current.y+1);

			if (neighbor.x < 0 || neighbor.x > tilesWide-1) continue;
			if (neighbor.y < 0 || neighbor.y > tilesHigh-1) continue;

			int neighborCollisionIndex = neighbor.y * tilesWide + neighbor.x;

			int cost = 1;
			if (tiles[neighborCollisionIndex] == 1) {
				outGrid[neighborCollisionIndex] = -1;
				continue;
			}

			{ /// Make sure neighbor can fit @incomplete Units bigger than 2x2
				bool top = false;
				bool bot = false;
				bool left = false;
				bool right = false;
				// logf("---\n");
				int iterSize = ceilf((float)(size-1)/2.0);
				for (int y = -iterSize; y <= iterSize; y++) {
					for (int x = -iterSize; x <= iterSize; x++) {
						// logf("%d %d\n", x, y);
						Vec2Tile tile = v2Tile(neighbor.x+x, neighbor.y+y);
						if (tile.x < 0 || tile.x > tilesWide-1) continue;
						if (tile.y < 0 || tile.y > tilesHigh-1) continue;

						int index = tile.y * tilesWide + tile.x;
						if (tiles[index] != 0) {
							if (x < 0) left = true;
							if (x > 0) right = true;
							if (y < 0) top = true;
							if (y > 0) bot = true;
						}
					}
				}

				bool bad = false;
				if (top && bot && !left && !right) bad = true;
				if (!top && !bot && left && right) bad = true;
				if (top && bot && left && right) bad = true;

				if (bad) {
					outGrid[neighborCollisionIndex] = -1;
					continue;
				}
			}

			int newCost;
			bool good = hashMapGet(costSoFar, &current, current.getFloatHashAsInt(), &newCost);
			newCost += cost;

			int neighborCost = -1;
			bool wasVisited = hashMapGet(costSoFar, &neighbor, neighbor.getFloatHashAsInt(), &neighborCost);

			if (!wasVisited || newCost < neighborCost) {
				outGrid[neighborCollisionIndex] = newCost;

				hashMapSet(costSoFar, &neighbor, neighbor.getFloatHashAsInt(), &newCost);
				priorityQueuePush(frontier, &neighbor, newCost);
			}
		}

		// for (int y = 0; y < tilesHigh; y++) {
		// 	for (int x = 0; x < tilesWide; x++) {
		// 		int index = y * tilesWide + x;
		// 		if (outGrid[index] != -1 && tiles[index] == 1) outGrid[index] = -1;
		// 	}
		// }
	}

	// int used = 0;
	// int max = 0;
	// for (int i = 0; i < costSoFar->listsNum; i++) {
	// 	int num = 0;

	// 	HashMapNode *node = costSoFar->lists[i];
	// 	while (node) {
	// 		num++;
	// 		node = node->next;
	// 	}

	// 	if (game->playerNumber == 0 && num != 0) {
	// 		used++;
	// 		if (max < num) max = num;
	// 		logf("Num in slot %d: %d (%d used %d max)\n", i, num, used, max);
	// 	}
	// }

	destroyHashMap(costSoFar);
	destroyPriorityQueue(frontier);
	// Panic("Done");
}

LightCasterRef *getLightCasterRef(int lightId) {
	for (int i = 0; i < game->lightCasterRefsNum; i++) {
		LightCasterRef *casterRef = &game->lightCasterRefs[i];
		if (casterRef->id == lightId) return casterRef;
	}

	return NULL;
}

int createLight(int teamNumber) {
	if (game->lightCasterRefsNum > LIGHT_CASTER_REFS_MAX-1) {
		logf("Too many lights\n");
		return 0;
	}

	LightCasterRef *casterRef = &game->lightCasterRefs[game->lightCasterRefsNum++];
	memset(casterRef, 0, sizeof(LightCasterRef));
	casterRef->id = ++game->lightCasterRefNextId;
	casterRef->teamNumber = teamNumber;

	return casterRef->id;
}

void destroyLight(int lightId) {
	for (int i = 0; i < game->lightCasterRefsNum; i++) {
		LightCasterRef *casterRef = &game->lightCasterRefs[i];
		if (casterRef->id == lightId) {
			if (!casterRef->position.isZero()) {
				modifyVisionRefs(casterRef->teamNumber, casterRef->position, casterRef->radius, -casterRef->value, casterRef->isFlying);
			}

			arraySpliceIndex(game->lightCasterRefs, game->lightCasterRefsNum, sizeof(LightCasterRef), i);
			game->lightCasterRefsNum--;
			return;
		}
	}

	logf("Attempted to destroy an unregistered light\n");
}

void propagateLight(int lightId, Vec2Tile position, int radius, int value) {
	LightCasterRef *casterRef = getLightCasterRef(lightId);
	if (!casterRef) {
		logf("Attempted to add to an unregistered light at %d %d\n", position.x, position.y);
		return;
	}

	bool needsUpdate = false;
	if (!position.equal(casterRef->position)) needsUpdate = true;
	if (radius != casterRef->radius) needsUpdate = true;

	if (needsUpdate) {
		if (!casterRef->position.isZero()) {
			modifyVisionRefs(casterRef->teamNumber, casterRef->position, casterRef->radius, -casterRef->value, casterRef->isFlying);
		}
		casterRef->position = position;
		casterRef->radius = radius;
		casterRef->value = value;
		modifyVisionRefs(casterRef->teamNumber, casterRef->position, casterRef->radius, casterRef->value, casterRef->isFlying);
	}
}

void modifyVisionRefs(int teamNumber, Vec2Tile position, int radius, int value, bool isFlying) {
	if (radius <= 0) return;
	if (position.isZero()) return;
	Map *map = &game->map;

	int minX = position.x - radius;
	int maxX = position.x + radius+1;
	int minY = position.y - radius;
	int maxY = position.y + radius+1;

	if (minX < 0) minX = 0;
	if (maxX > map->width-1) maxX = map->width-1;
	if (minY < 0) minY = 0;
	if (maxY > map->height-1) maxY = map->height-1;

	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			if (!contains(makeCircle(v2(position.x, position.y), radius), v2(x, y))) continue;
			int gridIndex = y * map->width + x;
			if (isFlying || hasLineOfSight(position, v2Tile(x, y), true)) {
				game->visionRefs[teamNumber][gridIndex] += value;
				float distPerc = 1;
				// float distPerc = 1.0 - (distanceBetween(position.x, position.y, x, y) / (float)radius);
				// Clamp01(distPerc);
				game->visionLightingRefs[teamNumber][gridIndex] += distPerc * 100.0 * (float)value;
			}
		}
	}
}

void stepPathfinding(Unit *unit, Vec2 navGoal) {
	Map *map = &game->map;

	bool goalHasChanged = false;
	if (!unit->navGoal.equal(navGoal)) {
		goalHasChanged = true;
		unit->navGoal = navGoal;
		unit->tileNavGoal = getClosestReachableNode(game->collisionData, map->width, map->height, worldToTile(unit->position), worldToTile(navGoal));

		if (unit->tileNavGoal.x == -1 && unit->tileNavGoal.y == -1) {
			logf("Bad pathing to %f %f\n", navGoal.x, navGoal.y);
			unit->navGoal = v2();
			unit->tileNavGoal = v2();
			return;
		}
	}

	Vec2 realGameSpaceTarget = tileToWorld(unit->tileNavGoal);

	Vec2Tile unitTile = worldToVec2Tile(unit->position);
	Vec2Tile targetTile = worldToVec2Tile(realGameSpaceTarget);

	bool hasLos = false;
	if (hasLos || unit->info->flying) hasLos = true;
	if (hasLos || hasLineOfSightThick(game->collisionData, map->width, map->height, unitTile, targetTile, ceilf((float)unit->info->size/(float)tileSize))) hasLos = true;
	if (hasLos) {
		unit->seekTarget = realGameSpaceTarget;
		unit->flowField = NULL;

		if (game->debugShowPathfindingData) drawLine(unit->position, navGoal, 2, 0xFF00FF00);
	} else {
		if (!goalHasChanged && unit->flowField) return; // Probably need to add to this

		int unitSize = ceilf((float)unit->info->size/(float)tileSize);
		for (int i = 0; i < FLOW_FIELDS_MAX; i++) {
			FlowField *field = &game->flowFields[i];
			if (field->targetTile.equal(unit->tileNavGoal) && field->size == unitSize) {
				unit->flowField = field;
				return;
			}
		}

		FlowField *freeField = NULL;

		for (int i = 0; i < FLOW_FIELDS_MAX; i++) {
			FlowField *field = &game->flowFields[i];
			FlowField *freeField = NULL;
			if (field->targetTile.equal(v2(-1, -1))) freeField = field;
		}

		if (!freeField) {
			for (int i = 0; i < FLOW_FIELDS_MAX; i++) {
				FlowField *field = &game->flowFields[i];
				bool used = false;
				for (int i = 0; i < game->unitsNum; i++) {
					Unit *otherUnit = &game->units[i];
					if (otherUnit == unit) continue;
					if (otherUnit->flowField == field) used = true;
				}

				if (!used) {
					freeField = field;
					break;
				}
			}
		}

		if (!freeField) {
			logf("Too many flowfields\n");
			return;
		}

		freeField->targetTile = unit->tileNavGoal;
		freeField->size = unitSize;

		NanoTime startTime;
		NanoTime endTime;

		getNanoTime(&startTime);

		generateDijkstraGrid(game->collisionData,  map->width, map->height, unit->tileNavGoal, unitSize, freeField->dijkstraData);

		getNanoTime(&endTime);
		float ms = getMsPassed(&startTime, &endTime);
		// logf("dGrid %f\n", ms);

		getNanoTime(&startTime);
		generateFlowField(game->collisionData, map->width, map->height, unit->tileNavGoal, freeField->dijkstraData, freeField->data);

		getNanoTime(&endTime);
		ms = getMsPassed(&startTime, &endTime);
		// logf("fGrid %f\n", ms);
		unit->flowField = freeField;
	}
}

void endPathfinding(Unit *unit) {
	unit->navGoal = v2();
	unit->tileNavGoal = v2();
	unit->flowField = NULL;
	unit->seekTarget = v2();
}

void createRightClickAction(Unit *unit, Vec2 position, bool queues) {
	int ATTACK = 0;
	int ATTACK_MOVE = 1;
	int MOVE = 2;
	int GATHER = 3;
	int RALLY = 4;

	int clickType = MOVE;
	Unit *target = NULL;
	if (unit->info->isBuilding) {
		clickType = RALLY;
	} else {
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *otherUnit = &game->units[i];

			if (unit->type == UNIT_PROBE && otherUnit->type == UNIT_MINERALS) {
				if (otherUnit->rect.contains(position)) {
					clickType = GATHER;
					target = otherUnit;
					break;
				}
			}

			if (otherUnit->teamNumber != -1 && otherUnit->teamNumber != unit->teamNumber && canSeeUnit(unit->teamNumber, otherUnit)) {
				if (otherUnit->rect.contains(position)) {
					bool canAttack = true;
					if (otherUnit->info->flying && unit->info->noAirAttack) canAttack = false;
					if (!otherUnit->info->flying && unit->info->noGroundAttack) canAttack = false;
					if (canAttack) {
						clickType = ATTACK;
						target = otherUnit;
					} else {
						clickType = ATTACK_MOVE;
					}
					break;
				}
			}
		}
	}

	if (clickType == MOVE) {
		Action *action = createAction(unit, queues);
		if (action) {
			action->type = ACTION_MOVE;
			action->position = position;
		}
	} else if (clickType == ATTACK) {
		Action *action = createAction(unit, queues);
		if (action) {
			action->type = ACTION_ATTACK;
			action->targetId = target->id;
			action->forceTarget = true;
		}
	} else if (clickType == ATTACK_MOVE) {
		Action *action = createAction(unit, queues);
		if (action) {
			action->type = ACTION_ATTACK_MOVE;
			action->position = position;
		}
	} else if (clickType == GATHER) {
		Action *action = createAction(unit, queues);
		if (action) {
			action->type = ACTION_GATHER;
			action->targetId = target->id;
		}
	} else if (clickType == RALLY) {
		unit->rallyPoint = position;
	} else {
		logf("Invalid click type\n");
	}
}

void processClickSelect(int playerNumber, int unitId, bool appends, bool local) {
	ControlGroup *group = local ? &game->localSelectedUnits : &game->selectedUnits[playerNumber];
	if (!appends) group->idsNum = 0;
	if (unitId == 0) return;
	addToControlGroup(group, unitId);
}

void processSpliceSelection(int playerNumber, int unitIndex, bool local) {
	ControlGroup *group = local ? &game->localSelectedUnits : &game->selectedUnits[playerNumber];
	if (unitIndex >= group->idsNum) return;
	arraySpliceIndex(group->ids, UNITS_MAX, sizeof(int), unitIndex);
	group->idsNum--;
}

Unit *getFirstUnit(ControlGroup *group) {
	if (group->idsNum == 0) return NULL;
	return getUnit(group->ids[0]);
}

Unit *getClosestUnit(ControlGroup *group, Vec2 position, UnitType type, ActionType filterActionType) {
	Unit *closest = NULL;
	float closestDist = 999999;
	for (int i = 0; i < group->idsNum; i++) {
		Unit *unit = getUnit(group->ids[i]);
		if (!unit) continue;
		if (type != UNIT_NONE && unit->type != type) continue;
		if (filterActionType != ACTION_NONE) {
			if (unit->actionsNum > 0 && unit->actions[0].type == filterActionType) continue;
		}

		float dist = unit->position.distance(position);
		if (dist < closestDist) {
			closest = unit;
			closestDist = dist;
		}
	}

	return closest;
}

void processTimeWarpBoxSelect(int playerNumber, Rect box, bool appends, UnitType onlyType, int turnsBack, bool local) {
	ControlGroup *group = local ? &game->localSelectedUnits : &game->selectedUnits[playerNumber];
	if (!appends) group->idsNum = 0;

	Game::PastTurnData *pastTurnData = &game->pastTurnData[turnsBack];

	Unit *firstBuilding = NULL;
	Unit *secondBuilding = NULL;
	for (int i = 0; i < pastTurnData->unitDatasNum; i++) {
		Game::UnitPastData *unitPastData = &pastTurnData->unitDatas[i];
		Unit *unit = getUnit(unitPastData->id);
		if (!unit) continue;
		if (unit->playerNumber != playerNumber) continue;

		Rect unitRect;
		unitRect.width = unit->info->size;
		unitRect.height = unit->info->size;
		unitRect.x = unitPastData->position.x - unitRect.width/2;
		unitRect.y = unitPastData->position.y - unitRect.height/2;

		bool isInBox = false;
		if (box.intersects(unitRect)) isInBox = true;

		if (onlyType != UNIT_NONE) {
			if (unit->type != onlyType) continue;
		} else {
			if (unit->info->isBuilding && !unit->info->alwaysAbleToBoxSelect) {
				if (isInBox) {
					if (!firstBuilding) firstBuilding = unit;
					else if (!secondBuilding) secondBuilding = unit;
				}
				continue;
			}
		}

		if (isInBox) {
			if (local) createEffect(EFFECT_UNIT_SELECTED, unit->id);
			addToControlGroup(group, unit->id);
		}
	}

	if (group->idsNum == 0 && firstBuilding && !secondBuilding) addToControlGroup(group, firstBuilding->id);
}

void doPassiveAggro(Unit *unit) {
	Unit *closestAttackingUnit = NULL;
	Unit *closestNonWorkerUnit = NULL;
	Unit *closestNonBuildingUnit = NULL;
	Unit *closestUnit = NULL;
	Unit *closestFriendlyAttackingUnit = NULL;

	int unitsInRangeNum = 0;
	Unit **unitsInRange = getUnitsInCircle(unit->position, unit->info->aggroRadius*tileSize, &unitsInRangeNum);

	for (int i = 0; i < unitsInRangeNum; i++) {
		Unit *otherUnit = unitsInRange[i];
		if (otherUnit->teamNumber == -1) continue;
		if (!canSeeUnit(unit->teamNumber, otherUnit)) continue;

		if (otherUnit->info->flying && unit->info->noAirAttack) continue;
		if (!otherUnit->info->flying && unit->info->noGroundAttack) continue;

		bool inAggroRange = true;

		if (inAggroRange) {
			if (otherUnit->teamNumber == unit->teamNumber) {
				if (otherUnit->actionsNum > 0) {
					Action *action = &otherUnit->actions[0];
					if (action->type == ACTION_ATTACK) {
						if (!closestFriendlyAttackingUnit) { 
							closestFriendlyAttackingUnit = otherUnit;
						} else {
							if (otherUnit->position.distance(unit->position) < closestFriendlyAttackingUnit->position.distance(unit->position)) {
								closestFriendlyAttackingUnit = otherUnit;
							}
						}
					}
				}
				continue;
			}

			if (otherUnit->actionsNum > 0) {
				Action *action = &otherUnit->actions[0];
				if (action->type == ACTION_ATTACK || action->type == ACTION_ATTACK_MOVE) {
					if (!closestAttackingUnit) { 
						closestAttackingUnit = otherUnit;
					} else {
						if (otherUnit->position.distance(unit->position) < closestAttackingUnit->position.distance(unit->position)) closestAttackingUnit = otherUnit;
					}
				}
			}

			if (otherUnit->type != UNIT_PROBE && !otherUnit->info->isBuilding) {
				if (!closestNonWorkerUnit) { 
					closestNonWorkerUnit = otherUnit;
				} else {
					if (otherUnit->position.distance(unit->position) < closestNonWorkerUnit->position.distance(unit->position)) closestNonWorkerUnit = otherUnit;
				}
			}

			if (!otherUnit->info->isBuilding) {
				if (!closestNonBuildingUnit) { 
					closestNonBuildingUnit = otherUnit;
				} else {
					if (otherUnit->position.distance(unit->position) < closestNonBuildingUnit->position.distance(unit->position)) closestNonBuildingUnit = otherUnit;
				}
			}

			if (!closestUnit) { 
				closestUnit = otherUnit;
			} else {
				if (otherUnit->position.distance(unit->position) < closestUnit->position.distance(unit->position)) closestUnit = otherUnit;
			}
		}
	}

	// if (unit->type == UNIT_SPIDER_MINE) logf("Did\n");
	Unit *targetUnit = NULL;
	if (closestAttackingUnit) {
		targetUnit = closestAttackingUnit;
	} else if (closestNonWorkerUnit) {
		targetUnit = closestNonWorkerUnit;
	} else if (closestNonBuildingUnit) {
		targetUnit = closestNonBuildingUnit;
	} else if (closestUnit) {
		targetUnit = closestUnit;
	} else if (closestFriendlyAttackingUnit) {
		Action *action = &closestFriendlyAttackingUnit->actions[0];
		targetUnit = getUnit(action->targetId);
	}

	if (targetUnit) {
		if (unit->actionsNum > 0 && unit->actions[0].type == ACTION_ATTACK) {
			Action *action = &unit->actions[0];
			action->targetId = targetUnit->id;
		} else {
			Action *action = NULL;
			if (unit->actionsNum > 0 && unit->actions[0].type == ACTION_ATTACK_MOVE) {
				memmove(&unit->actions[1], &unit->actions[0], unit->actionsNum * sizeof(Action));
				action = &unit->actions[0];
				memset(action, 0, sizeof(Action));
				unit->firstFrameOfAction = true;
				unit->actionsNum++;
			} else {
				action = createAction(unit, false);
			}
			if (action) {
				action->type = ACTION_ATTACK;
				action->targetId = targetUnit->id;
			}
		}
	}
}

void removeFromControlGroup(ControlGroup *group, int unitId) {
	for (int i = 0; i < group->idsNum; i++) {
		if (group->ids[i] == unitId) {
			arraySpliceIndex(group->ids, UNITS_MAX, sizeof(int), i);
			group->idsNum--;
			i--;
			continue;
		}
	}
}

void removeFromAllControlGroups(int unitId, bool local) {
	for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
		int playerNumber = i;
		for (int controlGroupIndex = 0; controlGroupIndex < CONTROL_GROUPS_MAX; controlGroupIndex++) {
			ControlGroup *group = local ? &game->localControlGroups[controlGroupIndex] : &game->controlGroups[playerNumber][controlGroupIndex];
			removeFromControlGroup(group, unitId);
		}
	}
}

void addToControlGroup(ControlGroup *group, int id) {
	for (int i = 0; i < group->idsNum; i++) {
		if (group->ids[i] == id) return;
	}

	group->ids[group->idsNum++] = id;
}

void replaceControlGroup(ControlGroup *dest, ControlGroup *src) {
	memcpy(dest, src, sizeof(ControlGroup));
}

Rect getUnitRect(UnitType type, Vec2 position) {
	UnitInfo *info = &game->unitInfos[type];

	Rect rect;
	rect.width = info->size;
	rect.height = info->size;
	rect.x = position.x - rect.width/2;
	rect.y = position.y - rect.height/2;
	return rect;
}

void drawGhostImage(UnitType unitType, Vec2 position, float angleDeg) {
	UnitInfo *info = &game->unitInfos[unitType];
	Rect rect = makeCenteredSquare(position, info->size);
	drawRect(rect, lerpColor(0x80FF0000, 0x00FF0000, timePhase(game->time)));
}

Vec2 tileToWorld(Vec2 tile) {
	Vec2 point;
	point.x = tile.x * tileSize + tileSize/2;
	point.y = tile.y * tileSize + tileSize/2;
	return point;
}

Vec2 tileToWorld(Vec2Tile tile) {
	Vec2 point;
	point.x = (int)tile.x * tileSize + tileSize/2;
	point.y = (int)tile.y * tileSize + tileSize/2;
	return point;
}

Rect tileToWorldRect(Vec2 tile) {
	Rect rect = makeRect(0, 0, tileSize, tileSize);
	rect.x = tile.x * tileSize;
	rect.y = tile.y * tileSize;
	return rect;
}

Rect tileToWorldRect(Vec2Tile tile) {
	Rect rect = makeRect(0, 0, tileSize, tileSize);
	rect.x = tile.x * tileSize;
	rect.y = tile.y * tileSize;
	return rect;
}

Vec2 worldToTile(Vec2 point) {
	Vec2 tile;
	tile.x = (int)(point.x / tileSize);
	tile.y = (int)(point.y / tileSize);
	if (tile.x < 0) tile.x = 0;
	if (tile.y < 0) tile.y = 0;
	if (tile.x > game->map.width-1) tile.x = game->map.width-1;
	if (tile.y > game->map.height-1) tile.y = game->map.height-1;
	return tile;
}

Vec2Tile worldToVec2Tile(Vec2 point) {
	Vec2Tile tile;
	tile.x = (u8)(point.x / tileSize);
	tile.y = (u8)(point.y / tileSize);
	return tile;
}

int getTilesUnderUnitType(UnitType type, Rect rect, Vec2Tile *tiles, int tilesMax) {
	int tilesNum = 0;

	UnitInfo *info = &game->unitInfos[type];
	int unitSizeInTiles = info->size/tileSize;

	// if ((int)(unit->info->size/tileSize) == 1) {
	// 	tiles[tilesNum++] = worldToTile(unit->position);
	// 	return;
	// }

	Vec2 topLeftTile;
	topLeftTile.x = (int)(rect.x/tileSize);
	topLeftTile.y = (int)(rect.y/tileSize);
	for (int y = 0; y < (int)(info->size/tileSize); y++) {
		for (int x = 0; x < (int)(info->size/tileSize); x++) {
			Vec2Tile unitTile;
			unitTile.x = topLeftTile.x + x;
			unitTile.y = topLeftTile.y + y;

			if (tilesNum < tilesMax) {
				tiles[tilesNum++] = unitTile;
			}
		}
	}

	return tilesNum;
}

int getTilesUnderRect(Rect rect, Vec2Tile *tiles, int tilesMax) {
	Vec2Tile tl = worldToVec2Tile(v2(rect.x, rect.y));
	Vec2Tile br = worldToVec2Tile(v2(rect.x+rect.width, rect.y+rect.height));

	int tilesNum = 0;
	for (int y = (int)tl.y; y < (int)br.y; y++) {
		for (int x = (int)tl.x; x < (int)br.x; x++) {
			if (tilesNum > tilesMax-1) {
				logf("Too many tiles under rect\n");
				return tilesNum;
			}

			tiles[tilesNum++] = v2Tile(x, y);
		}
	}

	if (tilesNum == 0) tiles[tilesNum++] = worldToVec2Tile(rect.center());

	return tilesNum;
}

int getOutlineTilesUnderRect(Rect rect, Vec2Tile *tiles, int tilesMax) {
	Vec2Tile *rawOutlineTiles = (Vec2Tile *)frameMalloc(tilesMax * sizeof(Vec2Tile));
	int rawOutlineTilesNum = getTilesUnderRect(rect, rawOutlineTiles, tilesMax);

	Vec2Tile min = {};
	Vec2Tile max = {};
	for (int i = 0; i < rawOutlineTilesNum; i++) {
		Vec2Tile tile = rawOutlineTiles[i];
		if (tile.x < min.x) min.x = tile.x;
		if (tile.x > max.x) max.x = tile.x;
		if (tile.y < min.y) min.y = tile.y;
		if (tile.y > max.y) max.y = tile.y;
	}

	int tilesNum = 0;
	for (int i = 0; i < rawOutlineTilesNum; i++) {
		Vec2Tile tile = rawOutlineTiles[i];
		if (
			tile.x == min.x ||
			tile.x == max.x ||
			tile.y == min.y ||
			tile.y == max.y
		) {
			tiles[tilesNum++] = tile;
		}
	}

	return tilesNum;
}

int getTilesUnderCircle(Vec2 position, float radius, Vec2Tile *tiles, int tilesMax) {
	Vec2Tile tl = worldToVec2Tile(position - v2(radius, radius));
	Vec2Tile br = worldToVec2Tile(position + v2(radius, radius));
	Vec2Tile centerTile = worldToVec2Tile(position);

	int tilesNum = 0;
	for (int y = (int)tl.y; y < (int)br.y; y++) {
		for (int x = (int)tl.x; x < (int)br.x; x++) {
			if (tilesNum > tilesMax-1) {
				logf("Too many tiles under rect\n");
				return tilesNum;
			}

			Vec2Tile tile = v2Tile(x, y);

			if (centerTile.distance(tile) > radius/tileSize) continue;
			tiles[tilesNum++] = tile;
		}
	}

	if (tilesNum == 0) tiles[tilesNum++] = centerTile;

	return tilesNum;
}

void recountPopulation() {
	for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) game->populationCounts[i] = 0;
	game->negOnePopulationCount = 0;

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->playerNumber == -1) {
			game->negOnePopulationCount += unit->info->population;
		} else {
			game->populationCounts[unit->playerNumber] += unit->info->population;
		}

		for (int i = 0; i < unit->actionsNum; i++) {
			if (unit->playerNumber == -1) continue;
			Action *action = &unit->actions[i];
			game->populationCounts[unit->playerNumber] += action->populationPrice;
		}
	}
}

int canSeeUnit(int teamNumber, Unit *unit) {
	if (unit->teamNumber == -1) return true;

	Map *map = &game->map;

	Vec2Tile tiles[TILES_UNDER_UNIT_MAX];

	int tilesNum = getTilesUnderRect(unit->rect, tiles, TILES_UNDER_UNIT_MAX);
	for (int i = 0; i < tilesNum; i++) {
		Vec2Tile tile = tiles[i];

		int tileIndex = tile.y * map->width + tile.x;
		if (canSeeTile(teamNumber, tileIndex)) {
			if (unit->info->stealth && unit->teamNumber != teamNumber) {
				for (int i = 0; i < game->unitsNum; i++) {
					Unit *otherUnit = &game->units[i];
					if (otherUnit->givingTrueVision && teamNumber == otherUnit->teamNumber) {
						float dist = unit->position.distance(otherUnit->position);
						LightCasterRef *casterRef = getLightCasterRef(otherUnit->lightId);
						if (casterRef) {
							if (dist < casterRef->radius*tileSize) return true;
						}
					}
				}

				return false;
			} else {
				return true;
			}
		}
	}

	return false;
}

int canSeeRect(int teamNumber, Rect rect) {
	Map *map = &game->map;

	Vec2Tile tiles[TILES_UNDER_UNIT_MAX];

	int tilesNum = getTilesUnderRect(rect, tiles, TILES_UNDER_UNIT_MAX);
	for (int i = 0; i < tilesNum; i++) {
		Vec2Tile tile = tiles[i];

		int tileIndex = tile.y * map->width + tile.x;
		if (canSeeTile(teamNumber, tileIndex)) return true;
	}

	return false;
}

int canSeeTile(int teamNumber, Vec2Tile tile) {
	Map *map = &game->map;
	return canSeeTile(teamNumber, tile.y * map->width + tile.x);
}

int canSeeTile(int teamNumber, u8 x, u8 y) {
	return canSeeTile(teamNumber, v2Tile(x, y));
}

int canSeeTile(int teamNumber, int tileIndex) {
	Map *map = &game->map;
	return game->visionRefs[teamNumber][tileIndex];
}

Effect *createEffect(EffectType type, Vec2 position) {
	if (game->effectsNum > EFFECTS_MAX-1) return NULL;

	Effect *effect = &game->effects[game->effectsNum++];
	memset(effect, 0, sizeof(Effect));

	effect->type = type;
	effect->position = position;
	return effect;
}

Effect *createEffect(EffectType type, int unitId) {
	Effect *effect = createEffect(type, v2());
	if (effect) {
		effect->unitId = unitId;
	}

	return effect;
}

int playGameSound(const char *name) {
	Sound *sound = getSound(name);
	if (sound) {
		Channel *channel = playSound(sound);
		return channel->id;
	}

	return 0;
}

int playGameMusic(const char *name) {
	Sound *sound = getSound(name);
	if (sound) {
		Channel *channel = playSound(sound, true);
		return channel->id;
	}

	return 0;
}

int playWorldSound(const char *name, Vec2 position) {
	Sound *sound = getSound(name);
	if (!sound) return 0;

	int count = 0;
	for (int i = 0; i < game->worldChannelsNum; i++) {
		Channel *channel = getChannel(game->worldChannels[i]);
		if (!channel) continue;
		if (streq(channel->sound->path, sound->path)) count++;
	}

	if (count > 10) { 
		return 0;
	}

	Channel *channel = playSound(sound);
	if (!channel) {
		logf("Failed to play sound\n");
		return 0;
	}
	channel->position = position;

	if (count > 2) { 
		channel->delay += rndFloat(0, 0.1);
		// channel->dupInterval = rndInt(3, 8);
	}

	if (game->worldChannelsNum < WORLD_CHANNELS_MAX-1) {
		game->worldChannels[game->worldChannelsNum++] = channel->id;
	} else {
		logf("Not enough world channels\n");
	}
	return channel->id;
}

void stepPingChart(PingChartType type) {
	PingChart *chart = &game->pingCharts[type];
	// memmove(&chart->time[0], chart->time
	NanoTime nanoTime = getNanoTime();
	float ms = getMsPassed(&chart->lastNanoTime, &nanoTime) / 1000.0;
	if (ms > 30 * 1000) ms = 0;
	chart->times[chart->nextTime++] = ms;
	chart->lastNanoTime = nanoTime;
	if (chart->nextTime > PING_CHART_TIMES_MAX-1) chart->nextTime = 0;
}

MsgPacket createMsgPacket(NetMsgType msgType) {
	MsgPacket packet = {};
	packet.msg.clientId = game->clientId;
	packet.msg.type = msgType;
	return packet;
}

void updateLoginThread(void *threadParam) {
	for (;;) {
		if (game->_shouldBeLoggedIn) break;
		platformSleep(16);
	}

	if (game->_supportsUdp) {
		game->_udpLoginSocket = createUdpSocket(sizeof(NetMsg));
		if (!game->_udpLoginSocket) {
			logf("Failed to create udp socket\n");
			logLastOSError();
			// Panic("Can't continue without udp socket");
		}

		bool good;
		good = socketSetNonblock(game->_udpLoginSocket);
		if (!good) {
			logf("Failed to make udp socket nonblocking\n");
			logLastOSError();
			Panic("Can't continue without nonblocking socket");
		}

		good = socketBind(game->_udpLoginSocket, createSockaddr(0, 0));
		if (!good) {
			logf("Failed to make bind udp socket\n");
			logLastOSError();
		}

		logf("Client listenening: ");
		printSocketInfo(game->_udpLoginSocket);
		logf("\n");
	} else {
		if (game->_tcpLoginSocket) socketClose(game->_tcpLoginSocket);
		game->_tcpLoginSocket = createTcpSocket(sizeof(NetMsg));

		if (!game->_tcpLoginSocket) {
			logf("Failed to create tcp socket for login\n");
			logLastOSError();
			Panic("Can't continue without tcp socket");
		}

		logf("Connecting to %s:%d\n", game->serverIp, game->serverPort+2);
		if (!socketConnect(game->_tcpLoginSocket, createSockaddr(game->serverIp, game->serverPort+2))) {
			logf("Failed to connect tcp socket for login\n");
			logLastOSError();
			Panic("Can't continue without connected tcp socket");
		}

		if (!socketSetNonblock(game->_tcpLoginSocket)) {
			logf("Failed to make tcp socket nonblocking for login\n");
			logLastOSError();
			Panic("Can't continue without nonblocking socket for login");
		}

		logf("Client connected to %s:%d\n", getSocketIp(game->_tcpLoginSocket), getSocketPort(game->_tcpLoginSocket));
	}

	logf("Is listening\n");

	Socket *loginSocket = game->_tcpLoginSocket;
	if (game->_supportsUdp) loginSocket = game->_udpLoginSocket;

	for (;;) {
		for (;;) {
			SocketlingStatus status = socketReceive(loginSocket);
			if (status == SOCKETLING_ZERO_RECEIVE) {
				logf("The login server was disconnected\n");
				// exit(0);
			} else if (status == SOCKETLING_CONNECTION_RESET) {
				logf("The login server was destroyed\n");
				// exit(0);
			} else if (status == SOCKETLING_WOULD_BLOCK) {
				// Nothing...
			} else if (status == SOCKETLING_PENDING) {
				// Nothing...
			} else if (status == SOCKETLING_ERROR) {
				// if (lastSocketErrorWasConnReset()) {
				// 	logf("Server was ECONNRESET\n");
				// 	exit(0);
				// }
				logLastOSError();
			} else if (status == SOCKETLING_GOOD) {
				// logf("Good\n");
				if (game->_debugPacketDropRateIn != 0 && rndPerc(game->_debugPacketDropRateIn)) continue;

				stepPingChart(loginSocket == game->_udpLoginSocket ? PING_CHART_LOGIN_UDP_RECV : PING_CHART_LOGIN_TCP_RECV);

				MsgPacket packet = {};
				memcpy(&packet.msg, loginSocket->receiveBuffer, sizeof(NetMsg));
				if (packet.msg.type != NET_MSG_StC_TURN_DATA) {
					bool good = threadSafeQueuePush(game->_loginPacketsIn, &packet);
					if (!good) logf("incoming queue is full\n");
				}
			} else {
				logf("Unknown socketling status %d\n", status);
			}

			if (status != SOCKETLING_GOOD) break;
		}

		for (;;) {
			MsgPacket packet;
			bool good = threadSafeQueueShift(game->_loginPacketsOut, &packet);
			if (!good) break;
			NetMsg msg = packet.msg;
			// if (msg.type == NET_MSG_CtS_TURN_DATA) logf("Player %d sending %d game commands\n", game->playerNumber, msg.turnData.gameCmdsNum);

			if (game->_debugPacketDropRate != 0) {
				if (rndPerc(game->_debugPacketDropRate)) {
					logf("Packet dropped\n");
					continue;
				}
			}

			if (game->_debugSenderLag != 0) platformSleep(game->_debugSenderLag);
			SocketlingStatus status = socketSend(loginSocket, &msg, game->_loginServerAddr);
			if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
				if (!threadSafeQueuePush(game->_loginPacketsOut, &packet)) {
					logf("_loginPacketsOut is full upon repush\n");
				}
			} else if (status == SOCKETLING_ERROR) {
				logLastOSError();
			} else if (status == SOCKETLING_GOOD) {
				// logf("Client sending %d\n", msg.type);
				stepPingChart(loginSocket == game->_udpLoginSocket ? PING_CHART_LOGIN_UDP_SEND : PING_CHART_LOGIN_TCP_SEND);
				game->_timeSinceLastContactedLoginServer = 0;
			} else {
				logf("Unknown socketling status %d\n", status);
			}
		}

		platformSleep(1);
	}
}

void gameCleanup() {
	deinitNetworking();
}
