import createVoroPP from "./voropp-module.js"
const mod = await createVoroPP();

const nItems = 17_000_000;

// Allocate on Emscripten heap
const nDataBytes = nItems * 8;
const dataPtr = mod._malloc(nDataBytes);

// Erect Float64 array over newly allocated area
const arr = new Float64Array(mod.HEAPU8.buffer, dataPtr, nItems);
for (let i = 0; i < nItems; ++i)
  arr[i] = i + 0.5;

// Call function and get result
const sum = mod._array_sum(dataPtr, nItems);

// Free heap
mod._free(dataPtr);

console.log(sum);
