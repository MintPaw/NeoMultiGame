/*
*A - Strafe left
B - Bubble
C - Claw
*D - Strafe right
E - Execute
F
G
H - Shield
*I - Info
J
K
L - Land
M - Map (handheld radar)
N - Night vision
O
P - Proxy mine
Q - Quick switch
*R - Reload
*S - Backwards
T
U
V - Melee/Sword
*W - Forward
X
Y
Z - Dash/Phase
*/
// Game last saved X min ago


// pressurePlacePress/release https://beepbox.co/#8n31s0k0l00e00t2mm0a7g0fj07i0r1o3210T1v1L4ub1q1d2f6y3z1C0c0A0F1B7V9Q0000Pe800E0111T1v1L4ub5q1d2f7y1z2C0c0A5F4B6V9Q0001PecaaE0099T1v1L4u92q1d4f7y2z1C0c2AbF6B6V9Q0490Pb976E0001T3v1L4uf5q1d5f7y1z6C1S1jsSIzsSrIAASJJb4h400000000h4g000000014h000000004h400000000p1d0000FB0F0YX00
// chant1 https://www.beepbox.co/#8n31s0k0l00e00t2mm0a7g0fj07i0r1o3210T1v1L4uecq1d7fay0z1C3c0AcF8B7VaQ0001PffffE0000T0v1L4u10q0d0f8y0z1C2w2c0h0T1v1L4uc1q1d2f6y3z1C0c0A1F0B4V2Q0050Pd66cE0191T4v1L4u04q1z6666ji8k8k3jSBKSJJAArriiiiii07JCABrzrrrrrrr00YrkqHrsrrrrjr005zrAqzrjzrrqr1jRjrqGGrrzsrsA099ijrABJJJIAzrrtirqrqjqixzsrAjrqjiqaqqysttAJqjikikrizrHtBJJAzArzrIsRCITKSS099ijrAJS____Qg99habbCAYrDzh00b4h400000000h4g000000014h000000004h400000000p1sKrPLzl2c0005cFVEQ6zgqcwkQ580
// aweful alarm https://www.beepbox.co/#8n31s0k0l00e03t40m0a7g0fj07i0r1o3210T1v1L4uadq3d1f7y1z2C0c0A1F0B2V1Q31bdPb525E011bT0v1L4u11q0d0f8y0z1C2w1c0h0T1v1L4u37q1d1f6y1z6C1c0AbF6B0V1Q07beP9996E0006T4v1L4uf0q1z6666ji8k8k3jSBKSJJAArriiiiii07JCABrzrrrrrrr00YrkqHrsrrrrjr005zrAqzrjzrrqr1jRjrqGGrrzsrsA099ijrABJJJIAzrrtirqrqjqixzsrAjrqjiqaqqysttAJqjikikrizrHtBJJAzArzrIsRCITKSS099ijrAJS____Qg99habbCAYrDzh00b4h400000000h4g000000014h000000004h400000000p1VFH-y-wLEbW2Y1bQOV6qd1EGpv7wOoKgkw02Czf8W2ewzE8W2ewzE8W2c0

float bloomScale = 0.25;
#define PLAYER_HEIGHT 2
#define LEG_HEIGHT_PERC 0.3
#define PLAYER_VIEW_HEIGHT 2

enum ActorType;
enum WeaponType;

enum EntityType {
	ENTITY_NONE,
	ENTITY_DEBUG_SPHERE,
	ENTITY_CAMERA_POSITION,
	ENTITY_MODEL,
	ENTITY_MARKER,
	ENTITY_BOUNDS,
	ENTITY_PORTAL_SPAWNER,
	ENTITY_LASER_WALL,
	ENTITY_SOUND_EMITTER,
};

const char *entityTypeStrings[] = {
	"None",
	"Debug sphere",
	"Camera position",
	"Model",
	"Marker",
	"Bounds",
	"Actor portal spawner",
	"Laser wall",
	"Sound emitter",
};

struct Entity {
	int id;
#define ENTITY_NAME_MAX_LEN 64
	char name[ENTITY_NAME_MAX_LEN];
	char path[PATH_MAX_LEN];
	EntityType type;
	Vec3 position;
	Vec3 scale;
	Vec3 rotation;
	int tint;
	float alpha;
	ActorType actorType;
	bool isCollider;
	bool hiddenInGame;
	bool hiddenInEditor;
	bool alwaysPowered;
	int rail;
	int railToPower;
	int targetEntity;
	bool needsPower;

	/// Spawner
	int spawnCountMax;
	float spawnInterval;
	WeaponType weaponType;

	// Unserialized
	Matrix4 matrix;
	Matrix4 invMatrix;
	AABB bounds;
	bool powered;
};

enum ActorType {
	ACTOR_NONE,
	ACTOR_PLAYER,
	ACTOR_ZOMBIE,
	ACTOR_PORTAL,
	ACTOR_GOAL,
	ACTOR_START,
	ACTOR_AMMO,
	ACTOR_PRESSURE_PLATE,
	ACTOR_SHOOTER,
	ACTOR_WEAPON,
	ACTOR_SLASHER,
	ACTOR_TELEPORTER,
	ACTOR_END,
};

#define ACTOR_TYPES_MAX ACTOR_END

const char *actorTypeStrings[] = {
	"None",
	"Player",
	"Zombie",
	"Portal",
	"Goal",
	"Start",
	"Ammo",
	"Pressure plate",
	"Shooter",
	"Weapon",
	"Slasher",
	"Teleporter",
};

enum ActorState {
	ASTATE_NONE,
	ASTATE_WALKING,
	ASTATE_ATTACKING,
	ASTATE_ATTACKING2,
	ASTATE_DASHING,
	ASTATE_DYING,
	ASTATE_FLINCHING,
	ASTATE_CROUCHING,
};

enum WeaponType {
	WEAPON_NONE,
	WEAPON_GLOCK,
	WEAPON_UZI,
	WEAPON_SHOTGUN,
	WEAPON_END,
};
#define WEAPONS_MAX WEAPON_END

char *weaponTypeString[] = {
	"none",
	"glock",
	"uzi",
	"shotgun",
};

char *weaponTypeStringCaped[] = {
	"None",
	"Glock",
	"Uzi",
	"Shotgun",
};

struct AnimQueueItem {
	bool exists;
	int index;
	float timeScale;
	float newLength;
	bool loops;
	bool skippable;

	bool reloadsAfter;
	bool toggleInfoToolAfter;
	bool reloadChain;
	bool noneStateAfter;
	ActorState stateAfter;
};

struct Actor {
	int id;
	ActorType type;
	int sourceEntityId;
	int sourcePortalId;
#define ACTOR_NAME_MAX_LEN 64
	char name[ACTOR_NAME_MAX_LEN];
	Skeleton *skeleton;
	char meshName[PATH_MAX_LEN];

	Vec3 position;
	Vec3 scale;
	Vec3 rotation;
	Matrix4 matrix;
	int tint;

	ActorState state;
	ActorState prevState;
	float stateTime;

	AABB bounds;
	AABB skeletonBounds;

	Vec3 velo;
	Vec3 accel;

	bool hasCollision;

	bool isOnGround;
	float offGroundTime;

	WeaponType weaponType;
	WeaponType primaryWeapon;
	WeaponType secondaryWeapon;
	bool lastWeaponWasSecondary;
	float shotTime;

	int value; // ammo count

	ActorType typeToSpawn;
	int spawnCount;
	int spawnCountMax;
	float timeSinceLastSpawn;
	float spawnInterval;
	Vec3 spawnPosition;
	Vec3 spawnRotation;
	Vec3 spawnScale;

	float hp;
	float timeSinceLastHit;
	int bullets[WEAPONS_MAX];
	int reserveBullets[WEAPONS_MAX];

	bool powered;
	bool shouldBeDestoryed;
	float slow;
	bool repulses;
	bool hostile;

	bool hitting;

	MeshTri *navNextMeshTri;
	int navNextStepTick;
	Vec3 navNextTarget;

#define ANIM_INDEX_QUEUE_MAX 32
	AnimQueueItem animQueue[ANIM_INDEX_QUEUE_MAX];
	int animQueueNum;
	bool forceSkip;
	AnimQueueItem currentAnimQueueItem;
	bool currentlyPlaying;

	float phaseBootsTime;

	bool tryingToCancel;
	Vec3 dashTargetPoint;
	float timeSinceLastDashHit;

	Vec3 visualAutoAimRadsOff;
	float lastIn;
	float groundMoveDistance;
	bool forceWeaponSwitch;
};

struct ActorInfo {
	bool usesOverrideBounds;
	Vec3 overrideBoundsOffset;
	Vec3 overrideBoundsSize;

	/// Unserialized
	char *name;
};

struct NavEdge {
	Vec3 start;
	Vec3 end;
};

struct Scene {
#define SCENE_NAME_MAX_LEN 128
	char name[SCENE_NAME_MAX_LEN];
	int nextEntityId;
	int entitiesNum;
};

struct Globals {
	float resolutionPerc;
	float sensitivity;

	/// Unserialized
	bool godMode;
	bool superSpeed;
	bool showNavMeshTris;
	bool showActorBounds;
	bool showActorSkeletonBounds;

	bool showHitCapsules;
	bool showHitCapsulesOnTop;
};

enum EffectType {
	EFFECT_WORLD_TEXT,
};

struct Effect {
	EffectType type;
	Vec3 position;
	char *valueString;
	float time;
};

struct Bullet {
	bool ally;
	Vec3 position;
	Vec3 direction;
	float speed;
	float time;
	int soundSourceId;
};

struct WeaponInfo {
	WeaponType type;
	char *name;
	char *cappedName;
	int maxBullets;
	float reloadTime;
	float shotInterval;
	bool fullAuto;
	int damage;
	int extraShots;
	float spread;
	bool needsPump;
	Vec2 aimerSize;
	float aimerSpeed;
};

struct Game {
	Font *defaultFont;
	Font *bigFont;
	Texture *gameTexture;
	Texture *gameBloomTexture;
	Texture *gameSmallBloomTexture;
	float gameWidth;
	float gameHeight;

	bool usingEditorCamera;
	Vec3 realCameraPosition;
	Vec3 realCameraRotation;
	float realCameraSpeed;
	Matrix4 cameraMatrix;
	Skybox *skybox;

	Globals globals;

	Scene scene;
#define ENTITIES_MAX 128
	Entity entities[ENTITIES_MAX];
#define ACTORS_MAX 512
	Actor actors[ACTORS_MAX];
	int nextActorId;
	int actorsNum;
	int poweredBoundsEntityId;
#define RAILS_MAX 1024
	int poweredRails[RAILS_MAX];
	int poweredRailsNum;

	ActorInfo actorInfos[ACTOR_TYPES_MAX];

	int sceneFrames;
	float timeScale;
	float time;

	int currentNavMeshEntityId;
	MeshTri *lastKnownPlayerMeshTri;
	Skeleton *gunSkeleton;

	bool usingInfoTool;

#define EFFECTS_MAX 128
	Effect effects[EFFECTS_MAX];
	int effectsNum;

#define BULLETS_MAX 1024
	Bullet bullets[BULLETS_MAX];
	int bulletsNum;
	WeaponInfo weaponInfos[WEAPONS_MAX];

	Rect aimerRect;
	Vec2 aimerCursor;
	Vec3 aimerTarget;

	/// Player vars
	Actor *playerStandingOn;

	/// Editor
	bool inEditor;
	bool inNavMode;
	int selectedEntityId;
	Vec3 editorCursorPosition;
	Vec3 editorCursorRotation;
	bool editorCursorEntityRayTesting;
	bool editorRayCursorMoving;
	bool editorRaySelecting;

	/// Debug
	bool showingFrameTimes;
};
Game *game = NULL;

void runGame();
void updateGame();
Entity *getEntity(int id);
Actor *getActor(int id);
Entity *getEntityByName(const char *name);
Vec3 getRayFromSceenSpace(Vec2 screenPosition, Vec3 cameraOffset=v3());
bool rayTestEntity(Entity *entity, Vec3 start, Vec3 ray, float *dist, MeshTri **exactTri=NULL);
bool AABBIntersectsEntity(AABB bounds, Entity *entity);
bool rayTestActorsAndEntities(Vec3 start, Vec3 dir, Actor **outActor, Entity **outEntity, float *outDist, Actor *actorToIgnore, bool onlyColliders, bool ignoreDead);
Vec2 worldSpaceToScreenSpace(Vec3 point);
Rect worldSpaceAABBToScreenSpaceRect(AABB bounds);
Actor *createActor(ActorType type);
void dealDamage(Actor *dest, float amount, Actor *src=NULL, Vec3 hitPoint=v3());
Effect *createWorldText(Vec3 position, char *text);
bool testLineOfSight(Vec3 start, Vec3 end);
void updateNav(Actor *actor, MeshTri *destTri, Vec3 destPoint);
void shootRayBullet(Vec3 start, Vec3 dir, Actor *actor);
Bullet *shootProjectileBullet(Vec3 start, Vec3 dir, Actor *actor);
AnimQueueItem *createAnimQueueItem(Actor *actor, char *animName, float length, bool loops);
void clearAnimQueue(Actor *actor);
void updateActorAnimation(Actor *actor);

void log3f(Vec3 point, const char *msg, ...);
void drawGlowingBeam(Vec3 start, Vec3 end, float thickness=0.1, int color=0xFFFFFFFF);

/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameA(hModule, exeDir, PATH_MAX_LEN);

	char *lastSlash = strrchr(exeDir, '\\');
	if (!lastSlash) Panic("No last slash found in exe path");
	*lastSlash = 0;

#if defined(FALLOW_INTERNAL) // This needs to be a macro
		// if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/sinGame");
#else
		snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#endif

