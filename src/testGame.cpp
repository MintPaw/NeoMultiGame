struct Globals {
};

struct Game {
	Texture *gameTexture;

	Globals globals;
	float time;

	bool debugMode;

	// SpineSkeleton *skeleton;
};

Game *game = NULL;

void runGame();
void updateGame();
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
#if !defined(FALLOW_INTERNAL) // This needs to be a macro
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#else
	// if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/???");
#endif
#endif

	initFileOperations();

	initPlatform(1280, 720, "A game?");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	// initMesh();
	// initModel();
	// initSkeleton();
	initFonts();
	initTextureSystem();
	// if (!initNetworking()) logf("Failed to init networking\n");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		// RegMem(Globals, );
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);
		maximizeWindow();
	}

	// {
	// 	BoneData *data = &skeleton->boneData[spSkeleton_findBoneIndex(skeleton->spSkeletonInstance, "bone2")];
	// 	data->boneEffect = BONE_EFFECT_EASE_END;
	// }

	// drawRect(0, 0, 500, 500, 0xFFFF0000);

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);

	clearRenderer(0xFFFFFFFF);

	if (keyJustPressed(KEY_BACKTICK)) game->debugMode = !game->debugMode;
	if (game->debugMode) {
	}

#if 0
	if (platform->frameCount == 1) {
		SpineBaseSkeleton *base = loadSpineBaseSkeleton("assets/spine/JiggleTest");
		game->skeleton = deriveSkeleton(base);
	}

	SpineSkeleton *skeleton = game->skeleton;
	ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	for (int i = 2; i < skeleton->boneDataNum; i++) {
		ImGui::PushID(i);

		BoneData *data = &skeleton->boneData[i];
		spBone *bone = skeleton->spSkeletonInstance->bones[i];
		ImGui::Text("%s", bone->data->name);
		ImGui::Combo("Effect type", (int *)&data->boneEffect, boneEffectTypeStrings, ArrayLength(boneEffectTypeStrings));
		if (data->boneEffect == BONE_EFFECT_EASE_START || data->boneEffect == BONE_EFFECT_EASE_END) {
			ImGui::SliderFloat("Ease amount", &data->easeAmount, 0, 1);
		}

		if (data->boneEffect == BONE_EFFECT_SPRING_START || data->boneEffect == BONE_EFFECT_SPRING_END) {
			ImGui::InputFloat("Tension", &data->tension);
			ImGui::SliderFloat("Damping", &data->damping, 0, 1);
		}
		ImGui::Separator();

		ImGui::PopID();
	}
	ImGui::End();

	updateSkeletonAnimation(skeleton, elapsed);

	Vec2 rootPos = platform->mouse;
	{
		spBone *bone = spSkeleton_findBone(skeleton->spSkeletonInstance, "root");
		bone->x = rootPos.x;
		bone->y = rootPos.y;
	}

	updateSkeletonPhysics(skeleton, elapsed);

	{
		for (int i = 0; i < skeleton->spSkeletonInstance->slotsCount; i++) {
			spSlot *slot = skeleton->spSkeletonInstance->drawOrder[i];

			spAttachment *attachment = slot->attachment;
			if (!attachment) continue;

			drawAttachment(skeleton, slot, attachment);
		}
	}

#if 0
	for (int i = 0; i < skeleton->spSkeletonInstance->bonesCount; i++) {
		spBone *bone = skeleton->spSkeletonInstance->bones[i];
		BoneData *data = &skeleton->boneData[i];
		if (data->boneEffect == BONE_EFFECT_NONE) continue;

		Vec2 start;
		spBone_localToWorld(bone, 0, 0, &start.x, &start.y);

		Vec2 end;
		spBone_localToWorld(bone, bone->data->length, 0, &end.x, &end.y);

		// drawCircle(start, 10, 0xFFFFFF00);
		// drawLine(start, end, 10, 0xFFFF00FF);

		drawCircle(data->prevWorldStart + v2(bone->data->x, bone->data->y), 10, 0xFFFFFF00);
		drawCircle(data->prevWorldEnd + v2(bone->data->x, bone->data->y), 10, 0xFF00FFFF);
	}
#endif
#endif

