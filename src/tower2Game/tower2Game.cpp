// Hp bar with tick marks
// Allow map editor to regenerate perlin heightmap
// Have the option to destroy portals
// Cumulative saves
// Add the rest of the enemies

// Graphics:
// Ground plane/fog
// Damage slowmo effect
// Tower models and textures

// Upgrade ideas:
// Tower has a small chance of freezing
// Poison explosion
// Fire spread
// More levels

// Mode ideas:
// A huge amount of starting money with few (1?) big waves
// Dying enemies spawn a copy of the previous type
// 8/10-split
// Hyrda roulette
// All towers do 80% of their damage as poison

#define FROST_FALL_DISTANCE 64

#define POISON_COLOR 0xFF6B4876
#define BURN_COLOR 0xFFDCAB2C
#define BLEED_COLOR 0xFF770000
#define SHIELD_COLOR 0xFF718691
#define HP_COLOR 0xFF00FF00
#define ARMOR_COLOR 0xFFFFD66E

#define XP_PER_SEC 10
#define FLAME_RADS (toRad(15))

#define StartForEachUpgradeEffect for (int i = 0; i < data->ownedUpgradesNum; i++) { \
				Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]); \
				for (int i = 0; i < upgrade->effectsNum; i++) { \
					UpgradeEffect *effect = &upgrade->effects[i];
#define EndForEachUpgradeEffect }}

#define CORE_HEADER
#include "tower2GameCore.cpp"

#define SCALE_3D (1.0/64.0)
enum EffectType {
	EFFECT_DEFAULT_CORE_EVENT,
};
struct Effect {
	EffectType type;
	ActorType actorType;
	Vec3 position;

	float time;
	float floatValue;

	CoreEvent coreEvent;
};

struct WorldChannel {
	int channelId;
	Vec3 position;
};

struct NguiNamedStyleStack {
#define NGUI_NAMED_STYLE_STACK_NAME_MAX_LEN 64
	char name[NGUI_NAMED_STYLE_STACK_NAME_MAX_LEN];
	NguiStyleStack style;
};

struct Globals {
	Vec3 cameraAngle;
	float cameraBaseDistance;

#define CUSTOM_STYLE_STACKS_MAX 64
	NguiNamedStyleStack customStyleStacks[CUSTOM_STYLE_STACKS_MAX];
	int customStyleStacksNum;
};

enum ParticleType {
	PARTICLE_FLAME,
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

enum GameState {
	GAME_MENU,
	GAME_PLAY,
};

struct Game {
	Font *defaultFont;

	Globals globals;

	bool isDemo;
	bool inEditor;
	float timeScale;
	Vec2 size;
	Vec2 mouse;

	GameState prevState;
	GameState state;
	float stateTime;

	bool shouldReset;
	char *shouldLoadState;
	bool is2d;

	Vec3 mouseRayPos;
	Vec3 mouseRayDir;
	Vec2i hovered3dTilePos;
	Camera lastMainPassCamera;

	RenderTexture *gameTexture;
	RenderTexture *finalTexture;
	RenderTexture *sunTexture;

	Shader *fxaaShader;
	int fxaaResolutionLoc;

	Shader *postShader;

	Particle *particles;
	int particlesNum;
	int particlesMax;

	Vec3 sunPosition;

#define WORLD_SOUNDS_MAX CHANNELS_MAX
	WorldChannel worldChannels[WORLD_SOUNDS_MAX];
	int worldChannelsNum;

#define EFFECTS_MAX 1024
	Effect effects[EFFECTS_MAX];
	int effectsNum;

	bool uiUpgradeListOpened;
	bool uiWavesListOpened;

	bool isOnLastStep;

	Core core;

	/// Editor/debug
	bool debugShowFrameTimes;
	bool debugShowDijkstraValues;
	bool debugShowFlowFieldValues;
	bool debugShowPerlinValues;
	bool debugDrawChunkLines;
	bool debugDrawTileLines;
	bool debugShowActorVelo;
	bool debugDisableBackfaceCulling;

	char debugNewSaveStateName[PATH_MAX_LEN];

	Tile *selectedTile;
	Vec2i selectedTilePosition;
};

Game *game = NULL;
GameData *data = NULL;
Core *core = NULL;

void runGame();
void updateGame();

bool isMouseClicked();
Vec2i getTileHovering();
bool isHoveringActor(Actor *actor);
CoreEvent *createCoreEvent(CoreEventType type, Actor *src=NULL, Actor *dest=NULL);
#include "tower2GameCore.cpp"
void drawGame(float elapsed);
Effect *createEffect(EffectType type);

char *getUpgradeDescription(Upgrade *upgrade);

float getTile3dHeight(Vec2i tilePos);
AABB getTileAABB(Vec2i tilePos);
AABB getAABB(Actor *actor);
Vec3 to3d(Vec2 value);
float to3d(float value);
Vec3 radsTo3dDir(float rads);
Vec2 to2d(Vec3 value);
Vec2 toScreenPass(Vec3 worldPosition);
Matrix3 toMatrix3(Rect rect);

Matrix4 toMatrix(AABB aabb);
#define getBeamMatrix(a, b, c) (getBeamMatrix)(a, b, c*0.5)
void draw3dRing(Vec3 center, float radius, int color, int points=24, float thickness=0.2);
Particle *createParticle(ParticleType type);

void updateAndDrawOverlay(float elapsed);

int playWorldSound(char *path, Vec3 worldPosition);
void pushGameStyleStack(char *name);
void popGameStyleStack(char *name);

void saveGlobals();
void loadGlobals();
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

	Vec2 res = v2(1280, 720);
#ifdef __EMSCRIPTEN__
	// res *= 0.5;

	EM_ASM({
		let statusElement = document.getElementById("status");
		statusElement.style.display = "none";
	});
#endif

	defeatWindowsScalingInHtml5 = true;
	initPlatform(res.x, res.y, "tower2");
	platform->sleepWait = true;
	initAudio();
	initRenderer(res.x, res.y);
	initTextureSystem();
	initFonts();
	nguiInit();

	logf("Core+Game is %.1fmb\n", (sizeof(Core)+sizeof(Game)) / (float)(Megabytes(1)));

	platformUpdateLoop(updateGame);
}

void updateGame() {
	bool isFirstStart = false;
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		core = &game->core;
		data = &core->data;

		game->defaultFont = createFont("assets/common/arial.ttf", 80);

		game->fxaaShader = loadShader(NULL, "assets/common/shaders/raylib/glsl330/fxaa.fs", NULL, "assets/common/shaders/raylib/glsl100/fxaa.fs");
		game->fxaaResolutionLoc = getUniformLocation(game->fxaaShader, "resolution");

		game->postShader = loadShader(NULL, "assets/shaders/glsl330/post.fs", NULL, "assets/shaders/glsl100/post.fs");

#ifdef __EMSCRIPTEN__
		game->isDemo = true;
#endif

		// if (ArrayLength(upgradeEffectTypeStrings) != UPGRADE_EFFECT_TYPES_MAX) Panic("Upgrade type string mismatch\n");

		loadGlobals();

		game->timeScale = 1;
		game->is2d = false;
		// game->sunPosition = v3(0, -300, 200);

		maximizeWindow();

		rndInt(0, 3); // Burn an rnd seed???

		if (game->isDemo) {
			initCore(MAP_GEN_NONE);
			loadState("assets/states/demo.save_state");
			game->state = GAME_PLAY;
		}
	}

	if (!equal(game->size, v2(platform->windowSize))) {
		game->size = v2(platform->windowSize);

		if (game->gameTexture) destroyTexture(game->gameTexture);
		game->gameTexture = createRenderTexture(game->size.x, game->size.y);

		if (game->finalTexture) destroyTexture(game->finalTexture);
		game->finalTexture = createRenderTexture(game->size.x, game->size.y);

		if (game->sunTexture) destroyTexture(game->sunTexture);
		game->sunTexture = createRenderTexture(game->size.x, game->size.y);
	}

	ngui->mouse = platform->mouse;
	ngui->screenSize = game->size;
	ngui->uiScale = game->size.y / 2160;

	if (game->prevState != game->state) {
		game->prevState = game->state;
		game->stateTime = 0;
	}

	if (game->state == GAME_MENU) {
		ImGui::SetNextWindowPos(ImVec2(game->size.x/2, game->size.y/2), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button("Continue") || keyJustPressed('W')) {
			initCore(MAP_GEN_NONE);
			loadState("assets/states/autosave.save_state");
			game->state = GAME_PLAY;
		}

		if (ImGui::Button("Play random")) {
			initCore(MAP_GEN_RANDOM);
			game->state = GAME_PLAY;
		}

		if (ImGui::Button("Play controlled split")) {
			initCore(MAP_GEN_CONTROLLED_SPLIT);
			game->state = GAME_PLAY;
		}

		ImGui::End();
	} else if (game->state == GAME_PLAY) {
		if (game->shouldReset) {
			game->shouldReset = false;
			initCore(data->mapGenMode);
		}

		if (game->shouldLoadState) {
			loadState(game->shouldLoadState);
			free(game->shouldLoadState);
			game->shouldLoadState = NULL;
		}

		int stepsToTake = 1;
		float elapsed = platform->elapsed;
		Globals *globals = &game->globals;

		if (game->timeScale > 1) {
			stepsToTake = game->timeScale;
		} else if (game->timeScale < 1) {
			elapsed *= game->timeScale;
		}

		for (int i = 0; i < stepsToTake; i++) {
			game->isOnLastStep = false;
			if (i == stepsToTake-1) game->isOnLastStep = true;

			stepGame(elapsed);
			drawGame(elapsed);
		}

		updateAndDrawOverlay(elapsed*stepsToTake);

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

				Vec2 screenPos = worldSpaceTo2dNDC01(game->lastMainPassCamera, worldChannel->position) * game->size;

				Vec2 screenPerc = (game->size/2 - screenPos) / game->size*2;
				float dist = distance(game->lastMainPassCamera.position, worldChannel->position);
				float vol = clampMap(dist, 0, 200, 1, 0);
				float pan = screenPerc.x;
				channel->userVolume2 = vol;
				channel->pan = pan;
			}
		} ///
	}

	guiDraw(); // I'm not sure why this is here
}

