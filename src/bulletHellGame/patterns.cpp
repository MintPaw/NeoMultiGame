void updatePattern(int pattern, PatternData *data);
void playerShoot(Character shotStyle, int attackLevel, bool shooting);
void playerShotAddendums(int shotStyle, int attackLevel, bool shooting);
void playerBomb();
void enemyWaveA(PatternData *data, int subType);
void enemyWaveB(PatternData *data, int subType);
void enemyWaveC(PatternData *data, int subType);
void enemyWaveD(PatternData *data, int subType);
void enemyWaveE(PatternData *data, int subType);
void enemyWaveF(PatternData *data, int subType);
void enemyWaveG(PatternData *data, int subType);
//void enemyWaveH(PatternData *data, int subType);
void enemyWaveI(PatternData *data, int subType);
void justForFun(PatternData *data, int subType);
/// FUNCTIONS ^

void updatePattern(int pattern, PatternData *data) {
	//pattern = pattern == 99 ? rndInt(1, 20) : pattern;
	switch (pattern) {
		case 1:  enemyWaveB(data, 1); break;
		case 2:  enemyWaveB(data, 2); break;
		case 3:  enemyWaveB(data, 3); break;
		case 4:  enemyWaveB(data, 4); break;
		case 5:  enemyWaveC(data, 1); break;
		case 6:  enemyWaveC(data, 2); break;
		case 7:  enemyWaveC(data, 3); break;
		case 8:  enemyWaveC(data, 4); break;
		case 9:  enemyWaveD(data, 1); break;
		case 10: enemyWaveD(data, 2); break;
		case 11: enemyWaveD(data, 3); break;
		case 12: enemyWaveD(data, 4); break;
		case 13: enemyWaveE(data, 0); break;
		case 14: enemyWaveE(data, 0); break;
		case 15: enemyWaveE(data, 0); break;
		case 16: enemyWaveE(data, 0); break;
		case 17: enemyWaveF(data, 1); break;
		case 18: enemyWaveF(data, 2); break;
		case 19: enemyWaveF(data, 3); break;
		case 20: enemyWaveF(data, 4); break;
		case 21: enemyWaveG(data, 1); break;
		case 22: enemyWaveG(data, 2); break;
		case 23: enemyWaveG(data, 3); break;
		case 24: enemyWaveG(data, 4); break;
	//	case 25: enemyWaveH(data, 1); break;
	//	case 26: enemyWaveH(data, 2); break;
	//	case 27: enemyWaveH(data, 3); break;
	//	case 28: enemyWaveH(data, 4); break;
		case 29: enemyWaveI(data, 1); break;
		case 30: enemyWaveI(data, 2); break;
		case 31: enemyWaveI(data, 3); break;
		case 32: enemyWaveI(data, 4); break;

		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79: justForFun(data, 0); break;

		case 80: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, 1); break;
		case 81: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, 2); break;
		case 82: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, 3); break;
		case 83: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, 4); break;
		case 84: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, 5); break;
		case 85: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, 6); break;
		case 86: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, 7); break;
		case 87: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, 8); break;
		case 88:
		case 89: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_SPELL_ATTACK, rndInt(1, 8)); break;

		case 90: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_BOSS_FLOWERGIRL, data->difficulty); break;
		case 91: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_BOSS_GURUGURUGIRL, data->difficulty); break;
		case 92: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_BOSS_STARGIRL, data->difficulty); break;
		case 93: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_BOSS_THORNGIRL, data->difficulty); break;
		case 94: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_BOSS_KNIFEGIRL, data->difficulty); break;
		case 95: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, DTYPE_BOSS_WOLFGIRL, data->difficulty); break;
		case 96:
		case 97:
		case 98: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, (DanmakuType)rndInt(DTYPE_BOSS_FLOWERGIRL, DTYPE_BOSS_WOLFGIRL), data->difficulty); break;
		case 99: if(data->frame == 0) createShotU(v2(game->width / 2, game->height / 5), 0, 0, (DanmakuType)rndInt(DTYPE_BOSS_FLOWERGIRL, DTYPE_BOSS_WOLFGIRL), 4); break;
	}
}

