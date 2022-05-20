#define MULTITHREAD_SPRITE_SHEETS 0
#define TRIM_SPRITE_SHEETS 1

// #define SHEET_WIDTH_LIMIT 8192
// #define SHEET_HEIGHT_LIMIT 8192
#define SHEET_WIDTH_LIMIT 4096
#define SHEET_HEIGHT_LIMIT 4096

#define ANIM_NAME_LIMIT 256
#define ANIM_FRAME_LIMIT 256
struct Frame {
#define FRAME_NAME_MAX_LEN 64
	char name[FRAME_NAME_MAX_LEN];
	int textureNumber;

	int srcX;
	int srcY;
	int srcWidth;
	int srcHeight;

	int destOffX;
	int destOffY;
	int width;
	int height;

/// Unserialized
	RenderTexture *texture;
	int indexInAnim;
};

struct Animation {
	char name[ANIM_NAME_LIMIT];
	Frame *frames[ANIM_FRAME_LIMIT];
	int framesNum;
	bool loops;
	bool endsAtOne;
};

struct AnimationSystem {
	RenderTexture *garbageTexture;
	float frameRate;
	bool loopsByDefault;

#define FRAMES_PER_SHEET_LIMIT 32768
	Frame frames[FRAMES_PER_SHEET_LIMIT];
	int framesNum;

#define ANIMS_LIMIT 2048
	Animation anims[ANIMS_LIMIT];
	int animsNum;

	Allocator animsMapAllocator;
	HashMap *animsMap;

#define SPRITE_SHEETS_MAX 1024
	unsigned char *bitmapDatas[SPRITE_SHEETS_MAX];
	int bitmapDatasNum;

	RenderTexture *sheetTextures[SPRITE_SHEETS_MAX];
	int sheetTexturesNum;

	struct Buffer {
		void *data;
		int num;

		int index;
		int width;
		int height;

		Frame *frame;
		char path[PATH_MAX_LEN];
	};

	ThreadSafeQueue *spritePathsQueue;
	ThreadSafeQueue *spriteBitmapsOut;
};

AnimationSystem *animSys = NULL;

void initAnimations();
void packSpriteSheet(const char *dirName);
void regenerateSheetAnimations();
Animation *getAnimation(const char *animName);
Frame *getFrame(const char *frameName);
Frame *getAnimFrameAtSecond(Animation *anim, float time);
Frame *getAnimFrameAtPercent(Animation *anim, float percent);

void spriteLoadThread(void *threadStruct);

/// FUNCTIONS ^

void initAnimations() {
	animSys = (AnimationSystem *)zalloc(sizeof(AnimationSystem));
	animSys->frameRate = 10.0;

	animSys->garbageTexture = createRenderTexture(1, 1);
}