bool isMouseClicked() {
	bool ret = platform->mouseJustDown;
	if (ngui->mouseHoveringThisFrame) ret = false;
	if (ngui->mouseHoveringLastFrame) ret = false;
	if (ngui->mouseJustDownThisFrame) ret = false;
	if (data->prevTool != data->tool) ret = false;
	if (data->toolTime < 0.05) ret = false;
	if (!game->isOnLastStep) ret = false;
	return ret;
}

Vec2i getTileHovering() {
	if (game->is2d) {
		return worldToTile(game->mouse);
	} else {
		return game->hovered3dTilePos;
	}
}

bool isHoveringActor(Actor *actor) {
	if (game->is2d) {
		return contains(getRect(actor), game->mouse);
	} else {
		AABB aabb = getAABB(actor);
		Line3 line = makeLine3(game->mouseRayPos, game->mouseRayPos + game->mouseRayDir*100.0);
		return overlaps(aabb, line);
	}
}

CoreEvent *createCoreEvent(CoreEventType type, Actor *src, Actor *dest) {
	if (type == CORE_EVENT_WAVE_OVER) {
		game->uiWavesListOpened = false;
		return NULL;
	}

	if (core->coreEventsNum > CORE_EVENTS_MAX-1) {
		logf("Too many core events!!!\n");
		core->coreEventsNum--;
	}
	CoreEvent *event = &core->coreEvents[core->coreEventsNum++];
	memset(event, 0, sizeof(CoreEvent));
	event->type = type;
	if (src) event->srcId = src->id;
	if (dest) event->destId = dest->id;
	return event;
}

