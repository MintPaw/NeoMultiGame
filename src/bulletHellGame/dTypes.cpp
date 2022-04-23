void updateKoumaKannon(Danmaku *danmaku);
void updateSpreadAttacker(Danmaku *danmaku);
void updateMineDropper(Danmaku *danmaku);
void updateSpinner(Danmaku *danmaku);
void updateRain(Danmaku *danmaku);
void updateSeek(Danmaku *danmaku);
void updateZStrike(Danmaku *danmaku);
void updateOmni(Danmaku *danmaku);
void updateFlowerGirl(Danmaku *danmaku);
void updateGuruGuruGirl(Danmaku *danmaku);
void updateStarGirl(Danmaku *danmaku);
void updateThornGirl(Danmaku *danmaku);
void updateKnifeGirl(Danmaku *danmaku);
void updateWolfGirl(Danmaku *danmaku);
void updateSpecialLaserAdd(Danmaku *danmaku);
void updateSpecialStar(Danmaku *danmaku);
void updateSpecialSniper(Danmaku *danmaku);
void updateSpecialVine(Danmaku *danmaku);
void updateSpecialBarb(Danmaku *danmaku);
void updateSpecialOrbitar(Danmaku *danmaku);
void updateSpecialExploder(Danmaku *danmaku);
void updateSpecialChain(Danmaku *danmaku);
void updateSpellSwitchboard(Danmaku *danmaku);
void updateSpellSun(Danmaku *danmaku);
void updateSpellRainbow(Danmaku *danmaku);
void updateSpellNarrowing(Danmaku *danmaku);
void updateSpellMissile(Danmaku *danmaku);
void updateSpellXAimed(Danmaku *danmaku);
void updateSpellSpooky(Danmaku *danmaku);
void updateSpellSand(Danmaku *danmaku);
void updateSpellBlade(Danmaku *danmaku);
/// FUNCTIONS ^



bool propDanmakuRegen(DanmakuType dType) {
	switch (dType) {
		case DTYPE_BULLET:           return true;
		case DTYPE_PLAYER_BULLET:    return true;
		case DTYPE_BOMB:             return true;
		case DTYPE_SPECIAL_LASERADD: return true;
		case DTYPE_SPECIAL_STAR:     return true;
		case DTYPE_SPECIAL_SNIPER:   return true;
		case DTYPE_SPECIAL_VINE:     return true;
		case DTYPE_SPECIAL_BARB:     return true;
		case DTYPE_SPECIAL_ORBITAR:  return true;
		case DTYPE_SPECIAL_EXPLODER: return true;
		case DTYPE_SPECIAL_CHAIN:    return true;
		default:                     return false;
	}
}

bool propDanmakuHitsBullets(DanmakuType dType) {
	switch (dType) {
		case DTYPE_BOMB: return true;
		default: return false;
	}
}

bool propDanmakuIsEnemy(DanmakuType dType) {
	switch (dType) {
		case DTYPE_BULLET:           return false;
		case DTYPE_PLAYER_BULLET:    return false;
		case DTYPE_BOMB:             return false;
		case DTYPE_SPECIAL_LASERADD: return false;
		case DTYPE_SPECIAL_STAR:     return false;
		case DTYPE_SPECIAL_SNIPER:   return false;
		case DTYPE_SPECIAL_VINE:     return false;
		case DTYPE_SPECIAL_BARB:     return false;
		case DTYPE_SPECIAL_ORBITAR:  return false;
		case DTYPE_SPECIAL_EXPLODER: return false;
		case DTYPE_SPECIAL_CHAIN:    return false;
		case DTYPE_SPELL_ATTACK:     return false;
		default:                     return true;
	}
}

int propDanmakuGetHealth(DanmakuType dType) {
	switch (dType) {
		case DTYPE_BULLET:            return 60;
		case DTYPE_PLAYER_BULLET:     return 60;
		case DTYPE_BOMB:              return 16000;
		case DTYPE_KOUMA_KANNON:      return 600;
		case DTYPE_SPREAD_ATTACKER:   return 600;
		case DTYPE_MINE_DROPPER:      return 600;
		case DTYPE_SPINNER:           return 600;
		case DTYPE_RAIN:              return 2600;
		case DTYPE_SEEK:              return 1800;
		case DTYPE_ZSTRIKE:           return 600;
		case DTYPE_OMNI:              return 2600;
		case DTYPE_BOSS_FLOWERGIRL:   return 42000 * 0.8;
		case DTYPE_BOSS_GURUGURUGIRL: return 36000 * 0.8;
		case DTYPE_BOSS_STARGIRL:     return 33000 * 0.8;
		case DTYPE_BOSS_THORNGIRL:    return 52000 * 0.8;
		case DTYPE_BOSS_KNIFEGIRL:    return 50000 * 0.8;
		case DTYPE_BOSS_WOLFGIRL:     return 24000 * 0.8;
		case DTYPE_SPECIAL_LASERADD:  return 60;
		case DTYPE_SPECIAL_STAR:      return 30;
		case DTYPE_SPECIAL_SNIPER:    return 60;
		case DTYPE_SPECIAL_VINE:      return 60;
		case DTYPE_SPECIAL_BARB:      return 60;
		case DTYPE_SPECIAL_ORBITAR:   return 60;
		case DTYPE_SPECIAL_EXPLODER:  return 60;
		case DTYPE_SPECIAL_CHAIN:     return 60;
		case DTYPE_SPELL_ATTACK:      return 1;
		default:                      return 0;
	}
}

bool propDanmakuPersistsOffscreen(DanmakuType dType) {
	switch (dType) {
		case DTYPE_BOSS_WOLFGIRL: return true;
		default: return false;
	}
}

ColorName propDanmakuGetEnemyColor(DanmakuType dType) {
	switch (dType) {
		case DTYPE_KOUMA_KANNON:      return ORANGE;
		case DTYPE_SPREAD_ATTACKER:   return CYAN;
		case DTYPE_MINE_DROPPER:      return MAGENTA;
		case DTYPE_SPINNER:           return YELLOW;
		case DTYPE_RAIN:              return TEAL;
		case DTYPE_SEEK:              return LIME;
		case DTYPE_ZSTRIKE:           return VIOLET;
		case DTYPE_OMNI:              return PURPLE;
		case DTYPE_BOSS_FLOWERGIRL:   return INDIGO;
		case DTYPE_BOSS_GURUGURUGIRL: return CYAN;
		case DTYPE_BOSS_STARGIRL:     return VIOLET;
		case DTYPE_BOSS_THORNGIRL:    return GREEN;
		case DTYPE_BOSS_KNIFEGIRL:    return RED;
		case DTYPE_BOSS_WOLFGIRL:     return BLUE;
		default:                      return RED;
	}
}

