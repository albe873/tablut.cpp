import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const webRoot = path.resolve(__dirname, "..");
const src = path.join(webRoot, "node_modules", "coi-serviceworker", "coi-serviceworker.js");
const destDir = path.join(webRoot, "public");
const dest = path.join(destDir, "coi-serviceworker.js");

if (!fs.existsSync(src)) {
  console.error("coi-serviceworker not found. Did you run npm install?");
  process.exit(1);
}

fs.mkdirSync(destDir, { recursive: true });
fs.copyFileSync(src, dest);
console.log("Copied coi-serviceworker.js to public/");
