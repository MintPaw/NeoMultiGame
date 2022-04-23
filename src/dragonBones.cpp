#define DB_USE_SPRITE_SHEET 0

struct DbXform {
	Vec2 translation;
	float rotation;
	Vec2 scale;
	Vec2 shear;

	DbXform operator*= (float b);
	DbXform operator*= (DbXform b);
};
DbXform operator* (DbXform a, float b) {
	a.rotation *= b;
	a.translation *= b;
	a.scale = lerp(v2(1, 1), a.scale, b);
	a.shear *= b;
	return a;
}

DbXform operator* (DbXform a, DbXform b) {
	a.rotation += b.rotation;
	a.translation += b.translation;
	a.scale *= b.scale;
	a.shear += b.shear;
	return a;
}

DbXform DbXform::operator*= (float b) { *this = *this * b; return *this; }
DbXform DbXform::operator*= (DbXform b) { *this = *this * b; return *this; }

struct DbBone {
	char *name;
	char *parentName;
	float length;
	DbXform xform;

	Matrix3 localMatrix;
	Matrix3 modelMatrix;
	Matrix3 invModelMatrix;
};

struct DbSlot {
	char *name;
	char *bone;
	char *defaultAttachment;
};

struct DbVertex {
	Vec2 position;
	Vec2 uv;
	int boneInds[4];
	float boneWeights[4];
};

enum DbAttachmentType {
	DB_ATTACHMENT_REGION,
	DB_ATTACHMENT_MESH,
};
struct DbAttachment {
	char *name;
	char *path;
	DbAttachmentType type;
	DbVertex *verts;
	int vertsNum;
	u16 *inds;
	int indsNum;

	Vec2 regionPos;
	Vec2 regionScale;
	float regionRotation;
	Vec2 regionSize;
	Matrix3 regionMatrix;

#if !DB_USE_SPRITE_SHEET
	Texture *texture;
#endif
};
struct DbSkinSlot {
	char *name;
	DbAttachment *attachments;
	int attachmentsNum;
};
struct DbSkin {
	char *name;
	DbSkinSlot *skinSlots;
	int skinSlotsNum;
};

struct DbRotateKey {
	float time;
	Vec4 curve;
	float angle;
};

struct DbTranslateKey {
	float time;
	Vec4 curve;
	Vec2 delta;
};

struct DbScaleKey {
	float time;
	Vec4 curve;
	Vec2 scale;
};

struct DbShearKey {
	float time;
	Vec4 curve;
	Vec2 shear;
};

struct DbAnimationBone {
	char *name;

	DbRotateKey *rotateKeys;
	int rotateKeysNum;

	DbTranslateKey *translateKeys;
	int translateKeysNum;

	DbScaleKey *scaleKeys;
	int scaleKeysNum;

	DbShearKey *shearKeys;
	int shearKeysNum;
};

struct DbAnimation {
	char *name;
	float length;

	DbAnimationBone *animatedBones;
	int animatedBonesNum;
};

struct DbIk {
	char *name;
	char *target;
	bool bendPositive;
	char *bones[2];
	int bonesNum;
};

struct DbBaseSkeleton {
	DbBone *bones;
	int bonesNum;

	DbSlot *slots;
	int slotsNum;

	DbSkin *skins;
	int skinsNum;

	DbAnimation *animations;
	int animationsNum;

	DbIk *iks;
	int iksNum;
};

enum DbSkeletonTrackType {
	DB_ANIMATION_TRACK,
	DB_MANUAL_TRACK,
};

struct DbManualBoneData {
	char *boneName;
	DbXform xform;
};
struct DbSkeletonTrack {
	DbSkeletonTrackType type;

	DbAnimation *animation;
	float time;
	float weight;
	float speed;

	DbManualBoneData *manualBoneData;
	int manualBoneDataNum;
};

struct DbDrawSpriteAfterSlotProps {
	char *slotName;
	Texture *texture;
	RenderProps renderProps;
};

struct DbSkeleton {
	DbBaseSkeleton *base;
	Matrix3 graphicMatrix;

	Matrix3 *animationTransforms;
	Matrix3 *modelTransforms;

#define SKELETON_TRACKS_MAX 8
	DbSkeletonTrack tracks[SKELETON_TRACKS_MAX];
	int tracksNum;

#define DB_DRAW_SPRITE_AFTER_SLOT_PROPS_MAX 16
	DbDrawSpriteAfterSlotProps drawSpriteAfterSlotProps[DB_DRAW_SPRITE_AFTER_SLOT_PROPS_MAX];
	int drawSpriteAfterSlotPropsNum;
};

DbBone *getBone(DbBaseSkeleton *base, char *boneName);
int getBoneIndex(DbBaseSkeleton *base, char *boneName);
DbSkin *getSkin(DbBaseSkeleton *base, char *skinName);
DbSkinSlot *getSkinSlot(DbSkin *skin, char *skinSlotName);
DbAttachment *getAttachment(DbBaseSkeleton *base, char *skinName, char *slotName, char *attachmentName);
DbXform makeDbXform();
Matrix3 toMatrix(DbXform xform);