void playerShoot(Character shotStyle, int attackLevel, bool shooting) {
	switch (shotStyle) {
		case CHARACTER_DYNAMIC: {
			if (shooting) {
				createShotP(v2(game->playerPosition.x, game->playerPosition.y - 24), 9.8 + (game->focusLevel * 0.1), 270, TWIN, GREEN);
				for (int i = 1; i < attackLevel + 1; i++) {
					createShotP(v2(game->playerPosition.x + (i * 12 + game->focusLevel * 0.5), game->playerPosition.y - (24 - i * 8)), 9.8 + (game->focusLevel * 0.1), (270 + i * 6) - (game->focusLevel * (0.25 + 0.25 * i)), TWIN, GREEN);
					createShotP(v2(game->playerPosition.x - (i * 12 + game->focusLevel * 0.5), game->playerPosition.y - (24 - i * 8)), 9.8 + (game->focusLevel * 0.1), (270 - i * 6) + (game->focusLevel * (0.25 + 0.25 * i)), TWIN, GREEN);
				}
			}
		} break;
		case CHARACTER_TURRET: {
			if (game->focusLevel == 0) {
				if (game->playerOptions[0].distance(game->playerPosition)   > 30) game->playerOptions[0] = moveTowards(game->playerOptions[0], game->playerPosition,   5);
				if (game->playerOptions[1].distance(game->playerOptions[0]) > 30) game->playerOptions[1] = moveTowards(game->playerOptions[1], game->playerOptions[0], 5);
				if (game->playerOptions[2].distance(game->playerOptions[1]) > 30) game->playerOptions[2] = moveTowards(game->playerOptions[2], game->playerOptions[1], 5);
			}
			if (attackLevel == 0) {
				if (shooting) createShotP(v2(game->playerPosition.x, game->playerPosition.y - 24), 8.4, 270, TWIN, GREEN);
			}
			if (attackLevel == 1) {
				if (shooting) createShotP(v2(game->playerPosition.x + 8, game->playerPosition.y - 24), 8.4, 270, TWIN, GREEN);
				if (shooting) createShotP(v2(game->playerPosition.x - 8, game->playerPosition.y - 24), 8.4, 270, TWIN, GREEN);
			}
			if (attackLevel == 2) {
				if (shooting) createShotP(v2(game->playerPosition.x, game->playerPosition.y - 24), 8.4, 270, TWIN, GREEN);
				if (shooting) createShotP(v2(game->playerPosition.x + 12, game->playerPosition.y - 24), 8.4, 274, TWIN, GREEN);
				if (shooting) createShotP(v2(game->playerPosition.x - 12, game->playerPosition.y - 24), 8.4, 266, TWIN, GREEN);
			}
			if (attackLevel == 3) {
				if (shooting) createShotP(v2(game->playerPosition.x + 6, game->playerPosition.y - 24), 8.4, 271, TWIN, GREEN);
				if (shooting) createShotP(v2(game->playerPosition.x - 6, game->playerPosition.y - 24), 8.4, 269, TWIN, GREEN);
				if (shooting) createShotP(v2(game->playerPosition.x + 12, game->playerPosition.y - 24), 8.4, 273, TWIN, GREEN);
				if (shooting) createShotP(v2(game->playerPosition.x - 12, game->playerPosition.y - 24), 8.4, 267, TWIN, GREEN);
			}
			for (int i = 0; i < min(3, attackLevel); i++) {
				if (shooting) createShotP(v2(game->playerOptions[i].x, game->playerOptions[i].y - 15), 8.4, 270, TWIN, GREEN);
				drawCircle(game->playerOptions[i], 3, 0xFFE0AAFF);
			}
		} break;
		case CHARACTER_PRETTY_WAVE: {
			if (shooting) {
				for (int i = 0; i < attackLevel * 2 + 1; i++) {
					float waveAngle;
					if (i == 0) waveAngle = 0;
					if (i == 1) waveAngle = sin(game->frameCount * 0.05) *  15;
					if (i == 2) waveAngle = sin(game->frameCount * 0.05) * -15;
					if (i == 3) waveAngle = sin(game->frameCount * 0.04) *  40;
					if (i == 4) waveAngle = sin(game->frameCount * 0.04) * -40;
					if (i == 5) waveAngle = sin(game->frameCount * 0.03) *  80;
					if (i == 6) waveAngle = sin(game->frameCount * 0.03) * -80;
					waveAngle /= (game->focusLevel / 8.0 + 1);
					waveAngle += 270;
					createShotP(game->playerPosition + v2(cos(toRad(waveAngle)), sin(toRad(waveAngle))) * 15, 9.2, waveAngle, TWIN, GREEN);
				}
			}
		} break;
		case CHARACTER_DIRECTION_DRIFT: {
			if (shooting) {
				int shotStreams = attackLevel * 2 + 1;
				float shotAngle = 270 + game->playerMoveTrail.x * 32;
				float shotCone = 4.5 + game->playerMoveTrail.y * 4;

				shotAngle -= attackLevel * shotCone;
				for (int i = 0; i < shotStreams; i++) {
					createShotP(game->playerPosition + v2(cos(toRad(shotAngle)), sin(toRad(shotAngle))) * 24, 9.2, shotAngle + rndFloat(-0.8, 0.8), TWIN, GREEN);
					shotAngle += shotCone;
				}
			}
		} break;
		case CHARACTER_MIRROR: {
			//ORIGINAL
#if 0
			if (game->focusLevel == 0) game->playerOptions[0] = game->playerPosition;
			game->playerOptions[1] = v2(game->playerOptions[0].x*2 - game->playerPosition.x, game->playerPosition.y);
			drawCircle(game->playerOptions[0], 3, 0xFFE0AAFF);
			drawCircle(game->playerOptions[1], 3, 0xFFE0AAFF);

			if (shooting) {
				for (int i = 0; i < 2; i++) {
					Vec2 firePosition = i == 0 ? game->playerPosition : game->playerOptions[1];

					if (attackLevel == 0) {
						createShotP(firePosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
					}
					else if (attackLevel == 1) {
						createShotP(firePosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
					}
					else if (attackLevel == 2) {
						createShotP(firePosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition + v2(14, -24), 9.8, 273, TWIN, GREEN);
						createShotP(firePosition - v2(14, 24), 9.8, 267, TWIN, GREEN);
					}
					else if (attackLevel == 3) {
						createShotP(firePosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition + v2(18, -24), 9.8, 275, TWIN, GREEN);
						createShotP(firePosition - v2(18, 24), 9.8, 265, TWIN, GREEN);
					}
				}
			}
#endif

#if 0 //REWORK A
			if (game->focusLevel == 20) game->playerOptions[0] = lerp(game->playerOptions[0], game->playerPosition, 0.03);
			game->playerOptions[1] = v2(game->playerOptions[0].x*2 - game->playerPosition.x, game->playerPosition.y);
			drawLine(v2(game->playerOptions[0].x, 0), v2(game->playerOptions[0].x, game->height), 2, 0xFFE0AAFF);
			drawCircle(game->playerOptions[1], 3, 0xFFE0AAFF);

			if (shooting) {
				for (int i = 0; i < 2; i++) {
					Vec2 firePosition = i == 0 ? game->playerPosition : game->playerOptions[1];

					if (attackLevel == 0) {
						createShotP(firePosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
					}
					else if (attackLevel == 1) {
						createShotP(firePosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
					}
					else if (attackLevel == 2) {
						createShotP(firePosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition + v2(14, -24), 9.8, 273, TWIN, GREEN);
						createShotP(firePosition - v2(14, 24), 9.8, 267, TWIN, GREEN);
					}
					else if (attackLevel == 3) {
						createShotP(firePosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition + v2(18, -24), 9.8, 275, TWIN, GREEN);
						createShotP(firePosition - v2(18, 24), 9.8, 265, TWIN, GREEN);
					}
				}
			}
#endif

#if 0 //REWORK B
			if (game->focusLevel == 0) {
				game->playerOptions[0] = game->playerPosition;

				if (shooting) {
					if (attackLevel == 0) {
						createShotP(game->playerPosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
					}
					else if (attackLevel == 1) {
						createShotP(game->playerPosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
						createShotP(game->playerPosition - v2(12, 24), 9.8, 270, TWIN, GREEN);

						createShotP(game->playerPosition + v2(0, 24), 9.8, 90, TWIN, GREEN);
					}
					else if (attackLevel == 2) {
						createShotP(game->playerPosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
						createShotP(game->playerPosition + v2(14, -24), 9.8, 273, TWIN, GREEN);
						createShotP(game->playerPosition - v2(14, 24), 9.8, 267, TWIN, GREEN);

						createShotP(game->playerPosition + v2(28, 34), 9.8, 260, TWIN, GREEN);
						createShotP(game->playerPosition - v2(28, -34), 9.8, 280, TWIN, GREEN);

					}
					else if (attackLevel == 3) {
						createShotP(game->playerPosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
						createShotP(game->playerPosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
						createShotP(game->playerPosition + v2(18, -24), 9.8, 275, TWIN, GREEN);
						createShotP(game->playerPosition - v2(18, 24), 9.8, 265, TWIN, GREEN);

						createShotP(game->playerPosition + v2(0, 24), 9.8, 90, TWIN, GREEN);
						createShotP(game->playerPosition + v2(28, 36), 9.8, 265, TWIN, GREEN);
						createShotP(game->playerPosition - v2(28, -36), 9.8, 275, TWIN, GREEN);
					}
				}
			} else {
				game->playerOptions[1] = v2(game->playerOptions[0].x*2 - game->playerPosition.x, game->playerPosition.y);
				drawCircle(game->playerOptions[0], 3, 0xFFE0AAFF);
				drawCircle(game->playerOptions[1], 3, 0xFFE0AAFF);

				if (shooting) {
					for (int i = 0; i < 2; i++) {
						Vec2 firePosition = i == 0 ? game->playerPosition : game->playerOptions[1];

						if (attackLevel == 0) {
							createShotP(firePosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
						}
						else if (attackLevel == 1) {
							createShotP(firePosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
							createShotP(firePosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
						}
						else if (attackLevel == 2) {
							createShotP(firePosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
							createShotP(firePosition + v2(14, -24), 9.8, 273, TWIN, GREEN);
							createShotP(firePosition - v2(14, 24), 9.8, 267, TWIN, GREEN);
						}
						else if (attackLevel == 3) {
							createShotP(firePosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
							createShotP(firePosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
							createShotP(firePosition + v2(18, -24), 9.8, 275, TWIN, GREEN);
							createShotP(firePosition - v2(18, 24), 9.8, 265, TWIN, GREEN);
						}
					}
				}
			}
#endif

#if 1 //REWORK C
			if (game->focusLevel == 0) game->playerOptions[0].x = lerp(game->playerOptions[0].x, game->playerPosition.x, 0.03);
			game->playerOptions[1] = v2(game->playerOptions[0].x*2 - game->playerPosition.x, game->playerPosition.y);
			drawCircle(game->playerOptions[0], 3, 0xFFE0AAFF);
			drawCircle(game->playerOptions[1], 3, 0xFFE0AAFF);

			if (shooting) {
				for (int i = 0; i < 2; i++) {
					Vec2 firePosition = i == 0 ? game->playerPosition : game->playerOptions[1];

					if (attackLevel == 0) {
						createShotP(firePosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
					}
					else if (attackLevel == 1) {
						createShotP(firePosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
					}
					else if (attackLevel == 2) {
						createShotP(firePosition + v2(0, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition + v2(14, -24), 9.8, 273, TWIN, GREEN);
						createShotP(firePosition - v2(14, 24), 9.8, 267, TWIN, GREEN);
					}
					else if (attackLevel == 3) {
						createShotP(firePosition + v2(12, -24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition - v2(12, 24), 9.8, 270, TWIN, GREEN);
						createShotP(firePosition + v2(18, -24), 9.8, 275, TWIN, GREEN);
						createShotP(firePosition - v2(18, 24), 9.8, 265, TWIN, GREEN);
					}
				}
			}
#endif
		} break;
		case CHARACTER_SPRING: {
			if (shooting) {
				createShotP(v2(game->playerPosition.x, game->playerPosition.y - 24), 9.8, 270, TWIN, GREEN);
				if (attackLevel > 0) {
					createShotP(v2(game->playerPosition.x + 16, game->playerPosition.y - 8), game->focusLevel == 0 ? 9.8 : -2.6, game->focusLevel == 0 ? 275 : 272, TWIN, GREEN)->speedAccel = (game->focusLevel == 0 ? 0 : 0.2);
					createShotP(v2(game->playerPosition.x - 16, game->playerPosition.y - 8), game->focusLevel == 0 ? 9.8 : -2.6, game->focusLevel == 0 ? 265 : 268, TWIN, GREEN)->speedAccel = (game->focusLevel == 0 ? 0 : 0.2);
				}
				if (attackLevel > 1) {
					createShotP(v2(game->playerPosition.x + 16, game->playerPosition.y - 8), game->focusLevel == 0 ? 9.8 : -3.4, game->focusLevel == 0 ? 280 : 274, TWIN, GREEN)->speedAccel = (game->focusLevel == 0 ? 0 : 0.25);
					createShotP(v2(game->playerPosition.x - 16, game->playerPosition.y - 8), game->focusLevel == 0 ? 9.8 : -3.4, game->focusLevel == 0 ? 260 : 266, TWIN, GREEN)->speedAccel = (game->focusLevel == 0 ? 0 : 0.25);
				}
				if (attackLevel > 2) {
					createShotP(v2(game->playerPosition.x + 16, game->playerPosition.y - 8), game->focusLevel == 0 ? 9.8 : -4.2, game->focusLevel == 0 ? 285 : 276, TWIN, GREEN)->speedAccel = (game->focusLevel == 0 ? 0 : 0.3);
					createShotP(v2(game->playerPosition.x - 16, game->playerPosition.y - 8), game->focusLevel == 0 ? 9.8 : -4.2, game->focusLevel == 0 ? 255 : 264, TWIN, GREEN)->speedAccel = (game->focusLevel == 0 ? 0 : 0.3);
				}
			}
		} break;
		case 6: {//LUXIA
			//
		} break;
		case 7: {//TERRA
			//
		} break;
	}
}

void playerShotAddendums(int shotStyle, int attackLevel, bool shooting) {
	switch (shotStyle) {
		case 1: {//Laser
			float sineWave = sin(game->frameCount / 24.0) * 0.5 + 0.5;//Goes from 0 to 1
			if (shooting) {
				float laserDist = 7.0;
				float laserLength = game->playerPosition.y / laserDist;
				for (int i = 0; i < laserLength; i++) {
					Danmaku *tempShot = createShotP(game->playerPosition, 0, 90, BEAM, YELLOW);
					tempShot->positionOffset.x = 270;
					tempShot->positionOffset.y = i * laserDist;
					tempShot->type = DTYPE_SPECIAL_LASERADD;
					tempShot->alpha = sineWave;
					tempShot->health = sineWave * 10;
					addModifier(tempShot, deathDanmakuModifier, app->globals.playerFireRate);
				}
			}
		} break;
		case 2: {//Homing Missile
			if (shooting && (game->frameCount % (app->globals.playerFireRate * 8) < app->globals.playerFireRate)) {
				DanmakuModifier modifier = newDanmakuModifier();
				modifier.speedAccelAbsolute = true;
				modifier.speedAccelUpdate = 0;

				for (int i = 0; i < attackLevel + 1; i++) {
					Danmaku *tempShot = createShotP(game->playerPosition + v2(0, i * 3), 2.4, 270, TENGU, ORANGE);
					tempShot->speedAccel = 0.1;
					tempShot->flags |= _F_DF_BLEND_ADD;
					addModifier(tempShot, homingDanmakuModifier, 1);
					addModifier(tempShot, modifier, 100);
				}
			}
		} break;
		case 3: {//Interception Laser
			if (shooting && (game->frameCount % 60 < app->globals.playerFireRate)) {
				int targetIndex = getClosestEnemyShot(game->playerPosition);
				if (targetIndex != -1) {
					Danmaku *target = &game->danmakuList[targetIndex];
					drawLine(target->position, game->playerPosition, 5, 0xFFAADDFF);
					target->dead = true;
				}
			}
		} break;
		case 4: {//Super Homing Missile
			if (shooting) {
				DanmakuModifier modifier = newDanmakuModifier();
				modifier.speedAccelAbsolute = true;
				modifier.speedAccelUpdate = 0;

				for (int i = 0; i < 8*2; i++) {
					int flipFlop = i % 2 == 0 ? -1 : 1;
					int shotStreams = (i / 2) + 1;
					Danmaku *tempShot = createShotP(game->playerPosition, 2.4, 90 + game->frameCount * flipFlop * shotStreams, SCALE, ORANGE);
					tempShot->speedAccel = 0.1;
					tempShot->flags |= _F_DF_BLEND_ADD;
					addModifier(tempShot, homingDanmakuModifier, 1);
					addModifier(tempShot, modifier, 100);
				}

				for (int i = 0; i < 8*2; i++) {
					int flipFlop = i % 2 == 0 ? -1 : 1;
					int shotStreams = (i / 2) + 1;
					Danmaku *tempShot = createShotP(game->playerPosition, 1.9, 90 + game->frameCount * flipFlop * shotStreams, SCALE, ORANGE);
					tempShot->speedAccel = 0.1;
					tempShot->flags |= _F_DF_BLEND_ADD;
					addModifier(tempShot, homingDanmakuModifier, 1);
					addModifier(tempShot, modifier, 100);
				}

				for (int i = 0; i < 8*2; i++) {
					int flipFlop = i % 2 == 0 ? -1 : 1;
					int shotStreams = (i / 2) + 1;
					Danmaku *tempShot = createShotP(game->playerPosition, 1.4, 90 + game->frameCount * flipFlop * shotStreams, SCALE, ORANGE);
					tempShot->speedAccel = 0.1;
					tempShot->flags |= _F_DF_BLEND_ADD;
					addModifier(tempShot, homingDanmakuModifier, 1);
					addModifier(tempShot, modifier, 100);
				}
			}
		} break;
	}
}

void playerBomb() {
	Danmaku *tempShot = createShotS(game->playerPosition, 4.8, 270, YINYANG_L, GREEN);
	tempShot->spinAccel = 5;
	tempShot->scaleAccel = 0.024;

	DanmakuModifier modifier = newDanmakuModifier();
	modifier.die = true;
	addModifier(tempShot, modifier, 300);
}

void enemyWaveA(PatternData *data, int subType) {
	if (data->frame % 5 == 1) {
		if (subType == 1) {
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 1, 0), 5.4, 90, KOUMA, RED,    15);
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 2, 0), 5.4, 90, KOUMA, ORANGE, 15);
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 3, 0), 5.4, 90, KOUMA, YELLOW, 15);
		}
		else if (subType == 2) {
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 1, 0), 5.4, 90, KOUMA, LIME,  15);
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 2, 0), 5.4, 90, KOUMA, GREEN, 15);
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 3, 0), 5.4, 90, KOUMA, TEAL,  15);
		}
		else if (subType == 3) {
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 1, 0), 5.4, 90, KOUMA, CYAN,   15);
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 2, 0), 5.4, 90, KOUMA, BLUE,   15);
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 3, 0), 5.4, 90, KOUMA, INDIGO, 15);
		}
		else if (subType == 4) {
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 1, 0), 5.4, 90, KOUMA, PURPLE,  15);
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 2, 0), 5.4, 90, KOUMA, VIOLET,  15);
			createShotA(v2(rndFloat(30, 38) + (game->width / 4) * 3, 0), 5.4, 90, KOUMA, MAGENTA, 15);
		}
	}
}

