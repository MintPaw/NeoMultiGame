void initCore();
void stepGame(float elapsed);

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
void dealDamage(Actor *dest, float amount, float shieldDamageMulti, float armorDamageMulti, float hpDamageMulti, bool noCoreEvent=false);
void dealDamage(Actor *bullet, Actor *dest);
Rect getRect(Actor *actor);
Vec2 getFlowDirForRect(Rect rect);
float getRange(ActorType actorType, Vec2i tilePos);
float getRange(Actor *actor, Vec2i tilePos);
float getDamage(Actor *actor);
float getRpm(Actor *actor);
int getMaxLevel(ActorType actorType);
ActorTypeInfo *getInfo(Actor *actor);

Upgrade *getUpgrade(int id);
bool hasUpgrade(int id);
bool hasUpgradeEffect(UpgradeEffectType effectType, ActorType actorType);

Actor **getActorsInRange(Circle range, int *outNum, bool enemiesOnly);
Actor **getActorsInRange(Tri2 range, int *outNum, bool enemiesOnly);
void startNextWave();
Tri2 getAttackTri(Vec2 start, float range, float angle, float deviation);

CoreEvent *createCoreEvent(CoreEventType type, Actor *src=NULL, Actor *dest=NULL);

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

void initCore() {
	{ /// Setup actor type infos
		for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
			ActorTypeInfo *info = &game->actorTypeInfos[i];
			sprintf(info->name, "Actor %d", i);
			info->size = v3(TILE_SIZE*0.5, TILE_SIZE*0.5, TILE_SIZE*0.5);
			info->primaryColor = 0xFF000000;
		}

		for (int i = ACTOR_BALLISTA; i <= ACTOR_PARTICLE_CANNON; i++) {
			ActorTypeInfo *info = &game->actorTypeInfos[i];
			info->isTower = true;
			info->size = v3(TILE_SIZE*0.75, TILE_SIZE*0.75, TILE_SIZE*0.75);
		}

		for (int i = ACTOR_ENEMY1; i <= ACTOR_ENEMY64; i++) {
			ActorTypeInfo *info = &game->actorTypeInfos[i];
			info->isEnemy = true;
			info->size = v3(TILE_SIZE*0.25, TILE_SIZE*0.25, TILE_SIZE*0.25);
		}

		ActorTypeInfo *info = NULL;

		info = &game->actorTypeInfos[ACTOR_BALLISTA];
		strncpy(info->name, "Ballista", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 5;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 5;
		info->shieldDamageMulti = 5;
		info->baseRange = 5 * TILE_SIZE;
		info->rpm = 20;
		info->mana = 0;
		info->price = 10;
		info->priceMulti = 15;
		info->primaryColor = 0xFF800000;

		info = &game->actorTypeInfos[ACTOR_MORTAR_TOWER];
		strncpy(info->name, "Mortar", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 10;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 15;
		info->shieldDamageMulti = 5;
		info->baseRange = 10 * TILE_SIZE;
		info->rpm = 10;
		info->mana = 0;
		info->price = 200;
		info->priceMulti = 75;
		info->primaryColor = 0xFF525252;

		info = &game->actorTypeInfos[ACTOR_TESLA_COIL];
		strncpy(info->name, "Tesla Coil", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 10;
		info->hpDamageMulti = 6;
		info->armorDamageMulti = 3;
		info->shieldDamageMulti = 9;
		info->baseRange = 1.5 * TILE_SIZE;
		info->rpm = 30;
		info->mana = 5;
		info->price = 200;
		info->priceMulti = 75;
		info->primaryColor = 0xFFA0A0F0;

		info = &game->actorTypeInfos[ACTOR_FROST_KEEP];
		strncpy(info->name, "Frost Keep", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 5;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 5;
		info->shieldDamageMulti = 5;
		info->baseRange = 2 * TILE_SIZE;
		info->rpm = 180;
		info->mana = 0.3;
		info->price = 250;
		info->priceMulti = 100;
		info->primaryColor = 0xFFE3F0F5;

		info = &game->actorTypeInfos[ACTOR_FLAME_THROWER];
		strncpy(info->name, "Flame Thrower", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 5;
		info->hpDamageMulti = 6;
		info->armorDamageMulti = 9;
		info->shieldDamageMulti = 3;
		info->baseRange = 4 * TILE_SIZE;
		info->rpm = 60;
		info->mana = 1;
		info->price = 300;
		info->priceMulti = 75;
		info->primaryColor = lerpColor(BURN_COLOR, 0xFF000000, 0.75);

		info = &game->actorTypeInfos[ACTOR_POISON_SPRAYER];
		strncpy(info->name, "Poison Sprayer", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 5;
		info->hpDamageMulti = 6;
		info->armorDamageMulti = 3;
		info->shieldDamageMulti = 9;
		info->baseRange = 4 * TILE_SIZE;
		info->rpm = 60;
		info->mana = 1;
		info->price = 300;
		info->priceMulti = 75;
		info->primaryColor = lerpColor(POISON_COLOR, 0xFF000000, 0.75);

		info = &game->actorTypeInfos[ACTOR_SHREDDER];
		strncpy(info->name, "Shredder", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 10;
		info->hpDamageMulti = 20;
		info->armorDamageMulti = 10;
		info->shieldDamageMulti = 10;
		info->baseRange = 5 * TILE_SIZE;
		info->rpm = 5;
		info->price = 500;
		info->priceMulti = 100;
		info->primaryColor = 0xFF800000;

		info = &game->actorTypeInfos[ACTOR_ENCAMPENT];
		strncpy(info->name, "Encampent", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 20;
		info->hpDamageMulti = 10;
		info->armorDamageMulti = 15;
		info->shieldDamageMulti = 5;
		info->baseRange = 2 * TILE_SIZE;
		info->rpm = 5;
		info->price = 500;
		info->priceMulti = 100;

		info = &game->actorTypeInfos[ACTOR_LOOKOUT];
		strncpy(info->name, "Lookout", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 1;
		info->hpDamageMulti = 2;
		info->armorDamageMulti = 1;
		info->shieldDamageMulti = 3;
		info->baseRange = 8 * TILE_SIZE;
		info->rpm = 0;
		info->price = 500;
		info->priceMulti = 100;

		info = &game->actorTypeInfos[ACTOR_RADAR];
		strncpy(info->name, "Radar", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 20;
		info->hpDamageMulti = 20;
		info->armorDamageMulti = 10;
		info->shieldDamageMulti = 10;
		info->baseRange = 30 * TILE_SIZE;
		info->rpm = 700;
		info->price = 1000;
		info->priceMulti = 250;

		info = &game->actorTypeInfos[ACTOR_OBELISK];
		strncpy(info->name, "Obelisk", ACTOR_TYPE_NAME_MAX_LEN);
		info->damage = 8;
		info->hpDamageMulti = 5;
		info->armorDamageMulti = 10;
		info->shieldDamageMulti = 2;
		info->baseRange = 5 * TILE_SIZE;
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
		info->baseRange = 20 * TILE_SIZE;
		info->rpm = 360;
		info->mana = 12;
		info->price = 1000;
		info->priceMulti = 250;

		info = &game->actorTypeInfos[ACTOR_MANA_SIPHON];
		strncpy(info->name, "Mana Siphon", ACTOR_TYPE_NAME_MAX_LEN);
		info->price = 100;
		info->priceMulti = 10;
		info->primaryColor = 0xFFA4CCC8;

		info = &game->actorTypeInfos[ACTOR_MANA_CRYSTAL];
		strncpy(info->name, "Mana Crystal", ACTOR_TYPE_NAME_MAX_LEN);
		info->primaryColor = 0xFFA4B0CC;

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
		info->size = v3(0.1, 0.1, 0.1) * TILE_SIZE;
		info->bulletSpeed = 20;

		info = &game->actorTypeInfos[ACTOR_MORTAR];
		info->bulletSpeed = 0.5;
		info->baseRange = 2 * TILE_SIZE;
	} ///

	{ /// Setup upgrades
		auto createUpgrade = []() {
			if (game->upgradesNum > UPGRADES_MAX-1) Panic("Too many upgrades");

			Upgrade *upgrade = &game->upgrades[game->upgradesNum++];
			memset(upgrade, 0, sizeof(Upgrade));
			upgrade->id = ++game->nextUpgradeId;
			return upgrade;
		};

		Upgrade *upgrade = NULL;

		ActorType actorsCouldUpgrade[] = {
			ACTOR_BALLISTA, ACTOR_MORTAR_TOWER, ACTOR_TESLA_COIL, ACTOR_FROST_KEEP, ACTOR_FLAME_THROWER, ACTOR_POISON_SPRAYER, ACTOR_SHREDDER,
		};
		// ACTOR_ENCAMPENT, ACTOR_LOOKOUT, ACTOR_RADAR, ACTOR_OBELISK, ACTOR_PARTICLE_CANNON,

		game->upgradesNum = 0;
		game->nextUpgradeId = 0;
		for (int i = 0; i < ArrayLength(actorsCouldUpgrade); i++) {
			ActorType actorType = actorsCouldUpgrade[i];

			Upgrade *unlockUpgrade = NULL;
			if (actorType != ACTOR_BALLISTA) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_UNLOCK;
				effect->actorType = actorType;
				unlockUpgrade = upgrade;
			}

			Upgrade *prevUpgrade = NULL;

			prevUpgrade = unlockUpgrade;
			for (int i = 0; i < 3; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_DAMAGE_MULTI;
				effect->actorType = actorType;
				effect->value = 1 + (0.1 * (i+1));
				if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}

			prevUpgrade = unlockUpgrade;
			for (int i = 0; i < 3; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_RANGE_MULTI;
				effect->actorType = actorType;
				effect->value = 1 + (0.1 * (i+1));
				if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}

			prevUpgrade = unlockUpgrade;
			for (int i = 0; i < 3; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_RPM_MULTI;
				effect->actorType = actorType;
				effect->value = 1 + (0.1 * (i+1));
				if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}
		}

		{
			Upgrade *prevUpgrade = NULL;
			for (int i = 0; i < 3; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_EXTRA_CARDS;
				effect->value = 1;
				if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}
		}

		{
			Upgrade *prevUpgrade = NULL;
			for (int i = 0; i < 5; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_EXTRA_MONEY;
				effect->value = i+1;
				if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}
		}

		{
			Upgrade *prevUpgrade = NULL;
			for (int i = 0; i < 3; i++) {
				Upgrade *upgrade = createUpgrade();
				UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
				effect->type = UPGRADE_EFFECT_MANA_GAIN_MULTI;
				effect->value = 1.1;
				if (prevUpgrade) upgrade->prereqs[upgrade->prereqsNum++] = prevUpgrade->id;
				prevUpgrade = upgrade;
			}
		}

		{
			Upgrade *upgrade = createUpgrade();
			UpgradeEffect *effect = &upgrade->effects[upgrade->effectsNum++];
			effect->type = UPGRADE_EFFECT_UNLOCK;
			effect->actorType = ACTOR_MANA_SIPHON;
		}
	} ///
}

void stepGame(float elapsed) {
	float timeScale = elapsed / (1/60.0);
	game->coreEventsNum = 0;

	World *world = data->world;

	memset(game->actorTypeCounts, 0, sizeof(int) * ACTOR_TYPES_MAX);
	for (int i = 0; i < world->actorsNum; i++) {
		Actor *actor = &world->actors[i];
		game->actorTypeCounts[actor->type]++;
	}

	game->manaToGain = 1 * elapsed;
	int enemiesAlive = 0;
	{ /// Update actors
		for (int i = 0; i < world->actorsNum; i++) {
			Actor *actor = &world->actors[i];
			ActorTypeInfo *info = &game->actorTypeInfos[actor->type];

			actor->movementSpeed = info->movementSpeed;
			actor->movementSpeed *= clampMap(actor->slow, 0, 10, 1, 0.4);

			actor->slow -= 6*elapsed;

			if (actor->poison) {
				float toInflict = MinNum(actor->poison, 24*elapsed);
				actor->poison -= toInflict;
				if (actor->poison < 0) actor->poison = 0;

				dealDamage(actor, toInflict, 2, 0.5, 0.5, true);
			}

			if (actor->burn) {
				float toInflict = MinNum(actor->burn, 24*elapsed);
				actor->burn -= toInflict;
				if (actor->burn < 0) actor->burn = 0;

				dealDamage(actor, toInflict, 0.5, 2, 0.5, true);
			}

			if (actor->bleed) {
				float toInflict = MinNum(actor->bleed, 24*elapsed);
				actor->bleed -= toInflict;
				if (actor->bleed < 0) actor->bleed = 0;

				dealDamage(actor, toInflict, 0.5, 0.5, 2, true);
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
					Circle range = makeCircle(actor->position, getRange(actor, worldToTile(actor->position)));

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(range, &enemiesInRangeNum, true);

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
					if (target) {
						actor->aimTarget = target->id;
						actor->aimRads = radsBetween(actor->position, target->position);
					}
				}

				bool isActive = true;
				if (!data->playingWave && !towerIsActiveBetweenWaves) isActive = false;
				if (!target && towerCaresAboutTargets) isActive = false;

				actor->timeTillNextShot -= elapsed;
				if (isActive) {
					if (actor->timeTillNextShot < 0) {
						float rpm = getRpm(actor);
						actor->timeTillNextShot = 1.0/(rpm/60.0);

						float manaCost = info->mana;
						if (data->mana > manaCost) {
							data->mana -= manaCost;
							actor->timeSinceLastShot = 0;
							towerShouldFire = true;
							createCoreEvent(CORE_EVENT_SHOOT, actor);
						}
					}

					if (actor->level < getMaxLevel(actor->type)) {
						actor->xp += XP_PER_SEC * elapsed;
						if (actor->xp > maxXpPerLevels[actor->level]) {
							actor->level++;
							actor->xp = 0;
						}
					}
				}
			}

			if (info->isTower || actor->type == ACTOR_MANA_SIPHON) {
				if (data->tool == TOOL_NONE || data->tool == TOOL_SELECTED) {
					if (isMouseClicked() && isHoveringActor(actor)) {
						data->prevTool = TOOL_NONE;
						data->tool = TOOL_SELECTED;
						data->selectedActorsNum = 0;
						data->selectedActors[data->selectedActorsNum++] = actor->id;
					}
				}
			}

			if (actor->type == ACTOR_BALLISTA) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}
			} else if (actor->type == ACTOR_MORTAR_TOWER) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}
			} else if (actor->type == ACTOR_TESLA_COIL) {
				if (towerShouldFire) {
					Circle circle = makeCircle(actor->position, getRange(actor, worldToTile(actor->position)));
					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(circle, &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						dealDamage(actor, enemy);
					}
				}
			} else if (actor->type == ACTOR_FROST_KEEP) {
				if (towerShouldFire) {
					float range = getRange(actor, worldToTile(actor->position));
					Vec2i towerTilePos = worldToTile(actor->position);
					Vec2i min = towerTilePos;
					min.x -= (range / TILE_SIZE);
					min.y -= (range / TILE_SIZE);

					Vec2i max = towerTilePos;
					max.x += (range / TILE_SIZE);
					max.y += (range / TILE_SIZE);

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
			} else if (actor->type == ACTOR_FLAME_THROWER) {
				if (towerShouldFire) {
					float range = getRange(actor, worldToTile(actor->position));
					Tri2 tri = getAttackTri(actor->position, range, actor->aimRads, FLAME_RADS);

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(tri, &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						enemy->burn += getDamage(actor);
					}
				}
			} else if (actor->type == ACTOR_POISON_SPRAYER) {
				if (towerShouldFire) {
					float range = getRange(actor, worldToTile(actor->position));
					Tri2 tri = getAttackTri(actor->position, range, actor->aimRads, FLAME_RADS);

					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(tri, &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						enemy->poison += getDamage(actor);
					}
				}
			} else if (actor->type == ACTOR_SHREDDER) {
				if (towerShouldFire) {
					Actor *bullet = createBullet(actor, target);
				}
			} else if (actor->type == ACTOR_MANA_SIPHON) {
				int count = 0;
				Circle range = makeCircle(actor->position, TILE_SIZE);
				int actorsInRangeNum = 0;
				Actor **actorsInRange = getActorsInRange(range, &actorsInRangeNum, false);
				for (int i = 0; i < actorsInRangeNum; i++) {
					Actor *other = actorsInRange[i];
					if (other->type == ACTOR_MANA_CRYSTAL) count++;
				}

				game->manaToGain += (float)count * elapsed;
			} else if (actor->type == ACTOR_MANA_CRYSTAL) {
			} else if (actor->type >= ACTOR_ENEMY1 && actor->type <= ACTOR_ENEMY64) {
				enemiesAlive++;

				Vec2 dir = getFlowDirForRect(getRect(actor));

				actor->accel = dir * (actor->movementSpeed * elapsed) * 5;

				Vec2i goal = v2i(CHUNK_SIZE/2, CHUNK_SIZE/2);
				Rect goalRect = tileToWorldRect(goal);
				if (overlaps(getRect(actor), goalRect)) {
					data->hp--;
					actor->markedForDeletion = true;
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
			} else if (actor->type == ACTOR_MORTAR) {
				if (actor->time == 0) {
					Actor *target = getActor(actor->bulletTarget);
					if (target) actor->position = target->position;
				}

				float delayTime = info->bulletSpeed;
				float explodeRange = info->baseRange;
				if (actor->time >= delayTime) {
					int enemiesInRangeNum = 0;
					Actor **enemiesInRange = getActorsInRange(makeCircle(actor->position, explodeRange), &enemiesInRangeNum, true);
					for (int i = 0; i < enemiesInRangeNum; i++) {
						Actor *enemy = enemiesInRange[i];
						dealDamage(actor, enemy);
					}

					actor->markedForDeletion = true;

					createCoreEvent(CORE_EVENT_MORTAR_EXPLOSION, actor);
				}
			} else if (actor->type == ACTOR_FROST) {
				int maxTime = 10;
				float fallSpeedPerSec = FROST_FALL_DISTANCE / (float)maxTime;
				actor->position.y += fallSpeedPerSec * elapsed;

				Circle range = makeCircle(actor->position, getRect(actor).width/2);

				int enemiesInRangeNum = 0;
				Actor **enemiesInRange = getActorsInRange(range, &enemiesInRangeNum, true);
				for (int i = 0; i < enemiesInRangeNum; i++) {
					Actor *enemy = enemiesInRange[i];
					if (overlaps(getRect(enemy), getRect(actor))) {
						enemy->slow += 6;
						actor->markedForDeletion = true;
					}
				}

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
					Actor **enemiesInRange = getActorsInRange(circle, &enemiesInRangeNum, true);
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
			}

			Vec2 damping = v2(0.1, 0.1);
			actor->velo += (actor->accel - damping*actor->velo) * timeScale;
			actor->accel = v2();

			actor->position += actor->velo * timeScale;

			if (actor->hp <= 0 && info->maxHp > 0) actor->markedForDeletion = true;

			actor->time += elapsed;
		}
	} ///

	{ /// Post update actors
		for (int i = 0; i < world->actorsNum; i++) {
			Actor *actor = &world->actors[i];
			actor->timeSinceLastShot += elapsed;

			if (actor->markedForDeletion) {
				ActorTypeInfo *info = &game->actorTypeInfos[actor->type];
				if (info->isEnemy) {
					int moneyToGain = 0;
					if (actor->type == ACTOR_ENEMY1) {
						moneyToGain += 4; 
					} else {
						moneyToGain += info->enemySpawnStartingWave;
					}

					for (int i = 0; i < data->ownedUpgradesNum; i++) {
						Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]);
						for (int i = 0; i < upgrade->effectsNum; i++) {
							UpgradeEffect *effect = &upgrade->effects[i];
							if (effect->type == UPGRADE_EFFECT_EXTRA_MONEY) moneyToGain += effect->value;
						}
					}

					data->money += moneyToGain;
				}

				deinitActor(actor);
				arraySpliceIndex(world->actors, world->actorsNum, sizeof(Actor), i);
				world->actorsNum--;
				i--;
				continue;
			}
		}
	} ///

	if (data->playingWave) {
		{ /// Spawn enemies
			if (data->actorsToSpawnNum > 0) data->timeTillNextSpawn -= elapsed;
			if (data->timeTillNextSpawn <= 0) {
				data->timeTillNextSpawn += 1;

				Vec2 *spawnPoints = (Vec2 *)frameMalloc(sizeof(Vec2) * CHUNKS_MAX);
				int spawnPointsNum = 0;
				for (int i = 0; i < world->chunksNum; i++) {
					Chunk *chunk = &world->chunks[i];

					bool isPortal = false; //@copyPastedIsPortal
					if (chunk->connectionsNum == 1 && !isZero(chunk->position)) isPortal = true;

					if (!chunk->visible) continue;
					if (isPortal) {
						spawnPoints[spawnPointsNum++] = tileToWorld(chunkTileToWorldTile(chunk, v2i(CHUNK_SIZE/2, CHUNK_SIZE/2)));
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
					ActorType toSpawn = data->actorsToSpawn[0];
					Actor *actor = createActor(toSpawn);
					actor->position = spawnPoints[i];

					memmove(&data->actorsToSpawn[0], &data->actorsToSpawn[1], sizeof(ActorType) * (data->actorsToSpawnNum-1));
					data->actorsToSpawnNum--;
					if (data->actorsToSpawnNum == 0) break;
				}
			}
		} ///

		if (data->waveTime > 1 && enemiesAlive == 0 && data->actorsToSpawnNum == 0) { /// End wave
			data->playingWave = false;

			int *possible = (int *)frameMalloc(sizeof(int) * UPGRADES_MAX);
			int possibleNum = 0;
			for (int i = 0; i < game->upgradesNum; i++) {
				Upgrade *upgrade = &game->upgrades[i];
				if (hasUpgrade(upgrade->id)) continue;

				bool hasPrereqs = true;
				for (int i = 0; i < upgrade->prereqsNum; i++) {
					if (!hasUpgrade(upgrade->prereqs[i])) hasPrereqs = false;
				}
				if (!hasPrereqs) continue;

				possible[possibleNum++] = upgrade->id;
			}

			int maxUpgradeCards = 3;
			for (int i = 0; i < data->ownedUpgradesNum; i++) {
				Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]);
				for (int i = 0; i < upgrade->effectsNum; i++) {
					UpgradeEffect *effect = &upgrade->effects[i];
					if (effect->type == UPGRADE_EFFECT_EXTRA_CARDS) maxUpgradeCards += effect->value;
				}
			}

			game->presentedUpgradesNum = 0;
			for (int i = 0; i < maxUpgradeCards; i++) {
				if (possibleNum == 0) continue;
				int chosenIndex = rndInt(0, possibleNum-1);
				game->presentedUpgrades[game->presentedUpgradesNum++] = possible[chosenIndex];
				arraySpliceIndex(possible, possibleNum, sizeof(int), chosenIndex);
				possibleNum--;
			}
		} ///

		data->waveTime += elapsed;
	}

	{ /// Gain mana
		for (int i = 0; i < data->ownedUpgradesNum; i++) {
			Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]);
			for (int i = 0; i < upgrade->effectsNum; i++) {
				UpgradeEffect *effect = &upgrade->effects[i];
				if (effect->type == UPGRADE_EFFECT_MANA_GAIN_MULTI) game->manaToGain *= effect->value;
			}
		}

		if (data->playingWave) data->mana += game->manaToGain;
		if (data->mana > data->maxMana) data->mana = data->maxMana;
	} ///

	{ /// Update tool
		if (data->prevTool != data->tool) {
			data->prevTool = data->tool;
			data->toolTime = 0;
		}

		if (data->tool == TOOL_NONE) {
			/// Nothing...
		} else if (data->tool == TOOL_BUILDING) {
			if (platform->rightMouseDown) data->tool = TOOL_NONE;

			ActorTypeInfo *info = &game->actorTypeInfos[data->actorToBuild];

			Vec2i tilePosition = getTileHovering();
			Vec2 center = getCenter(tileToWorldRect(tilePosition));

			{
				CoreEvent *event = createCoreEvent(CORE_EVENT_SHOW_GHOST, NULL);
				event->ghostActorType = data->actorToBuild;
				event->ghostPosition = center;
			}

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
				float price = info->price + info->priceMulti*game->actorTypeCounts[data->actorToBuild];
				if (data->money >= price) {
					data->money -= price;
					Actor *newTower = createActor(data->actorToBuild);
					newTower->position = center;
					if (!keyPressed(KEY_SHIFT)) data->tool = TOOL_NONE;
				} else {
					infof("Not enough money\n");
				}
			}
		} else if (data->tool == TOOL_SELECTED) {
			if (isMouseClicked()) data->selectedActorsNum = 0;
			if (data->selectedActorsNum == 0) data->tool = TOOL_NONE;
		}

		data->toolTime += elapsed;
	} ///

	data->time += elapsed;
}

