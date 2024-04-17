#ifndef PI
#define PI M_PI
#endif

#include "spine/spine.h"
#include "spine/Animation.c"
#include "spine/AnimationState.c"
#include "spine/AnimationStateData.c"
#include "spine/Array.c"
#include "spine/Atlas.c"
#include "spine/AtlasAttachmentLoader.c"
#include "spine/Attachment.c"
#include "spine/AttachmentLoader.c"
#include "spine/Bone.c"
#include "spine/BoneData.c"
#include "spine/BoundingBoxAttachment.c"
#include "spine/ClippingAttachment.c"
#include "spine/Color.c"
#include "spine/Event.c"
#include "spine/EventData.c"
#include "spine/extension.c"
#include "spine/IkConstraint.c"
#include "spine/IkConstraintData.c"
#include "spine/Json.c"
#include "spine/MeshAttachment.c"
#include "spine/PathAttachment.c"
#include "spine/PathConstraint.c"
#include "spine/PathConstraintData.c"
#include "spine/PointAttachment.c"
#include "spine/RegionAttachment.c"
#include "spine/Skeleton.c"
// #include "spine/SkeletonBinary.c"
#include "spine/SkeletonBounds.c"
#include "spine/SkeletonClipping.c"
#include "spine/SkeletonData.c"
#include "spine/SkeletonJson.c"
#include "spine/Skin.c"
#include "spine/Slot.c"
#include "spine/SlotData.c"
#include "spine/TransformConstraint.c"
#include "spine/TransformConstraintData.c"
#include "spine/Triangulator.c"
#include "spine/VertexAttachment.c"
#include "spine/VertexEffect.c"

#define USING_SPINE_3_8_75 1


extern "C" char *_spUtil_readFile(const char *path, int *length) {
	if (!fileExists(path)) return NULL;

	char *data = (char *)readFile(path, length);
	return data;
}

extern "C" void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
	Texture *texture = createTexture(path);
	if (!texture) {
		logf("Spine failed to load texture at %s\n", path);
		return;
	}

	self->rendererObject = texture;
	self->width = texture->width;
	self->height = texture->height;
}

extern "C" void _spAtlasPage_disposeTexture(spAtlasPage *self) {
	if (!self->rendererObject) return;

	Texture *texture = (Texture *)self->rendererObject;
	destroyTexture(texture);
}

struct SpineBaseSkeleton {
	spAtlas *atlas;
	spSkeletonData *skeletonData;
	spAnimationStateData *animationStateData;
};

enum BoneEffectType {
	BONE_EFFECT_NONE,
	BONE_EFFECT_EASE_START,
	BONE_EFFECT_EASE_END,
	BONE_EFFECT_SPRING_START,
	BONE_EFFECT_SPRING_END,
};
char *boneEffectTypeStrings[] = {
	"None",
	"Ease start",
	"Ease end",
	"Spring start",
	"Spring end",
};
struct BoneData {
#define SPINE_BONE_NAME_MAX_LEN 64
	char path[SPINE_BONE_NAME_MAX_LEN];
	BoneEffectType boneEffect;

	float easeAmount;
	float tension;
	float damping;

	/// Unserialized
	Vec2 prevWorldStart;
	Vec2 prevWorldEnd;
	float prevWorldRotation;
	Vec2 velo;
};
struct SpineSkeleton {
	bool disableEffects;

	spSkeleton *spSkeletonInstance;
	spAnimationState *animationState;
	spSkeletonClipping *clipper;

#define SPINE_BONES_MAX 128
	BoneData boneDatas[SPINE_BONES_MAX];
	int boneDatasNum;
};

struct SpineSystem {
// #define SPINE_EXISTING_SKELETONS_MAX 32
// 	SpineSkeleton *existingSkeletons[SPINE_EXISTING_SKELETONS_MAX];
// 	int existingSkeletonsNum;
};

SpineSystem *spineSys = NULL;

SpineBaseSkeleton *loadSpineBaseSkeleton(char *path);
SpineSkeleton *deriveSkeleton(SpineBaseSkeleton *base);

void updateSkeletonAnimation(SpineSkeleton *skeleton, float elapsed);
void updateSkeletonPhysics(SpineSkeleton *skeleton, float elapsed);

char *getBonePath(spBone *bone); // Only exists for one frame
spBone *getBoneByPath(SpineSkeleton *skeleton, char *bonePath);
bool boneExists(SpineSkeleton *skeleton, char *bonePath);
Matrix3 getBoneMatrix(SpineSkeleton *skeleton, char *bonePath);
Line2 getBoneLine(SpineSkeleton *skeleton, char *bonePath);
float getBoneLength(SpineSkeleton *skeleton, char *bonePath);

