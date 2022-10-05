#define FROST_FALL_DISTANCE 64
#define POISON_COLOR (0xFF6B4876)
#define BURN_COLOR (0xFFDCAB2C)
#define BLEED_COLOR (0xFF770000)

struct Globals {
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
	float range;
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

	float time;
};

enum TileType {
	TILE_NONE,
	TILE_HOME,
	TILE_GROUND,
	TILE_ROAD,
	TILE_MANA_CRYSTAL,
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

struct Game {
	Font *defaultFont;

	Globals globals;
	bool inEditor;
	float timeScale;
	Vec2 size;
	Vec2 mouse;

	ActorTypeInfo actorTypeInfos[ACTOR_TYPES_MAX];

	/// Serialized
	float time;

	Vec2 cameraPosition;
	float cameraZoom;

	World *world;
	bool shouldReset;

	Tool prevTool;
	Tool tool;
	float toolTime;
	ActorType actorToBuild;

	int hp;
	int money;
	float mana;
	float maxMana;

	int wave;
	bool playingWave;

	ActorType actorsToSpawn[ACTORS_MAX];
	int actorsToSpawnNum;
	float timeTillNextSpawn;

#define SELECTED_ACTORS_MAX 2048
	int selectedActors[SELECTED_ACTORS_MAX];
	int selectedActorsNum;

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

void runGame();
void updateGame();
void stepGame(float elapsed, bool isLastStep);

bool isMouseClicked();

void generateMapFields();
Chunk *createChunk(Vec2i position);
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
Actor *createBullet(Actor *src, Actor *target);
void dealDamage(Actor *dest, float amount, float shieldDamageMulti, float armorDamageMulti, float hpDamageMulti);
void dealDamage(Actor *bullet, Actor *dest);
Rect getRect(Actor *actor);
Vec2 getFlowDirForRect(Rect rect);

Actor **getActorsInRange(Circle range, int *outNum);
Actor **getActorsInRange(Tri2 range, int *outNum);
void startNextWave();

void saveState(char *path);
void writeWorld(DataStream *stream, World *world);
void writeActor(DataStream *stream, Actor *actor);
void writeChunk(DataStream *stream, Chunk *chunk);
void writeTile(DataStream *stream, Tile tile);

void loadState(char *path);
void readWorld(DataStream *stream, World *world, int version);
void readActor(DataStream *stream, Actor *actor, int version);
void readChunk(DataStream *stream, Chunk *chunk, int version);
Tile readTile(DataStream *stream, int version);
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
		game->defaultFont = createFont("assets/common/arial.ttf", 80);

		game->timeScale = 1;

		maximizeWindow();

		rndInt(0, 3); // Burn an rnd seed???
		isFirstStart = true;
	}

	if (game->shouldReset) {
		game->shouldReset = false;

		World *world = game->world;
		for (int i = 0; i < world->actorsNum; i++) deinitActor(&world->actors[i]);
		free(world);
		game->world = NULL;
	}

