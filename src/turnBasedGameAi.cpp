Unit *getRandomOpponent(Unit *src) {
	Unit **possibleUnits = (Unit **)frameMalloc(sizeof(Unit *) * game->unitsNum);
	int possibleUnitsNum = 0;

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (src->ally == unit->ally) continue;
		if (unit->hp <= 0) continue;
		possibleUnits[possibleUnitsNum++] = unit;
	}

	if (possibleUnitsNum == 0) return NULL;

	Unit *chosenUnit = possibleUnits[rndInt(0, possibleUnitsNum-1)];
	return chosenUnit;
}

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
	}
	castSpell(src, NULL, SPELL_END_TURN);
}