void destroyBaseSkeleton(SpineBaseSkeleton *base);
void destroySkeleton(SpineSkeleton *skeleton);

void makeSureSpineGlobalVerticesIsBigEnough(int size);
void addSpineVertex(float x, float y, float u, float v, int index);
Rect drawAttachment(SpineSkeleton *skeleton, spSlot *slot, spAttachment *attachment, Matrix3 preBoneMatrix=mat3(), Matrix3 postBoneMatrix=mat3(), float alpha=1);
spTrackEntry *getTrackEntry(SpineSkeleton *skeleton, int trackIndex);
void mySpRegionAttachment_computeWorldVertices(spRegionAttachment *self, spBone *bone, float *vertices, Matrix3 preBoneMatrix);
/// FUNCTIONS^

SpineBaseSkeleton *loadSpineBaseSkeleton(char *path) {
	SpineBaseSkeleton *base = (SpineBaseSkeleton *)zalloc(sizeof(SpineBaseSkeleton));

	spBone_setYDown(1);

	char *dir = frameStringClone(path);
	char *lastSlash = strrchr(dir, '/');
	if (!lastSlash) {
		logf("Malformed path: %s\n", dir);
		return NULL;
	}
	*lastSlash = 0;

	int atlasLen;
	void *atlasData = readFile(frameSprintf("%s.atlas", path), &atlasLen);
	if (!atlasData) return NULL;
	base->atlas = spAtlas_create((char *)atlasData, atlasLen, dir, NULL);
	free(atlasData);
	if (!base->atlas) {
		logf("Failed to load atlas\n");
		return NULL;
	}

	spSkeletonJson *json = spSkeletonJson_create(base->atlas);
	int jsonLen = 0;
	void *jsonData = readFile(frameSprintf("%s.json", path), &jsonLen);
	base->skeletonData = spSkeletonJson_readSkeletonData(json, (char *)jsonData);
	free(jsonData);
	if (!base->skeletonData) {
		logf("Failed to load skeleton data (json)\n");
		return NULL;
	}
	spSkeletonJson_dispose(json);

	base->animationStateData = spAnimationStateData_create(base->skeletonData);

	return base;
}

SpineSkeleton *deriveSkeleton(SpineBaseSkeleton *base) {
	SpineSkeleton *skeleton = (SpineSkeleton *)zalloc(sizeof(SpineSkeleton));

	skeleton->spSkeletonInstance = spSkeleton_create(base->skeletonData);
	skeleton->animationState = spAnimationState_create(base->animationStateData);
	skeleton->animationState->userData = skeleton;
	skeleton->clipper = spSkeletonClipping_create();

	// skeleton->boneDatas = (BoneData *)zalloc(sizeof(BoneData) * skeleton->spSkeletonInstance->bonesCount);
	// skeleton->boneDatasNum = skeleton->spSkeletonInstance->bonesCount;

	// for (int i = 0; i < skeleton->spSkeletonInstance->bonesCount; i++) {
	// 	BoneData *data = &skeleton->boneDatas[i];
	// 	data->easeAmount = 0.5;
	// 	data->tension = 10;
	// 	data->damping = 0.5;
	// }

	return skeleton;
}

void updateSkeletonAnimation(SpineSkeleton *skeleton, float elapsed) {
	spSkeleton_setBonesToSetupPose(skeleton->spSkeletonInstance);
	spAnimationState_update(skeleton->animationState, elapsed);
	spAnimationState_apply(skeleton->animationState, skeleton->spSkeletonInstance);

	spSkeleton_updateWorldTransform(skeleton->spSkeletonInstance);
}

