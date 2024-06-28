#define SPRITE_SHEET_WIDTH_MAX 4096
#define SPRITE_SHEET_HEIGHT_MAX 4096

struct SpriteSheetImage {
	char *name;
	int srcX;
	int srcY;
	int srcWidth;
	int srcHeight;

	int destOffX;
	int destOffY;
	int width;
	int height;
};

struct SpriteSheet {
	char path[PATH_MAX_LEN];
	Texture *texture;
#define SPRITE_SHEET_TEXTURES_MAX 32
	Texture *textures[SPRITE_SHEET_TEXTURES_MAX];
	int texturesNum;

	SpriteSheetImage *images;
	int imagesNum;
};

#define CACHED_SPRITE_SHEETS_MAX 128
SpriteSheet *_cachedSpriteSheets[CACHED_SPRITE_SHEETS_MAX];
int _cachedSpriteSheetsNum;

SpriteSheet *getSpriteSheet(char *dir);
SpriteSheet *createSpriteSheet(char *dir);
void destroySpriteSheet(SpriteSheet *sheet);
void clearSpriteSheetCache();

SpriteSheetImage *getImage(SpriteSheet *sheet, char *name);
void getUvs(SpriteSheet *sheet, SpriteSheetImage *image, Vec2 *uv0, Vec2 *uv1);
/// FUNCTIONS ^

SpriteSheet *getSpriteSheet(char *dir) {
  for (int i = 0; i < _cachedSpriteSheetsNum; i++) {
    SpriteSheet *cachedSpriteSheet = _cachedSpriteSheets[i];
    if (streq(cachedSpriteSheet->path, dir)) {
      return cachedSpriteSheet;
    }
  }

	SpriteSheet *sheet = createSpriteSheet(dir);
  _cachedSpriteSheets[_cachedSpriteSheetsNum++] = sheet;
  return sheet;
}

SpriteSheet *createSpriteSheet(char *dir) {
	if (!directoryExists(dir)) return NULL;

	SpriteSheet *sheet = (SpriteSheet *)zalloc(sizeof(SpriteSheet));
  strcpy(sheet->path, dir);

	int pathsNum;
	char **paths = getFrameDirectoryList(dir, &pathsNum);

	bool goodDir = true;
	for (int i = 0; i < pathsNum; i++) {
		char *path = paths[i];
		if (!stringEndsWith(path, ".png")) goodDir = false;
	}
	if (!goodDir) return NULL;

	sheet->images = (SpriteSheetImage *)zalloc(sizeof(SpriteSheetImage) * pathsNum);

	stbrp_rect *rects = (stbrp_rect *)frameMalloc(pathsNum * sizeof(stbrp_rect));

	u8 **srcBitmaps = (u8 **)frameMalloc(sizeof(u8 *) * pathsNum);

	for (int i = 0; i < pathsNum; i++) {
		char *path = paths[i];

		stbrp_rect *stbRect = &rects[sheet->imagesNum];
		SpriteSheetImage *image = &sheet->images[sheet->imagesNum++];

		int pngDataSize;
		u8 *pngData = (u8 *)readFile(path, &pngDataSize);

		int srcWidth, srcHeight, channels;
		stbi_set_flip_vertically_on_load(false);
		srcBitmaps[i] = stbi_load_from_memory(pngData, pngDataSize, &srcWidth, &srcHeight, &channels, 4);
		stbi_set_flip_vertically_on_load(true);
		free(pngData);

		char *imageName = frameStringClone(path);
		imageName += strlen(dir);
		if (imageName[0] == '/') imageName++;
		image->name = stringClone(imageName);

		{ // Add trimming here!
			image->srcWidth = srcWidth;
			image->srcHeight = srcHeight;

			image->destOffX = 0;
			image->destOffY = 0;
			image->width = srcWidth;
			image->height = srcHeight;
		}

		stbRect->id = i;
		stbRect->w = image->srcWidth;
		stbRect->h = image->srcHeight;
	}

	stbrp_context context;
	int nodesMax = SPRITE_SHEET_WIDTH_MAX;
	stbrp_node *nodes = (stbrp_node *)frameMalloc(sizeof(stbrp_node) * nodesMax);
	stbrp_init_target(&context, SPRITE_SHEET_WIDTH_MAX-1, SPRITE_SHEET_HEIGHT_MAX-1, nodes, nodesMax);

	if (!stbrp_pack_rects(&context, rects, sheet->imagesNum)) {
		logf("Failed to pack rect!\n");
	}

	u8 *bitmapData = (u8 *)zalloc(SPRITE_SHEET_WIDTH_MAX * SPRITE_SHEET_HEIGHT_MAX * 4);
	memset(bitmapData, 0, SPRITE_SHEET_WIDTH_MAX * SPRITE_SHEET_HEIGHT_MAX * 4);

	int padding = 2;
	for (int i = 0; i < sheet->imagesNum; i++) {
		stbrp_rect *rpRect = &rects[i];

		SpriteSheetImage *image = &sheet->images[i];

		int startX = 0;
		int startY = 0;
		int endX = image->width;
		int endY = image->height;

		image->srcX = rpRect->x + startX + padding;
		image->srcY = rpRect->y + startY + padding;

		for (int y = startY; y < endY; y++) {
			for (int x = startX; x < endX; x++) {
				u8 *readStart = &srcBitmaps[i][((y+image->destOffY)*image->srcWidth + (x+image->destOffX))*4];

				int writeX = rpRect->x + padding + x;
				int writeY = rpRect->y + padding + y;
				u8 *writeStart = &bitmapData[(writeY*SPRITE_SHEET_WIDTH_MAX + writeX)*4];

				*(int *)writeStart = *(int *)readStart;
			}
		}

		free(srcBitmaps[i]);
	}

	sheet->texture = createTexture(SPRITE_SHEET_WIDTH_MAX, SPRITE_SHEET_HEIGHT_MAX);
	setTextureData(sheet->texture, bitmapData, SPRITE_SHEET_WIDTH_MAX, SPRITE_SHEET_HEIGHT_MAX, _F_TD_FLIP_Y);
	free(bitmapData);

	return sheet;
}

void destroySpriteSheet(SpriteSheet *sheet) {
	destroyTexture(sheet->texture);

	for (int i = 0; i < sheet->imagesNum; i++) {
		SpriteSheetImage *image = &sheet->images[i];
		if (image->name) free(image->name);
	}

	free(sheet->images);
	free(sheet);
}

void clearSpriteSheetCache() {
  for (int i = 0; i < _cachedSpriteSheetsNum; i++) {
    SpriteSheet *cachedSpriteSheet = _cachedSpriteSheets[i];
    destroySpriteSheet(cachedSpriteSheet);
  }

  _cachedSpriteSheetsNum = 0;
}

SpriteSheetImage *getImage(SpriteSheet *sheet, char *name) {
	for (int i = 0; i < sheet->imagesNum; i++) {
		SpriteSheetImage *image = &sheet->images[i];
		if (streq(image->name, name)) return image;
	}
	return NULL;
}

void getUvs(SpriteSheet *sheet, SpriteSheetImage *image, Vec2 *uv0, Vec2 *uv1) {
	uv0->x = (float)image->srcX / sheet->texture->width;
	uv0->y = (float)image->srcY / sheet->texture->height;
	uv1->x = (float)(image->srcX+image->srcWidth) / sheet->texture->width;
	uv1->y = (float)(image->srcY+image->srcHeight) / sheet->texture->height;
}
