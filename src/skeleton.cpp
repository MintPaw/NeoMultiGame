struct SkeletonAnimation {
	char *name;
	int firstFrame;
	int frameCount; 
	/// Unserialized
	float frameRate;
};

struct Bone {
	char *name;
	s8 parent;
	// char *parentName;

	Matrix4 localSpaceMatrix;
	Xform *poseXforms;

	/// Unserialized
	Matrix4 modelSpaceMatrix;
	Matrix4 invModelSpaceMatrix;
};

#define BONES_MAX 32
struct BaseSkeleton {
	char name[PATH_MAX_LEN];
	int frameCount;

	Bone *bones;
	int bonesNum;

	SkeletonAnimation *animations;
	int animationsNum;

	/// Unserialized
};

enum SkeletonBlendType {
	SKELETON_BLEND_ANIMATION,
	SKELETON_BLEND_MANUAL_BONES,
};

struct SkeletonBlend {
	SkeletonBlendType type;

#define BLEND_NAME_MAX_LEN 32
	char name[BLEND_NAME_MAX_LEN];
	SkeletonAnimation *animation;

	bool playing;
	bool loops;
	float time;
	float weight;
	float timeScale;

	Xform *poseXforms;
	bool *controlMask;
};

struct Skeleton {
	BaseSkeleton *base;

#define SKELETON_BLENDS_MAX 16
	SkeletonBlend blends[SKELETON_BLENDS_MAX];
	int blendsNum;

	Matrix4 *meshTransforms;
	float time;
};

struct SkeletonSystem {
#define BASE_SKELETONS_MAX 128
	BaseSkeleton baseSkeletons[BASE_SKELETONS_MAX];
	int baseSkeletonsNum;

	bool disableFrameBlending;
};

SkeletonSystem *skeletonSys = NULL;

void initSkeleton();
BaseSkeleton *getBaseSkeleton(char *path);
void readAnimation(DataStream *stream, SkeletonAnimation *anim);
void readBone(DataStream *stream, Bone *bone, int frameCount);
void readBone2(DataStream *stream, Bone *bone, int frameCount);
void readBaseSkeleton(DataStream *stream, BaseSkeleton *base);
Skeleton *deriveSkeleton(char *skeletonPath);
Skeleton *deriveSkeleton(BaseSkeleton *base);
SkeletonBlend *createSkeletonBlend(Skeleton *skeleton, const char *name, SkeletonBlendType type);
SkeletonBlend *getSkeletonBlend(Skeleton *skeleton, const char *name);
int getSkeletonAnimationIndexByName(Skeleton *skeleton, const char *name);
SkeletonAnimation *getAnimation(Skeleton *skeleton, const char *name);
int getBoneIndex(BaseSkeleton *base, const char *boneName);
int getBoneIndex(Skeleton *skeleton, const char *boneName);
void updateSkeleton(Skeleton *skeleton, float elapsed);
void destroySkeleton(Skeleton *skeleton);

/// FUNCTIONS ^

void initSkeleton() {
	skeletonSys = (SkeletonSystem *)zalloc(sizeof(SkeletonSystem));
}

BaseSkeleton *getBaseSkeleton(char *path) {
	if (!skeletonSys) initSkeleton();

	for (int i = 0; i < skeletonSys->baseSkeletonsNum; i++) {
		BaseSkeleton *base = &skeletonSys->baseSkeletons[i];
		if (streq(base->name, path)) return base;
	}

	if (!fileExists(path)) return NULL;

	if (skeletonSys->baseSkeletonsNum > BASE_SKELETONS_MAX-1) {
		logf("Too many baseSkeletons\n");
		return NULL;
	}

	BaseSkeleton *base = &skeletonSys->baseSkeletons[skeletonSys->baseSkeletonsNum++];
	memset(base, 0, sizeof(BaseSkeleton));
	strcpy(base->name, path);

	char *dir = frameMalloc(PATH_MAX_LEN);
	strcpy(dir, path);
	char *lastSlash = strrchr(dir, '/');
	if (lastSlash) *lastSlash = 0;

	DataStream *stream = loadDataStream(path);
	readBaseSkeleton(stream, base);
	destroyDataStream(stream);

	return base;
}

void readAnimation(DataStream *stream, SkeletonAnimation *anim) {
	anim->name = readString(stream);
	anim->firstFrame = readU32(stream);
	anim->frameCount = readU32(stream);
	anim->frameRate = 60;
}

void readBone(DataStream *stream, Bone *bone, int frameCount) {
	bone->name = readString(stream);
	bone->parent = (s8)readU8(stream);
	bone->modelSpaceMatrix = readMatrix4(stream);
	bone->localSpaceMatrix = bone->modelSpaceMatrix;
	bone->invModelSpaceMatrix = bone->modelSpaceMatrix.invert();
	bone->poseXforms = (Xform *)zalloc(sizeof(Xform) * frameCount);
	for (int i = 0; i < frameCount; i++) {
		Matrix4 matrix = readMatrix4(stream);
		bone->poseXforms[i] = toXform(matrix);
	}
}