void enemyWaveB(PatternData *data, int subType) {
	Danmaku *tempShot;
	int spawnRate = 40 + ((8 - data->difficulty) * 10);
	if (data->frame % spawnRate == 0) {
		DanmakuModifier modifier = newDanmakuModifier();

		if (subType == 1)      {tempShot = createShotU(v2(0, 300),               3, 0,   DTYPE_KOUMA_KANNON, data->difficulty); modifier.angleAccelUpdate = -2;}
		else if (subType == 2) {tempShot = createShotU(v2(game->width * 0.3, 0), 3, 90,  DTYPE_KOUMA_KANNON, data->difficulty); modifier.angleAccelUpdate = -2;}
		else if (subType == 3) {tempShot = createShotU(v2(game->width * 0.6, 0), 3, 90,  DTYPE_KOUMA_KANNON, data->difficulty); modifier.angleAccelUpdate =  2;}
		else if (subType == 4) {tempShot = createShotU(v2(game->width, 300),     3, 180, DTYPE_KOUMA_KANNON, data->difficulty); modifier.angleAccelUpdate =  2;}

		addModifier(tempShot, modifier, 100);

		modifier = newDanmakuModifier();
		modifier.angleAccelAbsolute = true;
		modifier.angleAccelUpdate = 0;
		addModifier(tempShot, modifier, 100 + 90);
	}
}

