# ENGINE_TYPE := custom
ENGINE_TYPE := raylib

ifeq ($(shell echo $$HOSTNAME), MintPaw-PC)
# GAME_NAME=bulletHellGame
# GAME_NAME=bulletHellGameServer
# GAME_NAME=turnBasedGame
# GAME_NAME=parametersGame
# GAME_NAME=petGame
# GAME_NAME=stockGame
GAME_NAME=towerGame
# GAME_NAME=dynaGame
# GAME_NAME=rtsClient2
# GAME_NAME=testGame
# GAME_NAME=catsFirstGame
# GAME_NAME=interrogationGame
# GAME_NAME=chessGame
# GAME_NAME=tdSlasherGame
# GAME_NAME=zooBoundGame
# GAME_NAME=horseGame
# GAME_NAME=gladiators2Game
# GAME_NAME=concreteJungleGame
# GAME_NAME=animationToolsGame
# GAME_NAME=rayGame
# GAME_NAME=rollerGame
# GAME_NAME=butt2GoGame
endif

ifeq ($(shell echo $$HOSTNAME), mintpaw-lappy)
GAME_NAME=horseGame
endif

ifeq ($(shell echo $$HOSTNAME), MintPaw-tablet)
GAME_NAME=concreteJungleGame
# GAME_NAME=butt2GoGame
# GAME_NAME=horseGame
endif


RAW_ASSETS_DIR=raw
WIN_LIBS=lib/win64
WIN_INCS=include/win64
PARAPHORE_COM_PATH := /d/paraphore.com
HORSE_GAME_EARLY_DIR := horse_34228
HORSE_GAME_DEV_DIR := horse_dev_23489
HORSE_GAME_PUBLIC_DIR := HoracesHoarseHorseWhores
BUTT_2_GO_PUBLIC_DIR := Butt2Go

SEVEN_ZIP="/c/Program Files/7-Zip/7z.exe"

ifeq ($(ENGINE_TYPE), raylib)
WIN_BUILD_BAT=buildWin32Ray
EMSCRIPTEN_BUILD_BAT=buildEmscriptenRay
else
WIN_BUILD_BAT=buildWin32
EMSCRIPTEN_BUILD_BAT=buildEmscripten
endif

ifeq ($(shell echo $$HOSTNAME), MintPaw-PC)
	PARAPHORE_COM_PATH := /d/paraphore.com
endif

ifeq ($(shell echo $$USERNAME), Nanachi)
	PARAPHORE_COM_PATH := /c/paraphore.com
endif

ifeq ($(shell echo $$HOSTNAME), MintPaw-PC)
	WIN_API_TRACE_DIR=d:/_tools/apitrace/bin
else
	WIN_API_TRACE_DIR=c:/_tools/apitrace/bin
endif

PYTHON3=/c/Users/MintPaw/AppData/Local/Programs/Python/Python37-32/python.exe

all:
	$(MAKE) b
	$(MAKE) r

ifneq (, $(findstring MSYS_NT, $(shell uname))) # -------------------------------------------------- Windows
bdebugrel:
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"

brel:
ifeq ($(GAME_NAME), horseGame)
	rsync -at "/c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets" "/c/Dropbox/MultiGame/multiGame/horseGameAssets"
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=0&& \
		set FAST_CLANG_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		set INTERNAL_MODE=0&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	while ! rm -rf "/c/Dropbox/MultiGame/multiGame/horseGameAssets/assets"; \
	do \
	sleep 1; \
	done;
	
else
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
endif

rel:
	$(MAKE) brel
	$(MAKE) r

optiDebug:
ifeq ($(GAME_NAME), horseGame)
	cp -r "/c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets" "/c/Dropbox/MultiGame/multiGame/horseGameAssets"
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set FAST_CLANG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set DEBUG_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	rm -rf "/c/Dropbox/MultiGame/multiGame/horseGameAssets/*"
else
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set INTERNAL_MODE=1&& \
		set DEBUG_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
endif
	$(MAKE) r

ray:
	$(MAKE) bray
	$(MAKE) r

bray:
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set CLANG_MODE=0&& \
		buildSystem\buildWin32Ray.bat\
		"

bwray:
	-cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set ASSETS_ONLY=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"
	
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		buildSystem\buildEmscriptenRay.bat\
		"

packRelAssets:
	echo hi

b:
ifeq ($(GAME_NAME), horseGame)
	rsync -at "/c/Dropbox/MultiGame/multiGame/commonAssets/assets/common" "/c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets" &