void updateSkeletonPhysics(SpineSkeleton *skeleton, float elapsed) {
	// spBone *root = spSkeleton_findBone(skeleton->spSkeletonInstance, "root");
	// if (root->scaleY > 0) root->scaleY *= -1;
	spSkeleton_updateWorldTransform(skeleton->spSkeletonInstance);

	if (elapsed) {
		for (int i = 0; i < skeleton->spSkeletonInstance->bonesCount; i++) {
			if (skeleton->disableEffects) break;
			spBone *bone = skeleton->spSkeletonInstance->bones[i];
			char *bonePath = getBonePath(bone);

			BoneData *data = &skeleton->boneDatas[i];
			for (int i = 0; i < skeleton->boneDatasNum; i++) {
				BoneData *possibleData = &skeleton->boneDatas[i];
				if (streq(possibleData->path, bonePath)) {
					data = possibleData;
					break;
				}
			}
			if (!data) continue;

			if (data->boneEffect == BONE_EFFECT_EASE_START) {
				Vec2 currentWorldStart = getPosition(getBoneMatrix(skeleton, bonePath));
				data->prevWorldStart = lerp(data->prevWorldStart, currentWorldStart, data->easeAmount);

				spBone_worldToLocal(bone->parent, data->prevWorldStart.x, data->prevWorldStart.y, &bone->x, &bone->y);
			} else if (data->boneEffect == BONE_EFFECT_EASE_END) {
				Line2 worldLine = getBoneLine(skeleton, bonePath);

				data->prevWorldEnd = lerp(data->prevWorldEnd, worldLine.end, data->easeAmount);
				Vec2 angle = vectorBetween(data->prevWorldEnd, worldLine.start);

				bone->rotation = spBone_worldToLocalRotation(bone, toDeg(angle));
			} else if (data->boneEffect == BONE_EFFECT_SPRING_START) {
				float mass = 1;
				Vec2 currentWorldStart = getPosition(getBoneMatrix(skeleton, bonePath));

				Vec2 force = data->tension * (currentWorldStart - data->prevWorldStart);
				// force += v2(0, 0.98); // Gravity
				Vec2 accel = force / mass;
				data->velo += accel * data->damping;
				data->prevWorldStart += (data->velo + force) * elapsed;

				spBone_worldToLocal(bone->parent, data->prevWorldStart.x, data->prevWorldStart.y, &bone->x, &bone->y);
			} else if (data->boneEffect == BONE_EFFECT_SPRING_END) {
				float mass = 1;
				Line2 worldLine = getBoneLine(skeleton, bonePath);

				Vec2 force = data->tension * (worldLine.end - data->prevWorldEnd);
				// force += v2(0, 0.98); // Gravity
				Vec2 accel = force / mass;
				data->velo += accel * data->damping;
				data->prevWorldEnd += (data->velo + force) * elapsed;

				Vec2 angle = vectorBetween(data->prevWorldEnd, worldLine.start);

				bone->rotation = spBone_worldToLocalRotation(bone, toDeg(angle));
			}
		}

		spSkeleton_updateWorldTransform(skeleton->spSkeletonInstance);
	}
}

char *getBonePath(spBone *bone) {
	char *str = (char *)bone->data->name;
	if (bone->parent) {
		str = frameSprintf("%s.%s", getBonePath(bone->parent), str);
	}
	return str;
}

spBone *getBoneByPath(SpineSkeleton *skeleton, char *bonePath) {
	for (int i = 0; i < skeleton->spSkeletonInstance->bonesCount; i++) {
		spBone *bone = skeleton->spSkeletonInstance->bones[i];
		if (streq(getBonePath(bone), bonePath)) return bone;
	}
	return NULL;
}

bool boneExists(SpineSkeleton *skeleton, char *bonePath) {
	spBone *bone = getBoneByPath(skeleton, bonePath);
	if (bone) return true;
	return false;
}

Matrix3 getBoneMatrix(SpineSkeleton *skeleton, char *bonePath) {
	spBone *bone = getBoneByPath(skeleton, bonePath);

	if (!bone) {
		logf("No bone called %s\n", bonePath);
		return mat3();
	}

	Matrix3 matrix = mat3();
	matrix.data[0] = bone->a;
	matrix.data[1] = bone->c;
	matrix.data[2] = 0;
	matrix.data[3] = bone->b;
	matrix.data[4] = bone->d;
	matrix.data[5] = 0;
	matrix.data[6] = bone->worldX;
	matrix.data[7] = bone->worldY;
	matrix.data[8] = 1;
	return matrix;
}

Line2 getBoneLine(SpineSkeleton *skeleton, char *bonePath) {
	spBone *bone = getBoneByPath(skeleton, bonePath);
	if (!bone) {
		logf("No bone called %s\n", bonePath);
		return makeLine2();
	}

	Matrix3 matrix = getBoneMatrix(skeleton, bonePath);
	Line2 line;
	line.start = matrix * v2();
	line.end = matrix * v2(bone->data->length, 0);

	return line;
}

float getBoneLength(SpineSkeleton *skeleton, char *bonePath) {
	spBone *bone = getBoneByPath(skeleton, bonePath);
	if (!bone) {
		logf("No bone called %s\n", bonePath);
		return 0;
	}
	return bone->data->length;
}

