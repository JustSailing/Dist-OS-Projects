cmake -S . -B build
cmake --build build
echo "Finished Comiling and build"
cd build
echo "About to run ./Client $1 $2 $3 $4"
./Client $1 $2 $3 $4

