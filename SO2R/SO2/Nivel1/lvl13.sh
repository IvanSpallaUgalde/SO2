echo "$ rm disco"
make clean
make
rm disco
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000

echo "$ ./simulacion disco"
./simulacion disco