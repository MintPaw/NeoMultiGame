#ifndef __DYNA_IMPL
void menuInit();
void runMenu();
/// FUNCTIONS ^
#else

void menuInit() {
	game->playersNum = 1;
	for (int i = 0; i < game->playersNum; i++) {
		game->mapPlayers[i].minRefuel = 80;
		game->mapPlayers[i].maxRefuel = 120;
	}
}

void runMenu() {
	ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Once, ImVec2(0.5, 0.5));
	ImGui::Begin("Main menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Fuel Chance: (Min:%d, Max:%d)", game->mapPlayers[0].minRefuel, game->mapPlayers[0].maxRefuel);
	if (ImGui::Button("Narrow Gap") && game->mapPlayers[0].minRefuel != 100) {game->mapPlayers[0].minRefuel++; game->mapPlayers[0].maxRefuel--;}
	ImGui::SameLine();
	if (ImGui::Button("Widen Gap") && game->mapPlayers[0].minRefuel != 0) {game->mapPlayers[0].minRefuel--; game->mapPlayers[0].maxRefuel++;}
	ImGui::Separator();
	if (ImGui::Button("Start Game")) game->currentState = GS_MAP;
	ImGui::Separator();
	if (ImGui::Button("Quick Debug Battle")) game->currentState = GS_BATTLE;
	ImGui::Checkbox("Use Keyboard Easing", &game->tempBool);
	ImGui::End();
}
#endif