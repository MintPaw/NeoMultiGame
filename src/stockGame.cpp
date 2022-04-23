#include "curl/curl.h"

struct Globals {
};

struct Quote {
	u32 time;
	float price;
};

struct Chart {
	int version;

	char symbol[8];

	double shares;
	double sharesBoughtAtPrice;

#define QUOTES_MAX 128
	Quote quotes[QUOTES_MAX];
	int quotesNum;

	/// Unserialized
	Texture *texture;
	float todayGainPerc;
	float todayGainDollar;
	float totalGainPerc;
	float totalGainDollar;
};

struct Game {
	CURL *curl;

	int lastCandle;

#define CHARTS_MAX 32
	Chart charts[CHARTS_MAX];
	int chartsNum;
	int lastUpdateTimeStamp;

	Vec2 lastMousePos;
	float timeSinceMouseMoved;

	Texture *tempTexture;
	Texture *cardTexture;
	bool needsImages;
	bool needsCardImage;

	bool editMode;
};

Game *game = NULL;

void runGame();
void updateGame();
size_t curlWriteFunction(void *ptr, size_t size, size_t nmemb, char **dataPtr);
float getCurrentPrice(char *symbol);
void writeCharts(DataStream *stream, Chart *charts, int chartsNum);
void readCharts(DataStream *stream, Chart *outCharts, int *outChartsNum);
char *unixTimeStampToFrameString(time_t stamp);
void drawChart(Chart *chart, Vec2 size, bool doExtras=false);
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameA(hModule, exeDir, PATH_MAX_LEN);

	char *lastSlash = strrchr(exeDir, '\\');
	if (!lastSlash) Panic("No last slash found in exe path");
	*lastSlash = 0;

#if defined(FALLOW_INTERNAL) // This needs to be a macro
		if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/MultiGame/multiGame/stockGameAssets");
#else
		snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#endif

