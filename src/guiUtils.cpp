ImColor toImColor(int argb);
int fromImColor(ImColor imColor);
bool guiInputRgb(const char *name, int *argb, bool showInputs=false);
bool guiInputArgb(const char *name, int *argb, bool showInputs=false);
void guiPushStyleColor(ImGuiCol style, int color);
void guiPopStyleColor(int amount=1);
int guiGetStyleColor(ImGuiCol style);
bool guiColoredTreeNodeEx(char *label, ImGuiTreeNodeFlags flags=0);
void guiColoredTreePop();
void guiMapVec4(char *srcName, char *destName, Vec4 *vec);

void toGradient(ImGradientHDRState *imGradState, Gradient *gradient);
void toImGradState(Gradient *gradient, ImGradientHDRState *imGradState);
void guiGradientEditor(char *name, Gradient *gradient);
int sampleGradient(Gradient gradient, float perc);
float sampleGradientFloat(Gradient gradient, float perc);

bool guiInputCurvePlot(char *label, CurvePlot *plot, Vec2 size=v2(800, 300));
float sampleCurvePlot(CurvePlot plot, float perc, bool flipY=false);

ImColor toImColor(int argb) {
	Vec4 vec = hexToArgbFloat(argb);
	return ImColor(ImVec4(vec.y, vec.z, vec.w, vec.x));
}

int fromImColor(ImColor imColor) {
	ImVec4 col = imColor.Value;
	return argbToHex(v4(col.w, col.x, col.y, col.z));
}

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

int guiGetStyleColor(ImGuiCol style) {
	ImVec4 col = ImGui::GetStyleColorVec4(style);
	return argbToHex(v4(col.w, col.x, col.y, col.z));
}

bool guiColoredTreeNodeEx(char *label, ImGuiTreeNodeFlags flags) {
	bool result = ImGui::TreeNodeEx(label, flags);
	if (!result) return false;

	int baseColor = guiGetStyleColor(ImGuiCol_FrameBg);
	int color = lerp(baseColor, stringHash32(label), 0.15);
	guiPushStyleColor(ImGuiCol_FrameBg, color);
	return true;
}
void guiColoredTreePop() { 
	guiPopStyleColor();
	ImGui::TreePop();
}

void guiMapVec4(char *srcName, char *destName, Vec4 *vec) {
	ImGui::PushID(frameSprintf("%s mapped to %s", srcName, destName));
	ImGui::PushItemWidth(120);

	bool hovering = false;

	ImGui::Text("%s", srcName);
	ImGui::SameLine();
	ImGui::InputFloat(frameSprintf("###SrcMin", srcName), &vec->x);
	ImGui::SameLine();
	ImGui::InputFloat(frameSprintf("###SrcMax", srcName), &vec->y);
	ImGui::SameLine();
	ImGui::Text("-> %s", destName);
	ImGui::SameLine();
	ImGui::InputFloat(frameSprintf("###DestMin", destName), &vec->z);
	ImGui::SameLine();
	ImGui::InputFloat(frameSprintf("###DestMax", destName), &vec->w);

	ImGui::SameLine();
	ImGui::Text("(?)");
	if (ImGui::IsItemHovered()) hovering = true;

	if (hovering) {
		StringBuilder builder = createStringBuilder(128);

		float srcMin = vec->x;
		float srcMax = vec->y;
		float destMin = vec->z;
		float destMax = vec->w;

		int colLen = MaxNum(strlen(srcName), strlen(destName));
		if (colLen < 8) colLen = 8;

		addText(&builder, frameSprintf("%*s -> %-*s\n", colLen, srcName, colLen, destName));
		for (int i = 0; i < 10; i++) {
			float perc = i / 10.0;
			char *srcStr = frameSprintf("%g", lerp(srcMin, srcMax, perc));
			char *destStr = frameSprintf("%g", lerp(destMin, destMax, perc));
			addText(&builder, frameSprintf("%*s -> %-*s\n", colLen, srcStr, colLen, destStr));
		}
		ImGui::BeginTooltip();
		ImGui::Text("%s", builder.string);
		ImGui::EndTooltip();

		destroy(builder);
	}

	ImGui::PopItemWidth();
	ImGui::PopID();
}

void toGradient(ImGradientHDRState *imGradState, Gradient *gradient) {
	gradient->colorMarkersNum = 0;
	for (int i = 0; i < imGradState->ColorCount; i++) {
		auto imColorMarker = imGradState->Colors[i];

		GradientColorMarker *colorMarker = &gradient->colorMarkers[gradient->colorMarkersNum++];
		colorMarker->position = imColorMarker.Position;
		colorMarker->color.x = imColorMarker.Color[0];
		colorMarker->color.y = imColorMarker.Color[1];
		colorMarker->color.z = imColorMarker.Color[2];
		colorMarker->intensity = imColorMarker.Intensity;
	}

	gradient->alphaMarkersNum = 0;
	for (int i = 0; i < imGradState->AlphaCount; i++) {
		auto imAlphaMarker = imGradState->Alphas[i];

		GradientAlphaMarker *alphaMarker = &gradient->alphaMarkers[gradient->alphaMarkersNum++];
		alphaMarker->position = imAlphaMarker.Position;
		alphaMarker->alpha = imAlphaMarker.Alpha;
	}
}

