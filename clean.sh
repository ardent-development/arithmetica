# clean.sh: Removes build artifacts
# Run from within src to clean

rm -rfv CMakeFiles
rm -rfv generated
rm -rfv pico-sdk
rm -rfv pioasm
rm -rfv pioasm-install
rm -rfv cmake_*
rm -rfv CMakeCache.txt
rm -rfv Makefile
rm -rfv arithmetica_*
rm -rfv pico_*
rm -rfv src/CMakeDoxyfile.in
rm -rfv src/CMakeDoxygenDefaults.cmake