void enemyWaveC(PatternData *data, int subType) {
	DanmakuModifier modifier = newDanmakuModifier();
	modifier.speedUpdate = -1;

	if (subType == 1) {
		if (data->frame == 0) {
			for (int i = 0; i < 3; i++) {
				Danmaku *tempShot = createShotU(v2((game->width / 4) * (i + 1), 0), 1, 90, DTYPE_RAIN, data->difficulty);	
				addModifier(tempShot, modifier, 80);
				addModifier(tempShot, modifier, 360);
			}
		}
	}
	else if (subType == 2 || subType == 3) {
		Danmaku *tempShot = NULL;
		if (data->frame == 0) tempShot = createShotU(v2((game->width / 4) * ((subType == 2 ? 0 : 2) + 1), 0), 1, 90, DTYPE_RAIN, data->difficulty);
		if (data->frame == 80) tempShot = createShotU(v2((game->width / 4) * (1 + 1), 0), 1, 90, DTYPE_RAIN, data->difficulty);
		if (data->frame == 160) tempShot = createShotU(v2((game->width / 4) * ((subType == 2 ? 2 : 0) + 1), 0), 1, 90, DTYPE_RAIN, data->difficulty);
		if (tempShot) {
			addModifier(tempShot, modifier, 80);
			addModifier(tempShot, modifier, 360);
		}
	}
	else if (subType == 4) {
		if (data->frame == 0) {
			Danmaku *tempShot = createShotU(v2((game->width / 4) * (1 + 1), 0), 1, 90, DTYPE_RAIN, data->difficulty);
			addModifier(tempShot, modifier, 80);
			addModifier(tempShot, modifier, 360);
		}
		if (data->frame == 120) {
			Danmaku *tempShot = createShotU(v2((game->width / 4) * (0 + 1), 0), 1, 90, DTYPE_RAIN, data->difficulty);
			addModifier(tempShot, modifier, 80);
			addModifier(tempShot, modifier, 360);
			tempShot = createShotU(v2((game->width / 4) * (2 + 1), 0), 1, 90, DTYPE_RAIN, data->difficulty);
			addModifier(tempShot, modifier, 80);
			addModifier(tempShot, modifier, 360);
		}
	}
}

