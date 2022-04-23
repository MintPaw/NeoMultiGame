struct Globals {
};

enum ActorType {
	ACTOR_NONE,
	ACTOR_PLAYER,
	ACTOR_FLOOR_TEXTURE,
	ACTOR_BITER,
	ACTOR_SLASHER,
	ACTOR_DASHER,
	ACTOR_SHOOTER,
	ACTOR_TYPES_MAX,
};
struct ActorTypeInfo {
	ActorType type;
	bool hittable;
	bool ally;
	float hp;
	float stamina;
	Vec2 size;
	bool hasSoftCollision;
};
struct Actor {
	ActorType type;
	ActorTypeInfo *info;

	Vec2 position;

	char texturePath[PATH_MAX_LEN];

	/// Unserialized
	Vec2 velo;
	Vec2 force;

	float hp;
	float stamina;
	Circle circle;
	Rect rect;
	bool markedForDeletion;

	float enemyTimer0;

	float timeSinceLastAttacked;
	float timeSinceLastHit;
	bool hitBySlashThisFrame;

	int prevPhaseTime;
	int phase;
	float phaseTime;
};

struct Map {
	Vec2 size;
#define ACTORS_MAX 256
	Actor actors[ACTORS_MAX];
	int actorsNum;
};

enum BulletType {
	BULLET_NONE,
	BULLET_SLASH,
};
struct Bullet {
	BulletType type;
	bool ally;
	Vec2 position;
	float rotation;

	float slashDistance;
	float slashSpread;
	Vec2 slashSizeMinMax;
};

enum GameState {
	GAME_NONE,
	GAME_PLAY,
};
struct Game {
	Font *defaultFont;
	// Font *hugeFont;
	Texture *gameTexture;

	Globals globals;
	bool inEditor;
	float timeScale;
	float time;
	Vec2 size;
	float sizeScale;
	Vec2 screenMouse;
	Vec2 worldMouse;

	Vec2 screenOverlayOffset;
	Vec2 screenOverlaySize;

	GameState state;
	GameState prevState;
	GameState nextState;
	float stateTransition_t;
	float stateTime;

	ActorTypeInfo actorTypeInfos[ACTOR_TYPES_MAX];

	Map map;
	Vec2 cameraTarget;

#define BULLETS_MAX 512
	Bullet bullets[BULLETS_MAX];
	int bulletsNum;

	/// Editor/debug
	bool debugShowFrameTimes;
};
Game *game = NULL;