void packSpriteSheet(const char *dirName) {
	logf("Packing sprite sheets...\n");

	if (!animSys) initAnimations();

	NanoTime nano = getNanoTime();

	{
		for (int i = 0; i < animSys->sheetTexturesNum; i++) destroyTexture(animSys->sheetTextures[i]);
		animSys->sheetTexturesNum = 0;

		animSys->bitmapDatasNum = 0;
		animSys->framesNum = 0;
	}

	int padding = 2;

	int pathsNum;
	char **paths = getDirectoryList(dirName, &pathsNum);  //@incomplete People take references to stuff in here, because it doesn't get freed?

	if (pathsNum > FRAMES_PER_SHEET_LIMIT) {
		logf("Too many frames for a sheet\n");
		Assert(0);
	}

	u8 **images = (u8 **)frameMalloc(FRAMES_PER_SHEET_LIMIT * sizeof(u8 *));
	int imagesNum = 0;

	stbrp_rect *rects = (stbrp_rect *)frameMalloc(FRAMES_PER_SHEET_LIMIT * sizeof(stbrp_rect));
	int rectsNum = 0;
	stbrp_rect *rectsLeft = (stbrp_rect *)frameMalloc(FRAMES_PER_SHEET_LIMIT * sizeof(stbrp_rect));
	int rectsLeftNum = 0;

	animSys->spritePathsQueue = createThreadSafeQueue(sizeof(AnimationSystem::Buffer), pathsNum);
	animSys->spriteBitmapsOut = createThreadSafeQueue(sizeof(AnimationSystem::Buffer), pathsNum);

	char *targetInfoPaths[ANIMS_LIMIT];
	int targetInfoPathsNum = 0;
	for (int i = 0; i < pathsNum; i++) {
		char *path = paths[i];
		if (strstr(path, ".txt")) {
			targetInfoPaths[targetInfoPathsNum++] = path;
		}

		if (!strstr(path, ".png")) continue;

		Frame *frame = &animSys->frames[animSys->framesNum++];
		memset(frame, 0, sizeof(Frame));
		// if (prefix) {
		// 	snprintf(frame->name, FRAME_NAME_MAX_LEN, "%s/%s", prefix, path + strlen(dirName) + 1);
		// } else {
		strcpy(frame->name, path + strlen(dirName) + 1);
		// }

		AnimationSystem::Buffer buf = {};
		strncpy(buf.path, path, PATH_MAX_LEN);
		buf.frame = frame;
		buf.index = animSys->framesNum-1;
		if (!threadSafeQueuePush(animSys->spritePathsQueue, &buf)) logf("Failed to store image path (%d/%d)\n", i, pathsNum);
	}

	// logf("Time to init: %f\n", getMsPassed(nano)); nano = getNanoTime();

#if MULTITHREAD_SPRITE_SHEETS
	Thread *threads[8];
	int threadsNum = 0;
	for (int i = 0; i < 8; i++) threads[threadsNum++] = createThread(spriteLoadThread);
	for (int i = 0; i < threadsNum; i++) joinThread(threads[i]);
#else
	spriteLoadThread(NULL);
#endif

	// logf("Time to load sprites: %f\n", getMsPassed(nano)); nano = getNanoTime();

	for (;;) {
		AnimationSystem::Buffer buf = {};
		if (!threadSafeQueueShift(animSys->spriteBitmapsOut, &buf)) break;

		Frame *frame = buf.frame;
		int frameIndex = buf.index;

		int width = buf.width;
		int height = buf.height;
		images[frameIndex] = (u8 *)buf.data;

		stbrp_rect *rpRect = &rects[rectsNum++];
		rpRect->id = frameIndex;
		rpRect->w = frame->width + padding*2;
		rpRect->h = frame->height + padding*2;
	}

	for (int i = 0; ; i++) {
		stbrp_context context;
		stbrp_node nodes[FRAMES_PER_SHEET_LIMIT] = {};
		stbrp_init_target(&context, SHEET_WIDTH_LIMIT-1, SHEET_HEIGHT_LIMIT-1, nodes, FRAMES_PER_SHEET_LIMIT);

		bool didAll = stbrp_pack_rects(&context, rects, rectsNum);

		unsigned char *bitmapData = (unsigned char *)malloc(SHEET_WIDTH_LIMIT * SHEET_HEIGHT_LIMIT * 4);
		memset(bitmapData, 0, SHEET_WIDTH_LIMIT * SHEET_HEIGHT_LIMIT * 4);

		for (int i = 0; i < rectsNum; i++) {
			stbrp_rect *rpRect = &rects[i];
			if (!rpRect->was_packed) {
				rectsLeft[rectsLeftNum++] = *rpRect;
				continue;
			}

			Frame *frame = &animSys->frames[rpRect->id];
			unsigned char *image = images[rpRect->id];

			int startX = 0;
			int startY = 0;
			int endX = frame->width;
			int endY = frame->height;

			frame->srcX = rpRect->x + startX + padding;
			frame->srcY = rpRect->y + startY + padding;

			for (int y = startY; y < endY; y++) {
				for (int x = startX; x < endX; x++) {
					unsigned char *readStart = &image[((y+frame->destOffY)*frame->srcWidth + (x+frame->destOffX))*4];
					unsigned char *writeStart = &bitmapData[((rpRect->y+padding+y)*SHEET_WIDTH_LIMIT + rpRect->x+padding+x)*4];

					int pixel = *(int *)readStart;

					*(int *)writeStart = pixel;
					// memcpy(writeStart, readStart, 4);
				}
			}

			free(image);
		}

		RenderTexture *texture = createRenderTexture(SHEET_WIDTH_LIMIT, SHEET_HEIGHT_LIMIT, NULL); // This used to srgb to linear?
		setTextureData(texture, bitmapData, SHEET_WIDTH_LIMIT, SHEET_HEIGHT_LIMIT, _F_TD_FLIP_Y);
		animSys->sheetTextures[animSys->sheetTexturesNum++] = texture;
		for (int i = 0; i < rectsNum; i++) {
			stbrp_rect *rpRect = &rects[i];
			Frame *frame = &animSys->frames[rpRect->id];
			frame->textureNumber = animSys->sheetTexturesNum-1;
			frame->texture = texture;
		}

		animSys->bitmapDatas[animSys->bitmapDatasNum++] = bitmapData;

		if (didAll) {
			break;
		} else {
			memcpy(rects, rectsLeft, sizeof(stbrp_rect) * rectsLeftNum);
			rectsNum = rectsLeftNum;
			rectsLeftNum = 0;
		}
	}

	regenerateSheetAnimations();

	logf("Done took %fms\n", getMsPassed(nano));
}

