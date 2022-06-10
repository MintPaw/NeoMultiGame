// Make sure you test for having 2 door going to the same play on the same map
// Make enemy spawners list what they spawn on the map
// Fix enemies not get stuck in walls
// Increase vision range for enemies when inPauseBeforeBattle==true

struct Globals {
};

#define MAP_VERSION 4
enum ActorType {
	ACTOR_NONE=0,
	ACTOR_SPAWNER=1,
	ACTOR_PLAYER=2,
	ACTOR_DOOR=3,
	ACTOR_NPC=4,
	ACTOR_ENEMY=5,
	ACTOR_ITEM=6,
	ACTOR_IMAGE=7,
	ACTOR_FOCUS_AREA=8,
	ACTOR_TYPES_MAX,
};
char *actorTypeStrings[] = {
	"None",
	"Spawner",
	"Player",
	"Door",
	"Npc",
	"Enemy",
	"Item",
	"Image",
	"Focus area",
};
struct ActorTypeInfo {
	ActorType type;
	Vec2 size;
	bool usesImageMatrix;
};

enum FighterType {
	FIGHTER_NONE=0,
	FIGHTER_MAIN_PLAYER=1,
	FIGHTER_BAT=2,
	FIGHTER_WOLF=3,
	FIGHTER_TYPES_MAX,
};
char *fighterTypeStrings[] = {
	"None",
	"Main player",
	"Bat",
	"Wolf",
};

enum ActionType {
	ACTION_DIALOG=0,
	ACTION_GO_TO_OVERWORLD=1,
	ACTION_PASS=2,
	ACTION_PARISH=3,
	ACTION_PUNCH=4,
	ACTION_WING_SLAP=5,
	ACTION_CAN_OF_HUGE_DAMAGE=6,
	ACTION_CHOMP=7,
	ACTION_TYPES_MAX,
};
char *actionTypeStrings[] = {
	"Dialog",
	"Go to overworld",
	"Pass",
	"Parish",
	"Punch",
	"Wing slap",
	"Can of huge damage",
	"Chomp",
};

struct SpawnEntry {
	bool isItem;
	int fighterOrItemType;
	int chances;
};

enum Direction {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
};
char *directionStrings[] = {
	"up",
	"down",
	"left",
	"right",
};

#define _FA_ALWAYS_ON_TOP       (1 << 0)
#define _FA_FADES_WHEN_OVERLAPS (1 << 1)
enum ActorState {
	ACTOR_STATE_STANDING,
	ACTOR_STATE_MOVING,
	ACTOR_STATE_CHASING,
};
struct Actor {
	ActorType type;
#define ACTOR_NAME_MAX_LEN 128
	char name[ACTOR_NAME_MAX_LEN];
	int id;
	int parentId;
	Vec2 position;
	float rotation;
	Vec2 scale;

	Rect collisionRect;

#define MAP_NAME_MAX_LEN 128
	char nextMapName[MAP_NAME_MAX_LEN];

#define CONVERSATION_NAME_MAX_LEN 128
	char convoName[CONVERSATION_NAME_MAX_LEN];

	ActionType itemActionType;
	int itemAmount;

#define SPAWN_ENTRIES_MAX 4
	SpawnEntry spawnEntries[SPAWN_ENTRIES_MAX];
	int amountToSpawn;
	float spawnRadius;

	FighterType fighterType;

	char imagePath[PATH_MAX_LEN];

	u32 flags;

	/// Unserialized
	float overlapFade;
	Vec2 prevPosition;
	Vec2 trueVelo;
	float timeLastMoved;
	float timeLastStoodStill;
	Direction direction;
	bool hidden;

	ActorState prevState;
	ActorState state;
	float stateTime;
	Vec2 targetPosition;

	ActorTypeInfo *info;
	bool markedForDeletion;
	Rect rect;
};

#define _F_MAP_TILE_COLLIDABLE (1 << 0)
struct MapTile {
	int tilesetIndex;
	u32 flags;
};
struct Map {
	char name[MAP_NAME_MAX_LEN];
	Vec2i size;
	MapTile *tiles;

#define ACTORS_MAX 512
	Actor actors[ACTORS_MAX];
	int actorsNum;
	int nextActorId;

	/// Unserialized
	RenderTexture *thumbnail;
};

enum EditorMode {
	EDITOR_ACTOR,
	EDITOR_TILEMAP,
	EDITOR_COLLISION,
};
char *editorModeStrings[] = {
	"Actor editor",
	"Tilemap editor",
	"Collision editor",
};

struct FighterTypeInfo {
	FighterType type;
	int hp;
	int mp;
	int speed;
};
struct Fighter {
	FighterType type;
	FighterTypeInfo *info;
	int id;

	bool ally;
	int hp;
	int mp;
	int speed;

	bool parishing;
	bool markedForDeletion;
};

enum TargetingType {
	TARGETING_ENEMIES,
	TARGETING_ALLIES,
	TARGETING_NONE,
	TARGETING_ALL,
};
struct ActionTypeInfo {
	ActionType type;
	TargetingType targetingType;
};
struct Action {
	ActionType type;
	ActionTypeInfo *info;

	int src;
	int dest;
	char *dialog;
};

enum FightMenu {
	FIGHT_MENU_NONE,
	FIGHT_MENU_ATTACK,
	FIGHT_MENU_ITEM,
	FIGHT_MENU_TARGET,
};
struct Fight {
#define FIGHTERS_MAX 32
	Fighter fighters[FIGHTERS_MAX];
	int fightersNum;
	int nextFighterId;

	FightMenu menu;
	ActionType selectedAction;

	int turns[FIGHTERS_MAX];
	int turnsNum;

#define ACTIONS_MAX 32
	Action actions[ACTIONS_MAX];
	int actionsNum;

	float actionTime;
	bool fightOver;
};

struct DialogEntry {
#define DIALOG_MAX_LEN 256
	char dialog[DIALOG_MAX_LEN];
};
struct Conversation {
	char name[CONVERSATION_NAME_MAX_LEN];

	DialogEntry *entries;
	int entriesNum;
};

struct Item {
	ActionType actionType;
	int amount;
};

enum GameState {
	GAME_NONE,
	GAME_OVERWORLD,
	GAME_FIGHT,
};
struct Game {
	Font *defaultFont;
	Font *hugeFont;
	Font *fightFont;
	Font *convoFont;
	RenderTexture *gameTexture;
	RenderTexture *debugTexture;
	Texture *paddedTileset;

	Globals globals;
	bool inEditor;
	float timeScale;
	float time;
	Vec2 size;
	float sizeScale;
	Vec2 mouse;

	Vec2 screenOverlayOffset;
	Vec2 screenOverlaySize;

	GameState state;
	GameState prevState;
	GameState nextState;
	float stateTransition_t;
	float stateTime;

	ActorTypeInfo actorTypeInfos[ACTOR_TYPES_MAX];

	float tileSize;
	Vec2 baseRes;
	int tilesetPad;

#define MAPS_MAX 128
	Map maps[MAPS_MAX];
	int prevMap;
	int currentMap;
	float mapTransition_t;
	int nextMapIndex;
	char standingOnDoorFromPrevMapName[MAP_NAME_MAX_LEN];

	bool initedMap;

	Vec2 cameraTarget;
	float cameraZoom;
	Rect cameraBounds;

#define PLAYER_FIGHERS_MAX 8
	FighterTypeInfo fighterTypeInfos[FIGHTER_TYPES_MAX];
	Fighter playerFighters[PLAYER_FIGHERS_MAX];
	int playerFightersNum;
	Fight fight;

	ActionTypeInfo actionTypeInfos[ACTION_TYPES_MAX];

#define ITEMS_MAX 512
	Item items[ITEMS_MAX];
	int itemsNum;

	bool inPauseBeforeBattle;
	float pauseBeforeBattleTime;

	Conversation *convos;
	int convosNum;
	int currentConvoIndex;
	int currentDialogEntryIndex;
	int currentDialogEntryCharacterIndex;
	float timeTillNextDialogCharacter;

	int currentFocusAreaId;
	float timeFocusAreaChanged;

	/// Editor/debug
	EditorMode editorMode;
	int selectedActor;
	Vec2 draggingActorOffset;
	bool draggingActor;
	int debugChangeMap;
	Vec2i debugNewMapSize;
	bool debugRegenerateTilesetSelector;
	int debugSelectedTilesetTile;
	bool debugShowFrameTimes;
	bool debugIntegerPositions;
	bool debugIntegerCamera;
	bool debugDrawSpritesWithPixelFiltering;
	bool debugDrawSpritesWithBilinear;
	bool debugDrawMainTextureWithPixelFiltering;
	bool debugDrawMainTextureWithBilinear;
	bool debugTrueLowRes;
	bool debugHideFocusAreas;
	bool debugDrawBottomOfImages; // Not on UI

	int copiedActorMapIndex;
	Actor copiedActor;
	bool attachingActor;
};
Game *game = NULL;

void runGame();
void updateGame();
void updateOverworld();
Vec4 getTileUvs(int tilesetIndex);
Vec4 getPaddedTileUvs(int tilesetIndex);

Actor *createActor();
void deleteActor(Actor *actor);
Actor *getActor(int id);
Actor *getActor(char *name);
Actor *pasteActor(Actor *srcActor, int srcActorMapIndex, Actor *parent=NULL);
// Vec2i worldToTile(Vec2 worldPosition);
// bool overlaps(Actor *actor1, Actor *actor2);
bool isTileCollidable(Vec2i tilemapPosition);
bool isOnCollision(Rect rect);
bool isOnCollision(Line2 worldLine);
bool rectOverlapsImage(Rect rect, Actor *imageActor, bool useCollisionRect=false);
void startConversation(char *convoName);

void saveMap(Map *map, int mapFileIndex);
void loadMap(Map *map, int mapFileIndex);
void saveConversations();
void loadConversations();
Matrix3 getImagePointMatrix(Actor *actor);
Matrix3 getImageMatrix(Actor *actor);
bool doesActorContain(Actor *actor, Vec2 point);

void resetFighter(Fighter *fighter);
void updateFight();
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
#if !defined(FALLOW_INTERNAL) // This needs to be a macro
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#else
	char dbPath[PATH_MAX_LEN];
	snprintf(dbPath, PATH_MAX_LEN, "%s/../zooBoundGameAssets", exeDir);
	if (directoryExists(dbPath)) {
		strcpy(projectAssetDir, dbPath);
	} else if (directoryExists("C:/Dropbox")) {
		strcpy(projectAssetDir, "C:/Dropbox/ZooBound/zooBoundGameAssets");
	} else if (directoryExists("O:/Dropbox")) {
		strcpy(projectAssetDir, "O:/Dropbox/ZooBound/zooBoundGameAssets");
	}