void drawGame(float elapsed) {
	Globals *globals = &game->globals;

	clearRenderer();
	Mesh *cubeMesh = getMesh("assets/common/models/Cube.Cube.mesh");
	Mesh *sphereMesh = getMesh("assets/common/models/Sphere.Sphere.mesh");

	{ /// Iterate CoreEvents
		for (int i = 0; i < core->coreEventsNum; i++) {
			CoreEvent *event = &core->coreEvents[i];
			if (
				event->type == CORE_EVENT_DAMAGE ||
				event->type == CORE_EVENT_SHOW_GHOST ||
				event->type == CORE_EVENT_MORTAR_EXPLOSION ||
				event->type == CORE_EVENT_SHOOT ||
				event->type == CORE_EVENT_HIT
			) {
				Effect *effect = createEffect(EFFECT_DEFAULT_CORE_EVENT);
				effect->coreEvent = *event;
			}
		}
	} ///

	Pass *mainPass = NULL;
	Pass *screenPass = NULL;
	{ /// Setup camera
		if (game->is2d) {
			Matrix3 cameraMatrix = mat3();
			cameraMatrix = mat3();
			cameraMatrix.TRANSLATE(game->size/2);
			cameraMatrix.SCALE(data->cameraZoom);
			cameraMatrix.TRANSLATE(-data->cameraPosition);

			game->mouse = cameraMatrix.invert() * platform->mouse;

			pushCamera2d(cameraMatrix);
		} else {
			mainPass = createPass();
			screenPass = createPass();
			pushPass(mainPass);

			Vec3 cameraTarget = to3d(data->cameraPosition);

			Matrix4 srcMatrix = mat4();
			srcMatrix.TRANSLATE(cameraTarget);
			srcMatrix.ROTATE_EULER(globals->cameraAngle);
			globals->cameraBaseDistance = 200;
			srcMatrix.TRANSLATE(0, 0, globals->cameraBaseDistance);
			srcMatrix.TRANSLATE(0, 0, -data->cameraZoom * 64);
			Vec3 cameraSrc = srcMatrix * v3();

			mainPass->camera.position = cameraSrc;
			mainPass->camera.target = cameraTarget;
			mainPass->camera.up = v3(0, 0, 1);

			mainPass->camera.fovy = 59;
			mainPass->camera.isOrtho = false;
			mainPass->camera.size = game->size;
			mainPass->camera.nearCull = 0.1;
			mainPass->camera.farCull = 500;
			game->lastMainPassCamera = mainPass->camera;

			screenPass->camera.position = v3(0, 0.0001, 2);
			screenPass->camera.target = v3(0, 0, 0);
			screenPass->camera.up = v3(0, 0, -1);
			screenPass->camera.isOrtho = true;
			screenPass->camera.nearCull = 0.1;
			screenPass->camera.farCull = 500;
			screenPass->camera.size = game->size;

			screenPass->camera.position += v3(screenPass->camera.size/2, 0);
			screenPass->camera.target += v3(screenPass->camera.size/2, 0);

			getMouseRay(mainPass->camera, platform->mouse, &game->mouseRayPos, &game->mouseRayDir);
		}
	} ///

	{ /// Draw map
		float closestHoveredTileDist = -1;
		Vec2i closestHoveredTilePos = v2i();

		for (int i = 0; i < data->chunksNum; i++) {
			Chunk *chunk = &data->chunks[i];
			if (!chunk->visible) continue;

			if (game->is2d) {
				Rect screenRect2d = {};
				screenRect2d.width = game->size.x / data->cameraZoom;
				screenRect2d.height = game->size.y / data->cameraZoom;
				screenRect2d.x = data->cameraPosition.x - screenRect2d.width/2;
				screenRect2d.y = data->cameraPosition.y - screenRect2d.height/2;

				if (!overlaps(screenRect2d, chunk->rect)) continue;
			} else {
				// Cull 3d here
			}

			for (int y = 0; y < CHUNK_SIZE; y++) {
				for (int x = 0; x < CHUNK_SIZE; x++) {
					int tileIndex = y*CHUNK_SIZE + x;
					Tile *tile = &chunk->tiles[tileIndex];

					int color = 0x00000000;
					if (tile->type == TILE_HOME) color = 0xFFFFF333;
					if (tile->type == TILE_GROUND) color = 0xFF017301;
					if (tile->type == TILE_ROAD) color = 0xFF966F02;

					float heightShadePerc = clampMap(tile->elevation, 0, 3, 0, 0.25);
					// float heightShadePerc = clampMap(tile->elevation, 0, 255, 0, 1);
					color = lerpColor(color, 0xFF000000, heightShadePerc);

					if (game->is2d) {
						Rect rect = {};
						rect.x = x*TILE_SIZE + chunk->rect.x;
						rect.y = y*TILE_SIZE + chunk->rect.y;
						rect.width = TILE_SIZE;
						rect.height = TILE_SIZE;
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
					} else {
						Vec2i tilePos = chunkTileToWorldTile(chunk, v2i(x, y));
						AABB aabb = getTileAABB(tilePos);
						Matrix4 matrix = toMatrix(aabb);
						passMesh(cubeMesh, matrix, color);
						if (game->isOnLastStep) {
							for (int i = 0; i < cubeMesh->indsNum/3; i++) {
								Tri tri = {};
								tri.verts[0] = matrix * cubeMesh->verts[cubeMesh->inds[i*3 + 0]].position;
								tri.verts[1] = matrix * cubeMesh->verts[cubeMesh->inds[i*3 + 1]].position;
								tri.verts[2] = matrix * cubeMesh->verts[cubeMesh->inds[i*3 + 2]].position;

								float dist;
								Vec2 uv;
								if (rayIntersectsTriangle(game->mouseRayPos, game->mouseRayDir, tri, &dist, &uv)) {
									if (closestHoveredTileDist == -1 || closestHoveredTileDist > dist) {
										closestHoveredTileDist = dist;
										closestHoveredTilePos = tilePos;
									}
								}
							}
						}
					}
				}
			}

			if (game->is2d && game->debugDrawChunkLines) drawRectOutline(chunk->rect, 8, 0xA0FFFFFF);
		}

		if (closestHoveredTileDist != -1) game->hovered3dTilePos = closestHoveredTilePos;
	} ///

	{ /// Draw actors
		for (int i = 0; i < data->actorsNum; i++) {
			Actor *actor = &data->actors[i];
			ActorTypeInfo *info = &core->actorTypeInfos[actor->type];

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
				if (game->is2d) {
					drawRect(inflatePerc(getRect(actor), 0.2), 0xFFEAF82A);
				} else {
				}

				if (info->isTower) {
					if (game->is2d) {
						Circle range = makeCircle(actor->position, getRange(actor, worldToTile(actor->position)));
						drawCircle(range, 0x80FF0000);
					} else {
						float radius = to3d(getRange(actor, worldToTile(actor->position)));
						draw3dRing(to3d(actor->position), radius, 0xFFFF0000, 24);
					}
				}
			}

			Rect rect = getRect(actor);
			AABB aabb = getAABB(actor);

			if (actor->type == ACTOR_BALLISTA) {
				if (game->is2d) {
					drawRect(rect, 0xFF800000);

					Line2 line;
					line.start = getCenter(rect);
					line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
					drawLine(line, 4, 0xFFFF0000);
				} else {
					Matrix4 matrix = toMatrix(aabb);
					// matrix.TRANSLATE(0, 0, timePhase(platform->time)*3);
					passMesh(cubeMesh, matrix, getInfo(actor)->primaryColor);

					{
						Vec3 start = getCenter(aabb);
						Vec3 dir = radsTo3dDir(actor->aimRads);
						Vec3 end = start + dir*1*TILE_SIZE*SCALE_3D;
						Matrix4 matrix = getBeamMatrix(start, end, 0.2*TILE_SIZE*SCALE_3D);
						passMesh(cubeMesh, matrix, 0xFF202020);
					}
				}
			} else if (actor->type == ACTOR_MORTAR_TOWER) {
				if (game->is2d) {
					drawRect(rect, 0xFF525252);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_TESLA_COIL) {
				float perc = clampMap(actor->timeSinceLastShot, 0, 0.5, 0.5, 0);

				if (game->is2d) {
					Circle circle = makeCircle(actor->position, getRange(actor, worldToTile(actor->position)));
					drawCircle(makeCircle(getCenter(rect), rect.width/2), 0xFFA0A0F0);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_FROST_KEEP) {
				if (game->is2d) {
					drawRect(rect, 0xFFE3F0F5);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_FLAME_THROWER) {
				if (game->is2d) {
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
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_POISON_SPRAYER) {
				if (game->is2d) {
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
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_SHREDDER) {
				if (game->is2d) {
					drawRect(rect, 0xFF800000);

					Line2 line;
					line.start = getCenter(rect);
					line.end = line.start + radToVec2(actor->aimRads)*(TILE_SIZE/2);
					line.start = line.end - radToVec2(actor->aimRads)*(TILE_SIZE);
					drawLine(line, 4, 0xFFFF0000);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_MANA_SIPHON) {
				if (game->is2d) {
					drawRect(rect, lerpColor(0xFFA4CCC8, 0xFF000000, 0.25));
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_MANA_CRYSTAL) {
				if (game->is2d) {
					drawRect(rect, 0xFFA4B0CC);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type >= ACTOR_ENEMY1 && actor->type <= ACTOR_ENEMY64) {
				if (game->is2d) {
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

					float poisonAmount = sumDotTicks(actor, DOT_POISON);
					float burnAmount = sumDotTicks(actor, DOT_BURN);
					float bleedAmount = sumDotTicks(actor, DOT_BLEED);

					if (poisonAmount) {
						Rect textRect = getRect(actor);
						textRect.x -= textRect.width;
						textRect.y -= textRect.height;
						DrawTextProps props = newDrawTextProps(game->defaultFont, POISON_COLOR);
						drawTextInRect(frameSprintf("%.0f", poisonAmount), props, textRect);
					}

					if (burnAmount) {
						Rect textRect = getRect(actor);
						textRect.y -= textRect.height;
						DrawTextProps props = newDrawTextProps(game->defaultFont, BURN_COLOR);
						drawTextInRect(frameSprintf("%.0f", burnAmount), props, textRect);
					}

					if (bleedAmount) {
						Rect textRect = getRect(actor);
						textRect.x += textRect.width;
						textRect.y -= textRect.height;
						DrawTextProps props = newDrawTextProps(game->defaultFont, BLEED_COLOR);
						drawTextInRect(frameSprintf("%.0f", bleedAmount), props, textRect);
					}
				} else {
					passMesh(cubeMesh, toMatrix(aabb), 0xFF008000);

					{ /// Draw enemy ui
						pushPass(screenPass);

						Vec3 actorTop = to3d(actor->position);
						actorTop.z += to3d(getInfo(actor)->size.z) + 1;

						Vec2 cursor = toScreenPass(actorTop);

						{
							float oldCursorX = cursor.x;
							cursor.x -= 50 * ngui->uiScale;
							auto drawPoints = [](Vec2 *cursor, float points, int color) {
								Vec2 pipSize = v2(16, 16);
								pipSize.x = clampMap(points, 200, 3000, 32, 6);
								pipSize.y = clampMap(points, 1000, 5000, 8, 32);
								int pointsPerPip = 100;
								int pipCount = ceilf(points / (float)pointsPerPip);

								for (int i = 0; i < pipCount; i++) {
									Rect pipRect = makeRect(*cursor, pipSize);
									if (i == pipCount-1) {
										float pointsLeft = points - ((pipCount-1) * pointsPerPip);
										float pipPercLeft = pointsLeft / (float)pointsPerPip;
										pipRect.width *= pipPercLeft;
									}

									passTexture(renderer->whiteTexture, toMatrix3(pipRect), color);
									cursor->x += pipRect.width;
									cursor->x += pipSize.x * 0.15;
								}
							};

							drawPoints(&cursor, actor->hp, 0xFF00FF00);
							drawPoints(&cursor, actor->armor, 0xFFFFD66E);
							drawPoints(&cursor, actor->shield, 0xFF718691);

							cursor.x = oldCursorX;
						}

						{
							Rect baseTextRect = makeRect(v2(), v2(100, 100)*ngui->uiScale);
							baseTextRect.x = cursor.x - baseTextRect.width/2;
							baseTextRect.y = cursor.y;

							float poisonAmount = sumDotTicks(actor, DOT_POISON);
							float burnAmount = sumDotTicks(actor, DOT_BURN);
							float bleedAmount = sumDotTicks(actor, DOT_BLEED);

							if (poisonAmount) {
								Rect textRect = baseTextRect;
								textRect.x -= textRect.width;
								DrawTextProps props = newDrawTextProps(game->defaultFont, POISON_COLOR);
								passTextInRect(frameSprintf("%.0f", poisonAmount), props, textRect);
							}

							if (burnAmount) {
								Rect textRect = baseTextRect;
								DrawTextProps props = newDrawTextProps(game->defaultFont, BURN_COLOR);
								passTextInRect(frameSprintf("%.0f", burnAmount), props, textRect);
							}

							if (bleedAmount) {
								Rect textRect = baseTextRect;
								textRect.x += textRect.width;
								DrawTextProps props = newDrawTextProps(game->defaultFont, BLEED_COLOR);
								passTextInRect(frameSprintf("%.0f", bleedAmount), props, textRect);
							}
						}

						popPass();
					} ///
				}
			} else if (actor->type == ACTOR_ARROW) {
				if (game->is2d) {
					Rect bulletRect = makeCenteredSquare(actor->position, 8);
					drawRect(bulletRect, 0xFFFF0000);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_MORTAR) {
				float delayTime = 2 / getBulletSpeed(actor);
				float explodeRange = info->baseRange;
				if (actor->time < delayTime) {
					float ghostPerc = clampMap(actor->time, 0, delayTime, 0.75, 1);
					if (game->is2d) {
						drawCircle(actor->position, explodeRange*ghostPerc, 0x80900000);
					} else {
						draw3dRing(to3d(actor->position), to3d(explodeRange*ghostPerc), 0xFF802020);
					}
				}
			} else if (actor->type == ACTOR_FROST) {
				if (game->is2d) {
					drawRect(rect, 0x80FFFFFF);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else if (actor->type == ACTOR_SAW) {
				if (game->is2d) {
					Rect bulletRect = makeCenteredSquare(actor->position, 8);
					drawRect(bulletRect, 0xFFFF0000);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			} else {
				if (game->is2d) {
					drawRect(rect, 0xFFFF00FF);
				} else {
					passMesh(cubeMesh, toMatrix(aabb), getInfo(actor)->primaryColor);
				}
			}

			if (info->isTower) {
				if (game->is2d) {
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
				} else {
				}
			}

			if (getInfo(actor)->isEnemy) {
				if (isHoveringActor(actor)) {
					if (game->is2d) {
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
					} else {
					}
				}
			}

			if (game->is2d) {
				if (game->debugShowActorVelo) {
					DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
					drawTextInRect(frameSprintf("%.01f\n%.01f", actor->velo.x, actor->velo.y), props, rect);
					// Vec2 start = actor->position;
					// Vec2 end = actor->position + normalize(actor->velo)*(TILE_SIZE/2);
					// drawLine(start, end, 5, 0xFFFF0000);
				}
			}
		}
	} ///

	{ /// Update effects
		for (int i = 0; i < game->effectsNum; i++) {
			Effect *effect = &game->effects[i];

			bool complete = false;
			float maxTime = 0.1;

			if (effect->type == EFFECT_DEFAULT_CORE_EVENT) {
				CoreEvent *event = &effect->coreEvent;
				Actor *src = getActor(event->srcId);
				Actor *dest = getActor(event->destId);

				if (event->type == CORE_EVENT_DAMAGE) {
					maxTime = 1;
					float perc = effect->time / maxTime;
					if (perc == 0) {
						if (src) {
							effect->position = to3d(src->position);
							effect->position.x += to3d(rndFloat(-1, 1) * (game->size.x*0.01));
							effect->position.y += to3d(rndFloat(-1, 1) * (game->size.y*0.01));
						}
					}

					if (game->is2d) {
						Rect textRect = makeCenteredRect(to2d(effect->position), game->size*v2(0.01, 0.01));
						DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);

						if (event->shieldValue != 0) {
							props.color = SHIELD_COLOR;
							drawTextInRect(frameSprintf("-%.0f", event->shieldValue), props, textRect);
							textRect.y += textRect.height;
						}

						if (event->armorValue != 0) {
							props.color = ARMOR_COLOR;
							drawTextInRect(frameSprintf("-%.0f", event->armorValue), props, textRect);
							textRect.y += textRect.height;
						}

						if (event->hpValue != 0) {
							props.color = HP_COLOR;
							drawTextInRect(frameSprintf("-%.0f", event->hpValue), props, textRect);
							textRect.y += textRect.height;
						}
					} else {
						pushPass(screenPass);
						Rect textRect = makeCenteredRect(toScreenPass(effect->position), game->size*v2(0.03, 0.03));
						DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);

						if (event->shieldValue != 0) {
							props.color = SHIELD_COLOR;
							passTextInRect(frameSprintf("-%.0f", event->shieldValue), props, textRect);
							textRect.y += textRect.height;
						}

						if (event->armorValue != 0) {
							props.color = ARMOR_COLOR;
							passTextInRect(frameSprintf("-%.0f", event->armorValue), props, textRect);
							textRect.y += textRect.height;
						}

						if (event->hpValue != 0) {
							props.color = HP_COLOR;
							passTextInRect(frameSprintf("-%.0f", event->hpValue), props, textRect);
							textRect.y += textRect.height;
						}
						popPass();
					}
				} else if (event->type == CORE_EVENT_SHOOT) {
					if (effect->time == 0 && src) {
						effect->actorType = src->type;
						effect->position = to3d(src->position);
					}
					float range = 0;
					if (src) range = getRange(src, worldToTile(src->position));

					if (effect->actorType == ACTOR_BALLISTA) {
						if (effect->time == 0) playWorldSound("assets/audio/shoot/ballista", effect->position);
					} else if (effect->actorType == ACTOR_MORTAR_TOWER) {
						if (effect->time == 0) {
							if (src) effect->position = to3d(src->position);
							playWorldSound("assets/audio/shoot/launch", effect->position);
						}
					} else if (effect->actorType == ACTOR_TESLA_COIL) {
						if (effect->time == 0) {
							if (src) effect->position = to3d(src->position);
							playWorldSound("assets/audio/shoot/teslaCoil", effect->position);
						}

						maxTime = 0.15;
						float perc = effect->time / maxTime;

						int color = lerpColor(0xFFB8FFFA, 0x00B8FFFA, perc);
						if (game->is2d) {
							Circle circle = makeCircle(to2d(effect->position), range);
							drawCircle(circle, color);
						} else {
							int ringCount = 5;
							for (int i = 0; i < ringCount; i++) {
								float radius = to3d(range) * ((i+1) / (float)ringCount);
								draw3dRing(effect->position, radius, color, 16);
							}
						}
					} else if (effect->actorType == ACTOR_FLAME_THROWER || effect->actorType == ACTOR_POISON_SPRAYER) {
						complete = true;

						if (effect->time == 0) {
							if (src) effect->position = to3d(src->position);
							if (effect->actorType == ACTOR_FLAME_THROWER) {
								playWorldSound("assets/audio/shoot/flame", effect->position);
							} else {
								playWorldSound("assets/audio/shoot/spray", effect->position);
							}

							for (int i = 0; i < 10; i++) {
								Particle *particle = createParticle(PARTICLE_FLAME);
								particle->position = effect->position;

								float rads = src->aimRads;
								rads += rndFloat(-1, 1) * FLAME_RADS;
								Vec3 dir = radsTo3dDir(rads);
								dir.z += rndFloat(-1, 1) * 0.3;
								particle->velo = dir * rndFloat(0.1, 0.2);

								particle->maxTime = 0.5;
								if (effect->actorType == ACTOR_FLAME_THROWER) {
									particle->tint = lerpColor(0xFFFF0000, 0xFFFFFF00, rndFloat(0, 1));
								} else {
									particle->tint = lerpColor(0xFF612B9E, 0xFFFFFFFF, rndFloat(0, 0.2));
								}
							}
						}
					}
				} else if (event->type == CORE_EVENT_HIT) {
					if (effect->time == 0) {
						if (src) effect->actorType = src->type;
						if (dest) effect->position = to3d(dest->position);
					}
					if (effect->actorType == ACTOR_BALLISTA) {
						if (effect->time == 0) playWorldSound("assets/audio/hit/ballista", effect->position);
					} else if (effect->actorType == ACTOR_TESLA_COIL) {
						if (effect->time == 0) {
							Channel *channel = getChannel(playWorldSound("assets/audio/hit/teslaCoil", effect->position));
							if (channel) channel->delay = rndFloat(0, 0.05);
						}
					} else if (effect->actorType == ACTOR_SHREDDER) {
					}
				} else if (event->type == CORE_EVENT_MORTAR_EXPLOSION) {
					if (effect->time == 0) {
						effect->position = to3d(event->ghostOrMortarPosition);
						playWorldSound("assets/audio/shoot/explosion", effect->position);
					}

					float explodeRange = core->actorTypeInfos[ACTOR_MORTAR].baseRange;
					maxTime = 0.25;
					float perc = effect->time / maxTime;
					int color = lerpColor(0xFFFFFFFF, 0x00FF0000, perc);

					if (game->is2d) {
						Circle circle = makeCircle(to2d(effect->position), explodeRange);
						drawCircle(circle, color);
					} else {
						int ringCount = 10;
						for (int i = 0; i < ringCount; i++) {
							float radius = to3d(explodeRange) * ((i+1) / (float)ringCount);
							draw3dRing(effect->position, radius, color, 24);
						}
					}
				} else if (event->type == CORE_EVENT_SHOW_GHOST) {
					complete = true;
					Vec2i tilePos = worldToTile(event->ghostOrMortarPosition);
					if (game->is2d) {
						Rect tileRect = tileToWorldRect(tilePos);
						drawRect(tileRect, lerpColor(0x80000088, 0xFF000088, timePhase(data->time*2)));

						Circle range = makeCircle(getCenter(tileRect), getRange(event->ghostActorType, tilePos));
						drawCircle(range, 0x80FF0000);
					} else {
						float height = getTile3dHeight(tilePos);
						AABB aabb = getTileAABB(worldToTile(event->ghostOrMortarPosition));
						aabb.min.z += height;
						aabb.max.z += height;
						int color = lerpColor(0xFF808080, 0xFF000080, timePhase(platform->time*2));
						passMesh(cubeMesh, toMatrix(aabb), color);

						Vec3 position = getCenter(getTileAABB(tilePos));
						float radius = getRange(event->ghostActorType, tilePos) * SCALE_3D;
						draw3dRing(position, radius, 0xFFFF0000, 24);
					}
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
			float scale = 1;
			Vec3 accel = v3();
			Vec3 damping = v3(0.05, 0.05, 0.05);
			int color = particle->tint;

			if (particle->type == PARTICLE_FLAME) {
				fadeInPerc = 0.02;
				fadeOutPerc = 0.70;
				scale = 0.3;
				damping *= 0.1;
			}

			float particleTimeScale = elapsed / (1/60.0);

			particle->velo += (accel - damping*particle->velo) * particleTimeScale;

			particle->position += particle->velo * particleTimeScale;

			float alpha =
				clampMap(particle->time, 0, particle->maxTime*fadeInPerc, 0, 1)
				* clampMap(particle->time, particle->maxTime*(1-fadeOutPerc), particle->maxTime, 1, 0);

			color = setAofArgb(color, getAofArgb(color)*alpha);

			if (game->is2d) {
			} else {
				Matrix4 matrix = mat4();
				matrix.TRANSLATE(particle->position);
				matrix.SCALE(scale);
				passMesh(cubeMesh, matrix, color);
			}
			// DrawBillboardCall billboard = {};
			// billboard.texture = renderer->circleTexture1024;
			// billboard.position = particle->position;
			// billboard.size = particle->size;
			// billboard.tint = particle->tint;
			// billboard.alpha = alpha;
			// pushBillboard(billboard);

			if (complete) {
				game->particles[i] = game->particles[game->particlesNum-1];
				game->particlesNum--;
				i--;
				continue;
			}
		}
	} ///

	{ /// Show explore buttons
		if (data->phase == PHASE_PLANNING && data->tool == TOOL_NONE && data->hp > 0) {
			for (int i = 0; i < data->chunksNum; i++) {
				Chunk *chunk = &data->chunks[i];
				if (!chunk->visible) continue;

				for (int i = 0; i < chunk->connectionsNum; i++) {
					Chunk *newChunk = getChunkAt(chunk->connections[i]);
					Assert(newChunk);

					if (newChunk->visible) continue;

					bool didExplore = false;
					if (game->is2d) {
						Vec2 middle = (getCenter(chunk->rect) + getCenter(newChunk->rect)) / 2;
						Rect exploreRect = makeCenteredRect(middle, v2(300, 128));

						if (contains(exploreRect, game->mouse)) {
							exploreRect = inflatePerc(exploreRect, 0.1);
							if (isMouseClicked()) didExplore = true;
						}

						drawRectOutline(exploreRect, 4, 0xFFCCCCCC);

						DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
						drawTextInRect("Explore here", props, inflatePerc(exploreRect, -0.1));
					} else {
						AABB aabb = {};
						aabb.min = to3d(getPosition(newChunk->rect));
						aabb.max = to3d(getPosition(newChunk->rect) + getSize(newChunk->rect));
						aabb.max.z = TILE_SIZE*SCALE_3D;

						float temp = aabb.min.y;
						aabb.min.y = aabb.max.y;
						aabb.max.y = temp;

						passMesh(cubeMesh, toMatrix(aabb), lerpColor(0xFFFF0000, 0xFFFF8080, timePhase(data->time)));

						Line3 line = makeLine3(game->mouseRayPos, game->mouseRayPos + game->mouseRayDir*100.0);
						if (isMouseClicked() && overlaps(aabb, line)) didExplore = true;
					}

					if (didExplore) {
						if (data->hp > 0 && data->wave != 1) {
							saveState("assets/states/autosave.save_state");
							copyFile("assets/states/autosave.save_state", "assets/states/prevAutosave.save_state");
						}
						newChunk->visible = true;
						startNextWave();
						break;
					}
				}

				bool isPortal = false; //@copyPastedIsPortal
				if (chunk->connectionsNum == 1 && !isZero(chunk->position)) isPortal = true;
				if (isPortal) {
					if (game->is2d) {
						Rect portalTileRect = tileToWorldRect(chunkTileToWorldTile(chunk, v2i(CHUNK_SIZE/2, CHUNK_SIZE/2)));
						drawRect(portalTileRect, 0x80FF0000);
					}
				}
			}
		}
	} ///

	{ /// Update sun
		Vec3 sunPosition = game->sunPosition;
		if (data->phase == PHASE_WAVE) {
			float wavePerc = 1 - ((float)(data->actorsToSpawnNum + core->enemiesAlive) / (float)data->startingActorsToSpawnNum);
			sunPosition.x = clampMap(wavePerc, 0, 1, -1000, 1000);
			sunPosition.y = 0;
			sunPosition.z = 200;
		} else {
			sunPosition.x = 1000;
			sunPosition.y = 0;
			sunPosition.z = 30;
		}

		if (isZero(game->sunPosition)) game->sunPosition = sunPosition;
		game->sunPosition = lerp(game->sunPosition, sunPosition, 0.01);
	} ///

	{ /// Update map editor tool
		if (data->tool == TOOL_TILE_SELECTION) {
			Vec2i tilePos = getTileHovering();
			Tile *tile = getTileAt(tilePos);
			if (tile) {
				if (game->is2d) {
				} else {
					Matrix4 matrix = toMatrix(getTileAABB(tilePos));
					passMesh(cubeMesh, matrix, lerpColor(0x00FF0000, 0xFFFF0000, timePhase(platform->time)));
				}

				if (isMouseClicked()) {
					game->selectedTile = tile;
					game->selectedTilePosition = tilePos;
				}
			}

			if (game->selectedTile) {
				if (game->is2d) {
				} else {
					Matrix4 matrix = toMatrix(getTileAABB(game->selectedTilePosition));
					passMesh(cubeMesh, matrix, lerpColor(0x00FF0000, 0xFFFFFF00, timePhase(platform->time*3)));
				}
			}

			if (platform->rightMouseDown) data->tool = TOOL_NONE;
		}
	} ///

	{ /// Render pass
		if (game->is2d) {
			popCamera2d();
		} else {
			popPass();

			// {
			// 	pushTargetTexture(game->sunTexture);
			// 	clearRenderer();

			// 	Camera oldCamera = mainPass->camera;
			// 	mainPass->camera.position = game->sunPosition;
			// 	mainPass->camera.target = v3();

			// 	start3d(mainPass->camera);
			// 	setBlending(true);
			// 	setBackfaceCulling(true);
			// 	if (game->debugDisableBackfaceCulling) setBackfaceCulling(false);

			// 	for (int i = 0; i < mainPass->cmdsNum; i++) {
			// 		PassCmd *cmd = &mainPass->cmds[i];
			// 		if (cmd->type == PASS_CMD_MESH) {
			// 			Material material = createMaterial();
			// 			material.values[Raylib::MATERIAL_MAP_DIFFUSE].color = hexToArgbFloat(cmd->meshTint);
			// 			drawMesh(cmd->mesh, cmd->meshMatrix, NULL, material);
			// 		}
			// 	}

			// 	end3d();

			// 	mainPass->camera = oldCamera;

			// 	popTargetTexture();
			// }

			pushTargetTexture(game->gameTexture);
			clearRenderer();

			Pass *passes[] = {
				mainPass,
				screenPass,
			};
			int passesNum = ArrayLength(passes);
			if (!game->isOnLastStep) passesNum = 0;

			for (int i = 0; i < passesNum; i++) {
				Pass *pass = passes[i];

				start3d(pass->camera);
				setBlending(true);
				setBackfaceCulling(true);
				if (game->debugDisableBackfaceCulling) setBackfaceCulling(false);

				renderer->lights[0].position.x = game->sunPosition.x;
				renderer->lights[0].position.y = game->sunPosition.y;
				renderer->lights[0].position.z = game->sunPosition.z;
				updateLightingShader(pass->camera);

				for (int i = 0; i < pass->cmdsNum; i++) {
					PassCmd *cmd = &pass->cmds[i];
					if (cmd->type == PASS_CMD_TRI || cmd->type == PASS_CMD_QUAD) {
						int vertCount = 0;
						if (cmd->type == PASS_CMD_TRI) {
							vertCount = 3;
							Raylib::rlBegin(RL_TRIANGLES);
						} else if (cmd->type == PASS_CMD_QUAD) {
							vertCount = 4;
							Raylib::rlBegin(RL_QUADS);
						}

						Raylib::rlCheckRenderBatchLimit(vertCount);
						if (cmd->texture) Raylib::rlSetTexture(cmd->texture->raylibTexture.id);

						for (int i = 0; i < vertCount; i++) {
							int a, r, g, b;
							hexToArgb(cmd->colors[i], &a, &r, &g, &b);

							Raylib::rlColor4ub(r, g, b, a);
							Raylib::rlTexCoord2f(cmd->uvs[i].x, cmd->uvs[i].y);
							Raylib::rlVertex3f(cmd->verts[i].x, cmd->verts[i].y, cmd->verts[i].z);
						}

						if (cmd->texture) Raylib::rlSetTexture(0);
						Raylib::rlEnd();
					} else if (cmd->type == PASS_CMD_MESH) {
#if 1
						Material material = createMaterial();
						material.values[Raylib::MATERIAL_MAP_DIFFUSE].color = hexToArgbFloat(cmd->meshTint);
						drawMesh(cmd->mesh, cmd->meshMatrix, NULL, material);
#else
						Mesh *mesh = cmd->mesh;
						Raylib::rlCheckRenderBatchLimit(mesh->indsNum);

						Raylib::rlBegin(RL_TRIANGLES);

						int a, r, g, b;
						hexToArgb(cmd->meshTint, &a, &r, &g, &b);
						Raylib::rlColor4ub(r, g, b, a);

						for (int i = 0; i < mesh->indsNum; i++) {
							MeshVertex *meshVert = &mesh->verts[mesh->inds[i]];
							Vec3 position = cmd->meshMatrix * meshVert->position;
							Raylib::rlTexCoord2f(meshVert->uv.x, meshVert->uv.y);
							Raylib::rlNormal3f(meshVert->normal.x, meshVert->normal.y, meshVert->normal.z);
							Raylib::rlVertex3f(position.x, position.y, position.z);
						}
						Raylib::rlEnd();
#endif
					}
				}

				end3d();
			}

			popTargetTexture();

			{
				Vec2 fxaaSize = game->size;
				setShaderUniform(game->fxaaShader, game->fxaaResolutionLoc, &fxaaSize.x, SHADER_UNIFORM_VEC2, 1);

				pushTargetTexture(game->finalTexture);
				clearRenderer();
				startShader(game->fxaaShader);
				RenderProps props = newRenderProps();
				drawTexture(game->gameTexture, props);
				endShader();
				popTargetTexture();
			}

			{
				pushTargetTexture(game->gameTexture);
				clearRenderer();
				startShader(game->postShader);
				RenderProps props = newRenderProps();
				drawTexture(game->finalTexture, props);
				endShader();
				popTargetTexture();
			}

			{
				RenderProps props = newRenderProps();
				drawTexture(game->gameTexture, props); //@incomplete Make real ping pong switcher
			}

			// {
			// 	RenderProps props = newRenderProps();
			// 	props.matrix.SCALE(0.25);
			// 	drawTexture(game->sunTexture, props);
			// }

			destroyPass(screenPass);
			destroyPass(mainPass);
		}
	} ///
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

char *getUpgradeDescription(Upgrade *upgrade) {
	char *desc = "";
	for (int i = 0; i < upgrade->effectsNum; i++) {
		UpgradeEffect *effect = &upgrade->effects[i];
		ActorTypeInfo *info = &core->actorTypeInfos[effect->actorType];

		char *line = "";
		if (effect->type == UPGRADE_EFFECT_UNLOCK_BALLISTA) {
			line = "Unlock Ballista";
		} else if (effect->type == UPGRADE_EFFECT_UNLOCK_MORTAR_TOWER) {
			line = "Unlock Mortar Tower";
		} else if (effect->type == UPGRADE_EFFECT_UNLOCK_TESLA_COIL) {
			line = "Unlock Tesla Coil";
		} else if (effect->type == UPGRADE_EFFECT_UNLOCK_FLAME_THROWER) {
			line = "Unlock Flame Thrower";
		} else if (effect->type == UPGRADE_EFFECT_UNLOCK_POISON_SPRAYER) {
			line = "Unlock Poison Sprayer";
		} else if (effect->type == UPGRADE_EFFECT_UNLOCK_SHREDDER) {
			line = "Unlock Shredder";
		} else if (effect->type == UPGRADE_EFFECT_UNLOCK_MANA_SIPHON) {
			line = "Unlock Mana Siphon";
		} else if (effect->type == UPGRADE_EFFECT_UNLOCK_FROST_KEEP) {
			line = "Unlock Frost Keep";
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
		} else if (effect->type == UPGRADE_EFFECT_EXTRA_TIME_SCALE) {
			line = frameSprintf("Allows timeScale to be set to %g", effect->value);
		} else if (effect->type == UPGRADE_EFFECT_RELOAD) {
			line = "Allows you to reload and try the wave again if you lose";
		} else if (effect->type == UPGRADE_EFFECT_EXTRA_SAW_PIERCE) {
			line = frameSprintf("Saws pierce through %g more enemies", effect->value);
		} else if (effect->type == UPGRADE_EFFECT_MIN_TOWER_LEVEL_PERC) {
			line = frameSprintf("All tower start at a minimum of %g%% leveled up", effect->value*100.0);
		} else if (effect->type == UPGRADE_EFFECT_BULLET_SPEED_MULTI) {
			line = frameSprintf("%s bullet speed %g%% faster", info->name, effect->value*100.0);
		} else if (effect->type == UPGRADE_EFFECT_MORE_POISON_TICKS) {
			line = frameSprintf("%g more ticks of poison damage", effect->value);
		} else if (effect->type == UPGRADE_EFFECT_MORE_BURN_TICKS) {
			line = frameSprintf("%g more ticks of burn damage", effect->value);
		} else if (effect->type == UPGRADE_EFFECT_MORE_BLEED_TICKS) {
			line = frameSprintf("%g more ticks of bleed damage", effect->value);
		} else if (effect->type == UPGRADE_EFFECT_GAIN_MONEY) {
			line = frameSprintf("Gain %g money", effect->value);
		} else {
			line = frameSprintf("Unlabeled effect %d", effect->type);
		}
		if (i != 0) desc = frameSprintf("%s\n", desc);
		desc = frameSprintf("%s%s", desc, line);
	}
	return desc;
}

float getTile3dHeight(Vec2i tilePos) {
	float height = TILE_SIZE * SCALE_3D * 0.25;

	Tile *tile = getTileAt(tilePos);
	if (tile) {
		if (tile->type == TILE_ROAD) height *= 0.1;
		height += tile->elevation * TILE_SIZE*SCALE_3D * 0.3;
	}
	return height;
}

AABB getTileAABB(Vec2i tilePos) {
	AABB aabb = {};
	aabb.min = v3(tilePos.x, tilePos.y, 0) * TILE_SIZE*SCALE_3D;
	aabb.max = aabb.min + TILE_SIZE*SCALE_3D;
	aabb.max.z = aabb.min.z + getTile3dHeight(tilePos);
	aabb.min.y *= -1;
	aabb.max.y *= -1;

	if (aabb.min.y > aabb.max.y) {
		float temp = aabb.min.y;
		aabb.min.y = aabb.max.y;
		aabb.max.y = temp;
	}
	return aabb;
}

AABB getAABB(Actor *actor) {
	Vec3 position = v3(actor->position.x, actor->position.y, getInfo(actor)->size.z/2);
	position *= SCALE_3D;
	position.y *= -1;

	Vec3 scale = getInfo(actor)->size*SCALE_3D;
	AABB aabb = makeCenteredAABB(position, scale);

	float tileHeight = getTile3dHeight(worldToTile(actor->position));
	aabb.min.z += tileHeight;
	aabb.max.z += tileHeight;
	return aabb;
}

Vec3 to3d(Vec2 value) {
	return v3(value, 0) * v3(1, -1, 1) * SCALE_3D;
}
float to3d(float value) {
	return value * SCALE_3D;
}
Vec3 radsTo3dDir(float rads) {
	Vec2 aimVec2 = radToVec2(rads);
	Vec3 dir = v3(aimVec2.x, -aimVec2.y, 0);
	return dir;
}

Vec2 to2d(Vec3 value) {
	return v2(value.x, -value.y) / SCALE_3D;
}
Vec2 toScreenPass(Vec3 worldPosition) {
	Vec2 screenPos = worldSpaceTo2dNDC01(game->lastMainPassCamera, worldPosition) * game->size;
	screenPos.y = game->size.y - screenPos.y;
	return screenPos;
}
Matrix3 toMatrix3(Rect rect) {
	Matrix3 matrix = mat3();
	matrix.TRANSLATE(getPosition(rect));
	matrix.SCALE(getSize(rect));
	return matrix;
}

Matrix4 toMatrix(AABB aabb) {
	if (aabb.min.y > aabb.max.y) {
		aabb.print("Need to flip aabb");
		float temp = aabb.min.y;
		aabb.min.y = aabb.max.y;
		aabb.max.y = temp;

		aabb = inflate(aabb, timePhase(data->time));
	}
	Matrix4 matrix = mat4();
	matrix.TRANSLATE(getCenter(aabb));
	matrix.SCALE(getSize(aabb));
	return matrix;
}

void draw3dRing(Vec3 center, float radius, int color, int points, float thickness) {
	Mesh *cubeMesh = getMesh("assets/common/models/Cube.Cube.mesh");

	for (int i = 0; i < points; i++) {
		float perc = i / (float)points;
		float rads = 2*M_PI * perc;
		rads += platform->time*0.2;
		Vec3 pos = v3();
		pos.x = center.x + cos(rads)*radius;
		pos.y = center.y + sin(rads)*radius;
		Vec2i tilePos = worldToTile(to2d(pos));

		pos.z = getTile3dHeight(tilePos);
		AABB aabb = makeCenteredAABB(pos, v3(thickness, thickness, thickness));
		passMesh(cubeMesh, toMatrix(aabb), color);
	}
}

Particle *createParticle(ParticleType type) {
	if (game->particlesNum > game->particlesMax-1) {
		game->particles = (Particle *)resizeArray(game->particles, sizeof(Particle), game->particlesNum, game->particlesMax*2 + 1);
		game->particlesMax = game->particlesMax*2 + 1;
	}

	Particle *particle = &game->particles[game->particlesNum++];
	memset(particle, 0, sizeof(Particle));
	particle->type = type;
	particle->tint = 0xFFFFFFFF;
	particle->size = v2(32, 32);
	return particle;
}


void updateAndDrawOverlay(float elapsed) {
	Globals *globals = &game->globals;

	{ /// Editor
		if (keyJustPressed('R')) game->is2d = !game->is2d;

		if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
		if (game->inEditor) {
			ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::Button("Tile selection tool")) data->tool = TOOL_TILE_SELECTION;

			if (ImGui::TreeNode("Globals")) {
				if (ImGui::Button("Save")) saveGlobals();
				ImGui::SameLine();
				if (ImGui::Button("Load")) loadGlobals();

				ImGui::InputFloat("cameraBaseDistance", &globals->cameraBaseDistance);
				ImGui::SliderFloat3("cameraAngle", &globals->cameraAngle.x, -M_PI/2, M_PI/2);

				if (ImGui::TreeNode("Ngui style")) {
					for (int i = 0; i < globals->customStyleStacksNum; i++) {
						ImGui::PushID(i);

						bool shouldSpliceStack = false;
						guiPushStyleColor(ImGuiCol_Button, 0xFF900000);
						if (ImGui::Button("X")) shouldSpliceStack = true;
						guiPopStyleColor();

						ImGui::SameLine();
						if (ImGui::ArrowButton("moveUp", ImGuiDir_Up)) {
							if (i > 0) {
								arraySwap(globals->customStyleStacks, globals->customStyleStacksNum, sizeof(NguiNamedStyleStack), i, i-1);
								ImGui::PopID();
								continue;
							}
						}
						ImGui::SameLine();
						if (ImGui::ArrowButton("moveDown", ImGuiDir_Down)) {
							if (i < globals->customStyleStacksNum-1) {
								arraySwap(globals->customStyleStacks, globals->customStyleStacksNum, sizeof(NguiNamedStyleStack), i, i+1);
								ImGui::PopID();
								continue;
							}
						}

						ImGui::SameLine();
						NguiNamedStyleStack *namedStyle = &globals->customStyleStacks[i];
						if (ImGui::TreeNode(frameSprintf("%d: %s###styleNode%d", i, namedStyle->name, i))) {
							ImGui::InputText("Name", namedStyle->name, NGUI_NAMED_STYLE_STACK_NAME_MAX_LEN);
							nguiShowImGuiStyleEditor(&namedStyle->style);
							ImGui::TreePop();
						}

						ImGui::PopID();

						if (shouldSpliceStack) {
							arraySpliceIndex(globals->customStyleStacks, globals->customStyleStacksNum, sizeof(NguiNamedStyleStack), i);
							globals->customStyleStacksNum--;
							i--;
						}
					}

					if (ImGui::Button("Create custom style")) {
						if (globals->customStyleStacksNum > CUSTOM_STYLE_STACKS_MAX-1) {
							logf("Too many custom style stacks!\n");
						} else {
							NguiNamedStyleStack *namedStyle = &globals->customStyleStacks[globals->customStyleStacksNum++];
							memset(namedStyle, 0, sizeof(NguiNamedStyleStack));
						}
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Stats")) {
				static int statWave = -1;
				ImGui::InputInt("statWave", &statWave);
				if (statWave < 0) statWave = 0;
				if (statWave > data->wave) statWave = data->wave;

				if (ImGui::BeginTable("Stats table", 7, ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_BordersH|ImGuiTableFlags_BordersV)) {
					ImGui::TableSetupColumn("name");
					ImGui::TableSetupColumn("investment");
					ImGui::TableSetupColumn("shots");
					ImGui::TableSetupColumn("shieldDamage");
					ImGui::TableSetupColumn("armorDamage");
					ImGui::TableSetupColumn("hpDamage");
					ImGui::TableSetupColumn("damagePerDollar");
					ImGui::TableHeadersRow();

					for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
						ActorTypeInfo *info = &core->actorTypeInfos[i];
						if (stringStartsWith(info->name, "Actor")) continue;

						Stats *stats = &data->actorTypeStats[i][statWave];

						ImGui::PushID(i);

						ImGui::TableNextColumn();
						ImGui::Text("%s", info->name);

						ImGui::TableNextColumn();
						ImGui::Text("$%d", stats->investment);

						ImGui::TableNextColumn();
						ImGui::Text("%d", stats->shots);

						ImGui::TableNextColumn();
						ImGui::Text("%.1f", stats->shieldDamage);

						ImGui::TableNextColumn();
						ImGui::Text("%.1f", stats->armorDamage);

						ImGui::TableNextColumn();
						ImGui::Text("%.1f", stats->hpDamage);

						ImGui::TableNextColumn();
						float damagePerDollar = (stats->hpDamage + stats->armorDamage + stats->shieldDamage) / (float)stats->investment;
						ImGui::Text("%.1f", damagePerDollar);

						ImGui::PopID();
					}

					if (data->selectedActorsNum > 0) {
						Actor *actor = getActor(data->selectedActors[0]);
						Stats *stats = &actor->stats[statWave];
						ImGui::TableNextColumn();
						ImGui::Text("Selected");

						ImGui::TableNextColumn();
						ImGui::Text("%d", stats->shots);

						ImGui::TableNextColumn();
						ImGui::Text("%.1f", stats->shieldDamage);

						ImGui::TableNextColumn();
						ImGui::Text("%.1f", stats->armorDamage);

						ImGui::TableNextColumn();
						ImGui::Text("%.1f", stats->hpDamage);
					}

					ImGui::EndTable();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Upgrades")) {
				for (int i = 0; i < core->upgradesNum; i++) {
					ImGui::PushID(i);

					Upgrade *upgrade = &core->upgrades[i];
					if (hasUpgrade(upgrade->id)) {
						if (ImGui::Button("Lock")) {
							for (int i = 0; i < data->ownedUpgradesNum; i++) {
								if (data->ownedUpgrades[i] == upgrade->id) {
									arraySpliceIndex(data->ownedUpgrades, data->ownedUpgradesNum, sizeof(int), i);
									data->ownedUpgradesNum--;
									break;
								}
							}
						}
					} else {
						if (hasPrereqs(upgrade->id)) {
							if (ImGui::Button("Unlock")) unlockUpgrade(upgrade);
						} else {
							ImGui::Text("[Missing prereqs]");
						}
					}
					ImGui::SameLine();

					ImGui::Text("%s", getUpgradeDescription(upgrade));
					ImGui::Separator();

					ImGui::PopID();
				}

				ImGui::Text("Total: %d\n", core->upgradesNum);
				ImGui::TreePop();
			}

			ImGui::Checkbox("Show Dijkstra values", &game->debugShowDijkstraValues);
			ImGui::Checkbox("Show Flow Field values", &game->debugShowFlowFieldValues);
			ImGui::Checkbox("Show perlin values", &game->debugShowPerlinValues);
			ImGui::Checkbox("Draw chunk lines", &game->debugDrawChunkLines);
			ImGui::Checkbox("Draw tile lines", &game->debugDrawTileLines);
			ImGui::Checkbox("Show actor velo", &game->debugShowActorVelo);
			ImGui::Checkbox("Is 2d", &game->is2d);
			ImGui::Checkbox("Debug disable backface culling", &game->debugDisableBackfaceCulling);
			ImGui::DragFloat3("sunPosition", &game->sunPosition.x);
			if (ImGui::Button("Reset game")) game->shouldReset = true;
			ImGui::SameLine();
			if (ImGui::Button("Explore all")) {
				for (int i = 0; i < data->chunksNum; i++) {
					Chunk *chunk = &data->chunks[i];
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

					Chunk *chunk = &data->chunks[rndInt(0, data->chunksNum-1)];
					if (!chunk->visible) continue;
					Vec2i chunkTilePos;
					chunkTilePos.x = rndInt(0, CHUNK_SIZE-1);
					chunkTilePos.y = rndInt(0, CHUNK_SIZE-1);
					Tile *tile = &chunk->tiles[chunkTilePos.y * CHUNK_SIZE + chunkTilePos.x];
					Vec2i tilePos = chunkTileToWorldTile(chunk, chunkTilePos);

					bool canBuild = true;
					if (tile->type != TILE_GROUND) canBuild = false;

					for (int i = 0; i < data->actorsNum; i++) {
						Actor *other = &data->actors[i];
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

			if (ImGui::Button("Kill everything")) {
				data->actorsToSpawnNum = 0;

				for (int i = 0; i < data->actorsNum; i++) {
					Actor *actor = &data->actors[i];
					if (getInfo(actor)->isEnemy) actor->hp = -10;
				}
			}
			ImGui::Separator();

			if (ImGui::Button("Resave all states")) {
				int filesNum = 0;
				char **files = getFrameDirectoryList("assets/states", &filesNum);
				for (int i = 0; i < filesNum; i++) {
					loadState(files[i]);
					saveState(files[i]);
				}
				loadState("assets/states/autosave.save_state");
			}
			ImGui::Separator();

			static bool loadAtLowTimeScale = false;
			ImGui::Checkbox("loadAtLowTimeScale", &loadAtLowTimeScale);

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
						if (loadAtLowTimeScale) game->timeScale = 0.001;
					}
					if ((pathCount+1) % 5 != 0) ImGui::SameLine();
					pathCount++;
				}
			}

			ImGui::NewLine();
			ImGui::Separator();
			ImGui::InputText("Campaign name", data->campaignName, CAMPAIGN_NAME_MAX_LEN);

			ImGui::End();

			if (game->selectedTile) {
				Tile *tile = game->selectedTile;
				ImGui::SetNextWindowPos(ImVec2(game->size.x, 0), ImGuiCond_Always, ImVec2(1, 0));
				ImGui::Begin("TileProps", NULL, ImGuiWindowFlags_AlwaysAutoResize);

				Chunk *selectedChunk = worldToChunk(tileToWorld(game->selectedTilePosition));
				if (ImGui::Button("Delete this chunk")) {
					removeChunk(selectedChunk);
					game->selectedTile = NULL;
				}

				Vec2i newChunkPos = v2i();
				if (ImGui::Button("New chunk ->")) newChunkPos = selectedChunk->position + v2i(1, 0);
				if (ImGui::Button("New chunk <-")) newChunkPos = selectedChunk->position + v2i(-1, 0);
				if (ImGui::Button("New chunk ^")) newChunkPos = selectedChunk->position + v2i(0, -1);
				if (ImGui::Button("New chunk v")) newChunkPos = selectedChunk->position + v2i(0, 1);

				if (!isZero(newChunkPos)) {
					if (getChunkAt(newChunkPos)) {
						logf("There's already a chunk there\n");
					} else {
						Chunk *chunk = createChunk(newChunkPos);
						chunk->visible = true;
						connect(selectedChunk, chunk);
					}
				}

				if (ImGui::Button("Recarve chunk connections")) carveChunkConnections(selectedChunk);

				ImGui::Separator();

				ImGui::Text("Elevation:");
				ImGui::SameLine();
				if (ImGui::Button("-###DecElevation") && tile->elevation > 0) tile->elevation--;
				ImGui::SameLine();
				ImGui::Text("%d", tile->elevation);
				ImGui::SameLine();
				if (ImGui::Button("+###IncElevation") && tile->elevation < 3) tile->elevation++;
				ImGui::SameLine();

				ImGui::End();
			}
		}
	} ///

	{ /// Get input
		Vec2 moveDir = v2();
		if (keyPressed('W')) moveDir.y--;
		if (keyPressed('S')) moveDir.y++;
		if (keyPressed('A')) moveDir.x--;
		if (keyPressed('D')) moveDir.x++;

		float oldCameraZoom = data->cameraZoom;
		data->cameraZoom += platform->mouseWheel * 0.1;
		data->cameraZoom = mathClamp(data->cameraZoom, 0.1, 3);
		if (oldCameraZoom != data->cameraZoom) logf("Zoom: %g\n", data->cameraZoom);

		data->cameraPosition += normalize(moveDir) * 20 / data->cameraZoom;

		for (int i = 1; i <= 9; i++) {
			if (keyPressed(KEY_ALT) && keyJustReleased('0' + i)) {
				loadState(frameSprintf("assets/states/%d.save_state", i));
			}
		}

		{
			float *timeScaleValues = (float *)frameMalloc(sizeof(float) * 64);
			int timeScaleValuesNum = 0;

			timeScaleValues[timeScaleValuesNum++] = 0.25;
			timeScaleValues[timeScaleValuesNum++] = 0.5;
			timeScaleValues[timeScaleValuesNum++] = 1;
			timeScaleValues[timeScaleValuesNum++] = 2;
			timeScaleValues[timeScaleValuesNum++] = 4;
			timeScaleValues[timeScaleValuesNum++] = 8;
			timeScaleValues[timeScaleValuesNum++] = 16;

			StartForEachUpgradeEffect;
			if (effect->type == UPGRADE_EFFECT_EXTRA_TIME_SCALE) timeScaleValues[timeScaleValuesNum++] = effect->value;
			EndForEachUpgradeEffect;

			auto qsortFloats = [](const void *a, const void *b)->int {
				float fa = *(const float*) a;
				float fb = *(const float*) b;
				return (fa > fb) - (fa < fb);
			};

			qsort(timeScaleValues, timeScaleValuesNum, sizeof(float), qsortFloats);

			int defaultTimeScaleIndex = 0;
			int timeScaleIndex = 0;
			for (int i = 0; i < timeScaleValuesNum; i++) {
				if (timeScaleValues[i] == game->timeScale) timeScaleIndex = i;
				if (timeScaleValues[i] == 1) defaultTimeScaleIndex = i;
			}

			if (keyJustPressed('-')) timeScaleIndex--;
			if (keyJustPressed('=')) timeScaleIndex++;
			if (keyJustPressed('0')) timeScaleIndex = defaultTimeScaleIndex;
			if (timeScaleIndex < 0) timeScaleIndex = 0;
			if (timeScaleIndex > timeScaleValuesNum-1) timeScaleIndex = timeScaleValuesNum-1;

			if (game->timeScale != timeScaleValues[timeScaleIndex]) {
				game->timeScale = timeScaleValues[timeScaleIndex];
				logf("Time scale: %.3f\n", game->timeScale);
			}
		}
	} ///

	{ /// Update tool related ui
		pushGameStyleStack("Base");

		if (data->tool == TOOL_NONE) {
			{ /// Tools window
				nguiStartWindow("Tools window", game->size*v2(0.5, 1), v2(0.5, 1));
				nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 4);

				ActorType *typesCanBuy = (ActorType *)frameMalloc(sizeof(ActorType) * ACTOR_TYPES_MAX);
				int typesCanBuyNum = 0;
				if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK_BALLISTA)) typesCanBuy[typesCanBuyNum++] = ACTOR_BALLISTA;
				if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK_MORTAR_TOWER)) typesCanBuy[typesCanBuyNum++] = ACTOR_MORTAR_TOWER;
				if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK_TESLA_COIL)) typesCanBuy[typesCanBuyNum++] = ACTOR_TESLA_COIL;
				if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK_FROST_KEEP)) typesCanBuy[typesCanBuyNum++] = ACTOR_FROST_KEEP;
				if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK_FLAME_THROWER)) typesCanBuy[typesCanBuyNum++] = ACTOR_FLAME_THROWER;
				if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK_POISON_SPRAYER)) typesCanBuy[typesCanBuyNum++] = ACTOR_POISON_SPRAYER;
				if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK_SHREDDER)) typesCanBuy[typesCanBuyNum++] = ACTOR_SHREDDER;
				if (hasUpgradeEffect(UPGRADE_EFFECT_UNLOCK_MANA_SIPHON)) typesCanBuy[typesCanBuyNum++] = ACTOR_MANA_SIPHON;

				for (int i = 0; i < typesCanBuyNum; i++) {
					ActorType actorType = typesCanBuy[i];
					ActorTypeInfo *info = &core->actorTypeInfos[actorType];
					float price = info->price + info->priceMulti*core->actorTypeCounts[actorType];
					char *label = frameSprintf("%s $%.0f\n", info->name, price);
					if (nguiButton(label)) {
						data->tool = TOOL_BUILDING;
						data->actorToBuild = actorType;
					}
				}

				nguiPopStyleVar(NGUI_STYLE_ELEMENTS_IN_ROW);
				nguiEndWindow();
			} ///

			{ /// Upgrades window
				Vec2 nextWindowPosition = game->size*v2(0, 1);
				if (game->uiUpgradeListOpened) {
					pushGameStyleStack("Upgrade List");
					nguiStartWindow("Upgrade List Window", nextWindowPosition, v2(0, 1));
					for (int i = 0; i < data->ownedUpgradesNum; i++) {
						Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]);
						char *str = frameSprintf("%d: %s", i, getUpgradeDescription(upgrade));
						nguiButton(str);
					}
					nguiEndWindow();
					popGameStyleStack("Upgrade List");

					nextWindowPosition = getPosition(ngui->lastWindowRect);
				}

				pushGameStyleStack("Upgrade Expander");
				nguiStartWindow("Upgrade List Epander", nextWindowPosition, v2(0, 1));
				if (nguiButton("Upgrades")) game->uiUpgradeListOpened = !game->uiUpgradeListOpened;
				nguiEndWindow();
				popGameStyleStack("Upgrade Expander");
			} ///

			{ /// Waves window
				Vec2 nextWindowPosition = getPosition(ngui->lastWindowRect);
				if (game->uiWavesListOpened) {
					pushGameStyleStack("Waves List");
					nguiStartWindow("Wave List Window", nextWindowPosition, v2(0, 1));

					ActorType *actorsToSpawn = NULL;
					int actorsToSpawnNum = 0;

					if (data->phase == PHASE_PLANNING) { 
						actorsToSpawn = generateWave(data->wave + 1, &actorsToSpawnNum);
					} else {
						actorsToSpawn = data->actorsToSpawn;
						actorsToSpawnNum = data->actorsToSpawnNum;
					}

					for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
						ActorType actorType = (ActorType)i;
						int count = 0;
						for (int i = 0; i < actorsToSpawnNum; i++){
							if (actorsToSpawn[i] == actorType) count++;
						}
						if (count == 0) continue;

						ActorTypeInfo *info = &core->actorTypeInfos[actorType];
						char *label = frameSprintf("%s[", info->name);
						if (info->maxShield) label = frameSprintf("%sS:%g ", label, info->maxShield);
						if (info->maxArmor) label = frameSprintf("%sA:%g ", label, info->maxArmor);
						if (info->maxHp) label = frameSprintf("%sH:%g ", label, info->maxHp);
						if (label[strlen(label)-1] == ' ') label[strlen(label)-1] = 0;
						label = frameSprintf("%s] x%d", label, count);
						nguiButton(label);
					}
					nguiEndWindow();
					popGameStyleStack("Waves List");

					nextWindowPosition = getPosition(ngui->lastWindowRect);
				}

				pushGameStyleStack("Waves Expander");
				nguiStartWindow("Waves List Epander", nextWindowPosition, v2(0, 1));
				if (nguiButton("Waves")) game->uiWavesListOpened = !game->uiWavesListOpened;
				nguiEndWindow();
				popGameStyleStack("Waves Expander");
			} ///

			{ /// Toggles window
				pushGameStyleStack("Toggles");
				nguiStartWindow("Toggles Window", game->size, v2(1, 1));
				if (hasUpgradeEffect(UPGRADE_EFFECT_RELOAD, ACTOR_NONE)) {
					if (data->hp <= 0 && data->phase == PHASE_PLANNING) {
						if (nguiButton("Reload")) game->shouldLoadState = stringClone("assets/states/autosave.save_state");
					}
				}
				nguiEndWindow();
				popGameStyleStack("Toggles");
			} ///
		} else if (data->tool == TOOL_BUILDING) {
			// Nothing...
		} else if (data->tool == TOOL_SELECTED) {
			nguiStartWindow("Selected window", game->size*v2(0.5, 1), v2(0.5, 1));
			nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 4);

			if (nguiButton("Sell")) {
				for (int i = 0; i < data->selectedActorsNum; i++) {
					Actor *actor = getActor(data->selectedActors[i]);
					ActorTypeInfo *info = &core->actorTypeInfos[actor->type];

					actor->markedForDeletion = true;
					int toGain = 0;
					toGain += actor->amountPaid;
					toGain += info->price + ((core->actorTypeCounts[actor->type]-1) * info->priceMulti);
					data->money += toGain;
					addInvestmentStat(actor, -toGain);

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

	if (data->phase == PHASE_RESULTS) {
		pushGameStyleStack("Upgrades");
		nguiStartWindow("Upgrade window", v2(0, platform->windowHeight/2), v2(0, 0.5));
		if (core->presentedUpgradesNum > 0) {
			for (int i = 0; i < core->presentedUpgradesNum; i++) {
				Upgrade *upgrade = getUpgrade(core->presentedUpgrades[i]);
				char *label = getUpgradeDescription(upgrade);

				if (nguiButton(label)) {
					unlockUpgrade(upgrade);
					data->phase = PHASE_PLANNING;
				}
			}
		} else {
			if (nguiButton("Next...")) {
				data->phase = PHASE_PLANNING;
			}
		}
		nguiEndWindow();
		popGameStyleStack("Upgrades");
	}

	{
		char *demoStr = "";
		if (game->isDemo) {
			demoStr = "\nWASD: Camera\nMouse wheel: Zoom\nR: Switch rendering mode";
		}

		Rect rect = makeRect(v2(), game->size*v2(0.2, 0.3));
		DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
		drawTextInRect(frameSprintf(
			"Wave: %d\nHp: %d\nMoney $%d\nMana: %.1f/%.1f (+%.1f/s)%s",
			data->wave,
			data->hp,
			data->money,
			data->mana,
			data->maxMana,
			core->manaToGain/elapsed,
			demoStr
		), props, rect, v2());
	}

	popGameStyleStack("Base");
	nguiDraw(elapsed);

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) drawText(game->defaultFont, frameSprintf("%.1fms", platform->frameTimeAvg), v2(300, 0), 0xFF808080);

	drawOnScreenLog();
}

