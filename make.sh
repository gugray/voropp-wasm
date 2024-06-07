rm -rf dist
mkdir dist

emcc -O3 src/voropp.cc -o dist/voropp-module.js \
  -sEXPORTED_FUNCTIONS=_array_sum,_malloc,_free \
  -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
  -s TOTAL_MEMORY=1024MB \
  -s EXPORT_ES6=1 -sMODULARIZE -s EXPORT_NAME="createVoroPP"

cp dist/voropp-module.* examples/
