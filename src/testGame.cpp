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
	if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/MultiGame/multiGame/testGameAssets");
#endif
#endif

	initFileOperations();

	initPlatform(1920, 1080, "A game?");
	initAudio();
	initRenderer(1920, 1080);
	initFonts();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		// RegMem(Globals, );
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);
		// maximizeWindow();
	}

	// {
	// 	BoneData *data = &skeleton->boneData[spSkeleton_findBoneIndex(skeleton->spSkeletonInstance, "bone2")];
	// 	data->boneEffect = BONE_EFFECT_EASE_END;
	// }

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);

	clearRenderer(0xFF000000);

	// drawRect(0, 0, 500, 500, 0xFFFF0000);
	// if (keyJustPressed('A')) {
	// 	logf("You pressed a!\n");
	// 	drawRect(0, 0, 500, 500, 0xFF00FF00);
	// }

	// if (platform->frameCount % 30 == 0) logf("Hello\n");

	// if (keyJustPressed(KEY_BACKTICK)) game->debugMode = !game->debugMode;
	if (game->debugMode) {
	}

#if 1
	{
		Texture *texture = getTexture("assets/images/card.png");
		RenderProps props = newRenderProps();
		props.matrix.TRANSLATE(0, 0);
		props.matrix.SCALE(getSize(texture));
		drawTexture(texture, props);
	}

	// {
	// 	pushColor(v4(0.5, 1, 1, 1));
	// 	Texture *texture = getTexture("assets/images/card.png");
	// 	RenderProps props = newRenderProps();
	// 	props.matrix.TRANSLATE(100, 0);
	// 	props.matrix.SCALE(getSize(texture) * 5);
	// 	drawTexture(texture, props);
	// 	popColor();
	// }
#endif

#if 0

	if (platform->frameCount % 10 == 0) {
		logf("Playing sound\n");
		Sound *sound = getSound("assets/audio/monoPluck.ogg");
		Channel *channel = playSound(sound);
		channel->userVolume = 0;
	}

#endif