#endif
#endif

	initFileOperations();

	Vec2 res = v2(1600, 900);

	initPlatform(res.x, res.y, "ZooBound");
	platform->sleepWait = true;
	initAudio();
	initRenderer(res.x, res.y);
	initTextureSystem();
	initFonts();
	initAnimations();
	animSys->loopsByDefault = true;

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		logf("Game struct is %.2fmb btw\n", (float)sizeof(Game) / (float)Megabytes(1));
		if (ArrayLength(actionTypeStrings) != ACTION_TYPES_MAX) Panic("Mismatch actionTypeStrings");
		if (ArrayLength(actorTypeStrings) != ACTOR_TYPES_MAX) Panic("Mismatch actorTypeStrings");
		if (ArrayLength(fighterTypeStrings) != FIGHTER_TYPES_MAX) Panic("Mismatch fighterTypeStrings");

		// RegMem(Globals, useFXAA);

		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 20);
		game->hugeFont = createFont("assets/common/arial.ttf", 42);
		game->fightFont = createFont("assets/fonts/minecraft.ttf", 16);
		game->convoFont = createFont("assets/fonts/minecraft.ttf", 16);

		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);
		// for (int i = 0; i < MAPS_MAX; i++) {
		// 	Map *map = &game->maps[i];
		// 	map->size = v2i(1, 1);
		// 	map->tiles = (MapTile *)zalloc(sizeof(MapTile) * map->size.x*map->size.y);
		// 	saveMap(map, i);
		// }
		for (int i = 0; i < MAPS_MAX; i++) loadMap(&game->maps[i], i);
		loadConversations();

		game->timeScale = 1;
		game->baseRes = v2(512, 448);
		game->size = v2(512, 448);
		game->tileSize = 32;
		game->prevMap = -1;
		game->nextMapIndex = -1;
		game->currentConvoIndex = -1;
		game->cameraZoom = 1;
		game->debugIntegerPositions = true;
		game->debugDrawSpritesWithBilinear = true;
		game->debugDrawSpritesWithPixelFiltering = true;
		game->tilesetPad = 2;

		{ /// Setup actor type info
			ActorTypeInfo *info;

			info = &game->actorTypeInfos[ACTOR_SPAWNER];
			info->size = v2(game->tileSize, game->tileSize)*2.5;

			info = &game->actorTypeInfos[ACTOR_IMAGE];
			info->usesImageMatrix = true;

			info = &game->actorTypeInfos[ACTOR_FOCUS_AREA];
			info->usesImageMatrix = true;

			for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
				ActorTypeInfo *info = &game->actorTypeInfos[i];
				if (isZero(info->size)) info->size = v2(game->tileSize, game->tileSize);
			}
		}

		{ /// Setup action type info
			ActionTypeInfo *info;

			info = &game->actionTypeInfos[ACTION_PUNCH];
			info->targetingType = TARGETING_ENEMIES;

			info = &game->actionTypeInfos[ACTION_WING_SLAP];
			info->targetingType = TARGETING_ENEMIES;

			info = &game->actionTypeInfos[ACTION_CAN_OF_HUGE_DAMAGE];
			info->targetingType = TARGETING_NONE;

			for (int i = 0; i < ACTION_TYPES_MAX; i++) {
				ActionTypeInfo *info = &game->actionTypeInfos[i];
				info->type = (ActionType)i;
			}
		}

		{ /// Setup fighter type infos
			FighterTypeInfo *info;

			info = &game->fighterTypeInfos[FIGHTER_MAIN_PLAYER];
			info->hp = 100;
			info->mp = 20;
			info->speed = 3;

			info = &game->fighterTypeInfos[FIGHTER_BAT];
			info->hp = 5;
			info->mp = 20;
			info->speed = 2;

			info = &game->fighterTypeInfos[FIGHTER_WOLF];
			info->hp = 10;
			info->mp = 20;
			info->speed = 4;

			for (int i = 0; i < FIGHTER_TYPES_MAX; i++) {
				FighterTypeInfo *info = &game->fighterTypeInfos[i];
				info->type = (FighterType)i;
			}
		}

		{ /// Starting fighters
			Fighter *fighter = &game->playerFighters[game->playerFightersNum++];
			fighter->type = FIGHTER_MAIN_PLAYER;

			for (int i = 0; i < game->playerFightersNum; i++) {
				Fighter *fighter = &game->playerFighters[i];
				fighter->ally = true;
				resetFighter(fighter);
			}
		}

		packSpriteSheet("assets/images/sprites");
		maximizeWindow();

		//if (SDL_Init(SDL_INIT_HAPTIC) < 0) logf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		//SDL_Joystick* gGameController = NULL;
		//SDL_Haptic* gControllerHaptic = NULL;
		//if (SDL_NumJoysticks() < 1) logf("Warning: No joysticks connected!\n");
		////Load joystick
		//gGameController = SDL_JoystickOpen(0);
		//if( gGameController == NULL ) logf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		////Get controller haptic device
		//gControllerHaptic = SDL_HapticOpenFromJoystick( gGameController );
		//if (gControllerHaptic == NULL) logf("Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError());
		////Get initialize rumble
		//if( SDL_HapticRumbleInit( gControllerHaptic ) < 0 ) logf("Warning: Unable to initialize rumble! SDL Error: %s\n", SDL_GetError());
	}

	Globals *globals = &game->globals;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	game->mouse = (platform->mouse - game->screenOverlayOffset) * (game->size/game->screenOverlaySize);

	if (game->debugTrueLowRes) {
		game->sizeScale = 1;
		if (!equal(game->size, game->baseRes)) {
			game->size = game->baseRes;
			if (game->gameTexture) destroyTexture(game->gameTexture);
			game->gameTexture = NULL;
			if (game->debugTexture) destroyTexture(game->debugTexture);
			game->debugTexture = NULL;
		}
	} else {
		{ /// Resizing
			Vec2 ratio = game->baseRes;
			game->sizeScale = MinNum(platform->windowWidth/ratio.x, platform->windowHeight/ratio.y);
			Vec2 newSize = ratio * game->sizeScale;

			if (!equal(game->size, newSize)) {
				game->size = newSize;

				if (game->gameTexture) destroyTexture(game->gameTexture);
				game->gameTexture = NULL;
				if (game->debugTexture) destroyTexture(game->debugTexture);
				game->debugTexture = NULL;

				game->screenOverlaySize = game->size;
				game->screenOverlayOffset.x = (float)platform->windowWidth/2 - game->size.x/2;
				game->screenOverlayOffset.y = (float)platform->windowHeight/2 - game->size.y/2;
			}
		}
	}

	if (!game->gameTexture) game->gameTexture = createRenderTexture(game->size.x, game->size.y);
	if (!game->debugTexture) game->debugTexture = createRenderTexture(game->size.x, game->size.y);

	pushTargetTexture(game->debugTexture);
	clearRenderer();
	popTargetTexture();

	pushTargetTexture(game->gameTexture);
	clearRenderer(0xFF101010);

	if (game->state != game->nextState) {
		game->stateTransition_t += 0.05;
		if (game->stateTransition_t >= 1) {
			game->state = game->nextState;
		}
	} else {
		game->stateTransition_t -= 0.05;
	}
	game->stateTransition_t = Clamp01(game->stateTransition_t);

	if (game->prevState != game->state) {
		game->prevState = game->state;
		game->stateTime = 0;

		game->inPauseBeforeBattle = false;
	}

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;

	if (game->state == GAME_NONE) {
		game->nextState = GAME_OVERWORLD;
	} else if (game->state == GAME_OVERWORLD) {
		updateOverworld();
	} else if (game->state == GAME_FIGHT) {
		updateFight();
	}

	game->stateTime += elapsed;

	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, game->stateTransition_t)); // Not far enough
	popTargetTexture(); // game->gameTexture

	clearRenderer();

	if (game->debugTrueLowRes) {
		Vec2 offset;
		offset.x = (float)platform->windowWidth/2 - game->size.x/2;
		offset.y = (float)platform->windowHeight/2 - game->size.y/2;

		Rect gameRect = getInnerRectOfAspect(makeRect(0, 0, platform->windowWidth, platform->windowHeight), v2(game->size.x, game->size.y));
		game->screenOverlaySize = getSize(gameRect);
		game->screenOverlayOffset = getPosition(gameRect);
	}

	{
		RenderTexture *texture = game->gameTexture;
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(game->screenOverlaySize);

		if (game->debugDrawMainTextureWithBilinear) {
			setTextureSmooth(game->gameTexture, true);
		} else {
			setTextureSmooth(game->gameTexture, false);
		}

		if (game->debugDrawMainTextureWithPixelFiltering) {
			drawPixelArtFilterTexture(texture, matrix);
		} else {
			drawSimpleTexture(texture, matrix);
		}

		drawSimpleTexture(game->debugTexture, matrix);
	}

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) {
		char *str = frameSprintf("%.2fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
	}

	guiDraw();
	drawOnScreenLog();

	game->time += elapsed;
}


