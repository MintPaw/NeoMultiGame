//
/// Hash map start
//

struct HashMapNode {
	void *key;
	void *value;
	HashMapNode *next;
};

struct HashMap {
	HashMapNode **lists;
	int listsNum;
	int keySize;
	int valueSize;
	Allocator *allocator;
	bool usesStreq;
};

HashMap *createHashMap(int keySize, int valueSize, int listsNum, Allocator *allocator);
HashMap *createHashMap(int keySize, int valueSize, int listsNum, Allocator *allocator) {
	HashMap *map = NULL;
	map = (HashMap *)allocateFrom(allocator, sizeof(HashMap));
	memset(map, 0, sizeof(HashMap));
	map->allocator = allocator;
	map->listsNum = listsNum;
	map->lists = (HashMapNode **)allocateFrom(map->allocator, sizeof(HashMapNode) * map->listsNum);
	memset(map->lists, 0, sizeof(HashMapNode) * map->listsNum);
	map->keySize = keySize;
	map->valueSize = valueSize;

	return map;
}

int FORCE_INLINE hashToIndex(HashMap *map, int hash) {
	int index = hash % map->listsNum;
	if (index < 0) return -index;
	return index;
}

void hashMapSet(HashMap *map, void *key, int hash, void *value) {
	int index = hashToIndex(map, hash);
	HashMapNode *list = map->lists[index];
	HashMapNode *temp = list;
	while (temp) {
		bool found = false;
		if (map->usesStreq && streq(*(char **)temp->key, *(char **)key)) found = true;
		if (!map->usesStreq && memcmp(temp->key, key, map->keySize) == 0) found = true;
		if (found) {
			memcpy(temp->key, key, map->keySize);
			memcpy(temp->value, value, map->valueSize);
			return;
		}
		temp = temp->next;
	}

	HashMapNode *newNode = NULL;
	newNode = (HashMapNode *)allocateFrom(map->allocator, sizeof(HashMapNode));
	memset(newNode, 0, sizeof(HashMapNode));

	newNode->key = allocateFrom(map->allocator, map->keySize);
	memcpy(newNode->key, key, map->keySize);

	newNode->value = allocateFrom(map->allocator, map->valueSize);
	memcpy(newNode->value, value, map->valueSize);

	newNode->next = list;
	map->lists[index] = newNode;
}

bool hashMapGet(HashMap *map, void *key, int hash, void *outValue=NULL);
bool hashMapGet(HashMap *map, void *key, int hash, void *outValue) {
	int index = hashToIndex(map, hash);
	HashMapNode *list = map->lists[index];

	HashMapNode *temp = list;
	while (temp) {
		// static int iter = 0;
		// iter++;
		// if (iter % 10000 == 0) logf("Iter: %d\n", iter);
		bool found = false;
		if (map->usesStreq && streq(*(char **)temp->key, *(char **)key)) found = true;
		if (!map->usesStreq && memcmp(temp->key, key, map->keySize) == 0) found = true;
		if (found) {
			if (outValue) memcpy(outValue, temp->value, map->valueSize);
			return true;
		}
		temp = temp->next;
	}

	return false;
}

void destroyHashMap(HashMap *map) {
	for (int i = 0; i < map->listsNum; i++) {
		HashMapNode *node = map->lists[i];
		while (node) {
			HashMapNode *nextNode = node->next;
			freeFrom(map->allocator, node->key);
			freeFrom(map->allocator, node->value);
			freeFrom(map->allocator, node);
			node = nextNode;
		}
	}

	freeFrom(map->allocator, map);
}

//
/// Hash map end
//

//
/// Priority queue start
//

struct PriorityQueueNode {
	void *data;
	int priority;
	PriorityQueueNode *next;
};

struct PriorityQueue {
	PriorityQueueNode *head;
	int dataSize;
	int length;
	Allocator *allocator;
};

PriorityQueue *createPriorityQueue(int dataSize, Allocator *allocator);
void priorityQueuePush(PriorityQueue *queue, void *data, int priority);
bool priorityQueueShift(PriorityQueue *queue, void *data);
void destroyPriorityQueue(PriorityQueue *queue);