#endif

	initFileOperations();

	initPlatform(720, 1000, "A stock thing.");
	platform->sleepWait = true;
	initAudio();
	initRenderer(720, 1000);
	// initMesh();
	// initModel();
	// initSkeleton();
	initFonts();
	initTextureSystem();
	// if (!initNetworking()) logf("Failed to init networking\n");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		curl_global_init(CURL_GLOBAL_DEFAULT);
		game->curl = curl_easy_init();
		if (!game->curl) logf("No curl!\n");

		DataStream *stream = loadDataStream("assets/charts");
		readCharts(stream, game->charts, &game->chartsNum);
		destroyDataStream(stream);

		ImPlot::GetStyle().UseLocalTime = true;
	}

	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);
	clearRenderer();

	time_t currentTime = time(NULL);
	tm globalTime = *gmtime(&currentTime);
	int year = globalTime.tm_year + 1900;
	int month = globalTime.tm_mon + 1;
	int day = globalTime.tm_mday;
	int dayOfWeek = globalTime.tm_wday;
	int hour = globalTime.tm_hour - 4;
	int min = globalTime.tm_min;
	int sec = globalTime.tm_sec;
	// char *globalDate = frameSprintf("%d%02d%02d %d:%d:%d", year, month, day, hour, min, sec);

	int candle = hour*4;
	int minsLeft = min;
	while (minsLeft > 10) {
		candle++;
		minsLeft -= 10;
	}
	if (game->lastCandle != candle) {
		game->lastCandle = candle;
		if (
			hour >= 9-1 &&
			hour <= 16+1 &&
			dayOfWeek != 0 &&
			dayOfWeek != 6 &&
			platform->frameCount > 10
		) {
			logf("Taking quotes at: %s\n", unixTimeStampToFrameString(time(NULL)));
			game->needsImages = true;
			for (int i = 0; i < game->chartsNum; i++) {
				Chart *chart = &game->charts[i];
				if (chart->quotesNum > QUOTES_MAX-1) {
					memmove(&chart->quotes[0], &chart->quotes[1], sizeof(Quote) * chart->quotesNum-1);
					chart->quotesNum--;
				}

				Quote *quote = &chart->quotes[chart->quotesNum++];
				quote->time = currentTime;
				quote->price = getCurrentPrice(chart->symbol);
			}

			DataStream *stream = newDataStream();
			writeCharts(stream, game->charts, game->chartsNum);
			writeDataStream("assets/charts", stream);
			destroyDataStream(stream);
		}
	}

	if (game->needsImages) {
		game->needsImages = false;
		game->needsCardImage = true;
		game->lastUpdateTimeStamp = currentTime;
		Vec2 chartSize = v2(256, 64);

		if (!game->tempTexture) game->tempTexture = createTexture(platform->windowWidth, platform->windowHeight);
		for (int i = 0; i < game->chartsNum; i++) {
			Chart *chart = &game->charts[i];

			pushTargetTexture(game->tempTexture); // I don't actually know if you need to push and pop these every time
			clearRenderer();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(chartSize.x, chartSize.y));
			ImGui::Begin(
				"###Chart",
				NULL,
				ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoBackground|ImGuiWindowFlags_NoTitleBar
			);
			drawChart(chart, v2(chartSize.x, chartSize.y));
			ImGui::End();
			ImGui::PopStyleVar();

			guiDraw();
			guiStartFrame();
			popTargetTexture();

			if (!chart->texture) chart->texture = createTexture(chartSize.x, chartSize.y);
			pushTargetTexture(chart->texture);
			clearRenderer();
			RenderProps props = newRenderProps();
			drawTexture(game->tempTexture, props);
			processBatchDraws();

			u8 *bitmapData = getFramebufferData(makeRect(0, 0, chart->texture->width, chart->texture->height));
			flipBitmapData(bitmapData, chart->texture->width, chart->texture->height);

			char *filePath = frameSprintf("%s/assets/images/%s.png", projectAssetDir, chart->symbol);

			if (!stbi_write_png(filePath, chart->texture->width, chart->texture->height, 4, bitmapData, chart->texture->width*4)) logf("Failed to create %s\n", filePath);
			popTargetTexture();

			free(bitmapData);
		}
	}

	if (!game->cardTexture) game->cardTexture = createTexture(600, 1000);
	if (game->needsCardImage) {
		pushTargetTexture(game->cardTexture);
		clearRenderer(0xFF000000);
	}

	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(game->cardTexture->width, game->cardTexture->height));
		ImGui::Begin(
			"Charts card",
			NULL,
			ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoScrollbar
		);
		if (!game->editMode) {
			ImGui::PushFont(gui->bigFont);
		}

		ImGui::Text("Updated %s", unixTimeStampToFrameString(game->lastUpdateTimeStamp));

		if (!game->needsCardImage) {
			if (ImGui::TreeNode("Debug")) {
				ImGui::Checkbox("Edit mode", &game->editMode);
				if (ImGui::Button("Generate images")) game->needsImages = true;
				if (ImGui::Button("Save charts")) {
					DataStream *stream = newDataStream();
					writeCharts(stream, game->charts, game->chartsNum);
					writeDataStream("assets/charts", stream);
					destroyDataStream(stream);
				}
				if (ImGui::Button("Load charts")) {
					DataStream *stream = loadDataStream("assets/charts");
					readCharts(stream, game->charts, &game->chartsNum);
					destroyDataStream(stream);
				}
				ImGui::TreePop();
			}
		}

		for (int i = 0; i < game->chartsNum; i++) {
			Chart *chart = &game->charts[i];
			ImGui::PushID(i);
			drawChart(chart, v2(300, 90), true);
			if (game->editMode) {
				if (ImGui::Button("Move up")) arraySwap(game->charts, game->chartsNum, sizeof(Chart), i, i-1);
				if (ImGui::Button("Move down")) arraySwap(game->charts, game->chartsNum, sizeof(Chart), i, i+1);
			}
			ImGui::Separator();
			ImGui::PopID();
		}

		if (!game->editMode) {
			ImGui::PopFont();
		}
		ImGui::End();
	}

	if (game->needsCardImage) {
		game->needsCardImage = false;

		processBatchDraws();
		guiDraw();
		guiStartFrame();

		Texture *cardTexture = game->cardTexture;
		u8 *bitmapData = getFramebufferData(makeRect(0, 0, cardTexture->width, cardTexture->height));
		flipBitmapData(bitmapData, cardTexture->width, cardTexture->height);
		for (int i = 0; i < cardTexture->width * cardTexture->height; i++) bitmapData[i * 4 + 3] = 255;

		char *filePath = frameSprintf("%s/assets/images/card.png", projectAssetDir);

		if (!stbi_write_png(filePath, cardTexture->width, cardTexture->height, 4, bitmapData, cardTexture->width*4)) logf("Failed to create %s\n", filePath);
		free(bitmapData);
		popTargetTexture();

#ifdef __LINUX__
		char *command = "scp -r -i /home/pi/skey /home/pi/multiGame/stockGameAssets/assets/images ubuntu@184.72.9.156:/var/www/html";
		if (system(command) != 0) logf("Images failed to send\n");
#endif
	}

	{
		game->timeSinceMouseMoved += platform->realElapsed;
		if (!equal(game->lastMousePos, platform->mouse)) {
			game->lastMousePos = platform->mouse;
			game->timeSinceMouseMoved = 0;
		}

		if (game->timeSinceMouseMoved > 5*60) {
			platformSleep(5000);
		}
	}

	drawOnScreenLog();
}