bool propDanmakuIsAlly(DanmakuType dType) {
	switch (dType) {
		case DTYPE_PLAYER_BULLET:    return true;
		case DTYPE_BOMB:             return true;
		case DTYPE_SPECIAL_LASERADD: return true;
		case DTYPE_SPELL_ATTACK:     return true;
		default:                     return false;
	}
}

void eDanmakuKill(Danmaku *danmaku) {
	if (danmaku->dead) return;
	danmaku->dead = true;
	if (propDanmakuIsEnemy (danmaku->type)) {
		Sound *sound = getSound("assets/audio/effects/enemyDie.ogg");
		Channel *channel = playSound(sound);
		channel->userVolume = 0.4;
		for (int i = 0; i < game->returnFireQuantity; i++) createShotA(danmaku->position, rndFloat(0.8, 3.6), rndFloat(0, 359), BALL_S, (ColorName) rndInt(0, 11), 24);
	}
	switch (danmaku->type) {
		case DTYPE_BULLET:            createItem(STAR, danmaku->position, true,  true);  return;
		case DTYPE_KOUMA_KANNON:      createItem(MANA, danmaku->position, false, false); return;
		case DTYPE_SPREAD_ATTACKER:   createItem(MANA, danmaku->position, false, false); return;
		case DTYPE_MINE_DROPPER:      createItem(MANA, danmaku->position, false, false); return;
		case DTYPE_SPINNER:           createItem(MANA, danmaku->position, false, false); return;
		case DTYPE_RAIN:              for (int i = 0; i < 3; i++) createItem(MANA, danmaku->position + v2(rndFloat(-19, 19), rndFloat(-19, 19)), false, false); return;
		case DTYPE_SEEK:              for (int i = 0; i < 2; i++) createItem(MANA, danmaku->position + v2(rndFloat(-19, 19), rndFloat(-19, 19)), false, false); return;
		case DTYPE_ZSTRIKE:           createItem(MANA, danmaku->position, false, false); return;
		case DTYPE_OMNI:              for (int i = 0; i < 3; i++) createItem(MANA, danmaku->position + v2(rndFloat(-19, 19), rndFloat(-19, 19)), false, false); return;
		case DTYPE_BOSS_FLOWERGIRL:   createItem(FULL, danmaku->position, false, false); return;
		case DTYPE_BOSS_GURUGURUGIRL: createItem(FULL, danmaku->position, false, false); return;
		case DTYPE_BOSS_STARGIRL:     createItem(FULL, danmaku->position, false, false); return;
		case DTYPE_BOSS_THORNGIRL:    createItem(FULL, danmaku->position, false, false); return;
		case DTYPE_BOSS_KNIFEGIRL:    createItem(FULL, danmaku->position, false, false); return;
		case DTYPE_BOSS_WOLFGIRL:     createItem(FULL, danmaku->position, false, false); return;
		case DTYPE_SPECIAL_STAR:      createItem(STAR, danmaku->position, true,  true);  return;
		case DTYPE_SPECIAL_SNIPER:    createItem(STAR, danmaku->position, true,  true);  return;
		case DTYPE_SPECIAL_VINE:      createItem(STAR, danmaku->position, true,  true);  return;
		case DTYPE_SPECIAL_BARB:      createItem(STAR, danmaku->position, true,  true);  return;
		case DTYPE_SPECIAL_ORBITAR:   createItem(STAR, danmaku->position, true,  true);  return;
		case DTYPE_SPECIAL_EXPLODER:  createItem(STAR, danmaku->position, true,  true);  return;
		case DTYPE_SPECIAL_CHAIN:     createItem(STAR, danmaku->position, true,  true);  return;
		default: return;
	}
}

void eDanmakuUpdate(Danmaku *danmaku) {
	DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[danmaku->graphic];

	bool offScreen = false;
	if (danmaku->position.x + info->textureRadius < 0) offScreen = true;
	if (danmaku->position.x - info->textureRadius > game->width) offScreen = true;
	if (danmaku->position.y + info->textureRadius < 0) offScreen = true;
	if (danmaku->position.y - info->textureRadius > game->height) offScreen = true;

	if (propDanmakuRegen(danmaku->type)) {
		if (offScreen) {danmaku->health--;}
		else {danmaku->health = min(danmaku->health + 1, propDanmakuGetHealth(danmaku->type));}
	}
	else {
		if (offScreen && danmaku->frame > 90 && !propDanmakuPersistsOffscreen(danmaku->type)) danmaku->dead = true;
	}

	switch (danmaku->type) {
		case DTYPE_KOUMA_KANNON:      updateKoumaKannon(danmaku);      break;
		case DTYPE_SPREAD_ATTACKER:   updateSpreadAttacker(danmaku);   break;
		case DTYPE_MINE_DROPPER:      updateMineDropper(danmaku);      break;
		case DTYPE_SPINNER:           updateSpinner(danmaku);          break;
		case DTYPE_RAIN:              updateRain(danmaku);             break;
		case DTYPE_SEEK:              updateSeek(danmaku);             break;
		case DTYPE_ZSTRIKE:           updateZStrike(danmaku);          break;
		case DTYPE_OMNI:              updateOmni(danmaku);             break;
		case DTYPE_BOSS_FLOWERGIRL:   updateFlowerGirl(danmaku);       break;
		case DTYPE_BOSS_GURUGURUGIRL: updateGuruGuruGirl(danmaku);     break;
		case DTYPE_BOSS_STARGIRL:     updateStarGirl(danmaku);         break;
		case DTYPE_BOSS_THORNGIRL:    updateThornGirl(danmaku);        break;
		case DTYPE_BOSS_KNIFEGIRL:    updateKnifeGirl(danmaku);        break;
		case DTYPE_BOSS_WOLFGIRL:     updateWolfGirl(danmaku);         break;
		case DTYPE_SPECIAL_LASERADD:  updateSpecialLaserAdd(danmaku);  break;
		case DTYPE_SPECIAL_STAR:      updateSpecialStar(danmaku);      break;
		case DTYPE_SPECIAL_SNIPER:    updateSpecialSniper(danmaku);    break;
		case DTYPE_SPECIAL_VINE:      updateSpecialVine(danmaku);      break;
		case DTYPE_SPECIAL_BARB:      updateSpecialBarb(danmaku);      break;
		case DTYPE_SPECIAL_ORBITAR:   updateSpecialOrbitar(danmaku);   break;
		case DTYPE_SPECIAL_EXPLODER:  updateSpecialExploder(danmaku);  break;
		case DTYPE_SPECIAL_CHAIN:     updateSpecialChain(danmaku);     break;
		case DTYPE_SPELL_ATTACK:      updateSpellSwitchboard(danmaku); break;
		default:                                                       break;
	}

	if (danmaku->health <= 0) {danmaku->dead = true;}
}