void enemyWaveD(PatternData *data, int subType) {
	int left = 0;
	int right = game->width;
	int spawnRate = 15 * (9 - data->difficulty);
	bool flipFlop = (data->frame / spawnRate) % 2;
	if (data->frame < data->difficulty * spawnRate) {
		if (data->frame % spawnRate == 0) {
			     if (subType == 1) createShotU(v2(left,  rndFloat(0, 30)),                   8, rndFloat(-8, 8) +                    20, DTYPE_MINE_DROPPER, data->difficulty);
			else if (subType == 2) createShotU(v2(right, rndFloat(0, 30)),                   8, rndFloat(-8, 8) +                   160, DTYPE_MINE_DROPPER, data->difficulty);
			else if (subType == 3) createShotU(v2(flipFlop ? left : right, rndFloat(0, 30)), 8, rndFloat(-8, 8) + (flipFlop ? 20 : 160), DTYPE_MINE_DROPPER, data->difficulty);
			else if (subType == 4) createShotU(v2(flipFlop ? left : right, rndFloat(0, 30)), 8, rndFloat(-8, 8) + (flipFlop ? 40 : 140), DTYPE_MINE_DROPPER, data->difficulty);
		}
	}
}

void enemyWaveE(PatternData *data, int subType) {
	if (data->frame < (30 * data->difficulty)) {
		if (data->frame % 15 == 0) {
			Vec2 spawnPosition = v2(rndFloat(0, game->width), 0);
			Vec2 desiredLocation = v2(game->width - spawnPosition.x, 150);
			float speedFactor = 20;
			float speedOutcome = spawnPosition.distance(desiredLocation) / speedFactor;
			Danmaku *tempShot = createShotU(spawnPosition, speedOutcome, degsBetween(spawnPosition, desiredLocation), DTYPE_SPREAD_ATTACKER, data->difficulty);
			tempShot->speedAccel = -0.5 * (speedOutcome / speedFactor);

			DanmakuModifier modifier = newDanmakuModifier();
			modifier.speedAbsolute = true;
			modifier.speedUpdate = 0;
			modifier.speedAccelAbsolute = true;
			modifier.speedAccelUpdate = 0;
			addModifier(tempShot, modifier, speedFactor + 8);

			modifier = newDanmakuModifier();
			modifier.speedAbsolute = true;
			modifier.speedUpdate = 1;
			modifier.speedAccelAbsolute = true;
			modifier.speedAccelUpdate = 0.1;
			modifier.angleUpdate = 180;
			addModifier(tempShot, modifier, speedFactor + 8 + 30);
		}
	}
}

