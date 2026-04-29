import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const webRoot = path.resolve(__dirname, "..");
const repoRoot = path.resolve(webRoot, "..");
const srcDir = path.join(repoRoot, "build-wasm", "wasm");
const destDir = path.join(webRoot, "public", "wasm");

if (!fs.existsSync(srcDir)) {
  console.warn("WASM build output not found. Run the wasm build first.");
  process.exit(0);
}

fs.mkdirSync(destDir, { recursive: true });

const entries = fs.readdirSync(srcDir).filter((name) => name.startsWith("tablut_wasm."));
if (entries.length === 0) {
  console.warn("No tablut_wasm.* files found in build-wasm/wasm.");
  process.exit(0);
}

for (const name of entries) {
  fs.copyFileSync(path.join(srcDir, name), path.join(destDir, name));
}

console.log("Copied wasm assets to public/wasm:", entries.join(", "));