void generateMapFields() {
	World *world = data->world;
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
	World *world = data->world;

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
			tile->elevation = clampMap(perlinValue, 0.5, 1, 0, 3);
		}
	}

	return chunk;
}

Chunk *getChunkAt(Vec2i position) {
	World *world = data->world;

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
	World *world = data->world;

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
	World *world = data->world;
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

	float damage = getDamage(tower);

	ActorTypeInfo *towerInfo = &game->actorTypeInfos[tower->type];
	dealDamage(dest, damage, towerInfo->shieldDamageMulti, towerInfo->armorDamageMulti, towerInfo->hpDamageMulti);

	createCoreEvent(CORE_EVENT_HIT, src, dest);
}

void dealDamage(Actor *dest, float amount, float shieldDamageMulti, float armorDamageMulti, float hpDamageMulti, bool noCoreEvent) {
	float damageLeft = amount;

	float toBreakShield = dest->shield / shieldDamageMulti;
	float shieldDamage = MinNum(damageLeft, toBreakShield*1.01);
	damageLeft -= shieldDamage;
	float shieldRealDamage = shieldDamage * shieldDamageMulti;
	dest->shield -= shieldRealDamage;

	float toBreakArmor = dest->armor / armorDamageMulti;
	float armorDamage = MinNum(damageLeft, toBreakArmor*1.01);
	damageLeft -= armorDamage;
	float armorRealDamage = armorDamage * armorDamageMulti;
	dest->armor -= armorRealDamage;

	float hpRealDamage = damageLeft * hpDamageMulti;
	dest->hp -= hpRealDamage;

	if (!noCoreEvent) {
		CoreEvent *event = createCoreEvent(CORE_EVENT_DAMAGE, dest);
		event->armorValue = armorRealDamage;
		event->shieldValue = shieldRealDamage;
		event->hpValue = hpRealDamage;
	}
}

