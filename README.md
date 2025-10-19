# IMG2PDF  
Minimal Image to PDF Converter  


---

## 🚀 Overview
**IMG2PDF** is a lightweight, dependency-free C++ command-line tool that converts image files into valid single-page PDF documents.  
It’s designed for simplicity, portability, and educational clarity — showing how to construct a minimal PDF writer from scratch using only the C++ standard library and `stb_image`.

---

## ✅ Features
- 📄 Converts **JPEG images** directly into a valid PDF (embedded without re-encoding).  
- 🧠 Uses `stb_image` for image metadata (width, height, channels).  
- 🧱 Minimal and portable C++ code — no external PDF libraries required.  
- ⚡ Fast and lightweight (~a few hundred lines of code).  
- 🧰 Works on **Windows**, **Linux**, and **macOS**.

---

## 💻 Usage

```bash
IMG2PDF -i <input_image_path> -o <output_pdf_path>
```

### Example:
```bash
IMG2PDF -i photo.jpg -o photo.pdf
```

If the input image is invalid, the tool prints an error message and exits with a non-zero status code.

---

## 🛠️ Build Instructions

### 🔹 Windows (Visual Studio)
1. Open the solution file `IMG2PDF.sln`.
2. Select **Release** or **Debug** mode.
3. Build the project (Ctrl + Shift + B).
4. Run the generated `IMG2PDF.exe`.

### 🔹 Linux / macOS (GCC or Clang)
```bash
g++ -std=c++17 main.cpp img_pdf_writer.cpp -o img2pdf
```
Ensure that `stb_image.h` is in the include path.

---

## ⚙️ How It Works
1. The tool uses `stbi_info()` from **stb_image** to read image metadata (width, height, color channels).  
2. For **JPEG files**, it reads the raw binary data directly.  
3. The program writes a simple PDF structure consisting of:
   - Catalog and Page tree  
   - Image XObject (using `/Filter /DCTDecode` for JPEG)  
   - Content stream referencing the image  
   - Cross-reference table and trailer  
4. The result is a valid, fully viewable single-page PDF.

---

## 🧩 Example Output Structure
A simplified version of the generated PDF structure:

```
%PDF-1.4
1 0 obj <</Type /Catalog /Pages 2 0 R>>
2 0 obj <</Type /Pages /Kids [3 0 R] /Count 1>>
3 0 obj <</Type /Page /Parent 2 0 R /Resources << /XObject << /Im0 4 0 R >> >> /MediaBox [0 0 W H] /Contents 5 0 R>>
4 0 obj <</Type /XObject /Subtype /Image /Width W /Height H /ColorSpace /DeviceRGB /BitsPerComponent 8 /Filter /DCTDecode /Length N>>
stream
(binary JPEG data)
endstream
endobj
...
xref
...
%%EOF
```

---

## 🧠 Future Improvements
- [ ] Add **PNG support** (decode + embed raw pixels or recompress to JPEG).  
- [ ] Support **multiple images per PDF** (multi-page).  
- [ ] Add PDF **metadata** (title, author, creation date).  
- [ ] Improve **error handling** and **logging**.  

---

## 🧪 Testing
After generating a PDF, open it in any standard viewer such as:
- Adobe Acrobat Reader  
- macOS Preview  
- Evince / Okular (Linux)

To inspect technical details:
```bash
pdfinfo output.pdf
```

---

## 📄 License
```
MIT License

Copyright (c) 2025 Ramez Essam
```

---

## 🙏 Acknowledgements
- **stb_image.h** by *Sean Barrett* — used for reading image metadata.  
- The **PDF Reference (ISO 32000-1)** — for guidance on minimal PDF structure.  

---

## 🧰 Repository
👉 [github.com/RamezEssam/IMG2PDF](https://github.com/RamezEssam/IMG2PDF)
