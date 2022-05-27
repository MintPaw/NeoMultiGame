enum ActorType {
	ACTOR_NONE=0,
	ACTOR_PLAYER=1,
	ACTOR_DEBUG_WALL=2,
	ACTOR_BUG_DEN=3,
	ACTOR_BUG=4,
	ACTOR_TOWER=5,
	ACTOR_ITEM=6,
	ACTOR_FLAME_TOWER=7,
	ACTOR_END,
};
#define ACTOR_TYPES_MAX (ACTOR_END)
char *actorTypeStrings[] = {
	"None",
	"Player",
	"Debug wall",
	"Bug den",
	"Bug",
	"Tower",
	"Item",
	"Flame tower",
};
struct ActorTypeInfo;
struct Actor {
	int id;
	ActorType type;
	int variant;

	Vec2 position;
	float rotation;

	Vec2 velo;
	Vec2 accel;

	int hp;
	int energy;
	int tick;

	bool unkillable;
	float denTimeTillNextSpawn;

	bool isSpawner;
	bool hasSpawned;
	bool wasSpawned;
	int srcSpawner;

	int targetActor;
	int homeDen;
	int hitAggroById;

	int buildAmount;

	/// Unserialized
	int immTint;
	ActorTypeInfo *info;
	Texture *texture;
	Rect hitRect;

	bool markedForDeletion;
};

enum Team {
	TEAM_NEUTRAL,
	TEAM_ALLY,
	TEAM_ENEMY,
};

struct ActorTypeInfo {
	ActorType type;
	Vec2 size;
	Team team;

	bool staticCollide;
	bool dynamicCollide;

	float towerRange;

	int buildAmountNeeded;

	int maxEnergy;
	int maxHp;
	float mass;

	bool burns;
};

struct Scene {
#define ACTORS_MAX 2048
	Actor actors[ACTORS_MAX];
	int actorsNum;
	int nextActorId;
};

enum UiMode {
	UI_NORMAL,
	UI_BUILD,
};

enum EffectType {
	EFFECT_TOWER_SHOT,
};
struct Effect {
	EffectType type;
	Vec2 startPos;
	Vec2 endPos;
	float time;
};

struct GameData {
	int ticks;
	bool hasTowerRange;
	bool hasEnergyTank;
	bool hasAutoBuilder;
};

struct Game {
	// Texture *gameTexture;

	GameData gameData;
	ActorTypeInfo actorTypeInfos[ACTOR_TYPES_MAX];
	Scene scene;
	Pool *actorsAroundPool;

	Rect worldRect;
	QuadTree *actorsTree;

	UiMode uiMode;
	int selectedActor;
	ActorType typeToBuild;

	Vec2 cameraCenter;
	float zoom;

	bool autoBuildEnabled;

#define EFFECTS_MAX 256
	Effect effects[EFFECTS_MAX];
	int effectsNum;

	bool debugMode;
	bool showSpawners;

	bool debugFreeCamera;
	bool debugGodMode;
	bool debugKillAttackers;

	bool debugDoBigStep;
	int debugBigStepCount;
};

Game *game = NULL;

void runGame();
void updateGame();
void stepGame();
Actor *createActor(ActorType type);
Actor *getActor(int id);
Actor **getActorsAround(Vec2 position, float radius, ActorType type, int *outActorsNum);
int countActorsAround(Vec2 position, float radius, ActorType type);
Actor **getActorsAroundRect(Rect rect, ActorType type, int *outActorsNum);
Actor *getClosestEnemyActor(Vec2 position, float radius);
void dealDamage(Actor *actor, int damage, Actor *src=NULL);
Effect *createEffect(EffectType type);
void tryBuildTower(Actor *player, Actor *tower);
int getMaxEnergy(Actor *actor);
float getTowerRange(Actor *actor);
float getTowerRange(ActorType type);
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameA(hModule, exeDir, PATH_MAX_LEN);

	char *lastSlash = strrchr(exeDir, '\\');
	if (!lastSlash) Panic("No last slash found in exe path");
	*lastSlash = 0;

#if defined(FALLOW_INTERNAL) // This needs to be a macro
		// if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/???");
#else
		snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#endif

