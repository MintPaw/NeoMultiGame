bool guiInputRgb(const char *name, int *argb, bool showInputs=false);
bool guiInputArgb(const char *name, int *argb, bool showInputs=false);
void guiPushStyleColor(ImGuiCol style, int color);
void guiPopStyleColor(int amount=1);

bool guiInputRgb(const char *name, int *argb, bool showInputs) {
	int a;
	int intCol[3];
	hexToArgb(*argb, &a, &intCol[0], &intCol[1], &intCol[2]);

	float floatCol[3];
	floatCol[0] = intCol[0]/255.0;
	floatCol[1] = intCol[1]/255.0;
	floatCol[2] = intCol[2]/255.0;

	int flags = ImGuiColorEditFlags_DisplayHex;
	if (!showInputs) flags |= ImGuiColorEditFlags_NoInputs;
	bool ret = ImGui::ColorEdit3(name, floatCol, flags);

	intCol[0] = floatCol[0] * 255.0;
	intCol[1] = floatCol[1] * 255.0;
	intCol[2] = floatCol[2] * 255.0;
	*argb = argbToHex(255, intCol[0], intCol[1], intCol[2]);

	return ret;
}

bool guiInputArgb(const char *name, int *argb, bool showInputs) {
	int intCol[4];
	hexToArgb(*argb, &intCol[0], &intCol[1], &intCol[2], &intCol[3]);

	float floatCol[4];
	floatCol[0] = intCol[1]/255.0;
	floatCol[1] = intCol[2]/255.0;
	floatCol[2] = intCol[3]/255.0;
	floatCol[3] = intCol[0]/255.0;

	int flags = 0;
	if (showInputs) {
		flags |= ImGuiColorEditFlags_DisplayHex;
	} else {
		flags |= ImGuiColorEditFlags_NoInputs;
	}
	bool ret = ImGui::ColorEdit4(name, floatCol, flags | ImGuiColorEditFlags_DisplayHex);

	intCol[0] = floatCol[3] * 255.0;
	intCol[1] = floatCol[0] * 255.0;
	intCol[2] = floatCol[1] * 255.0;
	intCol[3] = floatCol[2] * 255.0;
	*argb = argbToHex(intCol[0], intCol[1], intCol[2], intCol[3]);

	return ret;
}

void guiPushStyleColor(ImGuiCol style, int color) {
	Vec4 vecColor = hexToArgbFloat(color);
	ImGui::PushStyleColor(style, ImVec4(vecColor.y, vecColor.z, vecColor.w, vecColor.x));
}

void guiPopStyleColor(int amount) {
	ImGui::PopStyleColor(amount);
}

bool guiColoredTreeNodeEx(char *label, ImGuiTreeNodeFlags flags=0);
bool guiColoredTreeNodeEx(char *label, ImGuiTreeNodeFlags flags) {
	bool result = ImGui::TreeNodeEx(label, flags);
	if (!result) return false;
	ImVec4 imColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);

	int baseColor = argbToHex(v4(imColor.w, imColor.x, imColor.y, imColor.z));
	int color = lerp(baseColor, stringHash32(label), 0.25);
	guiPushStyleColor(ImGuiCol_FrameBg, color);
	return true;
}
void guiColoredTreePop();
void guiColoredTreePop() { 
	guiPopStyleColor();
	ImGui::TreePop();
}
    // IMGUI_API bool          TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags = 0);
