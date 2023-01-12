struct Globals {
};

enum TargetType {
	TARGET_SINGLE,
	TARGET_NONE,
};

enum SpellType {
	SPELL_NONE,
	SPELL_HERO_ATTACK,
	SPELL_SMALL_ATTACK,
	SPELL_MEDIUM_ATTACK,
	SPELL_LARGE_ATTACK,
	SPELL_DEFEND,
	SPELL_END_TURN,
	SPELL_TYPES_MAX,
};
struct SpellTypeInfo {
#define SPELL_TYPE_NAME_MAX_LEN 64
	char name[SPELL_TYPE_NAME_MAX_LEN];
	TargetType targetType;
	int damage;

	bool canTargetAllies;
};
struct Spell {
	SpellType type;
	int id;
	SpellTypeInfo *info;
	int srcId;
	int destId;
};

enum UnitType {
	UNIT_PLAYER1,
	UNIT_PLAYER2,
	UNIT_STANDARD_A,
	UNIT_STANDARD_B,
	UNIT_STANDARD_C,
	UNIT_TYPES_MAX,
};
struct UnitTypeInfo {
#define UNIT_TYPE_NAME_MAX_LEN 64
	char name[UNIT_TYPE_NAME_MAX_LEN];

	int maxHp;
	int maxMana;
};
struct Unit {
	UnitType type;
	int id;
	UnitTypeInfo *info;

#define SCREEN_NAME_MAX_LEN 128
	char screenName[SCREEN_NAME_MAX_LEN];

	bool ally;
	int hp;
	int mana;

#define SPELLS_AVAILABLE_MAX 128
	SpellType spellsAvailable[SPELLS_AVAILABLE_MAX];
	int spellsAvailableNum;
};

struct Game {
	Globals globals;

	UnitTypeInfo unitTypeInfos[UNIT_TYPES_MAX];
#define UNITS_MAX 128
	Unit units[UNITS_MAX];
	int unitsNum;
	int nextUnitId;

	SpellTypeInfo spellTypeInfos[SPELL_TYPES_MAX];
#define SPELL_QUEUE_MAX 128
	Spell spellQueue[SPELL_QUEUE_MAX];
	int spellQueueNum;
	int nextSpellId;

	int turnQueue[UNITS_MAX];
	int turnQueueNum;

	int wave;

	SpellType currentSpellType;
	float spellTime;

	bool inEditor;
};

Game *game = NULL;

void runGame();
void updateGame();

Unit *getUnit(int id);
Spell *castSpell(Unit *src, Unit *dest, SpellType type);
void dealDamage(Unit *src, Unit *dest, int amount);
Unit *createUnit(UnitType type);
void nextWave();
/// FUNCTIONS ^

