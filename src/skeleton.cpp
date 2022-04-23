#define BONES_PER_ANIMATION_LIMIT 64 // Must match shader

struct SkeletonAnimation {
	char *name;
	int firstFrame;
	int frameCount; 
	/// Unserialized
	float frameRate;
	float length;
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

	float length;
};

#define BONES_MAX 32
struct BaseSkeleton {
	char version;
	char name[PATH_MAX_LEN];
	int frameCount;

	Bone *bones;
	int bonesNum;

	SkeletonAnimation *animations;
	int animationsNum;

	/// Unserialized
};

void readAnimation(DataStream *stream, SkeletonAnimation *anim) {
	anim->name = readString(stream);
	anim->firstFrame = readU32(stream);
	anim->frameCount = readU32(stream);
	anim->frameRate = 60;
}

void readBone(DataStream *stream, Bone *bone, int frameCount) {
	bone->name = readString(stream);
	// logf("Bone name: %s\n", bone->name);
	bone->parent = (s8)readU8(stream);
	bone->modelSpaceMatrix = readMatrix4(stream);
	bone->localSpaceMatrix = bone->modelSpaceMatrix;
	bone->invModelSpaceMatrix = bone->modelSpaceMatrix.invert();
	bone->poseXforms = (Xform *)zalloc(sizeof(Xform) * frameCount);
	// logf("%s would read %d matrices\n", bone->name, frameCount);
	for (int i = 0; i < frameCount; i++) {
		Matrix4 matrix = readMatrix4(stream);
		bone->poseXforms[i] = toXform(matrix);
	}
}

void readBaseSkeleton(DataStream *stream, BaseSkeleton *base) {
	base->version = readU8(stream);
	char *name = readString(stream);
	strncpy(base->name, name, PATH_MAX_LEN);
	free(name);

	base->frameCount = readU32(stream);

	base->bonesNum = readU32(stream);
	base->bones = (Bone *)zalloc(sizeof(Bone) * base->bonesNum);
	for (int i = 0; i < base->bonesNum; i++) {
		Bone *bone = &base->bones[i];
		readBone(stream, bone, base->frameCount);
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

	// for (int i = 0; i < base->bonesNum; i++) {
	// 	Bone *bone = &base->bones[i];
	// 	for (int i = 0; i < base->frameCount; i++) {
	// 		if (bone->parent < 0) continue;
	// 		Bone *parent = &base->bones[bone->parent];
	// 		bone->poseXforms[i] = toXform(parent->invModelSpaceMatrix * toMatrix(bone->poseXforms[i]));
	// 	}
	// }
}

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

	int prevAnimationFrame;
	int animationFrame;
	int justStartedAnimationFrame;

	float noAnimTime;
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
};

SkeletonSystem *skeletonSys = NULL;

void initSkeleton();
BaseSkeleton *getBaseSkeleton(char *path);
void readAnimation(DataStream *stream, SkeletonAnimation *anim);
void readBone(DataStream *stream, Bone *bone, int frameCount);
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
bool skeletonBlendJustPlayedFrame(SkeletonBlend *blend, int frameIndex);
void destroySkeleton(Skeleton *skeleton);

/// FUNCTIONS ^

void initSkeleton() {
	skeletonSys = (SkeletonSystem *)zalloc(sizeof(SkeletonSystem));
}