void updateOverworld() {
	Map *map = &game->maps[game->currentMap];

	Texture *tileset = getTexture("assets/images/tileset.png");
	int tilesetTilesWide = tileset->width/game->tileSize;
	int tilesetTilesHigh = tileset->height/game->tileSize;
	setTextureSmooth(tileset, false);

	if (!game->paddedTileset) {
		Texture *tileset = getTexture("assets/images/tileset.png");

		int pngSize;
		void *pngData = readFile("assets/images/tileset.png", &pngSize);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		u8 *tilesetBitmapData = stbi_load_from_memory((unsigned char *)pngData, pngSize, &width, &height, &channels, 4);
		free(pngData);

		int bitmapDataWidth = tileset->width + (tilesetTilesWide+1)*game->tilesetPad;
		int bitmapDataHeight = tileset->height + (tilesetTilesHigh+1)*game->tilesetPad;
		if (bitmapDataWidth > 8192-1) bitmapDataWidth = 8192-1;
		if (bitmapDataHeight > 8192-1) bitmapDataHeight = 8192-1;

		u8 *bitmapData = (u8 *)frameMalloc(bitmapDataWidth * bitmapDataHeight * 4);
		for (int tileY = 0; tileY < tilesetTilesHigh; tileY++) {
			for (int tileX = 0; tileX < tilesetTilesWide; tileX++) {
				for (int y = 0; y < game->tileSize; y++) {
					for (int x = 0; x < game->tileSize; x++) {
						int srcX = tileX * game->tileSize + x;
						int srcY = tileY * game->tileSize + y;
						int srcIndex = srcY * tileset->width + srcX;

						int destX = srcX + (tileX+1)*game->tilesetPad;
						int destY = srcY + (tileY+1)*game->tilesetPad;
						if (destX > bitmapDataWidth-1) continue;
						if (destY > bitmapDataHeight-1) continue;
						int destIndex = destY * bitmapDataWidth + destX;

						memcpy(&bitmapData[destIndex*4], &tilesetBitmapData[srcIndex*4], 4);

						if (x == 0) {
							int newDestIndex = (destY) * bitmapDataWidth + (destX-1);
							memcpy(&bitmapData[newDestIndex*4], &tilesetBitmapData[srcIndex*4], 4);
						}

						if (y == 0) {
							int newDestIndex = (destY-1) * bitmapDataWidth + (destX);
							memcpy(&bitmapData[newDestIndex*4], &tilesetBitmapData[srcIndex*4], 4);
						}

						if (x == game->tileSize-1) {
							int newDestIndex = (destY) * bitmapDataWidth + (destX+1);
							memcpy(&bitmapData[newDestIndex*4], &tilesetBitmapData[srcIndex*4], 4);
						}

						if (y == game->tileSize-1) {
							int newDestIndex = (destY+1) * bitmapDataWidth + (destX);
							memcpy(&bitmapData[newDestIndex*4], &tilesetBitmapData[srcIndex*4], 4);
						}
					}
				}
			}
		}

		game->paddedTileset = createTexture(bitmapDataWidth, bitmapDataHeight, bitmapData);
		free(tilesetBitmapData);
	}

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	if (game->prevMap != game->currentMap) {
		game->prevMap = game->currentMap;
		game->debugNewMapSize = map->size;
		game->debugChangeMap = game->currentMap;
	}

	if (keyPressed('K')) game->cameraZoom += 0.001;
	if (keyPressed('J')) game->cameraZoom -= 0.001;

	Vec2 camTranslate = -game->cameraTarget + game->baseRes/2;
	if (game->debugIntegerCamera) {
		camTranslate.x = roundf(camTranslate.x);
		camTranslate.y = roundf(camTranslate.y);
	}

	Matrix3 cameraMatrix = mat3();
	if (!game->debugTrueLowRes) cameraMatrix.SCALE(game->size / game->baseRes);
	cameraMatrix.TRANSLATE(camTranslate);
	cameraMatrix.SCALE(game->cameraZoom);
	pushCamera2d(cameraMatrix);

	game->mouse = cameraMatrix.invert() * game->mouse;

	Rect worldSpaceCameraRect = cameraMatrix.invert() * makeRect(v2(), game->size);

	if (game->inEditor) {
		ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::TreeNode("Tweaks")) {
			ImGui::Checkbox("True low res", &game->debugTrueLowRes);
			ImGui::Checkbox("Integer position", &game->debugIntegerPositions);
			ImGui::Checkbox("Integer camera", &game->debugIntegerCamera);
			ImGui::Checkbox("Draw sprites with bilinear", &game->debugDrawSpritesWithBilinear);
			ImGui::Checkbox("Draw sprites with pixel filtering", &game->debugDrawSpritesWithPixelFiltering);
			ImGui::Checkbox("Draw main texture with bilinear", &game->debugDrawMainTextureWithBilinear);
			ImGui::Checkbox("Draw main texture with pixel filter", &game->debugDrawMainTextureWithPixelFiltering);
			if (ImGui::Button("Reset camera zoom")) game->cameraZoom = 1;
			ImGui::TreePop();
		}

		ImGui::Separator();

		ImGui::Combo("Editor mode", (int *)&game->editorMode, editorModeStrings, ArrayLength(editorModeStrings));

		ImGui::SameLine();
		ImGui::Text("(?)");

		if (ImGui::IsItemHovered()) {
			if (game->editorMode == EDITOR_ACTOR) {
				ImGui::SetTooltip(
					"Mode info:\nClick actors to select them, or choose from the list on the right\n"
					"You can drag them around, and change their properties\n"
					"Actor types 'Player', 'Enemy', and 'Item' are not saved, use spawners to create them"
				);
			} else if (game->editorMode == EDITOR_TILEMAP) {
				ImGui::SetTooltip(
					"Mode info:\nChose a tile from the pallet on the right, then left click to draw\n"
					"You can shift+click tiles that already exist on the map to change your brush"
				);
			} else if (game->editorMode == EDITOR_COLLISION) {
				ImGui::SetTooltip(
					"Mode info:\nLeft click to create collision, right click to remove it"
				);
			}
		}

		ImGui::Separator();

		if (ImGui::TreeNodeEx("Map", ImGuiTreeNodeFlags_DefaultOpen)) {
#if 1
		{ /// Generate thumbnails
			Texture *tileset = getTexture("assets/images/tileset.png");
			if (game->maps[0].thumbnail == NULL) {
				float thumbnailScaleDown = 0.25;
				Matrix3 zoomedOutMatrix = renderer->currentCameraMatrix.invert();
				zoomedOutMatrix.SCALE(thumbnailScaleDown);
				pushCamera2d(zoomedOutMatrix);

				for (int i = 0; i < MAPS_MAX; i++) {
					Map *map = &game->maps[i];
					RenderTexture *texture = createRenderTexture(128, 128);

					pushTargetTexture(texture);
					clearRenderer();
					{
						for (int y = 0; y < map->size.y; y++) {
							for (int x = 0; x < map->size.x; x++) {
								int tileMapIndex = y * map->size.x + x;
								MapTile *tile = &map->tiles[tileMapIndex];
								Vec4 uvuv = getTileUvs(tile->tilesetIndex);
								Vec2 uv0 = v2(uvuv.x, uvuv.y);
								Vec2 uv1 = v2(uvuv.z, uvuv.w);

								Matrix3 mat = mat3();
								mat.TRANSLATE(v2(x, y) * game->tileSize);
								mat.SCALE(game->tileSize);

								drawSimpleTexture(tileset, mat, uv0, uv1);
							}
						}
					}

					// {
					// 	Rect textRect = makeRect(v2(), getSize(texture));
					// 	textRect.width *= 1 / thumbnailScaleDown;
					// 	textRect.height *= 1 / thumbnailScaleDown;
					// 	textRect.height *= 0.25;
					// 	DrawTextProps props = newDrawTextProps();
					// 	props.font = game->defaultFont;
					// 	props.color = 0xFF000000;
					// 	drawTextInRect(frameSprintf("%d: %s", i, map->name), props, textRect);
					// }

					popTargetTexture();
					map->thumbnail = texture;
				}
				popCamera2d();
			}
		}

			if (ImGui::Button("Change map")) ImGui::OpenPopup("changeMapPopup");
			if (ImGui::BeginPopup("changeMapPopup")) {
				for (int i = 0; i < MAPS_MAX; i++) {
					Map *map = &game->maps[i];
					ImGui::Text("%d: %s", i, map->name);
					if (guiImageButton(map->thumbnail)) {
						if (game->currentMap != i) {
							game->nextMapIndex = i;
							ImGui::CloseCurrentPopup();
						}
					}
				}
				ImGui::EndPopup();
			}
#else
			ImGui::InputInt("Change map", &game->debugChangeMap);
			game->debugChangeMap = mathClamp(game->debugChangeMap, 0, MAPS_MAX-1);
			if (game->currentMap != game->debugChangeMap) {
				ImGui::SameLine();
				if (ImGui::Button("Go")) {
					game->nextMapIndex = game->debugChangeMap;
				}
			}
#endif

			if (keyPressed('S') && ImGui::Button("Save all maps")) {
				for (int i = 0; i < MAPS_MAX; i++) saveMap(&game->maps[i], i);
			}
			if (ImGui::Button("Save map")) saveMap(map, game->currentMap);
			ImGui::SameLine();
			if (ImGui::Button("Load map")) {
				loadMap(map, game->currentMap);
				game->nextMapIndex = game->currentMap;
			}
			ImGui::InputText("Name", map->name, MAP_NAME_MAX_LEN);

			ImGui::InputInt2("Map size", &game->debugNewMapSize.x);
			if (game->debugNewMapSize.x < 1) game->debugNewMapSize.x = 1;
			if (game->debugNewMapSize.y < 1) game->debugNewMapSize.y = 1;
			if (!equal(map->size, game->debugNewMapSize)) {
				ImGui::SameLine();
				guiPushStyleColor(ImGuiCol_Button, lerpColor(0x800000FF, 0xFF0000FF, timePhase(game->time * 3)));
				if (ImGui::Button("Apply resize")) {
					MapTile *newTiles = (MapTile *)zalloc(sizeof(MapTile) * game->debugNewMapSize.x*game->debugNewMapSize.y);
					for (int y = 0; y < map->size.y; y++) {
						if (y > game->debugNewMapSize.y-1) continue;

						for (int x = 0; x < map->size.x; x++) {
							if (x > game->debugNewMapSize.x-1) continue;

							MapTile mapTile = map->tiles[y * map->size.x + x];
							newTiles[y * game->debugNewMapSize.x + x] = mapTile;
						}
					}
					free(map->tiles);
					map->tiles = newTiles;
					map->size = game->debugNewMapSize;
				}
				guiPopStyleColor();
			}
			ImGui::TreePop();
		}

		ImGui::End();

		if (game->editorMode == EDITOR_TILEMAP) {
			ImGui::SetNextWindowPos(ImVec2(platform->windowWidth, 0), ImGuiCond_Always, ImVec2(1, 0));
			ImGui::Begin("Tile set", NULL, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("Selected tileset tile: %d\n", game->debugSelectedTilesetTile);
			ImGui::BeginChild("TilesetChild", ImVec2(500, 500), false, ImGuiWindowFlags_HorizontalScrollbar);
			static RenderTexture *tilesetSelector;
			if (game->debugRegenerateTilesetSelector && tilesetSelector) {
				game->debugRegenerateTilesetSelector = false;
				destroyTexture(tilesetSelector);
				tilesetSelector = NULL;
			}
			if (!tilesetSelector) {
				tilesetSelector = createRenderTexture(tileset->width, tileset->height);
				pushTargetTexture(tilesetSelector);
				pushCamera2d(renderer->currentCameraMatrix.invert());
				clearRenderer();
				drawSimpleTexture(tileset);

				Rect selectedTileRect = makeRect(v2(), v2(game->tileSize, game->tileSize));
				selectedTileRect.x = (game->debugSelectedTilesetTile % tilesetTilesWide) * game->tileSize;
				selectedTileRect.y = (game->debugSelectedTilesetTile / tilesetTilesWide) * game->tileSize;
				drawRectOutline(selectedTileRect, 2, 0xFFFF0000);

				popCamera2d();
				popTargetTexture();
			}
			guiTexture(tilesetSelector);
			if (ImGui::IsItemClicked()) {
				ImVec2 imMin = ImGui::GetItemRectMin();
				Vec2 min = v2(imMin.x, imMin.y);
				Vec2 off = platform->mouse - min;
				Vec2i tilePos = v2i(off / game->tileSize);
				game->debugSelectedTilesetTile = tilePos.y * tilesetTilesWide + tilePos.x;
				game->debugRegenerateTilesetSelector = true;
			}
			ImGui::EndChild();
			ImGui::End();
		} else if (game->editorMode == EDITOR_ACTOR) {
			ImGui::SetNextWindowPos(ImVec2(platform->windowWidth, 0), ImGuiCond_Always, ImVec2(1, 0));
			ImGui::Begin("Actors", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Checkbox("Hide focus areas", &game->debugHideFocusAreas);
			ImGui::BeginChild("ActorListChild", ImVec2(400, 200));

			for (int i = 0; i < map->actorsNum; i++) {
				ImGui::PushID(i);
				Actor *actor = &map->actors[i];

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				if (ImGui::ArrowButton("upButton", ImGuiDir_Up)) {
					if (i > 0) arraySwap(map->actors, map->actorsNum, sizeof(Actor), i, i-1);
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("downButton", ImGuiDir_Down)) {
					if (i < map->actorsNum-1) arraySwap(map->actors, map->actorsNum, sizeof(Actor), i, i+1);
				}
				ImGui::PopStyleVar();

				char *name;
				if (actor->type == ACTOR_IMAGE) {
					name = frameSprintf("%s (%s)###%d", actorTypeStrings[actor->type], actor->imagePath, i);
				} else {
					name = frameSprintf("%s###%d", actorTypeStrings[actor->type], i);
				}
				ImGui::SameLine();
				if (ImGui::Selectable(name, game->selectedActor == actor->id, 0, ImVec2(320, 0))) {
					game->selectedActor = actor->id;
				}

				ImGui::PopID();
			}
			ImGui::EndChild();
			if (ImGui::Button("Create actor")) {
				Actor *actor = createActor();
				if (actor) {
					actor->position = game->cameraTarget;
					game->selectedActor = actor->id;
				}
			}

			if (map != &game->maps[0] && ImGui::TreeNode("Summon premade actor")) {
				Map *baseMap = &game->maps[0];
				for (int i = 0; i < baseMap->actorsNum; i++) {
					Actor *baseActor = &baseMap->actors[i];
					if (baseActor->name[0] == '#') {
						if (ImGui::Button(frameSprintf("%s###%d", baseActor->name, i))) {
							Actor *newActor = pasteActor(baseActor, 0, NULL);
							if (newActor) {
								newActor->position = game->cameraTarget;
								game->selectedActor = newActor->id;
							}
						}
					}
				}

				ImGui::TreePop();
			}
			ImGui::Separator();
			ImGui::Separator();

			Actor *actor = getActor(game->selectedActor);
			if (actor) {
				if (ImGui::Button("Delete actor")) actor->markedForDeletion = true;
				ImGui::SameLine();
				if (ImGui::Button("Duplicate actor")) {
					Actor *newActor = createActor();
					if (newActor) {
						int id = newActor->id;
						*newActor = *actor;
						newActor->id = id;
						newActor->position += v2(10, 10);
						game->selectedActor = newActor->id;
					}
				}

				ImGui::Combo("Actor type", (int *)&actor->type, actorTypeStrings, ArrayLength(actorTypeStrings));
				ImGui::InputText("Name", actor->name, ACTOR_NAME_MAX_LEN);
				ImGui::SameLine();
				ImGui::Text("Id: %d", actor->id);
				ImGui::DragFloat2("Position", &actor->position.x);

				Vec2 nearestTile = roundToNearest(actor->position - game->tileSize/2, v2(game->tileSize, game->tileSize)) + game->tileSize/2;
				if (!equal(actor->position, nearestTile)) {
					ImGui::SameLine();
					if (ImGui::Button("Snap to nearest tile")) actor->position = nearestTile;
				}
				if (actor->type == ACTOR_IMAGE) {
					ImGui::DragFloat("Rotation", &actor->rotation);
					ImGui::DragFloat2("Scale", &actor->scale.x, 0.005);
				}
				if (actor->type == ACTOR_FOCUS_AREA) {
					ImGui::DragFloat2("Scale", &actor->scale.x, 1);
				}

				ImGui::CheckboxFlags("Always on top", &actor->flags, _FA_ALWAYS_ON_TOP);
				ImGui::CheckboxFlags("Fades when overlaps", &actor->flags, _FA_FADES_WHEN_OVERLAPS);

				ImGui::Separator();

				if (actor->type == ACTOR_SPAWNER) {
					ImGui::InputInt("Amount to spawn", &actor->amountToSpawn);
					ImGui::DragFloat("Spawn radius", &actor->spawnRadius, 0.1);
					for (int i = 0; i < SPAWN_ENTRIES_MAX; i++) {
						SpawnEntry *entry = &actor->spawnEntries[i];
						// FighterTypeInfo *info = &game->fighterTypeInfo[entry->type];
						char *name = NULL;
						if (entry->isItem) {
							name = frameSprintf("%d: %s (%d chances)###entry%d", i, actionTypeStrings[entry->fighterOrItemType], entry->chances, i);
						} else {
							name = frameSprintf("%d: %s (%d chances)###entry%d", i, fighterTypeStrings[entry->fighterOrItemType], entry->chances, i);
						}
						if (ImGui::TreeNode(name)) {
							if (ImGui::Checkbox("Is item", &entry->isItem)) entry->fighterOrItemType = 0;
							if (entry->isItem) {
								ImGui::Combo("Item type", (int *)&entry->fighterOrItemType, actionTypeStrings, ArrayLength(actionTypeStrings));
							} else {
								ImGui::Combo("Fighter type", (int *)&entry->fighterOrItemType, fighterTypeStrings, ArrayLength(fighterTypeStrings));
							}
							ImGui::InputInt("Chances to spawn", &entry->chances);
							ImGui::TreePop();
						}
					}
				}

				if (actor->type == ACTOR_DOOR) {
					ImGui::InputText("Next map name", actor->nextMapName, MAP_NAME_MAX_LEN);
				}

				if (actor->type == ACTOR_NPC) {
					ImGui::InputText("Conversation name", actor->convoName, CONVERSATION_NAME_MAX_LEN);
				}

				if (actor->type == ACTOR_ITEM) {
					ImGui::Combo("Item type", (int *)&actor->itemActionType, actionTypeStrings, ArrayLength(actionTypeStrings));
					ImGui::InputInt("Item amount", &actor->itemAmount);
				}

				if (actor->type == ACTOR_IMAGE) {
					ImGui::InputText("Image path", actor->imagePath, PATH_MAX_LEN);
					ImGui::DragFloat4("Collision rect (xywh)", &actor->collisionRect.x, 0.005);
					ImGui::InputInt("Parent", &actor->parentId);
					if (!actor->parentId) {
						ImGui::SameLine();
						if (ImGui::Button("Attach")) {
							game->attachingActor = true;
						}
					} else {
						ImGui::SameLine();
						if (ImGui::Button("Detach")) {
							logf("Detached!\n");
							Actor *parent = getActor(actor->parentId);
							actor->parentId = 0;
							if (parent) {
								Matrix3 matrix = getImagePointMatrix(parent) * getImagePointMatrix(actor);
								actor->position = getPosition(matrix);
								actor->rotation = getRotationDeg(matrix);
								actor->scale = getScale(matrix);
							} else {
								logf("No parent?\n");
							}
						}
					}
				}

				//@todo Make global actor fixup
				if (actor->parentId == actor->id) {
					actor->parentId = 0;
					logf("Can't be parented to self...\n");
				}
				if (actor->type != ACTOR_IMAGE) {
					actor->parentId = 0;
				}
			}
			ImGui::End();

			if (keyPressed(KEY_CTRL) && keyJustPressed('C')) {
				Actor *actor = getActor(game->selectedActor);
				if (actor) {
					logf("Copied actor\n");
					game->copiedActor = *actor;
					game->copiedActorMapIndex = game->currentMap;
				}
			}

			if (keyPressed(KEY_CTRL) && keyJustPressed('V')) {
				if (game->copiedActor.type != ACTOR_NONE) {
					logf("Pasted actor\n");

					Actor *parent = NULL;
					if (game->copiedActorMapIndex == game->currentMap) parent = getActor(game->copiedActor.parentId);
					Actor *newActor = pasteActor(&game->copiedActor, game->copiedActorMapIndex, parent);
					if (newActor) game->selectedActor = newActor->id;
				}
			}
		}

		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, 0), ImGuiCond_Always, ImVec2(0.5, 0));
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
		ImGui::Begin("Conversation editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button("Save conversations")) saveConversations();
		ImGui::SameLine();
		if (ImGui::Button("Load conversations")) loadConversations();

		ImGui::Separator();

		int deleteConvoIndex = -1;

		for (int i = 0; i < game->convosNum; i++) {
			ImGui::PushID(i);

			Conversation *convo = &game->convos[i];
			if (ImGui::TreeNode(frameSprintf("%s###%d", convo->name, i))) {
				ImGui::InputText("Name", convo->name, CONVERSATION_NAME_MAX_LEN);
				ImGui::SameLine();
				if (ImGui::Button("Delete convo")) deleteConvoIndex = i;

				for (int i = 0; i < convo->entriesNum; i++) {
					DialogEntry *entry = &convo->entries[i];
					int lineCount = countChar(entry->dialog, '\n');
					ImGui::InputTextMultiline(frameSprintf("Dialog entry %d", i), entry->dialog, DIALOG_MAX_LEN, ImVec2(500, 15*(lineCount+1)+5));
				}

				if (ImGui::Button("Add dialog entry")) {
					convo->entries = (DialogEntry *)resizeArray(convo->entries, sizeof(DialogEntry), convo->entriesNum, convo->entriesNum+1);
					convo->entriesNum++;
				}
				ImGui::SameLine();
				if (ImGui::Button("Add 5 dialog entries")) {
					convo->entries = (DialogEntry *)resizeArray(convo->entries, sizeof(DialogEntry), convo->entriesNum, convo->entriesNum+5);
					convo->entriesNum += 5;
				}

				ImGui::TreePop();
			}
			// ImGui::Separator();

			ImGui::PopID();
		}

		if (deleteConvoIndex > -1) {
			arraySpliceIndex(game->convos, game->convosNum, sizeof(Conversation), deleteConvoIndex);
			game->convosNum--;
		}

		ImGui::Separator();
		if (ImGui::Button("Add conversation")) {
			game->convos = (Conversation *)resizeArray(game->convos, sizeof(Conversation), game->convosNum, game->convosNum+1);
			game->convosNum++;
		}

		ImGui::End();

		float scrollSpeed = 5;
		if (keyPressed(KEY_LEFT) || keyPressed('A')) game->cameraTarget.x -= scrollSpeed;
		if (keyPressed(KEY_RIGHT) || keyPressed('D')) game->cameraTarget.x += scrollSpeed;
		if (keyPressed(KEY_UP) || keyPressed('W')) game->cameraTarget.y -= scrollSpeed;
		if (keyPressed(KEY_DOWN) || keyPressed('S')) game->cameraTarget.y += scrollSpeed;
	}

	for (int y = 0; y < map->size.y; y++) {
		for (int x = 0; x < map->size.x; x++) {
			Rect tilemapRect = makeRect(v2(x, y) * game->tileSize, v2(game->tileSize, game->tileSize));
			if (!contains(worldSpaceCameraRect, tilemapRect)) continue;

			int tileMapIndex = y * map->size.x + x;
			MapTile *tile = &map->tiles[tileMapIndex];

			// Vec4 uvuv = getTileUvs(tile->tilesetIndex);
			Vec4 uvuv = getPaddedTileUvs(tile->tilesetIndex);
			Vec2 uv0 = v2(uvuv.x, uvuv.y);
			Vec2 uv1 = v2(uvuv.z, uvuv.w);

			Matrix3 mat = mat3();
			mat.TRANSLATE(getPosition(tilemapRect));
			mat.SCALE(game->tileSize);

			// drawSimpleTexture(tileset, mat, uv0, uv1);
			// drawSimpleTexture(game->paddedTileset, mat, uv0, uv1);
			drawPixelArtFilterTexture(game->paddedTileset, mat, uv0, uv1);

			if (game->inEditor && game->editorMode == EDITOR_COLLISION) {
				if (tile->flags & _F_MAP_TILE_COLLIDABLE) {
					drawRect(tilemapRect, lerpColor(0x40FF0000, 0x80FF0000, secondPhase));
				}
			}
		}
	}

	if (!game->initedMap) {
		game->initedMap = true;

		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->type == ACTOR_SPAWNER) {
				for (int i = 0; i < actor->amountToSpawn; i++) {
					int weights[SPAWN_ENTRIES_MAX] = {};
					bool canSpawn = false;
					for (int i = 0; i < SPAWN_ENTRIES_MAX; i++) {
						weights[i] = actor->spawnEntries[i].chances;
						if (weights[i] > 0) canSpawn = true;
					}
					if (!canSpawn) continue;

					SpawnEntry *entry = &actor->spawnEntries[rndPick(weights, SPAWN_ENTRIES_MAX)];

					Actor *newActor = createActor();
					if (!newActor) continue;

					if (entry->isItem) {
						newActor->type = ACTOR_ITEM;
						newActor->itemActionType = (ActionType)entry->fighterOrItemType;
						newActor->itemAmount = 1;
					} else {
						newActor->type = ACTOR_ENEMY;
						newActor->fighterType = (FighterType)entry->fighterOrItemType;
					}

					int maxSpawnTries = 100;
					for (int i = 0; i < maxSpawnTries; i++) {
						newActor->position = actor->position;

						float rads = rndFloat(0, M_PI*2);
						float dist = rndFloat(0, actor->spawnRadius);
						newActor->position.x += cos(rads) * dist;
						newActor->position.y += sin(rads) * dist;

						ActorTypeInfo *info = &game->actorTypeInfos[actor->type];
						Rect rect = makeRect(newActor->position - info->size/2, info->size);
						if (!isOnCollision(rect)) break;
						if (i == maxSpawnTries-1) {
							newActor->markedForDeletion = true;
							logf("Tried to spawn %d times, but gave up\n", maxSpawnTries);
						}
					}
				}
			}
		}
	}

	Actor *player = NULL;
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->type == ACTOR_PLAYER) player = actor;
	}

	if (!player) {
		Vec2 spawnPosition = v2();
		if (game->standingOnDoorFromPrevMapName[0]) {
			for (int i = 0; i < map->actorsNum; i++) {
				Actor *actor = &map->actors[i];
				if (actor->type == ACTOR_DOOR && streq(actor->nextMapName, game->standingOnDoorFromPrevMapName)) {
					spawnPosition = actor->position;
				}
			}
		} else {
			Vec2 closest = v2();
			for (int i = 0; i < map->actorsNum; i++) {
				Actor *actor = &map->actors[i];
				if (actor->type != ACTOR_DOOR) continue;

				if (isZero(closest) || actor->position.length() < closest.length()) {
					closest = actor->position;
				}

				if (!actor->nextMapName[0]) {
					closest = actor->position;
					break;
				}
			}

			if (isZero(closest)) logf("No player, and no door to spawn player... 0,0 it is!\n");
			spawnPosition = closest;
		}

		if (isZero(spawnPosition)) {
			for (int i = 0; i < map->actorsNum; i++) {
				Actor *actor = &map->actors[i];
				if (actor->type == ACTOR_DOOR) {
					strcpy(game->standingOnDoorFromPrevMapName, actor->nextMapName);
					spawnPosition = actor->position;
					break;
				}
			}
		}

		player = createActor();
		if (!player) {
			map->actorsNum--;
			player = createActor();
			logf("Not enough free actors to spawn the player, the last one was deleted...\n");
		}

		player->type = ACTOR_PLAYER;
		player->position = spawnPosition;
	}

	/// Actors first iteration
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		actor->rect.x = actor->position.x - actor->info->size.x/2;
		actor->rect.y = actor->position.y - actor->info->size.y/2;
		actor->rect.width = actor->info->size.x;
		actor->rect.height = actor->info->size.y;
	}

	// if (keyJustPressed('1')) {
	// 	Fight *fight = &game->fight;
	// 	memset(fight, 0, sizeof(Fight));

	// 	for (int i = 0; i < 2; i++) {
	// 		{ /// Create/add fighter
	// 			Fighter *fighter = &fight->fighters[fight->fightersNum++];
	// 			memset(fighter, 0, sizeof(Fighter));
	// 			fighter->id = ++fight->nextFighterId;
	// 			fighter->type = FIGHTER_BAT;
	// 		}
	// 	}

	// 	for (int i = 0; i < fight->fightersNum; i++) { // Reset only the non-player fighters
	// 		Fighter *fighter = &fight->fighters[i];
	// 		resetFighter(fighter);
	// 	}

	// 	for (int i = 0; i < game->playerFightersNum; i++) {
	// 		game->playerFighters[i].id = ++fight->nextFighterId;
	// 		fight->fighters[fight->fightersNum++] = game->playerFighters[i];
	// 	}

	// 	game->nextState = GAME_FIGHT;
	// }

	// if (keyJustPressed('2')) {
	// 	game->currentConvoIndex = 0;
	// 	game->currentDialogEntryIndex = 0;
	// }

	bool showZIcon = false;
	/// Update actors
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];

		if (actor->prevState != actor->state) {
			actor->prevState = actor->state;
			actor->stateTime = 0;
		}	

		if (actor->type == ACTOR_PLAYER) {
			if (!game->inEditor) {
				Vec2 target = player->position;
				if (game->cameraBounds.width > game->baseRes.x) {
					if (target.x < game->cameraBounds.x + game->baseRes.x/2) target.x = game->cameraBounds.x + game->baseRes.x/2;
					if (target.x > game->cameraBounds.x + game->cameraBounds.width - game->baseRes.x/2) target.x = game->cameraBounds.x + game->cameraBounds.width - game->baseRes.x/2;
				} else {
					target.x = getCenter(game->cameraBounds).x;
				}

				if (game->cameraBounds.height > game->baseRes.y) {
					if (target.y < game->cameraBounds.y + game->baseRes.y/2) target.y = game->cameraBounds.y + game->baseRes.y/2;
					if (target.y > game->cameraBounds.y + game->cameraBounds.height - game->baseRes.y/2) target.y = game->cameraBounds.y + game->cameraBounds.height - game->baseRes.y/2;
				} else {
					target.y = getCenter(game->cameraBounds).y;
				}

				game->cameraTarget = lerp(game->cameraTarget, target, 0.1);
			}

			bool canMove = true;
			if (game->inEditor) canMove = false;
			if (game->currentConvoIndex != -1) canMove = false;
			if (game->nextState != GAME_OVERWORLD) canMove = false;
			if (game->inPauseBeforeBattle) canMove = false;

			Vec2 moveInput = v2();
			if (canMove) {
				if (keyPressed(KEY_LEFT) || keyPressed('A')) moveInput.x--;
				if (keyPressed(KEY_RIGHT) || keyPressed('D')) moveInput.x++;
				if (keyPressed(KEY_UP) || keyPressed('W')) moveInput.y--;
				if (keyPressed(KEY_DOWN) || keyPressed('S')) moveInput.y++;
				moveInput = moveInput.normalize();
			}

			Vec2 velo = moveInput * 2;
			Vec2 newPosition = actor->position + velo;

			bool beenCorrected = false;
			Rect rect = actor->rect.inflate(-2);

			Rect rectX = rect;
			rectX.x += velo.x;
			if (isOnCollision(rectX)) {
				newPosition.x = actor->position.x;
				beenCorrected = true;
			}

			Rect rectY = rect;
			rectY.y += velo.y;
			if (isOnCollision(rectY)) {
				newPosition.y = actor->position.y;
				beenCorrected = true;
			}

			Rect newRect = rect;
			newRect.x += velo.x;
			newRect.y += velo.y;
			if (!beenCorrected && isOnCollision(newRect)) {
				newPosition = actor->position;
			}

			actor->position = newPosition;
		} else if (actor->type == ACTOR_DOOR) {
			if (!streq(actor->nextMapName, game->standingOnDoorFromPrevMapName)) {
				if (contains(actor->rect, player->rect.inflate(-2))) {
					if (game->nextMapIndex == -1) {
						for (int i = 0; i < MAPS_MAX; i++) {
							Map *possibleMap = &game->maps[i];
							if (streq(possibleMap->name, actor->nextMapName)) {
								game->nextMapIndex = i;
								break;
							}
						}
					}
				}
			}
		} else if (actor->type == ACTOR_NPC) {
			if (contains(actor->rect, player->rect)) {
				showZIcon = true;
				if (game->currentConvoIndex == -1 && keyJustPressed('Z')) {
					startConversation(actor->convoName);
				}
			}
		} else if (actor->type == ACTOR_ITEM) {
			if (contains(actor->rect, player->rect)) {
				showZIcon = true;
				if (game->currentConvoIndex == -1 && keyJustPressed('Z')) {
					if (game->itemsNum > ITEMS_MAX-1) {
						logf("You have too many items...\n");
					} else {
						Item *item = &game->items[game->itemsNum++];
						item->actionType = actor->itemActionType;
						item->amount = actor->itemAmount;
						logf("You got %s x%d\n", actionTypeStrings[item->actionType], item->amount);
						actor->markedForDeletion = true;
					}
				}
			}
		} else if (actor->type == ACTOR_ENEMY) {
			bool canChasePlayer = true;
			if (isOnCollision(makeLine2(actor->position, player->position))) canChasePlayer = false;
			if (distance(actor->position, player->position) > 100) canChasePlayer = false;

			if (actor->state == ACTOR_STATE_STANDING) {
				if (actor->stateTime > rndFloat(3, 5)) {
					for (int i = 0; i < 10; i++) {
						Vec2 targetPosition = actor->position;
						targetPosition.x += rndFloat(-50, 50);
						targetPosition.y += rndFloat(-50, 50);
						if (!isOnCollision(makeLine2(actor->position, targetPosition))) {
							actor->targetPosition = targetPosition;
							actor->state = ACTOR_STATE_MOVING;
							break;
						}
					}
				}
			} else if (actor->state == ACTOR_STATE_MOVING) {
				actor->position = moveTowards(actor->position, actor->targetPosition, 3);
				if (equal(actor->position, actor->targetPosition)) actor->state = ACTOR_STATE_STANDING;
			} else if (actor->state == ACTOR_STATE_CHASING) {
				Vec2 targetPoint = player->rect.getClosestPoint(actor->position);
				actor->position = moveTowards(actor->position, targetPoint, 3);
				if (!canChasePlayer) actor->state = ACTOR_STATE_STANDING;
			}

			if (contains(player->rect, actor->rect) && !game->inPauseBeforeBattle && !actor->markedForDeletion) {
				game->inPauseBeforeBattle = true;
			}

			if (canChasePlayer) actor->state = ACTOR_STATE_CHASING;
			if (game->nextState != GAME_OVERWORLD) actor->state = ACTOR_STATE_STANDING;
		} else {
		}

		actor->trueVelo = actor->position - actor->prevPosition;

		if (!isZero(actor->trueVelo)) {
			if (fabs(actor->trueVelo.x) > fabs(actor->trueVelo.y)) {
				if (actor->trueVelo.x < 0) actor->direction = DIR_LEFT;
				else actor->direction = DIR_RIGHT;
			} else {
				if (actor->trueVelo.y < 0) actor->direction = DIR_UP;
				else actor->direction = DIR_DOWN;
			}
			actor->prevPosition = actor->position;

			actor->timeLastMoved = game->time;
		} else {
			actor->timeLastStoodStill = game->time;
		}

		actor->stateTime += elapsed;
	}

	if (game->standingOnDoorFromPrevMapName[0]) {
		bool stillStandingOnDoor = false;
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->type == ACTOR_DOOR && contains(actor->rect, player->rect.inflate(2)) && streq(actor->nextMapName, game->standingOnDoorFromPrevMapName)) {
				stillStandingOnDoor = true;
			}
		}

		if (!stillStandingOnDoor) game->standingOnDoorFromPrevMapName[0] = 0;
	}

	/// Draw actors
	Actor **drawOrder = (Actor **)frameMalloc(sizeof(Actor *) * map->actorsNum);
	int drawOrderNum = 0;
	for (int i = 0; i < map->actorsNum; i++) drawOrder[drawOrderNum++] = &map->actors[i];

	auto qsortActors = [](const void *a, const void *b)->int {
		Actor *actorA = *(Actor **)a;
		Actor *actorB = *(Actor **)b;

		Rect rectA = actorA->rect;
		if (actorA->info->usesImageMatrix) rectA = getImageMatrix(actorA) * makeRect(0, 0, 1, 1);
		Rect rectB = actorB->rect;
		if (actorB->info->usesImageMatrix) rectB = getImageMatrix(actorB) * makeRect(0, 0, 1, 1);
		float botA = rectA.y + rectA.height;
		float botB = rectB.y + rectB.height;
		return botA - botB;
	};

	qsort(drawOrder, drawOrderNum, sizeof(Actor *), qsortActors);

	for (int pass = 0; pass < 2; pass++) {
		for (int i = 0; i < drawOrderNum; i++) {
			Actor *actor = drawOrder[i];
			if (actor->hidden) continue;
			if (pass == 0 && (actor->flags & _FA_ALWAYS_ON_TOP)) continue;
			if (pass == 1 && !(actor->flags & _FA_ALWAYS_ON_TOP)) continue;

			pushAlpha(1.0 - actor->overlapFade);

			if (actor->type == ACTOR_PLAYER) {
				// drawCircle(actor->position, actor->size.x, 0xFF0000FF);

				Frame *frame = NULL;
				if (isZero(actor->trueVelo)) {
					Animation *anim = getAnimation(frameSprintf("zoo/idle_%s", directionStrings[actor->direction]));
					frame = getAnimFrameAtSecond(anim, game->time - actor->timeLastMoved);
				} else {
					Animation *anim = getAnimation(frameSprintf("zoo/walk_%s", directionStrings[actor->direction]));
					frame = getAnimFrameAtSecond(anim, game->time - actor->timeLastStoodStill);
				}

				if (frame) {
					Vec2 spritePos;
					spritePos.x = actor->position.x;
					spritePos.y = actor->position.y -frame->height/2 + actor->info->size.y/2 - 2;

					Matrix3 matrix = mat3();
					matrix.TRANSLATE(spritePos);
					matrix.TRANSLATE(frame->destOffX, frame->destOffY);
					matrix.TRANSLATE(-frame->srcWidth/2, -frame->srcHeight/2);
					matrix.SCALE(frame->width, frame->height);

					Matrix3 uvMatrix = mat3();
					uvMatrix.TRANSLATE(frame->srcX / (float)frame->texture->width, frame->srcY / (float)frame->texture->height);
					uvMatrix.SCALE(frame->width / (float)frame->texture->width, frame->height / (float)frame->texture->height);

					if (game->debugDrawSpritesWithBilinear) {
						setTextureSmooth(frame->texture, true);
					} else {
						setTextureSmooth(frame->texture, false);
					}

					if (game->debugDrawSpritesWithPixelFiltering) {
						Vec2 uv0 = uvMatrix * v2(0, 0);
						Vec2 uv1 = uvMatrix * v2(1, 1);
						// drawSimpleTexture(frame->texture, matrix, uv0, uv1);
						drawPixelArtFilterTexture(frame->texture, matrix, uv0, uv1);
					} else {
						RenderProps props = newRenderProps();
						props.matrix = matrix;
						props.uvMatrix = uvMatrix;
						props.srcWidth = 1;
						props.srcHeight = 1;

						drawTexture(frame->texture, props);
					}
				}

			} else if (actor->type == ACTOR_SPAWNER) {
				if (game->inEditor) {
					if (game->selectedActor == actor->id) drawCircle(actor->position, actor->spawnRadius, lerpColor(0x00FF0000, 0x80FF0000, secondPhase));
					// drawRect(actor->rect, 0xFF404040);
					drawCircle(actor->position, actor->info->size.x/2, 0xFF404040);
					DrawTextProps props = newDrawTextProps();
					props.font = game->defaultFont;
					props.color = 0xFF000000;
					drawTextInRect("Spawner", props, actor->rect);
				}
			} else if (actor->type == ACTOR_DOOR) {
				drawRect(actor->rect, 0xFF7D682D);
			} else if (actor->type == ACTOR_NPC) {
				drawCircle(actor->position, actor->info->size.x/2, 0xFF007DA6);
				DrawTextProps props = newDrawTextProps();
				props.font = game->defaultFont;
				props.color = 0xFF000000;
				drawTextInRect("NPC", props, actor->rect);
			} else if (actor->type == ACTOR_ENEMY) {
				drawCircle(actor->position, actor->info->size.x/2, 0xFFFF0000);
				DrawTextProps props = newDrawTextProps();
				props.font = game->defaultFont;
				props.color = 0xFF000000;
				drawTextInRect(fighterTypeStrings[actor->fighterType], props, actor->rect);
			} else if (actor->type == ACTOR_ITEM) {
				drawCircle(actor->position, actor->info->size.x/2, 0xFF00FFFF);
				DrawTextProps props = newDrawTextProps();
				props.font = game->defaultFont;
				props.color = 0xFF000000;
				drawTextInRect("Item", props, actor->rect);
			} else if (actor->type == ACTOR_IMAGE) {
				Matrix3 imageMatrix = getImageMatrix(actor);

				Texture *texture = getTexture(actor->imagePath);
				if (texture) {
					setTextureClamped(texture, true);

					if (game->debugDrawSpritesWithBilinear) {
						setTextureSmooth(texture, true);
					} else {
						setTextureSmooth(texture, false);
					}
					if (game->debugDrawSpritesWithPixelFiltering) {
						drawPixelArtFilterTexture(texture, imageMatrix);
					} else {
						drawSimpleTexture(texture, imageMatrix);
					}
				} else {
					RenderProps props = newRenderProps();
					props.matrix = imageMatrix;
					props.srcWidth = props.srcHeight = 1;
					props.tint = 0xFFFFFF00;
					props.alpha = 0.5;
					drawTexture(renderer->whiteTexture, props);
					// drawSimpleTexture(renderer->whiteTexture, imageMatrix, v2(0, 0), v2(1, 1), 0.25);
					// if (game->inEditor) drawPixelArtFilterTexture(renderer->whiteTexture, imageMatrix, v2(0, 0), v2(1, 1), 0.25);
				}

				if (actor->flags & _FA_FADES_WHEN_OVERLAPS) {
					if (rectOverlapsImage(player->rect, actor)) {
						actor->overlapFade += 0.05;
					} else {
						actor->overlapFade -= 0.05;
					}
					actor->overlapFade = mathClamp(actor->overlapFade, 0, 0.5);
				}

				if (game->inEditor) {
					pushCamera2d(imageMatrix);
					drawRect(actor->collisionRect, lerpColor(0xA0FF0000, 0x80FF0000, secondPhase));
					popCamera2d();
				}

				if (game->debugDrawBottomOfImages) {
					Rect rect = getImageMatrix(actor) * makeRect(0, 0, 1, 1);
					Vec2 bot;
					bot.x = rect.x + rect.width/2;
					bot.y = rect.y + rect.height;
					drawCircle(bot, 5, 0xFFFF0000);
				}
			} else if (actor->type == ACTOR_FOCUS_AREA) {
				Matrix3 imageMatrix = getImageMatrix(actor);

				if (game->inEditor) {
					RenderProps props = newRenderProps();
					props.matrix = imageMatrix;
					props.srcWidth = props.srcHeight = 1;
					props.tint = 0xFFFF00FF;
					props.alpha = 0.5;
					drawTexture(renderer->whiteTexture, props);
				}
			} else {
				drawCircle(actor->position, actor->info->size.x/2, 0xFFFFFF00);
			}

			if (game->inEditor) {
				int drawLinesColor = 0;
				if (game->selectedActor == actor->id) {
					drawLinesColor = lerpColor(0x80FF00FF, 0xF0FF00FF, secondPhase);

					Actor *parent = getActor(actor->parentId);
					if (parent) {
						drawLine(getPosition(getImagePointMatrix(parent)), getPosition(getImagePointMatrix(actor)), 4, 0x8000FF00);
					}

				} else if (doesActorContain(actor, game->mouse)) {
					drawLinesColor = 0xF0A0A000;
				}

				if (drawLinesColor) {
					Rect rect = actor->rect;
					if (actor->type == ACTOR_IMAGE || actor->type == ACTOR_FOCUS_AREA) rect = makeRect(0, 0, 1, 1);

					Line2 lines[4];
					toLines(rect, lines);

					Matrix3 imageMatrix = getImageMatrix(actor);
					for (int i = 0; i < 4; i++) {
						Line2 line = lines[i];
						if (actor->type == ACTOR_IMAGE || actor->type == ACTOR_FOCUS_AREA) line = imageMatrix * line;
						drawLine(line, 2, drawLinesColor);
					}
				}
			}

			popAlpha();
		}
	}

	{ /// Focus areas
		bool hittingSameFocusArea = false;
		Actor *focusArea = NULL;
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->type == ACTOR_FOCUS_AREA) {
				actor->hidden = game->debugHideFocusAreas;
				Rect rect = makeRect(actor->position - actor->scale/2, actor->scale);
				if (contains(player->rect, rect)) {
					if (actor->id == game->currentFocusAreaId) hittingSameFocusArea = true;
					focusArea = actor;
				}
			}
		}

		game->cameraBounds = makeRect(v2(), v2(map->size) * game->tileSize);
		if (hittingSameFocusArea) focusArea = getActor(game->currentFocusAreaId);
		if (focusArea) {
			if (game->currentFocusAreaId != focusArea->id) {
				game->currentFocusAreaId = focusArea->id;
				game->timeFocusAreaChanged = game->time;
			}

			game->cameraBounds = makeRect(focusArea->position - focusArea->scale/2, focusArea->scale);

			float perc = clampMap(game->time - game->timeFocusAreaChanged, 0, 0.25, 0, 1);
			int outlineColor = lerpColor(0, 0x80000000, perc);

			Rect topRect = makeRect(0, 0, game->baseRes.x*3, game->baseRes.y);
			topRect.x = game->cameraBounds.x + game->cameraBounds.width/2 - topRect.width/2;
			topRect.y = game->cameraBounds.y - topRect.height;
			drawRect(topRect, outlineColor);

			Rect rightRect = makeRect(0, 0, game->baseRes.x, game->baseRes.y);
			rightRect.x = game->cameraBounds.x + game->cameraBounds.width;
			rightRect.y = game->cameraBounds.y;
			drawRect(rightRect, outlineColor);

			Rect leftRect = makeRect(0, 0, game->baseRes.x, game->baseRes.y);
			leftRect.x = game->cameraBounds.x - leftRect.width;
			leftRect.y = game->cameraBounds.y;
			drawRect(leftRect, outlineColor);

			Rect bottomRect = makeRect(0, 0, game->cameraBounds.width, game->baseRes.y);
			bottomRect.x = game->cameraBounds.x;
			bottomRect.y = game->cameraBounds.y + game->cameraBounds.height;
			drawRect(bottomRect, outlineColor);
		} else {
			game->currentFocusAreaId = 0;
		}
	}

	if (game->inEditor) {
		drawRectOutline(makeRect(v2(0, 0), v2(map->size)*game->tileSize), 3, 0xFF808080);
		drawCircle(game->mouse, 3, 0xFFFF0000);

		Vec2i tilemapTileHovering = v2i(game->mouse / game->tileSize);
		tilemapTileHovering.x = mathClamp(tilemapTileHovering.x, 0, map->size.x-1);
		tilemapTileHovering.y = mathClamp(tilemapTileHovering.y, 0, map->size.y-1);
		int tilemapTileIndexHovering = tilemapTileHovering.y * map->size.x + tilemapTileHovering.x;

		Rect rectHovering = makeRect(v2(), v2(game->tileSize, game->tileSize));
		rectHovering.x = (tilemapTileIndexHovering % map->size.x) * game->tileSize;
		rectHovering.y = (tilemapTileIndexHovering / map->size.x) * game->tileSize;

		if (game->editorMode == EDITOR_TILEMAP) {
			{
				Vec4 uvuv = getTileUvs(game->debugSelectedTilesetTile);
				Vec2 uv0 = v2(uvuv.x, uvuv.y);
				Vec2 uv1 = v2(uvuv.z, uvuv.w);

				Matrix3 mat = mat3();
				mat.TRANSLATE(game->mouse);
				mat.SCALE(game->tileSize);
				mat.TRANSLATE(-v2(0.5, 0.5));

				drawSimpleTexture(tileset, mat, uv0, uv1, 0.25);
			}

			drawRectOutline(rectHovering, 1, lerpColor(0x00FF0000, 0x80FF0000, secondPhase));
			if (platform->mouseDown) {
				if (keyPressed(KEY_SHIFT)) {
					game->debugSelectedTilesetTile = map->tiles[tilemapTileIndexHovering].tilesetIndex;
					game->debugRegenerateTilesetSelector = true;
				} else {
					map->tiles[tilemapTileIndexHovering].tilesetIndex = game->debugSelectedTilesetTile;
				}
			}
		} else if (game->editorMode == EDITOR_COLLISION) {
			if (platform->mouseDown) {
				map->tiles[tilemapTileIndexHovering].flags |= _F_MAP_TILE_COLLIDABLE;
			}
			if (platform->rightMouseDown) {
				map->tiles[tilemapTileIndexHovering].flags &= ~_F_MAP_TILE_COLLIDABLE;
			}
		} else if (game->editorMode == EDITOR_ACTOR) {
			bool selectedSomething = false;
			Line2 attachLine = makeLine2();

			if (game->attachingActor) {
				Actor *selectedActor = getActor(game->selectedActor);
				attachLine.start = selectedActor->position;
				attachLine.end = game->mouse;
			}

			for (int i = 0; i < map->actorsNum; i++) {
				Actor *actor = &map->actors[i];

				if (game->debugHideFocusAreas && actor->type == ACTOR_FOCUS_AREA) continue;

				if (game->attachingActor) {
					if (actor->type != ACTOR_IMAGE) continue;

					Actor *selectedActor = getActor(game->selectedActor);

					if (doesActorContain(actor, game->mouse)) {
						attachLine.end = getPosition(getImagePointMatrix(actor));

						if (platform->mouseJustUp) {
							logf("Attached!\n");
							Matrix3 matrix = getImagePointMatrix(actor).invert() * getImagePointMatrix(selectedActor);
							selectedActor->position = getPosition(matrix);
							selectedActor->rotation = getRotationDeg(matrix);
							selectedActor->scale = getScale(matrix);

							selectedActor->parentId = actor->id;

							game->attachingActor = false;
							selectedSomething = true;
						}
					}
				} else {
					if (platform->mouseJustUp && !game->draggingActor && doesActorContain(actor, game->mouse)) {
						game->selectedActor = actor->id;
						selectedSomething = true;
					}
				}
			}

			if (!isZero(attachLine)) drawLine(attachLine, 2, 0x8000FF00);

			if (platform->mouseJustUp && !game->draggingActor && !selectedSomething) {
				if (game->attachingActor) {
					logf("Attached nothing...\n");
					game->attachingActor = false;
				} else {
					game->selectedActor = 0;
				}
			}
		}

		auto getParentMatrix = [](Actor *actor)->Matrix3 {
			Actor *parent = getActor(actor->parentId);
			if (parent) return getImagePointMatrix(parent);
			return mat3();
		};

		Actor *actor = getActor(game->selectedActor);
		if (actor) {
			if (platform->mouseDown && doesActorContain(actor, game->mouse)) {

				if (!game->draggingActor) game->draggingActorOffset = getParentMatrix(actor).invert()*game->mouse - actor->position;
				game->draggingActor = true;
			}

			if (!platform->mouseDown || game->editorMode != EDITOR_ACTOR) game->draggingActor = false;

			if (game->draggingActor) {
				Vec2 mouse = game->mouse - game->draggingActorOffset;

				mouse = getParentMatrix(actor).invert() * mouse;

				actor->position = mouse;
				if (keyPressed(KEY_SHIFT)) {
					actor->position.x = roundToNearest(actor->position.x + game->tileSize/2, game->tileSize) - game->tileSize/2; // This works I guess...
					actor->position.y = roundToNearest(actor->position.y + game->tileSize/2, game->tileSize) - game->tileSize/2;
				}
			}
		}
	}

	/// Delete actors
	if (!game->inPauseBeforeBattle) {
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->markedForDeletion) {
				deleteActor(actor);
			}
		}
	}

	if (showZIcon) {
		Rect rect = player->rect;
		rect = inflatePerc(rect, -0.5);
		rect.y -= 30;

		DrawTextProps props = newDrawTextProps();
		props.font = game->defaultFont;
		props.color = lerpColor(0xFF000000, 0xFFFFFFFF, secondPhase);
		drawTextInRect("[Z]", props, rect);
	}

	if (game->inPauseBeforeBattle) {
		game->pauseBeforeBattleTime += elapsed;
		if (game->pauseBeforeBattleTime > 3 && game->nextState != GAME_FIGHT) {
			Fight *fight = &game->fight;
			memset(fight, 0, sizeof(Fight));

			for (int i = 0; i < map->actorsNum; i++) {
				Actor *actor = &map->actors[i];
				if (actor->type == ACTOR_ENEMY && contains(player->rect, actor->rect)) {
					actor->markedForDeletion = true;
					{ /// Create/add fighter
						Fighter *fighter = &fight->fighters[fight->fightersNum++];
						memset(fighter, 0, sizeof(Fighter));
						fighter->id = ++fight->nextFighterId;
						fighter->type = actor->fighterType;
						resetFighter(fighter);
					}
				}
			}

			for (int i = 0; i < game->playerFightersNum; i++) {
				game->playerFighters[i].id = ++fight->nextFighterId;
				fight->fighters[fight->fightersNum++] = game->playerFighters[i];
			}

			game->nextState = GAME_FIGHT;
		}
	} else {
		game->pauseBeforeBattleTime = 0;
	}

	if (game->nextMapIndex != -1) {
		game->mapTransition_t += 0.1;
	} else {
		game->mapTransition_t -= 0.1;
	}
	game->mapTransition_t = Clamp01(game->mapTransition_t);

	if (game->mapTransition_t >= 1) {
		strcpy(game->standingOnDoorFromPrevMapName, map->name);

		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			bool willDespawn = false;
			if (actor->type == ACTOR_PLAYER) willDespawn = true;
			if (actor->type == ACTOR_ENEMY) willDespawn = true;
			if (willDespawn) deleteActor(actor);
		}
		game->initedMap = false;
		game->currentMap = game->nextMapIndex;

		game->nextMapIndex = -1;
	}

	popCamera2d();

	if (game->currentConvoIndex != -1) {
		Conversation *convo = &game->convos[game->currentConvoIndex];

		Rect convoRect = makeRect(v2(), game->size*v2(1, 0.25));
		convoRect.y = game->size.y - convoRect.height;

		if (game->currentDialogEntryIndex < convo->entriesNum) {
			DialogEntry *entry = &convo->entries[game->currentDialogEntryIndex];

			drawRect(convoRect, 0xFF100000);
			char *str = (char *)zalloc(game->currentDialogEntryCharacterIndex+1);
			strncpy(str, entry->dialog, game->currentDialogEntryCharacterIndex);
			DrawTextProps textProps = newDrawTextProps();
			textProps.font = game->convoFont;
			textProps.position = getPosition(convoRect) + v2(5, 5);
			textProps.color = 0xFFFFFFFF;
			textProps.maxWidth = convoRect.width - 10;
			textProps.scale = v2(game->sizeScale, game->sizeScale);
			drawText(str, textProps);
			// drawText(game->convoFont, str, getPosition(convoRect) + v2(5, 5), 0xFFFFFFFF, convoRect.width - 10);

			game->timeTillNextDialogCharacter -= elapsed;
			if (game->timeTillNextDialogCharacter <= 0) {
				game->timeTillNextDialogCharacter = 0.01;
				if (game->currentDialogEntryCharacterIndex < strlen(entry->dialog)) {
					game->currentDialogEntryCharacterIndex++;
				} else {
					showZIcon = true;
					if (keyJustPressed('Z')) {
						game->currentDialogEntryIndex++;
						game->currentDialogEntryCharacterIndex = 0;
					}
				}
			}
		} else {
			game->currentConvoIndex = -1;
		}
	}

	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, game->mapTransition_t)); // Not far enough

	float redPerc = clampMap(game->pauseBeforeBattleTime, 0, 1, 0, 1);
	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0x80FF0000, redPerc)); // Not far enough
}