void destroySkeleton(SpineSkeleton *skeleton) {
	spSkeleton_dispose(skeleton->spSkeletonInstance);
	spAnimationState_dispose(skeleton->animationState);
	spSkeletonClipping_dispose(skeleton->clipper);
	free(skeleton);
}

void destroyBaseSkeleton(SpineBaseSkeleton *base) {
	spAtlas_dispose(base->atlas);
	spSkeletonData_dispose(base->skeletonData);
	spAnimationStateData_dispose(base->animationStateData);
	free(base);
}

/// Drawing
float *globalSpineWorldVerticesPositions = NULL;
// u16 *globalSpineIndices = NULL;
int globalSpineVerticesMax = 0;

float *globalSpinePositions;
float *globalSpineUvs;

void makeSureSpineGlobalVerticesIsBigEnough(int size) {
	size += 2;

	if (globalSpineVerticesMax < size) {
		if (globalSpineWorldVerticesPositions) free(globalSpineWorldVerticesPositions);
		if (globalSpinePositions) free(globalSpinePositions);
		if (globalSpineUvs) free(globalSpineUvs);

		globalSpineVerticesMax = size;
		globalSpineWorldVerticesPositions = (float *)zalloc(sizeof(float) * globalSpineVerticesMax * 2);
		globalSpinePositions = (float *)zalloc(sizeof(float) * globalSpineVerticesMax * 2);
		globalSpineUvs = (float *)zalloc(sizeof(float) * globalSpineVerticesMax * 2);
	}
}

void addSpineVertex(float x, float y, float u, float v, int index) {
	if (index > globalSpineVerticesMax-1) Panic("Overflowed globalSpineVerticesMax");

	globalSpinePositions[index*2 + 0] = x;
	globalSpinePositions[index*2 + 1] = y;
	globalSpineUvs[index*2 + 0] = u;
	globalSpineUvs[index*2 + 1] = v;
}

