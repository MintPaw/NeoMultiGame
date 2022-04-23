struct Globals {
};

enum AnimRecordingMode {
	ANIM_PAUSED,
	ANIM_RECORDING,
	ANIM_PLAYING,
};

#define SAVE_VERSION 2

struct AnimControl {
#define ANIM_CONTROL_NAME_MAX_LEN 64
	char name[ANIM_CONTROL_NAME_MAX_LEN];
#define ANIM_CONTROL_BONE_NAME_MAX_LEN 64
	char boneName[ANIM_CONTROL_BONE_NAME_MAX_LEN];

	char controlsTransformConstraint[ANIM_CONTROL_BONE_NAME_MAX_LEN];
	float constraintPower;

	Vec2 *points;
	int pointsNum;
	int pointsMax; // Unserialized

	float playbackSmoothing;
	bool controlsTip;
	float faderControlPerc;
	float cameraControlPerc;

	/// Fader, camera motion

#define VISIBLE_MIXER_GROUPS 8
#define MIXER_GROUPS_MAX 32 // Changing this breaks the file format!
	bool parentMixerGroups[MIXER_GROUPS_MAX];

	// Unserialized
	Vec2 currentPoint;

	bool recording;
	bool playing;
	float power;
	bool enabled;
	bool inMixerGroup;

	AnimRecordingMode prevMode;
	AnimRecordingMode mode;
	int modeFrameCount;

	Vec2 force;
	Vec2 rotationForce;
	// float constraintVelo;
};

struct MixerGroup {
#define MIXER_GROUP_NAME_MAX_LEN 64
	char name[MIXER_GROUP_NAME_MAX_LEN];
	bool enabled;
	float power;
	float targetPower;
	float powerGainRate;
};

enum GameState {
	GAME_NONE,
	GAME_PLAY,
};
struct Game {
	Font *defaultFont;
	// Font *hugeFont;
	RenderTexture *gameTexture;
	RenderTexture *debugTexture;

	Globals globals;
	bool inEditor;
	float timeScale;
	float time;
	Vec2 size;
	float sizeScale;
	Vec2 mouse;

	Vec2 screenOverlayOffset;
	Vec2 screenOverlaySize;

	GameState state;
	GameState prevState;
	GameState nextState;
	float stateTransition_t;
	float stateTime;

	SpineSkeleton *skeleton;

	AnimControl *animControls;
	int animControlsNum;
	int selectedAnimControl;
	bool controlling;

	MixerGroup mixerGroups[MIXER_GROUPS_MAX];

	/// Editor/debug
	bool debugShowFrameTimes;
	bool debugDrawBoneLines;
	bool debugSimpleAnimControllerView;
};
Game *game = NULL;