void updateKoumaKannon(Danmaku *danmaku) {
	if (danmaku->frame % (100 - (danmaku->difficulty * 10)) == 0) {
		createShotA(danmaku->position, 4.6, degsBetween(danmaku->position, game->playerPosition), KOUMA, LIME, 3);
	}
}

void updateSpreadAttacker(Danmaku *danmaku) {
	if (danmaku->position.x < 0) return;
	if (danmaku->position.y < 0) return;
	if (danmaku->position.x > game->width) return;
	if (danmaku->position.y > game->height) return;

	if (danmaku->frame > 40) {
		if (danmaku->frame % ((9 - danmaku->difficulty) * 10) == 0) {
			float playerAngle = degsBetween(danmaku->position, game->playerPosition);
			createShotA(danmaku->position, 1.4 + (danmaku->difficulty / 3), playerAngle, BALL_S, RED, 3);
			for (int i = 0; i < (danmaku->difficulty + 1) / 2; i++) {
				createShotA(danmaku->position, 1.4 + (danmaku->difficulty / 3), playerAngle + ((35 - (danmaku->difficulty * 3)) * (i + 1)), BALL_S, RED, 3);
				createShotA(danmaku->position, 1.4 + (danmaku->difficulty / 3), playerAngle - ((35 - (danmaku->difficulty * 3)) * (i + 1)), BALL_S, RED, 3);
			}
		}
	}
}

void updateMineDropper(Danmaku *danmaku) {
	if (danmaku->position.x < 0) return;
	if (danmaku->position.y < 0) return;
	if (danmaku->position.x > game->width) return;
	if (danmaku->position.y > game->height) return;

	float angleOffset = rndFloat(0, 359);
	int frameTime = 8 * (10 - danmaku->difficulty);
	if (danmaku->frame % frameTime == frameTime - 1) {
		int maxNum = (danmaku->difficulty * 3) + 3;
		for (int i = 0; i < maxNum; i++) {
			createShotA(danmaku->position, 1.2, i * (360.0 / maxNum) + angleOffset, BALL_S, BLUE, 120);
		}
	}
}

void updateSpinner(Danmaku *danmaku) {
	int shotChannels = danmaku->difficulty * 0.5 + 3;
	int modNum = (9 - danmaku->difficulty);
	if (danmaku->frame % modNum == 0) {
		createShotA(danmaku->position, 3.4, ((((danmaku->frame / modNum) % shotChannels) * (360.0 / shotChannels)) + danmaku->frame) * (danmaku->angle + 90 < 180 ? 1 : -1), KOUMA, VIOLET, 16);
	}
}

void updateRain(Danmaku *danmaku) {
	int modNum = (42 - (danmaku->difficulty * 5));
	if (danmaku->frame % modNum == 0) {
		createShotA(danmaku->position + v2(rndFloat(-70 - (danmaku->difficulty * 6), 70 + (danmaku->difficulty * 6)), -32), rndFloat(3.4, 4.2), rndFloat(88, 92), rndBool() ? RICE_S : KOUMA, rndBool() ? CYAN : BLUE, 12);
	}
}

void updateSeek(Danmaku *danmaku) {
	float playerAngle = degsBetween(danmaku->position, game->playerPosition);
	int fireRate = 6;//danmaku->difficulty > 4 ? 12 : 6;
	float startSpeed = 2.2;
	int stopFrame = 100;
	int startingFrame = 50 - min(4, danmaku->difficulty) * 9;
	int endingFrame = 125 + min(4, danmaku->difficulty) * 9;

	if ((danmaku->frame < endingFrame) && (danmaku->frame > startingFrame) && (danmaku->frame % fireRate == 0)) {
		for (int i = 0; i < (danmaku->difficulty > 4 ? 3 : 1); i++) {
			Danmaku *tempShot = createShotA(danmaku->position, startSpeed, 0, RICE_S, GREEN, endingFrame - danmaku->frame);
			tempShot->speedAccel = -startSpeed / stopFrame;

			addModifier(tempShot, aimedDanmakuModifier, 0);

			DanmakuModifier modifier = newDanmakuModifier();
			modifier.speedAbsolute = true;
			modifier.speedAccelAbsolute = true;
			modifier.speedUpdate = 0;
			modifier.speedAccelUpdate = 0;
			addModifier(tempShot, modifier, stopFrame);

			modifier = newDanmakuModifier();
			modifier.speedAbsolute = true;
			modifier.speedUpdate = (danmaku->difficulty > 4) ? 2.2 : 3.6;
			addModifier(tempShot, modifier, stopFrame * 2);

			if (danmaku->difficulty > 4) {
				int shotCone = (5 - (danmaku->difficulty - 4)) * 30;
				modifier = newDanmakuModifier();
				modifier.angleUpdate = i * shotCone - shotCone;
				addModifier(tempShot, modifier, stopFrame * 2);
			}
		}
	}
}

void updateZStrike(Danmaku *danmaku) {
	int modNum = (42 - (danmaku->difficulty * 5));
	if (danmaku->frame % modNum == 0) {
		createShotA(danmaku->position + v2(rndFloat(-70 - (danmaku->difficulty * 6), 70 + (danmaku->difficulty * 6)), -32), rndFloat(3.4, 4.2), rndFloat(88, 92), rndBool() ? RICE_S : KOUMA, rndBool() ? CYAN : BLUE, 12);
	}
}

