#ifndef __DYNA_IMPL
enum MapTileGraphic {
	HOUSE,
	FOREST,
	EVENT,
	MAPTILE_GRAPHIC_END,
};

char *mapTileGraphicStrings[] = {
	"House",
	"Forest",
	"Event",
};

struct MapPoint {
	Vec2 position;
	MapTileGraphic type;
	u64 padding1;
	u64 padding2;
};

struct MapData {
	#define MAPPOINTS_MAX 256
	MapPoint mapPoints[MAPPOINTS_MAX];
	u8 mapPointsNum;
};

struct MapPlayer {
	u8 srcPoint;
	u8 destPoint;
	bool onPoint;
	float travelProgress;
	int fuel;
	float pixelsPerFuel;
	int minRefuel;
	int maxRefuel;
};

void mapGlobals();
void mapInit();
void runMap();
void retarget(u8 targetMapPointId);
void refuel();
void travel();
/// FUNCTIONS ^
#else

void mapGlobals() {
	if (ImGui::TreeNode("MapData")) {
		MapData *mapData = &game->globals.debugMapData;

		if (ImGui::Button("Add Map Point")) mapData->mapPoints[mapData->mapPointsNum++].position = v2(100.0, 100.0);
		for (int i = 0; i < mapData->mapPointsNum; i++) {
			MapPoint *mapPoint = &mapData->mapPoints[i];

			ImGui::DragFloat2(frameSprintf("MP_%d", i), &mapPoint->position.x, 1.0);
			ImGui::SameLine();
			ImGui::Combo(frameSprintf("type_%d", i), (int *)&mapPoint->type, mapTileGraphicStrings, MAPTILE_GRAPHIC_END);
		}
		ImGui::TreePop();
	}
}

void mapInit() {
	for (int i = 0; i < game->playersNum; i++) {
		game->mapPlayers[i].onPoint = true;
		game->mapPlayers[i].pixelsPerFuel = 2.0;
	}

	game->selectedPoint = -1;
	game->cursorPos = v2(640.0, 360.0);
}