#include "turnBasedGameAi.cpp"

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

	initPlatform(1920, 1080, "A game?");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1920, 1080);
	initMesh();
	initModel();
	initSkeleton();
	initFonts();
	initTextureSystem();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		{
			UnitTypeInfo *info = NULL;

			for (int i = 0; i < UNIT_TYPES_MAX;i ++) {
				UnitTypeInfo *info = &game->unitTypeInfos[i];
				info->maxHp = 10000;
				info->maxMana = 1000;
			}

			info = &game->unitTypeInfos[UNIT_PLAYER1];
			strcpy(info->name, "Player1");

			info = &game->unitTypeInfos[UNIT_PLAYER2];
			strcpy(info->name, "Player2");

			info = &game->unitTypeInfos[UNIT_STANDARD_A];
			info->maxHp = 5000;
			strcpy(info->name, "Standard A");

			info = &game->unitTypeInfos[UNIT_STANDARD_B];
			info->maxHp = 10000;
			strcpy(info->name, "Standard B");

			info = &game->unitTypeInfos[UNIT_STANDARD_C];
			info->maxHp = 20000;
			strcpy(info->name, "Standard C");
		}

		{
			SpellTypeInfo *info = NULL;

			for (int i = 0; i < SPELL_TYPES_MAX;i ++) {
				SpellTypeInfo *info = &game->spellTypeInfos[i];
				info->damage = 1000;
			}

			info = &game->spellTypeInfos[SPELL_NONE];
			strcpy(info->name, "None");

			info = &game->spellTypeInfos[SPELL_HERO_ATTACK];
			strcpy(info->name, "Hero Attack");
			info->damage = 5000;

			info = &game->spellTypeInfos[SPELL_SMALL_ATTACK];
			strcpy(info->name, "Small attack");
			info->damage = 500;

			info = &game->spellTypeInfos[SPELL_MEDIUM_ATTACK];
			strcpy(info->name, "Medium attack");
			info->damage = 1000;

			info = &game->spellTypeInfos[SPELL_LARGE_ATTACK];
			strcpy(info->name, "Large attack");
			info->damage = 2000;

			info = &game->spellTypeInfos[SPELL_DEFEND];
			info->targetType = TARGET_NONE;
			strcpy(info->name, "Defend");

			info = &game->spellTypeInfos[SPELL_END_TURN];
			strcpy(info->name, "End Turn");
		}

		{
			Unit *unit = NULL;

			unit = createUnit(UNIT_PLAYER1);
			unit->ally = true;

			unit = createUnit(UNIT_PLAYER2);
			unit->ally = true;

			nextWave();
		}

		maximizeWindow();
	}

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	if (game->inEditor) {
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::End();
	}

	{ /// Display
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.5), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Turns", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Turns: ");
		for (int i = 0; i < game->turnQueueNum; i++) {
			Unit *unit = getUnit(game->turnQueue[i]);
			ImGui::SameLine();
			ImGui::Text("(%s)", unit->screenName);
		}
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.80), ImGuiCond_Always, ImVec2(0.5, 1));
		ImGui::Begin("Game", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		auto guiShowUnit = [](Unit *unit) {
			int color = 0xFFFFFFFF;
			if (game->turnQueueNum != 0 && game->turnQueue[0] == unit->id) color = lerpColor(color, 0xFF00FF00, 0.5);
			if (game->spellQueueNum != 0 && game->spellQueue[0].destId == unit->id) color = lerpColor(color, 0xFFFF0000, 0.5);

			ImVec4 imCol = guiGetImVec4Color(color);
			ImGui::TextColored(imCol, "%s", unit->screenName);
			ImGui::TextColored(imCol, "Hp: %d/%d", unit->hp, unit->info->maxHp);
			ImGui::Separator();
		};

		ImGui::BeginChild("AlliesChild", ImVec2(300, 300));
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (!unit->ally) continue;
			guiShowUnit(unit);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("EnemiesChild", ImVec2(300, 300));
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->ally) continue;
			guiShowUnit(unit);
		}
		ImGui::EndChild();

		ImGui::End();
	} ///

	{ /// Update turn
		if (game->turnQueueNum == 0) {
			for (int i = 0; i < game->unitsNum; i++) {
				Unit *unit = &game->units[i];
				if (unit->hp <= 0) continue;
				game->turnQueue[game->turnQueueNum++] = unit->id;
			}
		}

		Unit *currentUnit = getUnit(game->turnQueue[0]);
		if (!currentUnit && platform->frameCount%60 == 0) logf("No current unit\n");

		if (game->spellQueueNum == 0) {
			int allyCount = 0;
			int enemyCount = 0;
			for (int i = 0; i < game->unitsNum; i++) {
				Unit *unit = &game->units[i];
				if (unit->hp <= 0) continue;
				if (unit->ally) allyCount++;
				if (!unit->ally) enemyCount++;
			}

			if (allyCount == 0) {
				ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.95), ImGuiCond_Always, ImVec2(0.5, 1));
				ImGui::Begin("Game Over", NULL, ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("You lose");
				if (ImGui::Button("Try again"));
				ImGui::End();
			} else if (enemyCount == 0) {
				ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.95), ImGuiCond_Always, ImVec2(0.5, 1));
				ImGui::Begin("Game Over", NULL, ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("You win");
				if (ImGui::Button("Continue")) {
					nextWave();
				}
				ImGui::End();
			} else if (currentUnit) {
				if (currentUnit->ally) {
					ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.95), ImGuiCond_Always, ImVec2(0.5, 1));
					ImGui::Begin("Spells", NULL, ImGuiWindowFlags_AlwaysAutoResize);
					if (game->currentSpellType == SPELL_NONE) {
						for (int i = 0; i < currentUnit->spellsAvailableNum; i++) {
							SpellType type = currentUnit->spellsAvailable[i];
							SpellTypeInfo *info = &game->spellTypeInfos[type];

							ImGui::PushID(i);
							if (ImGui::Button(info->name)) {
								game->currentSpellType = type;
							}
							ImGui::PopID();
						}
					} else {
						SpellTypeInfo *currentSpellInfo = &game->spellTypeInfos[game->currentSpellType];
						if (currentSpellInfo->targetType == TARGET_SINGLE) {
							ImGui::Text("Target:");
							for (int i = 0; i < game->unitsNum; i++) {
								Unit *unit = &game->units[i];
								if (unit->ally) continue;
								if (unit->hp <= 0) continue;
								if (ImGui::Button(frameSprintf("%d: %s", i, unit->screenName))) {
									castSpell(currentUnit, unit, game->currentSpellType);
									castSpell(currentUnit, NULL, SPELL_END_TURN);
									game->currentSpellType = SPELL_NONE;
								}
							}
							if (ImGui::Button("Cancel")) game->currentSpellType = SPELL_NONE;
						} else if (currentSpellInfo->targetType == TARGET_NONE) {
							castSpell(currentUnit, NULL, game->currentSpellType);
							castSpell(currentUnit, NULL, SPELL_END_TURN);
							game->currentSpellType = SPELL_NONE;
						}
					}
					ImGui::End();
				} else {
					aiTakeTurn(currentUnit);
				}
			}
		}
	} ///

	{ /// Update spells
		if (game->spellQueueNum > 0) {
			Spell *spell = &game->spellQueue[0];
			bool complete = false;

			Unit *src = getUnit(spell->srcId);
			Unit *dest = getUnit(spell->destId);

			float baseSpellTime = 0.25;

			if (
				spell->type == SPELL_HERO_ATTACK ||
				spell->type == SPELL_SMALL_ATTACK ||
				spell->type == SPELL_MEDIUM_ATTACK ||
				spell->type == SPELL_LARGE_ATTACK
			) {
				if (game->spellTime == 0) dealDamage(src, dest, spell->info->damage);
				if (game->spellTime > baseSpellTime) complete = true;
			} else if (spell->type == SPELL_DEFEND) {
				if (game->spellTime == 0) logf("Defend?\n");
				if (game->spellTime > baseSpellTime) complete = true;
			} else if (spell->type == SPELL_END_TURN) {
				if (game->spellTime > baseSpellTime) {
					Unit *currentUnit = getUnit(game->turnQueue[0]);
					logf("%s's turn is over\n", currentUnit->info->name);

					arraySpliceIndex(game->turnQueue, game->turnQueueNum, sizeof(int), 0);
					game->turnQueueNum--;
					complete = true;
				}
			} else {
				if (game->spellTime == 0) logf("%s (spellType %d) has no update loop\n", spell->info->name, spell->type);
				if (game->spellTime > 3) complete = true;
			}

			game->spellTime += elapsed;

			if (complete) {
				arraySpliceIndex(game->spellQueue, game->spellQueueNum, sizeof(Spell), 0);
				game->spellQueueNum--;
				game->spellTime = 0;
			}
		}
	} ///

	if (game->spellQueueNum == 0) {
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->ally) continue;
			if (unit->hp <= 0) {
				arraySpliceIndex(game->units, game->unitsNum, sizeof(Unit), i);
				game->unitsNum--;
				i--;
				continue;
			}
		}
	}

	clearRenderer();

	drawOnScreenLog();
}

