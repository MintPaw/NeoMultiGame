Unit *getRandomOpponent(Unit *src);
Unit *getRandomOpponent(Unit *src) {
	Unit **possibleUnits = (Unit **)frameMalloc(sizeof(Unit *) * game->unitsNum);
	int possibleUnitsNum = 0;

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (src->ally == unit->ally) continue;
		if (unit->hp <= 0) continue;
		if (isHidden(unit)) continue;
		possibleUnits[possibleUnitsNum++] = unit;
	}

	if (possibleUnitsNum == 0) return NULL;

	Unit *chosenUnit = possibleUnits[rndInt(0, possibleUnitsNum-1)];
	return chosenUnit;
}

Unit *getRandomFriend(Unit *src);
Unit *getRandomFriend(Unit *src) {
	Unit **possibleUnits = (Unit **)frameMalloc(sizeof(Unit *) * game->unitsNum);
	int possibleUnitsNum = 0;

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (src->ally != unit->ally) continue;
		if (unit->hp <= 0) continue;
		if (isHidden(unit)) continue;
		possibleUnits[possibleUnitsNum++] = unit;
	}

	if (possibleUnitsNum == 0) return NULL;

	Unit *chosenUnit = possibleUnits[rndInt(0, possibleUnitsNum-1)];
	return chosenUnit;
}

int countFriends(Unit *src);
int countFriends(Unit *src) {
	int count = 0;
	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (src->ally != unit->ally) continue;
		if (unit->hp <= 0) continue;
		if (unit == src) continue;
		count++;
	}
	return count;
}

void aiTakeTurn(Unit *src);
void aiTakeTurn(Unit *src) {
	if (src->type == UNIT_STANDARD_A) {
		Unit *target = getRandomOpponent(src);
		castSpell(src, target, SPELL_MEDIUM_ATTACK);
	} else if (src->type == UNIT_STANDARD_B) {
		Unit *target = getRandomOpponent(src);
		castSpell(src, target, SPELL_SMALL_ATTACK);
	} else if (src->type == UNIT_STANDARD_C) {
		Unit *target = getRandomOpponent(src);
		castSpell(src, target, SPELL_LARGE_ATTACK);
	} else if (src->type == UNIT_SWIFT) {
		Unit *target = getRandomOpponent(src);
		castSpell(src, target, SPELL_SMALL_ATTACK);
	} else if (src->type == UNIT_SMALL_SHIELDSTER) {
		int shields = countBuffs(src, BUFF_SHIELD);
		if (shields == 0) {
			castSpell(src, src, SPELL_CREATE_SHIELD);
		} else if (shields >= 5) {
			castSpell(src, getRandomOpponent(src), SPELL_SHIELD_BLAST);
		} else {
			castSpell(src, getRandomFriend(src), SPELL_CREATE_SHIELD);
		}
	} else if (src->type == UNIT_SUPER_SHIELDSTER) {
		int shields = countBuffs(src, BUFF_SHIELD);
		if (shields == 0) {
			castSpell(src, src, SPELL_CREATE_2_SHIELDS);
		} else {
			castSpell(src, getRandomOpponent(src), SPELL_SHIELD_BUMP);
		}
	} else if (src->type == UNIT_SHIELD_SUMMONER) {
		castSpell(src, getRandomOpponent(src), SPELL_SUMMONER_ATTACK);
	} else if (src->type == UNIT_FAKE_SHIELDSTER) {
		castSpell(src, getRandomFriend(src), SPELL_CREATE_SHIELD);
		castSpell(src, getRandomFriend(src), SPELL_CREATE_FAKE_SHIELD);
	} else if (src->type == UNIT_MANA_BRUISER) {
		Unit *p1 = getUnitByType(UNIT_PLAYER1);
		Unit *p2 = getUnitByType(UNIT_PLAYER2);

		int mpToTake = 50;

		bool ignoreIndices[2] = {};
		if (isHidden(p1)) ignoreIndices[0] = true;
		if (p1->mp < mpToTake) ignoreIndices[0] = true;
		if (isHidden(p2)) ignoreIndices[1] = true;
		if (p2->mp < mpToTake) ignoreIndices[1] = true;

		if (ignoreIndices[0] || ignoreIndices[1]) {
			if (!ignoreIndices[0]) castSpell(src, p1, SPELL_TAKE_MANA_BIG);
			if (!ignoreIndices[1]) castSpell(src, p2, SPELL_TAKE_MANA_BIG);
		} else {
			castSpell(src, p1, SPELL_TAKE_MANA);
			castSpell(src, p2, SPELL_TAKE_MANA);
		}
	} else if (src->type == UNIT_ACCELERATOR) {
		castSpell(src, NULL, SPELL_ACCELERATED_SLASH);
	} else if (src->type == UNIT_STUDENT) {
		castSpell(src, getRandomOpponent(src), SPELL_STUDENT_ATTACK);
	} else if (src->type == UNIT_TEACHER) {
		if (rndPerc(0.5) && countFriends(src) < 4) {
			castSpell(src, NULL, SPELL_CREATE_STUDENT);
		} else {
			castSpell(src, getRandomOpponent(src), SPELL_TEACHER_ATTACK);
		}
	} else if (src->type == UNIT_TEACHER_TEACHER) {
		if (rndPerc(0.5) && countFriends(src) < 4) {
			castSpell(src, NULL, SPELL_CREATE_TEACHER);
		} else {
			castSpell(src, getRandomOpponent(src), SPELL_TEACHER_TEACHER_ATTACK);
		}
	} else {
		logf("%s has no ai\n", src->info->name);
	}

	castSpell(src, NULL, SPELL_END_TURN);
}