Rect getRect(Actor *actor) {
	Rect rect = makeCenteredRect(actor->position, v2(getInfo(actor)->size));
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

float getRange(ActorType actorType, Vec2i tilePos) {
	ActorTypeInfo *info = &game->actorTypeInfos[actorType];
	float range = info->baseRange;

	Tile *tile = getTileAt(tilePos);
	if (tile) range += tile->elevation * TILE_SIZE;

	for (int i = 0; i < data->ownedUpgradesNum; i++) {
		Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]);
		for (int i = 0; i < upgrade->effectsNum; i++) {
			UpgradeEffect *effect = &upgrade->effects[i];
			if (effect->type == UPGRADE_EFFECT_RANGE_MULTI) range *= effect->value;
		}
	}

	return range;
}

float getRange(Actor *actor, Vec2i tilePos) {
	float range = getRange(actor->type, tilePos);
	return range;
}

float getDamage(Actor *actor) {
	ActorTypeInfo *info = &game->actorTypeInfos[actor->type];
	float damage = info->damage;

	for (int i = 0; i < data->ownedUpgradesNum; i++) {
		Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]);
		for (int i = 0; i < upgrade->effectsNum; i++) {
			UpgradeEffect *effect = &upgrade->effects[i];
			if (effect->type == UPGRADE_EFFECT_DAMAGE_MULTI) damage *= effect->value;
		}
	}

	damage *= 1 + actor->level*0.1;
	return damage;
}