void runGame();
void updateGame();
void saveAnimations();
void loadAnimations();
/// FUNCTIONS ^

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

	Vec2 res = v2(1600, 900);

	initPlatform(res.x, res.y, "minGame");
	platform->sleepWait = true;
	initAudio();
	initRenderer(res.x, res.y);
	initTextureSystem();
	initFonts();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 20);

		// RegMem(Globals, useFXAA);
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);

		game->timeScale = 1;
		game->size = v2(platform->windowWidth, platform->windowHeight);

		SpineBaseSkeleton *base = loadSpineBaseSkeleton("assets/spine/syrth");
		game->skeleton = deriveSkeleton(base);

		game->animControlsNum = 16;
		game->animControls = (AnimControl *)zalloc(sizeof(AnimControl) * game->animControlsNum);
		for (int i = 0; i < game->animControlsNum; i++) {
			AnimControl *control = &game->animControls[i];
			control->pointsNum = control->pointsMax = 8;
			control->points = (Vec2 *)zalloc(sizeof(Vec2) * control->pointsMax);
		}

		game->debugDrawBoneLines = true;
		game->debugSimpleAnimControllerView = true;

		maximizeWindow();
	}

	Globals *globals = &game->globals;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	game->mouse = (platform->mouse - game->screenOverlayOffset) * (game->size/game->screenOverlaySize);

	{ /// Resizing
		Vec2 ratio = v2(1600.0, 900.0);
		game->sizeScale = MinNum(platform->windowWidth/ratio.x, platform->windowHeight/ratio.y);
		Vec2 newSize = ratio * game->sizeScale;

		if (!equal(game->size, newSize)) {
			game->size = newSize;

			if (game->gameTexture) destroyTexture(game->gameTexture);
			game->gameTexture = NULL;
			if (game->debugTexture) destroyTexture(game->debugTexture);
			game->debugTexture = NULL;

			game->screenOverlaySize = game->size;
			game->screenOverlayOffset.x = (float)platform->windowWidth/2 - game->size.x/2;
			game->screenOverlayOffset.y = (float)platform->windowHeight/2 - game->size.y/2;
		}
	}

	if (!game->gameTexture) game->gameTexture = createRenderTexture(game->size.x, game->size.y);
	pushTargetTexture(game->gameTexture);
	if (!game->debugTexture) game->debugTexture = createRenderTexture(game->size.x, game->size.y);
	static int bgColor = 0xFF3A213A;
	clearRenderer(bgColor);

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;

	if (game->state != game->nextState) {
		game->stateTransition_t += 0.05;
		if (game->stateTransition_t >= 1) game->state = game->nextState;
	} else {
		game->stateTransition_t -= 0.05;
	}
	game->stateTransition_t = Clamp01(game->stateTransition_t);

	if (game->prevState != game->state) {
		game->prevState = game->state;
		game->stateTime = 0;
	}

	for (int i = 0; i < game->animControlsNum; i++) {
		AnimControl *control = &game->animControls[i];
		if (control->prevMode != control->mode) {
			control->prevMode = control->mode;
			control->modeFrameCount = 0;
		}
	}

	if (game->state == GAME_NONE) {
		if (game->stateTime == 0) {
			game->nextState = GAME_PLAY;
		}
	} else if (game->state == GAME_PLAY) {
		SpineSkeleton *skeleton = game->skeleton;
		static Vec2 rootPos = v2(600, 50);
		static Vec2 rootScale = v2(0.35, 0.35);

		static Vec2 cameraPan = v2();

		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
			ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::DragFloat2("rootPos", &rootPos.x);
			ImGui::DragFloat2("rootScale", &rootScale.x, 0.005);
			ImGui::DragFloat2("cameraPan", &cameraPan.x, 0.005);
			guiInputArgb("bgColor", &bgColor);

			ImGui::Checkbox("Draw debug lines", &game->debugDrawBoneLines);
			ImGui::Checkbox("Simple AnimController view", &game->debugSimpleAnimControllerView);

			ImGui::Separator();
			if (ImGui::Button("Save animations")) saveAnimations();
			ImGui::SameLine();
			if (ImGui::Button("Load animations")) loadAnimations();
			ImGui::InputInt("selectedAnimControl", &game->selectedAnimControl);
			game->selectedAnimControl = mathClamp(game->selectedAnimControl, 0, game->animControlsNum-1);
			if (!game->debugSimpleAnimControllerView) {
				if (ImGui::Button("Add AnimControl")) {
					game->animControls = (AnimControl *)resizeArray(game->animControls, sizeof(AnimControl), game->animControlsNum, game->animControlsNum+1);
					AnimControl *control = &game->animControls[game->animControlsNum++];
					control->pointsNum = control->pointsMax = 8;
					control->points = (Vec2 *)zalloc(sizeof(Vec2) * control->pointsMax);
				}
			}

			if (ImGui::TreeNode("Editor")) {
				for (int i = 0; i < game->animControlsNum; i++) {
					AnimControl *control = &game->animControls[i];
					if (ImGui::TreeNode(frameSprintf("%s (%s)###%d", control->name, control->boneName, i))) {
						bool popColor = false;
						if (game->selectedAnimControl == i) {
							popColor = true;
							guiPushStyleColor(ImGuiCol_FrameBg, 0xFF004000);
						}

						ImGui::PushItemWidth(150);
						ImGui::InputText("Control name", control->name, ANIM_CONTROL_NAME_MAX_LEN);
						ImGui::InputText("Bone name", control->boneName, ANIM_CONTROL_BONE_NAME_MAX_LEN);
						ImGui::PopItemWidth();
						ImGui::SameLine();
						ImGui::Checkbox("Controls tip", &control->controlsTip);
						if (!game->debugSimpleAnimControllerView) ImGui::SliderFloat("Fader control", &control->faderControlPerc, 0, 1);
						if (!game->debugSimpleAnimControllerView) ImGui::SliderFloat("Camera control", &control->cameraControlPerc, 0, 1);
						if (!game->debugSimpleAnimControllerView) {
							ImGui::Text("Mixer groups:");
							ImGui::SameLine();
							for (int i = 0; i < VISIBLE_MIXER_GROUPS; i++) {
								ImGui::Checkbox(frameSprintf("%d", i), &control->parentMixerGroups[i]);
								ImGui::SameLine();
							}
							ImGui::NewLine();
						}

						if (!game->debugSimpleAnimControllerView || control->controlsTransformConstraint[0]) {
							ImGui::PushItemWidth(150);
							ImGui::InputText("Constraint", control->controlsTransformConstraint, ANIM_CONTROL_BONE_NAME_MAX_LEN);
							ImGui::SameLine();
							ImGui::InputFloat("Power###constraintPower", &control->constraintPower);
							ImGui::PopItemWidth();
						}
						ImGui::SliderFloat("Playback smoothing", &control->playbackSmoothing, 0, 1);

						ImGui::Checkbox("Enabled", &control->enabled);
						ImGui::SameLine();
						ImGui::SliderFloat("Power", &control->power, 0, 1);

						if (ImGui::Button("Select")) game->selectedAnimControl = i;

						if (control->mode == ANIM_PAUSED) {
							ImGui::SameLine();
							if (ImGui::Button("Play")) {
								control->mode = ANIM_PLAYING;
								control->modeFrameCount = 0;
							}
						} else if (control->mode == ANIM_RECORDING) { 
						} else if (control->mode == ANIM_PLAYING) { 
							ImGui::SameLine();
							if (ImGui::Button("Pause")) {
								control->mode = ANIM_PAUSED;
								control->modeFrameCount = 0;
							}
						}

						if (!game->debugSimpleAnimControllerView && i > 0 && ImGui::Button("Move up")) {
							arraySwap(game->animControls, game->animControlsNum, sizeof(AnimControl), i, i-1);
						}

						if (!game->debugSimpleAnimControllerView && i < game->animControlsNum-1 && ImGui::Button("Move down")) {
							arraySwap(game->animControls, game->animControlsNum, sizeof(AnimControl), i, i+1);
						}

						ImGui::Separator();
						if (popColor) guiPopStyleColor();

						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Mixer")) {
				if (ImGui::BeginTable("mixerGroupTable", 5, ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_BordersH|ImGuiTableFlags_BordersV)) {
					ImGui::TableSetupColumn("#");
					ImGui::TableSetupColumn("Enabled");
					ImGui::TableSetupColumn("Buttons");
					ImGui::TableSetupColumn("Power rate");
					ImGui::TableSetupColumn("Power");
					ImGui::TableHeadersRow();
					ImGui::TableNextColumn();

					for (int i = 0; i < VISIBLE_MIXER_GROUPS; i++) {
						ImGui::PushID(i);

						MixerGroup *group = &game->mixerGroups[i];
						ImGui::Text("%d", i);
						ImGui::TableNextColumn();

						ImGui::Checkbox("###Enabled", &group->enabled);
						ImGui::TableNextColumn();

						if (ImGui::Button("Add power")) {
							logf("I dunno...");
						}
						ImGui::TableNextColumn();

						ImGui::PushItemWidth(150);
						ImGui::SliderFloat("###PowerGainRate", &group->powerGainRate, 0, 0.05);
						ImGui::PopItemWidth();
						ImGui::TableNextColumn();

						ImGui::SliderFloat("###Power", &group->targetPower, 0, 1);
						ImGui::ProgressBar(group->power, ImVec2(100, 0));
						ImGui::TableNextColumn();

						ImGui::PopID();
					}
					ImGui::EndTable();
				}

				if (ImGui::Button("Clear groups")) {
					for (int i = 0; i < game->animControlsNum; i++) {
						AnimControl *control = &game->animControls[i];
						control->inMixerGroup = false;
					}
				}

				ImGui::SameLine();
				if (ImGui::Button("Flip group")) {
					for (int i = 0; i < game->animControlsNum; i++) {
						AnimControl *control = &game->animControls[i];
						if (control->inMixerGroup) {
							control->enabled = !control->enabled;
						}
					}
				}

				if (ImGui::BeginTable("mixerTable", 5, ImGuiTableFlags_SizingStretchProp|ImGuiTableFlags_BordersH|ImGuiTableFlags_BordersV)) {
					ImGui::TableSetupColumn("MG");
					ImGui::TableSetupColumn("Enabled");
					ImGui::TableSetupColumn("Playback");
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Power");
					ImGui::TableHeadersRow();
					ImGui::TableNextColumn();

					for (int i = 0; i < game->animControlsNum; i++) {
						ImGui::PushID(i);
						AnimControl *control = &game->animControls[i];
						if (game->selectedAnimControl == i) guiPushStyleColor(ImGuiCol_FrameBg, 0xFF004000);

						ImGui::Checkbox("###In mixer group", &control->inMixerGroup);
						ImGui::TableNextColumn();

						ImGui::Checkbox("###Enabled", &control->enabled);
						ImGui::TableNextColumn();

						if (control->mode == ANIM_PAUSED) {
							if (ImGui::Button("Play")) {
								control->mode = ANIM_PLAYING;
								control->modeFrameCount = 0;
							}
						} else if (control->mode == ANIM_RECORDING) { 
						} else if (control->mode == ANIM_PLAYING) { 
							if (ImGui::Button("Pause")) {
								control->mode = ANIM_PAUSED;
								control->modeFrameCount = 0;
							}
						}
						ImGui::TableNextColumn();

						ImGui::Text("%d. %s (%s, %s)", i, control->name, control->boneName, control->controlsTransformConstraint);
						ImGui::TableNextColumn();

						// ImGui::PushItemWidth(100);
						ImGui::ProgressBar(control->power);
						// ImGui::PopItemWidth();
						ImGui::TableNextColumn();

						if (game->selectedAnimControl == i) guiPopStyleColor();
						ImGui::Separator();
						ImGui::PopID();
					}

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}

			ImGui::Separator();

			if (ImGui::TreeNode("BoneData")) {
				for (int i = 0; i < skeleton->boneDataNum; i++) {
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

				ImGui::TreePop();
			}
			ImGui::End();
		}

		Matrix3 cameraMatrix = mat3();
		cameraMatrix.TRANSLATE(cameraPan);

		for (int i = 0; i < game->animControlsNum; i++) {
			AnimControl *control = &game->animControls[i];
			if (!control->boneName[0]) continue;
			spBone *bone = spSkeleton_findBone(skeleton->spSkeletonInstance, control->boneName);
			if (!bone) continue;

			if (control->cameraControlPerc) {
				Vec2 dist = v2(bone->data->x, bone->data->y) - v2(bone->x, bone->y);
				cameraMatrix.TRANSLATE(dist * control->cameraControlPerc * control->power);
			}
		}

		pushCamera2d(cameraMatrix);

		if (game->stateTime == 0) {
			for (int i = 0; i < skeleton->boneDataNum; i++) {
				BoneData *data = &skeleton->boneData[i];
				spBone *bone = skeleton->spSkeletonInstance->bones[i];

				if (streq(bone->data->name, "leftBoob")) {
					data->boneEffect = BONE_EFFECT_SPRING_END;
					data->tension = 8;
					data->damping = 0.5;
				}

				if (streq(bone->data->name, "rightBoob")) {
					data->boneEffect = BONE_EFFECT_SPRING_END;
					data->tension = 6;
					data->damping = 0.5;
				}

				if (streq(bone->data->name, "body2")) {
					data->boneEffect = BONE_EFFECT_EASE_END;
				}

				if (streq(bone->data->name, "head")) {
					data->boneEffect = BONE_EFFECT_EASE_END;
					data->easeAmount = 0.25;
				}

				if (streq(bone->data->name, "leftEar1")) {
					data->boneEffect = BONE_EFFECT_EASE_END;
				}

				if (streq(bone->data->name, "leftEar2")) {
					data->boneEffect = BONE_EFFECT_EASE_END;
					data->easeAmount = 0.25;
				}

				if (streq(bone->data->name, "rightEar1")) {
					data->boneEffect = BONE_EFFECT_EASE_END;
				}

				if (streq(bone->data->name, "rightEar2")) {
					data->boneEffect = BONE_EFFECT_EASE_END;
					data->easeAmount = 0.25;
				}

				if (streq(bone->data->name, "leftButt")) {
					data->boneEffect = BONE_EFFECT_SPRING_START;
					data->tension = 10;
					data->damping = 0.5;
				}

				if (streq(bone->data->name, "rightButt")) {
					data->boneEffect = BONE_EFFECT_SPRING_START;
					data->tension = 20;
					data->damping = 0.5;
				}

			}
		}

		updateSkeletonAnimation(skeleton, elapsed);

#if 1
		skeleton->spSkeletonInstance->x = rootPos.x;
		skeleton->spSkeletonInstance->y = rootPos.y;
		skeleton->spSkeletonInstance->scaleX = rootScale.x;
		skeleton->spSkeletonInstance->scaleY = rootScale.y;
#else
		spBone *bone = spSkeleton_findBone(skeleton->spSkeletonInstance, "root");
		bone->x = rootPos.x;
		bone->y = rootPos.y;
		bone->scaleX = rootScale.x;
		bone->scaleY = rootScale.y;
#endif

		// updateSkeletonPhysics(skeleton, 0);

		for (int i = 0; i < game->animControlsNum; i++) {
			AnimControl *control = &game->animControls[i];
			if (!control->boneName[0]) continue;

			spBone *bone = spSkeleton_findBone(skeleton->spSkeletonInstance, control->boneName);
			if (!bone) continue;
			Vec2 force = control->force;
			bone->x += force.x;
			bone->y += force.y;
			bone->rotation += control->rotationForce.y;

			if (control->controlsTransformConstraint[0]) {
				// control->constraintVelo += -control->force.x * control->constraintPower;
				// control->constraintVelo *= 0.85;
				for (int i = 0; i < skeleton->spSkeletonInstance->transformConstraintsCount; i++) {
					spTransformConstraint *constraint = skeleton->spSkeletonInstance->transformConstraints[i];
					if (streq(constraint->data->name, control->controlsTransformConstraint)) {
						// constraint->translateMix += control->constraintVelo;
						constraint->translateMix = lerp(constraint->translateMix, 1, -control->force.x * control->constraintPower);
						constraint->translateMix = Clamp01(constraint->translateMix);
					}
				}
			}
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

		if (game->stateTime == 0) loadAnimations();

		AnimControl *control = &game->animControls[game->selectedAnimControl];
		// if (!control->boneName[0]) strcpy(control->boneName, "body1");

		if (control->mode == ANIM_PAUSED && keyJustPressed('R')) {
			logf("Recording\n");
			control->mode = ANIM_RECORDING;
			control->modeFrameCount = 0;
			control->pointsNum = 0;
		} else if (control->mode == ANIM_PAUSED && keyJustPressed('P')) {
			logf("Playing\n");
			control->mode = ANIM_PLAYING;
			control->modeFrameCount = 0;
		} else if (control->mode == ANIM_RECORDING && keyJustPressed('R')) {
			logf("Paused (recorded %d frames)\n", control->modeFrameCount);
			control->mode = ANIM_PAUSED;
			control->modeFrameCount = 0;
		} else if (control->mode == ANIM_PLAYING && keyJustPressed('P')) {
			logf("Paused\n");
			control->mode = ANIM_PAUSED;
			control->modeFrameCount = 0;
		}

		if (platform->mouseJustDown) game->controlling = !game->controlling;
		// control->power += platform->mousewheel * 0.05;
		// if (platform->mouseWheel) logf("%.2f\n", control->power);
		// control->power = Clamp01(control->power);

		if (control->mode == ANIM_PAUSED && game->controlling) {
			control->currentPoint = game->mouse;
		}

		if (control->mode == ANIM_RECORDING) {
			control->currentPoint = game->mouse;

			if (control->pointsNum > control->pointsMax-1) {
				control->points = (Vec2 *)resizeArray(control->points, sizeof(Vec2), control->pointsMax, control->pointsMax*1.5);
				control->pointsMax *= 1.5;
			}

			control->points[control->pointsNum++] = control->currentPoint;
		}

		if (boneExists(skeleton, control->boneName)) {
			drawCircle(getPosition(getBoneMatrix(skeleton, control->boneName)), 32, 0xFF808080);
		}
		drawCircle(control->currentPoint, 32, 0xFF404040);

		for (int i = 0; i < MIXER_GROUPS_MAX; i++) {
			MixerGroup *group = &game->mixerGroups[i];
			if (!group->enabled) continue;

			group->power = lerp(group->power, group->targetPower, group->powerGainRate);
		}

		for (int i = 0; i < game->animControlsNum; i++) {
			AnimControl *control = &game->animControls[i];
			if (!control->boneName[0]) continue;
			spBone *bone = spSkeleton_findBone(skeleton->spSkeletonInstance, control->boneName);
			if (!bone) continue;

			bool usingMixers = false;
			for (int i = 0; i < MIXER_GROUPS_MAX; i++) {
				MixerGroup *group = &game->mixerGroups[i];
				if (!control->parentMixerGroups[i]) continue;
				if (!group->enabled) continue;

				usingMixers = true;
				control->power = group->power;
			}

			if (!usingMixers) {
				if (control->enabled) {
					control->power += 0.05;
				} else {
					control->power -= 0.05;
				}
			}
			control->power = Clamp01(control->power);

			bool addForces = true;
			if (isZero(control->currentPoint)) addForces = false;

			if (control->mode == ANIM_PLAYING) {
				int pointNum = control->modeFrameCount % control->pointsNum;
				control->currentPoint = lerp(control->currentPoint, control->points[pointNum], 1 - control->playbackSmoothing);
			} else if (control->mode == ANIM_PAUSED) {
				if (!game->controlling || game->selectedAnimControl != i) {
					control->currentPoint = v2();
					addForces = false;
				}
			}

			if (addForces) {
				if (control->controlsTip) {
					Vec2 localPoint = control->currentPoint;
					if (bone->parent) spBone_worldToLocal(bone, localPoint.x, localPoint.y, &localPoint.x, &localPoint.y);
					float degs = toDeg(localPoint);
					control->rotationForce += degs * 0.1 * control->power;
				} else {
					Vec2 localPoint = control->currentPoint;
					if (bone->parent) spBone_worldToLocal(bone->parent, localPoint.x, localPoint.y, &localPoint.x, &localPoint.y);
					Vec2 delta = localPoint - v2(bone->x, bone->y);
					control->force += delta * 0.1 * control->power;
				}
			}

			control->rotationForce *= 0.95;
			control->force *= 0.95;

			control->modeFrameCount++;
		}

		if (game->debugDrawBoneLines) {
			for (int i = 0; i < skeleton->boneDataNum; i++) {
				spBone *bone = skeleton->spSkeletonInstance->bones[i];
				BoneData *data = &skeleton->boneData[i];

				Line2 line = getBoneLine(skeleton, bone->data->name);
				drawLine(line, 4, 0xFF800000);

				drawCircle(data->prevWorldStart, 10, 0xFF000080);
				drawCircle(data->prevWorldEnd, 10, 0xFF800000);
			}
		}

		float fadePerc = 0;
		for (int i = 0; i < game->animControlsNum; i++) {
			AnimControl *control = &game->animControls[i];
			if (control->faderControlPerc) fadePerc = control->faderControlPerc * control->power;
		}

		drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, fadePerc));

		drawCircle(game->mouse, 8, 0x80000000);

		popCamera2d();
	}

	game->stateTime += elapsed;

	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, game->stateTransition_t));
	popTargetTexture(); // game->gameTexture

	clearRenderer();

	{
		RenderTexture *texture = game->gameTexture;
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(game->screenOverlaySize);

		drawFxaaTexture(texture, matrix);
	}

	{
		RenderTexture *texture = game->debugTexture;
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(game->screenOverlaySize);

		drawSimpleTexture(texture, matrix);
	}

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) {
		char *str = frameSprintf("%.1fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
	}

	guiDraw();
	drawOnScreenLog();

	game->time += elapsed;
}

