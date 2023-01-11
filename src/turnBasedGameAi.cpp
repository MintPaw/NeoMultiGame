Unit *getRandomEnemy(Unit *src) {
	Unit **possibleUnits = (Unit **)frameMalloc(sizeof(Unit *) * game->unitsNum);
	int possibleUnitsNum = 0;

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (src->ally != unit->ally) possibleUnits[possibleUnitsNum++] = unit;
	}

	if (possibleUnitsNum == 0) return NULL;

	Unit *chosenUnit = possibleUnits[rndInt(0, possibleUnitsNum-1)];
	return chosenUnit;
}

void aiTakeTurn(Unit *src) {
	Unit *target = getRandomEnemy(src);
	castSpell(src, target, SPELL_ATTACK);
	castSpell(src, NULL, SPELL_END_TURN);
}