void enemyWaveF(PatternData *data, int subType) {
	if (data->frame % 95 == 0) {
		if (subType == 1) {
			createShotU(v2(0, rndFloat(-10, 90)), 3.6 - rndFloat(0, data->difficulty / 3.0),   0, DTYPE_SPINNER, data->difficulty);
		}
		else if (subType == 2) {
			createShotU(v2(game->width, rndFloat(-10, 90)), 3.6 - rndFloat(0, data->difficulty / 3.0), 180, DTYPE_SPINNER, data->difficulty);
		}
		else if (subType == 3 || subType == 4) {
			if (rndBool()) createShotU(v2(0,           rndFloat(-10, 90)), 3.6 - rndFloat(0, data->difficulty / 3.0),   0, DTYPE_SPINNER, data->difficulty);
			else           createShotU(v2(game->width, rndFloat(-10, 90)), 3.6 - rndFloat(0, data->difficulty / 3.0), 180, DTYPE_SPINNER, data->difficulty);
		}
	}
}

void enemyWaveG(PatternData *data, int subType) {
	int spawnRate = (200 - (data->difficulty * 10));
	if (data->frame % spawnRate == 0) {
		bool flipFlop = data->frame % (spawnRate * 2) == 0;
		Vec2 spawnPosition;
		     if (subType == 1) spawnPosition = v2(flipFlop ? 0 : game->width, rndFloat(40, 80));
		else if (subType == 2) spawnPosition = v2(flipFlop ? 0 : game->width, game->height - rndFloat(40, 80));
		else if (subType == 3) spawnPosition = v2(flipFlop ? 0 : game->width, rndFloat(40, game->height - 40));
		else                   spawnPosition = v2(flipFlop ? 0 : game->width, data->difficulty < 5 ? fmod(game->playerPosition.y + game->height * 0.5, game->height) : mathClamp(game->playerPosition.y, 20, game->height - 20) + rndFloat(-20, 20));
		float playerAngle = degsBetween(spawnPosition, game->playerPosition);

		createShotU(spawnPosition, 3.2, lerpDeg(flipFlop ? 0 : 180, playerAngle, 0.24), DTYPE_SEEK, data->difficulty);
	}
}