#if 0
	if (platform->frameCount == 0) {
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
	SkPaint paint = SkPaint();
	paint.setShader(NULL);

	if (platform->frameCount == 0) {
		resetSkia(v2(platform->windowWidth, platform->windowHeight), v2(1, 1), true, 16);
	}

	// if (game->debugMode) {
	// 	ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	// 	if (ImGui::TreeNode("Swf analyzer")) {
	// 		drawSwfAnalyzer();
	// 		ImGui::TreePop();
	// 	}
	// 	ImGui::End();
	// }

	static Swf *sharedSwf = NULL;
	static Swf *sofiSwf = NULL;
	static Swf *zuJoSwf = NULL;
	if (!sharedSwf) sharedSwf = loadSwf("C:/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/swf/Shared.swf");
	// if (!sofiSwf) sofiSwf = loadSwf("C:/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/swf/SofiAnim.swf");
	// if (!zuJoSwf) zuJoSwf = loadSwf("C:/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/swf/ZuJo.swf");

	startSkiaFrame();

	// SwfSprite *sofiSprite = getSpriteByName(sofiSwf, "SofiAnim");
	// SwfSprite *zuSprite = getSpriteByName(zuJoSwf, "ZuAnim");
	// SwfSprite *joSprite = getSpriteByName(zuJoSwf, "JoAnim");
	SwfSprite *dummyRect = getSpriteByName(sharedSwf, "SkiaDummyRect");

	auto doDrawSprite = [](SwfSprite *sprite, Vec2 offset) {
		CreateSpriteTransforms(transforms, transformsNum, 16);
		SpriteTransform *mainTrans = getSpriteTransform(transforms, &transformsNum, "_");
		mainTrans->frame = (int)(platform->time * 24.0) % sprite->framesNum-1;
		mainTrans->matrix.TRANSLATE(getSize(skiaSys->backTexture)/2 + offset);
		drawSprite(sprite, transforms, transformsNum);
	};

	// doDrawSprite(sofiSprite, v2());
	// doDrawSprite(zuSprite, v2());
	// doDrawSprite(joSprite, v2(-100, 0));
	if (keyPressed('A')) doDrawSprite(dummyRect, v2(0, 0));

	endSkiaFrame();

	{
		RenderProps props = newRenderProps();
		props.matrix.SCALE(getSize(skiaSys->backTexture));
		drawTexture(skiaSys->backTexture, props);
	}
#endif

#if 0
	if (platform->frameCount == -) {
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
#endif

#if 0
	if (platform->frameCount == -) {
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
#if 0
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
#endif

#if 0 // Hue shift test
	static int tint = 0xFFFFFFFF;
	static float hueShift = 0;
	guiInputArgb("tint", &tint);
	ImGui::SliderFloat("hueShiftRad", &hueShift, 0, 2*M_PI);
	ImGui::SliderFloat("hueShiftDeg", &hueShift, 0, 360);

	Texture *texture = getTexture("assets/images/bulletTest.png");

	startShader(renderer->danmakuShader);

	Vec4 hueShiftVec = v4(hueShift / (2*M_PI));
	Raylib::SetShaderValue(
		renderer->danmakuShader,
		renderer->danmakuShaderHueShiftValueLoc,
		&hueShiftVec,
		Raylib::SHADER_UNIFORM_VEC4
	);

	Matrix3 matrix = mat3();
	matrix.SCALE(getSize(texture));
	matrix.SCALE(10);
	drawSimpleTexture(texture, matrix);

	endShader();
#endif

#if 0 // ngui test
	clearRenderer(0xFF000000);

	static Xform2 axeXform = {v2(), v2(1, 1), 0};
	static Xform2 beeXform = {v2(), v2(1, 1), 0};
	static Xform2 bootXform = {v2(), v2(1, 1), 0};

	static NguiStyleStack style1 = {};
	static NguiStyleStack style2 = {};
	static int chosenStyle = 0;

	if (platform->frameCount > 0 && game->debugMode) {
		ImGui::Begin("Icon editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::DragFloat2("axe translation", &axeXform.translation.x, 0.01);
		ImGui::DragFloat2("axe scale", &axeXform.scale.x, 0.01);
		ImGui::DragFloat("axe rotation", &axeXform.rotation);
		ImGui::DragFloat2("bee translation", &beeXform.translation.x, 0.01);
		ImGui::DragFloat2("bee scale", &beeXform.scale.x, 0.01);
		ImGui::DragFloat("bee rotation", &beeXform.rotation);
		ImGui::DragFloat2("boot translation", &bootXform.translation.x, 0.01);
		ImGui::DragFloat2("boot scale", &bootXform.scale.x, 0.01);
		ImGui::DragFloat("boot rotation", &bootXform.rotation);
		ImGui::End();

#if 0
		ImGui::Begin("Default Style editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		DataStream *codeStream = newDataStream();
		for (int i = 0; i < ngui->globalStyleStack.varsNum; i++) {
			ImGui::PushID(i);
			NguiStyleVar *var = &ngui->globalStyleStack.vars[i];
			NguiStyleTypeInfo *styleTypeInfo = &ngui->styleTypeInfos[var->type];
			if (styleTypeInfo->dataType == NGUI_DATA_TYPE_INT) {
				writeString(codeStream, frameSprintf("nguiPushStyleInt(%s, %d);\n", styleTypeInfo->enumName, *(int *)var->data));
				ImGui::InputInt(styleTypeInfo->name, (int *)var->data);
			} else if (styleTypeInfo->dataType == NGUI_DATA_TYPE_COLOR_INT) {
				writeString(codeStream, frameSprintf("nguiPushStyleColorInt(%s, 0x%08X);\n", styleTypeInfo->enumName, *(int *)var->data));
				guiInputArgb(styleTypeInfo->name, (int *)var->data);
			} else if (styleTypeInfo->dataType == NGUI_DATA_TYPE_FLOAT) {
				writeString(codeStream, frameSprintf("nguiPushStyleFloat(%s, %g);\n", styleTypeInfo->enumName, *(float *)var->data));
				ImGui::DragFloat(styleTypeInfo->name, (float *)var->data, 0.01);
			} else if (styleTypeInfo->dataType == NGUI_DATA_TYPE_VEC2) {
				Vec2 *vec = (Vec2 *)var->data;
				writeString(codeStream, frameSprintf("nguiPushStyleVec2(%s, v2(%g, %g));\n", styleTypeInfo->enumName, vec->x, vec->y));
				ImGui::DragFloat2(styleTypeInfo->name, (float *)var->data, 0.01);
			} else if (styleTypeInfo->dataType == NGUI_DATA_TYPE_STRING_PTR) {
				writeString(codeStream, frameSprintf("nguiPushStyleStringPtr(%s, \"%s\");\n", styleTypeInfo->enumName, *(char **)var->data));
				ImGui::Text("%s: %s", styleTypeInfo->name, *(char **)var->data);
			}

			ImGui::PopID();
		}

		codeStream->index = 0;
		char *code = NULL;
		for (;;) {
			char *str = readFrameString(codeStream);
			if (!str || codeStream->dataMax - codeStream->index <= 4) break;
			if (code == NULL) {
				code = frameStringClone(str);
			} else {
				code = frameSprintf("%s%s", code, str);
			}
		}
		ImGui::InputTextMultiline("Code", code, strlen(code), ImVec2(1500, 400));

		destroyDataStream(codeStream);
		ImGui::End();
#endif
		ImGui::Begin("Style editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::RadioButton("No style", &chosenStyle, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Style 1", &chosenStyle, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Style 2", &chosenStyle, 2);

		NguiStyleStack *styleStack = NULL;
		if (chosenStyle == 1) styleStack = &style1;
		if (chosenStyle == 2) styleStack = &style2;

		if (styleStack) nguiShowImGuiStyleEditor(styleStack);

		ImGui::End();
	}

	if (platform->frameCount == 0) {
		nguiInit();
	}

	ngui->mouse = platform->mouse;

	static bool showingSubElements = false;

	NguiStyleStack *styleStack = NULL;
	if (chosenStyle == 1) styleStack = &style1;
	if (chosenStyle == 2) styleStack = &style2;
	if (styleStack) {
		nguiPushStyleStack(styleStack);
	}

	nguiStartWindow("Test Window");

	nguiPushStyleIconXform(axeXform);
	nguiPushStylePtr(NGUI_STYLE_ICON_PTR, getTexture("assets/images/icons/axe.png"));
	if (nguiButton("Hello", "subText")) {
		logf("You clicked the button!\n");
		showingSubElements = !showingSubElements;
	}
	nguiPopStyleVar(NGUI_STYLE_ICON_PTR);
	nguiPopStyleIconXform();

	nguiPushStyleIconXform(beeXform);
	nguiPushStylePtr(NGUI_STYLE_ICON_PTR, getTexture("assets/images/icons/bee.png"));
	if (nguiButton("Dummy button 1")) ;
	nguiPushStyleColorInt(NGUI_STYLE_ACTIVE_TINT, 0xFFFF0000);
	if (nguiButton("Dummy button 2", "This button goes red")) ;
	nguiPopStyleVar(NGUI_STYLE_ACTIVE_TINT);
	if (nguiButton("Dummy button 3")) ;
	if (nguiButton("Dummy button 4")) ;
	nguiPopStyleVar(NGUI_STYLE_ICON_PTR);
	nguiPopStyleIconXform();

	if (nguiButton("Second button")) logf("You clicked the second button!\n");

	if (showingSubElements) {
		bool doIndent = false;
		if (nguiGetStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW) == 1) doIndent = true;
		if (doIndent) nguiPushStyleFloat(NGUI_STYLE_INDENT, 80);

		nguiPushStyleIconXform(bootXform);
		nguiPushStylePtr(NGUI_STYLE_ICON_PTR, getTexture("assets/images/icons/boot.png"));
		if (nguiButton("Nested item 1")) logf("You clicked the first nested button!\n");
		if (nguiButton("Nested item 2")) logf("You clicked the second nested button!\n");
		if (nguiButton("Nested item 3")) logf("You clicked the third nested button!\n");
		if (nguiButton("Nested item 4")) logf("You clicked the fourth nested button!\n");
		nguiPopStyleVar(NGUI_STYLE_ICON_PTR);
		nguiPopStyleIconXform();

		if (doIndent) nguiPopStyleVar(NGUI_STYLE_INDENT);
	}

	if (nguiButton("Third button")) logf("You clicked the third button!\n");

	nguiEndWindow();

	nguiPushStyleVec2(NGUI_STYLE_BUTTON_HOVER_OFFSET, v2(-20, 0));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_POSITION, v2(platform->windowWidth, 0));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_PIVOT, v2(1, 0));
	nguiStartWindow("Combat");
	nguiPopStyleVar(NGUI_STYLE_WINDOW_PIVOT);
	nguiPopStyleVar(NGUI_STYLE_WINDOW_POSITION);

	static bool attackOpen = false;
	if (nguiButton("Attack")) attackOpen = !attackOpen;
	if (attackOpen) {
		nguiPushStyleFloat(NGUI_STYLE_INDENT, -80);
		int optionChoosen = 0;

		nguiPushStyleColorInt(NGUI_STYLE_ACTIVE_TINT, 0x80FF0000);
		if (nguiButton("Slash")) optionChoosen = 1;
		if (nguiButton("Stab")) optionChoosen = 2;
		if (nguiButton("Chop")) optionChoosen = 3;
		nguiPopStyleVar(NGUI_STYLE_ACTIVE_TINT);

		if (optionChoosen != 0) {
			logf("Did attack %d\n", optionChoosen);
			attackOpen = false;
		}
		nguiPopStyleVar(NGUI_STYLE_INDENT);
	}

	nguiPushStyleInt(NGUI_STYLE_ELEMENT_DISABLED, 1);
	if (nguiButton("Magic")) ;
	nguiPopStyleVar(NGUI_STYLE_ELEMENT_DISABLED);

	static bool itemsOpen = false;
	nguiPushStyleFloat(NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS, 0.8);
	nguiPushStyleColorInt(NGUI_STYLE_ACTIVE_TINT, 0xA00000FF);
	if (nguiButton("Items")) itemsOpen = !itemsOpen;
	nguiPopStyleVar(NGUI_STYLE_ACTIVE_TINT);
	nguiPopStyleVar(NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS);

	static float musicVolume = 1;
	nguiSlider("Music", &musicVolume);

	nguiEndWindow();
	nguiPopStyleVar(NGUI_STYLE_BUTTON_HOVER_OFFSET);

	if (itemsOpen) {
		nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 4);

		nguiPushStyleVec2(NGUI_STYLE_WINDOW_POSITION, v2(platform->windowWidth/2, 0));
		nguiPushStyleVec2(NGUI_STYLE_WINDOW_PIVOT, v2(0.5, 0));
		nguiStartWindow("ItemsWindow");
		nguiPopStyleVar(NGUI_STYLE_WINDOW_PIVOT);
		nguiPopStyleVar(NGUI_STYLE_WINDOW_POSITION);

		nguiPushStyleVec2(NGUI_STYLE_ELEMENT_SIZE, v2(50, 50));
		nguiPushStyleVec2(NGUI_STYLE_LABEL_GRAVITY, v2(0.5, 0.5));
		nguiPushStyleVec2(NGUI_STYLE_BUTTON_HOVER_OFFSET, v2(0, -20));
		for (int i = 0; i < 20; i++) {
			if (nguiButton(frameSprintf("Item #%d", i))) {
				logf("You clicked item %d\n", i);
			}
		}
		nguiPopStyleVar(NGUI_STYLE_BUTTON_HOVER_OFFSET);
		nguiPopStyleVar(NGUI_STYLE_LABEL_GRAVITY);
		nguiPopStyleVar(NGUI_STYLE_ELEMENT_SIZE);

		nguiEndWindow();

		nguiPopStyleVar(NGUI_STYLE_ELEMENTS_IN_ROW);
	}

	nguiPushStyleVec2(NGUI_STYLE_WINDOW_POSITION, v2(platform->windowWidth/2, platform->windowHeight));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_PIVOT, v2(0.5, 1));
	nguiStartWindow("Settings");
	nguiPopStyleVar(NGUI_STYLE_WINDOW_PIVOT);
	nguiPopStyleVar(NGUI_STYLE_WINDOW_POSITION);

	static bool settingsOpen = false;
	if (nguiButton("Open")) {
		settingsOpen = !settingsOpen;
	}
	if (settingsOpen) {
		nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 3);
		if (nguiButton("A")) ;
		if (nguiButton("B")) ;
		if (nguiButton("C")) ;
		if (nguiButton("D")) ;
		if (nguiButton("E")) ;
		if (nguiButton("F")) ;
		if (nguiButton("G")) ;
		if (nguiButton("H")) ;
		nguiPopStyleVar(NGUI_STYLE_ELEMENTS_IN_ROW);
	}
	nguiEndWindow();


	if (styleStack) {
	nguiPopStyleStack(styleStack);
	}

	nguiDraw(elapsed);
