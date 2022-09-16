struct Globals {
};

enum ActorType {
	ACTOR_NONE=0,
	ACTOR_TOWER1, ACTOR_TOWER2, ACTOR_TOWER3, ACTOR_TOWER4,
	ACTOR_TOWER5, ACTOR_TOWER6, ACTOR_TOWER7, ACTOR_TOWER8,
	ACTOR_TOWER9, ACTOR_TOWER10, ACTOR_TOWER11, ACTOR_TOWER12,
	ACTOR_ENEMY1,
	ACTOR_BULLET = 64,
	ACTOR_TYPES_MAX,
};
struct Actor {
	ActorType type;
	int id;

	Vec2 position;
	Vec2 velo;
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
	bool isPortal;
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

struct Game {
	Font *defaultFont;

	Globals globals;
	bool inEditor;
	float timeScale;
	float time;
	Vec2 size;

	World *world;

	Vec2 cameraPosition;
	float cameraZoom;

	Vec2 mouse;

	int wave;
	bool playingWave;

	ActorType actorsToSpawn[ACTORS_MAX];
	int actorsToSpawnNum;
	float timeTillNextSpawn;

	/// Editor/debug
	bool debugShowFrameTimes;
	bool debugShowDijkstraValues;
	bool debugShowFlowFieldValues;
	bool debugDrawChunkLines;
};
Game *game = NULL;

void runGame();
void updateGame();
Chunk *createChunk(Vec2i position);
Chunk *getChunkAt(Vec2i position);
Tile *getTileAt(Vec2i position);
Vec2i chunkToWorldTile(Chunk *chunk, Vec2i tile);
Vec2i worldToTile(Vec2 position);
bool tileBlocksPathing(TileType type);

Actor *createActor(ActorType type);
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

