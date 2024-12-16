import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";

export default defineConfig({
  plugins: [vue()],
  build: {
    rollupOptions: {
      input: {
        app: "index.html",
        scoreboard: "src/lib/scoreboard.ts",
      },
      output: [
        {
          format: "es",
          dir: "../data",
          entryFileNames: (chunk) => {
            if (chunk.name === "scoreboard") return "lib/[name].js";
            return "[name].js";
          },
          assetFileNames: "[name][extname]",
        },
      ],
      external: ["./dist/lib/scoreboard.js"],
    },
  },
});