void spriteLoadThread(void *threadStruct) {
	int maxWidth = 8192+1;
	u8 *tempRow = (u8 *)malloc(maxWidth * 4);

	int pngDataMax = Megabytes(5);
	u8 *pngData = (u8 *)malloc(pngDataMax);
	int pngDataSize = 0;

	for (;;) {
		AnimationSystem::Buffer inBuf = {};

		if (!threadSafeQueueShift(animSys->spritePathsQueue, &inBuf)) break;

		pngDataSize = 0;
		if (!readFileDirect(inBuf.path, pngData, pngDataMax, &pngDataSize)) {
			logf("Failed to load png: %s\n", inBuf.path);
			Panic("Can't continue");
		}
		// bool readFileDirect(const char *fileName, u8 *outData, int outDataSize, int *outSize);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		u8 *img = stbi_load_from_memory((unsigned char *)pngData, pngDataSize, &width, &height, &channels, 4);

		if (width >= maxWidth) logf("Max width too small %d needs to be at least %d\n", maxWidth, width);

		// {
		// 	for (int y = 0; y < height; y++) {
		// 		for (int x = 0; x < width; x++) {
		// 			int pixelIndex = (y*width+x)*4;
		// 			float a = (float)(((unsigned char *)img)[pixelIndex + 3]) / 255.0;
		// 			float r = (float)(((unsigned char *)img)[pixelIndex + 2]) / 255.0;
		// 			float g = (float)(((unsigned char *)img)[pixelIndex + 1]) / 255.0;
		// 			float b = (float)(((unsigned char *)img)[pixelIndex + 0]) / 255.0;

		// 			r = srgbToLinear(r) * a;
		// 			g = srgbToLinear(g) * a;
		// 			b = srgbToLinear(b) * a;
		// 			// a = srgbToLinear(a);

		// 			((unsigned char *)img)[pixelIndex + 3] = a * 255.0;
		// 			((unsigned char *)img)[pixelIndex + 2] = r * 255.0;
		// 			((unsigned char *)img)[pixelIndex + 1] = g * 255.0;
		// 			((unsigned char *)img)[pixelIndex + 0] = b * 255.0;
		// 		}
		// 	}
		// }

		for (int y = 0; y < height/2; y++) {
			int curTopRow = y;
			int curBottomRow = height - y - 1;
			u8 *topRowStart = img + curTopRow * width * 4;
			u8 *bottomRowStart = img + curBottomRow * width * 4;

			memcpy(tempRow, topRowStart, width * 4);
			memcpy(topRowStart, bottomRowStart, width * 4);
			memcpy(bottomRowStart, tempRow, width * 4);
		}

		Frame *frame = inBuf.frame;

#if TRIM_SPRITE_SHEETS
		int top = -1;
		int bot = 0;

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				u8 alpha = img[(y*width+x)*4 + 3];
				if (alpha > 0) {
					if (top == -1) top = y-1;
					bot = y+1;
				}
			}
		}

		int left = -1;
		int right = 0;
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				u8 alpha = img[(y*width+x)*4 + 3];
				if (alpha > 0) {
					if (left == -1) left = x-1;
					right = x+1;
				}
			}
		}

		if (top < 0) top = 0;
		if (bot > height-1) bot = height-1;

		if (left < 0) left = 0;
		if (right > width-1) right = width-1;

