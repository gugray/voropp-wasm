import createVoroPP from "./voropp-module.js"
const mod = await createVoroPP();

const rand = Math.random;
const log = false;

const wallA = 1;
const wallB = 0.4;
const wallD = 0.4;

const walls = [
  [wallA, wallB, 0, wallD],
  [wallA, -wallB, 0, wallD],
  [-wallA, -wallB, 0, wallD],
  [-wallA, wallB, 0, wallD],
  [0, wallB, wallA, wallD],
  [0, -wallB, wallA, wallD],
  [0, -wallB, -wallA, wallD],
  [0, wallB, -wallA, wallD],
];

const points = [
  [-0.15, 0.00, 0.87],
  [-0.19, -0.23, 0.15],
  [-0.39, 0.01, -0.51],
  [0.11, -0.00, -0.98],
  [0.34, 0.08, 0.05],
];

function genPoints(nPoints) {
  points.length = 0;
  for (let i = 0; i < nPoints; ++i) {
    const x = -1 + 2 * rand();
    const z = -1 + 2 * rand();
    let y = rand();
    y = Math.pow(y, 3);
    if (rand() < 0.5) y *= -1;
    y *= 0.5;
    points.push([x, y, z]);
  }
}

genPoints(6400);

const t0 = performance.now();

const szInput =
  6 + // Space bounds
  1 + walls.length * 4 + // Wall planes
  1 + points.length * 4; // Particles

const pInput = mod._malloc(szInput * 8);
const input = new Float64Array(mod.HEAPU8.buffer, pInput, szInput);
let ip = 0;
input[ip++] = -1;
input[ip++] = 1;
input[ip++] = -1;
input[ip++] = 1;
input[ip++] = -1;
input[ip++] = 1;

input[ip++] = walls.length;
for (let i = 0; i < walls.length; ++i) {
  input[ip++] = walls[i][0];
  input[ip++] = walls[i][1];
  input[ip++] = walls[i][2];
  input[ip++] = walls[i][3];
}

input[ip++] = points.length;
for (let i = 0; i < points.length; ++i) {
  input[ip++] = i;
  input[ip++] = points[i][0];
  input[ip++] = points[i][1];
  input[ip++] = points[i][2];
}

let strOutput = "";

const pRes = mod._calculate_voronoi(pInput);
const dummyArr = new Float64Array(mod.HEAPU8.buffer, pRes, 1);
const resSize = dummyArr[0];
const resArr = new Float64Array(mod.HEAPU8.buffer, pRes, resSize);
let nCells = resArr[1];
let pos = 2;

for (let cix = 0; cix < nCells; ++cix) {

  const id = resArr[pos++];
  const px = resArr[pos++];
  const py = resArr[pos++];
  const pz = resArr[pos++];
  if (log) console.log(`ID: ${id} (${px.toFixed(2)}, ${py.toFixed(2)}, ${pz.toFixed(2)})`);
  let strItem = `ID ${id} POS (${px},${py},${pz})`;

  const vol = resArr[pos++];

  let nVerts = resArr[pos++];
  if (log) console.log(`Vertices: ${nVerts}`);
  strItem += ` VERTS ${nVerts}`;

  for (let i = 0; i < nVerts; ++i) {
    const vx = resArr[pos++];
    const vy = resArr[pos++];
    const vz = resArr[pos++];
    if (log) console.log(`Vertex: (${vx.toFixed(2)}, ${vy.toFixed(2)}, ${vz.toFixed(2)})`);
    strItem += ` (${vx},${vy},${vz})`;
  }

  strItem += " EDGES 0";

  let nFaces = resArr[pos++];
  if (log) console.log(`Faces: ${nFaces}`);
  strItem += ` FACES ${nFaces}`;

  while (nFaces > 0) {
    --nFaces;
    let nVertsInFace = resArr[pos++];
    if (log) console.log(`Vertices in face: ${nVertsInFace}`);
    const faceVerts = [];
    while (nVertsInFace > 0) {
      --nVertsInFace;
      faceVerts.push(resArr[pos++]);
    }
    if (log) console.log(`Namely: ${faceVerts}`);
    strItem += " (";
    for (let i = 0; i < faceVerts.length; ++i) {
      if (i != 0) strItem += ",";
      strItem += `${faceVerts[i]}`;
    }
    strItem += ")"
  }

  strOutput += strItem + "\n";
}

mod._free(pRes);
mod._free(pInput);

const t1 = performance.now();

console.log(`Calculated result for ${points.length} particles, giving ${nCells} Voronoi cells: ${t1-t0} msec`);

// console.log(strOutput);

