import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";
import { minify } from 'terser';

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

async function main() {
  const source = fs.readFileSync(src, "utf8");
  const result = await minify(source, {
    compress: {
      drop_console: false,
      drop_debugger: true,
    },
    format: {
      comments: false,
    },
  });

  fs.mkdirSync(destDir, { recursive: true });
  fs.writeFileSync(dest, result.code);
  console.log("Copied and minified coi-serviceworker.js to public/");
}

main().catch((error) => {
  console.error("Failed to copy coi-serviceworker.js:", error);
  process.exit(1);
});