	if (!game->world) {
		game->world = (World *)zalloc(sizeof(World));
		World *world = game->world;

		{ /// Setup actors
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
			info->damage = 10;
			info->hpDamageMulti = 10;
			info->armorDamageMulti = 5;
			info->shieldDamageMulti = 5;
			info->range = 5 * TILE_SIZE;
			info->rpm = 20;
			info->mana = 0;
			info->price = 10;
			info->priceMulti = 15;

			info = &game->actorTypeInfos[ACTOR_MORTAR_TOWER];
			strncpy(info->name, "Mortar", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 20;
			info->hpDamageMulti = 10;
			info->armorDamageMulti = 15;
			info->shieldDamageMulti = 5;
			info->range = 10 * TILE_SIZE;
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
			info->range = 1.5 * TILE_SIZE;
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
			info->range = 2 * TILE_SIZE;
			info->rpm = 180;
			info->mana = 2.0/3.0;
			info->price = 250;
			info->priceMulti = 100;

			info = &game->actorTypeInfos[ACTOR_FLAME_THROWER];
			strncpy(info->name, "Flame Thrower", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 5;
			info->hpDamageMulti = 6;
			info->armorDamageMulti = 9;
			info->shieldDamageMulti = 3;
			info->range = 4 * TILE_SIZE;
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
			info->range = 4 * TILE_SIZE;
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
			info->range = 5 * TILE_SIZE;
			info->rpm = 5;
			info->price = 500;
			info->priceMulti = 100;

			info = &game->actorTypeInfos[ACTOR_ENCAMPENT];
			strncpy(info->name, "Encampent", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 20;
			info->hpDamageMulti = 10;
			info->armorDamageMulti = 15;
			info->shieldDamageMulti = 5;
			info->range = 2 * TILE_SIZE;
			info->rpm = 5;
			info->price = 500;
			info->priceMulti = 100;

			info = &game->actorTypeInfos[ACTOR_LOOKOUT];
			strncpy(info->name, "Lookout", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 1;
			info->hpDamageMulti = 2;
			info->armorDamageMulti = 1;
			info->shieldDamageMulti = 3;
			info->range = 8 * TILE_SIZE;
			info->rpm = 0;
			info->price = 500;
			info->priceMulti = 100;

			info = &game->actorTypeInfos[ACTOR_RADAR];
			strncpy(info->name, "Radar", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 20;
			info->hpDamageMulti = 20;
			info->armorDamageMulti = 10;
			info->shieldDamageMulti = 10;
			info->range = 30 * TILE_SIZE;
			info->rpm = 700;
			info->price = 1000;
			info->priceMulti = 250;

			info = &game->actorTypeInfos[ACTOR_OBELISK];
			strncpy(info->name, "Obelisk", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 8;
			info->hpDamageMulti = 5;
			info->armorDamageMulti = 10;
			info->shieldDamageMulti = 2;
			info->range = 5 * TILE_SIZE;
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
			info->range = 20 * TILE_SIZE;
			info->rpm = 360;
			info->mana = 12;
			info->price = 1000;
			info->priceMulti = 250;

			info = &game->actorTypeInfos[ACTOR_MANA_SIPHON];
			strncpy(info->name, "Mana Siphon", ACTOR_TYPE_NAME_MAX_LEN);
			info->damage = 0;
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
		} ///

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
		} ///

		game->hp = 10;
		game->money = 1000;
		game->mana = 100;
		game->maxMana = 100;
		generateMapFields();

		game->cameraZoom = 1;
		game->cameraPosition = v2();

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

	for (int i = 0; i < stepsToTake; i++) {
		bool isLastStep = i == stepsToTake-1;
		renderer->disabled = !isLastStep;
		stepGame(elapsed, isLastStep);
	}

	guiDraw();
}

void stepGame(float elapsed, bool isLastStep) {
	float timeScale = elapsed / (1/60.0);

	Globals *globals = &game->globals;
	World *world = game->world;

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	if (game->inEditor && isLastStep) {
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Checkbox("Show Dijkstra values", &game->debugShowDijkstraValues);
		ImGui::Checkbox("Show Flow Field values", &game->debugShowFlowFieldValues);
		ImGui::Checkbox("Show perlin values", &game->debugShowPerlinValues);
		ImGui::Checkbox("Draw chunk lines", &game->debugDrawChunkLines);
		ImGui::Checkbox("Draw tile lines", &game->debugDrawTileLines);
		ImGui::Checkbox("Show actor velo", &game->debugShowActorVelo);
		if (ImGui::Button("Explore all")) {
			for (int i = 0; i < world->chunksNum; i++) {
				Chunk *chunk = &world->chunks[i];
				chunk->visible = true;
			}
		}
		ImGui::InputInt("Money", &game->money);
		ImGui::InputFloat("Mana", &game->mana);
		ImGui::InputFloat("MaxMana", &game->mana);
		ImGui::InputInt("Wave", &game->wave);
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

		ImGui::End();
	}

	clearRenderer();

	Matrix3 cameraMatrix = mat3();
	Rect screenRect = {};
	{
		cameraMatrix.TRANSLATE(game->size/2);
		cameraMatrix.SCALE(game->cameraZoom);
		cameraMatrix.TRANSLATE(-game->cameraPosition);

		game->mouse = cameraMatrix.invert() * platform->mouse;

		screenRect.width = game->size.x / game->cameraZoom;
		screenRect.height = game->size.y / game->cameraZoom;
		screenRect.x = game->cameraPosition.x - screenRect.width/2;
		screenRect.y = game->cameraPosition.y - screenRect.height/2;
	}

	pushCamera2d(cameraMatrix);

	int *typeCounts = (int *)frameMalloc(sizeof(int) * ACTOR_TYPES_MAX);
	for (int i = 0; i < world->actorsNum; i++) {
		Actor *actor = &world->actors[i];
		typeCounts[actor->type]++;
	}

	if (isLastStep) {
		Vec2 moveDir = v2();
		if (keyPressed('W')) moveDir.y--;
		if (keyPressed('S')) moveDir.y++;
		if (keyPressed('A')) moveDir.x--;
		if (keyPressed('D')) moveDir.x++;

		game->cameraZoom += platform->mouseWheel * 0.1;
		game->cameraZoom = mathClamp(game->cameraZoom, 0.1, 20);

		game->cameraPosition += normalize(moveDir) * 20 / game->cameraZoom;

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
	}

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
					if (tile->type == TILE_GROUND || tile->type == TILE_MANA_CRYSTAL) color = 0xFF017301;
					if (tile->type == TILE_ROAD) color = 0xFF966F02;

					float heightShadePerc = clampMap(tile->height, 0, 3, 0, 0.25);
					// float heightShadePerc = clampMap(tile->height, 0, 255, 0, 1);
					color = lerpColor(color, 0xFF000000, heightShadePerc);

					drawRect(rect, color);

					if (tile->type == TILE_MANA_CRYSTAL) {
						drawRect(inflatePerc(rect, -0.6), 0xFF19C4FF);
					}

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

	{ /// Update and draw actors
		int enemiesAlive = 0;

		for (int i = 0; i < world->actorsNum; i++) {
			Actor *actor = &world->actors[i];
			ActorTypeInfo *info = &game->actorTypeInfos[actor->type];

			bool isSelected = false;
			for (int i = 0; i < game->selectedActorsNum; i++) {
				if (game->selectedActors[i] == actor->id) {
					isSelected = true;
					break;
				}
			}

			Rect rect = getRect(actor);

			actor->movementSpeed = info->movementSpeed;
			actor->movementSpeed *= clampMap(actor->slow, 0, 10, 1, 0.4);

			actor->slow -= 6*elapsed;

			if (actor->poison) {
				float toInflict = MinNum(actor->poison, 24*elapsed);
				actor->poison -= toInflict;
				if (actor->poison < 0) actor->poison = 0;

				dealDamage(actor, toInflict, 1, 0.5, 0.5);
			}

			if (actor->burn) {
				float toInflict = MinNum(actor->burn, 24*elapsed);
				actor->burn -= toInflict;
				if (actor->burn < 0) actor->burn = 0;

				dealDamage(actor, toInflict, 0.5, 1, 0.5);
			}

			if (actor->bleed) {
				float toInflict = MinNum(actor->bleed, 24*elapsed);
				actor->bleed -= toInflict;
				if (actor->bleed < 0) actor->bleed = 0;

				dealDamage(actor, toInflict, 0.5, 0.5, 1);
			}

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
					Circle range = makeCircle(actor->position, info->range);

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(range, &enemiesInRangeNum);

					Actor *bestEnemy = NULL;
					float bestEnemyScore = 0;
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						// ActorTypeInfo *enemyInfo = &game->actorTypeInfos[enemy->type];
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
					if (target) actor->aimRads = radsBetween(actor->position, target->position);
				}

				bool isActive = true;
				if (!game->playingWave && !towerIsActiveBetweenWaves) isActive = false;
				if (!target && towerCaresAboutTargets) isActive = false;

				if (isActive) {
					actor->timeTillNextShot -= elapsed;

					if (actor->timeTillNextShot < 0) {
						actor->timeTillNextShot = 1.0/(info->rpm/60.0);

						float manaCost = info->mana;
						if (game->mana > manaCost) {
							game->mana -= manaCost;
							towerShouldFire = true;
						}
					}
				}

				if (game->tool == TOOL_NONE || game->tool == TOOL_SELECTED) {
					if (isMouseClicked() && contains(getRect(actor), game->mouse)) {
						game->prevTool = TOOL_NONE;
						game->tool = TOOL_SELECTED;
						game->selectedActorsNum = 0;
						game->selectedActors[game->selectedActorsNum++] = actor->id;
					}
				}

				if (isSelected) {
					drawRect(inflatePerc(getRect(actor), 0.2), 0xFFEAF82A);
				}
			}

			if (actor->type == ACTOR_BALLISTA) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}

				drawRect(rect, 0xFF800000);

				Line2 line;
				line.start = getCenter(rect);
				line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
				drawLine(line, 4, 0xFFFF0000);
			} else if (actor->type == ACTOR_MORTAR_TOWER) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}

				drawRect(rect, 0xFF525252);
			} else if (actor->type == ACTOR_TESLA_COIL) {
				if (towerShouldFire) {
					Circle circle = makeCircle(actor->position, info->range);
					drawCircle(circle, 0xFFB8FFFA);
					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(circle, &enemiesInRangeNum);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						dealDamage(actor, enemy);
					}
				}

				drawCircle(makeCircle(getCenter(rect), rect.width/2), 0xFFA0A0F0);
			} else if (actor->type == ACTOR_FROST_KEEP) {
				if (towerShouldFire) {
					Vec2i towerTilePos = worldToTile(actor->position);
					Vec2i min = towerTilePos;
					min.x -= (info->range / TILE_SIZE);
					min.y -= (info->range / TILE_SIZE);

					Vec2i max = towerTilePos;
					max.x += (info->range / TILE_SIZE);
					max.y += (info->range / TILE_SIZE);

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

				drawRect(rect, 0xFFE3F0F5);
			} else if (actor->type == ACTOR_FLAME_THROWER) {
				drawRect(rect, lerpColor(BURN_COLOR, 0xFF000000, 0.75));

				if (towerShouldFire) {
					Vec2 start = actor->position;
					float angle = toRad(15);
					Vec2 end0 = start + radToVec2(actor->aimRads - angle) * info->range;
					Vec2 end1 = start + radToVec2(actor->aimRads + angle) * info->range;

					Tri2 tri = makeTri2(start, end0, end1);
					drawLine(tri.verts[0], tri.verts[1], 5, BURN_COLOR);
					drawLine(tri.verts[1], tri.verts[2], 5, BURN_COLOR);
					drawLine(tri.verts[2], tri.verts[0], 5, BURN_COLOR);

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(tri, &enemiesInRangeNum);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						int amount = info->damage;
						Actor *enemy = enemiesInRange[i];
						if (enemy->shield > 0) amount *= info->shieldDamageMulti;
						else if (enemy->armor > 0) amount *= info->armorDamageMulti;
						else amount *= info->hpDamageMulti;

						enemy->burn += amount;
					}
				}

				Line2 line;
				line.start = getCenter(rect);
				line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
				drawLine(line, 12, 0xFF000000);
			} else if (actor->type == ACTOR_POISON_SPRAYER) {
				drawRect(rect, lerpColor(POISON_COLOR, 0xFF000000, 0.75));

				if (towerShouldFire) {
					Vec2 start = actor->position;
					float angle = toRad(15);
					Vec2 end0 = start + radToVec2(actor->aimRads - angle) * info->range;
					Vec2 end1 = start + radToVec2(actor->aimRads + angle) * info->range;

					Tri2 tri = makeTri2(start, end0, end1);
					drawLine(tri.verts[0], tri.verts[1], 5, POISON_COLOR);
					drawLine(tri.verts[1], tri.verts[2], 5, POISON_COLOR);
					drawLine(tri.verts[2], tri.verts[0], 5, POISON_COLOR);

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(tri, &enemiesInRangeNum);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						int amount = info->damage;
						Actor *enemy = enemiesInRange[i];
						if (enemy->shield > 0) amount *= info->shieldDamageMulti;
						else if (enemy->armor > 0) amount *= info->armorDamageMulti;
						else amount *= info->hpDamageMulti;

						enemy->poison += amount;
					}
				}

				Line2 line;
				line.start = getCenter(rect);
				line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
				drawLine(line, 12, 0xFF000000);
			} else if (actor->type == ACTOR_SHREDDER) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}

				drawRect(rect, 0xFF800000);

				Line2 line;
				line.start = getCenter(rect);
				line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
				line.start = line.end - radToVec2(actor->aimRads)*(TILE_SIZE);
				drawLine(line, 4, 0xFFFF0000);
			} else if (actor->type >= ACTOR_ENEMY1 && actor->type <= ACTOR_ENEMY64) {
				enemiesAlive++;

				Vec2 dir = getFlowDirForRect(rect);

				actor->accel = dir * (actor->movementSpeed * elapsed) * 5;

				Vec2i goal = v2i(CHUNK_SIZE/2, CHUNK_SIZE/2);
				Rect goalRect = tileToWorldRect(goal);
				if (overlaps(rect, goalRect)) {
					game->hp--;
					actor->markedForDeletion = true;
				}

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

				if (!actor->bleed) actor->hp += info->hpGainPerSec * elapsed;
				if (!actor->burn) actor->armor += info->armorGainPerSec * elapsed;
				if (!actor->poison) actor->shield += info->shieldGainPerSec * elapsed;
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

				Rect bulletRect = makeCenteredSquare(actor->position, 8);
				drawRect(bulletRect, 0xFFFF0000);
			} else if (actor->type == ACTOR_MORTAR) {
				if (actor->time == 0) {
					Actor *target = getActor(actor->bulletTarget);
					if (target) {
						actor->position = target->position;
					}
				}

				float delayTime = 0.5;
				float explodeRange = 2 * TILE_SIZE;
				if (actor->time < delayTime) {
					float ghostPerc = clampMap(actor->time, 0, delayTime, 0.75, 1);
					drawCircle(actor->position, explodeRange*ghostPerc, 0x20900000);
				}

				if (actor->time >= delayTime) {
					Circle circle = makeCircle(actor->position, explodeRange);
					drawCircle(circle, 0xFFFFFFFF);
					actor->markedForDeletion = true;

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(circle, &enemiesInRangeNum);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						dealDamage(actor, enemy);
					}
				}
			} else if (actor->type == ACTOR_FROST) {
				int maxTime = 10;
				float fallSpeedPerSec = FROST_FALL_DISTANCE / (float)maxTime;
				actor->position.y += fallSpeedPerSec * elapsed;
				rect = makeCenteredSquare(getCenter(rect), 32);

				Circle range = makeCircle(getCenter(rect), rect.width/2);

				int enemiesInRangeNum = 0;
				Actor **enemiesInRange = getActorsInRange(range, &enemiesInRangeNum);
				for (int i = 0; i < enemiesInRangeNum; i++) {
					Actor *enemy = enemiesInRange[i];
					if (overlaps(getRect(enemy), rect)) {
						enemy->slow += 6;
						actor->markedForDeletion = true;
					}
				}

				drawRect(rect, 0x80FFFFFF);
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
					Actor **enemiesInRange = getActorsInRange(circle, &enemiesInRangeNum);
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

						if (saw->sawHitListNum > 6-1) saw->markedForDeletion = true;
						saw->sawHitList[saw->sawHitListNum++] = enemy->id;
						dealDamage(saw, enemy);
					}
				}

				Rect bulletRect = makeCenteredSquare(actor->position, 8);
				drawRect(bulletRect, 0xFFFF0000);
			} else {
				drawRect(rect, 0xFFFF00FF);
			}

			Vec2 damping = v2(0.1, 0.1);
			actor->velo += (actor->accel - damping*actor->velo) * timeScale;
			actor->accel = v2();

			actor->position += actor->velo * timeScale;

			if (actor->hp <= 0 && info->maxHp > 0) actor->markedForDeletion = true;

			if (game->debugShowActorVelo) {
				DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
				drawTextInRect(frameSprintf("%.01f\n%.01f", actor->velo.x, actor->velo.y), props, rect);
				// Vec2 start = actor->position;
				// Vec2 end = actor->position + normalize(actor->velo)*(TILE_SIZE/2);
				// drawLine(start, end, 5, 0xFFFF0000);
			}

			actor->time += elapsed;
		}

		if (game->playingWave && enemiesAlive == 0 && game->actorsToSpawnNum == 0) {
			game->playingWave = false;
			if (game->hp > 0) {
				if (fileExists("assets/states/autosave.save_state")) {
					if (fileExists("assets/states/prevAutosave.save_state")) deleteFile("assets/states/prevAutosave.save_state");
					copyFile("assets/states/autosave.save_state", "assets/states/prevAutosave.save_state");
				}
				saveState("assets/states/autosave.save_state");
			}
		}
	} ///

	for (int i = 0; i < world->actorsNum; i++) {
		Actor *actor = &world->actors[i];

		if (actor->markedForDeletion) {
			ActorTypeInfo *info = &game->actorTypeInfos[actor->type];
			if (info->isEnemy) {
				if (actor->type == ACTOR_ENEMY1) {
					game->money += 4; 
				} else {
					game->money += info->enemySpawnStartingWave;
				}
			}

			deinitActor(actor);
			arraySpliceIndex(world->actors, world->actorsNum, sizeof(Actor), i);
			world->actorsNum--;
			i--;
			continue;
		}
	}

	{ /// Update hud and tool
		if (game->prevTool != game->tool) {
			game->prevTool = game->tool;
			game->toolTime = 0;
		}

		if (game->tool == TOOL_NONE) {
			nguiStartWindow("Tools window", game->size*v2(0.5, 1), v2(0.5, 1));
			nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 4);

			ActorType typesCanBuy[] = {
				ACTOR_BALLISTA, ACTOR_MORTAR_TOWER, ACTOR_TESLA_COIL, ACTOR_FROST_KEEP, ACTOR_FLAME_THROWER, ACTOR_POISON_SPRAYER, ACTOR_SHREDDER
			};
			int typesCanBuyNum = ArrayLength(typesCanBuy);

			for (int i = 0; i < typesCanBuyNum; i++) {
				ActorType actorType = typesCanBuy[i];
				ActorTypeInfo *info = &game->actorTypeInfos[actorType];
				float price = info->price + info->priceMulti*typeCounts[actorType];
				char *label = frameSprintf("%s $%.0f\n", info->name, price);
				if (nguiButton(label)) {
					game->tool = TOOL_BUILDING;
					game->actorToBuild = actorType;
				}
			}

			nguiPopStyleVar(NGUI_STYLE_ELEMENTS_IN_ROW);
			nguiEndWindow();
		}

		if (game->tool == TOOL_BUILDING) {
			if (platform->rightMouseDown) game->tool = TOOL_NONE;

			ActorTypeInfo *info = &game->actorTypeInfos[game->actorToBuild];

			Vec2i tilePosition = worldToTile(game->mouse);
			Rect tileRect = tileToWorldRect(tilePosition);
			drawRect(tileRect, lerpColor(0x80000088, 0xFF000088, timePhase(game->time*2)));

			Vec2 center = getCenter(tileRect);

			Circle range = makeCircle(getCenter(tileRect), info->range);
			drawCircle(range, 0x80FF0000);

			Tile *tile = getTileAt(tilePosition);

			bool canBuild = true;
			Chunk *chunk = worldToChunk(center);
			if (!chunk) canBuild = false;
			if (canBuild && !chunk->visible) canBuild = false;
			if (canBuild && !tile) canBuild = false;
			if (canBuild && tile->type != TILE_GROUND) canBuild = false;

			for (int i = 0; i < world->actorsNum; i++) {
				Actor *other = &world->actors[i];
				if (equal(worldToTile(other->position), tilePosition)) canBuild = false;
			}

			if (canBuild && isMouseClicked()) {
				float price = info->price + info->priceMulti*typeCounts[game->actorToBuild];
				if (game->money >= price) {
					game->money -= price;
					Actor *newTower = createActor(game->actorToBuild);
					newTower->amountPaid = price;
					newTower->position = center;
					if (!keyPressed(KEY_SHIFT)) game->tool = TOOL_NONE;
				} else {
					infof("Not enough money\n");
				}
			}
		}

		if (game->tool == TOOL_SELECTED) {
			nguiStartWindow("Selected window", game->size*v2(0.5, 1), v2(0.5, 1));
			nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 4);

			if (nguiButton("Sell")) {
				for (int i = 0; i < game->selectedActorsNum; i++) {
					Actor *actor = getActor(game->selectedActors[i]);
					if (!actor) continue;

					actor->markedForDeletion = true;
					game->money += actor->amountPaid;

					arraySpliceIndex(game->selectedActors, game->selectedActorsNum, sizeof(int), i);
					game->selectedActorsNum--;
					i--;
					continue;
				}
			}

			if (game->selectedActorsNum == 1) {
				Actor *actor = getActor(game->selectedActors[0]);
				if (nguiButton(frameSprintf("Priority: %s", priorityStrings[actor->priority]))) {
					actor->priority = (Priority)((int)actor->priority + 1);
					if (actor->priority > PRIORITIES_MAX-1) {
						actor->priority = (Priority)0;
					}
				}
			}

			nguiPopStyleVar(NGUI_STYLE_ELEMENTS_IN_ROW);
			nguiEndWindow();

			if (isMouseClicked()) game->selectedActorsNum = 0;

			if (game->selectedActorsNum == 0) game->tool = TOOL_NONE;
		}

		game->toolTime += elapsed;

		for (int i = 0; i < game->selectedActorsNum; i++) {
			Actor *actor = getActor(game->selectedActors[i]);
			if (!actor) continue;
			ActorTypeInfo *info = &game->actorTypeInfos[actor->type];

			if (info->isTower) {
				Circle range = makeCircle(actor->position, info->range);
				drawCircle(range, 0x80FF0000);
			}
		}
	} ///

	if (game->playingWave) {
		if (game->actorsToSpawnNum > 0) game->timeTillNextSpawn -= elapsed;
		if (game->timeTillNextSpawn <= 0) {
			game->timeTillNextSpawn += 1;

			Vec2 *spawnPoints = (Vec2 *)frameMalloc(sizeof(Vec2) * CHUNKS_MAX);
			int spawnPointsNum = 0;
			for (int i = 0; i < world->chunksNum; i++) {
				Chunk *chunk = &world->chunks[i];

				bool isPortal = false; //@copyPastedIsPortal
				if (chunk->connectionsNum == 1 && !isZero(chunk->position)) isPortal = true;

				if (!chunk->visible) continue;
				if (isPortal) {
					spawnPoints[spawnPointsNum++] = tileToWorld(chunkTileToWorldTile(chunk, v2i(CHUNK_SIZE/2, CHUNK_SIZE/2)));
					drawCircle(spawnPoints[spawnPointsNum-1], 32, 0xFF0000FF);
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
				ActorType toSpawn = game->actorsToSpawn[0];
				Actor *actor = createActor(toSpawn);
				actor->position = spawnPoints[i];

				memmove(&game->actorsToSpawn[0], &game->actorsToSpawn[1], sizeof(ActorType) * (game->actorsToSpawnNum-1));
				game->actorsToSpawnNum--;
				if (game->actorsToSpawnNum == 0) break;
			}
		}

		float manaRegenPerSec = 1;
		game->mana += manaRegenPerSec * elapsed;
		if (game->mana > game->maxMana) game->mana = game->maxMana;
	}

	{ /// Show explore buttons
		if (!game->playingWave && game->tool == TOOL_NONE) {
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

	// drawCircle(game->mouse, 10, 0xFFFF0000);

	popCamera2d();

	{
		Rect rect = makeRect(0, 0, 350, 100);
		DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
		drawTextInRect(frameSprintf("Hp: %d\nMoney $%d\nMana: %.1f/%.1f", game->hp, game->money, game->mana, game->maxMana), props, rect);
	}

	nguiDraw(elapsed);

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) drawText(game->defaultFont, frameSprintf("%.1fms", platform->frameTimeAvg), v2(300, 0), 0xFF808080);

	drawOnScreenLog();

	game->time += elapsed;
}

bool isMouseClicked() {
	bool ret = platform->mouseJustDown;
	if (ngui->mouseHoveringThisFrame) ret = false;
	if (ngui->mouseHoveringLastFrame) ret = false;
	if (ngui->mouseJustDownThisFrame) ret = false;
	if (game->prevTool != game->tool) ret = false;
	if (game->toolTime < 0.05) ret = false;
	return ret;
}

void generateMapFields() {
	World *world = game->world;
	for (int i = 0; i < world->chunksNum; i++) {
		Chunk *chunk = &world->chunks[i];
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
	for (int i = 0; i < world->chunksNum; i++) {
		Chunk *chunk = &world->chunks[i];

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
	World *world = game->world;

	if (world->chunksNum > CHUNKS_MAX-1) {
		logf("Too many chunks!\n");
		return NULL;
	}

	Chunk *chunk = &world->chunks[world->chunksNum++];
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
			tile->height = clampMap(perlinValue, 0.5, 1, 0, 3);
		}
	}

	return chunk;
}

Chunk *getChunkAt(Vec2i position) {
	World *world = game->world;

	for (int i = 0; i < world->chunksNum; i++) {
		Chunk *chunk = &world->chunks[i];
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
	World *world = game->world;

	if (world->actorsNum > ACTORS_MAX-1) {
		Panic("Too many actors\n"); //@robustness
	}

	Actor *actor = &world->actors[world->actorsNum++];
	memset(actor, 0, sizeof(Actor));
	actor->type = type;
	initActor(actor);
	actor->id = ++world->nextActorId;

	ActorTypeInfo *info = &game->actorTypeInfos[actor->type];
	actor->hp = info->maxHp;
	actor->armor = info->maxArmor;
	actor->shield = info->maxShield;

	return actor;
}

void initActor(Actor *actor) {
	if (actor->type == ACTOR_SAW) {
		actor->sawHitList = (int *)zalloc(sizeof(int) * SAW_HIT_LIST_MAX);
	}
}
void deinitActor(Actor *actor) {
	if (actor->sawHitList) free(actor->sawHitList);
}

Actor *getActor(int id) {
	World *world = game->world;
	if (id == 0) return NULL;

	for (int i = 0; i < world->actorsNum; i++) {
		Actor *actor = &world->actors[i];
		if (actor->id == id) return actor;
	}
	return NULL;
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

	ActorTypeInfo *srcInfo = &game->actorTypeInfos[src->type];
	if (srcInfo->isTower) {
		tower = src;
	} else {
		tower = getActor(src->parentTower);
	}
	if (!tower) {
		logf("Null damage source?\n");
		return;
	}

	ActorTypeInfo *towerInfo = &game->actorTypeInfos[tower->type];

	float damage = towerInfo->damage;
	dealDamage(dest, damage, towerInfo->shieldDamageMulti, towerInfo->armorDamageMulti, towerInfo->hpDamageMulti);
}

void dealDamage(Actor *dest, float amount, float shieldDamageMulti, float armorDamageMulti, float hpDamageMulti) {
	float damageLeft = amount;

	float shieldDamage = MinNum(damageLeft, dest->shield);
	damageLeft -= shieldDamage;
	dest->shield -= shieldDamage * shieldDamageMulti;

	float armorDamage = MinNum(damageLeft, dest->armor);
	damageLeft -= armorDamage;
	dest->armor -= armorDamage * armorDamageMulti;

	dest->hp -= damageLeft * hpDamageMulti;
}

Rect getRect(Actor *actor) {
	Rect rect = {};
	rect.width = TILE_SIZE * 0.5;
	rect.height = TILE_SIZE * 0.5;
	rect.x = actor->position.x - rect.width/2;
	rect.y = actor->position.y - rect.height/2;
	return rect;
}

Vec2 getFlowDirForRect(Rect rect) {
	Vec2 position = getPosition(rect);
	Vec2 size = getSize(rect);
	Vec2 corners[4] = {
		position + size*v2(0, 0),
		position + size*v2(0, 1),
		position + size*v2(1, 0),
		position + size*v2(1, 1)
	};

	Vec2 dir = v2();
	for (int i = 0; i < 4; i++) {
		Vec2i tilePosition = worldToTile(corners[i]);
		Tile *tile = getTileAt(tilePosition);
		if (tile && !isZero(tile->flow)) dir += tile->flow;
	}
	dir = normalize(dir);
	return dir;
}

Actor **getActorsInRange(Circle range, int *outNum) {
	World *world = game->world;

	*outNum = 0;

	Actor **enemiesInRange = (Actor **)frameMalloc(sizeof(Actor **) * world->actorsNum);
	int enemiesInRangeNum = 0;
	for (int i = 0; i < world->actorsNum; i++) {
		Actor *actor = &world->actors[i];
		ActorTypeInfo *otherInfo = &game->actorTypeInfos[actor->type];
		if (!otherInfo->isEnemy) continue;

		// if (contains(range, actor->position)) enemiesInRange[enemiesInRangeNum++] = actor;
		if (contains(getRect(actor), range)) enemiesInRange[enemiesInRangeNum++] = actor;
	}

	*outNum = enemiesInRangeNum;
	return enemiesInRange;
}

Actor **getActorsInRange(Tri2 range, int *outNum) {
	World *world = game->world;

	*outNum = 0;

	Actor **enemiesInRange = (Actor **)frameMalloc(sizeof(Actor **) * world->actorsNum);
	int enemiesInRangeNum = 0;
	for (int i = 0; i < world->actorsNum; i++) {
		Actor *actor = &world->actors[i];
		ActorTypeInfo *otherInfo = &game->actorTypeInfos[actor->type];
		if (!otherInfo->isEnemy) continue;

		// if (contains(range, actor->position)) enemiesInRange[enemiesInRangeNum++] = actor;
		if (overlaps(getRect(actor), range)) enemiesInRange[enemiesInRangeNum++] = actor;
	}

	*outNum = enemiesInRangeNum;
	return enemiesInRange;
}

void startNextWave() {
	game->playingWave = true;
	game->wave++;

	ActorType *possibleActors = (ActorType *)frameMalloc(sizeof(ActorType) * ACTOR_TYPES_MAX);
	int possibleActorsNum = 0;

	for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
		ActorTypeInfo *info = &game->actorTypeInfos[i];
		if (info->enemySpawnStartingWave != 0 && info->enemySpawnStartingWave <= game->wave) {
			possibleActors[possibleActorsNum++] = (ActorType)i;
		}
	}

	if (possibleActorsNum == 0) {
		logf("No possible actor types to spawn???\n");
		possibleActors[possibleActorsNum++] = ACTOR_ENEMY1;
	}

	int maxEnemies = game->wave * game->wave;
	for (int i = 0; i < maxEnemies; i++) {
		float value = rndFloat(0, 1);
		value = tweenEase(value, QUAD_IN);
		int index = roundf(lerp(0, possibleActorsNum-1, value)); // Not perfect distribution
		game->actorsToSpawn[game->actorsToSpawnNum++] = possibleActors[index];
	}
}

void saveState(char *path) {
	logf("Saving...\n");
	DataStream *stream = newDataStream();

	writeU32(stream, 8); // version
	writeFloat(stream, lcgSeed);
	writeFloat(stream, game->time);
	writeVec2(stream, game->cameraPosition);
	writeFloat(stream, game->cameraZoom);
	writeWorld(stream, game->world);

	writeU32(stream, game->prevTool);
	writeU32(stream, game->tool);
	writeFloat(stream, game->toolTime);

	writeU32(stream, game->actorToBuild);
	writeU32(stream, game->hp);
	writeU32(stream, game->money);
	writeFloat(stream, game->mana);
	writeFloat(stream, game->maxMana);
	writeU32(stream, game->wave);
	writeU8(stream, game->playingWave);

	writeU32(stream, game->actorsToSpawnNum);
	for (int i = 0; i < game->actorsToSpawnNum; i++) writeU32(stream, game->actorsToSpawn[i]);
	writeFloat(stream, game->timeTillNextSpawn);

	writeU32(stream, game->selectedActorsNum);
	for (int i = 0; i < game->selectedActorsNum; i++) writeU32(stream, game->selectedActors[i]);

	writeDataStream(path, stream);
	destroyDataStream(stream);
}

void writeWorld(DataStream *stream, World *world) {
	writeU32(stream, world->actorsNum);
	for (int i = 0; i < world->actorsNum; i++) writeActor(stream, &world->actors[i]);
	writeU32(stream, world->nextActorId);

	writeU32(stream, world->chunksNum);
	for (int i = 0; i < world->chunksNum; i++) writeChunk(stream, &world->chunks[i]);
}

void writeActor(DataStream *stream, Actor *actor) {
	writeU32(stream, actor->type);
	writeU32(stream, actor->id);
	writeVec2(stream, actor->position);
	writeVec2(stream, actor->velo);
	writeVec2(stream, actor->accel);
	writeFloat(stream, actor->aimRads);
	writeFloat(stream, actor->hp);
	writeFloat(stream, actor->armor);
	writeFloat(stream, actor->shield);
	writeFloat(stream, actor->timeTillNextShot);
	writeU8(stream, actor->markedForDeletion);

	writeFloat(stream, actor->slow);
	writeFloat(stream, actor->poison);
	writeFloat(stream, actor->burn);
	writeFloat(stream, actor->bleed);
	writeFloat(stream, actor->movementSpeed);
	writeU32(stream, actor->priority);
	writeU32(stream, actor->bulletTarget);
	writeVec2(stream, actor->bulletTargetPosition);
	writeU32(stream, actor->parentTower);

	writeU32(stream, actor->sawHitListNum);
	for (int i = 0; i < actor->sawHitListNum; i++) writeU32(stream, actor->sawHitList[i]);

	writeU32(stream, actor->amountPaid);

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
	writeU8(stream, tile.height);
}

void loadState(char *path) {
	World *world = game->world;
	for (int i = 0; i < world->actorsNum; i++) deinitActor(&world->actors[i]);

	DataStream *stream = loadDataStream(path);
	if (!stream) {
		logf("No state at %s\n", path);
		return;
	}

	int version = readU32(stream);
	if (version >= 1) lcgSeed = readU32(stream);
	game->time = readFloat(stream);
	game->cameraPosition = readVec2(stream);
	game->cameraZoom = readFloat(stream);
	readWorld(stream, game->world, version);
	if (version >= 6) game->prevTool = (Tool)readU32(stream);
	game->tool = (Tool)readU32(stream);
	if (version >= 6) game->toolTime = readFloat(stream);
	game->actorToBuild = (ActorType)readU32(stream);
	game->hp = version >= 3 ? readU32(stream) : 10;
	game->money = readU32(stream);
	game->mana = version >= 7 ? readFloat(stream) : 100;
	game->maxMana = version >= 7 ? readFloat(stream) : 100;
	game->wave = readU32(stream);
	game->playingWave = readU8(stream);
	game->actorsToSpawnNum = readU32(stream);
	for (int i = 0; i < game->actorsToSpawnNum; i++) game->actorsToSpawn[i] = (ActorType)readU32(stream);
	game->timeTillNextSpawn = readFloat(stream);

	if (version >= 5) {
		game->selectedActorsNum = readU32(stream);
		for (int i = 0; i < game->selectedActorsNum; i++) game->selectedActors[i] = readU32(stream);
	}

	destroyDataStream(stream);

	generateMapFields();
}

void readWorld(DataStream *stream, World *world, int version) {
	world->actorsNum = readU32(stream);
	for (int i = 0; i < world->actorsNum; i++) readActor(stream, &world->actors[i], version);
	world->nextActorId = readU32(stream);
	world->chunksNum = readU32(stream);
	for (int i = 0; i < world->chunksNum; i++) readChunk(stream, &world->chunks[i], version);
}

void readActor(DataStream *stream, Actor *actor, int version) {
	actor->type = (ActorType)readU32(stream);
	initActor(actor);
	actor->id = readU32(stream);
	actor->position = readVec2(stream);
	actor->velo = readVec2(stream);
	actor->accel = version >= 8 ? readVec2(stream) : v2();
	actor->aimRads = readFloat(stream);
	actor->hp = readFloat(stream);
	if (version >= 2) actor->armor = readFloat(stream);
	if (version >= 2) actor->shield = readFloat(stream);
	actor->timeTillNextShot = readFloat(stream);
	actor->markedForDeletion = readU8(stream);

	if (version >= 8) {
		actor->slow = readFloat(stream);
		actor->poison = readFloat(stream);
		actor->burn = readFloat(stream);
		actor->bleed = readFloat(stream);
		actor->movementSpeed = readFloat(stream);
		actor->priority = (Priority)readU32(stream);
		actor->bulletTarget = readU32(stream);
		actor->bulletTargetPosition = readVec2(stream);
		actor->parentTower = readU32(stream);

		actor->sawHitListNum = readU32(stream);
		for (int i = 0; i < actor->sawHitListNum; i++) actor->sawHitList[i] = readU32(stream);

		actor->amountPaid = readU32(stream);

		actor->time = readFloat(stream);
	}
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
	if (version >= 4) tile.height = readU8(stream);
	return tile;
}

//@consider Frost keep has a square range
//@consider Everything but the frost keep scale mana usage by base damage?
