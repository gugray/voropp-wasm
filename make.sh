rm -rf dist
mkdir dist

EXPORTED_FUNCTIONS="\
  _malloc,_free,\
  _create_container,_delete_container,\
  _container_point_inside,_container_put,_container_add_wall,\
  _create_wall_plane,_delete_wall"

emcc src/voropp.cc src/voropp/voro++.cc \
  -o dist/voropp-module.js \
  -O3 \
  -s EXPORTED_FUNCTIONS="$EXPORTED_FUNCTIONS" \
  -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
  -s TOTAL_MEMORY=1024MB \
  -s EXPORT_ES6=1 -sMODULARIZE -s EXPORT_NAME="createVoroPP"

cp dist/voropp-module.* examples/
