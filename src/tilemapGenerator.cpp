namespace Tile {

	enum Direction {
		DIRECTION_UP,
		DIRECTION_DOWN,
		DIRECTION_LEFT,
		DIRECTION_RIGHT,
	};
	enum RoomType {
		ROOM_NORMAL,
		ROOM_HALL,
	};
	struct Room {
		RoomType type;
		Rect rect;

		Direction hallDirection;
		int hallThickness;
#define TILES_MAX 64
		Vec2 tiles[TILES_MAX];
		int tilesNum;
	};

	struct GeneratorProps {
		int roomsMin;
		int roomsMax;
		int roomSizeMin;
		int roomSizeMax;
		int hallwaySizeMin;
		int hallwaySizeMax;
		bool pruneExtraWalls;
		// int hallThicknessMin;
		// int hallThicknessMax;
	};

	struct Map {
		GeneratorProps props;
#define ROOMS_MAX 64
		Room rooms[ROOMS_MAX];
		int roomsNum;

		int *tiles;
		int width;
		int height;

		bool dead;
	};

	GeneratorProps newGeneratorProps();
	Map *startMap(int width, int height, GeneratorProps props);
	Map *generate(int width, int height);
	void finalizeMap(Map *map);
	Room getRoom(Map *map, int x, int y, int width, int height, Room *hall=NULL);
	void addRoom(Map *map, Room newRoom);
	bool canPlaceRoom(Map *map, Rect rect);
	Room getHall(Map *map, int length);
	/// FUNCTIONS ^

	GeneratorProps newGeneratorProps() {
		GeneratorProps props = {};
		props.roomsMin = 3;
		props.roomsMax = 5;
		props.roomSizeMin = 3;
		props.roomSizeMax = 4;
		props.hallwaySizeMin = 2;
		props.hallwaySizeMax = 5;
		props.pruneExtraWalls = true;
		return props;
	}

	Map *startMap(int width, int height, GeneratorProps props) {
		Map *map = (Map *)zalloc(sizeof(Map));
		map->width = width;
		map->height = height;
		map->props = props;

		map->tiles = (int *)zalloc(sizeof(int) * map->width * map->height);
		for (int i = 0; i < width * height; i++) map->tiles[i] = 1;
		return map;
	}

	Map *generate(int width, int height, GeneratorProps props) {
		Map *map = startMap(width, height, props);
		// int roomSizeMin = 3;
		// int roomSizeMax = 4;

		if (props.roomSizeMax * props.roomSizeMax > TILES_MAX-1) {
			logf("Room size too big for TILES_MAX\n");
			return NULL;
		}
		{ /// Starting room
			int width = rndInt(props.roomSizeMin, props.roomSizeMax);
			int height = rndInt(props.roomSizeMin, props.roomSizeMax);
			int x = map->width/2 - width/2;
			int y = map->height/2 - height/2;
			Room room = getRoom(map, x, y, width, height);
			addRoom(map, room);
		}

		// int hallwaySizeMin = 2;
		// int hallwaySizeMax = 5;

		// int roomsMin = 3;
		// int roomsMax = 5;
		int roomsCount = rndInt(props.roomsMin, props.roomsMax);
		for (int i = 0; i < roomsCount; i++) {
			int roomAttempts = 0;
			while (true) {
				roomAttempts++;
				if (roomAttempts > 1000) {
					logf("Map gen died because of roomAttempts\n");
					break;
				}
				Room hall = getHall(map, rndInt(props.hallwaySizeMin, props.hallwaySizeMax));
				if (map->dead) break;
				Vec2 lastPoint = hall.tiles[hall.tilesNum-1];
				Room room = getRoom(map, lastPoint.x, lastPoint.y, rndInt(props.roomSizeMin, props.roomSizeMax), rndInt(props.roomSizeMin, props.roomSizeMax), &hall);

				if (canPlaceRoom(map, room.rect.inflate(2))) {
					addRoom(map, hall);
					addRoom(map, room);
					break;
				}

				if (map->dead) break;
			}
		}

		finalizeMap(map);
		return map;
	}

	void finalizeMap(Map *map) {
		int *toPruneInds = (int *)frameMalloc(sizeof(int) * map->width * map->height);
		int toPruneIndsNum = 0;
		if (map->props.pruneExtraWalls) {
			for (int y = 0; y < map->height; y++) {
				for (int x = 0; x < map->width; x++) {
					bool canPrune = true;
					if (x-1 >= 0 && map->tiles[y * map->width + (x-1)] == 0) canPrune = false;
					if (x+1 <= map->width-1 && map->tiles[y * map->width + (x+1)] == 0) canPrune = false;
					if (y-1 >= 0 && map->tiles[(y-1) * map->width + x] == 0) canPrune = false;
					if (y+1 <= map->height-1 && map->tiles[(y+1) * map->width + x] == 0) canPrune = false;
					if (x-1 >= 0 && y-1 >= 0 && map->tiles[(y-1) * map->width + (x-1)] == 0) canPrune = false;
					if (x+1 <= map->width-1 && y-1 >= 0 && map->tiles[(y-1) * map->width + (x+1)] == 0) canPrune = false;
					if (x-1 >= 0 && y+1 <= map->height-1 && map->tiles[(y+1) * map->width + (x-1)] == 0) canPrune = false;
					if (x+1 <= map->width-1 && y+1 <= map->height-1 && map->tiles[(y+1) * map->width + (x+1)] == 0) canPrune = false;

					if (canPrune) toPruneInds[toPruneIndsNum++] = y * map->width + x;
				}
			}
		}

		for (int i = 0; i < toPruneIndsNum; i++) {
			map->tiles[toPruneInds[i]] = -1;
		}
	}

	Room getHall(Map *map, int length) {
		Room hall = {};
		hall.type = ROOM_HALL;

		int *possibleTiles = (int *)frameMalloc(sizeof(int) * map->width * map->height);
		int possibleTilesNum = 0;
		for (int i = 0; i < map->width * map->height; i++) {
			int x = i % map->width;
			int y = i / map->width;

			if (map->tiles[y * map->width + x] == 0) {
				if (
					map->tiles[y * map->width + (x+1)] == 1 || 
					map->tiles[y * map->width + (x-1)] == 1 || 
					map->tiles[(y+1) * map->width + x] == 1 || 
					map->tiles[(y-1) * map->width + x] == 1
				) {
					possibleTiles[possibleTilesNum++] = i;
				}
			}
		}

		int hallThicknessMin = 1;
		int hallThicknessMax = 1;
		int hallAttempts = 0;
		while (true) {
			hallAttempts++;
			if (hallAttempts == 100000) {
				map->dead = true;
				logf("Map gen died because of hallAttempts\n");
				return hall;
			}

			// hall.hallThickness = rndInt(hallThicknessMin, hallThicknessMax);
			hall.hallThickness = 1;
			hall.tilesNum = 0;

			int emptyIndex = possibleTiles[rndInt(0, possibleTilesNum-1)];
			Vec2 hallStart;
			hallStart.x = emptyIndex % map->width;
			hallStart.y = emptyIndex / map->width;

			hall.tiles[hall.tilesNum++] = hallStart;

			hall.hallDirection = (Direction)rndInt(0, 3);

			for (int i = 0; i < length; i++) {
				if (hall.tilesNum > TILES_MAX-1) {
					logf("Too many hall tiles\n");
					break;
				}

				Vec2 newTile = hall.tiles[hall.tilesNum-1];
				if (hall.hallDirection == DIRECTION_LEFT) newTile.x--;
				if (hall.hallDirection == DIRECTION_RIGHT) newTile.x++;
				if (hall.hallDirection == DIRECTION_UP) newTile.y--;
				if (hall.hallDirection == DIRECTION_DOWN) newTile.y++;

				hall.tiles[hall.tilesNum++] = newTile;
			}

			// const int HALL_THICKNESS_MAX = 256;
			// int hallThickness[HALL_THICKNESS_MAX];
			// int hallThicknessNum = 0;

			// hall.wouldBeDoor.push(hall.tiles[hall.tiles.length - 1]);

			// for (i in 0...hall.tiles.length)
			// {
			// 	var addToThickness:Array<FlxPoint> = [];
			// 	if (hall.direction == FlxObject.LEFT || hall.direction == FlxObject.RIGHT)
			// 	{
			// 		if (hall.thickness >= 2) addToThickness.push(hall.tiles[i].copyTo().add(0, -1));
			// 		if (hall.thickness >= 3) addToThickness.push(hall.tiles[i].copyTo().add(0, -2));
			// 	}
			// 	if (hall.direction == FlxObject.UP || hall.direction == FlxObject.DOWN)
			// 	{
			// 		if (hall.thickness >= 2) addToThickness.push(hall.tiles[i].copyTo().add(-1, 0));
			// 		if (hall.thickness >= 3) addToThickness.push(hall.tiles[i].copyTo().add(-2, 0));
			// 	}

			// 	for (j in 0...addToThickness.length)
			// 	{
			// 		hallThickness.push(addToThickness[j]);
			// 		// if (i == hall.tiles.length - 1) hall.wouldBeDoor.push(addToThickness[j]);
			// 	}
			// }

			// for (i in 0...hallThickness.length) hall.tiles.push(hallThickness[i]);

			// for (i in 0...hall.tiles.length)
			// {
			// 	toScan.push(hall.tiles[i].copyTo());
			// 	if (hall.direction == FlxObject.LEFT || hall.direction == FlxObject.RIGHT)
			// 	{
			// 		toScan.push(hall.tiles[i].copyTo().add(0, -1));
			// 		toScan.push(hall.tiles[i].copyTo().add(0, 1));
			// 	}
			// 	if (hall.direction == FlxObject.UP || hall.direction == FlxObject.DOWN)
			// 	{
			// 		toScan.push(hall.tiles[i].copyTo().add(-1, 0));
			// 		toScan.push(hall.tiles[i].copyTo().add(1, 0));
			// 	}
			// }

			Vec2 min = v2(99999, 99999);
			Vec2 max = v2(-99999, -99999);
			for (int i = 0; i < hall.tilesNum; i++) {
				Vec2 tile = hall.tiles[i];
				if (min.x > tile.x) min.x = tile.x;
				if (min.y > tile.y) min.y = tile.y;
				if (max.x < tile.x) max.x = tile.x;
				if (max.y < tile.y) max.y = tile.y;
			}

			hall.rect.x = min.x;
			hall.rect.y = min.y;
			hall.rect.width = max.x - min.x;
			hall.rect.height = max.y - min.y;

			if (hall.rect.width == 0) hall.rect.width = 1;
			if (hall.rect.height == 0) hall.rect.height = 1;

			if (canPlaceRoom(map, hall.rect)) {
				break;
			}
			// if (scanTiles(toScan)) break;
		}

		return hall;
	}

	bool canPlaceRoom(Map *map, Rect rect) {
		int startX = rect.x;
		int startY = rect.y;
		int endX = startX + rect.width;
		int endY = startY + rect.height;

		if (startX < 0) return false;
		if (startY < 0) return false;
		if (endX > map->width-1) return false;
		if (endY > map->height-1) return false;

		for (int y = startY; y < endY; y++) {
			for (int x = startX; x < endX; x++) {
				int index = y * map->width + x;
				if (map->tiles[index] == 0) return false;
			}
		}

		return true;
	}

	Room getRoom(Map *map, int x, int y, int width, int height, Room *hall) {
		Room room = {};

		if (width % 2 == 0) width++;
		if (height % 2 == 0) height++;

		if (hall) {
			if (hall->hallDirection == DIRECTION_LEFT) x -= width;
			if (hall->hallDirection == DIRECTION_RIGHT) x += width;
			if (hall->hallDirection == DIRECTION_UP) y -= height;
			if (hall->hallDirection == DIRECTION_DOWN) y += height;

			if (hall->hallDirection == DIRECTION_LEFT || hall->hallDirection == DIRECTION_RIGHT) y -= rndInt(0, height-1);
			if (hall->hallDirection == DIRECTION_UP || hall->hallDirection == DIRECTION_DOWN) x -= rndInt(0, width-1);
		}

		room.rect = makeRect(x, y, width, height);

		int startX = room.rect.x;
		int startY = room.rect.y;
		int endX = startX + room.rect.width;
		int endY = startY + room.rect.height;
		for (int y = startY; y < endY; y++) {
			for (int x = startX; x < endX; x++) {
				room.tiles[room.tilesNum++] = v2(x, y);
			}
		}

		return room;
	}

	void addRoom(Map *map, Room newRoom) {
		if (map->roomsNum > ROOMS_MAX-1) {
			logf("Too many rooms\n");
			return;
		}

		Room *room = &map->rooms[map->roomsNum++];
		*room = newRoom;

		int startX = room->rect.x;
		int startY = room->rect.y;
		int endX = startX + room->rect.width;
		int endY = startY + room->rect.height;
		for (int y = startY; y < endY; y++) {
			for (int x = startX; x < endX; x++) {
				int index = y * map->width + x;
				map->tiles[index] = 0;
			}
		}
	}

}
