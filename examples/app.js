import createVoroPP from "./voropp-module.js"
const mod = await createVoroPP();

const nInitMem = 8;
const nExpectedParticles = 256;

// const nGridSize = Math.ceil(Math.pow(nExpectedParticles / nInitMem, 1/3) + 2);
const nGridSize = 3;
console.log(nGridSize);

const t0 = performance.now();

const pCon = mod._create_container(-1, 1, -1, 1, -1, 1,
  nGridSize, nGridSize, nGridSize, nInitMem);

const walls = [
  mod._create_wall_plane(1, 1, 1, 1),
  mod._create_wall_plane(-1, -1, 1, 1),
  mod._create_wall_plane(1, -1, -1, 1),
  mod._create_wall_plane(-1, 1, -1, 1),
];
walls.forEach(pw => mod._container_add_wall(pCon, pw));

let nActualParticles = 0;
for (let i = 0; i < nExpectedParticles; ++i) {
  const x = -1 + 2 * Math.random();
  const y = -1 + 2 * Math.random();
  const z = -1 + 2 * Math.random();
  if (mod._container_point_inside(pCon, x, y, z)) {
    // console.log(`ID: ${i} (${x.toFixed(2)}, ${y.toFixed(2)}, ${z.toFixed(2)})`);
    mod._container_put(pCon, i, x, y, z);
    ++nActualParticles;
  }
}

const pRes = mod._container_compute(pCon);
const dummyArr = new Float64Array(mod.HEAPU8.buffer, pRes, 1);
const resSize = dummyArr[0];
const resArr = new Float64Array(mod.HEAPU8.buffer, pRes, resSize);
let nCells = resArr[1];
let pos = 2;
while (nCells > 0) {
  --nCells;
  const id = resArr[pos++];
  const px = resArr[pos++];
  const py = resArr[pos++];
  const pz = resArr[pos++];
  // console.log(`ID: ${id} (${px.toFixed(2)}, ${py.toFixed(2)}, ${pz.toFixed(2)})`);
  let nVerts = resArr[pos++];
  while (nVerts > 0) {
    --nVerts;
    const vx = resArr[pos++];
    const vy = resArr[pos++];
    const vz = resArr[pos++];
    // console.log(`Vertex: (${vx.toFixed(2)}, ${vy.toFixed(2)}, ${vz.toFixed(2)})`);
  }

  let nFaces = resArr[pos++];
  // console.log(`Faces: ${nFaces}`);
  while (nFaces > 0) {
    --nFaces;
    let nVertsInFace = resArr[pos++];
    // console.log(`Vertices in face: ${nVertsInFace}`);
    const faceVerts = [];
    while (nVertsInFace > 0) {
      --nVertsInFace;
      faceVerts.push(resArr[pos++]);
    }
    // console.log(`Namely: ${faceVerts}`);
  }
}
mod._free(pRes);

walls.forEach(pw => mod._delete_wall(pw));
mod._delete_container(pCon);

const t1 = performance.now();

console.log(`Calculated Voronoi cells for ${nActualParticles} actual particles: ${t1-t0} msec`);