void updateOmni(Danmaku *danmaku) {
	int modNum = 3;//20 - danmaku->difficulty * 2;
	if (danmaku->frame % modNum == 0) {
		float flipWave = sin(danmaku->frame * 0.1) * 60;
		bool flipFlop = flipWave >= sin((danmaku->frame-1) * 0.1) * 60;
		int spokes = danmaku->difficulty + 2;
		for (int i = 0; i < spokes; i++) {
			float firingAngle = (i * (360 / spokes) + danmaku->frame * 0.65) * (flipFlop ? -1 : 1);
			float spawnDist = 40;
			Vec2 spawnPosition = v2(spawnDist * cos(toRad(firingAngle)), spawnDist * sin(toRad(firingAngle)));
			createShotA(danmaku->position + spawnPosition, flipFlop ? 3.6 : 3.2, firingAngle, RICE_S, flipFlop ? VIOLET : PURPLE, 8);
		}
	}
}

/*void updateOmniOld(Danmaku *danmaku) {
	int modNum = 3;//20 - danmaku->difficulty * 2;
	if (danmaku->frame % modNum == 0) {
		int flipFlopFreq = modNum * 5;
		bool flipFlop = danmaku->frame % (flipFlopFreq * 2) >= flipFlopFreq;
		int spokes = danmaku->difficulty + 2;
		for (int i = 0; i < spokes; i++) {
			float firingAngle = i * (360 / spokes) + danmaku->frame;
			Vec2 spawnPosition = v2(40 * cos(toRad(firingAngle)), 40 * sin(toRad(firingAngle)));
			createShotA(danmaku->position + spawnPosition, flipFlop ? 3.6 : 3.2, firingAngle + (flipFlop ? 0 : 180), RICE_S, flipFlop ? VIOLET : PURPLE, 8);
		}
	}
}*/

void updateFlowerGirl(Danmaku *danmaku) {
	Danmaku *tempShot;

	if (danmaku->difficulty == 1 || danmaku->difficulty == 4) {
		if (danmaku->frame % 120 == 119) {
			float playerAngle = degsBetween(danmaku->position, game->playerPosition);
			tempShot = createShotA(danmaku->position, 4.4, playerAngle + rndFloat(-8, 8), BALL_S, YELLOW, 16);
			tempShot->scale = 2;
			tempShot->type = DTYPE_SPECIAL_EXPLODER;
		}
	}

	if (danmaku->difficulty == 2 || danmaku->difficulty == 4) {
		if (danmaku->frame % 60 < 40) {
			createShotA(danmaku->position, 2.8 + (danmaku->frame % 2), danmaku->frame + ((danmaku->frame % 4) * 90), RICE_S, YELLOW, 16);
		}
		else {
			createShotA(danmaku->position, 4.6, -danmaku->frame + ((danmaku->frame % 2) * 180), RICE_S, YELLOW, 16)->scale = 2;
		}
	}

	if (danmaku->difficulty == 3 || danmaku->difficulty == 4) {
		int fireRate = 80;
		if (danmaku->frame % fireRate == fireRate - 1) {
			DanmakuModifier modifier;
			float playerAngle = degsBetween(danmaku->position, game->playerPosition);
			float randPos = rndFloat(0, 359);
			Vec2 posOffset = v2(cos(toRad(randPos)) * 60, sin(toRad(randPos)) * 60);
			for (int i = 0; i < 8; i++) {
				tempShot = createShotA(danmaku->position + posOffset, 1.6, playerAngle, RICE_S, YELLOW, 16);
				tempShot->spin = i*45;
				tempShot->spinAccel = 6;
				tempShot->scale = 2.4;
				tempShot->positionOffset.y = 8;

				modifier = newDanmakuModifier();
				modifier.aimed = true;
				modifier.speedUpdate = 1.4;
				addModifier(tempShot, modifier, (fireRate * 2) * 1);
				addModifier(tempShot, modifier, (fireRate * 2) * 2);
				addModifier(tempShot, modifier, (fireRate * 2) * 3);
			}
		}
	}

	if (danmaku->frame % 120 == 59) {
		Vec2 desiredLocation = v2(rndFloat(100, game->width - 100), rndFloat(0, game->height / 3));
		danmaku->speed = danmaku->position.distance(desiredLocation) / 60;
		danmaku->angle = degsBetween(danmaku->position, desiredLocation);
	}

	if (danmaku->frame % 120 == 119) {
		danmaku->speed = 0.3;
	}
}

void updateGuruGuruGirl(Danmaku *danmaku) {
	Vec2 orbitPos = v2(game->width / 2, game->width / 4);
	Danmaku *tempShot;
	DanmakuModifier modifier;

	if (danmaku->difficulty == 1 || danmaku->difficulty == 4) {
		if (danmaku->frame % 4 == 0) {
			for (int i = 0; i < 10; i++) {
				tempShot = createShotA(orbitPos, 5.2, (15 * sin(danmaku->frame / 75.0)) + (i * 36), RICE_S, INDIGO, 5);
				tempShot->angleAccel = -4;

				modifier = newDanmakuModifier();
				modifier.angleAccelAbsolute = true;
				modifier.angleAccelUpdate = 0;
				addModifier(tempShot, modifier, 90);
			}
		}
	}

	if (danmaku->difficulty == 2 || danmaku->difficulty == 4) {
		if (danmaku->frame % 50 == 49) {
			int circleDensity = 12;
			bool flipFlop = (danmaku->frame / 50) % 2 == 0;
			float rando = rndFloat(0, 359);
			for (int i = 0; i < circleDensity; i++) {
				float calcAng = i * (360.0 / circleDensity) + rando;
				Vec2 posOffset = v2(cos(toRad(calcAng)) * 80, sin(toRad(calcAng)) * 80);
				tempShot = createShotA(game->playerPosition + posOffset, 0.8, calcAng + (flipFlop ? 80 : -80), BALL_S, CYAN, 30);
				tempShot->angleAccel = flipFlop ? -0.4 : 0.4;

				modifier = newDanmakuModifier();
				modifier.angleAccelAbsolute = true;
				modifier.angleAccelUpdate = 0;
				addModifier(tempShot, modifier, 180);
			}
		}
	}

	if (danmaku->difficulty == 3 || danmaku->difficulty == 4) {
		if (danmaku->frame % 30 == 0) {
			for (int i = 0; i < 24; i++) {
				createShotA(danmaku->position, 2.6, i * (360 / 24.0), RICE_S, BLUE, 5)->scale = 2;
			}
		}
	}

	float sineWaver = sin(danmaku->frame / 80.0);
	float doubleSineWaver = sin((danmaku->frame / 80.0) * 2);
	danmaku->position = orbitPos + (v2(sineWaver, doubleSineWaver * 0.4) * 140);
}

