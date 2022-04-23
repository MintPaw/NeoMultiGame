command -v "7za"
if [[ "${?}" -ne 0 ]]; then
	sudo apt update
	sudo apt install make p7zip-full libsdl2-dev clang libglew-dev libopenal-dev llvm gdb -y
fi

killall bulletHellGameServer
# rm -rf bulletHellGameServer
# mkdir bulletHellGameServer

# 7z x bulletHellGameServer.7z -obulletHellGameServer
cd bulletHellGameServer
make buildBulletHellGameServer
ASAN_OPTIONS=symbolize=1 ./bin/bulletHellGameServer
# ASAN_OPTIONS=symbolize=1 ASAN_SYMBOLIZER_PATH=$(shell which llvm-symbolizer) ./main-clang

