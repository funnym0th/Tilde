<div align="center">
  <img src="res/tilde.svg" width="128" height="128" alt="Tilde Logo" />
  <h1>Tilde</h1>
  <p><b>A lightning-fast, native C++ Markdown & LaTeX editor built on Qt 6 & KDE KatePart</b></p>
  
  [![Qt 6](https://img.shields.io/badge/Qt-6.x-41CD52?style=flat-square&logo=qt&logoColor=white)](https://www.qt.io/)
  [![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/20)
  [![KDE KatePart](https://img.shields.io/badge/Engine-KDE_KatePart-1D99F3?style=flat-square&logo=kde&logoColor=white)](https://api.kde.org/frameworks/ktexteditor/html/)
  [![Memory](https://img.shields.io/badge/RAM-~50_MB-brightgreen?style=flat-square)](https://github.com/funnym0th/Tilde)
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://opensource.org/licenses/MIT)
</div>

---

## ⚡ Why Tilde?

Modern document editors are often bloated Electron applications consuming hundreds of megabytes of RAM just to render text. **Tilde** is different. Built natively in **C++20** with **Qt 6** and the industrial-grade **KDE KatePart (`KTextEditor`)** engine, Tilde gives you full IDE capabilities while resting at roughly **~50 MB of RAM** and near **0% idle CPU usage**.

Whether you are drafting technical notes in Markdown or writing complex scientific papers in LaTeX, Tilde provides a seamless, distraction-free editing experience with live visual previews and instant PDF compilation.

---

## ✨ Key Features

* **🚀 Industrial-Grade Editor (`KDE KatePart`)**:
  * Full **Language Server Protocol (`LSP`)** support out of the box (`auto-completion, diagnostics, and jump-to-definition`).
  * Built-in **Vim / Vi modal editing** support (`enable via Kate editor preferences`).
  * Multi-cursor support, minimap, line numbering, and syntax highlighting for over 300 languages.
  * Native find & replace with regular expressions.

* **📐 Debounced Dual Rendering Engine**:
  * **Markdown Live Preview**: Instant live rendering of headers, lists, code blocks, tables, and formatting via Qt's native text engine as you type (`Ctrl+P`).
  * **Asynchronous LaTeX Preview**: Real-time background compilation using `pdflatex` + `pdftoppm`. Debounced input timers ensure zero UI freezes or lag even while editing heavy mathematical equations (`$E=mc^2$`).

* **📄 One-Click Dual PDF Export (`Ctrl+E`)**:
  * Export clean, paginated, publication-ready **PDFs directly from Markdown**.
  * Export fully compiled **LaTeX documents to PDF** seamlessly.

* **🖥️ Native Desktop Integration**:
  * Full command-line argument support (`tilde <filename>` or `tilde %U` from your file manager).
  * Custom 3D mechanical keycap vector branding (`res/tilde.svg`).
  * Exclusive, conflict-free keyboard shortcuts across all actions.

---

## ⌨️ Keyboard Shortcuts

| Shortcut | Action |
| :--- | :--- |
| **`Ctrl + N`** | Create a new blank document |
| **`Ctrl + O`** | Open an existing document from disk |
| **`Ctrl + S`** | Save the current document |
| **`Ctrl + Shift + S`** | Save As new file |
| **`Ctrl + P`** | Toggle Show Live Preview (`Markdown / LaTeX`) |
| **`Ctrl + E`** | Export current document to PDF |
| **`Ctrl + Q`** | Quit application |

---

## 🛠️ Building & Installation

### Prerequisites (`Linux / KDE Plasma`)

Make sure you have Qt 6, KTextEditor (`KF6`), and CMake installed alongside a C++20 compiler (`GCC/Clang`):

#### **Arch Linux / Manjaro**
```bash
sudo pacman -S base-devel cmake qt6-base qt6-tools ktexteditor kparts poppler texlive-bin
```

#### **Fedora**
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qttools-devel kf6-ktexteditor-devel kf6-kparts-devel poppler-utils texlive-scheme-basic
```

#### **Ubuntu / Debian (`KF6 / Qt6`)**
```bash
sudo apt update && sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev libkf6texteditor-dev libkf6parts-dev poppler-utils texlive-latex-base
```

### Compiling from Source

```bash
# 1. Clone the repository
git clone https://github.com/funnym0th/Tilde.git
cd Tilde

# 2. Configure with CMake
cmake -B build -S .

# 3. Build natively (`using all CPU cores`)
cmake --build build -j$(nproc)

# 4. Run Tilde
./build/tilde
```

---

## 📂 Example Documents

Check out the included `examples/` directory for sample test files:
* **`examples/benchmark.tex`**: A rich LaTeX document with mathematical equations, custom formatting, and physics formulas to test the debounced live rendering engine.

---

## 📜 License

This project is licensed under the **MIT License**. Feel free to inspect, modify, and distribute!