	logf("Game+World is %.1fmb\n", (sizeof(World)+sizeof(Game)) / (float)(Megabytes(1)));

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 80);

		game->timeScale = 1;

		maximizeWindow();

		rndInt(0, 3); // Burn an rnd seed???
		game->cameraZoom = 1;

		game->world = (World *)zalloc(sizeof(World));
		World *world = game->world;

		{ /// Generate map
			Chunk **chunksCouldExpand = (Chunk **)frameMalloc(sizeof(Chunk) * CHUNKS_MAX);
			int chunksCouldExpandNum = 0;

			Chunk *chunk = createChunk(v2i(0, 0));

			for (;;) {
				if (world->chunksNum > 45-1) {
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
					logf("%d (%d)\n", chosenIndex, possiblePositionsNum-1);
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
				if (chunk->connectionsNum == 1 && !isZero(chunk->position)) chunk->isPortal = true;
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

				for (int i = 0; i < ceilf(CHUNK_SIZE/2.0); i++) {
					if (cutLeft) chunk->tiles[centerTile.y * CHUNK_SIZE + (centerTile.x - i)].type = TILE_ROAD;
					if (cutRight) chunk->tiles[centerTile.y * CHUNK_SIZE + (centerTile.x + i)].type = TILE_ROAD;
					if (cutUp) chunk->tiles[(centerTile.y - i) * CHUNK_SIZE + centerTile.x].type = TILE_ROAD;
					if (cutDown) chunk->tiles[(centerTile.y + i) * CHUNK_SIZE + centerTile.x].type = TILE_ROAD;
				}
			}

			world->chunks[0].visible = true;
		} ///

		{ /// Build dijkstra
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

					if (tileBlocksPathing(currentTile->type)) {
						currentTile->dijkstraValue = -1;
						continue;
					}

					for (int i = 0; i < 4; i++) {
						Vec2i neighbor = current;
						if (i == 0) neighbor += v2i(-1, 0);
						if (i == 1) neighbor += v2i(1, 0);
						if (i == 2) neighbor += v2i(0, -1);
						if (i == 3) neighbor += v2i(0, 1);

						Tile *neighborTile = getTileAt(neighbor);
						if (!neighborTile) continue;

						if (tileBlocksPathing(neighborTile->type)) {
							neighborTile->dijkstraValue = -1;
							continue;
						}

						int newCost = currentTile->costSoFar + 1;

						if (neighborTile->costSoFar == 0 || newCost < neighborTile->costSoFar) {
							neighborTile->dijkstraValue = newCost;
							neighborTile->costSoFar = newCost;

							priorityQueuePush(frontier, &neighbor, newCost);
						}
					}
				}

				destroyPriorityQueue(frontier);
			}
		} ///

		{ /// Build Flow Field
			for (int i = 0; i < world->chunksNum; i++) {
				Chunk *chunk = &world->chunks[i];

				for (int y = 0; y < CHUNK_SIZE; y++) {
					for (int x = 0; x < CHUNK_SIZE; x++) {
						Vec2i localPos = v2i(x, y);
						Vec2i worldPos = chunkToWorldTile(chunk, localPos);

						Tile *currentTile = getTileAt(worldPos);

						if (currentTile->dijkstraValue <= 0) {
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
		} ///
	}

	game->size = v2(platform->windowSize);

	Globals *globals = &game->globals;
	World *world = game->world;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	if (game->inEditor) {
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Checkbox("Show Dijkstra values", &game->debugShowDijkstraValues);
		ImGui::Checkbox("Show Flow Field values", &game->debugShowFlowFieldValues);
		ImGui::Checkbox("Draw chunk lines", &game->debugDrawChunkLines);
		if (ImGui::Button("Explore all")) {
			for (int i = 0; i < world->chunksNum; i++) {
				Chunk *chunk = &world->chunks[i];
				chunk->visible = true;
			}
		}

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

	{
		Vec2 moveDir = v2();
		if (keyPressed('W')) moveDir.y--;
		if (keyPressed('S')) moveDir.y++;
		if (keyPressed('A')) moveDir.x--;
		if (keyPressed('D')) moveDir.x++;

		game->cameraZoom += platform->mouseWheel * 0.1;
		game->cameraZoom = mathClamp(game->cameraZoom, 0.1, 20);

		game->cameraPosition += normalize(moveDir) * 10 / game->cameraZoom;
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

					if (tile->type == TILE_ROAD) color = 0xFF966F02;
					if (tile->type == TILE_GROUND) color = 0xFF017301;
					if (tile->type == TILE_HOME) color = 0xFFFFF333;

					drawRect(rect, color);

					if (game->debugShowDijkstraValues) {
						DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
						drawTextInRect(frameSprintf("%d", tile->dijkstraValue), props, rect);
					}

					if (game->debugShowFlowFieldValues) {
						if (tile->type == TILE_ROAD) {
							Vec2 start = getCenter(rect);
							Vec2 end = start + tile->flow*TILE_SIZE/2;
							drawLine(start, end, 4, 0xFFFF0000);
						}
					}
				}
			}
		}
	} ///

	{ /// Update and draw actors
		for (int i = 0; i < world->actorsNum; i++) {
			Actor *actor = &world->actors[i];

			int color = 0xFFFF00FF;

			Rect rect = {};
			rect.width = TILE_SIZE * 0.5;
			rect.height = TILE_SIZE * 0.5;
			rect.x = actor->position.x - rect.width/2;
			rect.y = actor->position.y - rect.height/2;

			float speed = 1;

			if (actor->type == ACTOR_ENEMY1) {
				color = 0xFF008000;

				Vec2 position = getPosition(rect);
				Vec2 size = getSize(rect);
				Vec2 corners[4] = {
					position + size*v2(-1, -1),
					position + size*v2(1, -1),
					position + size*v2(1, 1),
					position + size*v2(-1, 1)
				};

				Vec2 dir = v2();
				for (int i = 0; i < 4; i++) {
					Vec2i tilePosition = worldToTile(corners[i]);
					Tile *tile = getTileAt(tilePosition);
					if (tile && !isZero(tile->flow)) dir += tile->flow;
				}
				dir = normalize(dir);
				actor->velo = dir * speed;
			}

			actor->velo *= 0.9;
			actor->position += actor->velo;

			drawRect(rect, color);
		}
	}

	if (game->playingWave) {
		if (game->actorsToSpawnNum > 0) game->timeTillNextSpawn -= elapsed;
		if (game->timeTillNextSpawn <= 0) {
			game->timeTillNextSpawn += 1;

			Vec2 *spawnPoints = (Vec2 *)frameMalloc(sizeof(Vec2) * CHUNKS_MAX);
			int spawnPointsNum = 0;
			for (int i = 0; i < world->chunksNum; i++) {
				Chunk *chunk = &world->chunks[i];
				if (!chunk->visible) continue;
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
				Actor *actor = createActor(ACTOR_ENEMY1);
				actor->position = spawnPoints[i];

				memmove(&game->actorsToSpawn[0], &game->actorsToSpawn[1], sizeof(ActorType) * (game->actorsToSpawnNum-1));
				game->actorsToSpawnNum--;
				if (game->actorsToSpawnNum == 0) break;
			}
		}
	} else {
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
					if (platform->mouseJustUp) {
						newChunk->visible = true;
						game->playingWave = true;
						for (int i = 0; i < 10; i++) {
							game->actorsToSpawn[game->actorsToSpawnNum++] = ACTOR_ENEMY1;
						}
					}
				}

				drawRectOutline(exploreRect, 4, 0xFFCCCCCC);

				DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
				drawTextInRect("Explore here", props, inflatePerc(exploreRect, -0.1));
			}

			if (chunk->isPortal) drawRect(chunk->rect, 0x80FF0000);
		}
	}

	if (game->debugDrawChunkLines) {
		Vec2i minChunk = v2i();
		Vec2i maxChunk = v2i();
		for (int i = 0; i < world->chunksNum; i++) {
			Chunk *chunk = &world->chunks[i];
			if (minChunk.x > chunk->position.x) minChunk.x = chunk->position.x;
			if (minChunk.y > chunk->position.y) minChunk.y = chunk->position.y;
			if (maxChunk.x < chunk->position.x) maxChunk.x = chunk->position.x;
			if (maxChunk.y < chunk->position.y) maxChunk.y = chunk->position.y;
		}

		for (int y = minChunk.y; y < maxChunk.y; y++) {
			Vec2 start = v2();
			start.x = minChunk.x * CHUNK_SIZE * TILE_SIZE;
			start.y = y * CHUNK_SIZE * TILE_SIZE;

			Vec2 end = start;
			start.x = maxChunk.x * CHUNK_SIZE * TILE_SIZE;

			drawLine(start, end, 0xFFFF0000);
		}
		for (int x = minChunk.x; x < maxChunk.x; x++) {
		}
	}

	// drawCircle(game->mouse, 10, 0xFFFF0000);

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

	for (int y = 0; y < CHUNK_SIZE; y++) {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			Tile *tile = &chunk->tiles[y * CHUNK_SIZE + x];
			tile->type = TILE_GROUND;
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

Vec2i chunkToWorldTile(Chunk *chunk, Vec2i tile) {
	tile.x += chunk->position.x * CHUNK_SIZE;
	tile.y += chunk->position.y * CHUNK_SIZE;
	return tile;
}

Vec2i worldToTile(Vec2 position) {
	Vec2i tile;
	tile.x = position.x / TILE_SIZE;
	tile.y = position.y / TILE_SIZE;
	return tile;
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
	actor->id = ++world->nextActorId;
	actor->type = type;

	return actor;
}