void enemyWaveI(PatternData *data, int subType) {
	DanmakuModifier modifier = newDanmakuModifier();
	modifier.speedUpdate = -1;

	if (subType == 1) {
		if (data->frame == 0) {
			for (int i = 0; i < 3; i++) {
				Danmaku *tempShot = createShotU(v2((game->width / 4) * (i + 1), 0), 1, 90, DTYPE_OMNI, data->difficulty);	
				addModifier(tempShot, modifier, 80);
				addModifier(tempShot, modifier, 360);
			}
		}
	}
	else if (subType == 2 || subType == 3) {
		Danmaku *tempShot = NULL;
		if (data->frame == 0) tempShot = createShotU(v2((game->width / 4) * ((subType == 2 ? 0 : 2) + 1), 0), 1, 90, DTYPE_OMNI, data->difficulty);
		if (data->frame == 80) tempShot = createShotU(v2((game->width / 4) * (1 + 1), 0), 1, 90, DTYPE_OMNI, data->difficulty);
		if (data->frame == 160) tempShot = createShotU(v2((game->width / 4) * ((subType == 2 ? 2 : 0) + 1), 0), 1, 90, DTYPE_OMNI, data->difficulty);
		if (tempShot) {
			addModifier(tempShot, modifier, 80);
			addModifier(tempShot, modifier, 360);
		}
	}
	else if (subType == 4) {
		if (data->frame == 0) {
			Danmaku *tempShot = createShotU(v2((game->width / 4) * (1 + 1), 0), 1, 90, DTYPE_OMNI, data->difficulty);
			addModifier(tempShot, modifier, 80);
			addModifier(tempShot, modifier, 360);
		}
		if (data->frame == 120) {
			Danmaku *tempShot = createShotU(v2((game->width / 4) * (0 + 1), 0), 1, 90, DTYPE_OMNI, data->difficulty);
			addModifier(tempShot, modifier, 80);
			addModifier(tempShot, modifier, 360);
			tempShot = createShotU(v2((game->width / 4) * (2 + 1), 0), 1, 90, DTYPE_OMNI, data->difficulty);
			addModifier(tempShot, modifier, 80);
			addModifier(tempShot, modifier, 360);
		}
	}
}