void toImGradState(Gradient *gradient, ImGradientHDRState *imGradState) {
	while (imGradState->ColorCount > 0) imGradState->RemoveColorMarker(0);
	for (int i = 0; i < gradient->colorMarkersNum; i++) {
		GradientColorMarker *colorMarker = &gradient->colorMarkers[i];
		imGradState->AddColorMarker(colorMarker->position, {colorMarker->color.x, colorMarker->color.y, colorMarker->color.z}, colorMarker->intensity);
	}

	while (imGradState->AlphaCount > 0) imGradState->RemoveAlphaMarker(0);
	for (int i = 0; i < gradient->alphaMarkersNum; i++) {
		GradientAlphaMarker *alphaMarker = &gradient->alphaMarkers[i];
		imGradState->AddAlphaMarker(alphaMarker->position, alphaMarker->alpha);
	}
}

void guiInputGradient(char *name, Gradient *gradient) {
	ImGradientHDRState imGradState;
	toImGradState(gradient, &imGradState);

	ImGui::PushID(frameSprintf("Gradient_Input_%s", name));
	ImGui::Separator();
	ImGui::Text("%s", name);
	ImGradientHDR(stringHash32(name), imGradState, gradient->imTempState, true);
	if (ImGui::IsItemClicked()) gradient->imTempState.selectedMarkerType = ImGradientHDRMarkerType::Unknown;
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Deselect");

	bool markerDeleted = false;
	if (gradient->imTempState.selectedMarkerType != ImGradientHDRMarkerType::Unknown) {
		if (ImGui::Button("Delete marker")) {
			markerDeleted = true;
			if (gradient->imTempState.selectedMarkerType == ImGradientHDRMarkerType::Color) {
				imGradState.RemoveColorMarker(gradient->imTempState.selectedIndex);
				gradient->imTempState = ImGradientHDRTemporaryState{};
			} else if (gradient->imTempState.selectedMarkerType == ImGradientHDRMarkerType::Alpha) {
				imGradState.RemoveAlphaMarker(gradient->imTempState.selectedIndex);
				gradient->imTempState = ImGradientHDRTemporaryState{};
			}
		}
	}

	if (!markerDeleted) {
		if (gradient->imTempState.selectedMarkerType == ImGradientHDRMarkerType::Color) {
			auto selectedColorMarker = imGradState.GetColorMarker(gradient->imTempState.selectedIndex);
			if (selectedColorMarker != nullptr) {
				ImGui::ColorEdit3("Color", selectedColorMarker->Color.data(), ImGuiColorEditFlags_DisplayHex);
				ImGui::SliderFloat("Intensity", &selectedColorMarker->Intensity, 0, 1);
			}
		}

		if (gradient->imTempState.selectedMarkerType == ImGradientHDRMarkerType::Alpha) {
			auto selectedAlphaMarker = imGradState.GetAlphaMarker(gradient->imTempState.selectedIndex);
			if (selectedAlphaMarker != nullptr) {
				ImGui::SliderFloat("Alpha", &selectedAlphaMarker->Alpha, 0, 1);
			}
		}
	}

	toGradient(&imGradState, gradient);
	ImGui::PopID();
}

int sampleGradient(Gradient gradient, float perc) {
	ImGradientHDRState imGradState;
	toImGradState(&gradient, &imGradState);
	auto combinedColor = imGradState.GetCombinedColor(perc);
	return argbToHex(combinedColor[3] * 255, combinedColor[0] * 255, combinedColor[1] * 255, combinedColor[2] * 255);
}

float sampleGradientFloat(Gradient gradient, float perc) {
	ImGradientHDRState imGradState;
	toImGradState(&gradient, &imGradState);
	auto combinedColor = imGradState.GetCombinedColor(perc);
	return combinedColor[1];
}

bool guiInputCurvePlot(char *label, CurvePlot *plot, Vec2 size) {
	ImGui::PushID(label);
	bool ret = false;
	if (plot->pointsNum < CURVE_PLOT_POINTS_MAX-1) plot->points[plot->pointsNum].x = ImGui::CurveTerminator;
	if (ImGui::Curve(label, ImVec2(size.x, size.y), CURVE_PLOT_POINTS_MAX, (ImVec2 *)plot->points, &plot->imSelected)) {
		plot->pointsNum = 0;
		for (int i = 0; i < CURVE_PLOT_POINTS_MAX; i++) {
			if (plot->points[i].x != ImGui::CurveTerminator) plot->pointsNum++;
		}

		ret = true;
	}

	ImGuiIO& io = ImGui::GetIO();

	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::PushItemWidth(io.DisplaySize.x * 0.03);
	ImGui::DragFloat("Max", &plot->max, 0.01);
	ImGui::DragFloat("Min", &plot->min, 0.01);
	ImGui::PopItemWidth();
	ImGui::EndGroup();

	ImGui::PopID();
	return ret;
}

float sampleCurvePlot(CurvePlot plot, float perc, bool flipY) {
	if (plot.pointsNum == 0) {
		plot.points[plot.pointsNum++] = v2(0, 0);
		plot.points[plot.pointsNum++] = v2(1, 1);
	}

	perc = ImGui::CurveValueSmooth(perc, plot.pointsNum, (ImVec2 *)plot.points);

	if (flipY) perc = 1 - perc;

	return lerp(plot.min, plot.max, perc);
}
