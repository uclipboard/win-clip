set -e
rm -f ./*.exe
g++ -std=c++20 -O3 win-clip.cpp -o win-clip.exe
strip win-clip.exe 
ln -s win-clip.exe win-paste.exe
ln -s win-clip.exe win-copy.exe