float getRpm(Actor *actor) {
	ActorTypeInfo *info = &game->actorTypeInfos[actor->type];
	float rpm = info->rpm;

	for (int i = 0; i < data->ownedUpgradesNum; i++) {
		Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]);
		for (int i = 0; i < upgrade->effectsNum; i++) {
			UpgradeEffect *effect = &upgrade->effects[i];
			if (effect->type == UPGRADE_EFFECT_RPM_MULTI) rpm *= effect->value;
		}
	}

	return rpm;
}

int getMaxLevel(ActorType actorType) {
	return 3;
}

ActorTypeInfo *getInfo(Actor *actor) {
	return &game->actorTypeInfos[actor->type];
}

Upgrade *getUpgrade(int id) {
	for (int i = 0; i < game->upgradesNum; i++) {
		Upgrade *upgrade = &game->upgrades[i];
		if (upgrade->id == id) return upgrade;
	}

	logf("No upgrade with id %d\n", id);
	return NULL;
}

bool hasUpgrade(int id) {
	for (int i = 0; i < data->ownedUpgradesNum; i++) {
		if (data->ownedUpgrades[i] == id) return true;
	}

	return false;
}

bool hasUpgradeEffect(UpgradeEffectType effectType, ActorType actorType) {
	for (int i = 0; i < data->ownedUpgradesNum; i++) {
		Upgrade *upgrade = getUpgrade(data->ownedUpgrades[i]);
		for (int i = 0; i < upgrade->effectsNum; i++) {
			UpgradeEffect *effect = &upgrade->effects[i];
			if (effect->type == effectType && effect->actorType == actorType) return true;
		}
	}

	return false;
}