PriorityQueue *createPriorityQueue(int dataSize, Allocator *allocator) {
	PriorityQueue *queue = (PriorityQueue *)allocateFrom(allocator, sizeof(PriorityQueue));
	queue->allocator = allocator;
	queue->dataSize = dataSize;
	return queue;
}

void priorityQueuePush(PriorityQueue *queue, void *data, int priority) {
	PriorityQueueNode *newNode = NULL;
	newNode = (PriorityQueueNode *)allocateFrom(queue->allocator, sizeof(PriorityQueueNode));
	newNode->data = allocateFrom(queue->allocator, queue->dataSize);
	memcpy(newNode->data, data, queue->dataSize);
	newNode->priority = priority;

	queue->length++;

	if (!queue->head) {
		queue->head = newNode;
		return;
	}

	if (queue->head->priority > newNode->priority) {
		newNode->next = queue->head;
		queue->head = newNode;
		return;
	}

	PriorityQueueNode *temp = queue->head;
	while (temp->next && temp->next->priority < newNode->priority) {
		temp = temp->next;
	}

	newNode->next = temp->next;
	temp->next = newNode;
}

bool priorityQueueShift(PriorityQueue *queue, void *data) {
	if (queue->length <= 0) return false;

	PriorityQueueNode *node = queue->head;
	queue->head = queue->head->next;
	queue->length--;

	memcpy(data, node->data, queue->dataSize);
	freeFrom(queue->allocator, node->data);
	freeFrom(queue->allocator, node);
	return true;
}

void destroyPriorityQueue(PriorityQueue *queue) {
	PriorityQueueNode *node = queue->head;
	while (node) {
		PriorityQueueNode *nextNode = node->next;
		freeFrom(queue->allocator, node->data);
		freeFrom(queue->allocator, node);
		node = nextNode;
	}

	freeFrom(queue->allocator, queue);
}

//
/// Priority queue end
//

//
/// Queue start
//

struct Queue {
	void *data;
	int elementSize;
	int elementsMaxNum;
	int length;
	int pushIndex;
	int shiftIndex;

	Allocator *allocator;
};

Queue *createQueue(int elementSize, int elementsMaxNum, Allocator *allocator) {
	Queue *queue = (Queue *)allocateFrom(allocator, sizeof(Queue));
	queue->allocator = allocator;
	queue->elementSize = elementSize;
	queue->elementsMaxNum = elementsMaxNum;
	queue->data = allocateFrom(queue->allocator, elementSize * elementsMaxNum);
	return queue;
}

bool queuePush(Queue *queue, void *data) {
	if (queue->length > queue->elementsMaxNum-1) return false;
	queue->length++;

	void *dest = (char *)queue->data + queue->pushIndex*queue->elementSize;
	memcpy(dest, data, queue->elementSize);

	queue->pushIndex++;
	if (queue->pushIndex > queue->elementsMaxNum-1) queue->pushIndex = 0;
	return true;
}

bool queueShift(Queue *queue, void *result) {
	if (queue->length <= 0) return false;
	queue->length--;

	void *src = (char *)queue->data + queue->shiftIndex*queue->elementSize;
	memcpy(result, src, queue->elementSize);

	queue->shiftIndex++;
	if (queue->shiftIndex > queue->elementsMaxNum-1) queue->shiftIndex = 0;
	return true;
}

void destroyQueue(Queue *queue) {
	freeFrom(queue->allocator, queue->data);
	freeFrom(queue->allocator, queue);
}

//
/// Queue end
//

//
/// Octree start
//


// struct Octree {
// 	AABB bounds;

// 	void *elements;
// 	int elementsNum;
// 	int elementsMax;
// 	int elementsSize;

// 	Octree *children;
// 	bool subdivided;
// };

// Octree *createOctree(int elementSize, int elementsPerNodeMax);
// Octree *createOctree(int elementSize, int elementsPerNodeMax) {
// 	Octree *octree = (Octree *)zalloc(sizeof(Octree));

// 	octree->elementMax = elementsPerNodeMax;
// 	octree->elementSize = elementSize;
// 	octree->elements = zalloc(elementSize);