size_t curlWriteFunction(void *ptr, size_t size, size_t nmemb, char **dataPtr) {
	char *data = *dataPtr;

	int bytesToCopy = size * nmemb;
	if (strlen(data)+bytesToCopy+1 > 512) {
		bytesToCopy = 512 - strlen(data) - 1;
	}

	char *dataEnd = data + strlen(data);
	memcpy(dataEnd, ptr, bytesToCopy);
	dataEnd[bytesToCopy] = 0;
	return size * nmemb;
}

float getCurrentPrice(char *symbol) {
	char *token = "pk_457afdf3f4b34a2c850749c8b233cd80";

	char *url = frameSprintf(
		"https://cloud.iexapis.com/stable/stock/%s/quote/latestPrice?token=%s",
		symbol,
		token
	);
	// logf("url: %s\n", url);

	curl_easy_setopt(game->curl, CURLOPT_URL, url);
	curl_easy_setopt(game->curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(game->curl, CURLOPT_MAXREDIRS, 50);
	curl_easy_setopt(game->curl, CURLOPT_TCP_KEEPALIVE, 1);
	curl_easy_setopt(game->curl, CURLOPT_SSL_VERIFYPEER, false);

	char *responseString = frameMalloc(512);
	char *headerString = frameMalloc(512);
	curl_easy_setopt(game->curl, CURLOPT_WRITEFUNCTION, curlWriteFunction);
	curl_easy_setopt(game->curl, CURLOPT_WRITEDATA, &responseString);
	curl_easy_setopt(game->curl, CURLOPT_HEADERDATA, &headerString);

	CURLcode res = curl_easy_perform(game->curl);
	if (res != CURLE_OK) {
		logf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}

	// logf("Header:\n%s\n", headerString);
	// logf("Response:\n%s\n", responseString);
	float price = atof(responseString);

	return price;
}

void writeCharts(DataStream *stream, Chart *charts, int chartsNum) {
	writeU32(stream, chartsNum);

	for (int i = 0; i < chartsNum; i++) {
		Chart *chart = &charts[i];
		writeU32(stream, 3);
		writeString(stream, chart->symbol);

		writeDouble(stream, chart->shares);
		writeDouble(stream, chart->sharesBoughtAtPrice);

		writeU32(stream, chart->quotesNum);
		for (int i = 0; i < chart->quotesNum; i++) {
			Quote *quote = &chart->quotes[i];
			writeU32(stream, quote->time);
			writeFloat(stream, quote->price);
		}
	}
}

void readCharts(DataStream *stream, Chart *outCharts, int *outChartsNum) {
	int count = readU32(stream);
	for (int i = 0; i < count; i++) {
		Chart *chart = &outCharts[*outChartsNum];
		*outChartsNum = *outChartsNum + 1;

		chart->version = readU32(stream);

		char *str = readString(stream);
		strcpy(chart->symbol, str);
		free(str);

		if (chart->version >= 3) {
			chart->shares = readDouble(stream);
			chart->sharesBoughtAtPrice = readDouble(stream);
		}

		int quotesCount = readU32(stream);
		if (quotesCount > QUOTES_MAX) quotesCount = QUOTES_MAX;
		for (int i = 0; i < quotesCount; i++) {
			Quote *quote = &chart->quotes[chart->quotesNum++];
			quote->time = readU32(stream);
			quote->price = readFloat(stream);
		}
	}
}

char *unixTimeStampToFrameString(time_t stamp) {
	tm localTime = *localtime(&stamp);
	int year = localTime.tm_year + 1900;
	int month = localTime.tm_mon + 1;
	int day = localTime.tm_mday;
	int hour = localTime.tm_hour;
	int min = localTime.tm_min;
	int sec = localTime.tm_sec;
	if (hour > 12) hour -= 12;
	char *date = frameSprintf("%02d/%02d/%d %02d:%02d:%02d", month, day, year, hour, min, sec);
	return date;
}

void drawChart(Chart *chart, Vec2 size, bool doExtras) {
	if (chart->quotesNum == 0) return;

	u32 currentTime = time(NULL);
	u32 yesterdayTime = currentTime - 60*60*24;

	float openingPrice = 0;
	{
		Quote *prevQuote = NULL;
		for (int i = chart->quotesNum-1; i >= 0; i--) {
			Quote *quote = &chart->quotes[i];
			if (!prevQuote) {
				prevQuote = quote;
				continue;
			}

			u32 gap = prevQuote->time - quote->time;
			if (gap > 60*60*8) {
				yesterdayTime = prevQuote->time;
				openingPrice = quote->price; // Go back an extra one to actually get yesterday's closing price
				break;
			}

			prevQuote = quote;
		}
	}

	currentTime = yesterdayTime + 60*60*10;


	float *times = (float *)frameMalloc(sizeof(float) * chart->quotesNum);
	float *prices = (float *)frameMalloc(sizeof(float) * chart->quotesNum);
	float *openings = (float *)frameMalloc(sizeof(float) * chart->quotesNum);

	float minPrice = 9999;
	float maxPrice = -9999;
	for (int i = 0; i < chart->quotesNum; i++) {
		Quote *quote = &chart->quotes[i];
		times[i] = quote->time;
		prices[i] = quote->price;
		openings[i] = openingPrice;

		if (minPrice > quote->price) minPrice = quote->price;
		if (maxPrice < quote->price) maxPrice = quote->price;
	}

	if (minPrice > openingPrice) minPrice = openingPrice;
	if (maxPrice < openingPrice) maxPrice = openingPrice;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0)); // Not sure if I really need this
	ImGui::BeginChild(frameSprintf("%s chart child", chart->symbol), ImVec2(size.x, size.y), 0, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::PushFont(gui->defaultFont);

	ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
	ImPlot::SetNextPlotLimits(yesterdayTime, currentTime, minPrice, maxPrice, ImGuiCond_Always);
	char *label = frameSprintf("###%s", chart->symbol);
	if (doExtras) label = frameSprintf("%s", chart->symbol);

	if (ImPlot::BeginPlot(
			label,
			NULL,
			NULL,
			ImVec2(size.x, size.y),
			ImPlotFlags_NoLegend,
			ImPlotAxisFlags_Time|ImPlotAxisFlags_NoDecorations,
			ImPlotAxisFlags_NoDecorations|ImPlotAxisFlags_AutoFit
	)) {
		int color = stringHash32(chart->symbol);
		color |= 0xFF000000;
		color = lerpColor(color, 0xFF000000, 0.5);
		plotPushStyleColor(ImPlotCol_Line, color);
		plotPushStyleColor(ImPlotCol_Fill, color);

		ImPlot::PlotLine("Prices", times, prices, chart->quotesNum, 0, sizeof(float));
		ImPlot::PlotShaded("Prices", times, prices, chart->quotesNum, -INFINITY, 0, sizeof(float));

		plotPopStyleColor(2);

		ImPlot::PlotLine("Opening price", times, openings, chart->quotesNum, 0, sizeof(float));
		ImPlot::EndPlot();
	}
	ImPlot::PopStyleVar();
	ImGui::PopFont();
	ImGui::EndChild();

	if (doExtras) {
		ImGui::SameLine();

		Quote *lastQuote = &chart->quotes[chart->quotesNum-1];

		float startOfDayEquity = chart->shares * openingPrice;
		float equity = chart->shares * lastQuote->price;
		float initialEquity = chart->shares * chart->sharesBoughtAtPrice;
		chart->todayGainPerc = 1.0 - openingPrice/lastQuote->price;
		chart->todayGainDollar = startOfDayEquity * chart->todayGainPerc;
		chart->totalGainPerc = 1.0 - initialEquity/equity;
		chart->totalGainDollar = equity - initialEquity;

		ImGui::BeginChild(frameSprintf("%s info child", chart->symbol), ImVec2(size.x, size.y), 0, ImGuiWindowFlags_AlwaysAutoResize);
		if (game->editMode) {
			ImGui::InputDouble("Shares", &chart->shares, 0, 0, "%.6f");
			ImGui::InputDouble("Bought at", &chart->sharesBoughtAtPrice, 0, 0, "%.6f");
		}

		// ImGui::Text("Share price:   $%.2f", lastQuote->price);

		guiPushStyleColor(ImGuiCol_Text, chart->todayGainPerc > 0 ? 0xFF44FF44 : 0xFFFF4444);
		ImGui::Text("Today:  $%.2f (%.2f%%)", chart->todayGainDollar, chart->todayGainPerc*100.0);
		guiPopStyleColor();

		guiPushStyleColor(ImGuiCol_Text, chart->totalGainPerc > 0 ? 0xFF44FF44 : 0xFFFF4444);
		ImGui::Text("Total:    $%.2f (%.2f%%)", chart->totalGainDollar, chart->totalGainPerc*100.0);
		guiPopStyleColor();

		ImGui::EndChild();
	}
	ImGui::PopStyleVar(1);
}