void runMap() {
	MapData *mapData = &game->globals.debugMapData;
	{ /// DRAW MAP
		for (int i = 0; i < mapData->mapPointsNum; i++) {
			RenderProps props = newRenderProps();
			props.matrix.TRANSLATE(mapData->mapPoints[i].position);
			if(i == game->selectedPoint) props.matrix.SCALE(0.9 + timePhase(platform->time) * 0.2);
			props.matrix.TRANSLATE(-64, -64); //The map tile is 128x128 hence -64

			drawTexture(getTexture("assets/images/MapTile.png"), props);
			drawTexture(getTexture(frameSprintf("assets/images/mapIcons/%s.png", mapTileGraphicStrings[mapData->mapPoints[i].type])), props);
		}
	}

	MapPlayer *mapPlayer = &game->mapPlayers[game->currentPlayer];
	if (mapPlayer->onPoint) {
		if (game->prevMouse.x != platform->mouse.x || game->prevMouse.y != platform->mouse.y) game->mouseDisableCooldown = 40;

		bool mouseEnabled = game->mouseDisableCooldown-- > 0;
		bool moving = false;

		if (mouseEnabled) {
			if (platform->mouseDown) {
				game->cursorPos = platform->mouse;
				moving = true;
			}
		} else {
			if (keyPressed(KEY_UP)) {moving = true; game->cursorPos.y -= 4.0;}
			if (keyPressed(KEY_DOWN)) {moving = true; game->cursorPos.y += 4.0;}
			if (keyPressed(KEY_LEFT)) {moving = true; game->cursorPos.x -= 4.0;}
			if (keyPressed(KEY_RIGHT)) {moving = true; game->cursorPos.x += 4.0;}
		}

		if (moving) {
			game->selectedPoint = -1;
		} else {
			game->selectedPoint = -1;
			float currentScore = mouseEnabled ? 64.0 : 32.0;
			for (int i = 0; i < mapData->mapPointsNum; i++) {
				float dist = distanceBetween(game->cursorPos.x, game->cursorPos.y, mapData->mapPoints[i].position.x, mapData->mapPoints[i].position.y);
				if (dist < currentScore) {
					game->selectedPoint = i;
					currentScore = dist;
					game->cursorPos = mapData->mapPoints[i].position;
				}
			}
		}

		if (game->selectedPoint != -1) {
			ImGui::Begin("Context Menu", NULL, ImGuiWindowFlags_None);
				if (keyJustPressed('E') || ImGui::Button("Embark (E)")) {
					refuel();
					retarget((u8)game->selectedPoint);
					game->selectedPoint = -1;
				}

				MapPoint *srcPoint = &mapData->mapPoints[mapPlayer->destPoint];
				MapPoint *destPoint = &mapData->mapPoints[game->selectedPoint];
				float dist = distanceBetween(srcPoint->position.x, srcPoint->position.y, destPoint->position.x, destPoint->position.y);
				int fuelNeeded = ceil(dist/mapPlayer->pixelsPerFuel);
				float chanceOfFailure = (float)(fuelNeeded - mapPlayer->minRefuel) / (mapPlayer->maxRefuel - mapPlayer->minRefuel);
				chanceOfFailure = mathClamp(chanceOfFailure * 100, 0.0, 100.0);
				float chanceOfSuccess = 100.0 - chanceOfFailure;

				ImGui::Text("Fuel Needed: %d", fuelNeeded);
				ImGui::Text("Chance of Success: %.1f%%", chanceOfSuccess);
			ImGui::End();
		}

		if (!mouseEnabled) {
			Texture *texture = getTexture("assets/images/Cursor.png");
			RenderProps props = newRenderProps();
			props.matrix.TRANSLATE(game->cursorPos);
			props.matrix.TRANSLATE(texture->width * -0.5, texture->height * -0.5);

			drawTexture(texture, props);
		}
	}
	else {
		if (keyJustPressed('F')) refuel();
		if (mapPlayer->fuel > 0) travel();
	}

	{ /// DRAW PLAYER
		MapPoint *srcPoint = &mapData->mapPoints[mapPlayer->srcPoint];
		MapPoint *destPoint = &mapData->mapPoints[mapPlayer->destPoint];

		Vec2 playerPosition;
		if (mapPlayer->onPoint) {
			playerPosition = destPoint->position;
		}
		else {
			float dist = distanceBetween(srcPoint->position.x, srcPoint->position.y, destPoint->position.x, destPoint->position.y);
			dist = (1 / dist) * (dist - mapPlayer->travelProgress);
			playerPosition = lerp(destPoint->position, srcPoint->position, dist);
		}
		drawCircle(playerPosition, 8, 0xFFEEBBFF);
	}
}

void retarget(u8 targetMapPointId) {
	MapPlayer *mapPlayer = &game->mapPlayers[game->currentPlayer];
	logf("Retargetting..");
	mapPlayer->srcPoint = mapPlayer->destPoint;
	mapPlayer->destPoint = targetMapPointId;
	mapPlayer->onPoint = false;
	mapPlayer->travelProgress = 0.0;
}

void refuel() {
	MapPlayer *mapPlayer = &game->mapPlayers[game->currentPlayer];
	mapPlayer->fuel += rndInt(mapPlayer->minRefuel, mapPlayer->maxRefuel);
	logf("Fuel: %d", mapPlayer->fuel);
}

void travel() {
	MapPlayer *mapPlayer = &game->mapPlayers[game->currentPlayer];
	mapPlayer->fuel--;
	mapPlayer->travelProgress += mapPlayer->pixelsPerFuel;

	MapData *mapData = &game->globals.debugMapData;
	MapPoint *srcPoint = &mapData->mapPoints[mapPlayer->srcPoint];
	MapPoint *destPoint = &mapData->mapPoints[mapPlayer->destPoint];
	float dist = distanceBetween(srcPoint->position.x, srcPoint->position.y, destPoint->position.x, destPoint->position.y);
	dist = (1 / dist) * (dist - mapPlayer->travelProgress);

	if (dist <= 0.0) {
		mapPlayer->onPoint = true;
		mapPlayer->fuel = 0;
		logf("Reached");
	}
}
#endif