else ifeq ($(GAME_NAME), rtsClient2)
	rsync -at "/c/Dropbox/MultiGame/multiGame/commonAssets/assets/common" "/c/Dropbox/rtsGame/rtsClientAssets/assets" &
else ifeq ($(GAME_NAME), catsFirstGame)
	rsync -at "/c/Dropbox/MultiGame/multiGame/commonAssets/assets/common" "/c/Dropbox/CatFallowWing/catsFirstGame/assets" &
else ifeq ($(GAME_NAME), interrogationGame)
	rsync -at "/c/Dropbox/MultiGame/multiGame/commonAssets/assets/common" "/c/Dropbox/CatFallowWing/interrogationGame/assets" &
else ifeq ($(GAME_NAME), zooBoundGame)
	rsync -at "/c/Dropbox/MultiGame/multiGame/commonAssets/assets/common" "/c/Dropbox/ZooBound/zooBoundGameAssets/assets" &
endif
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set CLANG_MODE=0&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"

clang:
	$(MAKE) bclang
	$(MAKE) r

bclang:
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set CLANG_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"

r:
	@echo .
	@echo .
	@echo .
	@(cd /c/bin; ./$(GAME_NAME).exe)

bwf:
	echo $(GAME_NAME)
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set CODE_ONLY=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"

bw:
	-cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"

bwdebugrel:
	-cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"

bwrel:
	echo $(GAME_NAME)
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(EMSCRIPTEN_BUILD_BAT).bat\
		"

rw:
	clear; cd C:/bin/webgl; $(PYTHON3) -m http.server

debugVs:
	# $(MAKE) debugC
	cmd /c "devenv C:/bin/$(GAME_NAME).exe"

debugC:
	-cmd /c "D:\_tools\remedyBg\remedyBg.exe C:/bin/$(GAME_NAME).exe" &
	-cmd /c "C:\_tools\remedyBg\remedyBg.exe C:/bin/$(GAME_NAME).exe" &

resetSite:
	# if [[ ! -d "$(PARAPHORE_COM_PATH)" ]]; then \
	# 	git clone git@github.com:FallowWing/paraphore.com.git $(PARAPHORE_COM_PATH) --depth=1 --recurse; \
	# 	fi
	
	# cd $(PARAPHORE_COM_PATH) && \
	# 	git fetch origin && \
	# 	git reset --hard origin/master && \
	# 	git pull

shipDir:
	cd $(SHIP_DIR); \
		newPrefix=`pwd | grep -o "paraphore.com.*"`; \
		newPrefix=$${newPrefix:14}; \
		s3cmd sync --delete-removed --acl-public --exclude '.git/*' . s3://paraphore.com/$$newPrefix/;

shipAll:
	cd $(PARAPHORE_COM_PATH); \
		newPrefix=`pwd | grep -o "paraphore.com.*"`; \
		newPrefix=$${newPrefix:14}; \
		s3cmd sync --delete-removed --acl-public --exclude '.git/*' . s3://paraphore.com
endif # ------------------------------------------------------------------------------------------ End Windows

ifneq (, $(findstring Linux, $(shell uname))) # -------------------------------------------------- Linux

b:
	mkdir -p bin
	clang -g \
		-I include/linux \
		-I include/all \
		src/main.cpp \
		-DPLAYING_$(GAME_NAME) \
		-x c++ \
		include/win64/stb_image.h \
		include/win64/stb_sprintf.h \
		include/win64/stb_image_write.h \
		include/win64/imstb_truetype.h \
		include/win64/imstb_rectpack.h \
		-o bin/$(GAME_NAME) \
		-lm -lGL -lGLEW -lstdc++ -lopenal -lSDL2 -lpthread -no-pie

r:
	(cd bin; ./$(GAME_NAME))
endif # ------------------------------------------------------------------------------------------ End Linux

buildAssets:
	rm -rf assets/audio
	for file in `find $(RAW_ASSETS_DIR)/audio/ -type f -name "*.wav"`; do \
		baseFileName=`basename $$file`; \
		baseFileName="$${baseFileName%.*}"; \
		endDir=`dirname $$file`; \
		endDir=assets/"$${endDir#*/}"; \
		endFile=$$endDir/$$baseFileName.ogg; \
		echo $$file to $$endFile; \
		mkdir -p $$endDir; \
		ffmpeg -i $$file -ar 44100 -loglevel error -qscale:a 2 $$endFile -y& \
	done; \
	wait