#endif

	initFileOperations();

	initPlatform(1280, 720, "Towers");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	// initMesh();
	// initModel();
	// initSkeleton();
	initFonts();
	initTextureSystem();
	// if (!initNetworking()) logf("Failed to init networking\n");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		RegMem(Actor, id);
		RegMem(Actor, type);
		RegMem(Actor, variant);
		RegMem(Actor, position);
		RegMem(Actor, rotation);
		RegMem(Actor, velo);
		RegMem(Actor, accel);
		RegMem(Actor, hp);
		RegMem(Actor, energy);
		RegMem(Actor, tick);
		RegMem(Actor, unkillable);
		RegMem(Actor, denTimeTillNextSpawn);
		RegMem(Actor, isSpawner);
		RegMem(Actor, hasSpawned);
		RegMem(Actor, wasSpawned);
		RegMem(Actor, srcSpawner);
		RegMem(Actor, targetActor);
		RegMem(Actor, homeDen);
		RegMem(Actor, hitAggroById);
		RegMem(Actor, buildAmount);

		RegMem(Scene, nextActorId);

		{ /// Set up ActorTypeInfo's
			for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
				ActorTypeInfo *info = &game->actorTypeInfos[i];
				info->type = (ActorType)i;
				info->mass = 1;
			}

			ActorTypeInfo *info = NULL;

			info = &game->actorTypeInfos[ACTOR_PLAYER];
			info->team = TEAM_ALLY;
			info->size = v2(32, 32);
			info->dynamicCollide = true;
			info->maxEnergy = 200;
			info->maxHp = 100;

			info = &game->actorTypeInfos[ACTOR_DEBUG_WALL];
			info->size = v2(96, 96);
			info->staticCollide = true;

			info = &game->actorTypeInfos[ACTOR_BUG_DEN];
			info->size = v2(96, 96);
			info->team = TEAM_ENEMY;
			info->maxHp = 100;

			info = &game->actorTypeInfos[ACTOR_BUG];
			info->size = v2(16, 16);
			info->dynamicCollide = true;
			info->mass = 0.05;
			info->team = TEAM_ENEMY;
			info->maxHp = 100;
			info->maxEnergy = 100;
			info->burns = true;

			info = &game->actorTypeInfos[ACTOR_TOWER];
			info->team = TEAM_ALLY;
			info->size = v2(64, 64);
			info->staticCollide = true;
			info->towerRange = 200;
			info->buildAmountNeeded = 100;
			info->maxHp = 200;
			info->maxEnergy = 100;

			info = &game->actorTypeInfos[ACTOR_ITEM];
			info->size = v2(16, 16);

			info = &game->actorTypeInfos[ACTOR_FLAME_TOWER];
			info->team = TEAM_ALLY;
			info->size = v2(32, 32);
			info->staticCollide = true;
			info->towerRange = 50;
			info->buildAmountNeeded = 100;
			info->maxHp = 50;
			info->maxEnergy = 100;
		} ///

		Scene *scene = &game->scene;
		{ //@copyPastedLoadScene
			loadStruct("Scene", "assets/info/scene.txt", &game->scene);
			loadStructArray("Actor", "assets/info/actors.txt", scene->actors, ACTORS_MAX, sizeof(Actor));

			for (int i = 0; i < ACTORS_MAX; i++) {
				Actor *actor = &scene->actors[i];
				if (actor->type == ACTOR_NONE) {
					scene->actorsNum = i;
					break;
				}
				actor->info = &game->actorTypeInfos[actor->type];
			}
		}

		if (game->scene.actorsNum == 0) {
			Actor *actor = createActor(ACTOR_PLAYER);
			actor->isSpawner = true;
		}

		game->zoom = 1;

		game->actorsAroundPool = createPool(Megabytes(8));
		game->worldRect = makeCenteredSquare(v2(0, 0), 10000);
	}

	if (game->debugDoBigStep) {
		game->debugDoBigStep = false;
		renderer->disabled = true;

		NanoTime nano = getNanoTime();
		for (int i = 0; i < game->debugBigStepCount; i++) {
			stepGame();
		}

		renderer->disabled = false;
		logf("Took %.1fms\n", getMsPassed(nano));
	}

	if (game->gameData.ticks == 0) {
		for (int i = 0; i < 1000; i++) stepGame();
	}

	stepGame();
}

void drawQuadTree(QuadTree *tree, int depth=0);
void drawQuadTree(QuadTree *tree, int depth) {
	drawRectOutline(tree->bounds, clampMap(depth, 0, 5, 20, 5), 0x80FFFFFF);
	if (tree->subdivided) {
		for (int i = 0; i < 4; i++) drawQuadTree(&tree->children[i], depth+1);
	}
}