void readBone2(DataStream *stream, Bone *bone, int frameCount) {
	bone->name = readString(stream);
	bone->parent = (s8)readU8(stream);
	Xform xform;
	xform.translation = readVec3(stream);
	xform.rotation = eulerToQuaternion(readVec3(stream));
	xform.scale = readVec3(stream);

	bone->modelSpaceMatrix = toMatrix(xform);
	bone->localSpaceMatrix = bone->modelSpaceMatrix;
	bone->invModelSpaceMatrix = bone->modelSpaceMatrix.invert();
	bone->poseXforms = (Xform *)zalloc(sizeof(Xform) * frameCount);
	for (int i = 0; i < frameCount; i++) {
		bone->poseXforms[i].translation = readVec3(stream);
		Vec3 eu = readVec3(stream);
		bone->poseXforms[i].rotation = eulerToQuaternion(eu);
		bone->poseXforms[i].scale = readVec3(stream);
	}
}

void readBaseSkeleton(DataStream *stream, BaseSkeleton *base) {
	int version = readU8(stream);
	if (version == 1) logf("Loading version 1 skeleton\n");

	char *name = readString(stream);
	strncpy(base->name, name, PATH_MAX_LEN);
	free(name);

	base->frameCount = readU32(stream);

	base->bonesNum = readU32(stream);
	base->bones = (Bone *)zalloc(sizeof(Bone) * base->bonesNum);
	for (int i = 0; i < base->bonesNum; i++) {
		Bone *bone = &base->bones[i];
		if (version == 1) {
			readBone(stream, bone, base->frameCount);
		} else {
			readBone2(stream, bone, base->frameCount);
		}
	}

	base->animationsNum = readU32(stream);
	base->animations = (SkeletonAnimation *)zalloc(sizeof(SkeletonAnimation) * base->animationsNum);
	for (int i = 0; i < base->animationsNum; i++) {
		SkeletonAnimation *anim = &base->animations[i];
		readAnimation(stream, anim);
	}

	for (int i = 0; i < base->bonesNum; i++) {
		Bone *bone = &base->bones[i];
		if (bone->parent < 0) continue;
		Bone *parent = &base->bones[bone->parent];
		bone->localSpaceMatrix = parent->modelSpaceMatrix.invert() * bone->modelSpaceMatrix;
	}

	if (base->bonesNum > BONES_MAX) logf ("Too many bones (%d on %s)!!!\n", base->bonesNum, base->name);
}

Skeleton *deriveSkeleton(char *skeletonPath) {
	BaseSkeleton *base = getBaseSkeleton(skeletonPath);
	if (!base) {
		logf("No base skeleton at %s\n", skeletonPath);
		return NULL;
	}
	Skeleton *skeleton = deriveSkeleton(base);
	return skeleton;
}

Skeleton *deriveSkeleton(BaseSkeleton *base) {
	Skeleton *skeleton = (Skeleton *)zalloc(sizeof(Skeleton));
	skeleton->base = base;
	skeleton->meshTransforms = (Matrix4 *)zalloc(sizeof(Matrix4) * base->bonesNum);
	for (int i = 0; i < base->bonesNum; i++) {
		skeleton->meshTransforms[i] = mat4();
	}
	return skeleton;
}

SkeletonBlend *createSkeletonBlend(Skeleton *skeleton, const char *name, SkeletonBlendType type) {
	if (skeleton->blendsNum > SKELETON_BLENDS_MAX-1) {
		logf("Too many blends on %s\n", skeleton->base->name);
		return NULL;
	}

	SkeletonBlend *blend = &skeleton->blends[skeleton->blendsNum++];
	memset(blend, 0, sizeof(SkeletonBlend));
	blend->type = type;
	strncpy(blend->name, name, BLEND_NAME_MAX_LEN);
	blend->weight = 1;
	blend->timeScale = 1;
	blend->playing = true;
	blend->loops = true;
	blend->poseXforms = (Xform *)zalloc(sizeof(Xform) * skeleton->base->bonesNum);
	blend->controlMask = (bool *)zalloc(sizeof(bool) * skeleton->base->bonesNum);
	for (int i = 0; i < skeleton->base->bonesNum; i++) blend->controlMask[i] = false;

	return blend;
}

SkeletonBlend *getSkeletonBlend(Skeleton *skeleton, const char *name) {
	for (int i = 0; i < skeleton->blendsNum; i++) {
		SkeletonBlend *blend = &skeleton->blends[i];
		if (streq(blend->name, name)) {
			return blend;
		}
	}

	return NULL;
}

