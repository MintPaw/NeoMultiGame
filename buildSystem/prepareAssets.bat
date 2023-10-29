set ASSETS_DIR=%PROJECT_DIR%\%GAME_NAME%Assets

if "%GAME_NAME%" == "horseGame" (
	set ASSETS_DIR=C:/Dropbox/FallowCandy/HorseProjects/horseGameAssets
)
if "%GAME_NAME%" == "catCardGame" (
	set ASSETS_DIR=C:/Dropbox/CasinoCatChaos/runTree
)
if "%GAME_NAME%" == "boxingGame" (
	set ASSETS_DIR=C:/Dropbox/KoBoxing/runTree
)
if "%GAME_NAME%" == "gladiators2Game" (
	set ASSETS_DIR=C:/Dropbox/Gladiators/runTree
)
if "%GAME_NAME%" == "catAnimGame" (
	set ASSETS_DIR=C:/Dropbox/CatDoingGaySex/runTree
)
if "%GAME_NAME%" == "deskGame" (
	set ASSETS_DIR=C:/Dropbox/MultiGame/multiGamePrivate/deskGameAssets
)
if "%GAME_NAME%" == "pkGame" (
	set ASSETS_DIR=C:/Dropbox/MultiGame/multiGamePrivate/pkGameAssets
)

robocopy commonAssets\assets\common %ASSETS_DIR%\assets\common /MIR /W:5 /njh /njs /ndl /nc /ns /XO