Vec4 getTileUvs(int tilesetIndex) {
	Texture *tileset = getTexture("assets/images/tileset.png");
	int tilesetTilesWide = tileset->width/game->tileSize;

	Vec2 tilesetTileStart;
	tilesetTileStart.x = (tilesetIndex % tilesetTilesWide) * game->tileSize;
	tilesetTileStart.y = (tilesetIndex / tilesetTilesWide) * game->tileSize;
	Vec2 uv0 = tilesetTileStart / getSize(tileset);
	Vec2 uv1 = uv0 + v2(game->tileSize, game->tileSize)/getSize(tileset);
	return v4(uv0.x, uv0.y, uv1.x, uv1.y);
}

Vec4 getPaddedTileUvs(int tilesetIndex) {
	Texture *tileset = getTexture("assets/images/tileset.png");
	int tilesetTilesWide = tileset->width/game->tileSize;

	int tileX = tilesetIndex % tilesetTilesWide;
	int tileY = tilesetIndex / tilesetTilesWide;

	Vec2 tilesetTileStart;
	tilesetTileStart.x = (tileX*game->tileSize) + (tileX+1)*game->tilesetPad;
	tilesetTileStart.y = (tileY*game->tileSize) + (tileY+1)*game->tilesetPad;
	Vec2 uv0 = tilesetTileStart / getSize(game->paddedTileset);
	Vec2 uv1 = uv0 + v2(game->tileSize, game->tileSize)/getSize(game->paddedTileset);
	return v4(uv0.x, uv0.y, uv1.x, uv1.y);
}