#if 0 // Outline
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (y != top && y != bot && x != left && x != right) continue;

				images[imagesNum-1][(y*width+x)*4 + 0] = 0;
				images[imagesNum-1][(y*width+x)*4 + 1] = 0;
				images[imagesNum-1][(y*width+x)*4 + 2] = 0;
				images[imagesNum-1][(y*width+x)*4 + 3] = 255;
			}
		}
#endif

		frame->destOffX = left;
		frame->destOffY = top;
		frame->width = right-left;
		frame->height = bot-top;
#else
		frame->width = width;
		frame->height = height;
#endif
		frame->srcWidth = width;
		frame->srcHeight = height;

		{
			AnimationSystem::Buffer outBuf = {};
			outBuf.data = img;
			outBuf.width = width;
			outBuf.height = height;
			outBuf.frame = frame;
			outBuf.index = inBuf.index;
			if (!threadSafeQueuePush(animSys->spriteBitmapsOut, &outBuf)) logf("Failed to store image data\n");
		}
	}

	free(pngData);
	free(tempRow);
}

void regenerateSheetAnimations() {
	animSys->animsNum = 0;

	if (animSys->animsMap) destroyHashMap(animSys->animsMap);
	animSys->animsMapAllocator.type = ALLOCATOR_DEFAULT;
	animSys->animsMap = createHashMap(sizeof(char) * ANIM_NAME_LIMIT, sizeof(Animation *), 128, &animSys->animsMapAllocator);

	for (int i = 0; i < animSys->framesNum; i++) {
		Frame *frame = &animSys->frames[i];

		char *nameEnd = NULL;
		if (!nameEnd) {
			int nameEndIndex = strlen(frame->name) - 5;
			while (
				nameEndIndex >= 0 &&
				((frame->name[nameEndIndex] >= '0' && frame->name[nameEndIndex] <= '9') || frame->name[nameEndIndex] == '_')
			) {
				nameEnd = &frame->name[nameEndIndex];
				if (frame->name[nameEndIndex] == '_') break;
				nameEndIndex--;
			}
		}
		if (!nameEnd) nameEnd = strchr(frame->name, '.');
		if (!nameEnd) nameEnd = frame->name + strlen(frame->name);
		int nameLen = nameEnd - frame->name;

		char frameAnimName[ANIM_NAME_LIMIT];
		strncpy(frameAnimName, frame->name, nameLen);
		frameAnimName[nameLen] = '\0';

		Animation *anim = NULL;
		for (int i = 0; i < animSys->animsNum; i++) {
			if (streq(animSys->anims[i].name, frameAnimName)) {
				anim = &animSys->anims[i];
				break;
			}
		}

		if (!anim) {
			if (animSys->animsNum > ANIMS_LIMIT-1) {
				logf("Too many animations\n");
				Panic("Cannot continue");
			}
			anim = &animSys->anims[animSys->animsNum++];
			memset(anim, 0, sizeof(Animation));
			strcpy(anim->name, frameAnimName);

			hashMapSet(animSys->animsMap, anim->name, stringHash32(anim->name), &anim);
		}

		frame->indexInAnim = anim->framesNum;
		if (anim->framesNum < 0 || anim->framesNum > 1000) {
			logf("?");
		}
		anim->frames[anim->framesNum++] = frame;
	}

	if (animSys->loopsByDefault) {
		for (int i = 0; i < animSys->animsNum; i++) {
			Animation *anim = &animSys->anims[i];
			anim->loops = true;
		}
	}
}

void saveSpriteSheets(char *dir);
void saveSpriteSheets(char *dir) {
	for (int i = 0; i < animSys->sheetTexturesNum; i++) {
		char *path = frameSprintf("%s/sheet%d.png", dir, i);

		RenderTexture *texture = animSys->sheetTextures[i];
		u8 *bitmapData = getTextureData(texture); // Alpha is purposely not unmultiplied

		stbi_flip_vertically_on_write(true);
		if (!stbi_write_png(frameSprintf("%s%s", filePathPrefix, path), texture->width, texture->height, 4, bitmapData, texture->width*4)) {
			logf("Failed to write sprite sheet: %s\n", path);
		}
	}

	DataStream *stream = newDataStream();

	writeU32(stream, 1); // Version
	writeU32(stream, animSys->sheetTexturesNum);

	writeU32(stream, animSys->framesNum);
	for (int i = 0; i < animSys->framesNum; i++) {
		Frame *frame = &animSys->frames[i];
		writeString(stream, frame->name);
		writeU32(stream, frame->textureNumber);
		writeU32(stream, frame->srcX);
		writeU32(stream, frame->srcY);
		writeU32(stream, frame->srcWidth);
		writeU32(stream, frame->srcHeight);
		writeU32(stream, frame->destOffX);
		writeU32(stream, frame->destOffY);
		writeU32(stream, frame->width);
		writeU32(stream, frame->height);
	}

	writeDataStream(frameSprintf("%s/sheetData.bin", dir), stream);
	destroyDataStream(stream);
}