debugGl:
	cd /c/bin; \
	rm -f gl.trace; \
	cmd /c "$(WIN_API_TRACE_DIR)/apitrace trace -o gl.trace -v --api gl $(GAME_NAME).exe"; \
	cmd /c "$(WIN_API_TRACE_DIR)/qapitrace gl.trace"

runChromium:
	cmd /c "D:/_tools/chromium/chrome.exe"

debugGlChrome:
	# cmd /c "D:/_tools/chromium/chrome.exe --no-sandbox --disable-gpu-watchdog --gpu-startup-dialog"
	cmd /c "set RENDERDOC_HOOK_EGL=0 && D:/_tools/chromium/chrome.exe --disable-gpu-sandbox --gpu-startup-dialog"

mtune:
	cmd /c "D:\_tools\mtuner\mtuner.exe C:\bin\$(GAME_NAME).exe" &
	cmd /c "C:\_tools\mtuner\mtuner.exe C:\bin\$(GAME_NAME).exe" &

debugGlGpuDemo:
	cd /c/Dropbox/rtsGame/rtsClientAssets/assets/__raw/tools/iqm/demo/; \
	rm -f gl.trace; \
	cmd /c "$(WIN_API_TRACE_DIR32)/apitrace trace -o gl.trace -v --api gl gpu-demo.exe"; \
	cmd /c "$(WIN_API_TRACE_DIR32)/qapitrace gl.trace"

ctags:
	ctags -R .tags src/$(GAME_NAME).cpp
	# ctags -R --tag-relative=yes -f .tags src/$(GAME_NAME).cpp

