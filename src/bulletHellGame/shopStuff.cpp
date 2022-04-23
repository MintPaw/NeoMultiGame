#ifndef __SHOPSTUFF_IMPL

void fillPlayerShop(bool player);
void castSpell(bool player, SpellType castedSpellType, bool sendToOpponent);
void addSpellToPlayer(bool player, SpellType effect);
void removeSpellFromPlayer(bool player, SpellType effect);
int getSpellBoss(SpellType effect);
int getSpellBossDifficulty(SpellType effect);
int getSpellAttackPattern(SpellType effect);
/// FUNCTIONS ^
#else

void fillPlayerShop(bool player) {
		Game *game = player ? realGame : realGame2;

		if (app->playMode == PLAY_TRAINING) {addSpellToPlayer(player, SPELL_BUBBLE_SHIELD);}
		addSpellToPlayer(player, SPELL_RETURN_FIRE);
		addSpellToPlayer(player, (SpellType)(SPELL_BOSS_SKULL_EASY + rndInt(0, 5) * 4));
		addSpellToPlayer(player, SPELL_UPGRADE_ATTACK);
		addSpellToPlayer(player, SPELL_GROW_ENEMY);
		addSpellToPlayer(player, SPELL_SHRINK_SELF);
		addSpellToPlayer(player, SPELL_SCREEN_WRAP);
		addSpellToPlayer(player, SPELL_SUMMON_ATTACK_SUN);
}

void castSpell(bool player, SpellType castedSpellType, bool sendToOpponent) {
	Game *game = player ? realGame : realGame2;
	Game *otherGame = player ? realGame2 : realGame;

	if (sendToOpponent) {
		if (app->playMode != PLAY_ONLINE || game->playerNumber == 1) {
			if (otherGame->incomingSpellsNum < INCOMING_SPELLS_MAX-1) {
				SpellCast *cast = &otherGame->incomingSpells[otherGame->incomingSpellsNum++];
				memset(cast, 0, sizeof(SpellCast));
				cast->bossSpellLevel = game->bossSpellLevel;
				cast->attackPatternLevel = game->attackPatternLevel;
				cast->type = castedSpellType;
				cast->frame = otherGame->frameCount + app->streamedInputsNum + NET_MSG_INPUTS_MAX*2 + 15;
				// cast->frame = 3000;

				if (app->playMode == PLAY_ONLINE) {
					NetMsg *msg = newOutMsg(NET_MSG_CtC_SPELL_CAST);
					msg->unionSpellCast.cast = *cast;
				}

				if (castedSpellType == SPELL_GROW_ENEMY) {
					addSpellToPlayer(!player, SPELL_SHRINK_SELF);
					removeSpellFromPlayer(player, SPELL_GROW_ENEMY);
				} else if (castedSpellType == SPELL_SUMMON_ATTACK_SUN) {
					addSpellToPlayer(player, SPELL_SUMMON_ATTACK_RAINBOW);
					removeSpellFromPlayer(player, SPELL_SUMMON_ATTACK_SUN);
				} else if (castedSpellType == SPELL_SUMMON_ATTACK_RAINBOW) {
					addSpellToPlayer(player, SPELL_SUMMON_ATTACK_NARROWING);
					removeSpellFromPlayer(player, SPELL_SUMMON_ATTACK_RAINBOW);
				} else if (castedSpellType == SPELL_SUMMON_ATTACK_NARROWING) {
					addSpellToPlayer(player, SPELL_SUMMON_ATTACK_MISSILE);
					removeSpellFromPlayer(player, SPELL_SUMMON_ATTACK_NARROWING);
				} else if (castedSpellType == SPELL_SUMMON_ATTACK_MISSILE) {
					addSpellToPlayer(player, SPELL_SUMMON_ATTACK_XAIMED);
					removeSpellFromPlayer(player, SPELL_SUMMON_ATTACK_MISSILE);
				} else if (castedSpellType == SPELL_SUMMON_ATTACK_XAIMED) {
					addSpellToPlayer(player, SPELL_SUMMON_ATTACK_SPOOKY);
					removeSpellFromPlayer(player, SPELL_SUMMON_ATTACK_XAIMED);
				} else if (castedSpellType == SPELL_SUMMON_ATTACK_SPOOKY) {
					addSpellToPlayer(player, SPELL_SUMMON_ATTACK_SAND);
					removeSpellFromPlayer(player, SPELL_SUMMON_ATTACK_SPOOKY);
				} else if (castedSpellType == SPELL_SUMMON_ATTACK_SAND) {
					addSpellToPlayer(player, SPELL_SUMMON_ATTACK_BLADE);
					removeSpellFromPlayer(player, SPELL_SUMMON_ATTACK_SAND);
				} else if (castedSpellType == SPELL_SUMMON_ATTACK_BLADE) {
					if (app->playMode == PLAY_TRAINING) {addSpellToPlayer(player, SPELL_SUMMON_ATTACK_SUN);}
					removeSpellFromPlayer(player, SPELL_SUMMON_ATTACK_BLADE);
				} else if (getSpellBossDifficulty(castedSpellType) == 0) {
					addSpellToPlayer(player, (SpellType)(SPELL_BOSS_SKULL_MED + rndInt(0, 5) * 4));
					removeSpellFromPlayer(player, SPELL_BOSS_SKULL_EASY);
					removeSpellFromPlayer(player, SPELL_BOSS_QUIET_EASY);
					removeSpellFromPlayer(player, SPELL_BOSS_SPACE_EASY);
					removeSpellFromPlayer(player, SPELL_BOSS_SPORE_EASY);
					removeSpellFromPlayer(player, SPELL_BOSS_ZAPPY_EASY);
					removeSpellFromPlayer(player, SPELL_BOSS_FLAME_EASY);
				} else if (getSpellBossDifficulty(castedSpellType) == 1) {
					addSpellToPlayer(player, (SpellType)(SPELL_BOSS_SKULL_HARD + rndInt(0, 5) * 4));
					removeSpellFromPlayer(player, SPELL_BOSS_SKULL_MED);
					removeSpellFromPlayer(player, SPELL_BOSS_QUIET_MED);
					removeSpellFromPlayer(player, SPELL_BOSS_SPACE_MED);
					removeSpellFromPlayer(player, SPELL_BOSS_SPORE_MED);
					removeSpellFromPlayer(player, SPELL_BOSS_ZAPPY_MED);
					removeSpellFromPlayer(player, SPELL_BOSS_FLAME_MED);
				} else if (getSpellBossDifficulty(castedSpellType) == 2) {
					removeSpellFromPlayer(player, SPELL_BOSS_SKULL_HARD);
					removeSpellFromPlayer(player, SPELL_BOSS_QUIET_HARD);
					removeSpellFromPlayer(player, SPELL_BOSS_SPACE_HARD);
					removeSpellFromPlayer(player, SPELL_BOSS_SPORE_HARD);
					removeSpellFromPlayer(player, SPELL_BOSS_ZAPPY_HARD);
					removeSpellFromPlayer(player, SPELL_BOSS_FLAME_HARD);
					addSpellToPlayer(player, (SpellType)(SPELL_BOSS_SKULL_HARD + rndInt(0, 5) * 4));
				}
			} else {
				logf("Too many spells, it's gone now\n");
			}
		}
	} else {
		if (castedSpellType == SPELL_BUBBLE_SHIELD) {game->hasBubbleShield = true; removeSpellFromPlayer(player, SPELL_BUBBLE_SHIELD);}
		else if (castedSpellType == SPELL_RETURN_FIRE) {game->returnFireQuantity++;}
		else if (getSpellBoss(castedSpellType) != -1) {
			Danmaku *danmaku = createShotU(v2(game->width/2, game->height/5), 0, 0, (DanmakuType)(DTYPE_BOSS_FLOWERGIRL + getSpellBoss(castedSpellType)), getSpellBossDifficulty(castedSpellType) + 1);
			game->danmakuWithHpBar = danmaku->id;
		} else if (castedSpellType == SPELL_UPGRADE_ATTACK) {
			game->attackLevel++;
			if (game->attackLevel == 3) {removeSpellFromPlayer(player, SPELL_UPGRADE_ATTACK);}
		}
		else if (castedSpellType == SPELL_GROW_ENEMY) {game->playerScale = 1.5;}
		else if (castedSpellType == SPELL_SHRINK_SELF) {game->playerScale = 0.5; removeSpellFromPlayer(player, SPELL_SHRINK_SELF); addSpellToPlayer(!player, SPELL_GROW_ENEMY);}
		else if (castedSpellType == SPELL_SCREEN_WRAP) {game->hasScreenWrap = true; removeSpellFromPlayer(player, SPELL_SCREEN_WRAP);}
		else if (getSpellAttackPattern(castedSpellType) != -1) {Danmaku *danmaku = createShotU(v2(game->width/2, game->height/5), 0, 0, DTYPE_SPELL_ATTACK, getSpellAttackPattern(castedSpellType) + 1);}
		else {logf("Non-spell\n");}
	}
}