#if 0
	if (platform->frameCount == 1) {
		resetSkia(v2(platform->windowWidth, platform->windowHeight), v2(1, 1), true, 16);
	}
	if (game->debugMode) {
		ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::TreeNode("Swf analyzer")) {
			drawSwfAnalyzer();
			ImGui::TreePop();
		}
		ImGui::End();
	}

	static Swf *swf = NULL;
	if (!swf) swf = loadSwf("assets/swf/test.swf");

	startSkiaFrame();
	SwfSprite *sprite = getSpriteByName(swf, "TestSymbol");
	drawSprite(sprite, transforms, transformsNum);
	endSkiaFrame();

	{
		RenderProps props = newRenderProps();
		drawTexture(skiaSys->backTexture, props);
	}
#endif

#if 0
	if (platform->frameCount == 1) {
		SpineBaseSkeleton *base = loadSpineBaseSkeleton("assets/spine/syrth");
		game->skeleton = deriveSkeleton(base);
	}

	SpineSkeleton *skeleton = game->skeleton;

	static Vec2 rootPos = v2(750, 500);
	static Vec2 rootScale = v2(0.35, 0.35);

	ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat2("rootPos", &rootPos.x);
	ImGui::DragFloat2("rootScale", &rootScale.x, 0.005);
	// for (int i = 2; i < skeleton->boneDataNum; i++) {
	// 	ImGui::PushID(i);

	// 	BoneData *data = &skeleton->boneData[i];
	// 	spBone *bone = skeleton->spSkeletonInstance->bones[i];
	// 	ImGui::Text("%s", bone->data->name);
	// 	ImGui::Combo("Effect type", (int *)&data->boneEffect, boneEffectTypeStrings, ArrayLength(boneEffectTypeStrings));
	// 	if (data->boneEffect == BONE_EFFECT_EASE_START || data->boneEffect == BONE_EFFECT_EASE_END) {
	// 		ImGui::SliderFloat("Ease amount", &data->easeAmount, 0, 1);
	// 	}

	// 	if (data->boneEffect == BONE_EFFECT_SPRING_START || data->boneEffect == BONE_EFFECT_SPRING_END) {
	// 		ImGui::InputFloat("Tension", &data->tension);
	// 		ImGui::SliderFloat("Damping", &data->damping, 0, 1);
	// 	}
	// 	ImGui::Separator();

	// 	ImGui::PopID();
	// }
	ImGui::End();

	updateSkeletonAnimation(skeleton, elapsed);

	{
		spBone *bone = spSkeleton_findBone(skeleton->spSkeletonInstance, "root");
		bone->x = rootPos.x;
		bone->y = rootPos.y;
		bone->scaleX = rootScale.x;
		bone->scaleY = rootScale.y;
	}

	Vec2 realPos = getPosition(getBoneMatrix(skeleton, "body1"));
	drawCircle(realPos, 20, 0xFFFF0000);
	Vec2 between = realPos - platform->mouse;
	// Vec2 bodyPos = platform->mouse;
	{
		spBone *bone = spSkeleton_findBone(skeleton->spSkeletonInstance, "body1");
		// bone->x = bodyPos.x;
		// bone->y = bodyPos.y;
		bone->x += between.x;
		bone->y += between.y;
	}

	updateSkeletonPhysics(skeleton, elapsed);

	{
		for (int i = 0; i < skeleton->spSkeletonInstance->slotsCount; i++) {
			spSlot *slot = skeleton->spSkeletonInstance->drawOrder[i];

			spAttachment *attachment = slot->attachment;
			if (!attachment) continue;

			drawAttachment(skeleton, slot, attachment);
		}
	}

	realPos = getPosition(getBoneMatrix(skeleton, "body1"));
	drawCircle(realPos, 20, 0xFFFF0000);

#if 0
	for (int i = 0; i < skeleton->spSkeletonInstance->bonesCount; i++) {
		spBone *bone = skeleton->spSkeletonInstance->bones[i];
		BoneData *data = &skeleton->boneData[i];
		if (data->boneEffect == BONE_EFFECT_NONE) continue;

		Vec2 start;
		spBone_localToWorld(bone, 0, 0, &start.x, &start.y);

		Vec2 end;
		spBone_localToWorld(bone, bone->data->length, 0, &end.x, &end.y);

		// drawCircle(start, 10, 0xFFFFFF00);
		// drawLine(start, end, 10, 0xFFFF00FF);

		drawCircle(data->prevWorldStart + v2(bone->data->x, bone->data->y), 10, 0xFFFFFF00);
		drawCircle(data->prevWorldEnd + v2(bone->data->x, bone->data->y), 10, 0xFF00FFFF);
	}