void stepGame() {
	Scene *scene = &game->scene;
	GameData *gameData = &game->gameData;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);

	clearRenderer();
	game->actorsTree = createQuadTree(game->worldRect);

	if (keyJustPressed(KEY_BACKTICK)) game->debugMode = !game->debugMode;

	if (game->debugMode) {
		if (keyJustReleased(' ')) game->debugFreeCamera = !game->debugFreeCamera;

		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
			ImGui::Begin("Game", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			if (ImGui::Button("Save scene")) {
				Actor *actorsToSave = (Actor *)frameMalloc(sizeof(Actor) * scene->actorsNum);
				int actorsToSaveNum = 0;

				for (int i = 0; i < scene->actorsNum; i++) {
					Actor *actor = &scene->actors[i];
					if (actor->wasSpawned) continue;

					Actor toSave = *actor;
					toSave.hasSpawned = false;
					actorsToSave[actorsToSaveNum++] = toSave;
				}
				saveStruct("Scene", "assets/info/scene.txt", &game->scene);
				saveStructArray("Actor", "assets/info/actors.txt", actorsToSave, actorsToSaveNum, sizeof(Actor));
			}
			if (ImGui::Button("Load scene")) {
				{ //@copyPastedLoadScene
					loadStruct("Scene", "assets/info/scene.txt", &game->scene);
					loadStructArray("Actor", "assets/info/actors.txt", scene->actors, ACTORS_MAX, sizeof(Actor));

					for (int i = 0; i < ACTORS_MAX; i++) {
						Actor *actor = &scene->actors[i];
						if (actor->type == ACTOR_NONE) {
							scene->actorsNum = i;
							break;
						}
						actor->info = &game->actorTypeInfos[actor->type];
					}
				}
			}

			ImGui::Separator();
			ImGui::Checkbox("God mode", &game->debugGodMode);
			ImGui::Checkbox("Kill attackers", &game->debugKillAttackers);
			ImGui::Checkbox("Show spawners", &game->showSpawners);

			ImGui::InputInt("###debugBigStepCount", &game->debugBigStepCount);
			ImGui::SameLine();
			if (ImGui::Button("Do big step")) game->debugDoBigStep = true;

			ImGui::Separator();
			ImGui::Text(
				"actorsAroundPool: %d/%d (%.1f%%)\n",
				game->actorsAroundPool->dataHighIndexMark,
				game->actorsAroundPool->dataMax,
				game->actorsAroundPool->dataHighIndexMark/(float)game->actorsAroundPool->dataMax * 100
			);

			ImGui::End();
		}
	}

	{ /// Ui mode
		ActorType typeToBuild = ACTOR_NONE;

		if (keyJustReleased('1')) typeToBuild = ACTOR_TOWER;
		if (keyJustReleased('2')) typeToBuild = ACTOR_FLAME_TOWER;
		if (game->uiMode == UI_NORMAL) {
		} else if (game->uiMode == UI_BUILD) {
			if (keyJustReleased('Q')) game->uiMode = UI_NORMAL;
		}

		if (typeToBuild) {
			game->typeToBuild = typeToBuild;
			game->uiMode = UI_BUILD;
		}
	} ///

	Actor *player = NULL;
	for (int i = 0; i < scene->actorsNum; i++) { // Init loop
		Actor *actor = &scene->actors[i];
		actor->immTint = 0;
		insert(game->actorsTree, actor->hitRect, (u64)actor);

		if (actor->isSpawner) {
			if (actor->hasSpawned) continue;

			actor->hasSpawned = true; //@copyPastedDoSpawn
			Actor *newActor = createActor(actor->type);
			newActor->unkillable = actor->unkillable;
			newActor->position = actor->position;
			newActor->variant = actor->variant;
			newActor->wasSpawned = true;
			newActor->srcSpawner = actor->id;
		} else {
			if (actor->type == ACTOR_PLAYER) player = actor;
		}
	}

	if (game->debugMode && game->debugFreeCamera) {
		Vec2 cameraCenter = game->cameraCenter;
		float speed = 10;
		if (keyPressed('W')) cameraCenter.y -= speed;
		if (keyPressed('S')) cameraCenter.y += speed;
		if (keyPressed('A')) cameraCenter.x -= speed;
		if (keyPressed('D')) cameraCenter.x += speed;
		game->cameraCenter = lerp(game->cameraCenter, cameraCenter, 0.8);
	} else {
		Vec2 cameraCenter = v2();
		cameraCenter = player->position;
		game->cameraCenter = lerp(game->cameraCenter, cameraCenter, 0.8);
	}

	if (platform->mouseWheel > 0) {
		game->zoom += 0.05;
		logf("Zoom: %f\n", game->zoom);
	}

	if (platform->mouseWheel < 0) {
		game->zoom -= 0.05;
		if (game->zoom < 0.1) game->zoom = 0.1;
		logf("Zoom: %f\n", game->zoom);
	}

	Matrix3 cameraMatrix = mat3();
	Vec2 cameraOffset;
	cameraMatrix.TRANSLATE(platform->windowWidth/2, platform->windowHeight/2);
	cameraMatrix.SCALE(game->zoom);
	cameraMatrix.TRANSLATE(1.0 - game->cameraCenter);
	// cameraOffset.x = platform->windowWidth/2 - game->cameraCenter.x;
	// cameraOffset.y = platform->windowHeight/2 - game->cameraCenter.y;
	// cameraMatrix.TRANSLATE(cameraOffset);
	pushCamera2d(cameraMatrix);

	Vec2 mouse = cameraMatrix.invert() * platform->mouse;

	// {
	// 	Rect rect = makeRect(-400, -400, 500, 500);
	// 	drawRectOutline(rect, 5, 0xFFFF0000);
	// 	int actorsAroundNum;
	// 	Actor **actorsAround = getActorsAroundRect(rect, ACTOR_NONE, &actorsAroundNum);
	// 	for (int i = 0; i < actorsAroundNum; i++) {
	// 		Actor *actor = actorsAround[i];
	// 		drawRect(actor->hitRect, 0xFFFF0000);
	// 	}
	// }

	float denRadii[] = {128, 256};
	float denSpawnTimes[] = {5, 1.5};
	// float denSpawnCounts[] = {4, 15};
	float denSpawnCounts[] = {4*5, 15*5};

	for (int i = 0; i < scene->actorsNum; i++) { // Main update
		Actor *actor = &scene->actors[i];
		if (actor->isSpawner) continue;

		if (actor->targetActor) {
			Actor *target = getActor(actor->targetActor);
			if (!target) actor->targetActor = 0;
		}

		actor->texture = NULL;

		float passiveAggroRange = 0;
		Actor *chaseTarget = NULL;

		if (actor->type == ACTOR_PLAYER) {
			if (actor->tick % 10 == 0) actor->energy++;

			actor->texture = getTexture("assets/images/player.png");

			Vec2 inputVec = v2();
			if (!game->debugMode || !game->debugFreeCamera) {
				if (keyPressed('W')) inputVec.y--;
				if (keyPressed('S')) inputVec.y++;
				if (keyPressed('A')) inputVec.x--;
				if (keyPressed('D')) inputVec.x++;
			}

			actor->accel += inputVec.normalize() * 1;
		} else if (actor->type == ACTOR_DEBUG_WALL) {
			actor->texture = getTexture("assets/images/debugWall.png");
		} else if (actor->type == ACTOR_BUG_DEN) {
			actor->texture = getTexture("assets/images/bugDen.png");

			if (actor->variant == 1) actor->immTint = 0x20FF0000;

			actor->denTimeTillNextSpawn -= elapsed;
			if (actor->denTimeTillNextSpawn <= 0) {
				actor->denTimeTillNextSpawn = denSpawnTimes[actor->variant];

				int actorsAroundNum = countActorsAround(actor->position, denRadii[actor->variant], ACTOR_BUG);

				int denMax = denSpawnCounts[actor->variant];

				if (actorsAroundNum < denMax) {
					Actor *newActor = createActor(ACTOR_BUG);
					newActor->homeDen = actor->id;
					newActor->wasSpawned = true;
					newActor->position = actor->position;
				}
			}
		} else if (actor->type == ACTOR_BUG) {
			actor->texture = getTexture("assets/images/bug.png");

			actor->energy += 3;

			passiveAggroRange = 64;

			if (actor->targetActor) {
				chaseTarget = getActor(actor->targetActor);

				if (chaseTarget) {
					float dist = getDistance(actor->hitRect, chaseTarget->hitRect);
					if (actor->energy >= 100 && dist <= 4) {
						dealDamage(chaseTarget, 3, actor);
						actor->energy = 0;
					}
				}
			} else {
				actor->accel.x = rndFloat(-0.5, 0.5);
				actor->accel.y = rndFloat(-0.5, 0.5);

				Actor *den = getActor(actor->homeDen);
				if (den) {
					float dist = actor->position.distance(den->position);
					float denRadius = denRadii[actor->variant];
					if (dist > denRadius && dist < denRadius*3) {
						actor->accel += (den->position - actor->position).normalize() * 0.1;
					}
				}
			}
		} else if (actor->type == ACTOR_TOWER) {
			if (actor->buildAmount < actor->info->buildAmountNeeded) {
				actor->texture = getTexture("assets/images/unbuilt.png");
			} else {
				actor->texture = getTexture("assets/images/tower.png");

				actor->energy++;
				int shotEnergy = 100;
				if (actor->energy >= shotEnergy) {
					Actor *closest = getClosestEnemyActor(actor->position, getTowerRange(actor));

					if (closest) {
						actor->energy -= shotEnergy;
						dealDamage(closest, 60, actor);

						Effect *effect = createEffect(EFFECT_TOWER_SHOT);
						effect->startPos = actor->position;
						effect->endPos = closest->position;
					}
				}
			}
		} else if (actor->type == ACTOR_ITEM) {
			if (actor->variant == 0) {
				actor->texture = getTexture("assets/images/energyTank.png");
			} else if (actor->variant == 1) {
				actor->texture = getTexture("assets/images/towerRange.png");
			} else if (actor->variant == 2) {
				actor->texture = getTexture("assets/images/autoBuilder.png");
			}
		} else if (actor->type == ACTOR_FLAME_TOWER) {
			if (actor->buildAmount < actor->info->buildAmountNeeded) {
				actor->texture = getTexture("assets/images/unbuilt.png");
			} else {
				actor->texture = getTexture("assets/images/flameTower.png");

				actor->energy += 2;
				int shotEnergy = 100;
				if (actor->energy >= shotEnergy) {
					int actorsAroundNum;
					Actor **actorsAround = getActorsAround(actor->position, getTowerRange(actor), ACTOR_NONE, &actorsAroundNum);
					bool didShoot = false;
					for (int i = 0; i < actorsAroundNum; i++) {
						Actor *other = actorsAround[i];
						if (other->info->team != TEAM_ENEMY) continue;
						if (!didShoot) {
							didShoot = true;
							actor->energy -= shotEnergy;
						}
						dealDamage(other, 10, actor);
						Effect *effect = createEffect(EFFECT_TOWER_SHOT);
						effect->startPos = actor->position;
						effect->endPos = other->position;
					}
				}
			}
		}

		{ /// Passive aggro
			if (passiveAggroRange && actor->tick % 8 == 0) {
				int actorsAroundNum;
				Actor **actorsAround = getActorsAround(actor->position, passiveAggroRange, ACTOR_NONE, &actorsAroundNum);

				Actor *closestActor = NULL;
				float closestDist; 
				for (int i = 0; i < actorsAroundNum; i++) {
					Actor *other = actorsAround[i];
					if (other->info->team != TEAM_ALLY) continue;

					float dist = getDistance(actor->hitRect, other->hitRect);
					if (!closestActor || closestDist > dist) {
						closestActor = other;
						closestDist = dist;
					}
				}

				if (closestActor) actor->targetActor = closestActor->id;
			}
		}

		{ /// Chase
			if (chaseTarget) {
				Vec2 between = vectorBetween(actor->position, chaseTarget->position);
				actor->accel = between * 0.5;
			}
		}

		{ /// Hit aggro
			int hitId = actor->hitAggroById;
			actor->hitAggroById = 0;
			if (hitId) {
				if (actor->info->team == TEAM_ENEMY) {
					int actorsAroundNum;
					Actor **actorsAround = getActorsAround(actor->position, 256, ACTOR_NONE, &actorsAroundNum);
					for (int i = 0; i < actorsAroundNum; i++) {
						Actor *other = actorsAround[i];
						if (other->isSpawner) continue;
						if (other->targetActor) continue;

						if (other->type == ACTOR_BUG) {
							other->targetActor = hitId;
						}
					}
				}
			}
		}

		{ /// Auto build
			if (gameData->hasAutoBuilder && game->autoBuildEnabled) {
				if (
					actor->type == ACTOR_TOWER ||
					actor->type == ACTOR_FLAME_TOWER
				) {
					float dist = getDistance(actor->hitRect, player->hitRect);
					if (dist <= 16) {
						tryBuildTower(player, actor);
					}
				}
			}
		}

		if (actor->energy > getMaxEnergy(actor)) actor->energy = getMaxEnergy(actor);
		if (actor->hp > actor->info->maxHp) actor->hp = actor->info->maxHp;
		if (actor->hp <= 0 && actor->info->maxHp != 0) actor->markedForDeletion = true;

		actor->tick++;
	}

	for (int i = 0; i < scene->actorsNum; i++) { // Update all
		Actor *actor = &scene->actors[i];
		actor->hitRect = makeCenteredRect(actor->position, actor->info->size);
	}

	for (int i = 0; i < scene->actorsNum; i++) { // Phys
		Actor *actor = &scene->actors[i];
		if (actor->isSpawner) continue;

		actor->velo += actor->accel;
		actor->accel = v2();

		actor->velo *= 0.8;

		Vec2 newPosition = actor->position + actor->velo;

		if (actor->info->dynamicCollide) { // Handle collision with statics
			Vec2 searchPosition = (newPosition + actor->position)/2;

			float moveDist = actor->position.distance(newPosition);
			float diagonalSize = actor->info->size.length();
			float searchRadius = (diagonalSize+moveDist)*2;

			int actorsAroundNum;
			Actor **actorsAround = getActorsAround(searchPosition, searchRadius, ACTOR_NONE, &actorsAroundNum);
			for (int i = 0; i < actorsAroundNum; i++) {
				Actor *other = actorsAround[i];
				if (!other->info->staticCollide) continue;

				Rect newRect = makeCenteredRect(newPosition, actor->info->size);
				if (contains(other->hitRect, newRect)) {

					Rect newRectX = makeCenteredRect(v2(newPosition.x, actor->position.y), actor->info->size);
					if (contains(other->hitRect, newRectX)) {
						newPosition.x = actor->position.x;
						actor->velo.x *= 0.9;
					}

					Rect newRectY = makeCenteredRect(v2(actor->position.x, newPosition.y), actor->info->size);
					if (contains(other->hitRect, newRectY)) {
						newPosition.y = actor->position.y;
						actor->velo.y *= 0.9;
					}
				}
			}
		}

		actor->position = newPosition;

		if (actor->info->dynamicCollide) { // Handle collision with dynamics
			int actorsAroundNum;
			Actor **actorsAround = getActorsAround(actor->position, actor->info->size.x, ACTOR_NONE, &actorsAroundNum);
			for (int i = 0; i < actorsAroundNum; i++) {
				Actor *other = actorsAround[i];
				if (actor == other) continue;
				if (!other->info->dynamicCollide) continue;

				Circle circle = makeCircle(actor->position, actor->info->size.x);
				Circle otherCircle = makeCircle(other->position, other->info->size.x);
				if (!contains(circle, otherCircle)) continue;

				float totalMass = actor->info->mass + other->info->mass;
				float actorMassPerc = 1 - (actor->info->mass / totalMass);
				float otherMassPerc = 1 - (other->info->mass / totalMass);

				Vec2 separateVec = (actor->position - other->position).normalize();
				actor->accel += separateVec * 0.3*actorMassPerc;
				other->accel -= separateVec * 0.3*otherMassPerc;
			}
		}

		if (actor->velo.length() > 0.1) actor->rotation = toDeg(actor->velo);
	}

	for (int i = 0; i < scene->actorsNum; i++) { /// Draw / Render
		Actor *actor = &scene->actors[i];
		if (actor->isSpawner) continue;

		Texture *texture = actor->texture;

		if (!texture) {
			logf("Actor %s has no texture\n", actorTypeStrings[actor->type]);
			continue;
		}

		Rect rect = actor->hitRect;
		RenderProps props = newRenderProps();

		props.matrix.TRANSLATE(rect.x, rect.y);
		props.matrix.SCALE(rect.width, rect.height);

		props.matrix.TRANSLATE(0.5, 0.5);
		props.matrix.ROTATE(actor->rotation);
		props.matrix.TRANSLATE(-0.5, -0.5);

		props.srcWidth = props.srcHeight = 1;
		setTextureSmooth(texture, false);

		// if (actor->type == ACTOR_TOWER) {
		// 	props.tint = lerpColor(0x00FF0000, 0xA0FF0000, (float)actor->energy / getMaxEnergy(actor));
		// }
		props.tint += actor->immTint;

		drawTexture(texture, props);

		float barY = actor->hitRect.y;
		if (actor->energy < getMaxEnergy(actor) && getMaxEnergy(actor) != 0) {
			Rect bar = makeRect(0, 0, 24, 6);
			bar.x = actor->hitRect.x + actor->hitRect.width/2 - bar.width/2;
			bar.y = barY - bar.height - 4;
			drawRect(bar, 0xFFCCCCFF);

			bar.width *= (float)actor->energy / getMaxEnergy(actor);
			drawRect(bar, 0xFF5555FF);

			barY -= bar.height + 4;
		}

		if (actor->hp < actor->info->maxHp && actor->info->maxHp != 0) {
			Rect bar = makeRect(0, 0, 24, 6);
			bar.x = actor->hitRect.x + actor->hitRect.width/2 - bar.width/2;
			bar.y = barY - bar.height - 4;
			drawRect(bar, 0xFFFF0000);

			bar.width *= (float)actor->hp / (float)actor->info->maxHp;
			drawRect(bar, 0xFF00FF00);
		}

		if (keyPressed(KEY_SHIFT)) drawRect(actor->hitRect, 0x80FF0000);
	}

	int respawnPlayerFrom = 0;
	for (int i = 0; i < scene->actorsNum; i++) { // Deleted markedForDeletion
		Actor *actor = &scene->actors[i];

		if (actor->markedForDeletion) {
			if (actor->type == ACTOR_PLAYER) respawnPlayerFrom = actor->srcSpawner;
			scene->actors[i] = scene->actors[scene->actorsNum-1];
			scene->actorsNum--;
			i--;
			continue;
		}
	}

	if (respawnPlayerFrom) {
		Actor *actor = getActor(respawnPlayerFrom);
		if (!actor) Panic("No player or player spawner");

		actor->hasSpawned = true; //@copyPastedDoSpawn
		Actor *newActor = createActor(actor->type);
		newActor->unkillable = actor->unkillable;
		newActor->position = actor->position;
		newActor->variant = actor->variant;
		newActor->wasSpawned = true;
		newActor->srcSpawner = actor->id;
	}

	for (int i = 0; i < game->effectsNum; i++) {
		Effect *effect = &game->effects[i];
		bool effectDone = false;

		if (effect->type == EFFECT_TOWER_SHOT) {
			float perc = effect->time / 0.25;

			float thickness = lerp(5, 0, perc);
			float color = 0xFF880000;
			drawLine(effect->startPos, effect->endPos, thickness, color);

			if (perc > 1) effectDone = true;
		}

		effect->time += elapsed;

		if (effectDone) {
			arraySpliceIndex(game->effects, game->effectsNum, sizeof(Effect), i);
			game->effectsNum--;
			i--;
			continue;
		}
	}

	if (game->showSpawners) {
		for (int i = 0; i < scene->actorsNum; i++) {
			Actor *actor = &scene->actors[i];
			if (!actor->isSpawner) continue;

			Rect rect = makeCenteredRect(actor->position, actor->info->size);
			drawRect(rect, lerpColor(0x10FFFF00, 0xFFFFFF00, timePhase(platform->time*4)));
		}
	}

	if (game->uiMode == UI_NORMAL) {
		pushCamera2d(renderer->currentCameraMatrix.invert());
		drawText(fontSys->defaultFont, "Normal mode", v2(), 0xFF008800);
		popCamera2d();

		Actor *actor = getActor(game->selectedActor);
		if (actor) {
			drawRect(actor->hitRect, lerpColor(0x10FF00FF, 0x80FF00FF, secondPhase));

			Vec2 pos;
			pos.x = actor->hitRect.x + actor->hitRect.width;
			pos.y = actor->hitRect.y;
			pos = cameraMatrix * pos;
			ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), ImGuiCond_Always, ImVec2(0, 0));
			ImGui::Begin("Data", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			if (game->debugMode) {
				if (!actor->isSpawner) {
					if (actor->wasSpawned) {
						guiPushStyleColor(ImGuiCol_Text, 0xFFFF0000);
						ImGui::Text("Operating on spawned entity");
						guiPopStyleColor();
					} else {
						guiPushStyleColor(ImGuiCol_Text, 0xFFFFFF00);
						ImGui::Text("Operating on non-spawner");
						guiPopStyleColor();
					}
				}
				ImGui::Text("Id: %d", actor->id);
				ImGui::Text("Type: %s", actorTypeStrings[actor->type]);

				ImGui::DragFloat2("Position", &actor->position.x);
				ImGui::Separator();

				ImGui::Checkbox("Is spawner", &actor->isSpawner);
				ImGui::Text("Has spawned: %d", actor->hasSpawned);
				ImGui::Text("Was spawned: %d", actor->wasSpawned);
				ImGui::Separator();

				if (actor->type == ACTOR_BUG_DEN) {
					ImGui::InputInt("variant", &actor->variant);
					ImGui::Checkbox("unkillable", &actor->unkillable);
					ImGui::Text("denTimeTillNextSpawn: %f\n", actor->denTimeTillNextSpawn);
					drawCircle(actor->position, denRadii[actor->variant], 0x800000FF);
				} else if (actor->type == ACTOR_ITEM) {
					ImGui::InputInt("variant", &actor->variant);
				}

				ImGui::Separator();
				if (actor->wasSpawned) {
					if (ImGui::Button("Select spawner")) {
						game->selectedActor = actor->srcSpawner;
					}

					// if (ImGui::Button("Clone to spawner")) {
					// 	Actor *spawner = getActor(actor->srcSpawner);
					// 	if (spawner) {
					// 		int id = spawner->id;
					// 		*spawner = *actor;
					// 		spawner->id = id;
					// 		spawner->isSpawner = true;
					// 		spawner->hasSpawned = true;
					// 		spawner->wasSpawned = false;
					// 		spawner->srcSpawner = 0;
					// 	} else {
					// 		logf("No spawner???\n");
					// 	}
					// }
				}

				if (ImGui::Button("Duplicate")) {
					Actor *newActor = createActor(actor->type);
					int id = newActor->id;
					*newActor = *actor;
					newActor->id = id;
					game->selectedActor = newActor->id;
				}
				if (ImGui::Button("Delete")) actor->markedForDeletion = true;
			} else {
				if (actor->info->maxHp > 0) ImGui::Text("Hp: %d/%d", actor->hp, actor->info->maxHp);
				if (actor->buildAmount < actor->info->buildAmountNeeded) {
					ImGui::Text("Status: Unbuilt");
					ImGui::ProgressBar((float)actor->buildAmount / (float)actor->info->buildAmountNeeded);

					// Vec2 point = actor->hitRect.getClosestPoint(player->position);
					// Vec2 playerPoint = player->hitRect.getClosestPoint(point);
					// float dist = playerPoint.distance(point);
					float dist = getDistance(actor->hitRect, player->hitRect);
					if (dist > 16) {
						ImGui::Text("Too far away to build");
					} else {
						ImGui::Button("Build");
						if (ImGui::IsItemActive()) {
							tryBuildTower(player, actor);
						}
					}
				}

				if (getMaxEnergy(actor)) ImGui::Text("Energy: %d/%d", actor->energy, getMaxEnergy(actor));
				if (getTowerRange(actor)) drawCircle(actor->position, getTowerRange(actor), 0x80FF0000);

				if (actor->type == ACTOR_ITEM) {
					char *str = NULL;

					if (actor->variant == 0) {
						str = "Energy packs:\nExtends your energy storage";
					} else if (actor->variant == 1) {
						str = "Advanced radar:\nTowers get +50%% range";
					} else if (actor->variant == 2) {
						str = "Auto builder:\nAutomatically build structures";
					}

					ImGui::Text(str);

					float dist = getDistance(actor->hitRect, player->hitRect);
					if (dist < 16) {
						if (ImGui::Button("Take")) {
							actor->markedForDeletion = true;

							if (actor->variant == 0) {
								gameData->hasEnergyTank = true;
							} else if (actor->variant == 1) {
								gameData->hasTowerRange = true;
							} else if (actor->variant == 2) {
								gameData->hasAutoBuilder = true;
								game->autoBuildEnabled = true;
							}

						}
					} else {
						ImGui::Text("Too far to take");
					}
				}
			}
			ImGui::End();
		}

		if (platform->mouseJustUp) {
			bool foundActor = false;
			int lastSelectedActor = game->selectedActor;

			for (int i = 0; i < scene->actorsNum; i++) {
				Actor *actor = &scene->actors[i];
				if (actor->isSpawner && !game->showSpawners) continue;
				if (actor->id == lastSelectedActor) continue;

				if (contains(actor->hitRect, mouse)) {
					game->selectedActor = actor->id;
					foundActor = true;
				}
			}

			if (!foundActor) game->selectedActor = 0;
		}
	}

	if (game->uiMode == UI_BUILD) {
		pushCamera2d(renderer->currentCameraMatrix.invert());
		char *str = frameSprintf("Building: %s", actorTypeStrings[game->typeToBuild]);
		Vec2 size = getTextSize(fontSys->defaultFont, str);
		Vec2 pos = v2(platform->windowWidth - size.x, 0);
		drawText(fontSys->defaultFont, str, pos, 0xFF404080);
		popCamera2d();

		if (keyJustPressed(KEY_RIGHT)) game->typeToBuild = (ActorType)((int)game->typeToBuild+1);
		if (keyJustPressed(KEY_LEFT)) game->typeToBuild = (ActorType)((int)game->typeToBuild-1);
		if (game->typeToBuild < ACTOR_NONE) game->typeToBuild = (ActorType)(ACTOR_TYPES_MAX-1);
		if (game->typeToBuild > ACTOR_TYPES_MAX-1) game->typeToBuild = (ActorType)((int)ACTOR_NONE+1);

		ActorTypeInfo *info = &game->actorTypeInfos[game->typeToBuild];
		Rect rect = makeRect(v2(0, 0), info->size);

		rect.x = mouse.x - rect.width/2;
		rect.y = mouse.y - rect.height/2;

		if (keyPressed(KEY_SHIFT)) {
			rect.x = roundToNearest(rect.x, 16);
			rect.y = roundToNearest(rect.y, 16);
		}

		bool canBuild = true;

		{ // Check for static actors
			int actorsAroundNum;
			Actor **actorsAround = getActorsAroundRect(rect, ACTOR_NONE, &actorsAroundNum);
			for (int i = 0; i < actorsAroundNum; i++) {
				Actor *actor = actorsAround[i];
				if (actor->info->staticCollide) {
					canBuild = false;
					break;
				}
			}
		}

		if (canBuild) { // Check for dynamic actors
			int actorsAroundNum;
			Actor **actorsAround = getActorsAroundRect(rect.inflate(16), ACTOR_NONE, &actorsAroundNum);
			for (int i = 0; i < actorsAroundNum; i++) {
				Actor *actor = actorsAround[i];
				if (actor->info->dynamicCollide) {
					canBuild = false;
					break;
				}
			}
		}

		if (getTowerRange(info->type)) drawCircle(getCenter(rect), getTowerRange(info->type), 0x40C00000);

		int color = lerpColor(0x10FFFFFF, 0x80FFFFFF, secondPhase);
		if (!canBuild) color = 0x40FF0000;

		drawRect(rect, color);
		if (platform->mouseJustUp) {
			if (canBuild) {
				Actor *actor = createActor(game->typeToBuild);
				actor->position = getCenter(rect);
				if (actor->buildAmount < actor->info->buildAmountNeeded) {
					game->uiMode = UI_NORMAL;
				}
			} else {
				infof("Can't build here\n");
			}
		}
	}

	{
		ImGui::SetNextWindowPos(ImVec2(0, platform->windowHeight), ImGuiCond_Always, ImVec2(0, 1));
		ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_Always);
		ImGui::Begin("Status", NULL, ImGuiWindowFlags_NoResize);

		ImGui::Text("Hp");
		ImGui::SameLine();
		ImGui::ProgressBar((float)player->hp / (float)player->info->maxHp, ImVec2(-FLT_MIN, 0), frameSprintf("%d/%d", player->hp, player->info->maxHp));

		ImGui::Text("Energy");
		ImGui::SameLine();
		ImGui::ProgressBar((float)player->energy / getMaxEnergy(player), ImVec2(-FLT_MIN, 0), frameSprintf("%d/%d", player->energy, getMaxEnergy(player)));

		if (ImGui::Button("Energy cheat")) player->energy = getMaxEnergy(player);
		if (gameData->hasAutoBuilder) ImGui::Checkbox("Auto build", &game->autoBuildEnabled);

		ImGui::End();
	}

	popCamera2d();

	drawOnScreenLog();

	destroyQuadTree(game->actorsTree);
	poolFree(game->actorsAroundPool);
	gameData->ticks++;
}