Actor *createActor() {
	Map *map = &game->maps[game->currentMap];

	if (map->actorsNum > ACTORS_MAX-1) {
		logf("Out of actors!!!\n");
		return NULL;
	}

	Actor *actor = &map->actors[map->actorsNum++];
	memset(actor, 0, sizeof(Actor));
	actor->id = ++map->nextActorId;
	actor->info = &game->actorTypeInfos[actor->type];
	actor->scale = v2(1, 1);
	return actor;
}

void deleteActor(Actor *actor) {
	Map *map = &game->maps[game->currentMap];

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

	Map *map = &game->maps[game->currentMap];
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->id == id) return actor;
	}
	return NULL;
}

Actor *getActor(char *name) {
	if (!name || !name[0]) return NULL;

	Map *map = &game->maps[game->currentMap];
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (streq(actor->name, name)) return actor;
	} 

	return NULL;
}

Actor *pasteActor(Actor *srcActor, int srcActorMapIndex, Actor *parent) {
	Map *srcMap = &game->maps[srcActorMapIndex];

	Actor *newActor = createActor();
	if (!newActor) return NULL;

	int id = newActor->id;
	*newActor = *srcActor;
	newActor->id = id;
	if (!keyPressed(KEY_SHIFT) && !parent) newActor->position = game->mouse;
	newActor->parentId = 0;
	if (parent) newActor->parentId = parent->id;

	for (int i = 0; i < srcMap->actorsNum; i++) {
		Actor *possibleChild = &srcMap->actors[i];
		if (possibleChild->parentId == srcActor->id) {
			pasteActor(possibleChild, srcActorMapIndex, newActor);
		}
	}


	return newActor;
}

