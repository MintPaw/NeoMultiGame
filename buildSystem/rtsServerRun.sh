command -v "7za"
if [[ "${?}" -ne 0 ]]; then
	sudo apt update
	sudo apt install p7zip-full libsdl2-dev clang libglew-dev libopenal-dev llvm gdb -y
fi

pkill -f rtsServer.py

# cd ~/rtsClientDistro
# cp ~/runPackage/rtsServerAssets/assets/__raw/rtsServer.py .
# python3 rtsServer.py > ~/pythonServerLogs.txt &

killall rtsServer
cd ~/runPackage
echo empty > rtsServerAssets/assets/log
rm -rf bin/rtsServer
make buildRtsServer &> rtsServerAssets/assets/log
ASAN_OPTIONS=symbolize=1 ./bin/rtsServer
# ASAN_OPTIONS=symbolize=1 ASAN_SYMBOLIZER_PATH=$(shell which llvm-symbolizer) ./main-clang

