<div align="center">
  <img src="res/tilde.svg" width="128" height="128" alt="Tilde Logo" />
  <h1>Tilde</h1>
  <p><b>A native C++ Markdown & LaTeX editor built on Qt 6 & KDE KatePart</b></p>
  
  [![Qt 6](https://img.shields.io/badge/Qt-6.x-41CD52?style=flat-square&logo=qt&logoColor=white)](https://www.qt.io/)
  [![KDE KatePart](https://img.shields.io/badge/Engine-KDE_KatePart-1D99F3?style=flat-square&logo=kde&logoColor=white)](https://api.kde.org/frameworks/ktexteditor/html/)

</div>

---

## Why Tilde?

Modern document editors are often bloated Electron applications consuming hundreds of megabytes of RAM just to render text by using Chromium or other web-based renders. **Tilde** is built natively in C++ with Qt 6 and the KDE KatePart (`KTextEditor`) engine used in KDevelop and Kate, giving you full IDE capabilities without skyrocketing your RAM and CPU usage.

Whether you are drafting technical notes, repo README.md's in Markdown or writing complex scientific papers in LaTeX, Tilde provides a distraction-free editing experience with live visual previews and instant PDF compilation.

---

## Key Features

* **Dual Rendering**:
  * **Markdown Live Preview**: Instant live rendering of headers, lists, code blocks, tables, and formatting via Qt's native text engine.
  * **Asynchronous LaTeX Preview**: Real-time background compilation using `pdflatex` + `pdftoppm`.
  * **Scroll sync with preview**: Scroll on both the editor and preview and the same time.

* **PDF Export**:
  * Export clean, paginated, publication-ready **PDFs directly from Markdown**.
  * Export fully compiled **LaTeX documents to PDF** seamlessly.

* **Native Desktop Integration**:
  * Command-line argument support (`tilde <filename>` or `tilde %U` from your file manager).
  * Exclusive, conflict-free keyboard shortcuts across all actions.

## Building & Installation

### Prerequisites (`Linux / KDE Plasma`)
Make sure you have Qt 6, KTextEditor (`KF6`), and CMake installed alongside a C++17 compiler (`GCC/Clang`):

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

# 3. Build
cmake --build build -j$(nproc)

# 4. Run Tilde
./build/tilde
```

---

## TODO
* Add support for Markdown image tags from URLs

## License

This project is licensed under the **GNU General Public License Version 3 (GPL-3.0)**. 