// Vec2i worldToTile(Vec2 worldPosition) {
// 	Map *map = &game->maps[game->currentMap];
// 	Vec2i tilemapPosition = v2i(worldPosition / game->tileSize);
// 	if (tilemapPosition.x < 0) tilemapPosition.x = 0;
// 	if (tilemapPosition.x > map->size.x-1) tilemapPosition.x = map->size.x-1;
// 	if (tilemapPosition.y < 0) tilemapPosition.y = 0;
// 	if (tilemapPosition.y > map->size.y-1) tilemapPosition.y = map->size.y-1;
// 	return tilemapPosition;
// }

// bool overlaps(Actor *actor1, Actor *actor2) {
// 	if (actor1->type != ACTOR_IMAGE && actor2->type != ACTOR_IMAGE) {
// 		return contains(actor1->rect, actor2->rect);
// 	}

// 	if (actor1->type == ACTOR_IMAGE && actor2->type == ACTOR_IMAGE) {
// 		logf("Cannot overlap test 2 images!\n");
// 		return false;
// 	}

// 	Actor *imageActor;
// 	Actor *nonImageActor;

// 	if (actor1->type == ACTOR_IMAGE) {
// 		imageActor = actor1;
// 		nonImageActor = actor2;
// 	}

// 	Line2 lines[4];
// 	toLines(makeRect(0, 0, 1, 1), lines);
// 	Matrix3 imageMatrix = getImageMatrix(imageActor);
// 	for (int i = 0; i < 4; i++) {
// 		Line2 line = lines[i];
// 		line = imageMatrix * line;
// 		if (overlaps(nonImageActor->rect, line)) return true;
// 	}

