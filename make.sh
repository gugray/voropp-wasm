rm -rf dist
mkdir dist

EXPORTED_FUNCTIONS="\
  _malloc,_free,_calculate_voronoi"

emcc src/voro_wrap.cc src/buffer.cc \
  src/voropp/voro++.cc \
  -o dist/voropp-module.js \
  -O3 \
  -s EXPORTED_FUNCTIONS="$EXPORTED_FUNCTIONS" \
  -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
  -s TOTAL_MEMORY=1024MB \
  -s EXPORT_ES6=1 -sMODULARIZE -s EXPORT_NAME="createVoroPP"

cp dist/voropp-module.* examples/