void drawAttachment(DbSkeleton *skeleton, DbAttachment *attachment, Matrix3 graphicMatrix, char *boneName,Matrix3 postBoneMatrix=mat3(),Matrix3 attachmentMatrix=mat3());
int getAnimationIndex(DbSkeleton *skeleton, char *name);
Matrix3 getBoneMatrix(DbSkeleton *skeleton, char *boneName);
Line2 getBoneLine(DbSkeleton *skeleton, char *boneName);
/// FUNCTIONS ^

DbBaseSkeleton *loadDbBaseSkeleton(char *path);
DbBaseSkeleton *loadDbBaseSkeleton(char *path) {
	char *dir = frameStringClone(path);
	char *lastSlash = strrchr(dir, '/');
	if (lastSlash) *lastSlash = 0;

	DbBaseSkeleton *base = NULL;

	{ // json
		base = (DbBaseSkeleton *)zalloc(sizeof(DbBaseSkeleton));

		int jsonStrSize;
		char *jsonStr = (char *)readFile(frameSprintf("%s.json", path), &jsonStrSize);

		cJSON *json = cJSON_ParseWithLength(jsonStr, jsonStrSize);
		if (!json) logf("Failed to parse\n");

		// cJSON *jSkeleton = cJSON_GetObjectItem(json, "base");

		cJSON *jBones = cJSON_GetObjectItem(json, "bones");
		base->bonesNum = cJSON_GetArraySize(jBones);
		base->bones = (DbBone *)zalloc(sizeof(DbBone) * base->bonesNum);
		for (int i = 0; i < base->bonesNum; i++) {
			DbBone *bone = &base->bones[i];
			bone->xform = makeDbXform();
			cJSON *jBone = cJSON_GetArrayItem(jBones, i);

			bone->name = stringClone(cJSON_GetStringValue(cJSON_GetObjectItem(jBone, "name")));
			if (cJSON_HasObjectItem(jBone, "parent")) bone->parentName       = stringClone(cJSON_GetStringValue(cJSON_GetObjectItem(jBone, "parent")));
			if (cJSON_HasObjectItem(jBone, "length")) bone->length           = cJSON_GetNumberValue(cJSON_GetObjectItem(jBone, "length"));
			if (cJSON_HasObjectItem(jBone, "x")) bone->xform.translation.x   = cJSON_GetNumberValue(cJSON_GetObjectItem(jBone, "x"));
			if (cJSON_HasObjectItem(jBone, "y")) bone->xform.translation.y   = cJSON_GetNumberValue(cJSON_GetObjectItem(jBone, "y"));
			if (cJSON_HasObjectItem(jBone, "rotation")) bone->xform.rotation = cJSON_GetNumberValue(cJSON_GetObjectItem(jBone, "rotation"));
			if (cJSON_HasObjectItem(jBone, "scaleX")) bone->xform.scale.x    = cJSON_GetNumberValue(cJSON_GetObjectItem(jBone, "scaleX"));
			if (cJSON_HasObjectItem(jBone, "scaleY")) bone->xform.scale.y    = cJSON_GetNumberValue(cJSON_GetObjectItem(jBone, "scaleY"));
			if (cJSON_HasObjectItem(jBone, "shearX")) bone->xform.shear.x    = cJSON_GetNumberValue(cJSON_GetObjectItem(jBone, "shearX"));
			if (cJSON_HasObjectItem(jBone, "shearY")) bone->xform.shear.y    = cJSON_GetNumberValue(cJSON_GetObjectItem(jBone, "shearY"));

			bone->localMatrix = toMatrix(bone->xform);
			if (i == 0) bone->localMatrix.SCALE(v2(1, -1));
			// if (i == 0) bone->localMatrix.ROTATE(180);

			if (bone->parentName) {
				DbBone *parent = getBone(base, bone->parentName);
				bone->modelMatrix = parent->modelMatrix * bone->localMatrix;
			} else {
				bone->modelMatrix = bone->localMatrix;
			}
			bone->invModelMatrix = bone->modelMatrix.invert();
		}

		cJSON *jSlots = cJSON_GetObjectItem(json, "slots");
		base->slotsNum = cJSON_GetArraySize(jSlots);
		base->slots = (DbSlot *)zalloc(sizeof(DbSlot) * base->slotsNum);
		for (int i = 0; i < base->slotsNum; i++) {
			DbSlot *slot = &base->slots[i];
			cJSON *jSlot = cJSON_GetArrayItem(jSlots, i);

			slot->name = stringClone(cJSON_GetStringValue(cJSON_GetObjectItem(jSlot, "name")));
			slot->bone = stringClone(cJSON_GetStringValue(cJSON_GetObjectItem(jSlot, "bone")));
			if (cJSON_HasObjectItem(jSlot, "attachment")) slot->defaultAttachment = stringClone(cJSON_GetStringValue(cJSON_GetObjectItem(jSlot, "attachment")));
		}

		cJSON *jSkins = cJSON_GetObjectItem(json, "skins");
		base->skinsNum = cJSON_GetArraySize(jSkins);
		base->skins = (DbSkin *)zalloc(sizeof(DbSkin) * base->skinsNum);
		for (int i = 0; i < base->skinsNum; i++) {
			DbSkin *skin = &base->skins[i];
			cJSON *jSkin = cJSON_GetArrayItem(jSkins, i);

			skin->name = stringClone(jSkin->string);

			skin->skinSlotsNum = cJSON_GetArraySize(jSkin);
			skin->skinSlots = (DbSkinSlot *)zalloc(sizeof(DbSkinSlot) * skin->skinSlotsNum);
			for (int i = 0; i < skin->skinSlotsNum; i++) {
				DbSkinSlot *skinSlot = &skin->skinSlots[i];
				cJSON *jSkinSlot = cJSON_GetArrayItem(jSkin, i);

				skinSlot->name = stringClone(jSkinSlot->string);

				skinSlot->attachmentsNum = cJSON_GetArraySize(jSkinSlot);
				skinSlot->attachments = (DbAttachment *)zalloc(sizeof(DbAttachment) * skinSlot->attachmentsNum);

				for (int i = 0; i < skinSlot->attachmentsNum; i++) {
					DbAttachment *attachment = &skinSlot->attachments[i];
					cJSON *jAttachment = cJSON_GetArrayItem(jSkinSlot, i);

					attachment->name = stringClone(jAttachment->string);
					if (cJSON_HasObjectItem(jAttachment, "path")) {
						attachment->path = stringClone(cJSON_GetStringValue(cJSON_GetObjectItem(jAttachment, "path")));
					} else {
						attachment->path = stringClone(attachment->name);
					}

					char *typeStr = "region";
					if (cJSON_HasObjectItem(jAttachment, "type")) typeStr = cJSON_GetStringValue(cJSON_GetObjectItem(jAttachment, "type"));
					if (strstr(typeStr, "region")) {
						attachment->type = DB_ATTACHMENT_REGION;
					} else if (strstr(typeStr, "mesh")) {
						attachment->type = DB_ATTACHMENT_MESH;
					} else {
						logf("Unknown type of attachment (%s is type %s)\n", attachment->name, typeStr);
					}

#if !DB_USE_SPRITE_SHEET
					char *texturePath = frameSprintf("%s_texture/%s.png", path, attachment->path);
					attachment->texture = getTexture(texturePath);
					if (!attachment->texture) logf("Failed to load %s\n", texturePath);
#endif

					if (attachment->type == DB_ATTACHMENT_MESH) {
						cJSON *jUvs = cJSON_GetObjectItem(jAttachment, "uvs");
						cJSON *jVerts = cJSON_GetObjectItem(jAttachment, "vertices");

						attachment->vertsNum = cJSON_GetArraySize(jUvs)/2;
						attachment->verts = (DbVertex *)zalloc(sizeof(DbVertex) * attachment->vertsNum);

						int jVertsNum = cJSON_GetArraySize(jVerts);
						int hasBones = attachment->vertsNum*2 != jVertsNum;

						float *jFloatVerts = (float *)frameMalloc(sizeof(float) * jVertsNum);
						for (int i = 0; i < jVertsNum; i++) jFloatVerts[i] = cJSON_GetNumberValue(cJSON_GetArrayItem(jVerts, i));

						int currentVertIndex = 0;
						int floatInd = 0;
						for (;;) {
							if (currentVertIndex == attachment->vertsNum) break;

							DbVertex *vert = &attachment->verts[currentVertIndex];

							if (hasBones) {
								int bonesCount = jFloatVerts[floatInd++];
								for (int i = 0; i < bonesCount; i++) {
									int boneInd = jFloatVerts[floatInd++];
									float x = jFloatVerts[floatInd++];
									float y = jFloatVerts[floatInd++];
									float weight = jFloatVerts[floatInd++];

									for (int i = 0; i < 4; i++) {
										if (vert->boneWeights[i] < weight) {
											vert->boneWeights[i] = weight;
											vert->boneInds[i] = boneInd;

											DbBone *bone = &base->bones[(int)boneInd];
											vert->position = bone->modelMatrix * v2(x, y);
											break;
										}
									}
								}
								Vec4 vecWeights = v4(vert->boneWeights[0], vert->boneWeights[1], vert->boneWeights[2], vert->boneWeights[3]).normalize();
								vert->boneWeights[0] = vecWeights.x;
								vert->boneWeights[1] = vecWeights.y;
								vert->boneWeights[2] = vecWeights.z;
								vert->boneWeights[3] = vecWeights.w;
							} else {
								float x = jFloatVerts[floatInd++];
								float y = jFloatVerts[floatInd++];
								vert->position = v2(x, -y);
								vert->boneWeights[0] = 1;
							}

							currentVertIndex++;
						}

						for (int i = 0; i < attachment->vertsNum; i++) {
							DbVertex *vert = &attachment->verts[i];
							vert->uv.x = cJSON_GetNumberValue(cJSON_GetArrayItem(jUvs, i*2 + 0));
							vert->uv.y = cJSON_GetNumberValue(cJSON_GetArrayItem(jUvs, i*2 + 1));
						}

						cJSON *jTriangles = cJSON_GetObjectItem(jAttachment, "triangles");
						attachment->indsNum = cJSON_GetArraySize(jTriangles);
						attachment->inds = (u16 *)zalloc(sizeof(u16) * attachment->indsNum);
						for (int i = 0; i < attachment->indsNum; i++) {
							attachment->inds[i] = cJSON_GetNumberValue(cJSON_GetArrayItem(jTriangles, i));
						}
					} else if (attachment->type == DB_ATTACHMENT_REGION) {
						attachment->regionScale = v2(1, 1);
						// attachment->regionSize = getSize(attachment->texture);
						if (cJSON_HasObjectItem(jAttachment, "x")) attachment->regionPos.x = cJSON_GetNumberValue(cJSON_GetObjectItem(jAttachment, "x"));
						if (cJSON_HasObjectItem(jAttachment, "y")) attachment->regionPos.y = -cJSON_GetNumberValue(cJSON_GetObjectItem(jAttachment, "y"));
						if (cJSON_HasObjectItem(jAttachment, "scaleX")) attachment->regionScale.x = cJSON_GetNumberValue(cJSON_GetObjectItem(jAttachment, "scaleX"));
						if (cJSON_HasObjectItem(jAttachment, "scaleY")) attachment->regionScale.y = cJSON_GetNumberValue(cJSON_GetObjectItem(jAttachment, "scaleY"));
						if (cJSON_HasObjectItem(jAttachment, "rotation")) attachment->regionRotation = -cJSON_GetNumberValue(cJSON_GetObjectItem(jAttachment, "rotation"));
						if (cJSON_HasObjectItem(jAttachment, "width")) attachment->regionSize.x = cJSON_GetNumberValue(cJSON_GetObjectItem(jAttachment, "width"));
						if (cJSON_HasObjectItem(jAttachment, "height")) attachment->regionSize.y = cJSON_GetNumberValue(cJSON_GetObjectItem(jAttachment, "height"));

						attachment->regionMatrix = mat3();
						attachment->regionMatrix.SCALE(1, -1);
						attachment->regionMatrix.TRANSLATE(attachment->regionPos);
						attachment->regionMatrix.ROTATE(attachment->regionRotation);
						attachment->regionMatrix.SCALE(attachment->regionScale);
					}
				}
			}
		}

		cJSON *jAnimations = cJSON_GetObjectItem(json, "animations");
		base->animationsNum = cJSON_GetArraySize(jAnimations);
		base->animations = (DbAnimation *)zalloc(sizeof(DbAnimation) * base->animationsNum);
		for (int i = 0; i < base->animationsNum; i++) {
			cJSON *jAnimation = cJSON_GetArrayItem(jAnimations, i);
			DbAnimation *animation = &base->animations[i];

			animation->name = stringClone(jAnimation->string);

			cJSON *jBones = cJSON_GetObjectItem(jAnimation, "bones");
			animation->animatedBonesNum = cJSON_GetArraySize(jBones);
			animation->animatedBones = (DbAnimationBone *)zalloc(sizeof(DbAnimationBone) * animation->animatedBonesNum);
			for (int i = 0; i < animation->animatedBonesNum; i++) {
				cJSON *jBone = cJSON_GetArrayItem(jBones, i);
				DbAnimationBone *bone = &animation->animatedBones[i];
				bone->name = stringClone(jBone->string);
				int keyGroupsNum = cJSON_GetArraySize(jBone);
				for (int i = 0; i < keyGroupsNum; i++) {
					cJSON *jKeyGroup = cJSON_GetArrayItem(jBone, i);
					int keysNum = cJSON_GetArraySize(jKeyGroup);
					if (streq(jKeyGroup->string, "rotate")) {
						bone->rotateKeysNum = keysNum;
						bone->rotateKeys = (DbRotateKey *)zalloc(sizeof(DbRotateKey) * keysNum);
					} else if (streq(jKeyGroup->string, "translate")) {
						bone->translateKeysNum = keysNum;
						bone->translateKeys = (DbTranslateKey *)zalloc(sizeof(DbTranslateKey) * keysNum);
					} else if (streq(jKeyGroup->string, "scale")) {
						bone->scaleKeysNum = keysNum;
						bone->scaleKeys = (DbScaleKey *)zalloc(sizeof(DbScaleKey) * keysNum);
					} else if (streq(jKeyGroup->string, "shear")) {
						bone->shearKeysNum = keysNum;
						bone->shearKeys = (DbShearKey *)zalloc(sizeof(DbShearKey) * keysNum);
					} else {
						logf("Unknown key group type %s\n", jKeyGroup->string);
					}

					for (int i = 0; i < keysNum; i++) {
						cJSON *jKey = cJSON_GetArrayItem(jKeyGroup, i);
						float time = 0;
						Vec4 curve = v4(0.5, 0.5, 0.5, 0.5);
						if (cJSON_HasObjectItem(jKey, "time")) time = cJSON_GetNumberValue(cJSON_GetObjectItem(jKey, "time"));
						if (animation->length < time) animation->length = time;
						if (cJSON_HasObjectItem(jKey, "curve")) {
							cJSON *jCurve = cJSON_GetObjectItem(jKey, "curve");
							curve.x = cJSON_GetNumberValue(cJSON_GetArrayItem(jCurve, 0));
							curve.y = cJSON_GetNumberValue(cJSON_GetArrayItem(jCurve, 1));
							curve.z = cJSON_GetNumberValue(cJSON_GetArrayItem(jCurve, 2));
							curve.w = cJSON_GetNumberValue(cJSON_GetArrayItem(jCurve, 3));
						}

						if (streq(jKeyGroup->string, "rotate")) {
							DbRotateKey *key = &bone->rotateKeys[i];
							key->time = time;
							key->curve = curve;
							if (cJSON_HasObjectItem(jKey, "angle")) key->angle = cJSON_GetNumberValue(cJSON_GetObjectItem(jKey, "angle"));
						} else if (streq(jKeyGroup->string, "translate")) {
							DbTranslateKey *key = &bone->translateKeys[i];
							key->time = time;
							key->curve = curve;
							if (cJSON_HasObjectItem(jKey, "x")) key->delta.x = cJSON_GetNumberValue(cJSON_GetObjectItem(jKey, "x"));
							if (cJSON_HasObjectItem(jKey, "y")) key->delta.y = cJSON_GetNumberValue(cJSON_GetObjectItem(jKey, "y"));
						} else if (streq(jKeyGroup->string, "scale")) {
							DbScaleKey *key = &bone->scaleKeys[i];
							key->time = time;
							key->curve = curve;
							key->scale = v2(1, 1);
							if (cJSON_HasObjectItem(jKey, "x")) key->scale.x = cJSON_GetNumberValue(cJSON_GetObjectItem(jKey, "x"));
							if (cJSON_HasObjectItem(jKey, "y")) key->scale.y = cJSON_GetNumberValue(cJSON_GetObjectItem(jKey, "y"));
						} else if (streq(jKeyGroup->string, "shear")) {
							DbShearKey *key = &bone->shearKeys[i];
							key->time = time;
							key->curve = curve;
							if (cJSON_HasObjectItem(jKey, "x")) key->shear.x = cJSON_GetNumberValue(cJSON_GetObjectItem(jKey, "x"));
							if (cJSON_HasObjectItem(jKey, "y")) key->shear.y = cJSON_GetNumberValue(cJSON_GetObjectItem(jKey, "y"));
						} else {
							logf("Unknown key group type %s\n", jKeyGroup->string);
						}
					}
				}
			}
		}

		cJSON *jIks = cJSON_GetObjectItem(json, "ik");
		base->iksNum = cJSON_GetArraySize(jIks);
		base->iks = (DbIk *)zalloc(sizeof(DbIk) * base->iksNum);

		for (int i = 0; i < base->iksNum; i++) {
			cJSON *jIk = cJSON_GetArrayItem(jIks, i);
			DbIk *ik = &base->iks[i];

			ik->name = stringClone(cJSON_GetStringValue(cJSON_GetObjectItem(jIk, "name")));
			ik->target = stringClone(cJSON_GetStringValue(cJSON_GetObjectItem(jIk, "target")));
			if (cJSON_HasObjectItem(jIk, "bendPositive")) ik->bendPositive = cJSON_GetNumberValue(cJSON_GetObjectItem(jIk, "bendPositive"));

			cJSON *jBones = cJSON_GetObjectItem(jIk, "bones");
			ik->bonesNum = cJSON_GetArraySize(jBones);
			for (int i = 0; i < ik->bonesNum; i++) {
				ik->bones[i] = stringClone(cJSON_GetStringValue(cJSON_GetArrayItem(jBones, i)));
			}
		}

		free(jsonStr);
	}

#if DB_USE_SPRITE_SHEET
	char *spritesDir = frameSprintf("%s_texture", path);
	char *sheetsDir = frameSprintf("%s_atlas", path);
	char *frameInfoPath = frameSprintf("%s/frameInfo.txt", dir);
	packSpriteSheet(spritesDir, frameInfoPath, sheetsDir, false);
#endif

	return base;
}