int playWorldSound(char *path, Vec3 worldPosition) {
	Sound *sound = getSound(resolveFuzzyPath(path));
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

void pushGameStyleStack(char *name) {
	Globals *globals = &game->globals;
	for (int i = 0; i < globals->customStyleStacksNum; i++) {
		NguiNamedStyleStack *namedStyle = &globals->customStyleStacks[i];
		if (streq(namedStyle->name, name)) {
			nguiPushStyleStack(&namedStyle->style);
			return;
		}
	}

	logf("Couldn't find style stack %s\n", name);
}

void popGameStyleStack(char *name) {
	Globals *globals = &game->globals;
	for (int i = 0; i < globals->customStyleStacksNum; i++) {
		NguiNamedStyleStack *namedStyle = &globals->customStyleStacks[i];
		if (streq(namedStyle->name, name)) {
			nguiPopStyleStack(&namedStyle->style);
			return;
		}
	}

	logf("Couldn't find style stack %s\n", name);
}


void saveGlobals() {
	Globals *globals = &game->globals;
	DataStream *stream = newDataStream();

	writeU32(stream, 3); // version

	writeVec3(stream, globals->cameraAngle);
	writeFloat(stream, globals->cameraBaseDistance);

	writeU32(stream, globals->customStyleStacksNum);
	for (int i = 0; i < globals->customStyleStacksNum; i++) {
		NguiNamedStyleStack *namedStyle = &globals->customStyleStacks[i];
		writeString(stream, namedStyle->name);
		writeNguiStyleStack(stream, namedStyle->style);
	}


	writeDataStream("assets/globals.bin", stream);
	destroyDataStream(stream);
}

void loadGlobals() {
	Globals *globals = &game->globals;
	DataStream *stream = loadDataStream("assets/globals.bin");
	if (!stream) return;

	int version = readU32(stream);

	if (version >= 2) globals->cameraAngle = readVec3(stream);
	if (version >= 3) globals->cameraBaseDistance = readFloat(stream);

	if (version >= 1) {
		globals->customStyleStacksNum = readU32(stream);
		for (int i = 0; i < globals->customStyleStacksNum; i++) {
			NguiNamedStyleStack *namedStyle = &globals->customStyleStacks[i];
			readStringInto(stream, namedStyle->name, NGUI_NAMED_STYLE_STACK_NAME_MAX_LEN);
			namedStyle->style = readNguiStyleStack(stream);
		}
	}

	destroyDataStream(stream);
}

//@consider Frost keep has a square range
//@consider Everything but the frost keep scale mana usage by base damage?