// 	return false;
// }

bool isTileCollidable(Vec2i tilemapPosition) {
	Map *map = &game->maps[game->currentMap];

	if (tilemapPosition.x < 0) return true;
	if (tilemapPosition.x > map->size.x-1) return true;
	if (tilemapPosition.y < 0) return true;
	if (tilemapPosition.y > map->size.y-1) return true;

	int tilemapIndex = tilemapPosition.y * map->size.x + tilemapPosition.x;
	MapTile *tile = &map->tiles[tilemapIndex];
	if (tile->flags & _F_MAP_TILE_COLLIDABLE) return true;
	return false;
}

bool isOnCollision(Rect rect) {
	Map *map = &game->maps[game->currentMap];

	Vec2i min = v2i(getPosition(rect)/ game->tileSize);
	Vec2i max = v2i((getPosition(rect) + getSize(rect)) / game->tileSize);

	for (int y = min.y; y <= max.y; y++) {
		for (int x = min.x; x <= max.x; x++) {
			if (isTileCollidable(v2i(x, y))) return true;
		}
	}

	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->type == ACTOR_IMAGE && !isZero(actor->collisionRect)) {
			if (rectOverlapsImage(rect, actor, true)) return true;
		}
	}

	return false;
}

bool isOnCollision(Line2 worldLine) {
	Map *map = &game->maps[game->currentMap];

	Vec2i start = v2i(worldLine.start / game->tileSize);
	Vec2i goal = v2i(worldLine.end / game->tileSize);

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
		if (isTileCollidable(v2i(x0, y0))) return true;

		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}

	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->type == ACTOR_IMAGE && !isZero(actor->collisionRect)) {
			Line2 lines[4];
			toLines(actor->collisionRect, lines);
			Matrix3 imageMatrix = getImageMatrix(actor);
			for (int i = 0; i < 4; i++) {
				Line2 line = lines[i];
				line = imageMatrix * line;
				if (overlaps(line, line)) return true;
			}
		}
	}

	return false;
}

bool rectOverlapsImage(Rect rect, Actor *imageActor, bool useCollisionRect) {
	if (imageActor->type != ACTOR_IMAGE) {
		logf("Tried to overlap with a non-image actor\n");
		return false;
	}

	Tri2 rectTri0;
	Tri2 rectTri1;
	toTris(rect, &rectTri0, &rectTri1);

	Tri2 imageTri0;
	Tri2 imageTri1;
	toTris(useCollisionRect?imageActor->collisionRect:makeRect(0, 0, 1, 1), &imageTri0, &imageTri1);

	Matrix3 imageMatrix = getImageMatrix(imageActor);
	imageTri0 = imageMatrix * imageTri0;
	imageTri1 = imageMatrix * imageTri1;

	Line2 lines[12];
	toLines(rectTri0, &lines[0]);
	toLines(rectTri1, &lines[3]);
	toLines(imageTri0, &lines[6]);
	toLines(imageTri1, &lines[9]);

	// pushTargetTexture(game->debugTexture);
	// for (int i = 0; i < ArrayLength(lines); i++) {
	// 	drawLine(lines[i], 2, 0xFFFF0000);
	// }
	// popTargetTexture();

	if (overlaps(rectTri0, imageTri0)) return true;
	if (overlaps(rectTri1, imageTri0)) return true;
	if (overlaps(rectTri0, imageTri1)) return true;
	if (overlaps(rectTri1, imageTri1)) return true;
	return false;
}


void startConversation(char *convoName) {
	for (int i = 0; i < game->convosNum; i++) {
		Conversation *convo = &game->convos[i];
		if (streq(convo->name, convoName)) {
			game->currentConvoIndex = i;
			game->currentDialogEntryIndex = 0;
			return;
		}
	}
	logf("Couldn't find conversation named %s\n", convoName);
}

void saveMap(Map *map, int mapFileIndex) {
	DataStream *stream = newDataStream();

	writeU32(stream, MAP_VERSION);

	writeString(stream, map->name);
	writeVec2i(stream, map->size);

	for (int i = 0; i < map->size.x*map->size.y; i++) {
		MapTile *tile = &map->tiles[i];
		writeU32(stream, tile->tilesetIndex);
		writeU32(stream, tile->flags);
	}

	int actorsToSave = 0;
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		bool willSave = true;
		if (actor->type == ACTOR_PLAYER) willSave = false;
		if (actor->type == ACTOR_ENEMY) willSave = false;
		if (actor->type == ACTOR_ITEM) willSave = false;
		if (willSave) actorsToSave++;
	}

	writeU32(stream, actorsToSave);
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		bool willSave = true;
		if (actor->type == ACTOR_PLAYER) willSave = false;
		if (actor->type == ACTOR_ENEMY) willSave = false;
		if (actor->type == ACTOR_ITEM) willSave = false;
		if (!willSave) continue;

		writeU32(stream, actor->type);
		writeString(stream, actor->name);
		writeU32(stream, actor->id);
		writeU32(stream, actor->parentId);
		writeVec2(stream, actor->position);
		writeFloat(stream, actor->rotation);
		writeVec2(stream, actor->scale);
		writeRect(stream, actor->collisionRect);
		writeString(stream, actor->nextMapName);
		writeString(stream, actor->convoName);
		writeU32(stream, actor->itemActionType);
		writeU32(stream, actor->itemAmount);

		for (int i = 0; i < SPAWN_ENTRIES_MAX; i++) {
			SpawnEntry *entry = &actor->spawnEntries[i];
			writeU8(stream, entry->isItem);
			writeU32(stream, entry->fighterOrItemType);
			writeU32(stream, entry->chances);
		}
		writeU32(stream, actor->amountToSpawn);
		writeFloat(stream, actor->spawnRadius);
		writeU32(stream, actor->fighterType);
		writeString(stream, actor->imagePath);
		writeU32(stream, actor->flags);
	}
	writeU32(stream, map->nextActorId);

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

	int version = readU32(stream);

	readStringInto(stream, map->name, MAP_NAME_MAX_LEN);
	map->size = readVec2i(stream);

	map->tiles = (MapTile *)zalloc(sizeof(MapTile) * map->size.x*map->size.y);
	for (int i = 0; i < map->size.x*map->size.y; i++) {
		MapTile *tile = &map->tiles[i];
		tile->tilesetIndex = readU32(stream);
		tile->flags = readU32(stream);
	}

		map->actorsNum = readU32(stream);
		memset(map->actors, 0, sizeof(Actor) * ACTORS_MAX);
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			actor->type = (ActorType)readU32(stream);
			actor->info = &game->actorTypeInfos[actor->type];
			if (version >= 2) readStringInto(stream, actor->name, ACTOR_NAME_MAX_LEN);
			actor->id = readU32(stream);
			if (version >= 3) actor->parentId = readU32(stream);
			actor->position = readVec2(stream);
			actor->rotation = readFloat(stream);
			actor->scale = readVec2(stream);
			if (version >= 2) actor->collisionRect = readRect(stream);
			if (version <= 1) readU32(stream); // typeToSpawn
			readStringInto(stream, actor->nextMapName, MAP_NAME_MAX_LEN);
			readStringInto(stream, actor->convoName, CONVERSATION_NAME_MAX_LEN);
			actor->itemActionType = (ActionType)readU32(stream);
			actor->itemAmount = readU32(stream);
			for (int i = 0; i < SPAWN_ENTRIES_MAX; i++) {
				SpawnEntry *entry = &actor->spawnEntries[i];
				entry->isItem = readU8(stream);
				entry->fighterOrItemType = readU32(stream);
				entry->chances = readU32(stream);
			}
			actor->amountToSpawn = readU32(stream);
			actor->spawnRadius = readFloat(stream);
			actor->fighterType = (FighterType)readU32(stream);
			readStringInto(stream, actor->imagePath, PATH_MAX_LEN);
			if (version >= 4) actor->flags = readU32(stream);
		}

		map->nextActorId = readU32(stream);

	destroyDataStream(stream);
}

void saveConversations() {
	DataStream *stream = newDataStream();

	writeU32(stream, game->convosNum);
	for (int i = 0; i < game->convosNum; i++) {
		Conversation *convo = &game->convos[i];

		writeString(stream, convo->name);
		writeU32(stream, convo->entriesNum);
		for (int i = 0; i < convo->entriesNum; i++) {
			DialogEntry *entry = &convo->entries[i];
			writeString(stream, entry->dialog);
		}
	}

	writeDataStream("assets/info/conversations.bin", stream);
	destroyDataStream(stream);
}

void loadConversations() {
	DataStream *stream = loadDataStream("assets/info/conversations.bin");
	if (!stream) return;

	for (int i = 0; i < game->convosNum; i++) {
		Conversation *convo = &game->convos[i];
		free(convo->entries);
	}

	if (game->convos) free(game->convos);

	game->convosNum = readU32(stream);
	game->convos = (Conversation *)zalloc(sizeof(Conversation) * game->convosNum);

	for (int i = 0; i < game->convosNum; i++) {
		Conversation *convo = &game->convos[i];
		readStringInto(stream, convo->name, CONVERSATION_NAME_MAX_LEN);

		convo->entriesNum = readU32(stream);
		convo->entries = (DialogEntry *)zalloc(sizeof(DialogEntry) * convo->entriesNum);
		for (int i = 0; i < convo->entriesNum; i++) {
			DialogEntry *entry = &convo->entries[i];
			readStringInto(stream, entry->dialog, DIALOG_MAX_LEN);
		}
	}

	destroyDataStream(stream);
}

// Texture *getTextureOrWhiteTexture(char *path, int flags0) {
// 	Texture *texture = getTexture(path);
// 	if (!texture) texture = renderer->whiteTexture;
// 	return texture;
// }

Matrix3 getImagePointMatrix(Actor *actor) {
	Matrix3 matrix = mat3();

	if (game->debugIntegerPositions) {
		matrix.TRANSLATE(roundf(actor->position.x), roundf(actor->position.y));
	} else {
		matrix.TRANSLATE(actor->position);
	}
	matrix.ROTATE(actor->rotation);
	matrix.SCALE(actor->scale);

	Actor *parent = getActor(actor->parentId);
	if (parent) matrix = getImagePointMatrix(parent) * matrix;

	return matrix;

}

Matrix3 getImageMatrix(Actor *actor) {
	Texture *texture = getTexture(actor->imagePath);
	if (!texture) texture = renderer->whiteTexture;

	Matrix3 matrix = getImagePointMatrix(actor);
	matrix.SCALE(getSize(texture));
	matrix.TRANSLATE(-0.5, -0.5);
	return matrix;
}

bool doesActorContain(Actor *actor, Vec2 point) {
	if (actor->type == ACTOR_IMAGE || actor->type == ACTOR_FOCUS_AREA) {
		Matrix3 matrix = getImageMatrix(actor);
		point = matrix.invert() * point;
		if (contains(makeRect(0, 0, 1, 1), point)) return true;
	} else {
		if (contains(actor->rect, point)) return true;
	}

	return false;
}

//
/// Fight stuff
//

Fighter *getFighter(int id);
Fighter *getFighter(int id) {
	if (id == 0) return NULL;
	Fight *fight = &game->fight;

	for (int i = 0; i < fight->fightersNum; i++) {
		Fighter *fighter = &fight->fighters[i];
		if (fighter->id == id) return fighter;
	}

	return NULL;
}

void resetFighter(Fighter *fighter) {
	fighter->info = &game->fighterTypeInfos[fighter->type];
	fighter->hp = fighter->info->hp;
	fighter->mp = fighter->info->mp;
	fighter->speed = fighter->info->speed;
}

