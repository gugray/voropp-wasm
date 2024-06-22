rm -rf dist
mkdir dist

EXPORTED_FUNCTIONS="\
  _malloc,_free,_calculate_voronoi,\
  _create_volume_tester,_delete_volume_tester,_is_point_inside
  "

emcc src/voro_wrap.cc src/buffer.cc src/volume_tester.cc \
  src/voropp/voro++.cc \
  -o dist/voropp-module.js \
  -O3 \
  -s EXPORTED_FUNCTIONS="$EXPORTED_FUNCTIONS" \
  -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
  -s TOTAL_MEMORY=1024MB \
  -s ENVIRONMENT="web" \
  -s EXPORT_ES6=1 -sMODULARIZE -s EXPORT_NAME="createVoroPP"

cp dist/voropp-module.* examples/