Actor *createActor(ActorType type) {
	Scene *scene = &game->scene;

	if (scene->actorsNum > ACTORS_MAX-1) {
		Panic("Not enough actors!\n");
	}

	Actor *actor = &scene->actors[scene->actorsNum++]; //@robustness
	memset(actor, 0, sizeof(Actor));
	actor->id = ++scene->nextActorId;
	actor->type = type;
	actor->info = &game->actorTypeInfos[type];

	if (actor->info->buildAmountNeeded == 0) {
		actor->hp = actor->info->maxHp;
	} else {
		actor->hp = 1;
	}

	if (actor->type == ACTOR_PLAYER) actor->energy = 200;

	return actor;
}

Actor *getActor(int id) {
	Scene *scene = &game->scene;
	for (int i = 0; i < scene->actorsNum; i++) {
		Actor *actor = &scene->actors[i];
		if (actor->id == id) return actor;
	}

	return NULL;
}

Actor **getActorsAround(Vec2 position, float radius, ActorType type, int *outActorsNum) {
	Scene *scene = &game->scene;
#if 1
	int dataMax = scene->actorsNum;
	int dataNum = 0;
	u64 *data = (u64 *)poolZalloc(game->actorsAroundPool, sizeof(u64) * dataMax);
	query(game->actorsTree, makeCenteredSquare(position, radius*2), data, &dataNum, dataMax);

	Actor **possibleActors = (Actor **)data;
	int possibleActorsNum = dataNum;

	Actor **actors = (Actor **)poolZalloc(game->actorsAroundPool, sizeof(Actor *) * possibleActorsNum);
	int actorsNum = 0;

	Circle circle = makeCircle(position, radius);
	for (int i = 0; i < possibleActorsNum; i++) {
		Actor *actor = possibleActors[i];
		if (actor->isSpawner) continue;
		if (type != ACTOR_NONE && actor->type != type) continue;
		if (!contains(actor->hitRect, circle)) continue;
		actors[actorsNum++] = actor;
	}

	*outActorsNum = actorsNum;
	return actors;
#else

	// Actor **actors = (Actor **)frameMalloc(sizeof(Actor *) * scene->actorsNum);
	Actor **actors = (Actor **)poolZalloc(game->actorsAroundPool, sizeof(Actor *) * scene->actorsNum);
	int actorsNum = 0;

	for (int i = 0; i < scene->actorsNum; i++) {
		Actor *actor = &scene->actors[i];
		if (actor->isSpawner) continue;
		if (type != ACTOR_NONE && actor->type != type) continue;

		Circle circle = makeCircle(position, radius);
		if (!contains(actor->hitRect, circle)) continue;

		actors[actorsNum++] = actor;
	}

	*outActorsNum = actorsNum;
	return actors;
#endif
}