int getSkeletonAnimationIndexByName(Skeleton *skeleton, const char *name) {
	for (int i = 0; i < skeleton->base->animationsNum; i++) {
		SkeletonAnimation *anim = &skeleton->base->animations[i];
		char *animName = anim->name;
		// if (strchr(animName, '_')) animName = strchr(animName, '_')+1;

		if (streq(animName, name)) return i;
	}

	return -1;
}

SkeletonAnimation *getAnimation(Skeleton *skeleton, const char *name) {
	int index = getSkeletonAnimationIndexByName(skeleton, name);
	if (index == -1) return NULL;
	return &skeleton->base->animations[index];
}

int getBoneIndex(Skeleton *skeleton, const char *boneName) {
	for (int i = 0; i < skeleton->base->bonesNum; i++) {
		Bone *bone = &skeleton->base->bones[i];
		if (streq(bone->name, boneName)) return i;
	}

	return -1;
}

int getBoneIndex(BaseSkeleton *base, const char *boneName) {
	for (int i = 0; i < base->bonesNum; i++) {
		Bone *bone = &base->bones[i];
		if (streq(bone->name, boneName)) return i;
	}

	return -1;
}

void updateSkeleton(Skeleton *skeleton, float elapsed) {
	{ /// Generate pose per layer
		for (int i = 0; i < skeleton->blendsNum; i++) {
			SkeletonBlend *blend = &skeleton->blends[i];
			if (blend->type == SKELETON_BLEND_ANIMATION) {
				SkeletonAnimation *anim = blend->animation;
				if (!anim) {
					if (blend->time > 0) logf("Skeleton blend %s has no animation\n", blend->name);
					continue;
				}

				if (blend->playing) blend->time += elapsed*blend->timeScale;

				float floatFramesIn = blend->time * anim->frameRate;
				int framesIn = blend->time * anim->frameRate;
				int nextFramesIn = framesIn+1;
				float framePerc = floatFramesIn - framesIn;

				if (blend->loops) {
					framesIn = fmod(framesIn, anim->frameCount);
					nextFramesIn = fmod(nextFramesIn, anim->frameCount);
				} else {
					if (framesIn > anim->frameCount-1) framesIn = anim->frameCount-1;
					if (nextFramesIn > anim->frameCount-1) nextFramesIn = anim->frameCount-1;
				}
				framesIn += anim->firstFrame;
				nextFramesIn += anim->firstFrame;

				for (int i = 0; i < skeleton->base->bonesNum; i++) {
					blend->controlMask[i] = true;

					Bone *bone = &skeleton->base->bones[i];

					if (skeletonSys->disableFrameBlending) {
						blend->poseXforms[i] = bone->poseXforms[framesIn];
					} else {
						blend->poseXforms[i] = lerp(bone->poseXforms[framesIn], bone->poseXforms[nextFramesIn], framePerc);
					}
				}
			} else if (blend->type == SKELETON_BLEND_MANUAL_BONES) {
			} else {
				logf("Unknown blend type\n");
			}
		}
	} ///

	Xform *poseXforms = (Xform *)frameMalloc(sizeof(Xform) * BONES_MAX);
	{ /// Blend layers together
		if (skeleton->base->bonesNum > BONES_MAX) {
			logf("Too many pose xform\n");
			return;
		}

		for (int i = 0; i < skeleton->blendsNum; i++) {
			SkeletonBlend *blend = &skeleton->blends[i];
			for (int i = 0; i < skeleton->base->bonesNum; i++) {
				if (!blend->controlMask[i]) continue;
				Xform xform = blend->poseXforms[i];

				poseXforms[i].translation += xform.translation*blend->weight;

				if (poseXforms[i].rotation.dot(xform.rotation) < 0) xform.rotation = xform.rotation.negate();
				poseXforms[i].rotation += xform.rotation*blend->weight;

				poseXforms[i].scale += xform.scale*blend->weight;
			}
		}
	} ///

	{ /// Convert local space poses to model space
		Matrix4 *currentTransforms = (Matrix4 *)frameMalloc(sizeof(Matrix4) * BONES_MAX);

		for (int i = 0; i < skeleton->base->bonesNum; i++) {
			Bone *bone = &skeleton->base->bones[i];

			if (bone->parent >= 0) {
				currentTransforms[i] = currentTransforms[bone->parent] * toMatrix(poseXforms[i]);
			} else {
				currentTransforms[i] = toMatrix(poseXforms[i]);
			}

			skeleton->meshTransforms[i] = currentTransforms[i] * bone->invModelSpaceMatrix;
		}
	} ///

	skeleton->time += elapsed;
}

void destroySkeleton(Skeleton *skeleton) {
	for (int i = 0; i < skeleton->blendsNum; i++) {
		SkeletonBlend *blend = &skeleton->blends[i];
		free(blend->poseXforms);
		free(blend->controlMask);
	}

	// free(skeleton->currentTransforms);
	free(skeleton);
}