void saveAnimations() {
	DataStream *stream = newDataStream();

	Matrix3 invRootMatrix = getBoneMatrix(game->skeleton, "root").invert();

	writeU32(stream, SAVE_VERSION);
	writeU32(stream, game->animControlsNum);
	for (int i = 0; i < game->animControlsNum; i++) {
		AnimControl *control = &game->animControls[i];
		writeString(stream, control->name);
		writeString(stream, control->boneName);
		writeString(stream, control->controlsTransformConstraint);
		writeFloat(stream, control->constraintPower);
		writeU32(stream, control->pointsNum);
		for (int i = 0; i < control->pointsNum; i++) {
			Vec2 point = invRootMatrix * control->points[i];
			writeVec2(stream, point);
		}

		writeFloat(stream, control->playbackSmoothing);
		writeU8(stream, control->controlsTip);
		writeFloat(stream, control->faderControlPerc);
		writeFloat(stream, control->cameraControlPerc);
		for (int i = 0; i < MIXER_GROUPS_MAX; i++) writeU8(stream, control->parentMixerGroups[i]);
	}


	writeDataStream("assets/info/anims.bin", stream);
	destroyDataStream(stream);
}

void loadAnimations() {
	DataStream *stream = loadDataStream("assets/info/anims.bin");
	if (!stream) return;

	Matrix3 rootMatrix = getBoneMatrix(game->skeleton, "root");

	int version = readU32(stream);

	for (int i = 0; i < game->animControlsNum; i++) {
		free(game->animControls[i].points);
	}
	free(game->animControls);

	game->animControlsNum = readU32(stream);
	game->animControls = (AnimControl *)zalloc(sizeof(AnimControl) * game->animControlsNum);
	for (int i = 0; i < game->animControlsNum; i++) {
		AnimControl *control = &game->animControls[i];
		readStringInto(stream, control->name, ANIM_CONTROL_NAME_MAX_LEN);
		readStringInto(stream, control->boneName, ANIM_CONTROL_BONE_NAME_MAX_LEN);
		readStringInto(stream, control->controlsTransformConstraint, ANIM_CONTROL_BONE_NAME_MAX_LEN);
		control->constraintPower = readFloat(stream);
		control->pointsNum = control->pointsMax = readU32(stream);
		control->points = (Vec2 *)zalloc(sizeof(Vec2) * control->pointsNum);
		for (int i = 0; i < control->pointsNum; i++) {
			control->points[i] = rootMatrix * readVec2(stream);
		}

		control->playbackSmoothing = readFloat(stream);
		control->controlsTip = readU8(stream);
		control->faderControlPerc = readFloat(stream);
		control->cameraControlPerc = readFloat(stream);
		for (int i = 0; i < MIXER_GROUPS_MAX; i++) control->parentMixerGroups[i] = readU8(stream);

		if (control->boneName[0] && !isZero(control->points[0])) control->mode = ANIM_PLAYING;
	}

	destroyDataStream(stream);
}
