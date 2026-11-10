# ROADWAY TO HEAVEN v1.0

Permainan balapan arcade tanpa berhenti (*endless racer*) berbasis Command Line Interface (CLI) menggunakan bahasa C++ dan pustaka grafik terminal `ncurses`.

## Description

ROADWAY TO HEAVEN v1.0 adalah aplikasi game konsol interaktif real-time yang dikembangkan sebagai proyek akhir Mata Kuliah Dasar Pemrograman. Program ini mengintegrasikan pemrosesan grafik konsol *non-blocking* via pustaka `ncurses`, rintangan pintar (*Swerving AI*), akumulasi kecepatan desimal (*sub-pixel movement*), sistem perisai/koin, serta fitur pengolahan data lengkap (CRUD Leaderboard), pengurutan skor berbasis `stable_sort`, analisis statistik matematis, dan pengarsipan berkas permanen (`riwayatData.txt`).

## Getting Started

### Dependencies

- **Kompilator**: `g++` (Mendukung standar C++11 atau versi lebih baru).
- **Pustaka Visual**: `ncurses` (Linux / WSL) atau `PDCurses` (Windows Native).
- **Sistem Operasi**: 
  - Arch Linux / Manjaro
  - Ubuntu / Debian / WSL (Windows Subsystem for Linux)
  - Windows 10 / 11 (via WSL atau MSYS2/MinGW)

### Installing

Pilih langkah instalasi pustaka dependensi sesuai dengan Sistem Operasi yang Anda gunakan:

#### 1. Arch Linux / Manjaro
Buka terminal dan instal paket `base-devel` serta `ncurses` menggunakan `pacman`:
```bash
sudo pacman -Syu
sudo pacman -S base-devel ncurses gcc
```

#### 2. Ubuntu / Debian / WSL
Jika menggunakan Ubuntu, Debian, atau Windows Subsystem for Linux (WSL), jalankan:
```bash
sudo apt update
sudo apt install build-essential libncurses5-dev libncursesw5-dev -y
```

#### 3. Windows (Metode 1: WSL - Sangat Direkomendasikan)
Karena pustaka `ncurses` dirancang natively untuk sistem bertipe POSIX/Linux, metode termudah di Windows adalah menggunakan **WSL**:
1. Buka PowerShell/Command Prompt sebagai Administrator, lalu jalankan:
   ```cmd
   wsl --install
   ```
2. Restart komputer Anda dan buka aplikasi **Ubuntu** yang terpasang.
3. Jalankan perintah instalasi dependensi Ubuntu di atas.

#### 4. Windows (Metode 2: Native MSYS2 / MinGW)
Jika ingin menjalankan secara native tanpa WSL:
1. Unduh dan instal **[MSYS2](https://www.msys2.org/)**.
2. Buka terminal **MSYS2 MinGW 64-bit** dan pasang kompilator serta pustaka PDCurses/NCurses:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-ncurses mingw-w64-x86_64-pdcurses
   ```

---

### Executing program

Langkah-langkah kompilasi dan menjalankan program di terminal:

1. Buka terminal dan masuk ke direktori tempat berkas proyek disimpan:
   ```bash
   cd ~/path/ke/folder/proyek
   ```

2. Kompilasi seluruh berkas C++ modular dengan me-link pustaka `ncurses`:
   
   - **Di Linux (Arch / Ubuntu) & Windows WSL:**
     ```bash
     g++ main.cpp gameengine.cpp datamanager.cpp io.cpp -o mobil -lncurses
     ```
   
   - **Di Windows Native (MSYS2 / MinGW dengan PDCurses):**
     ```cmd
     g++ main.cpp gameengine.cpp datamanager.cpp io.cpp -o mobil.exe -lpdcurses
     ```

3. Jalankan eksekutabel aplikasi:
   - **Linux / WSL:**
     ```bash
     ./mobil
     ```
   - **Windows CMD / PowerShell:**
     ```cmd
     .\mobil.exe
     ```

## Help

- **Ukuran Layar Terminal Terlalu Kecil:**  
  Jika muncul pesan `[!] Terminal terlalu kecil untuk mode balapan`, perbesar atau maksimalkan (*maximize*) jendela terminal hingga memenuhi resolusi minimal (lebar > 50 karakter, tinggi > 26 baris).
- **Error Header `<ncurses.h>` Tidak Ditemukan:**  
  - Pada Arch Linux: Pastikan paket `ncurses` sudah terpasang via `sudo pacman -S ncurses`.
  - Pada Ubuntu/WSL: Pastikan paket dev sudah terinstal via `sudo apt install libncurses5-dev`.
- **Karakter Visual Berantakan di Windows Console:**  
  Jika menggunakan Windows CMD biasa, gunakan terminal modern seperti **Windows Terminal** atau jalankan via terminal WSL/MSYS2 agar dukungan karakter ASCII/Warna berjalan dengan benar.

## Authors

Anggota Kelompok 1 — Program Studi Teknik Informatika, Universitas Tadulako:

- **Andi Arafah Islami Pasha** (NIM: F5512620079) - *Main File C++ (`main.cpp`) & Alur Menu Utama*
- **Rafi Az Zarqi Athillah** (NIM: F5512620064) - *PJ Game Engine Single Player (`gameengine.cpp`)*
- **Moh. Fadlan Faidurrahman Irham** (NIM: F5512620065) - *PJ Game Engine Multiplayer / Duel Race (`gameengine.cpp`)*
- **Muh. Ghifary Sintae** (NIM: F5512620060) - *PJ Leaderboard & CRUD Data (`datamanager.cpp`)*
- **Mohammad Faiq Faizan Badja** (NIM: F5512620081) - *PJ Algorithm Sorting (`stable_sort`) & Statistik (`datamanager.cpp`)*
- :Moh. Ardi Pradista (NIM: F5512620052) - PJ File I/O (`io.cpp`) & Header Structure (`header.h`)
