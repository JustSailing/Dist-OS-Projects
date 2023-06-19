cmake -S . -B build
cmake --build build
echo "Finished Comiling and build"
cd build
echo "About to run ./server $1 $2 $3 $4"
./server $1 $2 $3 $4
