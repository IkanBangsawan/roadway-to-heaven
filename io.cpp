#include "header.h"

// Menyimpan data ke file teks dengan kolom Difficulty
void saveData() {
  ofstream file("riwayatData.txt");
  if (!file) {
    cout << "[!] Gagal membuka file 'riwayatData.txt' untuk menyimpan.\n";
    return;
  }

  for (const auto &rec : records) {
    file << rec.id << '|' << rec.name << '|' << rec.score << '|' << rec.mode
         << '|' << rec.difficulty << '|' << rec.timestamp << '\n';
  }
  cout << "\n[+] " << records.size()
       << " data berhasil disimpan ke 'riwayatData.txt'.\n";
}

// Membaca data dari file teks
void loadData() {
  ifstream file("riwayatData.txt");
  if (!file) {
    return;
  }

  records.clear();
  string line;
  int maxLoadedId = 0;

  while (getline(file, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string idStr, name, scoreStr, mode, diff, timestamp;

    if (getline(ss, idStr, '|') && getline(ss, name, '|') &&
        getline(ss, scoreStr, '|') && getline(ss, mode, '|') &&
        getline(ss, diff, '|') && getline(ss, timestamp)) {

      try {
        skorPemain rec;
        rec.id = stoi(idStr);
        rec.name = name;
        rec.score = stoi(scoreStr);
        rec.mode = mode;
        rec.difficulty = diff;
        rec.timestamp = timestamp;

        records.push_back(rec);
        if (rec.id > maxLoadedId) {
          maxLoadedId = rec.id;
        }
      } catch (...) {
      }
    }
  }
  nextId = maxLoadedId + 1;
}
