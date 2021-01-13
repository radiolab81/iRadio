sudo apt-get install libsdl2-*
sudo apt-get install libiw*
g++ tube.cxx -o magicEyed -lSDL2 -lSDL2_image -lSDL2_ttf
g++ -O3 -Wall signal.cxx -o signal -liw