DbBone *getBone(DbBaseSkeleton *base, char *boneName) {
	for (int i = 0; i < base->bonesNum; i++) {
		DbBone *bone = &base->bones[i];
		if (streq(bone->name, boneName)) {
			return bone;
		}
	}

	logf("Couldn't find bone %s\n", boneName);
	return NULL;
}

int getBoneIndex(DbBaseSkeleton *base, char *boneName) {
	for (int i = 0; i < base->bonesNum; i++) {
		DbBone *bone = &base->bones[i];
		if (streq(bone->name, boneName)) {
			return i;
		}
	}

	logf("Couldn't find bone %s\n", boneName);
	return -1;
}


DbSkin *getSkin(DbBaseSkeleton *base, char *skinName) {
	for (int i = 0; i < base->skinsNum; i++) {
		DbSkin *skin = &base->skins[i];
		if (streq(skin->name, skinName)) return skin;
	}

	logf("Couldn't find skin %s\n", skinName);
	return NULL;
}

DbSkinSlot *getSkinSlot(DbSkin *skin, char *skinSlotName) {
	for (int i = 0; i < skin->skinSlotsNum; i++) {
		DbSkinSlot *skinSlot = &skin->skinSlots[i];
		if (streq(skinSlot->name, skinSlotName)) return skinSlot;
	}

	return NULL;
}