#endif

	initFileOperations();

	initPlatform(1280, 720, "Quike");
	// initPlatform(1280, 720, "No title spoiler >:3");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	initMesh();
	initSkeleton();
	initFonts();
	initTextureSystem();

	bool good = initNetworking();
	if (!good) Panic("Failed to init networking");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	WorldProps *world = defaultWorld;

	if (!game) {
		RegMem(Scene, name);
		RegMem(Scene, nextEntityId);
		RegMem(Scene, entitiesNum);

		RegMem(Entity, id);
		RegMem(Entity, name);
		RegMem(Entity, path);
		RegMem(Entity, type);
		RegMem(Entity, position);
		RegMem(Entity, scale);
		RegMem(Entity, rotation);
		RegMem(Entity, tint);
		RegMem(Entity, alpha);
		RegMem(Entity, actorType);
		RegMem(Entity, isCollider);
		RegMem(Entity, hiddenInGame);
		RegMem(Entity, hiddenInEditor);
		RegMem(Entity, alwaysPowered);
		RegMem(Entity, rail);
		RegMem(Entity, railToPower);
		RegMem(Entity, targetEntity);
		RegMem(Entity, needsPower);
		RegMem(Entity, spawnCountMax);
		RegMem(Entity, spawnInterval);
		RegMem(Entity, weaponType);

		RegMem(Globals, resolutionPerc);
		RegMem(Globals, sensitivity);

		RegMem(ActorInfo, usesOverrideBounds);
		RegMem(ActorInfo, overrideBoundsOffset);
		RegMem(ActorInfo, overrideBoundsSize);

		game = (Game *)zalloc(sizeof(Game));

		loadStruct("Globals", "assets/info/globals.txt", &game->globals);
		loadStructArray("ActorInfo", "assets/info/actorInfos.txt", game->actorInfos, ACTOR_TYPES_MAX, sizeof(ActorInfo));

		{
			WeaponInfo *wInfo;

			wInfo = &game->weaponInfos[WEAPON_GLOCK];
			wInfo->maxBullets = 17;
			wInfo->reloadTime = 2;
			wInfo->fullAuto = false;
			wInfo->shotInterval = 0.1;
			wInfo->damage = 20;
			wInfo->aimerSpeed = 0.5;
			wInfo->aimerSize = v2(128, 128);

			wInfo = &game->weaponInfos[WEAPON_UZI];
			wInfo->maxBullets = 32;
			wInfo->reloadTime = 2;
			wInfo->fullAuto = true;
			wInfo->shotInterval = 0.05;
			wInfo->damage = 10;
			wInfo->aimerSpeed = 0.1;
			wInfo->aimerSize = v2(512, 128);

			wInfo = &game->weaponInfos[WEAPON_SHOTGUN];
			wInfo->maxBullets = 8;
			wInfo->reloadTime = 10;
			wInfo->fullAuto = false;
			wInfo->shotInterval = 1;
			wInfo->damage = 10;
			wInfo->extraShots = 11;
			wInfo->spread = 0.01;
			wInfo->needsPump = true;
			wInfo->aimerSpeed = 0.5;
			wInfo->aimerSize = v2(64, 64);

			for (int i = 0; i < WEAPONS_MAX; i++) {
				game->weaponInfos[i].name = (char *)weaponTypeString[i];
				game->weaponInfos[i].cappedName = (char *)weaponTypeStringCaped[i];
				game->weaponInfos[i].type = (WeaponType)i;
			}
		}

		for (int i = 0; i < ACTOR_TYPES_MAX; i++) { //@copyPastedRefreshActorInfos
			game->actorInfos[i].name = (char *)actorTypeStrings[i];
		}

		maximizeWindow();

		world->sunPosition = v3(0, 0, -100);
		game->realCameraSpeed = 1;
		game->defaultFont = createFont("assets/fonts/arial.ttf", 24);
		game->bigFont = createFont("assets/fonts/arial.ttf", 42);
		game->timeScale = 1;

		// game->skybox = createSkybox("assets/images/greySkybox");
		game->skybox = createSkybox("assets/images/skybox");
	}

	Globals *globals = &game->globals;

	if (!game->scene.name[0] && game->scene.entitiesNum == 0) {
		const char *levelName = "TestLevel";
		// const char *levelName = "EmptyLevel";

		char *scenePath = frameSprintf("assets/scenes/%sScene.txt", levelName);
		char *entitiesPath = frameSprintf("assets/scenes/%sEntities.txt", levelName);
		loadStruct("Scene", scenePath, &game->scene); //@copyPastedLoadLevel
		loadStructArray("Entity", entitiesPath, game->entities, ENTITIES_MAX, sizeof(Entity));
		game->sceneFrames = 0;
	}

	bool mouseJustDown = false;
	bool mouseJustUp = false;
	bool mouseDown = false;
	bool rightMouseJustDown = false;
	int mouseWheel = 0;
	{ /// Figure out mouse inputs
		mouseJustDown = platform->mouseJustDown;
		mouseJustUp = platform->mouseJustUp;
		mouseDown = platform->mouseDown;
		rightMouseJustDown = platform->rightMouseJustDown;
		mouseWheel = platform->mouseWheel;
		if (platform->hoveringGui) {
			mouseDown = false;
			mouseJustUp = false;
			mouseJustDown = false;
			rightMouseJustDown = false;
			mouseWheel = false;
		}

		bool rel = true;
#if !defined(__EMSCRIPTEN__)
		if (!platform->windowHasFocus) rel = false;
#endif
		if (game->inEditor && !game->usingEditorCamera) rel = false;
		platform->useRelativeMouse = rel;

		if (platform->useRelativeMouse) {
			// platform->mouse.x += platform->relativeMouse.x;
			// platform->mouse.y += platform->relativeMouse.y;
			platform->mouse.x = platform->windowWidth/2;
			platform->mouse.y = platform->windowHeight/2;
		}
	} ///

	Scene *scene = &game->scene;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	int gameHeight = platform->windowHeight * globals->resolutionPerc;
	int gameWidth = platform->windowWidth * globals->resolutionPerc;

	bool resolutionChanged = false;
	if (game->gameWidth != gameWidth || game->gameHeight != gameHeight || !game->gameTexture) {
		game->gameWidth = gameWidth;
		game->gameHeight = gameHeight;
		resolutionChanged = true;

		if (game->gameTexture) destroyTexture(game->gameTexture);
		game->gameTexture = createTexture(gameWidth, gameHeight, NULL);
		// game->gameTexture = createTexture(gameWidth, gameHeight, NULL, _F_TD_RGBA16F);

		if (game->gameBloomTexture) destroyTexture(game->gameBloomTexture);
		game->gameBloomTexture = createTexture(gameWidth, gameHeight);

		if (game->gameSmallBloomTexture) destroyTexture(game->gameSmallBloomTexture);
		game->gameSmallBloomTexture = createTexture(gameWidth * bloomScale, gameHeight * bloomScale);
	}

	bool doPost = true;

	pushCamera2d(mat3());
	Matrix3 mat = mat3();
	mat = mat.scale(globals->resolutionPerc, globals->resolutionPerc);
	pushCamera2d(mat);

	if (game->inEditor) clearRenderer();
	if (doPost) {
		pushTargetTexture(game->gameTexture);
		setTargetTextureEx(1, game->gameBloomTexture);
		setTargetTexturesNum(2);
		if (platform->frameCount == 0 || resolutionChanged) attachDepthBuffer(gameWidth, gameHeight); //@incomplete Unsure when this actually has to get called
	}

	if (game->inEditor) clearRenderer();
	drawSkyboxImm(game->skybox);

	Actor *player = NULL;
	MeshTri *playerMeshTri = NULL;
	{ /// Preupdate actors
		for (int i = 0; i < game->actorsNum; i++) {
			Actor *actor = &game->actors[i];
			if (actor->type == ACTOR_PLAYER) {
				player = actor;

				Vec3 size = getSize(actor->bounds);
				Entity *currentNavMesh = getEntity(game->currentNavMeshEntityId);
				if (currentNavMesh) {
					float rayDist;
					MeshTri *meshTri;
					Vec3 start = actor->position + v3(0, 0, size.z*LEG_HEIGHT_PERC);
					if (rayTestEntity(currentNavMesh, start, v3(0, 0, -1), &rayDist, &meshTri)) {
						playerMeshTri = meshTri;
						break;
					}
				}
			}
		}
	} ///

	if (playerMeshTri) game->lastKnownPlayerMeshTri = playerMeshTri;
	if (!playerMeshTri) playerMeshTri = game->lastKnownPlayerMeshTri;

	AABB poweredBounds = {};
	{
		Entity *boundsEntity = getEntity(game->poweredBoundsEntityId);
		if (boundsEntity) {
			poweredBounds = boundsEntity->bounds;
		}
	}

	if (!isZero(poweredBounds) && game->currentNavMeshEntityId == 0) {
		for (int i = 0; i < scene->entitiesNum; i++) {
			Entity *entity = &game->entities[i];
			if (!strContains(entity->name, "dumbNav")) continue;
			if (!intersects(entity->bounds, poweredBounds)) continue;

			if (game->currentNavMeshEntityId) {
				logf("%s(%d) is a double nav\n", entity->name, entity->id);
				continue;
			}

			Mesh *mesh = getMesh(entity->path);
			if (!mesh) {
				logf("Nav mesh %s(%d) has no mesh\n", entity->name, entity->id);
				break;
			}

			game->currentNavMeshEntityId = entity->id;
		}
	}

	bool hasViewModel = false;
	Skeleton *viewModelSkeleton;
	Mesh *viewModelMesh;
	Matrix4 gunMeshMatrix;

	{ /// Update actors
		Actor *actorsLeft[ACTORS_MAX];
		int actorsLeftNum = 0;

		for (int i = 0; i < game->actorsNum; i++) actorsLeft[actorsLeftNum++] = &game->actors[i];

		Actor *orderedActors[ACTORS_MAX];
		int orderedActorsNum = 0;

		for (int i = 0; i < actorsLeftNum; i++) {
			Actor *actor = actorsLeft[i];
			if (actor->type == ACTOR_PLAYER) {
				orderedActors[orderedActorsNum++] = actor;
				arraySpliceIndex(actorsLeft, actorsLeftNum, sizeof(Actor *), i);
				i--;
				actorsLeftNum--;
			}
		}

		for (int i = 0; i < actorsLeftNum; i++) {
			Actor *actor = actorsLeft[i];
			orderedActors[orderedActorsNum++] = actor;
			arraySpliceIndex(actorsLeft, actorsLeftNum, sizeof(Actor *), i);
			i--;
			actorsLeftNum--;
		}

		for (int i = 0; i < orderedActorsNum; i++) {
			Actor *actor = orderedActors[i];
			actor->powered = false;

			float playerClosestDist = 0;
			if (player) {
				Vec3 playerClosest;
				Vec3 enemyClosest;
				overlaps(actor->bounds, makeLine3(getCenter(actor->bounds), getCenter(player->bounds)), &enemyClosest);
				overlaps(player->bounds, makeLine3(getCenter(actor->bounds), getCenter(player->bounds)), &playerClosest);
				playerClosestDist = playerClosest.distance(enemyClosest);
			}

			Matrix4 mat = mat4();
			mat = mat.translate(actor->position);
			mat = mat.rotateQuaternion(eulerToQuaternion(actor->rotation));
			mat = mat.scale(actor->scale);
			actor->matrix = mat;

			Entity *sourceEntity = getEntity(actor->sourceEntityId);
			if (sourceEntity && sourceEntity->alwaysPowered) actor->powered = true;

			if (AABBContains(poweredBounds, actor->position)) {
				bool railIsOn = false;
				if (sourceEntity) {
					if (sourceEntity->rail == 0) {
						actor->powered = true;
					} else {
						for (int i = 0; i < game->poweredRailsNum; i++) {
							if (game->poweredRails[i] == sourceEntity->rail) {
								actor->powered = true;
								break;
							}
						}
					}
				}
			}

			if (actor->hp <= 0 && actor->type != ACTOR_PLAYER) {
				if (actor->state != ASTATE_DYING) {
					actor->state = ASTATE_DYING;
				}
			}

			if (actor->prevState != actor->state) {
				actor->prevState = actor->state;
				actor->stateTime = 0;
			}
			if (actor->type == ACTOR_NONE) {
				/// I should draw something in debug mode...
			} else if (actor->type == ACTOR_PLAYER) {
				actor->hasCollision = true;
				actor->repulses = true;

				if (!game->inEditor) {
					if (actor->hp <= 0) {
						game->realCameraRotation = actor->rotation + v3(0, -0.5, 0.1);
						game->realCameraPosition = actor->position + v3(0, 0, PLAYER_VIEW_HEIGHT/2.0);
						continue;
					}

					Vec3 playerGroundInputVector = v3();
					if (keyPressed('W')) {
						playerGroundInputVector = playerGroundInputVector.add(v3(0, 0, -1));
					}
					if (keyPressed('S')) {
						playerGroundInputVector = playerGroundInputVector.add(v3(0, 0, 1));
					}
					if (keyPressed('A')) {
						playerGroundInputVector = playerGroundInputVector.add(v3(-1, 0, 0));
					}
					if (keyPressed('D')) {
						playerGroundInputVector = playerGroundInputVector.add(v3(1, 0, 0));
					}
					if (!playerGroundInputVector.isZero()) playerGroundInputVector = playerGroundInputVector.normalize();


					bool canJump = true;
					if (actor->state == ASTATE_CROUCHING) canJump = false;
					if (canJump && actor->offGroundTime < elapsed*3 && keyJustPressed(' ')) {
						actor->velo.z += 0.5;
					}

					Vec4 addedCameraPosition4 = v4(playerGroundInputVector.x, playerGroundInputVector.y, playerGroundInputVector.z, 0);
					addedCameraPosition4 = game->cameraMatrix.invert().multiply(addedCameraPosition4);

					Vec2 playerInput2d = v2(addedCameraPosition4.x, addedCameraPosition4.y).normalize();
					playerGroundInputVector.x = playerInput2d.x;
					playerGroundInputVector.y = playerInput2d.y;
					playerGroundInputVector.z = 0;

					Vec3 moveAccel = playerGroundInputVector;

					float speed = 0.05;
					if (actor->state == ASTATE_CROUCHING) speed *= clampMap(actor->stateTime, 0, 1, 1, 0.5);
					{
						float boost = clampMap(actor->phaseBootsTime, 0, 5, 2, 1);
						actor->phaseBootsTime += elapsed;
						speed *= boost;
					}
					if (globals->superSpeed) speed = 0.5;

					actor->accel = actor->accel + moveAccel*speed;
					if (platform->useRelativeMouse && game->time > 0.1) {
						actor->rotation.z += platform->relativeMouse.x * 0.01 * (globals->sensitivity / 10.0);
						actor->rotation.x += platform->relativeMouse.y * 0.01 * (globals->sensitivity / 10.0);
					}

					float ep = 0.001;
					// if (actor->rotation.x > 0-ep) actor->rotation.x = 0-ep;
					// if (actor->rotation.x < -M_PI+ep) actor->rotation.x = -M_PI+ep;
				}

				{ //@copyPastedCreateBounds
					Vec3 playerSize = v3(1, 1, PLAYER_HEIGHT);

					Vec3 center = actor->position;
					actor->bounds.min.x = center.x - playerSize.x/2;
					actor->bounds.min.y = center.y - playerSize.y/2;
					actor->bounds.min.z = center.z;
					actor->bounds.max.x = center.x + playerSize.x/2;
					actor->bounds.max.y = center.y + playerSize.y/2;
					actor->bounds.max.z = center.z + playerSize.z;
				}

				if (!actor->skeleton) {
					BaseSkeleton *base = getBaseSkeleton("assets/models/ViewModel/ViewModel.ViewModelArm.skele.iqm");
					actor->skeleton = deriveSkeleton(base);

					createSkeletonBlend(actor->skeleton, "root", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(actor->skeleton, "main", SKELETON_BLEND_ANIMATION);
					// createSkeletonBlend(actor->skeleton, "recoil", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(actor->skeleton, "autoAim", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(actor->skeleton, "viewBob", SKELETON_BLEND_MANUAL_BONES);
				}

				Skeleton *skeleton = actor->skeleton;

				bool reloads = false;
				if (keyJustPressed('R')) reloads = true;

				if (actor->state == ASTATE_NONE) {
				}

				bool canReload = true;
				bool canShoot = true;
				bool canPushButtons = true;

				if (actor->currentlyPlaying) {
					if (!actor->currentAnimQueueItem.skippable) canPushButtons = false;
				}

				if (game->inEditor) canPushButtons = false;
				if (actor->animQueueNum > 0) canPushButtons = false;
				if (actor->state != ASTATE_NONE) canPushButtons = false;

				if (!canPushButtons) {
					canReload = false;
					canShoot = false;
				}

				if ((mouseJustDown || keyJustPressed('Q')) && !canPushButtons) {
					player->tryingToCancel = true;
				}

				int maxBullets = game->weaponInfos[actor->weaponType].maxBullets;
				if (actor->bullets[actor->weaponType] >= maxBullets) canReload = false;
				if (actor->reserveBullets[actor->weaponType] <= 0) canReload = false;

				WeaponInfo *wInfo = &game->weaponInfos[player->weaponType];
				if (actor->shotTime < wInfo->shotInterval) canShoot = false;

				bool shot = false;
				if (wInfo->fullAuto && mouseDown) shot = true;
				if (!wInfo->fullAuto && mouseJustDown) shot = true;

				if (canShoot && shot) {
					if (actor->bullets[actor->weaponType] > 0) {
						char *sfxPath = frameSprintf("assets/audio/sounds/%s/shoot.ogg", wInfo->name);
						SoundSource *source = playWorldSound(v3(), sfxPath);
						if (source) {
							alSourcef(source->source, AL_PITCH, rndFloat(0.9, 1.1));
							if (wInfo->type == WEAPON_GLOCK) setVolume(source, 0.1);
							if (wInfo->type == WEAPON_UZI) setVolume(source, 0.5);
							source->ui = true;
						}

						actor->shotTime = 0;
						actor->bullets[actor->weaponType]--;

						{
							AnimQueueItem *item = createAnimQueueItem(actor, frameSprintf("shooting%s", weaponTypeStringCaped[actor->weaponType]), 0.25, false);
							item->skippable = true;
						}

						if (wInfo->needsPump && actor->bullets[actor->weaponType] > 0) {
							AnimQueueItem *item = createAnimQueueItem(actor, frameSprintf("pump%s", weaponTypeStringCaped[actor->weaponType]), 1, false);
						}

						Vec3 start = game->realCameraPosition;
						Vec3 dir = getRayFromSceenSpace(game->aimerCursor);

						for (int i = 0; i < 1+wInfo->extraShots; i++) {
							Vec3 addDir;
							addDir.x = rndFloat(-1, 1) * 2.0*M_PI;
							addDir.y = rndFloat(-1, 1) * 2.0*M_PI;
							addDir.z = rndFloat(-1, 1) * 2.0*M_PI;
							addDir = addDir * wInfo->spread;

							shootRayBullet(start, dir+addDir, player);
						}

						// drawBeam(start, dir * 20, 0.1, 0xFFFF0000);

					} else {
						reloads = true;
					}
				}

				if (canReload && reloads) {
					WeaponInfo *wInfo = &game->weaponInfos[player->weaponType];

					if (wInfo->type == WEAPON_SHOTGUN) {
						createAnimQueueItem(actor, frameSprintf("reloading%sStart", weaponTypeStringCaped[actor->weaponType]), 0.5, false);

						AnimQueueItem *item = createAnimQueueItem(actor, frameSprintf("reloading%sNext", weaponTypeStringCaped[actor->weaponType]), 0.5, false);
						item->reloadChain = true;
					} else {
						AnimQueueItem *item = createAnimQueueItem(actor, frameSprintf("reloading%s", weaponTypeStringCaped[actor->weaponType]), wInfo->reloadTime, false);
						item->reloadsAfter = true;

						createAnimQueueItem(actor, frameSprintf("holding%s", weaponTypeStringCaped[actor->weaponType]), wInfo->reloadTime, false);
					}
				}

				{
					WeaponType newWeapon = WEAPON_NONE;
					if (actor->weaponType == WEAPON_NONE && actor->animQueueNum == 0 && actor->state == ASTATE_NONE) {
						if (actor->lastWeaponWasSecondary) {
							newWeapon = actor->secondaryWeapon;
						} else {
							newWeapon = actor->primaryWeapon;
						}
					}
					//@next Fill out room

					if (canPushButtons && (keyJustPressed('Q')) || actor->forceWeaponSwitch) {
						actor->forceWeaponSwitch = false;
						if (actor->weaponType != actor->primaryWeapon) {
							newWeapon = actor->primaryWeapon;
							actor->lastWeaponWasSecondary = false;
						} else {
							newWeapon = actor->secondaryWeapon;
							actor->lastWeaponWasSecondary = true;
						}
					}

					if (newWeapon != WEAPON_NONE) {
						actor->weaponType = newWeapon;
						if (game->gunSkeleton) {
							destroySkeleton(game->gunSkeleton);
							game->gunSkeleton = NULL;
						}

						AnimQueueItem *item = createAnimQueueItem(actor, frameSprintf("raising%s", weaponTypeStringCaped[actor->weaponType]), -1, false);
					}
				}

				if (canPushButtons && keyJustPressed('I')) {
					AnimQueueItem *item = createAnimQueueItem(actor, "pressLeftButton", 1, false);
					item->toggleInfoToolAfter = 1;
					actor->weaponType = WEAPON_NONE;
				}

				if (canPushButtons && keyJustReleased('Z')) {
					if (actor->phaseBootsTime > 15) actor->phaseBootsTime = 0;
				}

				SkeletonBlend *blend = getSkeletonBlend(actor->skeleton, "main");
				if (canPushButtons && keyJustPressed('C')) {
					AnimQueueItem *item = createAnimQueueItem(actor, "crouch", 0.75, false);
					actor->weaponType = WEAPON_NONE;
					actor->state = ASTATE_CROUCHING;
				}

				if (game->playerStandingOn) {
					if (game->playerStandingOn->type == ACTOR_WEAPON && canPushButtons) {
						WeaponInfo *wInfo = &game->weaponInfos[player->weaponType];
						WeaponInfo *otherWInfo = &game->weaponInfos[game->playerStandingOn->weaponType];
						log3f(game->playerStandingOn->position, "Swap %s for %s?", wInfo->name, otherWInfo->name);
						if (keyJustPressed('E')) {
							if (actor->lastWeaponWasSecondary) {
								actor->secondaryWeapon = otherWInfo->type;
							} else {
								actor->primaryWeapon = otherWInfo->type;
							}
							game->playerStandingOn->weaponType = wInfo->type;
							destroySkeleton(game->playerStandingOn->skeleton);
							game->playerStandingOn->skeleton = NULL;
							actor->weaponType = WEAPON_NONE;
						}
					} else if (game->playerStandingOn->type == ACTOR_TELEPORTER && canPushButtons) {
						log3f(game->playerStandingOn->position, "Activate?");
						if (keyJustPressed('E')) {
							Entity *source = getEntity(game->playerStandingOn->sourceEntityId);
							if (!source) logf("No source\n");
							Entity *target = getEntity(source->targetEntity);
							if (target) {
								actor->position = target->position;
							} else {
								logf("No target to teleport\n");
							}
						}
					}
				}


				if (actor->state == ASTATE_CROUCHING && !actor->currentlyPlaying) {
					if (keyJustPressed(' ')) {
						Vec3 dir = v3();
						if (keyPressed('W')) dir.z--;
						if (keyPressed('S')) dir.z++;
						if (keyPressed('A')) dir.x--;
						if (keyPressed('D')) dir.x++;

						bool scrubZ = true;
						if (dir.isZero()) {
							dir = v3(0, 1, -1);
							scrubZ = false;
						}

						dir = dir.normalize();
						Vec4 dir4 = v4(dir.x, dir.y, dir.z, 0);
						dir4 = game->cameraMatrix.invert() * dir4;

						if (scrubZ) dir4.z = 0;
						actor->velo += v3(dir4.x, dir4.y, dir4.z + 0.1) * 2;

						AnimQueueItem *item = createAnimQueueItem(actor, "crouchLaunch", 0.15, false);
						item->noneStateAfter = true;
					}
				}

				Vec3 viewNoise = v3();
				viewNoise.x = timePhase(game->time, .25) * 0.5;
				viewNoise.z = perlin2d(game->time+1, 0, 1, 1);
				viewNoise = viewNoise * 2 - 1;
				viewNoise = viewNoise * 0.01;
				viewNoise.y = 0;

				SkeletonBlend *root = getSkeletonBlend(skeleton, "root");
				if (root) {
					Xform xform = newXform();
					xform.translation = actor->position + v3(0, 0, PLAYER_VIEW_HEIGHT);
					Vec3 rot = v3();
					rot.z = -actor->rotation.z;
					rot.x = -(actor->rotation.x + M_PI/2);
					rot = rot - viewNoise;
					xform.rotation = eulerToQuaternion(rot);
					xform.scale = actor->scale;
					int boneIndex = 0;
					root->controlMask[boneIndex] = true;
					root->poseXforms[boneIndex] = xform;
				}

				SkeletonBlend *recoil = getSkeletonBlend(skeleton, "recoil");
				if (recoil) {
					Xform xform = newXform();
					// xform.translation = v3(0, 0, 0);
					float deg = clampMap(actor->shotTime, 0, 0.1, 3, 0);
					// float deg = clampMap(actor->shotTime, 0, 1, 50, 0);
					xform.rotation = eulerToQuaternion(v3(toRad(deg), 0, 0));

					int boneIndex = getBoneIndex(skeleton, "arm2.r");
					recoil->controlMask[boneIndex] = true;
					recoil->poseXforms[boneIndex] = xform;
				}

				SkeletonBlend *autoAim = getSkeletonBlend(skeleton, "autoAim");
				if (autoAim) {
					Vec2 off;
					if (game->aimerRect.width == 0) game->aimerRect.width = 1;
					if (game->aimerRect.height == 0) game->aimerRect.height = 1;
					off.x = norm(game->aimerRect.x, game->aimerRect.x+game->aimerRect.width, game->aimerCursor.x);
					off.y = norm(game->aimerRect.y, game->aimerRect.y+game->aimerRect.height, game->aimerCursor.y);
					off.x = off.x * 2.0 - 1.0;
					off.y = off.y * 2.0 - 1.0;
					float power = 0;
					float maxDist = 64;
					if (!game->aimerTarget.isZero()) {
						power = maxDist - player->position.distance(game->aimerTarget);
						power = power / maxDist;
					}
					power *= 0.2;
					if (power <= 0) power = 0.01;

					Vec3 radsOff;
					radsOff.z = -off.x * power;
					radsOff.y = 0;
					radsOff.x = -off.y * power;

					actor->visualAutoAimRadsOff = lerp(actor->visualAutoAimRadsOff, radsOff, 0.1);

					Xform bobXform = newXform();
					bobXform.translation.z = timePhase(actor->groundMoveDistance, 0.1) * 0.1;
					bobXform.translation.x = timePhase(actor->groundMoveDistance, 0.05) * 0.05;

					{
						Xform xform = newXform();
						xform.rotation = eulerToQuaternion(actor->visualAutoAimRadsOff);
						xform = multiplyXforms(xform, bobXform);

						int boneIndex = getBoneIndex(skeleton, "arm1.r");
						autoAim->controlMask[boneIndex] = true;
						autoAim->poseXforms[boneIndex] = xform;
					}

					{
						Xform xform = newXform();
						xform.rotation = eulerToQuaternion(quaternionToEuler(xform.rotation) * -1);
						xform = multiplyXforms(xform, bobXform);

						int boneIndex = getBoneIndex(skeleton, "arm1.l");
						autoAim->controlMask[boneIndex] = true;
						autoAim->poseXforms[boneIndex] = xform;
					}
				}

				SkeletonBlend *viewBob = getSkeletonBlend(skeleton, "viewBob");
				if (viewBob) {
					Xform bobXform = newXform();
					// bobXform.rotation = eulerToQuaternion(actor->visualAutoAimRadsOff);

					// int boneIndex = getBoneIndex(skeleton, "arm1.r");
					// viewBob->controlMask[boneIndex] = true;
					// viewBob->poseXforms[boneIndex] = bobXform;
				}

				updateActorAnimation(actor);
				updateSkeleton(actor->skeleton, elapsed);

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "reloadingGlock", 15)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/clipEject.ogg", wInfo->name));
				}

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "reloadingGlock", 80)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/clipInsert.ogg", wInfo->name));
				}

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "reloadingGlock", 145)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/pullSlide.ogg", wInfo->name));
				}

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "reloadingGlock", 155)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/releaseSlide.ogg", wInfo->name));
				}

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "reloadingUzi", 20)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/clipEject.ogg", wInfo->name));
				}

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "reloadingUzi", 80)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/clipInsert.ogg", wInfo->name));
				}

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "reloadingUzi", 110)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/clipTap.ogg", wInfo->name));
				}

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "pumpShotgun", 20)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/pump.ogg", wInfo->name));
				}

				if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "reloadingShotgunNext", 5)) {
					playUiSound(frameSprintf("assets/audio/sounds/%s/insertShell.ogg", wInfo->name));
				}

				{ /// Gun
					WeaponInfo *wInfo = &game->weaponInfos[player->weaponType];

					int boneIndex;
					if (wInfo->type == WEAPON_SHOTGUN) {
						boneIndex = getBoneIndex(skeleton, "gun.l");
					} else {
						boneIndex = getBoneIndex(skeleton, "gun.r");
					}

					Bone *bone = &skeleton->base->bones[boneIndex];
					gunMeshMatrix = skeleton->currentTransforms[boneIndex] * bone->modelSpaceMatrix;

					if (!game->gunSkeleton) {
						char *armPath = frameSprintf("assets/models/ViewModel/ViewModel.%sArm.skele.iqm", wInfo->cappedName);
						BaseSkeleton *base = getBaseSkeleton(armPath);
						if (base) {
							game->gunSkeleton = deriveSkeleton(base);
							createSkeletonBlend(game->gunSkeleton, "main", SKELETON_BLEND_ANIMATION);
						} else {
							logf("Can't find %s\n", armPath);
						}
					}

					SkeletonBlend *main = getSkeletonBlend(game->gunSkeleton, "main");
					if (main) {
						main->animation = getAnimation(game->gunSkeleton, "idle");
					}

					updateSkeleton(game->gunSkeleton, elapsed);
				} ///

				{ /// Camera
					Vec3 animatedCameraAngle = v3();
					Vec3 animatedCameraPosition = v3();
					if (player->skeleton) {
						SkeletonBlend *main = getSkeletonBlend(player->skeleton, "main");
						if (main) {
							int boneIndex = getBoneIndex(player->skeleton, "cameraAngle");
							animatedCameraPosition = main->poseXforms[boneIndex].translation;
							Vec4 quat = main->poseXforms[boneIndex].rotation;
							animatedCameraAngle = quaternionToEuler(quat);
							// logf("Anim: %s\n", main->animation->name);
							// quat.print("aca");
						}
					}

					Vec3 knockBackRot;
					{
						float hitLength = 0.5;
						float perc = 1.0 - (actor->timeSinceLastHit / hitLength);
						if (perc < 0) perc = 0;

						knockBackRot = v3(.1, 0, 0)*perc;
					}

					if (!game->inEditor) {
						game->realCameraPosition = actor->position + v3(0, 0, PLAYER_VIEW_HEIGHT) + animatedCameraPosition;
						game->realCameraRotation = actor->rotation + viewNoise + animatedCameraAngle + knockBackRot;
					}
				} ///

				hasViewModel = true;
				viewModelSkeleton = skeleton;
				game->playerStandingOn = NULL;

				Mesh *mesh = getMesh("assets/models/ViewModel/ViewModel.ViewModel.mesh.iqm");
				viewModelMesh = mesh;
			} else if (actor->type == ACTOR_ZOMBIE) {
				actor->repulses = true;
				actor->hasCollision = true;
				actor->hostile = true;

				float legHeightPerc = 0.3;

				if (!actor->skeleton) {
					BaseSkeleton *base = getBaseSkeleton("assets/models/Enemy/Enemy.EnemyArm.skele.iqm");
					actor->skeleton = deriveSkeleton(base);

					createSkeletonBlend(actor->skeleton, "root", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(actor->skeleton, "knockBack", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(actor->skeleton, "main", SKELETON_BLEND_ANIMATION);
				}

				if (actor->state == ASTATE_NONE) {
					actor->state = ASTATE_WALKING;

					AnimQueueItem *item = createAnimQueueItem(actor, "walk", -1, true);
				}

				bool hittingPlayer = playerClosestDist < 1;

				float attackLength = 0.5;
				if (actor->state == ASTATE_WALKING) {
					Vec2 loc2 = v2(actor->position.x, actor->position.y);
					Vec2 playerLoc = v2(player->position.x, player->position.y);

					Vec2 dir = (playerLoc - loc2).normalize();
					actor->rotation = v3(0, 0, toRad(dir) + toRad(90));
					float accel = 0.02;

					actor->velo.x += dir.x*accel;
					actor->velo.y += dir.y*accel;
					actor->velo.z = 0;

					if (hittingPlayer) {
						actor->state = ASTATE_ATTACKING;

						clearAnimQueue(actor);
						AnimQueueItem *item = createAnimQueueItem(actor, "attack", attackLength, false);
						item->noneStateAfter = true;
					}
				} else if (actor->state == ASTATE_ATTACKING) {
					float perc = actor->stateTime / attackLength;
					if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "attack", 5)) {
						if (hittingPlayer) {
							dealDamage(player, 5, actor);
							player->slow += 0.3;
						}
					}

					if (perc > 1) actor->state = ASTATE_WALKING;
				} else if (actor->state == ASTATE_DYING) {
					if (actor->stateTime == 0) {
						float dyingLength = 1;

						clearAnimQueue(actor);
						AnimQueueItem *item = createAnimQueueItem(actor, "dying", dyingLength, false);
					}
				}

				SkeletonBlend *root = getSkeletonBlend(actor->skeleton, "root");
				if (root) {
					Xform xform = toXform(actor->matrix);
					int boneIndex = 0;
					root->controlMask[boneIndex] = true;
					root->poseXforms[boneIndex] = xform;
				}

				updateActorAnimation(actor);
				updateSkeleton(actor->skeleton, elapsed);

				Mesh *mesh = getMesh("assets/models/Enemy/Enemy.Enemy.mesh.iqm");

				MeshProps props = newMeshProps();
				props.skeleton = actor->skeleton;
				drawMesh(mesh, props);
			} else if (actor->type == ACTOR_SHOOTER) {
				actor->repulses = true;
				actor->hasCollision = true;
				actor->hostile = true;

				if (!actor->skeleton) {
					BaseSkeleton *base = getBaseSkeleton("assets/models/Shooter/Shooter.ShooterArm.skele.iqm");
					actor->skeleton = deriveSkeleton(base);

					createSkeletonBlend(actor->skeleton, "root", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(actor->skeleton, "main", SKELETON_BLEND_ANIMATION);
					createSkeletonBlend(actor->skeleton, "knockBack", SKELETON_BLEND_MANUAL_BONES);
				}

				if (actor->state == ASTATE_NONE) {
					actor->state = ASTATE_WALKING;

					AnimQueueItem *item = createAnimQueueItem(actor, "walk", -1, true);
				}

				SkeletonBlend *root = getSkeletonBlend(actor->skeleton, "root");
				if (root) {
					root->controlMask[0] = true;
					root->poseXforms[0] = toXform(actor->matrix);
				}

				if (actor->state == ASTATE_WALKING) {
					updateNav(actor, playerMeshTri, player->position);

					if (!actor->navNextTarget.isZero()) {
						Vec3 dir = (actor->navNextTarget - actor->position).normalize();
						actor->accel = dir * 0.05;
						actor->rotation = v3(0, 0, toRad(v2(dir.x, dir.y)) + toRad(90));
					}

					Matrix4 mat = getCurrentBoneMatrix(actor->skeleton, "eyes");
					Vec3 start = mat.getPosition();
					Vec3 dest = getCenter(player->bounds);
					if (start.distance(dest) <= 20 && testLineOfSight(start, dest)) actor->state = ASTATE_ATTACKING;
				} else if (actor->state == ASTATE_ATTACKING) {
					float chargeTime = 1.0;
					if (actor->stateTime == 0) {
						clearAnimQueue(actor);
						AnimQueueItem *item = createAnimQueueItem(actor, "attack", chargeTime, false);

						SoundSource *source = playWorldSound(actor->position, "assets/audio/sounds/electricityCharge.ogg");
						setSoundLength(source, chargeTime);
					}

					float attackPerc = actor->stateTime / chargeTime;
					Matrix4 mat = getCurrentBoneMatrix(actor->skeleton, "arm2.r");
					Vec3 bonePos = mat.getPosition();
					Vec3 dir = (getCenter(player->bounds) - bonePos).normalize();
					drawSphere(bonePos + dir*3, lerp(0, 0.25, attackPerc), 0xFFFF0000);
					if (attackPerc >= 1) {
						SoundSource *source = playWorldSound(actor->position, "assets/audio/sounds/electricityShoot.ogg");
						shootProjectileBullet(bonePos, dir, actor);
						actor->state = ASTATE_NONE;
					}
				} else if (actor->state == ASTATE_FLINCHING) {
					if (actor->stateTime == 0) {
						clearAnimQueue(actor);
						AnimQueueItem *item = createAnimQueueItem(actor, "flinch2", 0.5, false);
						item->noneStateAfter = true;
					}
				} else if (actor->state == ASTATE_DYING) {
					if (actor->stateTime == 0) {
						clearAnimQueue(actor);
						AnimQueueItem *item = createAnimQueueItem(actor, "dying", 0.5, false);
					}
				}

				updateActorAnimation(actor);
				updateSkeleton(actor->skeleton, elapsed);

				Mesh *mesh = getMesh("assets/models/Shooter/Shooter.Shooter.mesh.iqm");

				MeshProps props = newMeshProps();
				props.skeleton = actor->skeleton;
				drawMesh(mesh, props);
			} else if (actor->type == ACTOR_SLASHER) {
				actor->repulses = true;
				actor->hasCollision = true;
				actor->hostile = true;

				if (!actor->skeleton) {
					BaseSkeleton *base = getBaseSkeleton("assets/models/Slasher/Slasher.SlasherArm.skele.iqm");
					actor->skeleton = deriveSkeleton(base);

					createSkeletonBlend(actor->skeleton, "root", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(actor->skeleton, "main", SKELETON_BLEND_ANIMATION);
					createSkeletonBlend(actor->skeleton, "knockBack", SKELETON_BLEND_MANUAL_BONES);
				}

				if (actor->state == ASTATE_NONE) {
					actor->state = ASTATE_WALKING;
					AnimQueueItem *item = createAnimQueueItem(actor, "walk", -1, true);
				}

				SkeletonBlend *root = getSkeletonBlend(actor->skeleton, "root");
				if (root) {
					root->controlMask[0] = true;
					root->poseXforms[0] = toXform(actor->matrix);
				}

				if (actor->state == ASTATE_WALKING) {
					updateNav(actor, playerMeshTri, player->position);

					if (!actor->navNextTarget.isZero()) {
						Vec3 dir = (actor->navNextTarget - actor->position).normalize();
						actor->accel = dir * 0.05;
						actor->rotation = v3(0, 0, toRad(v2(dir.x, dir.y)) + toRad(90));
					}

					// Matrix4 mat = getCurrentBoneMatrix(actor->skeleton, "eyes");
					Vec3 start = getCenter(actor->bounds);
					Vec3 dest = getCenter(player->bounds);
					if (start.distance(dest) <= 50 && testLineOfSight(start, dest)) actor->state = ASTATE_ATTACKING;
				} else if (actor->state == ASTATE_ATTACKING) {
					if (actor->stateTime == 0) {
						clearAnimQueue(actor);
						createAnimQueueItem(actor, "charge", 2, false);
						AnimQueueItem *item = createAnimQueueItem(actor, "dash", 1, false);
						item->stateAfter = ASTATE_DASHING;
					}

					SkeletonAnimation *anim = &actor->skeleton->base->animations[actor->currentAnimQueueItem.index];
					if (skeletonIsPlayingAnimation(actor->skeleton, "main", "dash")) {
						Vec3 dir = (player->position - actor->position).normalize();
						actor->rotation.z = lerp(actor->rotation.z, toRad(v2(dir.x, dir.y)) + toRad(90), 0.1);
					}
				} else if (actor->state == ASTATE_DASHING) {
					if (actor->stateTime == 0) {
						if (testLineOfSight(actor->position, player->position)) {
							actor->dashTargetPoint = player->position;
						} else {
							actor->state = ASTATE_NONE;
						}
					}

					Vec3 dir = (actor->dashTargetPoint - actor->position).normalize();
					actor->accel = dir * 0.2;

					if (playerClosestDist < 2 && actor->timeSinceLastDashHit > 1) {
						actor->timeSinceLastDashHit = 0;
						dealDamage(player, 5, actor);
						player->velo += (player->position - actor->position).normalize() * 0.5;
					}

					if (actor->dashTargetPoint.distance(actor->position) < actor->velo.length()*2 || actor->stateTime > 5) {
						if (playerClosestDist < 6) { 
							actor->state = ASTATE_ATTACKING2;
						} else {
							actor->state = ASTATE_NONE;
						}
					}
				} else if (actor->state == ASTATE_ATTACKING2) {
					float maxTime = 1;
					if (actor->stateTime == 0) {
						clearAnimQueue(actor);
						AnimQueueItem *item = createAnimQueueItem(actor, "slash", maxTime, false);
						item->noneStateAfter = true;
					}

					// SkeletonAnimation *anim = getAnimation(actor->skeleton, "slash");
					int hitFrame = 35;
					float hitsAtTime = (float)hitFrame * elapsed;
					float hitRadius = 5;

					Mesh *sphereMesh = getMesh("assets/common/models.Sphere.mesh.iqm");
					MeshProps props = newMeshProps();

					props.matrix.TRANSLATE(getCenter(actor->bounds));

					if (actor->stateTime < hitsAtTime) {
						float perc = actor->stateTime / hitsAtTime;
						props.matrix.SCALE(lerp(0.1, 1, perc));
						props.alpha = lerp(0, 0.5, perc);
					} else {
						float perc = (actor->stateTime - hitsAtTime) / (maxTime - hitsAtTime);
						props.matrix.SCALE(lerp(hitRadius, hitRadius+1, perc));
						props.alpha = lerp(0.5, 0, perc);
						// logf("a %f\n", perc);
					}

					drawMesh(sphereMesh, props);

					if (skeletonBlendJustPlayedFrame(actor->skeleton, "main", "slash", hitFrame)) {
						if (playerClosestDist < hitRadius) {
							dealDamage(player, 20, actor);
							player->velo += (player->position - actor->position).normalize() * 4;
						}
					}
				} else if (actor->state == ASTATE_DYING) {
					if (actor->stateTime == 0) {
						clearAnimQueue(actor);
						AnimQueueItem *item = createAnimQueueItem(actor, "dying", 0.5, false);
					}
				}

				updateActorAnimation(actor);
				updateSkeleton(actor->skeleton, elapsed);

				Mesh *mesh = getMesh("assets/models/Slasher/Slasher.Slasher.mesh.iqm");

				MeshProps props = newMeshProps();
				props.skeleton = actor->skeleton;
				drawMesh(mesh, props);
			} else if (actor->type == ACTOR_PORTAL) {
				if (!actor->powered) continue;
				if (actor->spawnCountMax == 0) actor->spawnCountMax = 1;

				if (actor->timeSinceLastSpawn > actor->spawnInterval) {
					actor->timeSinceLastSpawn = 0;
					if (actor->spawnCount < actor->spawnCountMax) {
						actor->spawnCount++;
						Actor *newActor = createActor(actor->typeToSpawn);
						newActor->position = actor->spawnPosition;
						newActor->rotation = actor->spawnRotation;
						newActor->scale = actor->spawnScale;
						newActor->sourcePortalId = actor->id;
						newActor->sourceEntityId = actor->sourceEntityId;
						newActor->weaponType = actor->weaponType;


						if (newActor->type == ACTOR_PLAYER) {
							newActor->primaryWeapon = WEAPON_GLOCK;
							newActor->rotation = newActor->rotation.multiply(-1);
							newActor->phaseBootsTime = 999;
							// newActor->reserveBullets[WEAPON_GLOCK] = 17 * 3;
						} else if (newActor->type == ACTOR_AMMO) {
							Entity *source = getEntity(actor->sourceEntityId);
							newActor->value = source->targetEntity;
						} else if (newActor->type == ACTOR_PRESSURE_PLATE) {
							newActor->position = actor->position;
						}
					}

				}

				Mesh *mesh = getMesh("assets/models/Items/Items.Portal.mesh.iqm");
				MeshProps props = newMeshProps();
				props.matrix = actor->matrix;
				drawMesh(mesh, props);

				actor->bounds = actor->matrix.multiply(mesh->bounds);

				actor->timeSinceLastSpawn += elapsed;
			} else if (actor->type == ACTOR_GOAL) {
				Mesh *mesh = getMesh("assets/common/models.Sphere.mesh.iqm");
				MeshProps props = newMeshProps();
				props.matrix = actor->matrix;
				props.tint = 0xFF00FF00;
				props.flags |= _F_3D_BLOOM;
				drawMesh(mesh, props);

				actor->bounds = actor->matrix.multiply(mesh->bounds);

				if (AABBContains(player->bounds, actor->position)) {
					game->poweredBoundsEntityId = 0;
					game->poweredRailsNum = 0;
					for (int i = 0; i < game->actorsNum; i++) {
						Actor *actor = &game->actors[i];
						if (actor->type == ACTOR_PORTAL) {
							if (actor->typeToSpawn != ACTOR_PLAYER) actor->spawnCount = 0;
						} else if (actor->type == ACTOR_PLAYER) {
							// Nothing...
						} else {
							actor->shouldBeDestoryed = true;
						}
					}
				}

			} else if (actor->type == ACTOR_START) {
				Mesh *mesh = getMesh("assets/common/models.Sphere.mesh.iqm");
				actor->bounds = actor->matrix.multiply(mesh->bounds);

				MeshProps props = newMeshProps();
				props.matrix = actor->matrix;
				props.tint = 0xFFFF0000;
				props.flags |= _F_3D_BLOOM;
				drawMesh(mesh, props);

				if (intersects(player->bounds, actor->bounds)) {
					Actor *sourceActor = getActor(actor->sourcePortalId);
					if (sourceActor) {
						Entity *sourceEntity = getEntity(sourceActor->sourceEntityId);
						if (sourceEntity) {
							if (sourceEntity->targetEntity == 0) { 
								for (int i = 0; i < scene->entitiesNum; i++) {
									Entity *entity = &game->entities[i];
									if (entity->type != ENTITY_BOUNDS) continue;
									if (AABBContains(entity->bounds, actor->position)) {
										game->poweredBoundsEntityId = entity->id;
										break;
									}
								}
							}	else {
								game->poweredBoundsEntityId = sourceEntity->targetEntity;
							}
						} else {
							logf("No entity with id %d\n", sourceActor->sourceEntityId);
						}
					} else {
						logf("No actor with id %d\n", actor->sourcePortalId);
					}
					actor->shouldBeDestoryed = true;
				}
			} else if (actor->type == ACTOR_AMMO) {
				Mesh *mesh = getMesh("assets/models/Items/Items.PistolAmmo.mesh.iqm");
				MeshProps props = newMeshProps();
				props.matrix = actor->matrix;
				drawMesh(mesh, props);

				actor->bounds = actor->matrix.multiply(mesh->bounds);

				// drawBoundsOutline(actor->bounds, 0.1, 0xFFFF0000);
				if (intersects(player->bounds, actor->bounds)) {
					int bullets = actor->value;
					if (bullets == 0) bullets = game->weaponInfos[actor->weaponType].maxBullets;
					logf("+%d bullets\n", bullets);
					player->reserveBullets[actor->weaponType] += bullets;
					actor->shouldBeDestoryed = true;
				}
			} else if (actor->type == ACTOR_PRESSURE_PLATE) {
				Mesh *mesh = getMesh("assets/models/Items/Items.PressurePlate.mesh.iqm");
				MeshProps props = newMeshProps();
				props.matrix = actor->matrix;
				drawMesh(mesh, props);

				actor->bounds = actor->matrix.multiply(mesh->bounds);
				// drawBoundsOutline(actor->bounds, 0.01, 0xFFFF0000);

				bool wasHitting = actor->hitting;
				actor->hitting = false;
				if (intersects(player->bounds, actor->bounds)) {
					actor->hitting = true;
					if (!wasHitting) {
						int rail = 0;
						Actor *sourceActor = getActor(actor->sourcePortalId);
						if (sourceActor) {
							Entity *entity = getEntity(sourceActor->sourceEntityId);
							if (entity) rail = entity->railToPower;
						}

						if (rail) {
							if (game->poweredRailsNum < RAILS_MAX) {
								bool shouldAdd = true;
								for (int i = 0; i < game->poweredRailsNum; i++) {
									if (rail == game->poweredRails[i]) {
										shouldAdd = false;
										break;
									}
								}

								if (shouldAdd) {
									game->poweredRails[game->poweredRailsNum++] = rail;
								}
							}
						}
					}
				}

				if (!wasHitting && actor->hitting) {
					playWorldSound(actor->position, "assets/audio/sounds/pressurePlatePress.ogg");
				}

				if (wasHitting && !actor->hitting) {
					playWorldSound(actor->position, "assets/audio/sounds/pressurePlateRelease.ogg");
				}
			} else if (actor->type == ACTOR_WEAPON) {
				if (!actor->skeleton) {
					char *armPath = frameSprintf("assets/models/ViewModel/ViewModel.%sArm.skele.iqm", weaponTypeStringCaped[actor->weaponType]);
					BaseSkeleton *base = getBaseSkeleton(armPath);
					if (!base) {
						logf("Weapon arm missing: '%s'\n", armPath);
						continue;
					}
					actor->skeleton = deriveSkeleton(base);

					createSkeletonBlend(actor->skeleton, "root", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(actor->skeleton, "idle", SKELETON_BLEND_ANIMATION);
				}

				float weaponScale = 0.5;
				SkeletonBlend *root = getSkeletonBlend(actor->skeleton, "root");
				if (root) {
					Xform xform = toXform(actor->matrix);
					xform.scale *= weaponScale;
					int boneIndex = 0;
					root->controlMask[boneIndex] = true;
					root->poseXforms[boneIndex] = xform;
				}

				updateSkeleton(actor->skeleton, elapsed);

				SkeletonBlend *idle = getSkeletonBlend(actor->skeleton, "idle");
				if (idle) {
					idle->animation = getAnimation(actor->skeleton, "idle");
				}

				char *meshPath = frameSprintf("assets/models/ViewModel/ViewModel.%s.mesh.iqm", weaponTypeStringCaped[actor->weaponType]);
				Mesh *mesh = getMesh(meshPath);
				if (!mesh) {
					logf("Weapon mesh missing: '%s'\n", meshPath);
					continue;
				}
				MeshProps props = newMeshProps();
				props.skeleton = actor->skeleton;
				drawMesh(mesh, props);

				actor->bounds = (actor->matrix.scale(weaponScale) * mesh->bounds);

				if (intersects(player->bounds, actor->bounds)) {
					if (!player->primaryWeapon) {
						player->primaryWeapon = actor->weaponType;
						actor->shouldBeDestoryed = true;
					} else if (!player->secondaryWeapon) {
						player->secondaryWeapon = actor->weaponType;
						player->forceWeaponSwitch = true;
						actor->shouldBeDestoryed = true;
					} else {
						game->playerStandingOn = actor;
					}
				}
			} else if (actor->type == ACTOR_TELEPORTER) {
				Mesh *mesh = getMesh("assets/common/models.Pyramid.mesh.iqm");
				MeshProps props = newMeshProps();
				props.matrix = actor->matrix;
				props.alpha = 0.1;
				drawMesh(mesh, props);

				actor->bounds = actor->matrix.multiply(mesh->bounds);

				if (intersects(actor->bounds, player->bounds)) {
					game->playerStandingOn = actor;
				}
			}

			ActorInfo *aInfo = &game->actorInfos[actor->type];

			if (actor->skeleton) {
				Skeleton *skeleton = actor->skeleton;

				SkeletonBlend *knockBack = getSkeletonBlend(actor->skeleton, "knockBack");
				if (knockBack) {
					for (int i = 0; i < actor->skeleton->base->bonesNum; i++) {
						Xform xform = newXform();
						knockBack->controlMask[i] = true;
						knockBack->poseXforms[i] = lerp(knockBack->poseXforms[i], newXform(), 0.1);
					}
				}

				bool calculateBoundsFromBones = false;
				for (int i = 0; i < skeleton->base->boneInfosNum; i++) {
					BoneInfo *bInfo = &skeleton->base->boneInfos[i];
					if (bInfo->width) {
						calculateBoundsFromBones = true;
						break;
					}
				}

				if (calculateBoundsFromBones) {
					AABB bounds;
					bounds.min = v3(9999, 9999, 9999);
					bounds.max = v3(-9999, -9999, -9999);
					for (int i = 0; i < skeleton->base->bonesNum; i++) {
						Bone *bone = &skeleton->base->bones[i];
						if (bone->parent < 0) continue;
						BoneInfo *bInfo = getBoneInfo(skeleton, bone);
						if (!bInfo) continue;

						Bone *parent = &skeleton->base->bones[bone->parent];
						BoneInfo *parentInfo = getBoneInfo(skeleton, parent);
						if (parentInfo->width <= 0.001) continue;

						Matrix4 boneMat = skeleton->currentTransforms[i] * bone->modelSpaceMatrix;
						Vec3 start = boneMat.getPosition();
						float radius = parentInfo->width;

						Vec3 min = start - radius;
						Vec3 max = start + radius;

						if (bounds.min.x > min.x) bounds.min.x = min.x;
						if (bounds.min.y > min.y) bounds.min.y = min.y;
						if (bounds.min.z > min.z) bounds.min.z = min.z;
						if (bounds.max.x < max.x) bounds.max.x = max.x;
						if (bounds.max.y < max.y) bounds.max.y = max.y;
						if (bounds.max.z < max.z) bounds.max.z = max.z;
					}

					actor->skeletonBounds = bounds;
					actor->bounds = bounds;
				}
			}

			if (aInfo->usesOverrideBounds) {
				actor->bounds.min = v3();
				actor->bounds.max = aInfo->overrideBoundsSize;

				actor->bounds.min += actor->position + aInfo->overrideBoundsOffset;
				actor->bounds.max += actor->position + aInfo->overrideBoundsOffset;
			}

			if (isZero(actor->bounds)) logf("Actor type %s has no bounds\n", actorTypeStrings[actor->type]);

			if (globals->showActorBounds) drawBoundsOutline(actor->bounds, 0.01, 0xFFFFFF00);
			if (globals->showActorSkeletonBounds) drawBoundsOutline(actor->skeletonBounds, 0.01, 0xFFFF00FF);

			for (int i = 0; i < game->actorsNum; i++) {
				Actor *other = &game->actors[i];
				if (!actor->repulses || actor->state == ASTATE_DYING || isZero(actor->bounds)) break;
				if (other == actor) continue;
				if (!other->repulses || other->state == ASTATE_DYING || isZero(other->bounds)) continue;

				if (intersects(actor->bounds, other->bounds)) {
					float dist = actor->position.distance(other->position);

					Vec2 pos2 = v2(actor->position.x, actor->position.y);
					Vec2 other2 = v2(other->position.x, other->position.y);

					Vec2 dir = (pos2 - other2).normalize();

					float pushForce = dist * 0.1;
					actor->velo.x += dir.x * pushForce;
					actor->velo.y += dir.y * pushForce;
				}
			}

			if (actor->hasCollision) actor->accel.z -= .025;
			actor->velo = actor->velo + actor->accel;
			actor->accel = v3();

			Vec3 drag = v3(0.8, 0.8, 0.95);
			actor->velo *= drag;

			float slowPerc = clampMap(actor->slow, 0, 1, 1, 0);
			if (actor->slow > 3) actor->slow = 3;
			if (actor->slow > 0) actor->slow -= 0.01;

			// Vec3 velo = actor->velo * slowPerc;
			if (actor->velo.length() > 2) actor->velo = actor->velo.normalize() * 2;
			// if (actor->type == ACTOR_PLAYER) logf("v: %f\n", velo.length());

			if (actor->hasCollision) {
				int iters = 6;
				actor->isOnGround = false;
				for (int i = 0; i < iters; i++) {
					float perc = 1.0/(float)iters;

					Vec3 oldPosition = actor->position;
					actor->position += actor->velo*perc * game->timeScale;

					for (int i = 0; i < scene->entitiesNum; i++) {
						Entity *entity = &game->entities[i];
						if (!entity->isCollider) continue;
						if (!intersects(inflate(actor->bounds, actor->velo.length()+5), entity->bounds)) continue;
						if (!entity->powered) continue;

						Vec3 size = getSize(actor->bounds);

						Vec3 capStart = entity->invMatrix * actor->position;
						Vec3 capEnd = capStart;
						capEnd.z += size.z;

						float capRadius = (size.x + size.y) / 4.0;
						capStart.z += capRadius;
						capEnd.z -= capRadius;
						AABB bounds = capsuleToAABB(capStart, capEnd, capRadius);
						// drawCapsule(capStart, capEnd, capRadius, 0xFFFF0000);

						Mesh *mesh = getMesh(entity->path);
						if (!mesh) continue;
						if (!mesh->triBins) generateTriangleBins(mesh);

						int *indsToCheck = (int *)frameMalloc(sizeof(int) * mesh->triBinsNum);
						int indsToCheckNum = getTriBinIndices(mesh, bounds, indsToCheck, mesh->triBinsNum);

						for (int i = 0; i < indsToCheckNum; i++) {
							TriBin *bin = &mesh->triBins[indsToCheck[i]];
							for (int i = 0; i < bin->trisNum; i++) {
								MeshTri *meshTri = bin->tris[i];
								Triangle tri;
								tri.verts[0] = meshTri->verts[0];
								tri.verts[1] = meshTri->verts[1];
								tri.verts[2] = meshTri->verts[2];

								IntersectionResult result = capsuleIntersectsTriangle(capStart, capEnd, capRadius, tri);
								if (result.length == -1) continue;
								if (result.penetrationDepth <= 0) continue;
								// drawBeam(tri.verts[0], tri.verts[1], 0.1, 0xFF00FF00);
								// drawBeam(tri.verts[1], tri.verts[2], 0.1, 0xFF00FF00);
								// drawBeam(tri.verts[2], tri.verts[0], 0.1, 0xFF00FF00);
								// drawSphere(result.calcCenter - result.penetrationNormal*result.length, 0.1, 0xFF00FF00);

								// Modify player velocity to slide on contact surface:
								Vec3 normal = result.penetrationNormal.normalize();
								float depth = result.penetrationDepth;

								bool groundHit = false;
								float slope = normal.dot(v3(0, 0, 1));
								if (slope > 0.8 && actor->velo.z < 0) groundHit = true;

								{
									float velocity_length = actor->velo.length();
									if (velocity_length == 0) continue;
									Vec3 velocity_normalized = actor->velo / velocity_length;
									// logf("%f\n", velocity_length);
									Vec3 undesired_motion = normal * velocity_normalized.dot(normal);
									Vec3 desired_motion = velocity_normalized - undesired_motion;
									actor->velo = desired_motion * velocity_length;
								}
								if (groundHit) {
									actor->isOnGround = true;
									actor->velo.z = 0;
								}

								// Remove penetration (penetration epsilon added to handle infinitely small penetration):
								Vec3 bump = (normal * (depth + 0.001));
								// if (groundHit) {
								// 	bump.x = 0;
								// 	bump.y = 0;
								// }
								capStart += bump;
								capEnd += bump;
								bounds.min += bump;
								bounds.max += bump;
								actor->position += bump;
								// drawSphere(result.calcCenter, capRadius, 0xFF00FF00);
							}
						}
					}

					if (actor->isOnGround) actor->groundMoveDistance += oldPosition.distance(actor->position);
				}

				actor->offGroundTime += elapsed;
				if (actor->isOnGround) actor->offGroundTime = 0;
			} else {
				actor->position += actor->velo;
			}

			actor->shotTime += elapsed;
			actor->stateTime += elapsed;
			actor->timeSinceLastHit += elapsed;
			actor->timeSinceLastDashHit += elapsed;
			// actor->position = actor->position.add(actorMoveDistance);

			// if (player) {
			// 	Matrix4 projectionView = defaultWorld->projectionMatrix * defaultWorld->viewMatrix;
			// 	if (frustumContains(projectionView, actor->position)) {
			// 		actor->lastIn += elapsed;
			// 	} else {
			// 		actor->lastIn = 0;
			// 	}
			// 	// Vec3 viewVec = quaternionToEuler(quaternionBetween(player->position, actor->position));
			// 	log3f(actor->position, "%f\n", actor->lastIn);
			// }

		}
	} ///

	{ /// Destroy actors
		for (int i = 0; i < game->actorsNum; i++) {
			Actor *actor = &game->actors[i];
			if (!actor->shouldBeDestoryed) continue;
			arraySpliceIndex(game->actors, game->actorsNum, sizeof(Actor), i);
			i--;
			game->actorsNum--;
			continue;
		}
	} ///

	{ /// Update entities
		for (int i = 0; i < scene->entitiesNum; i++) {
			Entity *entity = &game->entities[i];

			entity->powered = false;

			if (entity->needsPower) {
				if (AABBContains(poweredBounds, entity->position)) entity->powered = true;
			} else {
				entity->powered = true;
			}

			Matrix4 mat = mat4();
			mat = mat.translate(entity->position);
			mat = mat.rotateQuaternion(eulerToQuaternion(entity->rotation));
			mat = mat.scale(entity->scale);
			entity->matrix = mat;
			entity->invMatrix = mat.invert();
			entity->bounds = entity->matrix * makeAABB(v3(-1, -1, -1), v3(1, 1, 1));

			{ /// Figure out bounds
				Mesh *mesh = getMesh(entity->path);
				if (mesh) {
					entity->bounds = entity->matrix * mesh->bounds;
				}
			} ///

			if (entity->type == ENTITY_PORTAL_SPAWNER) {
				if (game->sceneFrames == 0) {
					Actor *actor = createActor(ACTOR_PORTAL);
					actor->typeToSpawn = entity->actorType;
					actor->spawnCountMax = entity->spawnCountMax;
					actor->spawnInterval = entity->spawnInterval;
					actor->weaponType = entity->weaponType;

					actor->position = entity->position;

					actor->spawnPosition = entity->position;
					actor->spawnRotation = entity->rotation;
					actor->spawnScale = entity->scale;

					Actor *closestActor;
					Entity *closestEntity;
					float rayDist;
					if (rayTestActorsAndEntities(entity->position, v3(0, 0, -1), &closestActor, &closestEntity, &rayDist, NULL, true, false)) {
						Vec3 hit = entity->position + v3(0, 0, -1)*rayDist;
						actor->position = hit;
					}

					actor->sourceEntityId = entity->id;

					if (actor->typeToSpawn == ACTOR_START) entity->alwaysPowered = true;
				}
			} else if (entity->type == ENTITY_LASER_WALL) {
				Mesh *mesh = getMesh("assets/models/items/items.Wall.mesh.iqm");
				MeshProps props = newMeshProps();
				props.matrix = entity->matrix;
				drawMesh(mesh, props);

				entity->bounds = entity->matrix * mesh->bounds;
			} else if (entity->type == ENTITY_SOUND_EMITTER) {
				if (game->inEditor) {
					if (keyJustPressed('J')) playWorldSound(entity->position, "assets/audio/sounds/sine.ogg");
					drawSphere(entity->position, 2, 0xFF000000);
				}
			}

			if (strstr(entity->name, "Sun")) {
				world->sunPosition = entity->position;
			}

			MeshProps props = newMeshProps();
			props.matrix = entity->matrix;
			props.tint = entity->tint;
			props.alpha = entity->alpha;

			bool shouldDraw = true;
			if (entity->hiddenInGame && !game->inEditor) {
				shouldDraw = false;
			}

			if (entity->hiddenInEditor && game->inEditor) {
				shouldDraw = false;
			}

			if (shouldDraw) {
				if (entity->type == ENTITY_DEBUG_SPHERE) {
					Mesh *sphereMesh = getMesh("assets/common/models.Sphere.mesh.iqm");
					drawMesh(sphereMesh, props);
				} else if (entity->type == ENTITY_CAMERA_POSITION) {
					if (game->inEditor) {
						Mesh *cameraMesh = getMesh("assets/common/models.Camera.mesh.iqm");
						drawMesh(cameraMesh, props);
					}
				} else if (entity->type == ENTITY_MODEL) {
					Mesh *mesh = getMesh(entity->path);
					if (mesh && entity->powered) {
						drawMesh(mesh, props);
					}
				} else if (entity->type == ENTITY_MARKER) {
					if (game->inEditor) {
						Mesh *pyramidMesh = getMesh("assets/common/models.Pyramid.mesh.iqm");
						MeshProps props = newMeshProps();
						props.matrix = entity->matrix;
						props.matrix.SCALE(0.1, 0.1, 1.0);
						props.tint = 0xFF00FF00;
						drawMesh(pyramidMesh, props);
						log3f(entity->matrix.multiply(v3()), "Marker: %s", entity->name);
					}
				} else if (entity->type == ENTITY_PORTAL_SPAWNER) {
					bool shouldDraw = true;
					int tint = 0xFF000000;
					if (entity->actorType == ACTOR_PLAYER) shouldDraw = false;
					if (entity->actorType == ACTOR_START) shouldDraw = false;
					if (entity->alwaysPowered) shouldDraw = false;
					if (entity->actorType == ACTOR_WEAPON) tint = 0xFF00FF00;
					if (entity->actorType == ACTOR_AMMO) tint = 0xFF00FF00;
					if (entity->actorType == ACTOR_END) tint = 0xFF0000FF;

					if (shouldDraw) {
						Mesh *sphereMesh = getMesh("assets/common/models.Sphere.mesh.iqm");
						MeshProps props = newMeshProps();
						Matrix4 mat = mat4();
						props.alpha = 0.05;
						props.tint = tint;
						props.matrix = entity->matrix;
						drawMesh(sphereMesh, props);
					}

					if (game->inEditor) {
						Mesh *pyramidMesh = getMesh("assets/common/models.Pyramid.mesh.iqm");
						MeshProps props = newMeshProps();
						props.matrix = entity->matrix;
						props.matrix.SCALE(0.1, 0.1, 1.0);
						props.tint = 0xFFFF0000;
						drawMesh(pyramidMesh, props);
						log3f(entity->matrix.multiply(v3()), "Spawner: %s", entity->name);
					}
				} else if (entity->type == ENTITY_BOUNDS) {
					if (game->inEditor) {
						drawBoundsOutline(entity->bounds, 0.1, 0xFF0000FF);
					}
				}
			}
		}
	} ///

	{ /// Update bullets
		for (int i = 0; i < game->bulletsNum; i++) {
			Bullet *bullet = &game->bullets[i];
			bool shouldDestroy = false;

			Vec3 oldPosition = bullet->position;
			bullet->position += bullet->direction * bullet->speed;
			drawCapsule(oldPosition, bullet->position, 0.5, 0xFFFF0000);
			if (!bullet->ally) {
				if (overlaps(player->bounds, makeLine3(oldPosition, bullet->position))) {
					// createWorldText(oldPosition, "old");
					// createWorldText(bullet->position, "current");
					// player->bounds.min.print("min");
					// player->bounds.max.print("max");
					shouldDestroy = true;
					dealDamage(player, 5, NULL);
				}
			}

			if (!testLineOfSight(oldPosition, bullet->position)) shouldDestroy = true;

			SoundSource *soundSource = getSoundSource(bullet->soundSourceId);
			if (soundSource) setPosition(soundSource, bullet->position);

			drawSphere(bullet->position, 0.5, 0xFF00FF00);

			if (bullet->time > 60) shouldDestroy = true;

			if (shouldDestroy) {
				if (soundSource) stop(soundSource);
				arraySpliceIndex(game->effects, game->bulletsNum, sizeof(Effect), i);
				i--;
				game->bulletsNum--;
				continue;
			}

			bullet->time += elapsed;
		}
	} ///

	if (game->usingInfoTool) {
		Vec2 screenPoint = v2(platform->windowWidth/2, platform->windowHeight/2);
		Vec3 rayWorld = getRayFromSceenSpace(screenPoint);
		Vec3 rayStart = game->realCameraPosition;
		float dist = 9999;
		Vec3 rayEnd = rayStart + rayWorld*dist;

		Actor *closestActor = NULL;
		float closestDist = NULL;
		AABB closestBounds;
		for (int i = 0; i < game->actorsNum; i++) {
			Actor *actor = &game->actors[i];
			if (actor->type == ACTOR_PLAYER) continue;

			AABB bounds = actor->bounds;
			if (isZero(bounds)) continue;

			Vec3 hitPoint;
			if (overlaps(bounds, makeLine3(rayStart, rayEnd), &hitPoint)) {
				float dist = rayStart.distance(hitPoint);
				if (!closestActor || dist < closestDist) {
					closestActor = actor;
					closestDist = dist;
					closestBounds = bounds;
				}
			}
		}

		if (closestActor) {
			rayEnd = rayStart + rayWorld*closestDist;
			log3f(rayEnd, "%s", actorTypeStrings[closestActor->type]);
			drawBoundsOutline(closestBounds, 0.1, 0xFFFF0000);
		}

		Vec3 cameraOffset = game->cameraMatrix.invert().multiplyAffine(v3(-1, 0, 0));
		drawCapsule(rayStart+cameraOffset, rayEnd, 0.1, 0xFFFF0000);

		// for (int i = 0; i < game->actorsNum; i++) {
		// 	Actor *actor = &game->actors[i];
		// 	if (!actor->skeleton) continue;
		// 	Skeleton *skeleton = actor->skeleton;
		// 	for (int i = 0; i < skeleton->base->bonesNum; i++) {
		// 		Bone *bone = &skeleton->base->bones[i];
		// 		if (bone->parent < 0) continue;
		// 		BoneInfo *bInfo = getBoneInfo(skeleton, bone);
		// 		if (!bInfo) continue;

		// 		Bone *parent = &skeleton->base->bones[bone->parent];
		// 		BoneInfo *parentInfo = getBoneInfo(skeleton, parent);
		// 		if (parentInfo->width <= 0.001) continue;

		// 		Matrix4 boneMat = skeleton->currentTransforms[i] * bone->modelSpaceMatrix;
		// 		Matrix4 parentBoneMat = skeleton->currentTransforms[bone->parent] * parent->modelSpaceMatrix;
		// 		Vec3 start = boneMat.getPosition();
		// 		Vec3 end = parentBoneMat.getPosition();
		// 		float dist = rayTestCapsule(rayStart, rayWorld, start, end, parentInfo->width);
		// 		if (dist != -1) {
		// 			drawCapsule(start, end, parentInfo->width, 0xFFFF0000);
		// 		} else {
		// 			drawCapsule(start, end, parentInfo->width, 0xFF00FF00);
		// 		}
		// 	}
		// }
	}

	if (globals->showNavMeshTris) {
		if (playerMeshTri) {
			MeshTri *tri = playerMeshTri;
			drawBeam(tri->verts[0], tri->verts[1], 0.3, 0xFF0000FF);
			drawBeam(tri->verts[1], tri->verts[2], 0.3, 0xFF0000FF);
			drawBeam(tri->verts[2], tri->verts[0], 0.3, 0xFF0000FF);
		}

		Entity *entity = getEntity(game->currentNavMeshEntityId);
		if (entity) {
			Mesh *mesh = getMesh(entity->path);
			if (mesh) {
				for (int i = 0; i < mesh->meshTrisNum; i++) {
					MeshTri *tri = &mesh->meshTris[i];
					Vec3 center = (tri->verts[0] + tri->verts[1] + tri->verts[2]) / 3;
					drawBeam(tri->verts[0], tri->verts[1], 0.1, 0xFFFF0000);
					drawBeam(tri->verts[1], tri->verts[2], 0.1, 0xFFFF0000);
					drawBeam(tri->verts[2], tri->verts[0], 0.1, 0xFFFF0000);
					drawSphere(center, 0.1, 0xFFFF0000);
					for (int i = 0; i < 3; i++) {
						if (tri->adjs[i] == -1) continue;
						MeshTri *otherTri = &mesh->meshTris[tri->adjs[i]];
						Vec3 otherCenter = (otherTri->verts[0] + otherTri->verts[1] + otherTri->verts[2]) / 3;
						// drawBeam(center, otherCenter, 0.2, 0xFF00FF00);
					}
				}
			}
		}
	}

	if (globals->showHitCapsules) {
		if (globals->showHitCapsulesOnTop) glClear(GL_DEPTH_BUFFER_BIT);
		u32 colors[] = {
			0x88FF0000,
			0x8800FF00,
			0x880000FF,
			0x88FFFF00,
			0x8800FFFF,
			0x88FF00FF,
			0xFFFF0000,
			0xFF00FF00,
			0xFF0000FF,
			0xFFFFFF00,
			0xFF00FFFF,
			0xFFFF00FF,
		};

		for (int i = 0; i < game->actorsNum; i++) {
			Actor *actor = &game->actors[i];
			if (!actor->skeleton) continue;
			Skeleton *skeleton = actor->skeleton;
			for (int i = 0; i < skeleton->base->bonesNum; i++) {
				Bone *bone = &skeleton->base->bones[i];
				BoneInfo *bInfo = getBoneInfo(skeleton, bone);
				if (!bInfo) continue;
				Matrix4 boneMat = skeleton->currentTransforms[i] * bone->modelSpaceMatrix;

				if (bone->parent >= 0) {
					Bone *parent = &skeleton->base->bones[bone->parent];
					BoneInfo *parentInfo = getBoneInfo(skeleton, parent);
					if (parentInfo->width <= 0.001) continue;

					Matrix4 parentBoneMat = skeleton->currentTransforms[bone->parent] * parent->modelSpaceMatrix;
					int color = colors[i%ArrayLength(colors)];
					Vec3 start = boneMat.getPosition();
					Vec3 end = parentBoneMat.getPosition();
					drawCapsule(start, end, parentInfo->width, color);
				}
			}
		}
	}

	// glClear(GL_DEPTH_BUFFER_BIT);

	if (hasViewModel) {
		MeshProps props = newMeshProps();
		props.skeleton = viewModelSkeleton;
		props.layer = 1;
		drawMesh(viewModelMesh, props);

		{
			char *gunMeshPath = frameSprintf("assets/models/ViewModel/ViewModel.%s.mesh.iqm", weaponTypeStringCaped[player->weaponType]);
			Mesh *gunMesh = getMesh(gunMeshPath);
			if (gunMesh) {
				MeshProps props = newMeshProps();
				props.skeleton = game->gunSkeleton;
				props.matrix = gunMeshMatrix;
				props.layer = 1;
				drawMesh(gunMesh, props);
			}
		}
	}

	{ /// Hud
		if (player) {
			{ /// Hp
				Rect rect = makeRect(0, 0, 1024, 32);
				rect.x = game->gameWidth/2 - rect.width/2;
				rect.y = 2;
				Rect bgRect = rect.inflate(5);
				rect.width *= player->hp / 100.0;

				draw2dRect(bgRect, 0xFF008800);
				int color = 0xFF00FF00;
				if (player->hp < 50) color = 0xFFFFFF00;
				if (player->hp < 25) color = lerpColor(0xFFFF0000, 0xFFCC0000, secondPhase);
				draw2dRect(rect, color);
			} ///
			{ /// Ammo
				char *str = frameSprintf("%d/%d", player->bullets[player->weaponType], player->reserveBullets[player->weaponType]);
				Vec2 textSize = getTextSize(game->defaultFont, str);
				Vec2 pos;
				pos.x = game->gameWidth - textSize.x;
				pos.y = game->gameHeight - textSize.y;
				int color = 0xFFFFFFFF;
				draw2dText(game->defaultFont, str, pos, color);
			} ///

			{ /// Aimer
				WeaponInfo *wInfo = &game->weaponInfos[player->weaponType];
				Vec2 screenCenter;
				screenCenter.x = game->gameWidth/2;
				screenCenter.y = game->gameHeight/2;

				game->aimerRect.width = wInfo->aimerSize.x;
				game->aimerRect.height = wInfo->aimerSize.y;
				game->aimerRect.x = screenCenter.x - game->aimerRect.width/2;
				game->aimerRect.y = screenCenter.y - game->aimerRect.height/2;
				draw2dRectOutline(game->aimerRect, 1, 0x44000000);

				Actor *aimedActor = NULL;
				float aimedActorDist;
				Matrix4 projectionView = defaultWorld->projectionMatrix * defaultWorld->viewMatrix;
				for (int i = 0; i < game->actorsNum; i++) {
					Actor *actor = &game->actors[i];
					if (!actor->hostile) continue;
					if (actor->hp <= 0) continue;
					if (!frustumContains(projectionView, getCenter(actor->skeletonBounds))) continue;

					Rect rect = worldSpaceAABBToScreenSpaceRect(actor->skeletonBounds);
					if (!rect.intersects(game->aimerRect)) continue;
					if (!testLineOfSight(game->realCameraPosition, getCenter(actor->skeletonBounds))) continue;

					Vec2 center = getCenter(rect);
					float dist = getCenter(player->bounds).distance(getCenter(actor->skeletonBounds));
					if (!aimedActor || dist < aimedActorDist) {
						aimedActor = actor;
						aimedActorDist = dist;
					}
				}

				game->aimerTarget = v3();
				Vec2 target = screenCenter;
				if (aimedActor) {
					Rect rect = worldSpaceAABBToScreenSpaceRect(aimedActor->skeletonBounds);
					draw2dRectOutline(rect, 1, 0xFFFF0000);
					target.x = rect.x + rect.width/2;
					target.y = rect.y + rect.height * 0.25;

					game->aimerTarget = getCenter(aimedActor->skeletonBounds);
				}

				game->aimerCursor = lerp(game->aimerCursor, target, wInfo->aimerSpeed);
				if (!game->aimerRect.contains(game->aimerCursor)) {
					game->aimerCursor = game->aimerRect.getClosestPoint(game->aimerCursor);
				}
			} ///
		}

		{
			char *str = frameSprintf(
				"Frame time: %dms\nRay count: %d\nAABBLine count: <removed>\nbulletsNum: %d\nDraw calls:%d\nActors: %d/%d\n",
				platform->frameTime,
				rayCount,
				game->bulletsNum,
				meshSys->lastDrawCalls,
				game->actorsNum, ACTORS_MAX
			);

			Vec2 textSize = getTextSize(game->defaultFont, str);

			Vec2 pos = v2();
			pos.x = game->gameWidth - textSize.x*2;
			draw2dText(game->bigFont, str, pos);
			rayCount = 0;
		}

		if (!game->inEditor) {
			{ /// Crosshair
				Texture *texture = getTexture("assets/images/crosshair.png");
				if (!texture) logf("Crosshair missing\n");

				if (texture) {
					Matrix3 mat = mat3();
					mat = mat.translate(game->gameWidth/2 - texture->width/2, game->gameHeight/2 - texture->height/2);

					RenderProps2d props = newRenderProps2d();
					props.matrix = mat;
					draw2dTexture(texture, props);
				}
			}

			{ /// Aimer cusror
				WeaponInfo *wInfo = &game->weaponInfos[0];
				if (player) wInfo = &game->weaponInfos[player->weaponType];
				Texture *texture = getTexture("assets/images/crosshair.png");
				if (texture) {
					Matrix3 mat = mat3();
					mat = mat.translate(game->aimerCursor.x, game->aimerCursor.y);
					mat = mat.scale(2, 2);
					mat = mat.rotate(game->time * clampMap(wInfo->aimerSpeed, 0, 1, 10, 100));
					mat = mat.translate(-texture->width/2, -texture->height/2);

					float isDiff = game->aimerCursor.distance(v2(game->gameWidth/2, game->gameHeight/2)) > 0.1;

					RenderProps2d props = newRenderProps2d();
					if (isDiff) {
						props.tint = lerpColor(0x44FF0000, 0xFFFF0000, secondPhase);
					} else {
						props.tint = 0x44FF0000;
					}
					props.matrix = mat;
					draw2dTexture(texture, props);
				}
			}
		}
	}

	{ /// Update effects
		// glEnable(GL_BLEND);
		for (int i = 0; i < game->effectsNum; i++) {
			Effect *effect = &game->effects[i];
			bool shouldDestroy = false;

			if (effect->type == EFFECT_WORLD_TEXT) {
				log3f(effect->position, effect->valueString);
				if (effect->time > 3) shouldDestroy = true;
			}

			if (shouldDestroy) {
				arraySpliceIndex(game->effects, game->effectsNum, sizeof(Effect), i);
				i--;
				game->effectsNum--;
				continue;
			}

			effect->time += elapsed;
		}
		// glDisable(GL_BLEND);
	} ///

	if (keyJustPressed(KEY_BACKTICK)) {
		game->inEditor = !game->inEditor;
		// game->justOpenedEditor = true;
	}

	if (game->inEditor) {
		if (keyJustPressed(' ')) {
			game->usingEditorCamera = !game->usingEditorCamera;
			platform->mouse.x = platform->windowWidth/2;
			platform->mouse.y = platform->windowHeight/2;
#if defined(_WIN32)
			POINT winPoint;
			winPoint.x = platform->mouse.x;
			winPoint.y = platform->mouse.y;
			if (ClientToScreen(platform->hwnd, &winPoint)) {
				SetCursorPos(winPoint.x, winPoint.y);
			}
#endif
		}

		if (rightMouseJustDown) {
			if (!game->editorCursorEntityRayTesting) {
				logf("Click to select\n");
				game->editorCursorEntityRayTesting = true;
				game->editorRaySelecting = true;
			} else {
				logf("Cancelled...\n");
				game->editorCursorEntityRayTesting = false;
			}
		}

		if (mouseWheel < 0) {
			game->realCameraSpeed -= 0.1;
			logf("Speed: %f\n", game->realCameraSpeed);
		}

		if (mouseWheel > 0) {
			game->realCameraSpeed += 0.1;
			logf("Speed: %f\n", game->realCameraSpeed);
		}
		if (game->realCameraSpeed < 0.1) game->realCameraSpeed = 0.1;

		if (game->editorCursorEntityRayTesting) {
			Vec3 rayWorld = getRayFromSceenSpace(platform->mouse);

			float closestRayDist = 0;
			Entity *closestEntity = NULL;
			MeshTri *closestTri = NULL;
			for (int i = 0; i < scene->entitiesNum; i++) {
				Entity *entity = &game->entities[i];
				if (entity->hiddenInEditor) continue;
				float rayDist = 0;
				MeshTri *meshTri;
				if (rayTestEntity(entity, game->realCameraPosition, rayWorld, &rayDist, &meshTri)) {
					if (!closestRayDist || rayDist < closestRayDist) {
						closestRayDist = rayDist;
						closestEntity = entity;
						closestTri = meshTri;
					}
				}
			}

			bool hit = closestRayDist != 0;

			int color = 0xFF0000FF;
			if (!hit) {
				closestRayDist = 100;
				color = 0xFFFF0000;
			}
			Vec3 end = game->realCameraPosition.add(rayWorld.multiply(closestRayDist));
			// drawBeam(game->realCameraPosition + v3(0, 0, -0.5), end, 0.1, 0xFFFF0000);

			if (closestEntity) {
				log3f(end, "%s\n", closestEntity->name);
				drawBoundsOutline(closestEntity->bounds, 0.1, 0xFFFF0000);

				if (closestTri) {
					MeshTri *meshTri = closestTri;
					drawBeam(meshTri->verts[0], meshTri->verts[1], 0.1, 0xFFFF0000);
					drawBeam(meshTri->verts[1], meshTri->verts[2], 0.1, 0xFFFF0000);
					drawBeam(meshTri->verts[2], meshTri->verts[0], 0.1, 0xFFFF0000);
				}
			}

			if (mouseJustUp) {
				game->editorCursorEntityRayTesting = false;
				if (hit) {
					if (game->editorRayCursorMoving) {
						game->editorCursorPosition = end;
						game->editorRayCursorMoving = false;
					}
					if (game->editorRaySelecting) {
						game->selectedEntityId = closestEntity->id;
						game->editorRaySelecting = false;
					}
				} else {
					logf("Missed...\n");
				}
			}
		}

		{ /// Draw editor cursor
			Matrix4 mat = mat4();
			mat = mat.translate(game->editorCursorPosition);
			mat = mat.rotateQuaternion(eulerToQuaternion(game->editorCursorRotation));

			MeshProps props = newMeshProps();
			props.matrix = mat;
			props.tint = lerpColor(0x00FF00FF, 0xFFFF00FF, secondPhase);

			Matrix4 pyramidMat = mat.scale(.2, .2, .2);
			props.matrix = pyramidMat;
			drawMesh(getMesh("assets/common/models.Pyramid.mesh.iqm"), props);
			props.matrix = mat;

			Mesh *torusMesh = getMesh("assets/common/models.ThinTorus.mesh.iqm");
			drawMesh(torusMesh, props);

			mat = mat.rotateEuler(-M_PI/2, 0, 0);
			drawMesh(torusMesh, props);

			mat = mat.rotateEuler(0, -M_PI/2, 0);
			drawMesh(torusMesh, props);
		} ///

		Vec2 nextWindowPos = v2();
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
		ImGui::Begin("Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		char *scenePath = frameSprintf("assets/scenes/%sScene.txt", scene->name);
		char *entitiesPath = frameSprintf("assets/scenes/%sEntities.txt", scene->name);
		if (ImGui::Button("Save")) {
			saveStruct("Scene", scenePath, scene);
			saveStructArray("Entity", entitiesPath, game->entities, scene->entitiesNum, sizeof(Entity));

			for (int i = 0; i < scene->entitiesNum; i++) {
				Entity *entity = &game->entities[i];
				if (entity->type == ENTITY_PORTAL_SPAWNER && entity->actorType == ACTOR_AMMO) {
					if (entity->weaponType == WEAPON_NONE) {
						logf("Ammo with no weapon type...\n");
					}
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load")) {
			loadStruct("Scene", scenePath, scene); //@copyPastedLoadLevel
			loadStructArray("Entity", entitiesPath, game->entities, ENTITIES_MAX, sizeof(Entity));
		}

		ImGui::InputText("Name", scene->name, SCENE_NAME_MAX_LEN);

		nextWindowPos.y += ImGui::GetWindowHeight();
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0, nextWindowPos.y), ImGuiCond_Always, ImVec2(0, 0));
		ImGui::Begin("Controls", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Checkbox("Fps camera", &game->usingEditorCamera);
		ImGui::SliderFloat("timeScale", &game->timeScale, 0.01, 1);
		if (ImGui::Button("cursor -> camera")) {
			game->editorCursorPosition = game->realCameraPosition;
			game->editorCursorRotation = game->realCameraRotation.multiply(-1);
		}

		if (ImGui::Button("cursor -> entity")) {
			Entity *entity = getEntity(game->selectedEntityId);
			if (entity) {
				game->editorCursorPosition = entity->position;
				game->editorCursorRotation = entity->rotation;
			} else {
				logf("No selected entity\n");
			}
		}

		if (ImGui::Button("cursor -> ray test")) {
			game->editorCursorEntityRayTesting = true;
			game->editorRayCursorMoving = true;
		}
		if (game->editorCursorEntityRayTesting) {
			ImGui::SameLine();
			ImGui::Text("Click the scene");
		}

		ImGui::Separator();

		if (ImGui::Button("camera -> cursor")) {
			game->realCameraPosition = game->editorCursorPosition;
			game->realCameraRotation = game->editorCursorRotation.multiply(-1);
		}

		ImGui::Separator();

		if (ImGui::Button("entity -> cursor")) {
			Entity *entity = getEntity(game->selectedEntityId);
			if (entity) {
				entity->position = game->editorCursorPosition;
				entity->rotation = game->editorCursorRotation;
			} else {
				logf("No selected entity\n");
			}
		}

		ImGui::DragFloat3("editorCursorPosition", &game->editorCursorPosition.x);
		ImGui::DragFloat3("editorCursorRotation", &game->editorCursorRotation.x);

		ImGui::Separator();
		ImGui::Separator();
		ImGui::Separator();
		if (ImGui::Button("Select ray test")) {
			game->editorCursorEntityRayTesting = true;
			game->editorRaySelecting = true;
		}
		if (ImGui::Button("Nav mode")) {
			game->inNavMode = !game->inNavMode;
		}
		if (game->inNavMode) {
			ImGui::SameLine();
			ImGui::Text("In nav mode");
		}

		if (ImGui::Button("Clear mesh cache")) {
			while (meshSys->meshesNum > 0) destroyMesh(&meshSys->meshes[0]);
		}

		nextWindowPos.x += ImGui::GetWindowWidth();
		nextWindowPos.y += ImGui::GetWindowHeight();

		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(nextWindowPos.x, 0), ImGuiCond_Always, ImVec2(0, 0));
		ImGui::Begin("Other", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::TreeNode("Globals")) {
			if (ImGui::Button("Save")) saveStruct("Globals", "assets/info/globals.txt", globals);
			ImGui::SameLine();
			if (ImGui::Button("Load")) loadStruct("Globals", "assets/info/globals.txt", globals);

			ImGui::Separator();

			ImGui::DragFloat("resolutionPerc", &globals->resolutionPerc, 0.01);
			ImGui::SliderFloat("sensitivity", &globals->sensitivity, 0, 10);
			ImGui::Checkbox("disableCulling", &meshSys->disableCulling);

			ImGui::Separator();

			ImGui::Separator();
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("BoneInfo", ImGuiTreeNodeFlags_DefaultOpen&0)) {
			ImGui::Checkbox("Show hit capsules", &globals->showHitCapsules);
			if (globals->showHitCapsules) ImGui::Checkbox("Show on top", &globals->showHitCapsulesOnTop);
			if (ImGui::Button("Save all")) {
				for (int i = 0; i < skeletonSys->baseSkeletonsNum; i++) {
					BaseSkeleton *base = &skeletonSys->baseSkeletons[i];
					saveStructArray("BoneInfo", base->infoPath, base->boneInfos, base->boneInfosNum, sizeof(BoneInfo));
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load all")) {
				for (int i = 0; i < skeletonSys->baseSkeletonsNum; i++) {
					BaseSkeleton *base = &skeletonSys->baseSkeletons[i];
					loadStructArray("BoneInfo", base->infoPath, base->boneInfos, base->boneInfosNum, sizeof(BoneInfo));
				}
			}

			for (int i = 0; i < skeletonSys->baseSkeletonsNum; i++) {
				BaseSkeleton *base = &skeletonSys->baseSkeletons[i];

				if (ImGui::TreeNode(base->name)) {
					for (int i = 0; i < base->boneInfosNum; i++) {
						ImGui::PushID(i);
						BoneInfo *bInfo = &base->boneInfos[i];
						ImGui::Text(bInfo->name);
						ImGui::SameLine();
						ImGui::SliderFloat("Width", &bInfo->width, 0, 2);
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("ActorInfo", ImGuiTreeNodeFlags_DefaultOpen&0)) {
			if (ImGui::Button("Save")) {
				saveStructArray("ActorInfo", "assets/info/actorInfos.txt", game->actorInfos, ACTOR_TYPES_MAX, sizeof(ActorInfo));
			}
			ImGui::SameLine();
			if (ImGui::Button("Load")) {
				loadStructArray("ActorInfo", "assets/info/actorInfos.txt", game->actorInfos, ACTOR_TYPES_MAX, sizeof(ActorInfo));
				for (int i = 0; i < ACTOR_TYPES_MAX; i++) { //@copyPastedRefreshActorInfos
					game->actorInfos[i].name = (char *)actorTypeStrings[i];
				}
			}

			for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
				ActorType type = (ActorType)i;
				ActorInfo *aInfo = &game->actorInfos[type];

				if (ImGui::TreeNode(aInfo->name)) {
					ImGui::Checkbox("usesOverrideBounds", &aInfo->usesOverrideBounds);
					ImGui::DragFloat3("overrideBoundsSize", &aInfo->overrideBoundsSize.x, 0.01);
					ImGui::DragFloat3("overrideBoundsOffset", &aInfo->overrideBoundsOffset.x, 0.01);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		ImGui::Checkbox("godMode", &globals->godMode);
		ImGui::Checkbox("superSpeed", &globals->superSpeed);
		ImGui::Checkbox("showNavMeshTris", &globals->showNavMeshTris);
		ImGui::Checkbox("showActorBounds", &globals->showActorBounds);
		ImGui::Checkbox("showActorSkeletonBounds", &globals->showActorSkeletonBounds);

		if (ImGui::Button("Bring player")) {
			player->position = game->realCameraPosition;
		}

		if (ImGui::Button("Get ammo")) {
			for (int i = 0; i < WEAPONS_MAX; i++) {
				player->bullets[i] = game->weaponInfos[i].maxBullets;
			}
		}

		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth, 0), ImGuiCond_Always, ImVec2(1, 0));
		ImGui::Begin("Entities", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Entities:");
		ImGui::BeginChild("Entities", ImVec2(0, 200), false, 0); 
		for (int i = 0; i < scene->entitiesNum; i++) {
			Entity *entity = &game->entities[i];
			ImGui::PushID(i);

			char *name = entity->name;
			if (ImGui::Selectable(name, game->selectedEntityId == entity->id)) {
				game->selectedEntityId = entity->id;
			}
			ImGui::PopID();
		}
		ImGui::EndChild();

		if (ImGui::Button("Create entity")) {
			if (scene->entitiesNum < ENTITIES_MAX) {
				Entity *entity = &game->entities[scene->entitiesNum++];
				memset(entity, 0, sizeof(Entity));
				entity->id = ++scene->nextEntityId;
				entity->position = game->editorCursorPosition;
				entity->rotation = game->editorCursorRotation;
				entity->scale = v3(1, 1, 1);
				entity->alpha = 1;
				game->selectedEntityId = entity->id;
			} else {
				logf("Too many game props\n");
			}
		}

		Entity *selectedEntity = NULL;
		for (int i = 0; i < scene->entitiesNum; i++) {
			Entity *entity = &game->entities[i];
			if (entity->id == game->selectedEntityId) {
				selectedEntity = entity;
				break;
			}
		}

		if (selectedEntity) {
			int selectedIndex = -1;
			for (int i = 0; i < scene->entitiesNum; i++) {
				Entity *entity = &game->entities[i];
				if (selectedEntity->id == entity->id) {
					selectedIndex = i;
					break;
				}
			}

			if (ImGui::Button("Duplicate")) {
				if (scene->entitiesNum < ENTITIES_MAX) {
					Entity *newEntity = &game->entities[scene->entitiesNum++];
					memcpy(newEntity, selectedEntity, sizeof(Entity));
					newEntity->id = ++scene->nextEntityId;
					game->selectedEntityId = newEntity->id;
				} else {
					logf("Too many game props\n");
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				for (int i = 0; i < scene->entitiesNum; i++) {
					Entity *entity = &game->entities[i];
					if (entity == selectedEntity) {
						arraySpliceIndex(game->entities, scene->entitiesNum, sizeof(Entity), i);
						scene->entitiesNum--;
						break;
					}
				}
				game->selectedEntityId = 0;
				selectedEntity = NULL;
			}

			if (ImGui::Button("Move up")) {
				if (selectedIndex > 0) ArraySwap(game->entities, selectedIndex, selectedIndex-1);
			}

			ImGui::SameLine();
			if (ImGui::Button("Move down")) {
				if (selectedIndex < scene->entitiesNum-1) ArraySwap(game->entities, selectedIndex, selectedIndex+1);
			}
		}

		ImGui::Separator();

		if (selectedEntity) {
			ImGui::Text("Id: %d", selectedEntity->id);
			ImGui::InputText("Name", selectedEntity->name, ENTITY_NAME_MAX_LEN);
			ImGui::Combo("Type", (int *)&selectedEntity->type, entityTypeStrings, ArrayLength(entityTypeStrings));

			ImGui::Separator();
			ImGui::DragFloat3("Position", &selectedEntity->position.x, 0.01);
			ImGui::DragFloat3("Size", &selectedEntity->scale.x, 0.01);
			ImGui::DragFloat3("Rotation", &selectedEntity->rotation.x, 0.01);
			guiInputArgb("Tint", &selectedEntity->tint);
			ImGui::SliderFloat("Alpha", &selectedEntity->alpha, 0, 1);

			ImGui::Separator();
			if (selectedEntity->type == ENTITY_MODEL) {
				ImGui::InputText("Model path", selectedEntity->path, PATH_MAX_LEN);
				ImGui::Checkbox("Is collider", &selectedEntity->isCollider);
				ImGui::Checkbox("Hidden in game", &selectedEntity->hiddenInGame);
				ImGui::Checkbox("Hidden in editor", &selectedEntity->hiddenInEditor);
				ImGui::Checkbox("Needs power", &selectedEntity->needsPower);
			}
			if (selectedEntity->type == ENTITY_PORTAL_SPAWNER) {
				ImGui::Combo("Actor type", (int *)&selectedEntity->actorType, actorTypeStrings, ArrayLength(actorTypeStrings));
				ImGui::InputInt("Spawn count max", &selectedEntity->spawnCountMax);
				ImGui::DragFloat("Spawn delay", &selectedEntity->spawnInterval);
				ImGui::Checkbox("Always powered", &selectedEntity->alwaysPowered);
				ImGui::InputInt("Rail", &selectedEntity->rail);

				if (selectedEntity->actorType == ACTOR_START) {
					ImGui::InputInt("Bounds to power", &selectedEntity->targetEntity);
					Entity *targetedBounds = getEntity(selectedEntity->targetEntity);
					if (!targetedBounds) {
						ImGui::TextColored(ImVec4(1, 0, 0, 1), "No bounds targeted");
					} else {
						ImGui::Text("(%d) %s", targetedBounds->id, targetedBounds->name);
						if (targetedBounds->type != ENTITY_BOUNDS) {
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "Target is not bounds");
						}
					}
				} else if (selectedEntity->actorType == ACTOR_PRESSURE_PLATE) {
					ImGui::InputInt("Rail to power", &selectedEntity->railToPower);
				} else if (selectedEntity->actorType == ACTOR_WEAPON) {
					ImGui::Combo("Weapon type", (int *)&selectedEntity->weaponType, weaponTypeStringCaped, ArrayLength(weaponTypeStringCaped));
				} else if (selectedEntity->actorType == ACTOR_AMMO) {
					ImGui::Combo("Weapon type", (int *)&selectedEntity->weaponType, weaponTypeStringCaped, ArrayLength(weaponTypeStringCaped));
					ImGui::InputInt("Ammo", &selectedEntity->targetEntity);
				} else if (selectedEntity->actorType == ACTOR_TELEPORTER) {
					Entity *currentTarget = getEntity(selectedEntity->targetEntity);

					char *name;
					if (currentTarget) {
						name = frameSprintf("%d %s - %s %s", currentTarget->id, currentTarget->name, entityTypeStrings[currentTarget->type], currentTarget->id == selectedEntity->id ? "(self)" : "");
					} else {
						name = frameSprintf("none");
					}

					if (ImGui::BeginCombo("Target teleporter", name, ImGuiComboFlags_None)) {
						for (int i = 0; i < scene->entitiesNum; i++) {
							Entity *otherEntity = &game->entities[i];
							char *name = frameSprintf("%d %s - %s %s", otherEntity->id, otherEntity->name, entityTypeStrings[otherEntity->type], otherEntity->id == selectedEntity->id ? "(self)" : "");
							if (ImGui::Selectable(name, selectedEntity->targetEntity == otherEntity->id)) {
								selectedEntity->targetEntity = otherEntity->id;
							}
						}

						ImGui::EndCombo();
					}
					// ImGui::Combo("Target teleporter", (int *)&selectedEntity->targetEntity, entityComboList, ArrayLength(entityComboList));
				}
			}
		}

		ImGui::End();
	}

	{ /// Figure out camera
		if (game->inEditor) {
			if (game->usingEditorCamera) {
				Vec3 addedCameraPosition = v3();
				float ep = 0.1f;
				if (keyPressed('W')) {
					addedCameraPosition = addedCameraPosition.add(v3(0, 0, -ep));
				}
				if (keyPressed('S')) {
					addedCameraPosition = addedCameraPosition.add(v3(0, 0, ep));
				}
				if (keyPressed('A')) {
					addedCameraPosition = addedCameraPosition.add(v3(-ep, 0, 0));
				}
				if (keyPressed('D')) {
					addedCameraPosition = addedCameraPosition.add(v3(ep, 0, 0));
				}
				if (keyPressed(KEY_SHIFT)) {
					addedCameraPosition = addedCameraPosition.add(v3(0, ep, 0));
				}
				if (keyPressed(KEY_CTRL)) {
					addedCameraPosition = addedCameraPosition.add(v3(0, -ep, 0));
				}

				float speed = 5 * game->realCameraSpeed;
				addedCameraPosition = addedCameraPosition.multiply(speed);
				Vec4 addedCameraPosition4 = v4(addedCameraPosition.x, addedCameraPosition.y, addedCameraPosition.z, 0);
				addedCameraPosition4 = game->cameraMatrix.invert().multiply(addedCameraPosition4);
				addedCameraPosition.x = addedCameraPosition4.x;
				addedCameraPosition.y = addedCameraPosition4.y;
				addedCameraPosition.z = addedCameraPosition4.z;
				game->realCameraPosition = game->realCameraPosition.add(addedCameraPosition);

				if (platform->useRelativeMouse) {
					game->realCameraRotation.z += platform->relativeMouse.x * 0.01 * (globals->sensitivity / 10.0);
					game->realCameraRotation.x += platform->relativeMouse.y * 0.01 * (globals->sensitivity / 10.0);
				}

				if (game->realCameraRotation.x > 0) game->realCameraRotation.x = 0;
				if (game->realCameraRotation.x < -M_PI) game->realCameraRotation.x = -M_PI;
			}
		}

		WorldProps *world = defaultWorld;

		Vec4 cameraQuat = {0, 0, 0, 1};
		cameraQuat = multiplyQuaternions(cameraQuat, eulerToQuaternion(v3(game->realCameraRotation.x, 0, 0)));
		cameraQuat = multiplyQuaternions(cameraQuat, eulerToQuaternion(v3(0, 0, game->realCameraRotation.z)));
		cameraQuat = multiplyQuaternions(cameraQuat, eulerToQuaternion(v3(0, game->realCameraRotation.y, 0)));
		cameraQuat = cameraQuat.normalize();

		game->cameraMatrix.setIdentity();
		game->cameraMatrix = game->cameraMatrix.rotateQuaternion(cameraQuat);
		game->cameraMatrix = game->cameraMatrix.translate(-game->realCameraPosition.x, -game->realCameraPosition.y, -game->realCameraPosition.z);

		world->viewMatrix = game->cameraMatrix;

		Matrix4 *projection = &world->projectionMatrix;
		projection->setIdentity();
		*projection = projection->perspective(90, (float)platform->windowWidth/(float)platform->windowHeight, 0.1, FAR_PLANE);
	} ///

	{
		if (keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->showingFrameTimes = !game->showingFrameTimes;
		if (game->showingFrameTimes) {
			float avg = 0;
			for (int i = 0; i < platform->frameTimesMax; i++) {
				avg += platform->frameTimes[i];
			}
			avg /= (float)platform->frameTimesMax;
			float fps = 1.0/(avg/1000.0);
			char *str = frameSprintf("frame time: %.2f(%.0ffps)", avg, fps);
			draw2dText(game->defaultFont, str, v2(0, 0), 0xFFFFFFFF);
		}
	}

	process3dDrawQueue();

	drawOnScreenLog();

	popCamera2d();

	if (doPost) {
		processBatchDraws();
		setTargetTextureEx(2, NULL);
		setTargetTexturesNum(1);
		popTargetTexture();

		Vec2 gameScale;
		gameScale.x = (float)platform->windowWidth / (float)game->gameTexture->width;
		gameScale.y = (float)platform->windowHeight / (float)game->gameTexture->height;
		{
			Matrix3 mat = mat3();
			mat = mat.scale(gameScale.x, gameScale.y);
			RenderProps props = newRenderProps();
			props.matrix = mat;
			props.flags |= _F_INVERSE_SRGB;
			// props.flags |= _F_FXAA;
			drawTexture(game->gameTexture, props);
		}

		{ /// Vignette
			Texture *texture = getTexture("assets/images/vignette.png");
			Vec2 vigScale;
			vigScale.x = (float)platform->windowWidth / (float)texture->width;
			vigScale.y = (float)platform->windowHeight / (float)texture->height;
			Matrix3 mat = mat3();
			mat = mat.scale(vigScale.x, vigScale.y);
			RenderProps props = newRenderProps();
			props.matrix = mat;
			props.alpha = 0.2;
			props.tint = 0xFF000000;
			drawTexture(texture, props);
		}

#if 0 /// Bloom
		{
			Matrix3 mat = mat3();
			mat = mat.scale(bloomScale, bloomScale);
			pushTargetTexture(game->gameSmallBloomTexture);
			clearRenderer();
			RenderProps2d props = newRenderProps2d();
			props.matrix = &mat;
			// props.flags |= _F_INVERSE_SRGB;
			// props.smooth = false;
			draw2dTexture(game->gameBloomTexture, props);
			popTargetTexture();
		}

		{
			drawGaussianBlurredXImm(game->gameSmallBloomTexture, 1.0/bloomScale);
			drawGaussianBlurredYImm(game->gameSmallBloomTexture, 1.0/bloomScale);
		}
#endif
	}

	popCamera2d();

	{ /// Update 3d sound
		float *m = game->cameraMatrix.data;

		Vec3 pos = game->realCameraPosition;
		Vec3 up = v3(m[4], m[5], m[6]);
		Vec3 front = v3(-m[8], -m[9], -m[10]);

		update3dSound(pos, front, up);
	}

	game->time += elapsed;
	game->sceneFrames++;
	// logf("Frame %d\n", platform->frameCount);
}

Entity *getEntityByName(const char *name) {
	if (!name || !name[0]) return NULL;

	for (int i = 0; i < game->scene.entitiesNum; i++) {
		Entity *entity = &game->entities[i];
		if (streq(entity->name, name)) return entity;
	}

	return NULL;
}

Entity *getEntity(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < game->scene.entitiesNum; i++) {
		Entity *entity = &game->entities[i];
		if (entity->id == id) return entity;
	}

	return NULL;
}

Actor *getActor(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < game->actorsNum; i++) {
		Actor *actor = &game->actors[i];
		if (actor->id == id) return actor;
	}

	return NULL;
}

Vec3 getRayFromSceenSpace(Vec2 screenPosition, Vec3 cameraOffset) {
	Vec3 start = game->realCameraPosition + cameraOffset;

	Vec3 mouse = v3(screenPosition.x, screenPosition.y, 1.0);
	mouse.x /= platform->windowWidth;
	mouse.y /= platform->windowHeight;
	mouse.x = mouse.x*2.0 - 1.0;
	mouse.y = mouse.y*2.0 - 1.0;
	mouse.y *= -1;

	Vec4 rayClip = v4(mouse.x, mouse.y, -1, 1);
	Vec4 rayEye = defaultWorld->projectionMatrix.invert().multiply(rayClip);
	rayEye = v4(rayEye.x, rayEye.y, -1, 0);
	Vec4 rayWorld4 = defaultWorld->viewMatrix.invert().multiply(rayEye);
	Vec3 rayWorld = v3(rayWorld4.x, rayWorld4.y, rayWorld4.z).normalize();

	return rayWorld;
};

bool rayTestEntity(Entity *entity, Vec3 start, Vec3 ray, float *dist, MeshTri **exactTri) {
	if (exactTri) *exactTri = NULL;

	if (entity->type == ENTITY_BOUNDS) return false;

	Vec3 hitPoint;
	// drawBeam(start, start+ray*999, 0.01, 0xFF00FF00);
	if (!overlaps(entity->bounds, makeLine3(start, start+ray*999), &hitPoint)) {
		// drawBoundsOutline(entity->bounds, 0.01, 0xFFFF0000);
		return false;
	} else {
		// drawBoundsOutline(entity->bounds, 0.01, 0xFF00FF00);
	}

	bool doBounds = false;
	if (entity->type != ENTITY_MODEL) doBounds = true;

	if (doBounds) {
		*dist = start.distance(hitPoint);
		return true;
	}
	// logf("Entity: %s has %d triangles\n", entity->name, mesh->indicesNum/3);

	Mesh *mesh = getMesh(entity->path);
	if (!mesh) return false;

	if (!mesh->triBins) generateTriangleBins(mesh);
	if (mesh->triBinsNum == 0) return false;

	float closestRayDist = 0;
	MeshTri *closestTri = NULL;
#if 1
	Vec3 size = getSize(mesh->triBins[0].bounds);

	Vec3 triRayStart = start - mesh->bounds.min;
	int startX = triRayStart.x / size.x;
	int startY = triRayStart.y / size.y;
	int startZ = triRayStart.z / size.z;
	if (startX < 0) startX = 0;
	if (startY < 0) startY = 0;
	if (startZ < 0) startZ = 0;
	if (startX > mesh->triBinsWide-1) startX = mesh->triBinsWide-1;
	if (startY > mesh->triBinsHigh-1) startY = mesh->triBinsHigh-1;
	if (startZ > mesh->triBinsDeep-1) startZ = mesh->triBinsDeep-1;

	int startIndex = (startZ * mesh->triBinsWide * mesh->triBinsHigh) + (startY * mesh->triBinsDeep) + startX;
	if (startIndex < 0 || startIndex > mesh->triBinsNum) return false;
	TriBin *bin = &mesh->triBins[startIndex];
	// drawBoundsOutline(bin->bounds, 0.1, 0xFFFFFF00);

	Vec3 end = triRayStart + ray*9999;

	Vec3 triRayEnd;
	bool hit = overlaps(mesh->bounds, makeLine3(end, start), &triRayEnd);
	end = triRayEnd - mesh->bounds.min;

	int endX = end.x / size.x;
	int endY = end.y / size.y;
	int endZ = end.z / size.z;
	if (endX < 0) endX = 0;
	if (endY < 0) endY = 0;
	if (endZ < 0) endZ = 0;
	if (endX > mesh->triBinsWide-1) endX = mesh->triBinsWide-1;
	if (endY > mesh->triBinsHigh-1) endY = mesh->triBinsHigh-1;
	if (endZ > mesh->triBinsDeep-1) endZ = mesh->triBinsDeep-1;

	int endIndex = (endZ * mesh->triBinsWide * mesh->triBinsHigh) + (endY * mesh->triBinsDeep) + endX;

	{
		TriBin *bin = &mesh->triBins[startIndex];
		for (int i = 0; i < bin->trisNum; i++) {
			MeshTri *tri = bin->tris[i];
			float rayDist;
			if (rayIntersectTriangle(start, ray, tri->verts[0], tri->verts[1], tri->verts[2], &rayDist)) {
				if (!closestRayDist || rayDist < closestRayDist) {
					closestTri = tri;
					closestRayDist = rayDist;
				}
			}
		}
	}

	int dx = endX-startX;
	int dy = endY-startY;
	int dz = endZ-startZ;
	int nx = abs(dx);
	int ny = abs(dy);
	int nz = abs(dz);
	int sign_x = dx > 0? 1 : -1;
	int sign_y = dy > 0? 1 : -1;
	int sign_z = dz > 0? 1 : -1;

	Vec3 p = v3(startX, startY, startZ);
	int ix = 0;
	int iy = 0;
	int iz = 0;
	int count = 0;
	while (ix < nx || iy < ny || iz < nz) {
		float distX = (0.5+(float)ix) / (float)nx;
		float distY = (0.5+(float)iy) / (float)ny;
		float distZ = (0.5+(float)iz) / (float)nz;
		if (distX <= distY && distX <= distZ) {
			p.x += sign_x;
			ix++;
		} else if (distY <= distX && distY <= distZ) {
			p.y += sign_y;
			iy++;
		} else {
			p.z += sign_z;
			iz++;
		}

		int index = (p.z * mesh->triBinsWide * mesh->triBinsHigh) + (p.y * mesh->triBinsDeep) + p.x;
		TriBin *bin = &mesh->triBins[index];
		for (int i = 0; i < bin->trisNum; i++) {
			MeshTri *tri = bin->tris[i];
			float rayDist;
			if (rayIntersectTriangle(start, ray, tri->verts[0], tri->verts[1], tri->verts[2], &rayDist)) {
				if (!closestRayDist || rayDist < closestRayDist) {
					closestTri = tri;
					closestRayDist = rayDist;
				}
			}
		}
		// if (count++ < 100) drawBoundsOutline(bin->bounds, 0.1, 0xFF00FFFF);
	}
#else
	for (int i = 0; i < mesh->triBinsNum; i++) {
		TriBin *bin = &mesh->triBins[i];
		if (bin->trisNum == 0) continue; // Comment this for 50% speed
		// Vec3 binCenter = getCenter(bin->bounds);
		// float centerDist = start.distance(binCenter) * 2 + 1;
		float centerDist = 99999;
		Vec3 hitPoint;
		if (overlaps(bin->bounds, makeLine3(start, start + ray*centerDist), &hitPoint)) {
			// drawBoundsOutline(bin->bounds, 0.1, 0xFF0000FF);
			// if (strstr(entity->name, "level0")) drawBoundsOutline(bin->bounds, 0.1, 0xFF0000FF);
			for (int i = 0; i < bin->trisNum; i++) {
				MeshTri *tri = bin->tris[i];
				float rayDist;
				if (rayIntersectTriangle(start, ray, tri->verts[0], tri->verts[1], tri->verts[2], &rayDist)) {
					// drawBeam(t0, t1, 0.1, 0xFFFF0000);
					// drawBeam(t1, t2, 0.1, 0xFFFF0000);
					// drawBeam(t2, t0, 0.1, 0xFFFF0000);
					if (!closestRayDist || rayDist < closestRayDist) {
						closestTri = tri;
						closestRayDist = rayDist;
					}
				}
			}
		}
	}
#endif

	if (closestRayDist == 0) return false;

	if (exactTri) *exactTri = closestTri;
	*dist = closestRayDist;
	return true;
}

bool rayTestActorsAndEntities(Vec3 start, Vec3 dir, Actor **outActor, Entity **outEntity, float *outDist, Actor *actorToIgnore, bool onlyColliders, bool ignoreDead) {
	Scene *scene = &game->scene;

	Entity *closestEntity = NULL;
	float closestEntityDist = 9999;

	Actor *closestActor = NULL;
	float closestActorDist = 9999;

	for (int i = 0; i < scene->entitiesNum; i++) {
		Entity *entity = &game->entities[i];
		if (onlyColliders && !entity->isCollider) continue;

		float dist;
		bool hit = rayTestEntity(entity, start, dir, &dist);

		if (hit) {
			if (!closestEntity || dist < closestEntityDist) {
				closestEntity = entity;
				closestEntityDist = dist;
			}
		}
	}

	for (int i = 0; i < game->actorsNum; i++) {
		Actor *otherActor = &game->actors[i];
		if (otherActor->hp <= 0) continue;
		if (actorToIgnore && otherActor == actorToIgnore) continue;

		for (int i = 0; i < 2; i++) {
			AABB bounds;
			if (i == 0) bounds = otherActor->bounds;
			if (i == 1) bounds = otherActor->skeletonBounds;
			if (isZero(bounds)) continue;

			Vec3 hitPoint = v3();
			bool hit = overlaps(bounds, makeLine3(start, start + dir * 9999), &hitPoint);
			if (hit) {
				float dist = start.distance(hitPoint);
				if (!closestActor || dist < closestActorDist) {
					closestActor = otherActor;
					closestActorDist = dist;
				}
			}
		}
	}

	*outEntity = NULL;
	*outActor = NULL;

	Vec3 end;
	if (closestEntityDist < closestActorDist) {
		closestActor = NULL;
		*outEntity = closestEntity;
		if (outDist) *outDist = closestEntityDist;
		return true;
	} else {
		closestEntity = NULL;
		*outActor = closestActor;
		if (outDist) *outDist = closestActorDist;
		return true;
	}

	return false;
}

bool AABBIntersectsEntity(AABB bounds, Entity *entity) {
	Mesh *mesh = getMesh(entity->path);
	if (!mesh) return false;

	if (!intersects(bounds, entity->bounds)) return false; // Super important early out, not documented in debug

	if (!mesh->triBins) generateTriangleBins(mesh);

	TriBin **bins = (TriBin **)frameMalloc(sizeof(TriBin *) * mesh->triBinsNum);
	int binsNum = 0;

	float closestRayDist = 0;
	for (int i = 0; i < mesh->triBinsNum; i++) {
		TriBin *bin = &mesh->triBins[i];
		if (bin->trisNum == 0) continue;
		// Vec3 binCenter = bin->bounds.max - bin->bounds.min;
		AABB binRelativeBounds = bounds;
		// binRelativeBounds.min = binRelativeBounds.min + mesh->bounds.min;  // I'm unsure why we don't need this
		// binRelativeBounds.max = binRelativeBounds.max + mesh->bounds.min;
		if (intersects(binRelativeBounds, bin->bounds)) {
			// drawBoundsOutline(bin->bounds, 0.1, 0xFFFF0000);
			bins[binsNum++] = bin;
		}
	}

	{
		Vec3 points[24];
		AABBToLines(bounds, points);

		for (int i = 0; i < 24/2; i++) {
			Vec3 start = points[i*2 + 0];
			Vec3 end = points[i*2 + 1];
			float dist = start.distance(end);
			Vec3 dir = end - start;

			for (int i = 0; i < binsNum; i++) {
				TriBin *bin = bins[i];
				for (int i = 0; i < bin->trisNum; i++) {
					MeshTri *tri = bin->tris[i];

					float rayDist;
					if (rayIntersectTriangle(start, dir, tri->verts[0], tri->verts[1], tri->verts[2], &rayDist)) {
						if (rayDist < dist) return true;
					}
				}
			}
		}

		return false;
	}
}

Actor *createActor(ActorType type) {
	if (game->actorsNum > ACTORS_MAX-1) {
		logf("Too many actors\n");
		return NULL;
	}

	Actor *actor = &game->actors[game->actorsNum++];
	memset(actor, 0, sizeof(Actor));
	actor->id = ++game->nextActorId;
	actor->type = type;
	actor->scale = v3(1, 1, 1);
	actor->hp = 100;

	return actor;
}

void dealDamage(Actor *dest, float amount, Actor *src, Vec3 hitPoint) {
	dest->timeSinceLastHit = 0;

	if (dest->type == ACTOR_SHOOTER) {
		float flinchChance = 0.5;
		if (rndPerc(flinchChance)) {
			dest->state = ASTATE_FLINCHING;
		}
	}

	if (game->globals.godMode && dest->type == ACTOR_PLAYER) return;

	if (dest->type == ACTOR_PLAYER) {
	} else {
		createWorldText(hitPoint, frameSprintf("Damage: %.0f", amount));
	}

	dest->hp -= amount;
}

Effect *createWorldText(Vec3 position, char *text) {
	if (game->effectsNum > EFFECTS_MAX-1) {
		logf("Too many effects\n");
		return NULL;
	}

	Effect *effect = &game->effects[game->effectsNum++];
	memset(effect, 0, sizeof(Effect));
	effect->type = EFFECT_WORLD_TEXT;
	effect->valueString = stringClone(text);
	effect->position = position;
	return effect;
}

void log3f(Vec3 point, const char *msg, ...) {
	va_list args;

	va_start(args, msg);
	int size = stbsp_vsnprintf(NULL, 0, msg, args);
	va_end(args);

	char *str = frameMalloc(size+1);

	va_start(args, msg);
	stbsp_vsnprintf(str, size+1, msg, args);
	va_end(args);

	{
		Vec2 screenSpace = worldSpaceToScreenSpace(point);
		draw2dCircle(screenSpace, 5, 0xFFFF0000);

		Vec2 size = getTextSize(game->defaultFont, str);
		Vec2 pos = screenSpace + v2(10, 10);
		draw2dRect(makeRect(pos, size), 0x44000000);
		draw2dText(game->defaultFont, str, pos);
	}
}

Vec2 worldSpaceToScreenSpace(Vec3 point) {
	WorldProps *world = defaultWorld;
	Globals *globals = &game->globals;

	point = game->cameraMatrix.multiply(point);
	point = world->projectionMatrix.multiply(point);
	if (point.z < 0 || point.z > 1) return v2(5000, 5000);

	Vec2 result;
	result.x = (point.x + 1) / 2;
	result.y = (-point.y + 1) / 2;
	result.x *= platform->windowWidth;
	result.y *= platform->windowHeight;

	result.x *= globals->resolutionPerc;
	result.y *= globals->resolutionPerc;
	return result;
}

Rect worldSpaceAABBToScreenSpaceRect(AABB bounds) {
	Vec3 verts3d[8];
	getVerts(bounds, verts3d);

	Vec2 verts2d[8];
	Vec2 min2d = v2(99999, 99999);
	Vec2 max2d = v2(-99999, -99999);
	for (int i = 0; i < 8; i++) {
		verts2d[i] = worldSpaceToScreenSpace(verts3d[i]);
		if (min2d.x > verts2d[i].x) min2d.x = verts2d[i].x;
		if (min2d.y > verts2d[i].y) min2d.y = verts2d[i].y;
		if (max2d.x < verts2d[i].x) max2d.x = verts2d[i].x;
		if (max2d.y < verts2d[i].y) max2d.y = verts2d[i].y;
	}

	Rect rect;
	rect.x = min2d.x;
	rect.y = min2d.y;
	rect.width = max2d.x - min2d.x;
	rect.height = max2d.y - min2d.y;
	return rect;
}

void drawGlowingBeam(Vec3 start, Vec3 end, float thickness, int color) {
	MeshProps props = newMeshProps();
	props.matrix = getBeamMatrix(start, end, thickness);
	props.tint = color;
	props.flags |= _F_3D_BLOOM;

	Mesh *cubeMesh = getMesh("assets/common/models.Cube.mesh.iqm");
	drawMesh(cubeMesh, props);
}

bool testLineOfSight(Vec3 start, Vec3 end) {
	float destDist = start.distance(end);
	Vec3 dir = (end - start).normalize();

	for (int i = 0; i < game->scene.entitiesNum; i++) {
		Entity *entity = &game->entities[i];
		if (!entity->isCollider) continue;
		float rayDist;
		if (rayTestEntity(entity, start, dir, &rayDist)) {
			if (rayDist < destDist) return false;
		}
	}

	return true;
}

void updateNav(Actor *actor, MeshTri *destTri, Vec3 destPoint) {
	actor->navNextStepTick++;

	actor->navNextTarget = v3();

	float destDist = actor->position.distance(destPoint);

	Entity *navMeshEntity = getEntity(game->currentNavMeshEntityId);
	if (!navMeshEntity) return;

	Vec3 size = getSize(actor->bounds);
	Vec3 start = actor->position + v3(0, 0, size.z-0.01);

	if (actor->navNextStepTick > 30) {
	Vec3 castStart = actor->position;
	destPoint.z += 0.1;
	castStart.z += 0.1;
	Vec3 castDir = (destPoint - castStart).normalize();
	bool hasLos = testLineOfSight(castStart, destPoint);
	// drawBeam(castStart, castStart + castDir*10, 0.1, 0xFFFF0000);

	if (hasLos) {
		actor->navNextTarget = destPoint;
		return;
	}

		MeshTri *currentMeshTri = NULL;
		float rayDist;
		MeshTri *meshTri;
		if (rayTestEntity(navMeshEntity, start, v3(0, 0, -1), &rayDist, &meshTri)) {
			// drawBeam(start, start + v3(0,0,-1)*5, 0.1, 0xFFFF0000);
			currentMeshTri = meshTri;
			// drawBeam(meshTri->verts[0], meshTri->verts[1], 0.3, 0xFF0000FF);
			// drawBeam(meshTri->verts[1], meshTri->verts[2], 0.3, 0xFF0000FF);
			// drawBeam(meshTri->verts[2], meshTri->verts[0], 0.3, 0xFF0000FF);
		}

		actor->navNextStepTick = 0;
		if (!currentMeshTri) return;
		Mesh *mesh = getMesh(navMeshEntity->path);

		MeshTri *startMeshTri = currentMeshTri;
		// Vec2Tile realStart = v2Tile(start.x, start.y);
		int meshTriPtrSize = sizeof(MeshTri *);

		Allocator priorityQueueAllocator = {};
		priorityQueueAllocator.type = ALLOCATOR_FRAME;
		PriorityQueue *frontier = createPriorityQueue(meshTriPtrSize, &priorityQueueAllocator);
		priorityQueuePush(frontier, &startMeshTri, 0);

		Allocator hashMapAllocator = {};
		hashMapAllocator.type = ALLOCATOR_FRAME;
		HashMap *costSoFar = createHashMap(meshTriPtrSize, sizeof(float), 32, &hashMapAllocator);

		HashMap *cameFrom = createHashMap(meshTriPtrSize, meshTriPtrSize, 32, &hashMapAllocator);

		float zero = 0;
		hashMapSet(costSoFar, &startMeshTri, hashToInt(startMeshTri), &zero);

		while (frontier->length > 0) {
			MeshTri *current;
			bool good = priorityQueueShift(frontier, &current);
			if (!good) logf("queue is empty\n");

			if (current == destTri) break;

			for (int i = 0; i < 3; i++) {
				int neighborIndex = current->adjs[i];
				if (neighborIndex == -1) continue;
				MeshTri *neighbor = &mesh->meshTris[neighborIndex];
				float newCost;
				bool good = hashMapGet(costSoFar, &current, hashToInt(current), &newCost);
				if (!good) logf("This needs to be good\n");

				float cost = 1;
				newCost += cost;

				float neighborCost = -1;
				bool wasVisited = hashMapGet(costSoFar, &neighbor, hashToInt(neighbor), &neighborCost);

				if (!wasVisited || newCost < neighborCost) {
					hashMapSet(cameFrom, &neighbor, hashToInt(neighbor), &current);
					hashMapSet(costSoFar, &neighbor, hashToInt(neighbor), &newCost);
					priorityQueuePush(frontier, &neighbor, newCost);
				}
			}
		}

		MeshTri **path = (MeshTri **)frameMalloc(sizeof(MeshTri *) * 1024);
		int pathNum = 0;

		MeshTri *current = destTri;
		for (;;) {
			if (!current) {
				logf("bad situation!\n");
				break;
			}

			path[pathNum++] = current;

			if (current == startMeshTri) break;
			bool good = hashMapGet(cameFrom, &current, hashToInt(current), &current);
			if (!good) {
				logf("That's no good\n");
				break;
			}
		}

		// for(int i = 0; i < pathNum; i++) {
		// 	MeshTri *meshTri = path[i];
		// 	drawBeam(meshTri->verts[0], meshTri->verts[1], 0.3, 0xFF0000FF);
		// 	drawBeam(meshTri->verts[1], meshTri->verts[2], 0.3, 0xFF0000FF);
		// 	drawBeam(meshTri->verts[2], meshTri->verts[0], 0.3, 0xFF0000FF);
		// }

		MeshTri *targetTri = NULL;
		if (pathNum > 0) targetTri = path[pathNum-1];
		if (pathNum > 1) targetTri = path[pathNum-2];
		actor->navNextMeshTri = targetTri;

		destroyHashMap(costSoFar);
		destroyHashMap(cameFrom);
		destroyPriorityQueue(frontier);
		// Panic("Done");
	}

	if (!actor->navNextMeshTri) return;

	Vec3 target = (actor->navNextMeshTri->verts[0] + actor->navNextMeshTri->verts[1] + actor->navNextMeshTri->verts[2]) / 3;

	Line3 currentEdges[3];
	currentEdges[0].start = actor->navNextMeshTri->verts[0];
	currentEdges[0].end = actor->navNextMeshTri->verts[1];
	currentEdges[1].start = actor->navNextMeshTri->verts[1];
	currentEdges[1].end = actor->navNextMeshTri->verts[2];
	currentEdges[2].start = actor->navNextMeshTri->verts[2];
	currentEdges[2].end = actor->navNextMeshTri->verts[0];

	for (int i = 0; i < 3; i++) {
		// drawBeam(currentEdges[i].start, currentEdges[i].end, 0.2, 0xFF00FF00);
		Vec3 project = projectPointOnLine(actor->position, currentEdges[i]);
		// drawSphere(project, 1, 0xFFFF0000);
		if (actor->position.distance(project) < actor->position.distance(target)) {
			target = project;
		}
	}

	actor->navNextTarget = target;

	float dist = actor->position.distance(actor->navNextTarget);
	if (dist < actor->velo.length()*2) actor->navNextStepTick = 999999;
}

Bullet *shootProjectileBullet(Vec3 start, Vec3 dir, Actor *actor) {
	if (game->bulletsNum > BULLETS_MAX-1) {
		logf("Too many bullets\n");
		return NULL;
	}
	Bullet *bullet = &game->bullets[game->bulletsNum++]; //@robustness Enfore BULLETS_MAX
	memset(bullet, 0, sizeof(Bullet));
	bullet->position = start;
	bullet->direction = dir;

	if (actor->type != ACTOR_PLAYER) bullet->ally = false;
	bullet->speed = 1;
	SoundSource *soundSource = playWorldSound(bullet->position, "assets/audio/sounds/sine.ogg");
	setVolume(soundSource, 0.01);
	bullet->soundSourceId = soundSource->id;

	return bullet;
}

void shootRayBullet(Vec3 start, Vec3 dir, Actor *actor) {
	WeaponInfo *wInfo = &game->weaponInfos[actor->weaponType];

	float rayDist;
	Entity *closestEntity;
	Actor *closestActor;
	if (rayTestActorsAndEntities(start, dir, &closestActor, &closestEntity, &rayDist, actor, true, true)) {
		if (closestActor && closestActor->skeleton) {
			Skeleton *skeleton = closestActor->skeleton;
			for (int i = 0; i < skeleton->base->bonesNum; i++) {
				Bone *bone = &skeleton->base->bones[i];
				if (bone->parent < 0) continue;
				BoneInfo *boneInfo = getBoneInfo(skeleton, bone);
				if (!boneInfo) continue;

				Bone *parent = &skeleton->base->bones[bone->parent];
				BoneInfo *parentInfo = getBoneInfo(skeleton, parent);
				if (parentInfo->width <= 0.001) continue;

				Matrix4 boneMat = skeleton->currentTransforms[i] * bone->modelSpaceMatrix;
				Matrix4 parentBoneMat = skeleton->currentTransforms[bone->parent] * parent->modelSpaceMatrix;
				Vec3 capsuleStart = boneMat.getPosition();
				Vec3 capsuleEnd = parentBoneMat.getPosition();
				float radius = parentInfo->width;
				float dist = rayTestCapsule(start, dir, capsuleStart, capsuleEnd, radius);
				if (dist != -1) {
					SkeletonBlend *knockBack = getSkeletonBlend(skeleton, "knockBack");
					if (knockBack) {
						int index = bone->parent;
						knockBack->poseXforms[index].translation.x += rndFloat(-1, 1);
						knockBack->poseXforms[index].translation.y += rndFloat(-1, 1);
						knockBack->poseXforms[index].translation.z += rndFloat(-1, 1);
					}
					dealDamage(closestActor, wInfo->damage, actor, start + dir*dist);
					break;
				}
			}
		}

		Vec3 end = start + dir * rayDist;
		drawSphere(end, 1, 0xFFFF0000);
	}
}

AnimQueueItem *createAnimQueueItem(Actor *actor, char *animName, float length, bool loops) {
	AnimQueueItem *item = &actor->animQueue[actor->animQueueNum++]; //@robustness enforce max
	memset(item, 0, sizeof(AnimQueueItem));
	item->exists = true;
	item->timeScale = 1;
	item->newLength = length;
	item->loops = loops;
	item->index = getSkeletonAnimationIndexByName(actor->skeleton, animName);
	return item;
}

void clearAnimQueue(Actor *actor) {
	actor->animQueueNum = 0;
	if (actor->currentlyPlaying) actor->forceSkip = true;
}

void updateActorAnimation(Actor *actor) {
	if (actor->skeleton) {
		SkeletonBlend *main = getSkeletonBlend(actor->skeleton, "main");
		if (main) {
			if (!main->animation) main->animation = getAnimation(actor->skeleton, "idle");
			bool playNext = false;
			bool animJustOver = false;

			if (main->animation) {
				if (main->animationFrame >= main->animation->frameCount-2) {
					// if (actor->type == ACTOR_PLAYER) logf("next\n");
					if (actor->currentlyPlaying) animJustOver = true;
					actor->currentlyPlaying = false;
					playNext = true;
				}

			} else {
				playNext = true;
			}

			if (!main->playing) playNext = true;

			if (actor->forceSkip && actor->animQueueNum > 0) {
				playNext = true;
			}

			if (actor->currentAnimQueueItem.exists && actor->currentAnimQueueItem.skippable && actor->animQueueNum > 0) {
				playNext = true;
			}

			if (animJustOver) {
				if (actor->currentAnimQueueItem.reloadChain) {
					WeaponInfo *wInfo = &game->weaponInfos[actor->weaponType];

					bool shouldEnd = false;

					actor->reserveBullets[wInfo->type]--;
					actor->bullets[wInfo->type]++;

					if (actor->bullets[wInfo->type] >= wInfo->maxBullets) shouldEnd = true;
					if (actor->reserveBullets[wInfo->type] <= 0) shouldEnd = true;
					if (actor->tryingToCancel) shouldEnd = true;

					if (shouldEnd) {
						createAnimQueueItem(actor, frameSprintf("reloading%sEnd", wInfo->cappedName), 0.5, false);
						createAnimQueueItem(actor, frameSprintf("pump%s", wInfo->cappedName), -1, false);
					} else {
						AnimQueueItem item = actor->currentAnimQueueItem;
						actor->animQueue[actor->animQueueNum++] = item;
					}
				}

				if (actor->currentAnimQueueItem.reloadsAfter) {
					int maxBullets = game->weaponInfos[actor->weaponType].maxBullets;
					int bulletsToAdd = maxBullets - actor->bullets[actor->weaponType];
					if (bulletsToAdd > actor->reserveBullets[actor->weaponType]) bulletsToAdd = actor->reserveBullets[actor->weaponType];
					actor->bullets[actor->weaponType] = bulletsToAdd;
					actor->reserveBullets[actor->weaponType] -= bulletsToAdd;
				}

				if (actor->currentAnimQueueItem.toggleInfoToolAfter) {
					game->usingInfoTool = !game->usingInfoTool;
				}

				if (actor->currentAnimQueueItem.noneStateAfter) {
					actor->state = ASTATE_NONE;
				}

				if (actor->currentAnimQueueItem.stateAfter != ASTATE_NONE) {
					actor->state = actor->currentAnimQueueItem.stateAfter;
				}

				if (actor->currentAnimQueueItem.loops) {
					AnimQueueItem item = actor->currentAnimQueueItem;
					actor->animQueue[actor->animQueueNum++] = item;
				}
			}

			if (actor->animQueueNum <= 0) playNext = false;

			if (playNext) {
				actor->currentlyPlaying = true;
				actor->tryingToCancel = false;
				actor->forceSkip = false;

				AnimQueueItem *item = &actor->animQueue[0];
				main->animation = &actor->skeleton->base->animations[item->index];
				main->timeScale = item->timeScale;
				main->loops = false;
				if (item->newLength != -1) main->timeScale = main->animation->length / item->newLength;
				main->time = 0;

				actor->currentAnimQueueItem = *item;
				arraySpliceIndex(actor->animQueue, actor->animQueueNum, sizeof(AnimQueueItem), 0);
				actor->animQueueNum--;
			}
		}
	}
}