#endif

#if 0 // Assemble Dalle 2 mini images
	static Font *hugeFont = createFont("assets/common/arial.ttf", 120);
	if (platform->frameCount == 2) {
		int filesNum;
		char **files = getDirectoryList("D:/vmShared/dalle/images", &filesNum);

		char **paths = (char **)frameMalloc(sizeof(char *) * filesNum);
		int pathsNum = filesNum;
		for (int i = 0; i < filesNum; i++) paths[i] = files[i];

		auto getOnlyFileName = [](char *path)->char * {
			path = frameStringClone(path);

			char *lastSlash = strrchr(path, '/');
			if (lastSlash) path = lastSlash+1;

			char *dotPtr = strrchr(path, '.');
			if (dotPtr) *dotPtr = 0;
			return path;
		};

		for (;;) {
			if (pathsNum == 0) break;
			char *currentPrefix = getOnlyFileName(paths[0]);
			stripNumbersFromEndOfString(currentPrefix);

			Texture *textures[9];
			int texturesNum = 0;

			logf("Prefix: %s\n", currentPrefix);
			for (int i = 0; i < pathsNum; i++) {
				char *fullPath = paths[i];
				char *path = getOnlyFileName(fullPath);

				char *strippedPath = frameStringClone(path);
				stripNumbersFromEndOfString(strippedPath);
				if (streq(currentPrefix, strippedPath)) {
					textures[texturesNum++] = createTexture(fullPath);
					logf("%s\n", path);
					arraySpliceIndex(paths, pathsNum, sizeof(char *), i);
					pathsNum--;
					i--;
					continue;
				}
			}

			int bgColor = 0xFF040524;

			int rows = 3;
			int cols = 3;

			float textHeight = 128;

			Vec2 edgePadding = v2(16, 16);
			Vec2 innerPadding = v2(8, 8);
			Vec2 textureSize = v2(256, 256);

			Vec2 size;
			size.x = textureSize.x * cols + edgePadding.x*2 + ((cols-1) * innerPadding.x);
			size.y = textureSize.y * rows + edgePadding.y*2 + ((rows-1) * innerPadding.y);
			size.y += textHeight;
			RenderTexture *finalTexture = createRenderTexture(size.x, size.y);

			pushTargetTexture(finalTexture);
			clearRenderer(bgColor);

			Rect textRect = makeRect(0, 0, finalTexture->width, textHeight);
			DrawTextProps props = newDrawTextProps(hugeFont, 0xFFFFFFFF);
			drawTextInRect(currentPrefix, props, textRect);

			Rect bottomRect = makeRect(finalTexture);
			bottomRect.height -= textHeight;
			bottomRect.y += textHeight;
			// drawRect(bottomRect, 0xFFFF0000);

			int textureIndex = 0;
			for (int y = 0; y < rows; y++) {
				for (int x = 0; x < cols; x++) {
					if (textureIndex > texturesNum-1) break;
					Texture *texture = textures[textureIndex];
					if (!texture) {
						logf("Texture is NULL\n");
						break;
					}

					Rect rect;
					rect.width = texture->width;
					rect.height = texture->height;
					rect.x = x * (rect.width+innerPadding.x) + edgePadding.x/2 + bottomRect.x;
					rect.y = y * (rect.height+innerPadding.y) + edgePadding.y/2 + bottomRect.y;

					// drawRect(rect, 0xFF00FF00);

					RenderProps props = newRenderProps();
					props.matrix.TRANSLATE(rect.x, rect.y);
					props.matrix.SCALE(rect.width, rect.height);
					props.srcWidth = props.srcHeight = 1;
					drawTexture(texture, props);

					textureIndex++;
				}
			}

			popTargetTexture();

#if 1
			Raylib::Image raylibImage = Raylib::LoadImageFromTexture(finalTexture->raylibRenderTexture.texture);
			Raylib::ImageFlipVertical(&raylibImage);
			if (!Raylib::ExportImage(raylibImage, frameSprintf("%s/assets/images/dalle/%s.png", filePathPrefix, currentPrefix))) {
				logf("Failed to save\n");
			}
#else
			u8 *data = getTextureData(finalTexture);
			if (!stbi_write_png(frameSprintf("%s/assets/images/dalle/%s.png", filePathPrefix, currentPrefix), finalTexture->width, finalTexture->width, 4, data, finalTexture->width * 4)) {
				logf("Failed to write image\n");
			}
			free(data);
#endif
			// destroyTexture(finalTexture);
		}

		exit(0);
	}