void justForFun(PatternData *data, int subType) {
	int iterations = 2;
	int spokeCount = 16;
	Vec2 centerPoint = v2(game->width / 2, game->height / 2);
	bool alphaFlipFlop = data->frame % 2 == 0;
	bool betaFlipFlop = data->frame % 4 / 2 == 0;
	float currentRotation = 0.8 * data->frame;
	int currentSpoke = data->frame * iterations;
	int timeOut = 100;
	float currentGrowth = (data->frame % timeOut) * 2.8;

	if (data->frame % (timeOut * 3) < timeOut) {
		for (int i = 0; i < iterations; i++) {
			currentSpoke += i;
			currentSpoke %= spokeCount;

			float outerDirection = currentRotation + (currentSpoke * (360.0 / spokeCount));
			outerDirection *= alphaFlipFlop ? -1 : 1;
			Vec2 firePoint = v2(cos(toRad(outerDirection)), sin(toRad(outerDirection)));

			Danmaku *tempShot = createShotA(centerPoint + firePoint * currentGrowth, 0.3, outerDirection + (alphaFlipFlop ? 90 : 270), SCALE, PURPLE, 40);

			DanmakuModifier modifier = newDanmakuModifier();
			modifier.speedAccelUpdate = betaFlipFlop ? 0.03 : 0.06;
			addModifier(tempShot, modifier, 240);
		}
	}
}


