DbAttachment *getAttachment(DbBaseSkeleton *base, char *skinName, char *slotName, char *attachmentName) {
	if (!attachmentName) return NULL;

	DbSkin *skin = getSkin(base, skinName);
	DbSkinSlot *skinSlot = getSkinSlot(skin, slotName);
	if (!skinSlot) return NULL;

	for (int i = 0; i < skinSlot->attachmentsNum; i++) {
		DbAttachment *attachment = &skinSlot->attachments[i];
		if (streq(attachment->name, attachmentName)) return attachment;
	}

	logf("Can't find attachment %s\n", attachmentName);
	return NULL;
}

void updateSkeleton(DbSkeleton *skeleton, float elapsed);
void updateSkeleton(DbSkeleton *skeleton, float elapsed) {
	DbXform *poseXforms = (DbXform *)frameMalloc(sizeof(DbXform) * skeleton->base->bonesNum);

	for (int i = 0; i < skeleton->base->bonesNum; i++) {
		skeleton->animationTransforms[i] = mat3();
		skeleton->modelTransforms[i] = mat3();
		poseXforms[i] = makeDbXform();
	}

	for (int i = 0; i < SKELETON_TRACKS_MAX; i++) {
		DbSkeletonTrack *track = &skeleton->tracks[i];
		if (track->type == DB_ANIMATION_TRACK) {
			DbAnimation *animation = track->animation;
			if (!animation) continue;

			for (int i = 0; i < animation->animatedBonesNum; i++) {
				DbAnimationBone *animBone = &animation->animatedBones[i];
				int boneIndex = getBoneIndex(skeleton->base, animBone->name);
				DbXform xform = makeDbXform();

				{
					DbRotateKey *key0 = NULL;
					for (int i = animBone->rotateKeysNum-1; i >= 0; i--) {
						DbRotateKey *key = &animBone->rotateKeys[i];
						if (key->time <= track->time) { key0 = key; break; }
					}

					DbRotateKey *key1 = NULL;
					for (int i = 0; i < animBone->rotateKeysNum; i++) {
						DbRotateKey *key = &animBone->rotateKeys[i];
						if (key->time > track->time) { key1 = key; break; }
					}

					if ((key0 && key1) && key0->time != key1->time) {
						float perc = cubicBezier(key0->curve, norm(key0->time, key1->time, track->time));
						xform.rotation = lerpDeg(key0->angle, key1->angle, perc);
					} else if (key0) {
						xform.rotation = key0->angle;
					}
				}

				{
					DbTranslateKey *key0 = NULL;
					for (int i = animBone->translateKeysNum-1; i >= 0; i--) {
						DbTranslateKey *key = &animBone->translateKeys[i];
						if (key->time <= track->time) { key0 = key; break; }
					}

					DbTranslateKey *key1 = NULL;
					for (int i = 0; i < animBone->translateKeysNum; i++) {
						DbTranslateKey *key = &animBone->translateKeys[i];
						if (key->time > track->time) { key1 = key; break; }
					}
					if (!key1) key1 = key0;

					if ((key0 && key1) && key0->time != key1->time) {
						float perc = cubicBezier(key0->curve, norm(key0->time, key1->time, track->time));
						xform.translation = lerp(key0->delta, key1->delta, perc);
					} else if (key0) {
						xform.translation = key0->delta;
					}
				}

				{
					DbScaleKey *key0 = NULL;
					for (int i = animBone->scaleKeysNum-1; i >= 0; i--) {
						DbScaleKey *key = &animBone->scaleKeys[i];
						if (key->time <= track->time) { key0 = key; break; }
					}

					DbScaleKey *key1 = NULL;
					for (int i = 0; i < animBone->scaleKeysNum; i++) {
						DbScaleKey *key = &animBone->scaleKeys[i];
						if (key->time > track->time) { key1 = key; break; }
					}
					if (!key1) key1 = key0;

					if ((key0 && key1) && key0->time != key1->time) {
						float perc = cubicBezier(key0->curve, norm(key0->time, key1->time, track->time));
						xform.scale = lerp(key0->scale, key1->scale, perc);
					} else if (key0) {
						xform.scale = key0->scale;
					}
				}

				{
					DbShearKey *key0 = NULL;
					for (int i = animBone->shearKeysNum-1; i >= 0; i--) {
						DbShearKey *key = &animBone->shearKeys[i];
						if (key->time <= track->time) { key0 = key; break; }
					}

					DbShearKey *key1 = NULL;
					for (int i = 0; i < animBone->shearKeysNum; i++) {
						DbShearKey *key = &animBone->shearKeys[i];
						if (key->time > track->time) { key1 = key; break; }
					}
					if (!key1) key1 = key0;

					if ((key0 && key1) && key0->time != key1->time) {
						float perc = cubicBezier(key0->curve, norm(key0->time, key1->time, track->time));
						xform.shear = lerp(key0->shear, key1->shear, perc);
					} else if (key0) {
						xform.shear = key0->shear;
					}
				}

				poseXforms[boneIndex] *= xform * track->weight;

				// DbXform *outXform = &poseXforms[boneIndex];
				// outXform->rotation += xform.rotation * track->weight;
				// outXform->translation += xform.translation * track->weight;
				// outXform->scale *= lerp(v2(1, 1), xform.scale, track->weight);
				// outXform->shear += xform.shear * track->weight;
			}

			if (animation->length != 0) track->time = fmod(track->time+(elapsed*track->speed), animation->length);
		} else if (track->type == DB_MANUAL_TRACK) {
			for (int i = 0; i < track->manualBoneDataNum; i++) {
				DbManualBoneData *data = &track->manualBoneData[i];
				int boneIndex = getBoneIndex(skeleton->base, data->boneName);
				poseXforms[boneIndex] *= data->xform * track->weight;
			}
		}
	}

	for (int i = 0; i < skeleton->base->bonesNum; i++) {
		DbBone *bone = &skeleton->base->bones[i];

#if 0
		Matrix3 parentMatrix = mat3();
		if (bone->parentName) {
			int parentIndex = getBoneIndex(skeleton->base, bone->parentName);
			parentMatrix = skeleton->modelTransforms[parentIndex];
			if (parentIndex > i) logf("Bad bone order!\n");
		}

		Matrix3 matrix = parentMatrix * bone->localMatrix * toMatrix(poseXforms[i]);

		skeleton->animationTransforms[i] = toMatrix(poseXforms[i]);
		skeleton->modelTransforms[i] = parentMatrix * bone->localMatrix * skeleton->animationTransforms[i];
#else
		Matrix3 parentMatrix = mat3();
		if (bone->parentName) {
			int parentIndex = getBoneIndex(skeleton->base, bone->parentName);
			parentMatrix = skeleton->modelTransforms[parentIndex];
		}

		Matrix3 matrix = parentMatrix * bone->localMatrix * toMatrix(poseXforms[i]);

		skeleton->modelTransforms[i] = matrix;
		skeleton->animationTransforms[i] = matrix * bone->invModelMatrix;
#endif
	}
}	