void updateStarGirl(Danmaku *danmaku) {
	Danmaku *tempShot;

	if (danmaku->difficulty == 1 || danmaku->difficulty == 4) {
		if (danmaku->frame % 15 == 0) {
			tempShot = createShotA(v2(rndFloat(0.0, game->width), 10.0), 0.3, rndFloat(75.0, 105.0), STAR_S, MAGENTA, 3);
			tempShot->speedAccel = 0.06;
			tempShot->spinAccel = -3.0;
		}
	}

	if (danmaku->difficulty == 2 || danmaku->difficulty == 4) {
		if (danmaku->frame % 85 == 0) {
			tempShot = createShotA(v2(danmaku->position.x, 10.0), 0.3, rndFloat(75.0, 105.0), STAR_S, YELLOW, 3);
			tempShot->type = DTYPE_SPECIAL_STAR;
			tempShot->speedAccel = 0.1;
			tempShot->spinAccel = -5.0;
			tempShot->scale = rndFloat(0.8, 2.4);
		}
	}

	if (danmaku->difficulty == 3 || danmaku->difficulty == 4) {
		if (danmaku->frame % 170 == 42) {
			tempShot = createShotA(danmaku->position, 3.4, rndFloat(0, 359), STAR_S, YELLOW, 10);
			tempShot->type = DTYPE_SPECIAL_SNIPER;
			tempShot->speedAccel = -0.034;
			tempShot->angleAccel = rndBool() ? 2.4 : -2.4;
			tempShot->spinAccel = -5.0;
			tempShot->flags |= _F_DF_BLEND_ADD;

			DanmakuModifier modifier = newDanmakuModifier();
			modifier.speedAccelAbsolute = true;
			addModifier(tempShot, modifier, 100);
		}
	}

	Vec2 orbitPos = v2(game->width / 2, game->width / 4);
	float sineWaver = sin(danmaku->frame / 50.0);
	float doubleSineWaver = sin((danmaku->frame / 50.0) * 2);
	danmaku->position = orbitPos + (v2(sineWaver, doubleSineWaver * 0.3) * 160);
}

void updateThornGirl(Danmaku *danmaku) {
	Danmaku *tempShot;

	if (danmaku->difficulty == 1 || danmaku->difficulty == 4) {
		if (danmaku->frame % 70 == 0) createShotA(v2(rndFloat(0.0, game->width), game->height + 10), 0.6, rndFloat(200, 340), BALL_S, GREEN, 0);
	}

	if (danmaku->difficulty == 2 || danmaku->difficulty == 4) {
		if (danmaku->frame % 300 == 100) {
			float playerAngle = degsBetween(danmaku->position, game->playerPosition);
			tempShot = createShotA(danmaku->position, 8.2, playerAngle, BALL_S, GREEN, 3);
			tempShot->type = DTYPE_SPECIAL_VINE;
			if ((danmaku->frame / 300) % 2 == 0) {
				tempShot->angle -= 12;
				tempShot->angleAccel = 0.6;
			}
			else {
				tempShot->angle += 12;
				tempShot->angleAccel = -0.6;
			}
		}
	}

	if (danmaku->difficulty == 3 || danmaku->difficulty == 4) {
		if (danmaku->frame % 150 == 50) {
			float playerAngle = degsBetween(danmaku->position, game->playerPosition);
			createShotA(danmaku->position, 1.4, playerAngle, BALL_S, GREEN, 60);
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					tempShot = createShotA(danmaku->position, 1.4, playerAngle, BALL_S, GREEN, 60);
					tempShot->positionOffset.x = 90 * j;
					tempShot->positionOffset.y = 30 * (i + 1);
					tempShot->scale = 2;
					tempShot->spinAccel = (danmaku->frame / 150) % 2 ? -1 : 1;
				}
			}
		}
	}

	if (danmaku->frame % 300 == 90) {
		Vec2 desiredLocation = v2(rndFloat(230, game->width - 230), rndFloat(65, 125));
		danmaku->speed = danmaku->position.distance(desiredLocation) / 15;
		danmaku->angle = degsBetween(danmaku->position, desiredLocation);
	}

	if (danmaku->frame % 300 == 105) {
		danmaku->speed = 0;
	}
}

void updateKnifeGirl(Danmaku *danmaku) {
	Danmaku *tempShot;
	DanmakuModifier modifier;

	if (danmaku->difficulty == 1 || danmaku->difficulty == 4) {
		if (rndBool()) tempShot = createShotA(v2(game->width + 10, rndFloat(0, game->height)), rndFloat(1.2, 1.8), 180, KOUMA, RED, 3);
		else tempShot = createShotA(v2(-10, rndFloat(0, game->height)), rndFloat(0.8, 3.4), 0, KOUMA, RED, 3);
		tempShot->angleAccel = rndBool() ? 1.8 : -1.8;

		modifier = newDanmakuModifier();
		modifier.die = true;
		addModifier(tempShot, modifier, 100);
	}

	if (danmaku->difficulty == 2 || danmaku->difficulty == 4) {
		if (danmaku->frame % 8 == 7) {
			float randDist = rndFloat(20, 85);
			float randPos = rndFloat(0, 359);
			Vec2 posOffset = v2(cos(toRad(randPos)) * randDist, sin(toRad(randPos)) * randDist);
			tempShot = createShotA(danmaku->position + posOffset, 0.8, 270, KOUMA, RED, 16);
			tempShot->spinAccel = 16;

			modifier = newDanmakuModifier();
			modifier.speedUpdate = 2.4;
			modifier.speedAccelUpdate = 0.08;
			modifier.spinAbsolute = true;
			modifier.spinUpdate = 0;
			modifier.spinAccelAbsolute = true;
			modifier.spinAccelUpdate = 0;
			modifier.aimed = true;
			addModifier(tempShot, modifier, 50);
		}
	}

	if (danmaku->difficulty == 3 || danmaku->difficulty == 4) {
		int circleDensity = 18;
		float tighteningAngle = 45.0;
		bool flipFlop = (danmaku->frame / 24) % 2 == 0;
		if (danmaku->frame % 24 == 23) {
			for (int i = 0; i < circleDensity; i++) {
				float calcAng = i * (360.0 / circleDensity);
				Vec2 posOffset = v2(cos(toRad(calcAng)) * 40, sin(toRad(calcAng)) * 40);
				tempShot = createShotA(danmaku->position + posOffset, 1.4, calcAng + (flipFlop ? tighteningAngle : -tighteningAngle), KOUMA, ORANGE, 16);

				modifier = newDanmakuModifier();
				modifier.speedUpdate = 0.6;
				modifier.angleAbsolute = true;
				modifier.angleUpdate = calcAng + (flipFlop ? -tighteningAngle : tighteningAngle);
				addModifier(tempShot, modifier, 10);
				modifier.angleUpdate = calcAng + (flipFlop ? tighteningAngle : -tighteningAngle);
				addModifier(tempShot, modifier, 20);
				modifier.angleUpdate = calcAng + (flipFlop ? -tighteningAngle : tighteningAngle);
				addModifier(tempShot, modifier, 30);
				modifier.angleUpdate = calcAng + (flipFlop ? tighteningAngle : -tighteningAngle);
				addModifier(tempShot, modifier, 40);
				modifier.angleUpdate = calcAng + (flipFlop ? -tighteningAngle : tighteningAngle);
				addModifier(tempShot, modifier, 50);
				modifier.angleUpdate = calcAng + (flipFlop ? tighteningAngle : -tighteningAngle);
				addModifier(tempShot, modifier, 60);
			}
		}
	}

	if (danmaku->frame % 120 == 59) {
		Vec2 desiredLocation = v2(rndFloat(180, game->width - 180), rndFloat(45, game->height / 3));
		danmaku->speed = danmaku->position.distance(desiredLocation) / 60;
		danmaku->angle = degsBetween(danmaku->position, desiredLocation);
	}

	if (danmaku->frame % 120 == 119) {
		danmaku->speed = 0.3;
	}
}