// 	return octree;
// }

// void insert(Octree *octree, Triangle tri) {
// 	bool isInside = false;
// 	if (contains(octree->bounds, tri.verts[0])) isInside = true;
// 	if (!isInside && contains(octree->bounds, tri.verts[1])) isInside = true;
// 	if (!isInside && contains(octree->bounds, tri.verts[2])) isInside = true;
// 	if (!isInside && intersects(octree->bounds, tri)) isInside = true;

// 	if (!isInside) return;

// 	if (octree->trisNum > TRI_OCTREE_TRIS_MAX-1) {
// 		if (!octree->subdivided) {
// 			octree->subdivided = true;
// 			octree->children = (TriOctree *)zalloc(sizeof(TriOctree) * 8);

// 			AABB bounds = octree->bounds;
// 			Vec3 size = getSize(bounds);
// 			bounds.max = bounds.min + size/2;

// 			size = getSize(bounds);

// 			AABB firstBounds = bounds;
// 			octree->children[0].bounds = bounds;

// 			bounds.min.x += size.x;
// 			bounds.max.x += size.x;
// 			octree->children[1].bounds = bounds;

// 			bounds = firstBounds;
// 			bounds.min.y += size.y;
// 			bounds.max.y += size.y;
// 			octree->children[2].bounds = bounds;

// 			bounds = firstBounds;
// 			bounds.min.x += size.x;
// 			bounds.max.x += size.x;
// 			bounds.min.y += size.y;
// 			bounds.max.y += size.y;
// 			octree->children[3].bounds = bounds;

// 			bounds = firstBounds;
// 			bounds.min.z += size.z;
// 			bounds.max.z += size.z;
// 			octree->children[4].bounds = bounds;

// 			bounds = firstBounds;
// 			bounds.min.x += size.x;
// 			bounds.max.x += size.x;
// 			bounds.min.z += size.z;
// 			bounds.max.z += size.z;
// 			octree->children[5].bounds = bounds;

// 			bounds = firstBounds;
// 			bounds.min.y += size.y;
// 			bounds.max.y += size.y;
// 			bounds.min.z += size.z;
// 			bounds.max.z += size.z;
// 			octree->children[6].bounds = bounds;

// 			bounds = firstBounds;
// 			bounds.min.x += size.x;
// 			bounds.max.x += size.x;
// 			bounds.min.y += size.y;
// 			bounds.max.y += size.y;
// 			bounds.min.z += size.z;
// 			bounds.max.z += size.z;
// 			octree->children[7].bounds = bounds;
// 		}

// 		for (int i = 0; i < 8; i++) {
// 			insert(&octree->children[i], tri);
// 		}
// 	} else {
// 		octree->tris[octree->trisNum++] = tri;
// 	}
// }

//
/// Octree end
//

//
/// Pool start
//

struct Pool { // This is actually an arena
	void *data;
	int dataMax;
	int dataIndex;
	int dataHighIndexMark;
};

Pool *createPool(int size);
Pool *createPool(int size) {
	Pool *pool = (Pool *)zalloc(sizeof(Pool));
	pool->data = zalloc(size);
	pool->dataMax = size;
	return pool;
}

void *poolAlloc(Pool *pool, int size);
void *poolAlloc(Pool *pool, int size) {
	if (pool->dataMax - pool->dataIndex < size) {
		logf("No more room in pool\n");
		return NULL;
	}

	void *mem = ((u8 *)pool->data) + pool->dataIndex;
	pool->dataIndex += size;
	return mem;
}

void *poolZalloc(Pool *pool, int size);
void *poolZalloc(Pool *pool, int size) {
	void *mem = poolAlloc(pool, size);
	if (!mem) return NULL;

	memset(mem, 0, size);
	return mem;
}

void poolFree(Pool *pool);
void poolFree(Pool *pool) {
	if (pool->dataHighIndexMark < pool->dataIndex) pool->dataHighIndexMark = pool->dataIndex;
	pool->dataIndex = 0;
}

//
/// Pool end
//

//
/// QuadTree start
//