DbSkeleton *createDbSkeleton(DbBaseSkeleton *base);
DbSkeleton *createDbSkeleton(DbBaseSkeleton *base) {
	DbSkeleton *skeleton = (DbSkeleton *)zalloc(sizeof(DbSkeleton));
	skeleton->base = base;
	skeleton->animationTransforms = (Matrix3 *)zalloc(sizeof(Matrix3) * skeleton->base->bonesNum);
	skeleton->modelTransforms = (Matrix3 *)zalloc(sizeof(Matrix3) * skeleton->base->bonesNum);
	for (int i = 0; i < SKELETON_TRACKS_MAX; i++) {
		DbSkeletonTrack *track = &skeleton->tracks[i];
		track->weight = 1;
		track->speed = 1;

		track->manualBoneData = (DbManualBoneData *)zalloc(sizeof(DbManualBoneData) * skeleton->base->bonesNum);
	}

	return skeleton;
}

DbXform makeDbXform() {
	DbXform xform = {};
	xform.scale = v2(1, 1);
	return xform;
}

Matrix3 toMatrix(DbXform xform) {
	if (!isZero(xform.shear)) Panic("Bone shear 3:");
	// if (!equal(xform.scale, v2(1, 1))) Panic("Bone scale 3:");

	Matrix3 matrix = mat3();
	matrix.TRANSLATE(xform.translation);
	matrix.ROTATE(xform.rotation);
	matrix.SCALE(xform.scale);
	// matrix.SHEAR(xform.shear);

	return matrix;
}