Actor **getActorsInRange(Circle range, int *outNum, bool enemiesOnly) {
	World *world = data->world;

	Actor **enemiesInRange = (Actor **)frameMalloc(sizeof(Actor **) * world->actorsNum);
	int enemiesInRangeNum = 0;
	for (int i = 0; i < world->actorsNum; i++) {
		Actor *actor = &world->actors[i];
		ActorTypeInfo *otherInfo = &game->actorTypeInfos[actor->type];
		if (enemiesOnly && !otherInfo->isEnemy) continue;

		// if (contains(range, actor->position)) enemiesInRange[enemiesInRangeNum++] = actor;
		if (contains(getRect(actor), range)) enemiesInRange[enemiesInRangeNum++] = actor;
	}

	*outNum = enemiesInRangeNum;
	return enemiesInRange;
}

Actor **getActorsInRange(Tri2 range, int *outNum, bool enemiesOnly) {
	World *world = data->world;

	Actor **enemiesInRange = (Actor **)frameMalloc(sizeof(Actor **) * world->actorsNum);
	int enemiesInRangeNum = 0;
	for (int i = 0; i < world->actorsNum; i++) {
		Actor *actor = &world->actors[i];
		ActorTypeInfo *otherInfo = &game->actorTypeInfos[actor->type];
		if (enemiesOnly && !otherInfo->isEnemy) continue;

		if (overlaps(getRect(actor), range)) {
			enemiesInRange[enemiesInRangeNum++] = actor;
		}
	}

	*outNum = enemiesInRangeNum;
	return enemiesInRange;
}

