struct Globals {
};

enum ActorType {
	ACTOR_NONE=0,
	ACTOR_TOWER1, ACTOR_TOWER2, ACTOR_TOWER3, ACTOR_TOWER4,
	ACTOR_TOWER5, ACTOR_TOWER6, ACTOR_TOWER7, ACTOR_TOWER8,
	ACTOR_TOWER9, ACTOR_TOWER10, ACTOR_TOWER11, ACTOR_TOWER12,
	ACTOR_ENEMY,
	ACTOR_BULLET = 64,
	ACTOR_TYPES_MAX,
};
struct Actor {
	ActorType type;
	Vec3 position;
};

enum TypeType {
	TILE_NONE,
	TILE_GROUND,
	TILE_ROAD,
};
struct Tile {
	u8 type;
};

#define TILE_SIZE 64
struct Chunk {
	Vec2i position;
#define CHUNK_SIZE 7
	Tile tiles[CHUNK_SIZE*CHUNK_SIZE];
};

struct World {
#define ACTORS_MAX 65535
	Actor actors[ACTORS_MAX];
	int actorsNum;

#define CHUNKS_MAX 512
	Chunk chunks[CHUNKS_MAX];
	int chunksNum;
};

struct Game {
	Font *defaultFont;

	Globals globals;
	bool inEditor;
	float timeScale;
	float time;

	World *world;

	Vec2 cameraPosition;
	float cameraZoom;

	/// Editor/debug
	bool debugShowFrameTimes;
};
Game *game = NULL;