#endif

#if 0
	static RenderTexture *soundTexture = NULL;;
	if (!soundTexture) {
		u8 *oggBytes;
		int oggBytesSize;

		oggBytes = (u8 *)readFile("assets/audio/monoPluck.ogg", &oggBytesSize);

		int channels, sampleRate;
		s16 *samples;
		int samplesNum = stb_vorbis_decode_memory(oggBytes, oggBytesSize, &channels, &sampleRate, &samples);

		if (channels != 1) logf("Not mono!!!\n");

		soundTexture = createRenderTexture(800, 100);

		pushTargetTexture(soundTexture);
		clearRenderer(0xFF000000);
		for (int i = 0; i < soundTexture->width; i++) {
			int sampleIndex = clampMap(i, 0, soundTexture->width, 0, samplesNum);
			float sample = clampMap(samples[sampleIndex], SHRT_MIN, SHRT_MAX, -1, 1);

			Vec2 start = v2(i, soundTexture->height/2);
			Vec2 end = start;
			end.y += sample * (soundTexture->height/2);
			// logf("%f (%d) (%d)\n", sample, samples[sampleIndex], sampleIndex);
			drawLine(start, end, 1, 0xFFFFFFFF);
		}
		popTargetTexture();
	}
	drawSimpleTexture(soundTexture);
#endif

#if 0
	char *url = "http://192.168.0.106/rss-bridge/?action=display&bridge=Youtube&context=Search+result&s=vsauce+reaction&pa=&duration_min=2&duration_max=100&format=Atom";
	char *hexStr = convertToHexString(url, strlen(url)+1);
	logf("http://192.168.0.106/rssGame.bin?URLyt_hyperpop_reaction__X__%s\n", hexStr);
	// if (keyJustPressed(' ')) {
	// 	Sound *sound = getSound();
	// }
#endif

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