void startNextWave() {
	data->playingWave = true;
	data->waveTime = 0;
	data->wave++;

	ActorType *possibleActors = (ActorType *)frameMalloc(sizeof(ActorType) * ACTOR_TYPES_MAX);
	int possibleActorsNum = 0;

	for (int i = 0; i < ACTOR_TYPES_MAX; i++) {
		ActorTypeInfo *info = &game->actorTypeInfos[i];
		if (info->enemySpawnStartingWave != 0 && info->enemySpawnStartingWave <= data->wave) {
			possibleActors[possibleActorsNum++] = (ActorType)i;
		}
	}

	if (possibleActorsNum == 0) {
		logf("No possible actor types to spawn???\n");
		possibleActors[possibleActorsNum++] = ACTOR_ENEMY1;
	}

	int maxEnemies = data->wave * data->wave;
	for (int i = 0; i < maxEnemies; i++) {
		float value = rndFloat(0, 1);
		value = tweenEase(value, QUAD_IN);
		int index = roundf(lerp(0, possibleActorsNum-1, value)); // Not perfect distribution
		data->actorsToSpawn[data->actorsToSpawnNum++] = possibleActors[index];
	}
}

Tri2 getAttackTri(Vec2 start, float range, float angle, float deviation) {
	Vec2 end0 = start + radToVec2(angle - deviation) * range;
	Vec2 end1 = start + radToVec2(angle + deviation) * range;
	Tri2 tri = makeTri2(start, end0, end1);
	return tri;
}

