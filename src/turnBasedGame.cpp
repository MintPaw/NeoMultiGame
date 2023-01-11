struct Globals {
};

enum TargetType {
	TARGET_SINGLE,
	TARGET_NONE,
};

enum SpellType {
	SPELL_NONE,
	SPELL_ATTACK,
	SPELL_DEFEND,
	SPELL_END_TURN,
	SPELL_TYPES_MAX,
};
struct SpellTypeInfo {
#define SPELL_TYPE_NAME_MAX_LEN 64
	char name[SPELL_TYPE_NAME_MAX_LEN];
	TargetType targetType;
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
	UNIT_ENEMY1,
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

			info = &game->unitTypeInfos[UNIT_ENEMY1];
			strcpy(info->name, "Enemy1");
		}

		{
			SpellTypeInfo *info = NULL;

			for (int i = 0; i < SPELL_TYPES_MAX;i ++) {
				SpellTypeInfo *info = &game->spellTypeInfos[i];
			}

			info = &game->spellTypeInfos[SPELL_NONE];
			strcpy(info->name, "None");

			info = &game->spellTypeInfos[SPELL_ATTACK];
			strcpy(info->name, "Attack");

			info = &game->spellTypeInfos[SPELL_DEFEND];
			info->targetType = TARGET_NONE;
			strcpy(info->name, "Defend");

			info = &game->spellTypeInfos[SPELL_END_TURN];
			strcpy(info->name, "End Turn");
		}

		{
			auto createUnit = [](UnitType type)->Unit * {
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

				unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_ATTACK;
				unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DEFEND;
				return unit;
			};

			Unit *unit = NULL;

			unit = createUnit(UNIT_PLAYER1);
			unit->ally = true;

			unit = createUnit(UNIT_PLAYER2);
			unit->ally = true;

			unit = createUnit(UNIT_ENEMY1);
		}

		maximizeWindow();
	}

	if (game->turnQueueNum == 0) {
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->hp <= 0) continue;
			game->turnQueue[game->turnQueueNum++] = unit->id;
		}
	}

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;

	Unit *currentUnit = getUnit(game->turnQueue[0]);
	if (!currentUnit && platform->frameCount%60 == 0) logf("No current unit");

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	if (game->inEditor) {
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::End();
	}

	{
		ImGui::Begin("Game", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Turns: ");
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			ImGui::SameLine();
			ImGui::Text("(%s)", unit->info->name);
		}

		auto guiShowUnit = [](Unit *unit) {
			ImGui::Text("Type: %s", unit->info->name);
			ImGui::Text("Hp: %d/%d", unit->hp, unit->info->maxHp);
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
	}

	{
		if (game->spellQueueNum == 0 && currentUnit) {
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
							if (ImGui::Button(frameSprintf("%d: %s", i, unit->info->name))) {
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

	{
		if (game->spellQueueNum > 0) {
			Spell *spell = &game->spellQueue[0];
			bool complete = false;

			Unit *src = getUnit(spell->srcId);
			Unit *dest = getUnit(spell->destId);

			if (spell->type == SPELL_ATTACK) {
				if (game->spellTime == 0) dealDamage(src, dest, 3000);
				if (game->spellTime > 1) complete = true;
			} else if (spell->type == SPELL_DEFEND) {
				if (game->spellTime == 0) logf("Defend?");
				if (game->spellTime > 1) complete = true;
			} else if (spell->type == SPELL_END_TURN) {
				if (game->spellTime > 1) {
					logf("Next turn\n");
					arraySpliceIndex(game->turnQueue, game->turnQueueNum, sizeof(int), 0);
					game->turnQueueNum--;
					complete = true;
				}
			} else {
				if (game->spellTime == 0) logf("Spell %d has no update loop\n", spell->type);
				if (game->spellTime > 3) complete = true;
			}

			game->spellTime += elapsed;

			if (complete) {
				arraySpliceIndex(game->spellQueue, game->spellQueueNum, sizeof(Spell), 0);
				game->spellQueueNum--;
				game->spellTime = 0;
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
}
