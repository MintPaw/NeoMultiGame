#ifndef __DYNA_IMPL
struct Player {
	Vec2 position;
	Vec2 momentum;
};

void battleInit();
void runBattle();
void keyboardMovement();
void experimentalKeyboardMovement();
void mouseMovement();
void exitBattle();
/// FUNCTIONS ^
#else

void battleInit() {
	Rect *playArea = &game->playArea;
	playArea->x = 10;
	playArea->y = 10;
	playArea->width = 460;//The playarea should be 9:16
	playArea->height = 700;

	Player *player = &game->player;
	player->position = v2(playArea->width / 2.0, playArea->height - 80);

	for (int i = 0; i < DANMAKU_GRAPHIC_MAX; i++) {
		DanmakuGraphicInfo *info = &game->danmakuGraphicInfos[i];
		char *path = frameSprintf("assets/images/bullets/%s.png", danmakuGraphicStrings[i]);
		Texture *texture = getTexture(path);
		info->size.x = texture->width;
		info->size.y = texture->height;
		float smallestRadius = min(info->size.x, info->size.y);
		info->radius = 8 + max(3, floor((smallestRadius - 12) / 3));
		info->bulletToBulletRadius = 8 + smallestRadius / 2;
		info->textureRadius = max(info->size.x, info->size.y);
		info->path = stringClone(path);
		info->name = danmakuGraphicStrings[i];
	}
}

void runBattle() {
	Rect *playArea = &game->playArea;
	//drawRectOutline(*playArea, 1, 0xFFFF0000);

	Player *player = &game->player;
	if (!game->tempBool) keyboardMovement();
	else experimentalKeyboardMovement();
	if (platform->mouseDown) mouseMovement();
	else platform->useRelativeMouse = false;
	player->position.x = mathClamp (player->position.x, 0, playArea->width);
	player->position.y = mathClamp (player->position.y, 0, playArea->height);
	drawCircle(player->position + v2(playArea->x, playArea->y), 4, 0xFFFFD040);

	game->levelProgress += 0.0001;//0.0007;
	if (game->levelProgress > 1.0) game->victory = true;
	if (game->levelProgress > 1.3) exitBattle();

	if (!game->victory) {
		Vec2 bossPosition = v2(playArea->width / 2.0, 180);
		Danmaku *tempShot = createShotA(bossPosition, game->frameCount % 2 == 0 ? 2.4 : 1.6, 1.8 * game->frameCount + ((game->frameCount / 2) % 2 == 0 ? 0 : 180), SCALE, VIOLET, 0);
		DanmakuModifier modifier = newDanmakuModifier();
		modifier.angleUpdate = game->frameCount % 2 == 0 ? 175 : -175;
		addModifier(tempShot, modifier, 80);
		modifier.angleAccelUpdate = game->frameCount % 2 == 0 ? 0.1 : -0.1;
		addModifier(tempShot, modifier, game->frameCount % 2 == 0 ? 390 : 475);
	}

	{ /// HANDLE DANMAKU
		for (int i = 0; i < game->danmakuListNum; i++) {
			Danmaku *danmaku = &game->danmakuList[i];
			preformActions(danmaku);
			preformDraw(danmaku);

			if (danmaku->health == 0) {
				game->danmakuList[i] = game->danmakuList[game->danmakuListNum-1];
				game->danmakuListNum--;
				i--;
				continue;
			}
		}
	}

	{ /// DRAW UI
		Texture *texture = getTexture("assets/images/Background.png");
		RenderProps props = newRenderProps();
		drawTexture(texture, props);

		texture = getTexture("assets/images/ProgressBar.png");
		props = newRenderProps();
		props.matrix.TRANSLATE(v2(500, 690));
		drawTexture(texture, props);

		texture = getTexture("assets/images/mapIcons/House.png");
		props = newRenderProps();
		props.matrix.TRANSLATE(v2(500, 562));
		drawTexture(texture, props);

		texture = getTexture("assets/images/mapIcons/Event.png");
		props = newRenderProps();
		props.matrix.TRANSLATE(lerp(v2(500, 562), v2(1122, 562), 0.5));
		drawTexture(texture, props);

		texture = getTexture("assets/images/mapIcons/Forest.png");
		props = newRenderProps();
		props.matrix.TRANSLATE(v2(1122, 562));
		drawTexture(texture, props);

		drawCircle(lerp(v2(510, 700), v2(1240, 700), min(1.0, game->levelProgress)), 8, 0xFFAAFF00);
	}
}

void keyboardMovement() {
	Vec2 inputVec = v2();
	if (keyPressed(KEY_UP)) inputVec.y--;
	if (keyPressed(KEY_DOWN)) inputVec.y++;
	if (keyPressed(KEY_LEFT)) inputVec.x--;
	if (keyPressed(KEY_RIGHT)) inputVec.x++;
	inputVec = inputVec.normalize() * ((keyPressed(KEY_SHIFT) || game->focusToggle) ? 1.5 : 3.0);
	Player *player = &game->player;
	player->position += inputVec;
}

void experimentalKeyboardMovement() {
	Vec2 inputVec = v2();
	if (keyPressed(KEY_UP)) inputVec.y--;
	if (keyPressed(KEY_DOWN)) inputVec.y++;
	if (keyPressed(KEY_LEFT)) inputVec.x--;
	if (keyPressed(KEY_RIGHT)) inputVec.x++;
	inputVec = inputVec.normalize() * 25.0;
	Player *player = &game->player;
	player->momentum += inputVec * ((keyPressed(KEY_SHIFT) || game->focusToggle) ? 0.015 : 0.03);
	player->momentum *= 0.85;
	player->position += player->momentum;
}

void mouseMovement() {
	platform->useRelativeMouse = platform->windowHasFocus;
	Player *player = &game->player;
	player->momentum += platform->relativeMouse * ((keyPressed(KEY_SHIFT) || game->focusToggle) ? 0.015 : 0.03);
	player->momentum *= 0.85;
	player->position += player->momentum;
}

void exitBattle() {
	platform->useRelativeMouse = false;
	game->currentState = GS_NONE;
}
#endif