CoreEvent *createCoreEvent(CoreEventType type, Actor *src, Actor *dest) {
	if (game->coreEventsNum > CORE_EVENTS_MAX-1) {
		logf("Too many core events!!!\n");
		game->coreEventsNum--;
	}
	CoreEvent *event = &game->coreEvents[game->coreEventsNum++];
	memset(event, 0, sizeof(CoreEvent));
	event->type = type;
	if (src) event->srcId = src->id;
	if (dest) event->destId = dest->id;
	return event;
}

void saveState(char *path) {
	logf("Saving...\n");
	DataStream *stream = newDataStream();

	writeU32(stream, 13); // version
	writeFloat(stream, lcgSeed);
	writeString(stream, data->campaignName);
	writeFloat(stream, data->time);
	writeVec2(stream, data->cameraPosition);
	writeFloat(stream, data->cameraZoom);
	writeWorld(stream, data->world);

	writeU32(stream, data->prevTool);
	writeU32(stream, data->tool);
	writeFloat(stream, data->toolTime);

	writeU32(stream, data->actorToBuild);
	writeU32(stream, data->hp);
	writeU32(stream, data->money);
	writeFloat(stream, data->mana);
	writeFloat(stream, data->maxMana);
	writeU32(stream, data->wave);
	writeFloat(stream, data->waveTime);
	writeU8(stream, data->playingWave);

	writeU32(stream, data->actorsToSpawnNum);
	for (int i = 0; i < data->actorsToSpawnNum; i++) writeU32(stream, data->actorsToSpawn[i]);
	writeFloat(stream, data->timeTillNextSpawn);

	writeU32(stream, data->selectedActorsNum);
	for (int i = 0; i < data->selectedActorsNum; i++) writeU32(stream, data->selectedActors[i]);

	writeU32(stream, data->ownedUpgradesNum);
	for (int i = 0; i < data->ownedUpgradesNum; i++) writeU32(stream, data->ownedUpgrades[i]);

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
	writeU32(stream, actor->aimTarget);
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

	writeU32(stream, actor->level);
	writeFloat(stream, actor->xp);

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
	writeU8(stream, tile.elevation);
}