void updateFight() {
	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	Fight *fight = &game->fight;

	auto refillTurns = [fight]() {
		int idsLeft[FIGHTERS_MAX];
		int idsLeftNum = 0;
		for (int i = 0; i < fight->fightersNum; i++) {
			Fighter *fighter = &fight->fighters[i];
			idsLeft[idsLeftNum++] = fighter->id;
		}

		for (;;) {
			if (idsLeftNum == 0) break;

			int fastestIdLeftIndex = -1;
			int fastestIdLeftSpeed;
			for (int i = 0; i < idsLeftNum; i++) {
				Fighter *fighter = getFighter(idsLeft[i]);
				if (fastestIdLeftIndex == -1 || fighter->speed > fastestIdLeftSpeed) {
					fastestIdLeftIndex = i;
					fastestIdLeftSpeed = fighter->speed;
				}
			}

			if (fastestIdLeftIndex == -1) {
				logf("Wtf, no fastest id left index?\n");
				break;
			}

			fight->turns[fight->turnsNum++] = idsLeft[fastestIdLeftIndex];
			arraySpliceIndex(idsLeft, idsLeftNum, sizeof(int), fastestIdLeftIndex);
			idsLeftNum--;
		}

		// for (int i = 0; i < fight->turnsNum; i++) {
		// 	Fighter *fighter = getFighter(fight->turns[i]);
		// 	logf("%s\n", fighterTypeStrings[fighter->type]);
		// }
	};

	if (game->stateTime == 0) {
	}

	// Check for win/lose

	if (fight->turnsNum == 0) {
		refillTurns();
	}

	Fighter *currentFighter = getFighter(fight->turns[0]);
	while (currentFighter == NULL) {
		if (fight->turnsNum == 0) {
			refillTurns();
		}

		arraySpliceIndex(fight->turns, fight->turnsNum, sizeof(int), 0);
		fight->turnsNum--;
		currentFighter = getFighter(fight->turns[0]);
	}

	auto getRandomAlly = []()->Fighter *{
		Fight *fight = &game->fight;
		Fighter **possible = (Fighter **)frameMalloc(sizeof(Fighter *) * fight->fightersNum);
		int possibleNum = 0;
		for (int i = 0; i < fight->fightersNum; i++) {
			Fighter *fighter = &fight->fighters[i];
			if (fighter->ally) possible[possibleNum++] = fighter;
		}

		return possible[rndInt(0, possibleNum-1)];
	};

	auto getRandomEnemy = []()->Fighter *{
		Fight *fight = &game->fight;
		Fighter **possible = (Fighter **)frameMalloc(sizeof(Fighter *) * fight->fightersNum);
		int possibleNum = 0;
		for (int i = 0; i < fight->fightersNum; i++) {
			Fighter *fighter = &fight->fighters[i];
			if (!fighter->ally) possible[possibleNum++] = fighter;
		}

		return possible[rndInt(0, possibleNum-1)];
	};

	auto createAction = [](ActionType type, int src=0, int dest=0)->Action *{
		Fight *fight = &game->fight;

		Action *action = NULL;
		if (fight->actionsNum > ACTIONS_MAX-1) {
			logf("Too many fight actions!!!\n");
			action = &fight->actions[fight->actionsNum-1];
		} else {
			action = &fight->actions[fight->actionsNum++];
		}

		memset(action, 0, sizeof(Action));
		action->type = type;
		action->src = src;
		action->dest = dest;
		action->info = &game->actionTypeInfos[action->type];
		return action;
	};

	int alliesLeft = 0;
	int enemiesLeft = 0;
	for (int i = 0; i < fight->fightersNum; i++) {
		Fighter *fighter = &fight->fighters[i];
		if (fighter->ally) {
			alliesLeft++;
		} else {
			enemiesLeft++;
		}
	}

	if (alliesLeft == 0 && !fight->fightOver) {
		fight->fightOver = true;
		Action *action;

		action = createAction(ACTION_DIALOG);
		action->dialog = mallocSprintf("You have lost the battle");

		action = createAction(ACTION_DIALOG);
		action->dialog = mallocSprintf("Better luck next time!");

		createAction(ACTION_GO_TO_OVERWORLD);
	}
	if (enemiesLeft == 0 && !fight->fightOver) {
		fight->fightOver = true;
		Action *action;

		action = createAction(ACTION_DIALOG);
		action->dialog = mallocSprintf("You have beaten the enemy");

		action = createAction(ACTION_DIALOG);
		action->dialog = mallocSprintf("You should gain XP and stuff...");

		createAction(ACTION_GO_TO_OVERWORLD);
	}

	if (fight->actionsNum > 0) {
		Action *currentAction = &fight->actions[0];
		char *fightText = NULL;

		Fighter *src = getFighter(currentAction->src);
		Fighter *dest = getFighter(currentAction->dest);

		char *srcName = NULL;
		char *destName = NULL;
		if (src) srcName = fighterTypeStrings[src->type];
		if (dest) destName = fighterTypeStrings[dest->type];

		bool completeOnOkButton = false;
		bool actionComplete = false;
		if (currentAction->type == ACTION_DIALOG) {
			fightText = frameSprintf(currentAction->dialog);
			completeOnOkButton = true;
		} else if (currentAction->type == ACTION_GO_TO_OVERWORLD) {
			if (fight->actionTime == 0) {
				for (int i = 0; i < game->playerFightersNum; i++) {
					Fighter *playerFighter = &game->playerFighters[i];
					Fighter *fighter = getFighter(playerFighter->id);
					if (fighter) *playerFighter = *fighter;
				}
				game->nextState = GAME_OVERWORLD;
			}
		} else if (currentAction->type == ACTION_PASS) {
			actionComplete = true;
			fight->menu = FIGHT_MENU_NONE;
			arraySpliceIndex(fight->turns, fight->turnsNum, sizeof(int), 0);
			fight->turnsNum--;
		} else if (currentAction->type == ACTION_PARISH) {
			fightText = frameSprintf("%s has parished\n", srcName);
			if (fight->actionTime > 1) {
				if (src) src->markedForDeletion = true;
				actionComplete = true;
			}
		} else if (currentAction->type == ACTION_PUNCH) {
			fightText = frameSprintf("%s punched %s\n", srcName, destName);
			if (fight->actionTime == 0) {
				if (dest) dest->hp -= 5;
			}
			if (fight->actionTime > 1) actionComplete = true;
		} else if (currentAction->type == ACTION_WING_SLAP) {
			fightText = frameSprintf("%s wing slapped %s\n", srcName, destName);
			if (fight->actionTime == 0) {
				if (dest) dest->hp -= 5;
			}
			if (fight->actionTime > 1) actionComplete = true;
		} else if (currentAction->type == ACTION_CHOMP) {
			fightText = frameSprintf("%s chomped %s\n", srcName, destName);
			if (fight->actionTime == 0) {
				if (dest) dest->hp -= 15;
			}
			if (fight->actionTime > 1) actionComplete = true;
		} else if (currentAction->type == ACTION_CAN_OF_HUGE_DAMAGE) {
			fightText = frameSprintf("%s unleashed a huge can of damage\n", srcName);
			if (fight->actionTime == 0) {
				for (int i = 0; i < fight->fightersNum; i++) {
					Fighter *fighter = &fight->fighters[i];
					if (!fighter->ally) fighter->hp -= 100;
				}
			}
			if (fight->actionTime > 2) actionComplete = true;
		} else {
			logf("Unaccounted for action type %s(%d)\n", actionTypeStrings[currentAction->type], currentAction->type);
			if (fight->actionTime > 3) actionComplete = true;
		}

		if (fightText) {
			ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, 0), ImGuiCond_Always, ImVec2(0.5, 0));
			ImGui::Begin("Information", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::SetWindowFontScale(2);
			ImGui::Text(fightText);
			if (completeOnOkButton) {
				if (ImGui::Button("Ok")) actionComplete = true;
			}
			ImGui::End();
		}

		fight->actionTime += elapsed;

		if (actionComplete) {
			if (currentAction->dialog) free(currentAction->dialog);
			arraySpliceIndex(fight->actions, fight->actionsNum, sizeof(Action), 0);
			fight->actionsNum--;
			fight->actionTime = 0;
		}
	} else {
		if (currentFighter->ally) {
			ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, 0), ImGuiCond_Always, ImVec2(0.5, 0));
			ImGui::Begin("Actions", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::SetWindowFontScale(2);
			ImGui::Text("%s's turn", fighterTypeStrings[currentFighter->type]);
			if (fight->menu == FIGHT_MENU_NONE) {
				if (ImGui::Button("Attack")) fight->menu = FIGHT_MENU_ATTACK;
				if (game->itemsNum != 0) {
					ImGui::SameLine();
					if (ImGui::Button("Items")) fight->menu = FIGHT_MENU_ITEM;
				}
			} else if (fight->menu == FIGHT_MENU_ATTACK) {
				ActionType actionTypes[] = {
					ACTION_PUNCH,
				};

				for (int i = 0; i < ArrayLength(actionTypes); i++) {
					// ActionTypeInfo *info = &game->actionTypeInfos[i];
					if (ImGui::Button(actionTypeStrings[actionTypes[i]])) {
						fight->selectedAction = actionTypes[i];
						fight->menu = FIGHT_MENU_TARGET;
					}
				}
			} else if (fight->menu == FIGHT_MENU_ITEM) {
				for (int i = 0; i < game->itemsNum; i++) {
					Item *item = &game->items[i];
					ActionTypeInfo *actionTypeInfo = &game->actionTypeInfos[item->actionType];
					if (ImGui::Button(actionTypeStrings[item->actionType])) {
						item->amount--;
						if (item->amount <= 0) {
							arraySpliceIndex(game->items, game->itemsNum, sizeof(Item), i);
							game->itemsNum--;
						}
						fight->selectedAction = item->actionType;
						fight->menu = FIGHT_MENU_TARGET;
					}
				}
			} else if (fight->menu == FIGHT_MENU_TARGET) {
				ActionTypeInfo *actionTypeInfo = &game->actionTypeInfos[fight->selectedAction];

				if (actionTypeInfo->targetingType == TARGETING_NONE) {
					Action *action = createAction(fight->selectedAction, currentFighter->id);
					createAction(ACTION_PASS, currentFighter->id);
				}

				for (int i = 0; i < fight->fightersNum; i++) {
					Fighter *fighter = &fight->fighters[i];

					if (actionTypeInfo->targetingType == TARGETING_ENEMIES) {
						if (fighter->ally) continue;
					} else if (actionTypeInfo->targetingType == TARGETING_ALLIES) {
						if (!fighter->ally) continue;
					}

					if (ImGui::Button(frameSprintf("%s###%d", fighterTypeStrings[fighter->type], i))) {
						Action *action = createAction(fight->selectedAction, currentFighter->id, fighter->id);
						createAction(ACTION_PASS, currentFighter->id);
					}
				}
			}

			ImGui::End();
		} else {
			if (currentFighter->type == FIGHTER_BAT) {
				Fighter *target = getRandomAlly();
				Action *action = createAction(ACTION_WING_SLAP, currentFighter->id, target->id);
				createAction(ACTION_PASS, currentFighter->id);
			} else {
				Fighter *target = getRandomAlly();
				Action *action = createAction(ACTION_CHOMP, currentFighter->id, target->id);
				createAction(ACTION_PASS, currentFighter->id);
			}
		}
	}

	for (int i = 0; i < fight->fightersNum; i++) {
		Fighter *fighter = &fight->fighters[i];
		if (fighter->hp <= 0 && !fighter->parishing) {
			fighter->parishing = true;
			createAction(ACTION_PARISH, fighter->id);
		}
	}

	for (int i = 0; i < fight->fightersNum; i++) {
		Fighter *fighter = &fight->fighters[i];
		if (fighter->markedForDeletion) {
			for (int i = 0; i < game->playerFightersNum; i++) {
				Fighter *playerFighter = &game->playerFighters[i];
				if (playerFighter->id == fighter->id) {
					*playerFighter = *fighter;
					playerFighter->parishing = false;
					playerFighter->markedForDeletion = false;
				}
			}
			arraySpliceIndex(fight->fighters, fight->fightersNum, sizeof(Fighter), i);
			fight->fightersNum--;
			i--;
			continue;
		}
	}

	Vec2 rectSize = v2(100, 100);
	Vec2 enemyCursor = v2(0, 50);
	Vec2 allyCursor = v2(0, game->size.y-rectSize.y);

	for (int i = 0; i < fight->fightersNum; i++) {
		Fighter *fighter = &fight->fighters[i];

		Rect rect;
		if (fighter->ally) {
			rect = makeRect(allyCursor, rectSize);
			allyCursor.x += rectSize.x;
		} else {
			rect = makeRect(enemyCursor, rectSize);
			enemyCursor.x += rectSize.x;
		}

		char *str = frameSprintf("%s\nHp: %d/%d\nMp: %d/%d", fighterTypeStrings[fighter->type], fighter->hp, fighter->info->hp, fighter->mp, fighter->info->mp);
		drawText(game->fightFont, str, getPosition(rect) + v2(5, 5), 0xFFFFFFFF, rect.width - 10);

		drawRectOutline(rect, 2, 0xFFFFFFFF);
	}
}
