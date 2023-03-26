cd ..
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
# Creates a symlink to the assets directory because the program needs it to run
ln -s ../assets assets