#endif
#endif

#if 0
	float fps = 1.0/(float)platform->frameTimeAvg * 1000.0;
	char *str = frameSprintf("Fps: %.1f (%.1fms)", fps, platform->frameTimeAvg);
	drawText(fontSys->defaultFont, str, v2(), 0xFF808080);
#endif

	// void testGame();
	// testGame();
	// printf("Frame %d\n", platform->frameCount);
	drawRect(0, 10, 256, 256, 0xFFFF0000);

	{
		Matrix3 matrix = mat3();
		matrix.ROTATE(3);
		matrix.ROTATE_X(75);
		Vec3 cameraPos = (matrix * v3(0, 0, 1000));

		Camera camera = {};
		camera.position = cameraPos;
		camera.target = v3(0, 0, 0);
		camera.up = v3(0, 0, 1);
		camera.fovy = 10;
		camera.isOrtho = true;

		start3d(camera, v2(platform->windowWidth, platform->windowHeight), -10000, 10000);

		startShader(renderer->alphaDiscardShader);
		Texture *tex1 = getTexture("assets/images/tex1.png");
		Texture *tex2 = getTexture("assets/images/tex2.png");

		Texture *textures[] = {
			tex1,
			tex2,
			tex1,
		};
		Vec3 positions[] = {
			v3(-300, 20, 20),
			v3(-220, 40, 30),
			v3(-325, 40, 25),
		};
		Vec2 sizes[] = {
			v2(128, 128),
			v2(64, 128),
			v2(128, 64),
		};

		for (int i = 0; i < ArrayLength(textures); i++) {
			Texture *texture = textures[i];
			Vec3 position = positions[i];
			Vec2 size = sizes[i];
			if (keyPressed('A')) {
				position.x -= size.x/2;
				position.z += size.y/2;
			}
			drawBillboard(camera, texture, position, size);
		}
		endShader();

		end3d();
	}

	game->time += elapsed;

	drawOnScreenLog();
}








struct AppleGame {
	Vec2 playerPosition;

#define APPLES_MAX 10
	Vec2 applePositions[APPLES_MAX];
};

AppleGame *appleGame = NULL;

void testGame() {

	if (appleGame == NULL) {
		appleGame = (AppleGame *)zalloc(sizeof(AppleGame));

		appleGame->playerPosition.x = 100;
		appleGame->playerPosition.y = 100;

		for (int i = 0; i < APPLES_MAX; i++) {
			appleGame->applePositions[i].x = rndFloat(0, 400);
			appleGame->applePositions[i].y = rndFloat(0, 400);
		}


	}

	Rect rect = makeRect(0, 0, 500, 500);

	drawRect(rect, 0xFF000000);

	for (int i = 0; i < 10000; i++) {
		drawRect(rndFloat(0, 400), rndFloat(0, 400), 8, 8, 0x80FF0000);
	}

	float playerSpeed = 10;
	if (keyPressed(KEY_LEFT)) appleGame->playerPosition.x -= playerSpeed;
	if (keyPressed(KEY_RIGHT)) appleGame->playerPosition.x += playerSpeed;
	if (keyPressed(KEY_UP)) appleGame->playerPosition.y -= playerSpeed;
	if (keyPressed(KEY_DOWN)) appleGame->playerPosition.y += playerSpeed;

	for (int i = 0; i < APPLES_MAX; i++) {
		Circle playerCircle = makeCircle(appleGame->playerPosition, 20);
		Circle appleCircle = makeCircle(appleGame->applePositions[i], 20);

		if (contains(playerCircle, appleCircle)) {
			appleGame->applePositions[i].x = rndFloat(0, 400);
			appleGame->applePositions[i].y = rndFloat(0, 400);
		}
	}

	drawCircle(appleGame->playerPosition, 20, 0xFF00FF00);

	for (int i = 0; i < APPLES_MAX; i++) {
		drawCircle(appleGame->applePositions[i], 5, 0xFFFF0000);
	}

}