void runGame();
void updateGame();
Actor *createActor(ActorType type);
void deleteActor(Actor *actor);
Bullet *createBullet(BulletType type, Vec2 position, float rotation);
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

	initPlatform(res.x, res.y, "tdSlasherGame");
	platform->sleepWait = true;
	initAudio();
	initRenderer(res.x, res.y);
	initTextureSystem();
	initFonts();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 20);

		// RegMem(Globals, useFXAA);
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);

		{ /// Setup actor type infos
			ActorTypeInfo *info;

			info = &game->actorTypeInfos[ACTOR_PLAYER];
			info->ally = true;
			info->hittable = true;
			info->hasSoftCollision = true;

			info = &game->actorTypeInfos[ACTOR_BITER];
			info->hittable = true;
			info->size = v2(32, 32);
			info->hp = 20;
			info->hasSoftCollision = true;

			info = &game->actorTypeInfos[ACTOR_SLASHER];
			info->hittable = true;
			info->hp = 30;
			info->hasSoftCollision = true;

			info = &game->actorTypeInfos[ACTOR_DASHER];
			info->hittable = true;
			info->hp = 20;
			info->hasSoftCollision = true;

			info = &game->actorTypeInfos[ACTOR_SHOOTER];
			info->hittable = true;
			info->hp = 20;
			info->hasSoftCollision = true;

			info = &game->actorTypeInfos[ACTOR_FLOOR_TEXTURE];

			for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
				ActorTypeInfo *info = &game->actorTypeInfos[i];
				info->type = (ActorType)i;
				if (isZero(info->size)) info->size = v2(64, 64);
				if (info->hp == 0) info->hp = 100;
				if (info->stamina == 0) info->stamina = 100;
			}
		}

		game->timeScale = 1;
		game->size = v2(platform->windowWidth, platform->windowHeight);

		maximizeWindow();
	}

	Globals *globals = &game->globals;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;
	Map *map = &game->map;

	game->screenMouse = (platform->mouse - game->screenOverlayOffset) * (game->size/game->screenOverlaySize);

	{ /// Resizing
		Vec2 ratio = v2(1600.0, 900.0);
		game->sizeScale = MinNum(platform->windowWidth/ratio.x, platform->windowHeight/ratio.y);
		Vec2 newSize = ratio * game->sizeScale;

		if (!equal(game->size, newSize)) {
			game->size = newSize;

			if (game->gameTexture) destroyTexture(game->gameTexture);
			game->gameTexture = NULL;

			game->screenOverlaySize = game->size;
			game->screenOverlayOffset.x = (float)platform->windowWidth/2 - game->size.x/2;
			game->screenOverlayOffset.y = (float)platform->windowHeight/2 - game->size.y/2;
		}
	}

	if (!game->gameTexture) game->gameTexture = createTexture(game->size.x, game->size.y);
	pushTargetTexture(game->gameTexture);
	clearRenderer(0xFF2E5228);

	if (game->state != game->nextState) {
		game->stateTransition_t += 0.05;
		if (game->stateTransition_t >= 1) game->state = game->nextState;
	} else {
		game->stateTransition_t -= 0.05;
	}
	game->stateTransition_t = Clamp01(game->stateTransition_t);

	if (game->prevState != game->state) {
		game->prevState = game->state;
		game->stateTime = 0;
	}

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;

	if (game->state == GAME_NONE) {
		game->nextState = GAME_PLAY;

	} else if (game->state == GAME_PLAY) {
		Matrix3 cameraMatrix = mat3();
		cameraMatrix.TRANSLATE(-game->cameraTarget);
		pushCamera2d(cameraMatrix);

		game->worldMouse = cameraMatrix.invert() * game->screenMouse;

		if (game->stateTime == 0) {
			map->size = v2(5000, 5000);
			for (int i = 0; i < 200; i++) {
				Actor *actor = createActor(ACTOR_FLOOR_TEXTURE);
				actor->position.x = rndFloat(0, map->size.x);
				actor->position.y = rndFloat(0, map->size.y);
				strcpy(actor->texturePath, "assets/images/rock.png");
			}

			for (int i = 0; i < 2; i++) {
				Actor *actor = createActor(ACTOR_SLASHER);
				actor->position.x = rndFloat(0, map->size.x);
				actor->position.y = rndFloat(0, map->size.y);
			}

			for (int i = 0; i < 1; i++) {
				Actor *actor = createActor(ACTOR_DASHER);
				actor->position.x = rndFloat(0, map->size.x);
				actor->position.y = rndFloat(0, map->size.y);
			}

			for (int i = 0; i < 1; i++) {
				Actor *actor = createActor(ACTOR_SHOOTER);
				actor->position.x = rndFloat(0, map->size.x);
				actor->position.y = rndFloat(0, map->size.y);
			}

			{
				// Actor *actor = createActor(ACTOR_DASHER);
				// actor->position.x = rndFloat(0, map->size.x);
				// actor->position.y = rndFloat(0, map->size.y);
			}
		}

		Actor *player = NULL;
		/// Initial actor update
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->type == ACTOR_PLAYER) player = actor;

			actor->hitBySlashThisFrame = false;
			actor->circle = makeCircle(actor->position, MaxNum(actor->info->size.x, actor->info->size.y)/2);
			actor->rect = makeRect(actor->position - actor->info->size/2, actor->info->size);
		}

		if (!player) {
			player = createActor(ACTOR_PLAYER);
			if (!player) Panic("No room for player??");

			player->position = map->size/2;
		}

		/// Update actors
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];

			Vec2 pathingTarget = v2();
			float pathingSpeed = 0;

			Vec2 playerVector = vectorBetween(actor->position, player->position);

			float playerDistance = distance(player->position, actor->position);
			playerDistance -= player->circle.radius;
			playerDistance -= actor->circle.radius;
			if (playerDistance < 0) playerDistance = 0;

			if (actor->prevPhaseTime != actor->phase) {
				actor->prevPhaseTime = actor->phase;
				actor->phaseTime = 0;
			}

			if (actor->type == ACTOR_PLAYER) {
				if (!game->inEditor) game->cameraTarget = lerp(game->cameraTarget, actor->position - game->size/2, 0.5);

				Vec2 input = v2();
				if (keyPressed('W')) input.y--;
				if (keyPressed('S')) input.y++;
				if (keyPressed('A')) input.x--;
				if (keyPressed('D')) input.x++;
				input = input.normalize();

				actor->velo += input * 3;

				if (platform->mouseJustDown && actor->timeSinceLastAttacked > 0.3) {
					actor->timeSinceLastAttacked = 0;

					float facingRads = radsBetween(player->position, game->worldMouse);

					Bullet *bullet = createBullet(BULLET_SLASH, actor->position, toDeg(facingRads));
					bullet->ally = true;
					bullet->slashDistance = 80;
					bullet->slashSpread = 120;
					bullet->slashSizeMinMax = v2(40, 50);
				}
			} else if (actor->type == ACTOR_BITER) {
				pathingTarget = player->position;
				pathingSpeed = 3;
			} else if (actor->type == ACTOR_SLASHER) {
				actor->stamina += 0.1;

				if (actor->phase == 0) {
					float wantedDist = 100;
					if (fabs(playerDistance - wantedDist) > 10) {
						if (playerDistance > wantedDist) actor->velo += playerVector * 2;
						else actor->velo += playerVector * -2.5;
					}

					if (actor->stamina > 40 && actor->phaseTime > 1) {
						actor->stamina -= 40;
						actor->phase = 1;
					}
				} else if (actor->phase == 1) {
					if (playerDistance > 1) {
						actor->velo += playerVector * 2;
					} else {
						Bullet *bullet = createBullet(BULLET_SLASH, actor->position, toDeg(playerVector));
						bullet->slashDistance = 80;
						bullet->slashSpread = 120;

						actor->phase = 0;
					}
				}

			} else if (actor->type == ACTOR_DASHER) {

				int MAKE_SPACE = 0;
				int CHARGE = 1;
				int DASHING = 2;

				if (actor->phase == MAKE_SPACE) {
					float wantedDist = 300;
					if (fabs(playerDistance - wantedDist) > 10) {
						if (playerDistance > wantedDist) actor->velo += playerVector * 2;
						else actor->velo += playerVector * clampMap(actor->phaseTime, 0, 1, -2.5, -1.0);
					}

					if (playerDistance < 100 && actor->timeSinceLastAttacked > 0.3) {
						actor->enemyTimer0 += elapsed;
						if (actor->enemyTimer0 > 0.5) {
							actor->enemyTimer0 = 0;
							actor->phaseTime = 0;

							Bullet *bullet = createBullet(BULLET_SLASH, actor->position, toDeg(playerVector));
							bullet->slashDistance = 80;
							bullet->slashSpread = 120;
							actor->timeSinceLastAttacked = 0;
							logf("Struck\n");
						}
					} else {
						actor->enemyTimer0 = 0;
					}

					if (actor->phaseTime > 3) actor->phase = CHARGE;
				} else if (actor->phase == CHARGE) {
					if (actor->phaseTime == 0) {
						actor->force += playerVector * -5;
					}

					if (actor->phaseTime > 0.5) {
						actor->force += playerVector * 20;
						actor->phase = DASHING;
					}
				} else if (actor->phase == DASHING) {
					if (playerDistance < 5 && actor->timeSinceLastAttacked > 0.3) {
						Bullet *bullet = createBullet(BULLET_SLASH, actor->position, toDeg(playerVector));
						bullet->slashDistance = 80;
						bullet->slashSpread = 120;
						actor->timeSinceLastAttacked = 0;
						logf("D Struck\n");
					}

					if (actor->force.length() < 1) actor->phase = MAKE_SPACE;
				}

			} else if (actor->type == ACTOR_SHOOTER) {
				if (actor->phase == 0) {
					float wantedDist = 400;
					if (fabs(playerDistance - wantedDist) > 10) {
						if (playerDistance > wantedDist) actor->velo += playerVector * 5;
						else actor->velo += playerVector * -2;
					}

					if (actor->phaseTime > 5) {
						Bullet *bullet = createBullet(BULLET_SLASH, actor->position, toDeg(playerVector));
						bullet->slashDistance = 80;
						bullet->slashSpread = 120;

						actor->prevPhaseTime = -1;
					}
				}
			}

			if (pathingSpeed) {
				Vec2 between = vectorBetween(actor->position, pathingTarget);
				actor->velo += between * pathingSpeed;
			}

			actor->force *= 0.95;
			actor->velo += actor->force;
			if (!isZero(actor->velo)) {
				float hitSlowMulti = clampMap(actor->timeSinceLastHit, 0, 0.5, 0.7, 1);
				actor->velo *= hitSlowMulti;
				actor->velo *= 0.75;
				actor->position += actor->velo;
			}

			if (actor->info->hasSoftCollision) {
				for (int i = 0; i < map->actorsNum; i++) {
					Actor *otherActor = &map->actors[i];
					if (actor == otherActor) continue;
					if (!otherActor->info->hasSoftCollision) continue;

					if (contains(actor->circle, otherActor->circle)) {
						Vec2 between = vectorBetween(actor->position, otherActor->position);
						actor->velo += between * -1;
						otherActor->velo += between * 1;
					}
				}
			}

			if (actor->hp <= 0) {
				actor->markedForDeletion = true;
			}

			actor->stamina = mathClamp(actor->stamina, 0, 100);

			actor->timeSinceLastAttacked += elapsed;
			actor->timeSinceLastHit += elapsed;
			actor->phaseTime += elapsed;
		}

		/// Draw actors
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];

			{ // Regenerate these so they're up to date for drawing
				actor->circle = makeCircle(actor->position, MaxNum(actor->info->size.x, actor->info->size.y)/2);
				actor->rect = makeRect(actor->position - actor->info->size/2, actor->info->size);
			}

			if (actor->type == ACTOR_PLAYER) {
				drawCircle(actor->circle, 0xFF0000FF);

			} else if (actor->type == ACTOR_FLOOR_TEXTURE) {
				Texture *texture = getTexture(actor->texturePath);
				if (texture) {
					RenderProps props = newRenderProps();
					props.matrix.TRANSLATE(actor->position);
					drawTexture(texture, props);
				} else {
					logf("Missing texture %s\n", actor->texturePath);
				}
			} else if (actor->type == ACTOR_BITER) {
				drawCircle(actor->circle, 0xFF300000);
			} else if (actor->type == ACTOR_SLASHER) {
				drawCircle(actor->circle, 0xFF400000);
			} else if (actor->type == ACTOR_DASHER) {
				drawCircle(actor->circle, 0xFF404000);
			} else if (actor->type == ACTOR_SHOOTER) {
				drawCircle(actor->circle, 0xFF004040);
			} else {
				drawCircle(actor->circle, 0xFFFFFF00);
			}

			if (actor->info->hittable && actor->hp != actor->info->hp) {
				Rect bgRect = makeRect(0, 0, actor->info->size.x, 8);
				bgRect.x = actor->position.x - bgRect.width/2;
				bgRect.y = actor->position.y + actor->info->size.y/2 + 4;
				Rect barRect = bgRect;
				barRect.width *= actor->hp / actor->info->hp;

				drawRect(barRect, 0xFFFF0000);
				drawRectOutline(bgRect, 1, 0xFF800000);
			}

			if (actor->stamina != actor->info->stamina) {
				Rect bgRect = makeRect(0, 0, actor->info->size.x, 8);
				bgRect.x = actor->position.x - bgRect.width/2;
				bgRect.y = actor->position.y + actor->info->size.y/2 + 15;
				Rect barRect = bgRect;
				barRect.width *= actor->stamina / actor->info->stamina;

				drawRect(barRect, 0xFF0000FF);
				drawRectOutline(bgRect, 1, 0xFF000080);
			}
		}

		/// Update bullets
		for (int i = 0; i < game->bulletsNum; i++) {
			Bullet *bullet = &game->bullets[i];
			bool shouldDestroy = false;

			if (bullet->type == BULLET_SLASH) {
				shouldDestroy = true;

#if 0
				float ringPerc = norm(0, 360, bullet->slashSpread);
				Texture *texture = getTexture("assets/images/slashCircle.png");
				RenderProps props = newRenderProps();
				props.matrix.TRANSLATE(bullet->position - bullet->slashDistance/2);
				props.matrix.TRANSLATE(bullet->slashDistance/2);
				props.matrix.ROTATE(bullet->rotation);
				props.matrix.TRANSLATE(-bullet->slashDistance/2);
				props.matrix.SCALE(bullet->slashDistance);
				props.params.x = ringPerc;
				props.flags |= _F_ARC_V2;
				props.srcWidth = props.srcHeight = 1;
				drawTexture(texture, props);
#else
				int circleCount = 10;
				for (int i = 0; i < circleCount; i++) {
					float perc = (float)i/(circleCount-1);

					Vec2 pos = bullet->position;
					float rads = toRad(lerp(bullet->slashSpread, 0, perc)) + toRad(bullet->rotation) -  toRad(bullet->slashSpread/2);

					pos.x += cos(rads) * bullet->slashDistance;
					pos.y += sin(rads) * bullet->slashDistance;

					float radiusPerc = clampMap(perc, 0, 0.5, 0, 1) * clampMap(perc, 0.5, 1, 1, 0);
					float radius = lerp(bullet->slashSizeMinMax.x, bullet->slashSizeMinMax.y, radiusPerc);
					Circle hitCircle = makeCircle(pos, radius);
					drawCircle(hitCircle, 0x80FF0000);

					for (int i = 0; i < map->actorsNum; i++) {
						Actor *actor = &map->actors[i];
						if (actor->hitBySlashThisFrame) continue;
						if (!actor->info->hittable) continue;
						if (bullet->ally == actor->info->ally) continue;

						if (contains(hitCircle, actor->circle)) {
							actor->hitBySlashThisFrame = true;
							actor->timeSinceLastHit = 0;
							Vec2 between = vectorBetween(bullet->position, actor->position);
							actor->velo += between * 20;
							actor->hp -= 10;
						}
					}
				}
#endif
			}

			if (shouldDestroy) {
				arraySpliceIndex(game->bullets, game->bulletsNum, sizeof(Bullet), i);
				game->bulletsNum--;
				i--;
				continue;
			}
		}

		/// Delete actors
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->markedForDeletion) {
				deleteActor(actor);
				i--;
				continue;
			}
		}

		popCamera2d();
	}

	game->stateTime += elapsed;

	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, game->stateTransition_t));
	drawCircle(game->screenMouse, 10, 0xFFFF0000);
	popTargetTexture(); // game->gameTexture

	clearRenderer();

	{
		Texture *texture = game->gameTexture;
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(game->screenOverlaySize);

		drawSimpleTexture(texture, matrix);
	}

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) {
		char *str = frameSprintf("%.1fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
	}

	guiDraw();
	drawOnScreenLog();

	game->time += elapsed;
}