/// Drawing

struct DrawSkeletonProps {
	bool showBones;
	Matrix3 matrix;
};

DrawSkeletonProps newDrawSkeletonProps() {
	DrawSkeletonProps props = {};
	props.matrix = mat3();
	return props;
}

void drawAttachment(DbSkeleton *skeleton, DbAttachment *attachment, Matrix3 graphicMatrix, char *boneName, Matrix3 postBoneMatrix, Matrix3 attachmentMatrix) {
	Frame *frame = getFrame(frameSprintf("%s.png", attachment->path));
	if (attachment->type == DB_ATTACHMENT_REGION) {
		int boneIndex = getBoneIndex(skeleton->base, boneName);
		RenderProps props = newRenderProps();
		props.matrix = graphicMatrix;
		props.matrix *= skeleton->modelTransforms[boneIndex];
		props.matrix *= postBoneMatrix;
		props.matrix *= attachment->regionMatrix;
		props.matrix *= attachmentMatrix;
#if DB_USE_SPRITE_SHEET
		props.matrix.TRANSLATE(frame->destOffX, frame->destOffY);
		props.matrix.TRANSLATE(-frame->srcWidth/2, -frame->srcHeight/2);
		props.uvMatrix.TRANSLATE(frame->srcX / (float)frame->texture->width, frame->srcY / (float)frame->texture->height);
		props.uvMatrix.SCALE(frame->width / (float)frame->texture->width, frame->height / (float)frame->texture->width);
		props.srcWidth = frame->width;
		props.srcHeight = frame->height;
		drawTexture(frame->texture, props);
#else
		props.matrix.TRANSLATE(-getSize(attachment->texture)/2);
		drawTexture(attachment->texture, props);
#endif
	} else if (attachment->type == DB_ATTACHMENT_MESH) {
		Vec2 *verts = (Vec2 *)frameMalloc(sizeof(Vec2) * attachment->vertsNum);
		Vec2 *uvs = (Vec2 *)frameMalloc(sizeof(Vec2) * attachment->vertsNum);
		for (int i = 0; i < attachment->vertsNum; i++) {
			DbVertex *dbVert = &attachment->verts[i];
			Matrix3 vertexMatrix = {};
			for (int i = 0; i < 4; i++) {
				Matrix3 boneMatrix = skeleton->animationTransforms[dbVert->boneInds[i]];
				vertexMatrix = vertexMatrix.add(boneMatrix * dbVert->boneWeights[i]);
			}
			verts[i] = vertexMatrix * dbVert->position;

#if DB_USE_SPRITE_SHEET
			Vec2 start = v2(frame->srcX, frame->srcY)/getSize(frame->texture);
			Vec2 end = v2(frame->srcX+frame->width, frame->srcY+frame->height)/getSize(frame->texture);
			uvs[i].x = lerp(start.x, end.x, dbVert->uv.x);
			uvs[i].y = lerp(start.y, end.y, dbVert->uv.y);
#else
			uvs[i] = dbVert->uv;
#endif
		}
#if DB_USE_SPRITE_SHEET
		draw2dMesh(verts, uvs, attachment->inds, attachment->indsNum, graphicMatrix, frame->texture);
#else
		draw2dMesh(verts, uvs, attachment->inds, attachment->indsNum, graphicMatrix, attachment->texture);
#endif
	}
}