void updateWolfGirl (Danmaku *danmaku) {
	int phaseFrames = 140;
	if (danmaku->difficulty == 1 || danmaku->difficulty == 4) {
		int fireRate = 60/12;
		float playerAngle = degsBetween(danmaku->position, game->playerPosition);
		if (danmaku->frame % phaseFrames * 2 == 0) {
			for (int i = 0; i < phaseFrames / fireRate; i++) {
				createShotA(danmaku->position, rndFloat(2.2, 3.2), playerAngle + rndFloat(-12, 12), BALL_S, CYAN, i * 4);
			}
		}
	}

	if (danmaku->difficulty == 2 || danmaku->difficulty == 4) {
		if ((danmaku->frame % phaseFrames > phaseFrames - 61) && (danmaku->frame % 15 == 0)) {
			DanmakuModifier modifier = newDanmakuModifier();
			modifier.alphaAccelUpdate = -0.02;
			Vec2 footPrintCenter = danmaku->position + v2(danmaku->frame % 30 == 0 ? 16 : -16, 0);
			float footPrintAngle = danmaku->angle;
			Danmaku *tempShot = createShotA(footPrintCenter + v2(cos(toRad(footPrintAngle)), sin(toRad(footPrintAngle))) * 16.0, 0, footPrintAngle, RICE_S, BLUE, 0);
			addModifier(tempShot, modifier, 1000);
			addModifier(tempShot, deathDanmakuModifier, 1050);
			footPrintAngle -= 30;
			tempShot = createShotA(footPrintCenter + v2(cos(toRad(footPrintAngle)), sin(toRad(footPrintAngle))) * 16.0, 0, footPrintAngle, RICE_S, BLUE, 0);
			addModifier(tempShot, modifier, 1000);
			addModifier(tempShot, deathDanmakuModifier, 1050);
			footPrintAngle += 60;
			tempShot = createShotA(footPrintCenter + v2(cos(toRad(footPrintAngle)), sin(toRad(footPrintAngle))) * 16.0, 0, footPrintAngle, RICE_S, BLUE, 0);
			addModifier(tempShot, modifier, 1000);
			addModifier(tempShot, deathDanmakuModifier, 1050);
			tempShot = createShotA(footPrintCenter, 0, 0, BALL_S, BLUE, 0);
			addModifier(tempShot, modifier, 1000);
			addModifier(tempShot, deathDanmakuModifier, 1050);
		}
	}

	if (danmaku->difficulty == 3 || danmaku->difficulty == 4) {
		int framesPerShot = 10;
		int shotCount = 6;
		int totalFrames = framesPerShot * shotCount;
		float windowSlice = game->width / shotCount;
		if (danmaku->frame % (totalFrames * 9) < totalFrames) {
			if (danmaku->frame % framesPerShot == 0) {
				Danmaku *tempShot = createShotB(danmaku->position, v2(((danmaku->frame % totalFrames) / framesPerShot) * windowSlice + (windowSlice / 2), 0), 80, BALL_S, PURPLE, 16);
				tempShot->type = DTYPE_SPECIAL_CHAIN;
				addModifier(tempShot, aimedDanmakuModifier, 79);
			}
		}
	}

	if (danmaku->frame % phaseFrames == phaseFrames - 61) {
		Vec2 desiredLocation = game->playerPosition + v2(rndFloat(-80, 80), rndFloat(-80, 80));
		danmaku->speed = danmaku->position.distance(desiredLocation) / 60;
		danmaku->angle = degsBetween(danmaku->position, desiredLocation);
	}

	if (danmaku->frame % phaseFrames == phaseFrames - 1) {
		danmaku->speed = 0.5;
	}
}

void updateSpecialLaserAdd(Danmaku *danmaku) {
	danmaku->position = game->playerPosition;
	//This only works if dtype updates happen before shot offset updates. =3
}

void updateSpecialStar(Danmaku *danmaku) {
	if (danmaku->frame % 5 == 0) {
		Danmaku *tempShot = createShotA(danmaku->position, 4.2, rndFloat(-10, 10) + danmaku->angle + 180, BALL_S, YELLOW, 16);
		tempShot->scale = danmaku->scale;
		tempShot->spinAccel = -3.0;
	}
}

void updateSpecialSniper(Danmaku *danmaku) {
	if (danmaku->frame == 150) addModifier(createShotA(danmaku->position, 0, 0, BALL_S, danmaku->color, 10), deathDanmakuModifier, 0);

	if (danmaku->frame == 160) {
		int maxLength = v2().distance(v2(game->width, game->height)) / 5;
		for (int i = 0; i < maxLength; i++) {
			Danmaku *tempShot = createShotA(lerp(danmaku->position, game->playerPosition, (i * 5) * (1.0 / maxLength)), 1.2, rndFloat(0, 359), BALL_S, danmaku->color, 16);
			tempShot->alphaAccel = -0.02;
			addModifier(tempShot, deathDanmakuModifier, 50);
		}
		danmaku->dead = true;
	}
}

