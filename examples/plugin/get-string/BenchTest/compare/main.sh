echo "time of wasm" >> compare.txt
{ time ./compare.sh >> compare.txt;} 2>> compare.txt

g++ -o compare compare.cpp
printf "\n\n"
echo "time of native cpp" >> compare.txt
{ time a=$(./compare 1423.111aaasihd 2455.2bbb );} 2>> compare.txt