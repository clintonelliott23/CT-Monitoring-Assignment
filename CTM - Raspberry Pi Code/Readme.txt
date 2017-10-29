To compile and run:

cd build
cmake ..
make
./main /dev/ttyUSB0



By running cmake from inside the "build" directory, all the compiled output is kept
separate from the source code. You can easily clean up by simply deleting the build
folder. The argument to cmake is the folder containing "CMakeLists.txt".