void drawSkeleton(DbSkeleton *skeleton, DrawSkeletonProps dbProps=newDrawSkeletonProps());
void drawSkeleton(DbSkeleton *skeleton, DrawSkeletonProps dbProps) {
	for (int i = 0; i < skeleton->base->slotsNum; i++) {
		DbSlot *slot = &skeleton->base->slots[i];
		DbAttachment *attachment = getAttachment(skeleton->base, "default", slot->name, slot->defaultAttachment);
		if (!attachment) continue;

		drawAttachment(skeleton, attachment, dbProps.matrix, slot->bone);

		for (int i = 0; i < skeleton->drawSpriteAfterSlotPropsNum; i++) {
			DbDrawSpriteAfterSlotProps *props = &skeleton->drawSpriteAfterSlotProps[i];
			if (streq(props->slotName, slot->name)) {
				drawTexture(props->texture, props->renderProps);
				break;
			}
		}
	}
	skeleton->drawSpriteAfterSlotPropsNum = 0;

	if (dbProps.showBones) {
		Vec2 position = getPosition(dbProps.matrix);
		for (int i = 0; i < skeleton->base->bonesNum; i++) {
			DbBone *bone = &skeleton->base->bones[i];
			if (strstr(bone->name, "_target")) continue;

			// Matrix3 boneTransform = skeleton->modelTransforms[i];
			Matrix3 boneTransform = skeleton->animationTransforms[i] * bone->modelMatrix;
			Vec2 start = boneTransform * v2();
			Vec2 end = boneTransform * v2(bone->length, 0);
			start += position;
			end += position;
			drawLine(start, end, 10, 0x80FF0000);
			drawText(fontSys->defaultFont, bone->name, start + v2(0, 0), 0xFF303030);
		}
	}
}

int getAnimationIndex(DbSkeleton *skeleton, char *name) {
	for (int i = 0; i < skeleton->base->animationsNum; i++) {
		DbAnimation *anim = &skeleton->base->animations[i];
		if (streq(anim->name, name)) return i;
	}

	logf("Couldn't find animation called %s\n", name);
	return -1;
}

Matrix3 getBoneMatrix(DbSkeleton *skeleton, char *boneName) {
	int boneIndex = getBoneIndex(skeleton->base, boneName);
	return skeleton->modelTransforms[boneIndex];
}

Line2 getBoneLine(DbSkeleton *skeleton, char *boneName) {
	int boneIndex = getBoneIndex(skeleton->base, boneName);
	DbBone *bone = &skeleton->base->bones[boneIndex];
	Vec2 start = getPosition(skeleton->modelTransforms[boneIndex]);
	Vec2 end = skeleton->modelTransforms[boneIndex] * v2(bone->length, 0);

	Line2 line = makeLine2(start, end);
	return line;
}
