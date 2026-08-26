# Deploying the Live WebAssembly Demo

This folder turns the C++ console app into a **live, in-browser demo**. The program is
compiled to WebAssembly (WASM) and driven through an [xterm.js](https://xtermjs.org/)
terminal, so visitors run the *actual* C++ — data structures and all — with no install.

```
web/
├── main_web.cpp      # Browser entry point (reuses ../src logic; async line input)
├── index.html        # Terminal UI + glue (loads registration.js)
├── build.sh          # One command: compiles main_web.cpp -> registration.js/.wasm
├── .nojekyll         # Tells GitHub Pages to serve files as-is
└── DEPLOY.md         # You are here
```

> **Why a build step?** Browsers can't run C++ directly, so the source must be compiled to
> WebAssembly with **Emscripten**. You have two ways to do that — let CI do it (no local
> install), or build locally.

---

## Option A — GitHub Pages via Actions (recommended, no local tools)

A workflow at [`.github/workflows/deploy-pages.yml`](../.github/workflows/deploy-pages.yml)
installs Emscripten **in CI**, builds the WASM, and publishes `web/`. You never touch a
compiler.

1. Push this project to a GitHub repository.
2. In the repo: **Settings → Pages → Build and deployment → Source: GitHub Actions**.
3. Push to `main` (or run the workflow manually from the **Actions** tab).
4. When it finishes, your link appears in the run summary and under **Settings → Pages**, e.g.
   `https://<username>.github.io/<repo>/`.

That URL is your shareable preview link.

---

## Option B — Build locally, then host anywhere

**1. Install Emscripten (one time):**

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh        # run in each new terminal
cd ..
```

**2. Build:**

```bash
bash web/build.sh            # produces web/registration.js + web/registration.wasm
```

**3. Preview locally** (WASM needs HTTP — `file://` won't work):

```bash
cd web
python3 -m http.server 8080
# open http://localhost:8080/
```

**4. Publish the `web/` folder** to any static host:

- **Netlify (fastest):** drag the `web/` folder onto <https://app.netlify.com/drop> → instant public URL.
- **Vercel:** `npm i -g vercel && vercel deploy web --prod`
- **GitHub Pages (manual):** copy the built `web/` contents to the root of a `gh-pages` branch and enable Pages on that branch.
- **Cloudflare Pages / Surge / any static host:** point it at the `web/` directory.

---

## Add the link to your README

Once you have a URL, drop it near the top of `README.md`:

```markdown
## Live Demo

▶️ **[Try it in your browser](https://YOUR-URL-HERE)** — the C++ program compiled to
WebAssembly, running an interactive terminal. Choose `14` to load sample data.

[![Live Demo](https://img.shields.io/badge/live%20demo-online-brightgreen)](https://YOUR-URL-HERE)
```

To embed it inline on another page:

```html
<iframe src="https://YOUR-URL-HERE" width="100%" height="520" style="border:0"></iframe>
```

---

## Notes & troubleshooting

- **Must be served over HTTP(S).** Opening `index.html` from disk (`file://`) fails — browsers
  block WASM and cross-origin script loads there. Use `python3 -m http.server` or a host.
- **Interactivity** relies on Emscripten **Asyncify** (`-sASYNCIFY` in `build.sh`), which lets
  the blocking menu loop pause for input without freezing the page. No special COOP/COEP
  headers are required, so plain static hosts (including GitHub Pages) work.
- **CSV/JSON export** (menu options 12–13) writes to the in-browser virtual filesystem, so the
  file won't appear on disk — the program still confirms the write. Everything else behaves
  exactly like the native build.
- **xterm.js** is loaded from a CDN in `index.html`. To vendor it locally instead, download the
  `xterm` and `xterm-addon-fit` files and update the `<script>`/`<link>` tags.
- **Action version:** `mymindstorm/setup-emsdk@v14` is pinned in the workflow; bump it if a
  newer release is available.