void loadState(char *path) {
	World *world = data->world;
	for (int i = 0; i < world->actorsNum; i++) deinitActor(&world->actors[i]);

	DataStream *stream = loadDataStream(path);
	if (!stream) {
		logf("No state at %s\n", path);
		return;
	}

	int version = readU32(stream);
	if (version >= 1) lcgSeed = readU32(stream);
	if (version >= 11) readStringInto(stream, data->campaignName, CAMPAIGN_NAME_MAX_LEN);
	data->time = readFloat(stream);
	data->cameraPosition = readVec2(stream);
	data->cameraZoom = readFloat(stream);
	readWorld(stream, data->world, version);
	if (version >= 6) data->prevTool = (Tool)readU32(stream);
	data->tool = (Tool)readU32(stream);
	if (version >= 6) data->toolTime = readFloat(stream);
	data->actorToBuild = (ActorType)readU32(stream);
	data->hp = version >= 3 ? readU32(stream) : 10;
	data->money = readU32(stream);
	data->mana = version >= 7 ? readFloat(stream) : 100;
	data->maxMana = version >= 7 ? readFloat(stream) : 100;
	data->wave = readU32(stream);
	if (version >= 10) data->waveTime = readFloat(stream);
	data->playingWave = readU8(stream);
	data->actorsToSpawnNum = readU32(stream);
	for (int i = 0; i < data->actorsToSpawnNum; i++) data->actorsToSpawn[i] = (ActorType)readU32(stream);
	data->timeTillNextSpawn = readFloat(stream);

	if (version >= 5) {
		data->selectedActorsNum = readU32(stream);
		for (int i = 0; i < data->selectedActorsNum; i++) data->selectedActors[i] = readU32(stream);
	}

	if (version >= 9) {
		data->ownedUpgradesNum = readU32(stream);
		for (int i = 0; i < data->ownedUpgradesNum; i++) data->ownedUpgrades[i] = readU32(stream);
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
	if (version >= 13) actor->aimTarget = readU32(stream);
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

		if (version >= 12) actor->level = readU32(stream);
		if (version >= 12) actor->xp = readFloat(stream);

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
	if (version >= 4) tile.elevation = readU8(stream);
	return tile;
}
