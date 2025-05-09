# Compile the C++ files
g++ src/*.cpp -o dstardb -lws2_32

# Run the executable with the --persist flag
./dstardb.exe --persist=true