void runGame();
void updateGame();
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

	logf("World is %.1fmb\n", sizeof(World) / (float)(Megabytes(1)));

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 20);

		game->timeScale = 1;

		maximizeWindow();

		game->cameraZoom = 1;

		game->world = (World *)zalloc(sizeof(World));
		World *world = game->world;

		{ /// Map gen
			Vec2i *validChunks = (Vec2i *)frameMalloc(sizeof(Vec2i) * CHUNKS_MAX);
			int validChunksNum = 0;

			Vec2i *chunksCouldExpand = (Vec2i *)frameMalloc(sizeof(Vec2i) * CHUNKS_MAX);
			int chunksCouldExpandNum = 0;

			auto isEmptyChunk = [validChunks, validChunksNum](Vec2i chunk)->bool {
				for (int i = 0; i < validChunksNum; i++) {
					if (equal(validChunks[i], chunk)) return false;
				}
				return true;
			};

			auto addNewExpandableChunkAround = [isEmptyChunk, chunksCouldExpand, chunksCouldExpandNum](Vec2i chunkToExpand) mutable {
				int dir = rndInt(0, 3);

				Vec2i possibleNewExpandChunks[4];
				int possibleNewExpandChunksNum = 0;
				possibleNewExpandChunks[possibleNewExpandChunksNum++] = chunkToExpand + v2i(-1, 0);
				possibleNewExpandChunks[possibleNewExpandChunksNum++] = chunkToExpand + v2i(1, 0);
				possibleNewExpandChunks[possibleNewExpandChunksNum++] = chunkToExpand + v2i(0, -1);
				possibleNewExpandChunks[possibleNewExpandChunksNum++] = chunkToExpand + v2i(0, 1);
				for (int i = 0; i < possibleNewExpandChunksNum; i++) {
					Vec2i possibleExpandChunk = possibleNewExpandChunks[i];
					if (!isEmptyChunk(possibleExpandChunk)) {
						arraySpliceIndex(possibleNewExpandChunks, possibleNewExpandChunksNum, sizeof(Vec2i), i);
						possibleNewExpandChunksNum--;
						i--;
						continue;
					}
				}

				if (possibleNewExpandChunksNum > 0) {
					chunksCouldExpand[chunksCouldExpandNum++] = possibleNewExpandChunks[rndInt(0, possibleNewExpandChunksNum-1)];
				}
			};

			validChunks[validChunksNum++] = v2i(0, 0);
			int dir = rndInt(0, 3);
			if (dir == 0) chunksCouldExpand[chunksCouldExpandNum++] = v2i(-1, 0);
			else if (dir == 1) chunksCouldExpand[chunksCouldExpandNum++] = v2i(1, 0);
			else if (dir == 2) chunksCouldExpand[chunksCouldExpandNum++] = v2i(0, -1);
			else if (dir == 3) chunksCouldExpand[chunksCouldExpandNum++] = v2i(0, 1);

			for (;;) {
				if (validChunksNum > CHUNKS_MAX-1) {
					logf("Done.\n");
					break;
				}

				if (chunksCouldExpandNum == 0) addNewExpandableChunkAround(validChunks[rndInt(0, validChunksNum-1)]);

				int expandIndex = rndInt(0, chunksCouldExpandNum-1);
				Vec2i chunkToExpand = chunksCouldExpand[expandIndex];

				if (!isEmptyChunk(chunkToExpand)) {
					arraySpliceIndex(chunksCouldExpand, chunksCouldExpandNum, sizeof(Vec2i), expandIndex);
					chunksCouldExpandNum--;
					continue;
				}

				validChunks[validChunksNum++] = chunkToExpand;
				addNewExpandableChunkAround(chunkToExpand);
			}
		} ///

		Chunk *chunk = &world->chunks[world->chunksNum++];
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				int tileIndex = y*CHUNK_SIZE + x;
				Tile *tile = &chunk->tiles[tileIndex];
				if (x == CHUNK_SIZE/2 || y == CHUNK_SIZE/2) {
					tile->type = TILE_ROAD;
				} else {
					tile->type = TILE_GROUND;
				}
			}
		}
	}

	Globals *globals = &game->globals;
	World *world = game->world;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;

	clearRenderer();

	Matrix3 cameraMatrix = mat3();
	cameraMatrix.TRANSLATE(v2(platform->windowSize)/2);
	cameraMatrix.SCALE(game->cameraZoom);
	cameraMatrix.TRANSLATE(-v2(platform->windowSize)/2);
	cameraMatrix.TRANSLATE(game->cameraPosition);

	pushCamera2d(cameraMatrix);

	{
		Vec2 moveDir = v2();
		if (keyPressed('W')) moveDir.y++;
		if (keyPressed('S')) moveDir.y--;
		if (keyPressed('A')) moveDir.x++;
		if (keyPressed('D')) moveDir.x--;

		game->cameraZoom += platform->mouseWheel * 0.1;
		game->cameraZoom = mathClamp(game->cameraZoom, 0.1, 20);

		game->cameraPosition += normalize(moveDir) * 10;
	}

	for (int i = 0; i < world->chunksNum; i++) {
		Chunk *chunk = &world->chunks[i];
		Rect chunkRect = {};
		chunkRect.width = TILE_SIZE * CHUNK_SIZE;
		chunkRect.height = TILE_SIZE * CHUNK_SIZE;
		chunkRect.x = chunk->position.x * chunkRect.width;
		chunkRect.y = chunk->position.y * chunkRect.height;
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int x = 0; x < CHUNK_SIZE; x++) {
				int tileIndex = y*CHUNK_SIZE + x;
				Rect rect = {};
				rect.x = x*TILE_SIZE + chunkRect.x;
				rect.y = y*TILE_SIZE + chunkRect.y;
				rect.width = TILE_SIZE;
				rect.height = TILE_SIZE;
				Tile *tile = &chunk->tiles[tileIndex];

				int color = 0x00000000;

				if (tile->type == TILE_ROAD) color = 0xFF966F02;
				if (tile->type == TILE_GROUND) color = 0xFF017301;

				drawRect(rect, color);
			}
		}
	}

	popCamera2d();
	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) {
		char *str = frameSprintf("%.1fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
	}

	guiDraw();
	drawOnScreenLog();

	game->time += elapsed;
}
