import createVoroPP from "./voropp-module.js"
const mod = await createVoroPP();

const pCon = mod._create_container(-1, 1, -1, 1, -1, 1,
  3, 3, 3, false, false, false, 8);

const walls = [
  mod._create_wall_plane(1, 1, 1, 1),
  mod._create_wall_plane(-1, -1, 1, 1),
  mod._create_wall_plane(1, -1, -1, 1),
  mod._create_wall_plane(-1, 1, -1, 1),
];
walls.forEach(pw => mod._container_add_wall(pCon, pw));

for (let i = 0; i < 64; ++i) {
  const x = -1 + 3 * Math.random();
  const y = -1 + 2 * Math.random();
  const z = -1 + 2 * Math.random();
  if (mod._container_point_inside(pCon, x, y, z)) {
    mod._container_put(i, x, y, z);
  }
  else console.log("burp");
}

walls.forEach(pw => mod._delete_wall(pw));
mod._delete_container(pCon);

// const nItems = 17_000_000;
// // Allocate on Emscripten heap
// const nDataBytes = nItems * 8;
// const dataPtr = mod._malloc(nDataBytes);
// // Erect Float64 array over newly allocated area
// const arr = new Float64Array(mod.HEAPU8.buffer, dataPtr, nItems);
// for (let i = 0; i < nItems; ++i)
//   arr[i] = i + 0.5;
// // Call function and get result
// const sum = mod._array_sum(dataPtr, nItems);
// // Free heap
// mod._free(dataPtr);
// console.log(sum);