void loadSpriteSheet(char *sheetDataPath);
void loadSpriteSheet(char *sheetDataPath) {
	char *dir = frameStringClone(sheetDataPath);
	*strrchr(dir, '/') = 0;

	DataStream *stream = loadDataStream(sheetDataPath);
	if (!stream) Panic("No sprite sheet data stream?\n");

	int version = readU32(stream);

	animSys->sheetTexturesNum = readU32(stream);
	for (int i = 0; i < animSys->sheetTexturesNum; i++) {
		animSys->sheetTextures[i] = createRenderTexture(frameSprintf("%s/sheet%d.png", dir, i), _F_TD_SKIP_PREMULTIPLY);
	}

	animSys->framesNum = readU32(stream);
	for (int i = 0; i < animSys->framesNum; i++) {
		Frame *frame = &animSys->frames[i];
		memset(frame, 0, sizeof(Frame));
		readStringInto(stream, frame->name, FRAME_NAME_MAX_LEN);
		frame->textureNumber = readU32(stream);
		frame->srcX = readU32(stream);
		frame->srcY = readU32(stream);
		frame->srcWidth = readU32(stream);
		frame->srcHeight = readU32(stream);
		frame->destOffX = readU32(stream);
		frame->destOffY = readU32(stream);
		frame->width = readU32(stream);
		frame->height = readU32(stream);

		frame->texture = animSys->sheetTextures[frame->textureNumber];
	}

	destroyDataStream(stream);

	regenerateSheetAnimations();
}

Animation *getAnimation(const char *animName) {
	// logf("Getting %s with %u\n", animName, stringHash32(animName));
	// bool hashMapGet(HashMap *map, void *key, int hash, void *outValue=NULL);
#if 1
	char realAnimName[ANIM_NAME_LIMIT] = {};
	strcpy(realAnimName, animName);
	Animation *anim = NULL;
	bool good = hashMapGet(animSys->animsMap, realAnimName, stringHash32(realAnimName), &anim);
	if (good) return anim;
	return NULL;
#else

	for (int i = 0; i < animSys->animsNum; i++) {
		Animation *anim = &animSys->anims[i];
		if (streq(anim->name, animName)) return anim;
	}

	return NULL;
#endif
}

Frame *getFrame(const char *frameName) {
	for (int i = 0; i < animSys->framesNum; i++) {
		Frame *frame = &animSys->frames[i];
		if (streq(frame->name, frameName)) return frame;
	}

	return NULL;
}

Frame *getAnimFrameAtPercent(Animation *anim, float percent) {
	if (!anim) Panic("Passed in NULL anim\n");

	if (percent == 0) return anim->frames[0];
	if (percent == 1) return anim->frames[anim->framesNum-1];

	int framesIn = (anim->framesNum-1) * percent;

	if (anim->loops) {
		framesIn = framesIn % anim->framesNum;
	} else {
		if (framesIn > anim->framesNum - 1) framesIn = anim->framesNum - 1;
	}

	if (anim->endsAtOne && framesIn == anim->framesNum - 1) framesIn = 0;

	return anim->frames[framesIn];
}

Frame *getAnimFrameAtSecond(Animation *anim, float time) {
	if (!anim) Panic("Passed in NULL anim\n");

	int framesIn = animSys->frameRate * time;

	if (anim->loops) {
		framesIn = framesIn % anim->framesNum;
	} else {
		if (framesIn > anim->framesNum - 1) framesIn = anim->framesNum - 1;
	}

	if (anim->endsAtOne && framesIn == anim->framesNum - 1) framesIn = 0;

	return anim->frames[framesIn];
}