void updateSpecialVine(Danmaku *danmaku) {
	bool makeBarb = danmaku->frame % 15 == 0;
	Danmaku *tempShot = createShotA(danmaku->position, 1.0, danmaku->angle + (danmaku->frame * 40.0), RICE_S, makeBarb ? RED : GREEN, 40);
	if (makeBarb) {
		tempShot->type = DTYPE_SPECIAL_BARB;
		tempShot->scale = 1.4;
	}

	addModifier(tempShot, freezeDanmakuModifier, makeBarb ? 4 : 2);
	addModifier(tempShot, deathDanmakuModifier, 300);
}

void updateSpecialBarb(Danmaku *danmaku) {
	if (danmaku->frame % 60 == 59) {
		Danmaku *tempShot = createShotA(danmaku->position, 0.6, rndFloat(-50, 50) + danmaku->angle, BALL_S, RED, rndInt(1, 9));
	}
}

void updateSpecialOrbitar(Danmaku *danmaku) {
	Danmaku *tempShot;

	tempShot = createShotA(danmaku->position, 35, danmaku->spin, BALL_S, GREEN, 4);
	addModifier(tempShot, deathDanmakuModifier, 3);
	tempShot = createShotA(danmaku->position, 35, danmaku->spin + 90, BALL_S, GREEN, 4);
	addModifier(tempShot, deathDanmakuModifier, 3);
	tempShot = createShotA(danmaku->position, 35, danmaku->spin + 180, BALL_S, GREEN, 4);
	addModifier(tempShot, deathDanmakuModifier, 3);
	tempShot = createShotA(danmaku->position, 35, danmaku->spin + 270, BALL_S, GREEN, 4);
	addModifier(tempShot, deathDanmakuModifier, 3);
}

void updateSpecialExploder(Danmaku *danmaku) {
	Danmaku *tempShot;

	//createParticle(danmaku->position + v2(rndFloat(-3, 3), rndFloat(-3, 3)), rndFloat(0.4, 1.6), rndFloat(0, M_PI * 2), 40, (ColorName)rndInt(0, 2));
	//createParticle(danmaku->position + v2(rndFloat(-3, 3), rndFloat(-3, 3)), rndFloat(0.4, 1.6), rndFloat(0, M_PI * 2), 40, (ColorName)rndInt(0, 2));
	//createParticle(danmaku->position + v2(rndFloat(-3, 3), rndFloat(-3, 3)), rndFloat(0.4, 1.6), rndFloat(0, M_PI * 2), 40, (ColorName)rndInt(0, 2));

	if (danmaku->position.distance(game->playerPosition) < 80) {
		for (int i = 0; i < 12; i++) {
			tempShot = createShotA(danmaku->position, 4.2, i * 30.0, RICE_S, danmaku->color, 8);
			tempShot->angleAccel = (i % 2 == 0) ? 7.5 : -7.5;
			addModifier(tempShot, deathDanmakuModifier, 48);
		}
		danmaku->dead = true;
	}
}

void updateSpecialChain(Danmaku *danmaku) {
	if (danmaku->frame == 80) {
		float maxLength = v2().distance(v2(game->width, game->height));
		Vec2 dest = v2(cos(toRad(danmaku->angle + rndFloat(-9, 9))) * maxLength, sin(toRad(danmaku->angle + rndFloat(-9, 9))) * maxLength) + danmaku->position;
		int chainSize = rndInt(14, 18);

		DanmakuModifier modifier = newDanmakuModifier();
		modifier.speedAccelUpdate = 0.02;

		for (int i = 1; i < chainSize + 1; i++) {
			Danmaku *tempShot = createShotB(danmaku->position, lerp(danmaku->position, dest, (float)i / chainSize), 80, KOUMA, PURPLE, 16);
			addModifier(tempShot, freezeDanmakuModifier, 80);
			addModifier(tempShot, modifier, 280);
		}

		danmaku->dead = true;
	}
}

void updateSpellSwitchboard(Danmaku *danmaku) {
	switch (danmaku->difficulty) {
		case 1: updateSpellSun(danmaku);       break;
		case 2: updateSpellRainbow(danmaku);   break;
		case 3: updateSpellNarrowing(danmaku); break;
		case 4: updateSpellMissile(danmaku);   break;
		case 5: updateSpellXAimed(danmaku);    break;
		case 6: updateSpellSpooky(danmaku);    break;
		case 7: updateSpellSand(danmaku);      break;
		case 8: updateSpellBlade(danmaku);     break;
	}
}

void updateSpellSun(Danmaku *danmaku) {
	danmaku->position = v2(rndFloat(80, game->width - 80), rndFloat(80, game->height - 80));
	float randOff = rndFloat(0, 359);
	int spokeCount = 32;
	float circleCut = (360.0 / spokeCount);
	for (int i = 0; i < spokeCount; i++) {
		Danmaku *tempShot = createShotA(danmaku->position, 0.8, randOff + i * circleCut, RICE_S, i % 2 ? YELLOW : LIME, i % 2 ? 48 : 36);
		tempShot->angleAccel = (i % 2 == 0) ? 6 : -6;
		DanmakuModifier modifier = newDanmakuModifier();
		modifier.speedUpdate = 1.2;
		modifier.repeat = true;
		addModifier(tempShot, modifier, 60);
		addModifier(tempShot, deathDanmakuModifier, 360);
	}
	danmaku->dead = true;
}

void updateSpellRainbow(Danmaku *danmaku) {
	if (danmaku->frame == 0) danmaku->position = v2(rndFloat(0, game->width), rndFloat(0, game->height / 2));
	if (danmaku->frame % 15 == 0) {
		float rando = rndFloat(0, 359);
		for (int i = 0; i < 12; i++) {
			createShotA(danmaku->position + v2(cos(i * 30.0), sin(i * 30.0)) * 60.0, 1.2, rando + i * 30.0, BALL_S, (ColorName) i, 8)->speedAccel = 0.01;
		}
	}
	if (danmaku->frame == 61) danmaku->dead = true;
}