clean:
	rm -rf bin/*
	rm -rf /c/bin/*

optimizePngs:
	find $(GAME_NAME)Assets/assets -iname *.png -print0 | xargs -0 -P 8 -n 4 optipng -o3

optimizePngsHard:
	find $(GAME_NAME)Assets/assets -iname *.png -print0 | xargs -0 -P 8 -n 4 optipng -o5

optimizePngsInsane:
	find $(GAME_NAME)Assets/assets -iname *.png -print0 | xargs -0 -P 8 -n 4 optipng -o7

convertWavesToOggs:
		for file in `find $(GAME_NAME)Assets/assets/audio -type f -name "*.wav"`; do \
		name=`basename $$file`; \
		name=$${name%.*}; \
		echo $$file $$name; \
		ffmpeg -hide_banner -loglevel warning -i $$file $$(dirname $$file)/$$name.ogg; \
		rm $$file; \
		done

reEncodeAllOggs:
	-mkdir bin
	rm -rf bin/tempReEncode
	cp -r $(GAME_NAME)Assets/assets/audio bin/tempReEncode
	cd bin/tempReEncode; \
		for file in `find . -type f -name "*.ogg"`; do \
		name=`basename $$file`; \
		name=$${name%.*}; \
		echo $$file $$name; \
		ffmpeg -hide_banner -loglevel warning -i $$file $$(dirname $$file)/$$name.wav; \
		ffmpeg -hide_banner -loglevel warning -i $$(dirname $$file)/$$name.wav $$file -y; \
		done; \
		find . -type f -name "*.wav" -delete

shipPlay:
	$(MAKE) shipDir SHIP_DIR="$(PARAPHORE_COM_PATH)/play"

shipFpsDesktop:
	cmd /c "\
		set GAME_NAME=fpsGame&& \
		set DEBUG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		finalName=fpsGame_`date +'%y_%m_%d_%H'`; \
		echo $$finalName; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at \
			--exclude '__*' --exclude '*.psd' --exclude '*_baked.png' --exclude 'Lightmaps' --exclude 'LilySurface' \
			--exclude '*.blend' --exclude '*.blend1' \
			/c/Dropbox/MultiGame/multiGame/fpsGameAssets/assets .; \
		$(SEVEN_ZIP) a -tzip -mx=9 -mm=LZMA fpsGame.zip *; \
		mv fpsGame.zip ../$$finalName.zip; \
		cp -r /c/bin/pack /d/laptopShare; \

shipFpsLaptop:
	$(MAKE) shipFpsDesktop
	rsync -a /c/bin/pack/* /z/

fpsEncodeAudio:
	rm -rf /c/Dropbox/MultiGame/multiGame/fpsGameAssets/assets/audio
	cp -r /c/Dropbox/MultiGame/multiGame/fpsGameAssets/assets/__raw/audio /c/Dropbox/MultiGame/multiGame/fpsGameAssets/assets/audio
	for file in `find /c/Dropbox/MultiGame/multiGame/fpsGameAssets/assets/audio -type f -name "*.wav"`; do \
		ffmpeg -i $$file -ar 44100 $${file%.*}.ogg; \
	done
	find /c/Dropbox/MultiGame/multiGame/fpsGameAssets/assets/audio -type f -name "*.wav" -delete

countEngine:
	cloc --by-file --not-match-d ".*Game.*" --not-match-f ".*GameData.*|.*Game.cpp|.*rts.*" src

countHorseGame:
	cloc --by-file src/horseGame.cpp src/horseGame/

pack: #@todo remove
	cmd /c "\
		set GAME_NAME=$(GAME_NAME)&& \
		set DEBUG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at \
			--exclude '__*' --exclude '*.psd' --exclude '*_baked.png' --exclude 'Lightmaps' --exclude 'LilySurface' \
			--exclude '*.blend' --exclude '*.blend1' \
			/c/Dropbox/MultiGame/multiGame/$(GAME_NAME)Assets/assets .; \
		rsync -a /c/bin/pack/* /c/bin/selfShip
		cp /c/bin/*.pdb /c/bin/selfShip
		
		cd /c/bin; \
			$(SEVEN_ZIP) a -tzip selfShip.zip selfShip

shipHorseGameToRc:
	-$(MAKE) clean
	cmd /c "\
		set GAME_NAME=horseGame&& \
		set FAST_CLANG_MODE=1&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		cp -r /c/bin/pack/* "/c/Dropbox/FallowCandy/HorseProjects/game"
	magick convert /c/Dropbox/FallowCandy/HorseProjects/raw/icon.png /c/bin/icon.ico
	while ! cmd /c "rcedit c:/Dropbox/FallowCandy/HorseProjects/game/horseGame.exe --set-icon C:/bin/icon.ico"; \
	do \
	sleep 1; \
	done;

shipHorseGamePublicToSite:
	$(MAKE) clean
	make bwrel GAME_NAME=horseGame
	cp /c/Dropbox/FallowCandy/HorseProjects/raw/html/index.html /c/bin/webgl
	-rm -rf $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_PUBLIC_DIR)
	mkdir $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_PUBLIC_DIR)
	cp -r /c/bin/webgl/* $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_PUBLIC_DIR)
	
	$(MAKE) clean
	make brel GAME_NAME=horseGame
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
	magick convert /c/Dropbox/FallowCandy/HorseProjects/raw/icon.png /c/bin/icon.ico
	while ! cmd /c "rcedit c:/bin/pack/horseGame.exe --set-icon C:/bin/icon.ico"; \
		do \
		sleep 1; \
		done;
	mv /c/bin/pack /c/bin/HoracesHoarseHorseWhores
	cd /c/bin/HoracesHoarseHorseWhores; \
		mv horseGame.exe HoracesHoarseHorseWhores.exe; \
		cp -r /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets .; \
		$(SEVEN_ZIP) a -tzip HoracesHoarseHorseWhores.zip ../HoracesHoarseHorseWhores; \
		cp HoracesHoarseHorseWhores.zip $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_PUBLIC_DIR);
	
	$(MAKE) shipPlay

genHorseGameZips:
	-rm -rf /c/temp/dupZips
	mkdir /c/temp/dupZips
	cd /c/temp/dupZips; \
		cp -r $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_PUBLIC_DIR)/* .; \
		cp /c/Dropbox/FallowCandy/HorseProjects/raw/html/rcIndex.html index.html; \
		cp /c/Dropbox/FallowCandy/HorseProjects/raw/html/rcMovie.html movie.html; \
		rm -f ../rcHtml.zip; \
		$(SEVEN_ZIP) a -tzip ../rcHtml.zip .; \
		cp /c/Dropbox/FallowCandy/HorseProjects/raw/html/ngIndex.html index.html; \
		rm -f HoracesHoarseHorseWhores.zip; \
		rm -f movie.html; \
		rm -f ../ngHtml.zip; \
		$(SEVEN_ZIP) a -tzip ../ngHtml.zip .
	

shipHorseGameEarlyToSite:
	$(MAKE) clean
	make bwrel GAME_NAME=horseGame
	cp /c/Dropbox/FallowCandy/HorseProjects/raw/html/index.html /c/bin/webgl
	-rm -rf $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_EARLY_DIR)
	mkdir $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_EARLY_DIR)
	cp -r /c/bin/webgl/* $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_EARLY_DIR)
	
	$(MAKE) clean
	make brel GAME_NAME=horseGame
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
	magick convert /c/Dropbox/FallowCandy/HorseProjects/raw/icon.png /c/bin/icon.ico
	while ! cmd /c "rcedit c:/bin/pack/horseGame.exe --set-icon C:/bin/icon.ico"; \
		do \
		sleep 1; \
		done;
	mv /c/bin/pack /c/bin/HoracesHoarseHorseWhores
	cd /c/bin/HoracesHoarseHorseWhores; \
		mv horseGame.exe HoracesHoarseHorseWhores.exe; \
		cp -r /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets .; \
		$(SEVEN_ZIP) a -tzip HoracesHoarseHorseWhores.zip ../HoracesHoarseHorseWhores; \
		cp HoracesHoarseHorseWhores.zip $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_EARLY_DIR);
	$(MAKE) shipPlay

shipHorseGameDevToSite:
	$(MAKE) clean
	make bwdebugrel GAME_NAME=horseGame
	cp /c/Dropbox/FallowCandy/HorseProjects/raw/html/index.html /c/bin/webgl
	-rm -rf $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_DEV_DIR)
	mkdir $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_DEV_DIR)
	cp -r /c/bin/webgl/* $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_DEV_DIR)
	
	$(MAKE) clean
	make bdebugrel GAME_NAME=horseGame
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
	magick convert /c/Dropbox/FallowCandy/HorseProjects/raw/icon.png /c/bin/icon.ico
	while ! cmd /c "rcedit c:/bin/pack/horseGame.exe --set-icon C:/bin/icon.ico"; \
		do \
		sleep 1; \
		done;
	mv /c/bin/pack /c/bin/HoracesHoarseHorseWhores
	cd /c/bin/HoracesHoarseHorseWhores; \
		mv horseGame.exe HoracesHoarseHorseWhores.exe; \
		cp -r /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets .; \
		$(SEVEN_ZIP) a -tzip HoracesHoarseHorseWhores.zip ../HoracesHoarseHorseWhores; \
		cp HoracesHoarseHorseWhores.zip $(PARAPHORE_COM_PATH)/play/$(HORSE_GAME_DEV_DIR);
	$(MAKE) shipPlay

shipZooBoundToDropbox:
	-$(MAKE) clean
	cmd /c "\
		set GAME_NAME=zooBoundGame&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		cp -r /c/bin/pack/* "/c/Dropbox/ZooBound/game"

shipZooBoundToSelf:
	-$(MAKE) clean
	cmd /c "\
		set GAME_NAME=zooBoundGame&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at \
			--exclude '__*' --exclude '*.psd' --exclude '*.blend' --exclude '*.blend1' \
			/c/Dropbox/ZooBound/zooBoundGameAssets/assets .; \
		rsync -a /c/bin/pack/* /c/bin/selfShip
		cp /c/bin/*.pdb /c/bin/selfShip

shipHorseGameToSelf:
	cmd /c "\
		set GAME_NAME=horseGame&& \
		set DEBUG_MODE=0&& \
		set FAST_CLANG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at \
			--exclude '__*' --exclude '*.psd' --exclude '*_baked.png' --exclude 'Lightmaps' --exclude 'LilySurface' \
			--exclude '*.blend' --exclude '*.blend1' \
			/c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets .; \
		rsync -a /c/bin/pack/* /c/bin/selfShip
		# cp /c/bin/*.pdb /c/bin/selfShip
		
		cd /c/bin; \
			$(SEVEN_ZIP) a -tzip selfShip.zip selfShip

encodeHorseGameAudio:
	rm -rf /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/audio
	cp -r /c/Dropbox/FallowCandy/HorseProjects/raw/audio /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/audio
	for file in `find /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/audio -type f -name "*.wav"`; do \
		ffmpeg -i $$file -ar 44100 $${file%.*}.ogg & \
	done; \
	wait
	find /c/Dropbox/FallowCandy/HorseProjects/horseGameAssets/assets/audio -type f -name "*.wav" -delete

encodeButt2GoGameAudio:
	rm -rf /c/Dropbox/FallowCandy/Butt2Go/butt2GoGameAssets/assets/audio
	cp -r /c/Dropbox/FallowCandy/Butt2Go/raw/audio /c/Dropbox/FallowCandy/Butt2Go/butt2GoGameAssets/assets/audio
	for file in `find /c/Dropbox/FallowCandy/Butt2Go/butt2GoGameAssets/assets/audio -type f -name "*.wav"`; do \
		ffmpeg -i $$file -ar 44100 $${file%.*}.ogg & \
	done; \
	wait
	find /c/Dropbox/FallowCandy/Butt2Go/butt2GoGameAssets/assets/audio -type f -name "*.wav" -delete

encodeConcreteJungleGameAudio:
	rm -rf /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets/audio
	cp -r /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets/__raw/audio /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets/audio
	for file in `find /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets/audio -type f -name "*.wav"`; do \
		ffmpeg -i $$file -ar 44100 $${file%.*}.ogg & \
	done; \
	wait
	find /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets/audio -type f -name "*.wav" -delete

buildBhgOptimizedDebug:
	cmd /c "\
		set GAME_NAME=bulletHellGame&& \
		set DEBUG_MODE=0&& \
		set FAST_CLANG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"

shipBulletHellGameToSelf:
	make clean
	cmd /c "\
		set GAME_NAME=bulletHellGame&& \
		set DEBUG_MODE=0&& \
		set FAST_CLANG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at \
			--exclude '__*' --exclude '*.psd' --exclude '*_baked.png' --exclude 'Lightmaps' --exclude 'LilySurface' \
			--exclude '*.blend' --exclude '*.blend1' \
			/c/Dropbox/MultiGame/multiGame/bulletHellGameAssets/assets .; \
		$(SEVEN_ZIP) a -tzip -mx=9 -mm=LZMA bulletHellGame.zip *; \

shipBulletHellGameToLaptop:
	$(MAKE) shipBulletHellGameToSelf
	rm -rf /z/*
	cp -r /c/bin/pack/* /z/

encodeBulletHellGameAudio:
	rm -rf /c/Dropbox/MultiGame/multiGame/bulletHellGameAssets/assets/audio
	cp -r /c/Dropbox/MultiGame/multiGame/bulletHellGameAssets/assets/__raw/audio /c/Dropbox/MultiGame/multiGame/bulletHellGameAssets/assets/audio
	for file in `find /c/Dropbox/MultiGame/multiGame/bulletHellGameAssets/assets/audio -type f -name "*.wav"`; do \
		ffmpeg -i $$file -ar 44100 $${file%.*}.ogg & \
	done; \
	wait
	find /c/Dropbox/MultiGame/multiGame/bulletHellGameAssets/assets/audio -type f -name "*.wav" -delete

bbulletHell:
	$(MAKE) b GAME_NAME=bulletHellGameServer
	$(MAKE) b GAME_NAME=bulletHellGame

bulletHell:
	$(MAKE) b GAME_NAME=bulletHellGameServer
	$(MAKE) b GAME_NAME=bulletHellGame
	$(MAKE) r GAME_NAME=bulletHellGame &
	$(MAKE) r GAME_NAME=bulletHellGame &
		$(MAKE) r GAME_NAME=bulletHellGameServer

rbulletHell:
	$(MAKE) r GAME_NAME=bulletHellGame &
	$(MAKE) r GAME_NAME=bulletHellGame &
	$(MAKE) r GAME_NAME=bulletHellGameServer

shipBulletHellServer:
	rsync -a --progress --human-readable -zz --compress-level=9 -e "ssh -i /d/_backup/bulletHellGameServerKey.pem" \
		src include/all include/linux bulletHellGameServerAssets buildSystem ubuntu@184.72.9.156:/home/ubuntu/bulletHellGameServer/
	scp -i /d/_backup/bulletHellGameServerKey.pem bulletHellGameServerAssets/assets/runServer.sh ubuntu@184.72.9.156:/home/ubuntu/runServer.sh
	ssh -i /d/_backup/bulletHellGameServerKey.pem -x ubuntu@184.72.9.156 "chmod +x runServer.sh; bash runServer.sh </dev/null >/home/ubuntu/buildLog.txt 2>&1 & "
	$(MAKE) watchBulletHellServer

watchBulletHellServer:
	ssh -i /d/_backup/bulletHellGameServerKey.pem ubuntu@184.72.9.156 -t 'watch -d -n 1 "cat buildLog.txt | tail -n 25"'

buildBulletHellGameServer:
	mkdir -p bin
		clang -g \
		-I include/linux \
		-I include/all \
		-I /usr/include/SDL2 \
		src/main.cpp \
		-DPLAYING_bulletHellGameServer \
		-x c++ \
		-fsanitize=address \
		-Wno-writable-strings \
		include/linux/stb_image.h \
		include/linux/stb_sprintf.h \
		include/linux/stb_image_write.h \
		include/linux/imstb_truetype.h \
		include/linux/imstb_rectpack.h \
		-D FALLOW_COMMAND_LINE_ONLY \
		-D PROJECT_ASSET_DIR=/home/ubuntu/bulletHellGameServer/bulletHellGameServerAssets \
		-o bin/bulletHellGameServer \
		-lm -lGL -lGLEW -lstdc++ -lopenal -lSDL2 -lpthread -lz -no-pie

stockGameLinux:
	-rm -rf bin/stockGame
	
	mkdir -p bin
		clang -g \
		-I include/linux \
		-I include/all \
		-I /usr/include/SDL2 \
		src/main.cpp \
		-DPLAYING_stockGame \
		-x c++ \
		-Wno-writable-strings \
		include/linux/stb_image.h \
		include/linux/stb_sprintf.h \
		include/linux/stb_image_write.h \
		include/linux/imstb_truetype.h \
		include/linux/imstb_rectpack.h \
		-D PROJECT_ASSET_DIR=/home/pi/multiGame/stockGameAssets \
		-o bin/stockGame \
		-lm -lGL -lGLEW -lstdc++ -lopenal -lSDL2 -lpthread -lz -lcurl -no-pie
		
		-killall stockGame
		export DISPLAY=:0; bin/stockGame &

getStockChartsFromPi:
	scp pi@192.168.2.21:/home/pi/multiGame/stockGameAssets/assets/charts stockGameAssets/assets/charts

shipStockGameToPi:
	$(MAKE) getStockChartsFromPi
	rsync -a --progress --human-readable -zz --compress-level=9 -e "ssh" \
		src include include stockGameAssets buildSystem Makefile pi@192.168.2.21:/home/pi/multiGame/
	ssh -x pi@192.168.2.21 "cd multiGame; make stockGameLinux </dev/null >/home/pi/stockGameBuildLog.txt 2>&1 & "
	$(MAKE) watchStockGame

watchStockGame:
	ssh pi@192.168.2.21 -t 'watch -d -n 1 "cat stockGameBuildLog.txt | tail -n 25"'

shipTestGameToSelf:
	cmd /c "\
		set GAME_NAME=testGame&& \
		set DEBUG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at \
			--exclude '__*' --exclude '*.psd' --exclude '*_baked.png' --exclude 'Lightmaps' --exclude 'LilySurface' \
			--exclude '*.blend' --exclude '*.blend1' \
			/c/Dropbox/MultiGame/multiGame/testGameAssets/assets .; \
		rsync -a /c/bin/pack/* /c/bin/selfShip
		cp /c/bin/*.pdb /c/bin/selfShip
		
		cd /c/bin; \
			$(SEVEN_ZIP) a -tzip selfShip.zip selfShip

shipCatsFirstGameToCat:
	-$(MAKE) clean
	cmd /c "\
		set GAME_NAME=catsFirstGame&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		cp -r /c/bin/pack/* "/c/Dropbox/CatFallowWing/catsFirstGame/bin"

shipInterrogationGameToCat:
	-$(MAKE) clean
	cmd /c "\
		set GAME_NAME=interrogationGame&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		cp -r /c/bin/pack/* "/c/Dropbox/CatFallowWing/interrogationGame/bin"
	cp /c/Dropbox/MultiGame/multiGame/src/interrogationGame.cpp /c/Dropbox/CatFallowWing/interrogationGame/sources/interrogationGame`date +"%m-%d-%Y-%H-%M-%S"`.cpp

shipConcreteJungleGameToLen:
	-$(MAKE) clean
	cmd /c "\
		set GAME_NAME=concreteJungleGame&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \

devShipConcreteJungleGameToSelf:
	-$(MAKE) clean
	cmd /c "\
		set GAME_NAME=concreteJungleGame&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=0&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		rsync -at --exclude '__*' --exclude '*.psd' --exclude '*.blend' --exclude '*.blend1' /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets .; \
		rsync -a /c/bin/pack/* /c/bin/selfShip
	
	cd /c/bin; \
		$(SEVEN_ZIP) a -tzip selfShip.zip selfShip; \
		mv selfShip.zip /c/Dropbox/Archive/devVersions/concreteJungle_$$(date +"%Y_%m_%d_%I_%M_%p").zip

optimizeConcreteJunglePng:
	find /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets -iname *.png -print0 | xargs -0 -P 8 -n 4 optipng -o1

shipButt2GoToRc:
	-$(MAKE) clean
	cmd /c "\
		set GAME_NAME=butt2GoGame&& \
		set FAST_CLANG_MODE=1&& \
		set DEBUG_MODE=1&& \
		set INTERNAL_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
		cp -r /c/bin/pack/* "/c/Dropbox/FallowCandy/Butt2Go/game"

shipButt2GoPublicToSite:
	$(MAKE) clean
	make bwrel GAME_NAME=butt2GoGame
	cp /c/Dropbox/FallowCandy/Butt2Go/raw/html/index.html /c/bin/webgl
	-rm -rf $(PARAPHORE_COM_PATH)/play/$(BUTT_2_GO_PUBLIC_DIR)
	mkdir $(PARAPHORE_COM_PATH)/play/$(BUTT_2_GO_PUBLIC_DIR)
	cp -r /c/bin/webgl/* $(PARAPHORE_COM_PATH)/play/$(BUTT_2_GO_PUBLIC_DIR)
	
	$(MAKE) clean
	rsync -at "/c/Dropbox/FallowCandy/Butt2Go/butt2GoGameAssets/assets" "/c/Dropbox/MultiGame/multiGame/butt2GoGameAssets"
	cmd /c "\
		set GAME_NAME=butt2GoGame&& \
		set DEBUG_MODE=0&& \
		set INTERNAL_MODE=0&& \
		set FAST_CLANG_MODE=1&& \
		set OPTIMIZED_MODE=1&& \
		buildSystem\$(WIN_BUILD_BAT).bat\
		"
	while ! rm -rf "/c/Dropbox/MultiGame/multiGame/butt2GoGameAssets"; \
	do \
	sleep 1; \
	done;
	
	cd /c/bin; \
		rm -rf pack; \
		mkdir pack; \
		cd pack; \
		cp /c/bin/*.exe .; \
		cp /c/bin/*.dll .; \
	magick convert /c/Dropbox/FallowCandy/Butt2Go/raw/icon.png /c/bin/icon.ico
	while ! cmd /c "rcedit c:/bin/pack/butt2GoGame.exe --set-icon C:/bin/icon.ico"; \
		do \
		sleep 1; \
		done;
	mv /c/bin/pack /c/bin/Butt2Go
	cd /c/bin/Butt2Go; \
		mv butt2GoGame.exe Butt2Go.exe; \
		cp -r /c/Dropbox/FallowCandy/Butt2Go/butt2GoGameAssets/assets .; \
		$(SEVEN_ZIP) a -tzip Butt2Go.zip ../Butt2Go; \
		cp Butt2Go.zip $(PARAPHORE_COM_PATH)/play/$(BUTT_2_GO_PUBLIC_DIR);
	
	$(MAKE) shipPlay

genButt2GoZips:
	-rm -rf /c/temp/dupZips
	mkdir /c/temp/dupZips
	cd /c/temp/dupZips; \
		cp -r $(PARAPHORE_COM_PATH)/play/$(BUTT_2_GO_PUBLIC_DIR)/* .; \
		cp /c/Dropbox/FallowCandy/Butt2Go/raw/html/rcIndex.html index.html; \
		cp /c/Dropbox/FallowCandy/Butt2Go/raw/html/rcMovie.html movie.html; \
		rm -f ../rcHtml.zip; \
		$(SEVEN_ZIP) a -tzip ../rcHtml.zip .; \
		cp /c/Dropbox/FallowCandy/Butt2Go/raw/html/ngIndex.html index.html; \
		rm -f Butt2Go.zip; \
		rm -f movie.html; \
		rm -f ../ngHtml.zip; \
		$(SEVEN_ZIP) a -tzip ../ngHtml.zip .

copyConcreteJungleUnitFrames:
	rm -rf /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets/frames
	cp -r /c/bin/frames/* /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets/frames

delConcreteJungleUnitFrames:
	find /c/Dropbox/concreteJungle/concreteJungleGameAssets/assets/frames -type f -name "*.png" -delete