struct QuadTreeNode {
	Rect rect;
	u64 data;
};

struct QuadTree {
	Rect bounds;

#define QUAD_TREE_NODES_MAX 4
	QuadTreeNode nodes[QUAD_TREE_NODES_MAX];
	int nodesNum;

	QuadTree *children;
	bool subdivided;
};

QuadTree *createQuadTree(Rect bounds);
QuadTree *createQuadTree(Rect bounds) {
	QuadTree *tree = (QuadTree *)zalloc(sizeof(QuadTree));
	tree->bounds = bounds;
	return tree;
}

void insert(QuadTree *tree, Rect rect, u64 data);
void insert(QuadTree *tree, Rect rect, u64 data) {
	if (!contains(tree->bounds, rect)) return;

	if (tree->nodesNum > QUAD_TREE_NODES_MAX-1) {
		if (!tree->subdivided) {
			tree->subdivided = true;
			tree->children = (QuadTree *)zalloc(sizeof(QuadTree) * 4);

			Rect bounds = tree->bounds;
			bounds.width /= 2.0;
			bounds.height /= 2.0;

			tree->children[0].bounds = bounds;

			bounds.x += bounds.width;
			tree->children[1].bounds = bounds;

			bounds.x -= bounds.width;
			bounds.y += bounds.height;
			tree->children[2].bounds = bounds;

			bounds.x += bounds.width;
			tree->children[3].bounds = bounds;
		}

		for (int i = 0; i < 4; i++) insert(&tree->children[i], rect, data);
	} else {
		QuadTreeNode *node = &tree->nodes[tree->nodesNum++];
		node->rect = rect;
		node->data = data;
	}
}

void query(QuadTree *tree, Rect rect, u64 *data, int *dataNum, int dataMax);
void query(QuadTree *tree, Rect rect, u64 *data, int *dataNum, int dataMax) {
	if (!contains(tree->bounds, rect)) return;

	for (int i = 0; i < tree->nodesNum; i++) {
		QuadTreeNode *node = &tree->nodes[i];
		if (contains(rect, node->rect)) {
			if (*dataNum < dataMax-1) {
				data[*dataNum] = node->data;
				*dataNum = *dataNum + 1;
			}
		}
	}

	if (tree->subdivided) {
		for (int i = 0; i < 4; i++) {
			query(&tree->children[i], rect, data, dataNum, dataMax);
		}
	}
}

void destroyQuadTree(QuadTree *tree, bool depth=0);
void destroyQuadTree(QuadTree *tree, bool depth) {
	if (tree->subdivided) {
		for (int i = 0; i < 4; i++) destroyQuadTree(&tree->children[i], depth+1);
		free(tree->children);
	}

	if (depth == 0) free(tree);
}

//
/// QuadTree end
//

struct StringReader {
	char *base;
	int max;
	int index;

	bool stripWhitespace;
};

StringReader *newStringReader(char *string);
StringReader *newStringReader(char *string) {
	StringReader *reader = (StringReader *)zalloc(sizeof(StringReader));
	reader->base = stringClone(string);
	reader->max = strlen(reader->base);
	return reader;
}

bool readLineInto(StringReader *reader, char *dest, int max);
bool readLineInto(StringReader *reader, char *dest, int max) {
	dest[0] = 0;

	char *start = reader->base + reader->index;
	int spacesRemoved = 0;
	if (reader->stripWhitespace) {
		while(*start == ' ' && start < reader->base+reader->max) {
			spacesRemoved++;
			start++;
		}
	}
	char *lineEnd = strchr(start, '\n');
	int len = 0;
	if (lineEnd) {
		len = lineEnd - start;
	} else {
		len = strlen(start);
		if (len == 0) return false;
	}

	if (len > max-1) {
		logf("String reader readLineInto too small (max: %d, needs %d)\n", max, len+1);
		return false;
	}

	strncpy(dest, start, len);
	dest[len] = 0;
	reader->index += len + spacesRemoved + 1;

	return true;
}

void destroyStringReader(StringReader *reader);
void destroyStringReader(StringReader *reader) {
	free(reader->base);
	free(reader);
}