int countActorsAround(Vec2 position, float radius, ActorType type) {
	int actorsNum = 0;
	getActorsAround(position, radius, type, &actorsNum);
	return actorsNum;
}

Actor **getActorsAroundRect(Rect rect, ActorType type, int *outActorsNum) {
	Scene *scene = &game->scene;

#if 1
	int dataMax = scene->actorsNum;
	int dataNum = 0;
	u64 *data = (u64 *)poolZalloc(game->actorsAroundPool, sizeof(u64) * dataMax);
	query(game->actorsTree, rect, data, &dataNum, dataMax);

	Actor **possibleActors = (Actor **)data;
	int possibleActorsNum = dataNum;

	Actor **actors = (Actor **)poolZalloc(game->actorsAroundPool, sizeof(Actor *) * possibleActorsNum);
	int actorsNum = 0;

	for (int i = 0; i < possibleActorsNum; i++) {
		Actor *actor = possibleActors[i];
		if (actor->isSpawner) continue;
		if (type != ACTOR_NONE && actor->type != type) continue;
		actors[actorsNum++] = actor;
	}

	*outActorsNum = actorsNum;
	return actors;
#else

	// Actor **actors = (Actor **)frameMalloc(sizeof(Actor *) * scene->actorsNum);
	Actor **actors = (Actor **)poolZalloc(game->actorsAroundPool, sizeof(Actor *) * scene->actorsNum);
	int actorsNum = 0;

	for (int i = 0; i < scene->actorsNum; i++) {
		Actor *actor = &scene->actors[i];
		if (actor->isSpawner) continue;
		if (type != ACTOR_NONE && actor->type != type) continue;

		if (!contains(rect, actor->hitRect)) continue;

		actors[actorsNum++] = actor;
	}

	*outActorsNum = actorsNum;
	return actors;
#endif
}