BaseSkeleton *getBaseSkeleton(char *path) {
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
		if (strchr(animName, '_')) animName = strchr(animName, '_')+1;

		if (streq(animName, name)) return i;
	}

	logf("Animation %s doesn't exist\n", name);
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
	for (int i = 0; i < skeleton->blendsNum; i++) {
		SkeletonBlend *blend = &skeleton->blends[i];
		if (blend->type == SKELETON_BLEND_ANIMATION) {
			SkeletonAnimation *anim = blend->animation;
			if (!anim) {
				if (blend->noAnimTime > 0) logf("Skeleton blend %s has no animation\n", blend->name);
				blend->noAnimTime += elapsed;
				continue;
			}
			blend->noAnimTime = 0;

			if (blend->playing) blend->time += elapsed*blend->timeScale;

			float framesIn = blend->time / (1.0/anim->frameRate);
			float nextFramesIn = framesIn+1;
			if (blend->loops) {
				while (framesIn >= anim->frameCount) framesIn -= anim->frameCount;
				while (nextFramesIn >= anim->frameCount) nextFramesIn -= anim->frameCount;
				framesIn += anim->firstFrame;
				nextFramesIn += anim->firstFrame;
			} else {
				if (framesIn >= anim->frameCount) framesIn = anim->frameCount;
				if (nextFramesIn >= anim->frameCount) nextFramesIn = anim->frameCount;
				framesIn += anim->firstFrame;
				nextFramesIn += anim->firstFrame;
			}
			int lowerFrame = (int)framesIn;
			int upperFrame = (int)nextFramesIn;
			// framesIn = 74;
			// nextFramesIn = 74;
			// lowerFrame = 74;
			// upperFrame = 74;
			float framePerc = framesIn - lowerFrame;
			// logf("%f %f %d %d\n", framesIn, nextFramesIn, anim->firstFrame, anim->frameCount);

			blend->justStartedAnimationFrame = -1;
			int relativeFramesIn = framesIn - anim->firstFrame;
			if (relativeFramesIn != blend->animationFrame) {
				blend->prevAnimationFrame = blend->animationFrame;
				blend->animationFrame = relativeFramesIn;
				blend->justStartedAnimationFrame = blend->animationFrame;
			}

			for (int i = 0; i < skeleton->base->bonesNum; i++) {
				blend->controlMask[i] = true;

				Bone *bone = &skeleton->base->bones[i];
				int boneIndex = i;

				Xform lowerXform = bone->poseXforms[lowerFrame];
				Xform upperXform = bone->poseXforms[upperFrame];
				//@todo Lerp toggle
				// blend->poseXforms[boneIndex] = lerp(lowerXform, upperXform, framePerc);
				blend->poseXforms[boneIndex] = lowerXform;
			}
		} else if (blend->type == SKELETON_BLEND_MANUAL_BONES) {
		} else {
			logf("Unknown blend type\n");
		}
	}

	const int POSE_XFORM_MAX = 128;
	Xform poseXforms[POSE_XFORM_MAX];
	if (skeleton->base->bonesNum > POSE_XFORM_MAX) {
		logf("Too many pose xform\n");
		return;
	}
	for (int i = 0; i < skeleton->base->bonesNum; i++) {
		poseXforms[i] = newXform();
	}

	for (int i = 0; i < skeleton->blendsNum; i++) {
		SkeletonBlend *blend = &skeleton->blends[i];
		for (int i = 0; i < skeleton->base->bonesNum; i++) {
			if (!blend->controlMask[i]) continue;
			Xform xform = blend->poseXforms[i];
			poseXforms[i] = multiplyXforms(poseXforms[i], lerp(newXform(), xform, blend->weight));
		}
	}

	for (int i = 0; i < skeleton->base->bonesNum; i++) {
		skeleton->meshTransforms[i] = mat4();
	}

	Matrix4 currentTransforms[BONES_MAX];

	for (int i = 0; i < skeleton->base->bonesNum; i++) {
		Bone *bone = &skeleton->base->bones[i];

		Matrix4 mat = mat4();
		if (bone->parent >= 0) {
			mat = currentTransforms[bone->parent] * mat;
		}

		mat = mat * toMatrix(poseXforms[i]);

		currentTransforms[i] = mat;

		mat = mat * bone->invModelSpaceMatrix;

		skeleton->meshTransforms[i] = mat;
	}

	skeleton->time += elapsed;
}

bool skeletonIsPlayingAnimation(Skeleton *skeleton, char *blendName, char *animName);
bool skeletonIsPlayingAnimation(Skeleton *skeleton, char *blendName, char *animName) {
	SkeletonBlend *blend = getSkeletonBlend(skeleton, blendName);
	if (!blend) return false;
	if (streq(blend->animation->name, animName)) return true;
	return false;
}

bool skeletonBlendJustPlayedFrame(Skeleton *skeleton, char *blendName, char *animName, int frameIndex);
bool skeletonBlendJustPlayedFrame(Skeleton *skeleton, char *blendName, char *animName, int frameIndex) {
	SkeletonBlend *blend = getSkeletonBlend(skeleton, blendName);
	if (!blend) return false;
	if (!strstr(blend->animation->name, animName)) return false;
	return skeletonBlendJustPlayedFrame(blend, frameIndex);
}

bool skeletonBlendJustPlayedFrame(SkeletonBlend *blend, int frameIndex) {
	if (blend->justStartedAnimationFrame == -1) return false;
	if (blend->justStartedAnimationFrame == frameIndex) return true;

	int prev = blend->prevAnimationFrame;
	int cur = blend->animationFrame;
	if (prev > cur) {
		int temp = prev;
		prev = cur;
		cur = prev;
	}
	if (prev < frameIndex && cur > frameIndex) return true;

	return false;
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