Unit *getUnit(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->id == id) return unit;
	}
	return NULL;
}

Spell *castSpell(Unit *src, Unit *dest, SpellType type) {
	if (game->spellQueueNum > SPELL_QUEUE_MAX-1) {
		logf("Spell queue is full!\n");
		game->spellQueueNum--;
	}

	Spell *spell = &game->spellQueue[game->spellQueueNum++];
	memset(spell, 0, sizeof(Spell));
	spell->type = type;
	spell->id = ++game->nextSpellId;
	spell->info = &game->spellTypeInfos[spell->type];
	if (src) spell->srcId = src->id;
	if (dest) spell->destId = dest->id;
	return spell;
}

void dealDamage(Unit *src, Unit *dest, int amount) {
	logf("%s dealt %d damage to %s\n", src->info->name, amount, dest->info->name);
	dest->hp -= amount;

	if (dest->hp <= 0) {
		for (int i = 0; i < game->turnQueueNum; i++) {
			if (game->turnQueue[i] == dest->id) {
				arraySpliceIndex(game->turnQueue, game->turnQueueNum, sizeof(int), i);
				game->turnQueueNum--;
				i--;
				continue;
			}
		}
	}
}

Unit *createUnit(UnitType type) {
	if (game->unitsNum > UNITS_MAX-1) {
		logf("No more units!\n");
		return NULL;
	}

	Unit *unit = &game->units[game->unitsNum++];
	memset(unit, 0, sizeof(Unit));
	unit->type = type;
	unit->id = ++game->nextUnitId;
	unit->info = &game->unitTypeInfos[unit->type];
	unit->hp = unit->info->maxHp;
	unit->mana = unit->info->maxMana;

	unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_HERO_ATTACK;
	unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DEFEND;

	{
		int existingCount = 0;
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->type == type) existingCount++;
		}
		if (existingCount == 1) {
			strcpy(unit->screenName, unit->info->name);
		} else {
			strcpy(unit->screenName, frameSprintf("%s (%d)", unit->info->name, existingCount-1));
		}
	}

	return unit;
};

void nextWave() {
	game->wave++;

	Unit *unit = NULL;

	if (game->wave == 1) {
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_B);
	} else if (game->wave == 2) {
		unit = createUnit(UNIT_STANDARD_B);
		unit = createUnit(UNIT_STANDARD_B);
	} else if (game->wave == 3) {
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_B);
		unit = createUnit(UNIT_STANDARD_A);
	} else if (game->wave == 4) {
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_C);
		unit = createUnit(UNIT_STANDARD_A);
	} else if (game->wave == 5) {
		logf("You win\n");
	}
}