void updateSpellNarrowing(Danmaku *danmaku) {
	if (danmaku->frame == 0) {
		danmaku->position = v2(rndFloat(80, game->width - 80), rndFloat(80, game->height / 2 - 80));
		danmaku->angle = degsBetween(danmaku->position, game->playerPosition);
	}

	int awawa = danmaku->frame % 3;
	createShotA(danmaku->position, 3.8 + awawa, danmaku->angle + (180 - danmaku->frame * 2) * ((danmaku->frame % 2) ? -1 : 1), SHAKU, TEAL, 8 * (1 + awawa));
	
	if (danmaku->frame == 20) createShotA(danmaku->position, 1.8, danmaku->angle, KOUMA, BLUE, 60)->scale = 2.5;

	if (danmaku->frame == 80) danmaku->dead = true;
}

void updateSpellMissile(Danmaku *danmaku) {
	danmaku->position = v2(rndFloat(0, game->width), rndFloat(0, game->height / 2));
	float randOff = rndFloat(0, 2 * M_PI);
	int missileCount = 5;
	float circleCut = (2 * M_PI / missileCount);
	for (int i = 0; i < missileCount; i++) {
		Vec2 shotOffset = v2(cos(randOff + i * circleCut) * 50, sin(randOff + i * circleCut) * 50);
		Danmaku *tempShot = createShotA(danmaku->position + shotOffset, 0.2, 270, MISSILE, RED, 120 + i * 16);
		tempShot->scale = 6.4;
		tempShot->speedAccel = 0.24;
		addModifier(tempShot, aimedDanmakuModifier, 0);
	}
	danmaku->dead = true;
}

void updateSpellXAimed(Danmaku *danmaku) {
	danmaku->position = v2(rndFloat(80, game->width - 80), rndFloat(80, game->height / 2 - 80));

	float fireAngle = degsBetween(danmaku->position, game->playerPosition);
	int streamCount = 13 * 2 + 1;
	float streamCone = 12;
	fireAngle -= (streamCount / 2) * streamCone;

	DanmakuModifier modifier = newDanmakuModifier();
	modifier.angleAccelAbsolute = true;
	modifier.angleAccelUpdate = 0;

	for (int i = 0; i < streamCount; i++) {
		Danmaku *tempShot = createShotA(danmaku->position, 2.2 + (abs((streamCount / 2 + 0) - i) * 0.8), fireAngle + i * streamCone, SCALE, MAGENTA, 100);
		if (i != streamCount / 2) {
			tempShot->angleAccel = (i < streamCount / 2) ? 1.2 : -1.2;
			addModifier(tempShot, modifier, 80);
		}
	}

	for (int i = 0; i < streamCount; i++) {
		Danmaku *tempShot = createShotA(danmaku->position, 2.6 + (abs((streamCount / 2 + 0) - i) * 0.7), fireAngle + i * streamCone, SCALE, MAGENTA, 100);
		if (i != streamCount / 2) {
			tempShot->angleAccel = (i < streamCount / 2) ? 1.2 : -1.2;
			addModifier(tempShot, modifier, 80);
		}
	}
	danmaku->dead = true;
}

void updateSpellSpooky(Danmaku *danmaku) {
	if (danmaku->frame == 0) danmaku->position = v2(rndFloat(140, game->width - 140), rndFloat(140, game->height - 140));

	DanmakuModifier sModifier = newDanmakuModifier();
	sModifier.speedAccelUpdate = 0.08;

	DanmakuModifier aModifier = newDanmakuModifier();
	aModifier.angleAccelAbsolute = true;
	aModifier.angleAccelUpdate = 0;

	int streamCount = 12;
	int currentStream = danmaku->frame % streamCount;

	float finalAngle = (danmaku->frame * 0.4) + currentStream * (360 / streamCount);
	float finalDist = 60 + min(danmaku->frame * 1.4, 120);

	finalAngle *= (danmaku->position.x < game->width / 2) ? -1 : 1;

	Danmaku *tempShot = createShotA(danmaku->position + v2(cos(toRad(finalAngle)), sin(toRad(finalAngle))) * finalDist, 1.2, finalAngle + rndFloat(-8, 8), KOUMA, PURPLE, 130);
	tempShot->angleAccel = 8;
	tempShot->flags |= _F_DF_BLEND_ADD;
	addModifier(tempShot, aModifier, 360 / abs(tempShot->angleAccel));
	addModifier(tempShot, sModifier, 360 / abs(tempShot->angleAccel));

	if (danmaku->frame == 240) danmaku->dead = true;
}

void updateSpellSand(Danmaku *danmaku) {
	Danmaku *tempShot;
	DanmakuModifier modifier;

	if (danmaku->frame == 0) danmaku->position = v2(rndFloat(20, game->width - 20), rndFloat(20, game->height / 2 - 20));
	if (danmaku->frame < 90) {
		int iteration = (danmaku->frame % 4) * 90;
		bool flipflop = danmaku->frame % 2;
		Vec2 shotOffset = v2(cos(toRad(((danmaku->frame * 2) + iteration) % 360)) * 50, sin(toRad(((danmaku->frame * 2) + iteration) % 360)) * 50);
		tempShot = createShotA(danmaku->position + shotOffset, 0.0, degsBetween(shotOffset, v2()), KOUMA, (ColorName) (ORANGE + flipflop), 30);

		modifier = newDanmakuModifier();
		modifier.speedUpdate = 0.7 - (flipflop * 0.1);
		modifier.speedAccelUpdate = 0.01;
		modifier.angleAccelUpdate = (flipflop - 0.5) * 0.6;
		addModifier(tempShot, modifier, 180 - danmaku->frame);
	}
	else {
		danmaku->dead = true;
	}
}

void updateSpellBlade(Danmaku *danmaku) {
	Danmaku *tempShot;
	DanmakuModifier modifier;

	if (danmaku->frame == 0) danmaku->position = v2(rndFloat(20, game->width - 20), rndFloat(20, game->height / 2 - 20));

	if (danmaku->frame * 3 < game->width) {
		if (danmaku->frame % 6 == 5) {
			if (danmaku->position.x < game->width / 2) tempShot = createShotA(v2(danmaku->frame * 3, 50), -1.4, rndFloat(-3, 3) + 90, KOUMA, BLUE, 50);
			else tempShot = createShotA(v2(game->width - danmaku->frame * 3, 50), -1.4, rndFloat(-3, 3) + 90, KOUMA, BLUE, 50);
			tempShot->scale = 1.8;
			tempShot->speedAccel = 0.05;

			modifier = newDanmakuModifier();
			modifier.speedUpdate = 0.8;
			modifier.speedAccelUpdate = 0.1;
			addModifier(tempShot, modifier, 30);
		}
	}
	else {
		danmaku->dead = true;
	}
}
