Actor *createActor(ActorType type) {
	Map *map = &game->map;
	if (map->actorsNum > ACTORS_MAX-1) return NULL;

	Actor *actor = &map->actors[map->actorsNum++];
	memset(actor, 0, sizeof(Actor));
	actor->type = type;
	actor->info = &game->actorTypeInfos[type];
	actor->hp = actor->info->hp;
	actor->stamina = actor->info->stamina;

	return actor;
}

void deleteActor(Actor *actor) {
	Map *map = &game->map;

	for (int i = 0; i < map->actorsNum; i++) {
		if (&map->actors[i] == actor) {
			arraySpliceIndex(map->actors, map->actorsNum, sizeof(Actor), i);
			map->actorsNum--;
			i--;
			return;
		}
	}

	logf("Couldn't delete actor, because it's not on this map...\n");
}

Bullet *createBullet(BulletType type, Vec2 position, float rotation) {
	if (game->bulletsNum > BULLETS_MAX-1) game->bulletsNum--;
	Bullet *bullet = &game->bullets[game->bulletsNum++];
	memset(bullet, 0, sizeof(Bullet));
	bullet->position = position;
	bullet->rotation = rotation;
	bullet->type = type;
	bullet->slashSizeMinMax = v2(20, 30);
	return bullet;
}
