struct TextureHandle {
	Texture *texture;
	char path[PATH_MAX_LEN];
};

struct TextureSystem {
	Allocator mapAllocator;
	HashMap *map;
};

TextureSystem *textureSys = NULL;

void initTextureSystem();
void initTextureSystem() {
	textureSys = (TextureSystem *)zalloc(sizeof(TextureSystem));
}

Texture *getTexture(char *path, int flags=0);
Texture *getTexture(char *path, int flags) {
  if (!textureSys->map) {
    textureSys->mapAllocator.type = ALLOCATOR_DEFAULT;
    textureSys->map = createHashMap(sizeof(char *), sizeof(Texture *), 64, &textureSys->mapAllocator);
    textureSys->map->usesStreq = true;
  }

	if (!path) return NULL;
	{
		Texture *texture;
		void *pathAsVoid = path;
		void *pathPtr = &pathAsVoid;
		if (hashMapGet(textureSys->map, pathPtr, stringHash32(path), &texture)) {
			return texture;
		}
	}

	if (!fileExists(path)) return NULL;

	Texture *texture = createTexture(path, flags);
	if (!texture) return NULL;

	void *pathAsVoid = texture->path;
	void *pathPtr = &pathAsVoid;
	hashMapSet(textureSys->map, pathPtr, stringHash32(texture->path), &texture);
	return texture;
}

void releaseTexture(Texture *texture);
void releaseTexture(Texture *texture) {
	logf("No way of removing textures from hashMap right now\n");
}

void clearTextureCache() {
  HashMapIterator iter;

  HashMapNode *node = init(&iter, textureSys->map);
  for (;;) {
    if (!node) break;

    Texture *texture = *(Texture **)node->value;
    destroyTexture(texture);

    node = next(&iter);
  }

  destroyHashMap(textureSys->map);
  textureSys->map = NULL;
}
