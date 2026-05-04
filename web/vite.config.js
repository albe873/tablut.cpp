import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

const isPages = process.env.GITHUB_PAGES === "1";

export default defineConfig({
  plugins: [react()],
  base: isPages ? "/tablut.cpp/" : "/",
  server: {
    /*
     * COOP/COEP headers needed to load the AI worker's WASM module in a cross-origin isolated context.
     * these headers are needed to be set in the server
     * if not possible, there is the workaround coi-serviceworker.js that can be registered in the main thread to set the headers for WASM requests.
     * github: https://github.com/gzuidhof/coi-serviceworker
    /*
    /*headers: {
      "Cross-Origin-Opener-Policy": "same-origin",
      "Cross-Origin-Embedder-Policy": "require-corp",
    },*/
  },
  preview: {
    /*headers: {
      "Cross-Origin-Opener-Policy": "same-origin",
      "Cross-Origin-Embedder-Policy": "require-corp",
    },*/
  },
  build: {
    minify: "terser",
    terserOptions: {
      compress: {
        drop_console: false,
        drop_debugger: true,
      },
      format: {
        comments: false,
      },
    },
    rollupOptions: {
      output: {
        manualChunks(id) {
          if (id.includes("node_modules/react") || id.includes("node_modules/react-dom")) {
            return "react";
          }
        },
        chunkFileNames: "js/[name]-[hash].js",
        entryFileNames: "js/[name]-[hash].js",
        assetFileNames: "[ext]/[name]-[hash].[ext]",
      },
    },
    cssCodeSplit: true,
    sourcemap: false,
  },
});