void addSpellToPlayer(bool player, SpellType effect) {
	Game *game = player ? realGame : realGame2;

	game->shopFlags |= (1ull << effect);
}

void removeSpellFromPlayer(bool player, SpellType effect) {
	Game *game = player ? realGame : realGame2;

	game->shopFlags &= ~(1ull << effect);

	game->spellIndex = (game->spellIndex + 1) % 64;
	while (!(game->shopFlags & (1ull << game->spellIndex))) {game->spellIndex = (game->spellIndex + 1) % 64;}
}

//I know these functions aren't necessary. But I feel it's justified. =P
int getSpellBoss(SpellType effect) {
	return (effect >= SPELL_BOSS_SKULL_EASY) && (effect <= SPELL_BOSS_FLAME_LUNA) ? (effect - SPELL_BOSS_SKULL_EASY) / 4 : -1;
}

int getSpellBossDifficulty(SpellType effect) {
	return (effect >= SPELL_BOSS_SKULL_EASY) && (effect <= SPELL_BOSS_FLAME_LUNA) ? (effect - SPELL_BOSS_SKULL_EASY) % 4 : -1;
}

int getSpellAttackPattern(SpellType effect) {
	return ((effect >= SPELL_SUMMON_ATTACK_SUN) && (effect <= SPELL_SUMMON_ATTACK_BLADE)) ? effect - SPELL_SUMMON_ATTACK_SUN : -1;
}

#endif