Rect drawAttachment(SpineSkeleton *skeleton, spSlot *slot, spAttachment *attachment, Matrix3 preBoneMatrix, Matrix3 postBoneMatrix, float alpha) {
	Texture *texture = 0;
	int vertexIndex = 0;
	Rect rect = {};
	if (attachment->type == SP_ATTACHMENT_REGION) {
		spRegionAttachment *regionAttachment = (spRegionAttachment *)attachment;

		texture = (Texture *)((spAtlasRegion *)regionAttachment->rendererObject)->page->rendererObject;

		makeSureSpineGlobalVerticesIsBigEnough(4);
		// mySpRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, globalSpineWorldVerticesPositions, preBoneMatrix);
		spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, globalSpineWorldVerticesPositions, 0, 2);

		float *verts = globalSpineWorldVerticesPositions;
		addSpineVertex(verts[0], verts[1], regionAttachment->uvs[0], regionAttachment->uvs[1], vertexIndex++);
		addSpineVertex(verts[2], verts[3], regionAttachment->uvs[2], regionAttachment->uvs[3], vertexIndex++);
		addSpineVertex(verts[4], verts[5], regionAttachment->uvs[4], regionAttachment->uvs[5], vertexIndex++);
		addSpineVertex(verts[4], verts[5], regionAttachment->uvs[4], regionAttachment->uvs[5], vertexIndex++);
		addSpineVertex(verts[6], verts[7], regionAttachment->uvs[6], regionAttachment->uvs[7], vertexIndex++);
		addSpineVertex(verts[0], verts[1], regionAttachment->uvs[0], regionAttachment->uvs[1], vertexIndex++);

		rect = insert(rect, v2(verts[0], verts[1]));
		rect = insert(rect, v2(verts[2], verts[3]));
		rect = insert(rect, v2(verts[4], verts[5]));
		rect = insert(rect, v2(verts[4], verts[5]));
		rect = insert(rect, v2(verts[6], verts[7]));
		rect = insert(rect, v2(verts[0], verts[1])); // Redundant??
	} else if (attachment->type == SP_ATTACHMENT_MESH) {
		spMeshAttachment *mesh = (spMeshAttachment *)attachment;

		texture = (Texture *)((spAtlasRegion *)mesh->rendererObject)->page->rendererObject;

		makeSureSpineGlobalVerticesIsBigEnough(mesh->trianglesCount * 3);
		spVertexAttachment_computeWorldVertices(&mesh->super, slot, 0, mesh->super.worldVerticesLength, globalSpineWorldVerticesPositions, 0, 2);

		for (int i = 0; i < mesh->trianglesCount; i++) {
			int index = mesh->triangles[i] * 2;
			Vec2 vert = v2(globalSpineWorldVerticesPositions[index], globalSpineWorldVerticesPositions[index + 1]);
			addSpineVertex(vert.x, vert.y, mesh->uvs[index], mesh->uvs[index + 1], vertexIndex++);
		}
	} else if (attachment->type == SP_ATTACHMENT_CLIPPING) {
		spClippingAttachment *clip = (spClippingAttachment *) slot->attachment;
		spSkeletonClipping_clipStart(skeleton->clipper, slot, clip);
	}

	int indsNum = vertexIndex;
	u16 *inds = (u16 *)frameMalloc(sizeof(u16) * indsNum);
	for (int i = 0; i < indsNum; i++) {
		inds[i] = i;
	}

	Vec2 *finalPositions = (Vec2 *)globalSpinePositions;
	Vec2 *finalUvs = (Vec2 *)globalSpineUvs;

	if (spSkeletonClipping_isClipping(skeleton->clipper)) {
		spSkeletonClipping_clipTriangles(skeleton->clipper, globalSpinePositions, vertexIndex * 2, inds, indsNum, globalSpineUvs, 2);
		finalPositions = (Vec2 *)skeleton->clipper->clippedVertices->items;
		finalUvs = (Vec2 *)skeleton->clipper->clippedUVs->items;
		inds = skeleton->clipper->clippedTriangles->items;
		indsNum = skeleton->clipper->clippedTriangles->size;
	}

	Matrix3 finalMatrix = postBoneMatrix;

	if (!texture) return makeRect();

	Vec2 *linearVerts = (Vec2 *)frameMalloc(sizeof(Vec2) * indsNum);
	int linearVertsNum = 0;
	Vec2 *linearUvs = (Vec2 *)frameMalloc(sizeof(Vec2) * indsNum);
	int *linearColors = (int *)frameMalloc(sizeof(int) * indsNum);

	Matrix3 matrix = renderer->baseMatrix2d * finalMatrix;

	int color = setAofArgb(0xFFFFFFFF, alpha*255.0);
	for (int i = 0; i < indsNum; i++) {
		int ind = inds[i];
		Vec2 pos = finalPositions[ind];
		Vec2 uv = finalUvs[ind];

		uv.y = 1 - uv.y;

		linearVerts[linearVertsNum] = pos;
		linearUvs[linearVertsNum] = uv;
		linearColors[linearVertsNum] = color;
		linearVertsNum++;
	}
	drawVerts(linearVerts, linearUvs, linearColors, linearVertsNum, texture);
	spSkeletonClipping_clipEnd(skeleton->clipper, slot);

	return rect;
}

spTrackEntry *getTrackEntry(SpineSkeleton *skeleton, int trackIndex) {
	if (trackIndex > skeleton->animationState->tracksCount-1) return NULL;
	spTrackEntry *entry = skeleton->animationState->tracks[trackIndex];
	return entry;
}

void mySpRegionAttachment_computeWorldVertices(spRegionAttachment *self, spBone *bone, float *vertices, Matrix3 preBoneMatrix) {
	float *offset = self->offset;

	float a = bone->a;
	float b = bone->b;
	float c = bone->c;
	float d = bone->d;
	float x = bone->skeleton->x + bone->worldX;
	float y = bone->skeleton->y + bone->worldY;

	Matrix3 matrix;
	matrix.data[0] = a;
	matrix.data[1] = c;
	matrix.data[2] = 0;
	matrix.data[3] = b;
	matrix.data[4] = d;
	matrix.data[5] = 0;
	matrix.data[6] = x;
	matrix.data[7] = y;
	matrix.data[8] = 1;

	matrix *= preBoneMatrix;

	a = matrix.data[0];
	c = matrix.data[1];
	b = matrix.data[3];
	d = matrix.data[4];
	x = matrix.data[6];
	y = matrix.data[7];

	vertices[0] = offset[0] * a + offset[1] * b + x;
	vertices[1] = offset[0] * c + offset[1] * d + y;
	vertices[2] = offset[2] * a + offset[3] * b + x;
	vertices[3] = offset[2] * c + offset[3] * d + y;
	vertices[4] = offset[4] * a + offset[5] * b + x;
	vertices[5] = offset[4] * c + offset[5] * d + y;
	vertices[6] = offset[6] * a + offset[7] * b + x;
	vertices[7] = offset[6] * c + offset[7] * d + y;
}