Actor *getClosestEnemyActor(Vec2 position, float radius) {
	int actorsAroundNum;
	Actor **actorsAround = getActorsAround(position, radius, ACTOR_NONE, &actorsAroundNum);

	Circle circle = makeCircle(position, radius);

	Actor *closest = NULL;
	float closestDist;
	for (int i = 0; i < actorsAroundNum; i++) {
		Actor *other = actorsAround[i];
		if (other->info->team != TEAM_ENEMY) continue;
		if (!contains(other->hitRect, circle)) continue;

		float dist = position.distance(other->position);
		if (!closest || closestDist > dist) {
			closest = other;
			closestDist = dist;
		}
	}

	return closest;
}

void dealDamage(Actor *actor, int damage, Actor *src) {
	if (actor->unkillable) return;
	if (actor->type == ACTOR_PLAYER) {
		if (game->debugKillAttackers && src) src->hp = 0;
		if (game->debugGodMode) return;
	}

	if (src->type == ACTOR_FLAME_TOWER && actor->info->burns) damage *= 5;

	actor->hp -= damage;
	actor->hitAggroById = src->id;
}


Effect *createEffect(EffectType type) {
	Effect *effect;

	if (game->effectsNum > EFFECTS_MAX-1) {
		effect = &game->effects[0];
		logf("Effect overflow\n");
	} else {
		effect = &game->effects[game->effectsNum++];
	}

	memset(effect, 0, sizeof(Effect));
	effect->type = type;
	return effect;
}

void tryBuildTower(Actor *player, Actor *tower) {
	if (tower->buildAmount >= tower->info->buildAmountNeeded) return;

	if (player->energy > 0) {
		player->energy--;
		tower->buildAmount++;
		if (tower->buildAmount < tower->info->buildAmountNeeded) {
			int hpToGivePerTick = tower->info->maxHp / (float)tower->info->buildAmountNeeded;
			if (hpToGivePerTick < 1) hpToGivePerTick = 1;
			tower->hp += hpToGivePerTick;
		}
	}
}

int getMaxEnergy(Actor *actor) {
	GameData *gameData = &game->gameData;

	int maxEnergy = actor->info->maxEnergy;

	if (actor->type == ACTOR_PLAYER && gameData->hasEnergyTank) maxEnergy += 800;

	return maxEnergy;
}

float getTowerRange(Actor *actor) {
	float towerRange = getTowerRange(actor->type);
	return towerRange;
}

float getTowerRange(ActorType type) {
	GameData *gameData = &game->gameData;
	ActorTypeInfo *info = &game->actorTypeInfos[type];

	float towerRange = info->towerRange;
	if (type == ACTOR_TOWER && gameData->hasTowerRange) towerRange *= 1.5;

	return towerRange;
}
