cmake -G "MinGW Makefiles" -B build-windows -S . ^
   -DCMAKE_BUILD_TYPE=Debug ^
   -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
   -DCMAKE_PREFIX_PATH="D:\projects\CPP\musiqer\vendor\Qt6-mingw"
