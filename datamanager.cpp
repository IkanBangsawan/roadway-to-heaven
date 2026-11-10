#include "header.h"

#include <cstdlib>
#include <ctime>
#include <ncurses.h> 
#include <unistd.h>
#include <vector>

// Awal pencarian index data berdasarkan ID.
int cariIndexData(int id) {
  for (size_t i = 0; i < records.size(); i++) {
    if (records[i].id == id) {
      return static_cast<int>(i);
    }
  }
  return -1;
}
// Akhir pencarian index data berdasarkan ID.

namespace {
bool skorTertinggiDulu = true;

// Awal penambahan kategori jika belum tersedia.
void tambahKategoriJikaBelumAda(vector<string> &categories,
                                const string &category) {
  if (find(categories.begin(), categories.end(), category) == categories.end()) {
    categories.push_back(category);
  }
}
// Akhir penambahan kategori jika belum tersedia.

// Awal pencetakan leaderboard berdasarkan kategori dan urutan skor.
void cetakLeaderboard(bool skorTertinggiDulu) {
  vector<string> diffCategories = {"EASY", "MEDIUM", "EXTREME"};
  vector<string> modeCategories = {"Single", "Duel"};

  for (const auto &rec : records) {
    tambahKategoriJikaBelumAda(diffCategories, rec.difficulty);
    tambahKategoriJikaBelumAda(modeCategories, rec.mode);
  }

  for (const auto &diff : diffCategories) {
    for (const auto &mode : modeCategories) {
      vector<const skorPemain *> categoryRecords;
      for (const auto &rec : records) {
        if (rec.difficulty == diff && rec.mode == mode) {
          categoryRecords.push_back(&rec);
        }
      }

      cout << "\n>>> DIFFICULTY: " << diff << " | MODE: " << mode << " <<<\n";
      cout << left << setw(8) << "Rank" << setw(6) << "ID" << setw(20)
           << "Nama Driver" << setw(12) << "Skor" << setw(20)
           << "Waktu Tanggal" << "\n";
      cout << string(66, '-') << "\n";

      if (categoryRecords.empty()) {
        cout << "   (Belum ada data untuk kategori ini)\n";
        continue;
      }

      stable_sort(categoryRecords.begin(), categoryRecords.end(),
                  [skorTertinggiDulu](const skorPemain *leftRecord,
                                      const skorPemain *rightRecord) {
                    if (leftRecord->score != rightRecord->score) {
                      return skorTertinggiDulu
                                 ? leftRecord->score > rightRecord->score
                                 : leftRecord->score < rightRecord->score;
                    }
                    return leftRecord->id < rightRecord->id;
                  });

      int rank = 1;
      for (size_t i = 0; i < categoryRecords.size(); i++) {
        if (i > 0 && categoryRecords[i]->score != categoryRecords[i - 1]->score) {
          rank = static_cast<int>(i) + 1;
        }
        const auto &rec = *categoryRecords[i];
        cout << left << setw(8) << rank << setw(6) << rec.id << setw(20)
             << rec.name.substr(0, 18) << setw(12) << rec.score << setw(20)
             << rec.timestamp << "\n";
      }
    }
  }
  cout << "\n" << string(66, '=') << "\n";
}
// Akhir pencetakan leaderboard berdasarkan kategori dan urutan skor.
} // namespace

// Awal penampilan leaderboard.
void tampilkanLeaderboard() {
  cout << "\n========================================\n";
  cout << "\n--- LEADERBOARD ---\n";
  cout << "========================================\n";

  if (records.empty()) {
    cout << "[i] Belum ada data riwayat balapan.\n";
    return;
  }

  cetakLeaderboard(skorTertinggiDulu);
}
// Akhir penampilan leaderboard.

// Awal pencarian data pengemudi berdasarkan nama.
void cariPengemudi() {
  cout << "\n========================================\n";
  cout << "\n--- CARI DRIVER ---\n";
  cout << "========================================\n";
  if (records.empty()) {
    cout << "[i] Data masih kosong.\n";
    return;
  }

  string keyword = readString("Masukkan Nama Driver yang dicari: ");
  bool found = false;

  cout << "\nHasil Pencarian:\n";
  cout << left << setw(6) << "ID" << setw(20) << "Nama" << setw(12) << "Skor"
       << setw(12) << "Mode" << setw(14) << "Difficulty" << "\n";
  cout << string(64, '-') << "\n";

  for (const auto &rec : records) {
    if (rec.name.find(keyword) != string::npos) {
      cout << left << setw(6) << rec.id << setw(20) << rec.name << setw(12)
           << rec.score << setw(12) << rec.mode << setw(14) << rec.difficulty
           << "\n";
      found = true;
    }
  }

  if (!found) {
    cout << "[!] Driver dengan nama '" << keyword << "' tidak ditemukan.\n";
  }
}
// Akhir pencarian data pengemudi berdasarkan nama.

// Awal pengubahan data pengemudi berdasarkan ID.
void ubahDataPengemudi() {
  cout << "\n========================================\n";
  cout << "\n--- UBAH DATA PENGEMUDI ---\n";
  cout << "========================================\n";
  tampilkanLeaderboard();
  if (records.empty())
    return;

  int id = readInt("\nMasukkan ID data yang ingin diubah: ", 1, 1000000);
  int idx = cariIndexData(id);

  if (idx == -1) {
    cout << "[!] Data dengan ID " << id << " tidak ditemukan.\n";
    return;
  }

  cout << "\nData Saat Ini:\n";
  cout << "Nama       : " << records[idx].name << "\n";
  cout << "Skor       : " << records[idx].score << "\n";
  cout << "Difficulty : " << records[idx].difficulty << "\n";

  records[idx].name = readString("Masukkan Nama Baru: ");
  records[idx].score = readInt("Masukkan Skor Baru (0 - 10000): ", 0, 10000);
  records[idx].timestamp = pengaturanWaktuLeaderboard();

  cout << "[+] Data ID " << id << " berhasil diperbarui!\n";
}
// Akhir pengubahan data pengemudi berdasarkan ID.

// Awal penghapusan data pengemudi berdasarkan ID.
void hapusDataPengemudi() {
  cout << "\n========================================\n";
  cout << "\n--- HAPUS DATA RIWAYAT ---\n";
  cout << "========================================\n";
  tampilkanLeaderboard();
  if (records.empty())
    return;

  int id = readInt("\nMasukkan ID yang ingin dihapus: ", 1, 1000000);
  int idx = cariIndexData(id);

  if (idx == -1) {
    cout << "[!] Data dengan ID " << id << " tidak ditemukan.\n";
    return;
  }

  cout << "Yakin ingin menghapus data driver '" << records[idx].name
       << "' (ID: " << id << ")?\n";
  cout << "1. Ya, Hapus\n2. Batal\n";
  int confirmChoice = readInt("Pilihan: ", 1, 2);

  if (confirmChoice == 1) {
    records.erase(records.begin() + idx);
    cout << "[+] Data berhasil dihapus!\n";
  } else {
    cout << "[-] Penghapusan dibatalkan.\n";
  }
}
// Akhir penghapusan data pengemudi berdasarkan ID.

// Awal penampilan statistik balapan.
void tampilkanStatistikBalapan() {
  cout << "\n========================================\n";
  cout << "       STATISTIK ROADWAY TO HEAVEN        \n";
  cout << "========================================\n";

  if (records.empty()) {
    cout << "[i] Belum ada data untuk dihitung.\n";
    return;
  }

  int totalScore = 0;
  int maxScore = records[0].score;
  int minScore = records[0].score;
  string topPlayer = records[0].name;
  string lowPlayer = records[0].name;

  for (const auto &rec : records) {
    totalScore += rec.score;
    if (rec.score > maxScore) {
      maxScore = rec.score;
      topPlayer = rec.name;
    }
    if (rec.score < minScore) {
      minScore = rec.score;
      lowPlayer = rec.name;
    }
  }

  double averageScore = static_cast<double>(totalScore) / records.size();

  cout << "Total Sesi Balapan : " << records.size() << " kali\n";
  cout << "Total Poin Terkumpul: " << totalScore << " Poin\n";
  cout << fixed << setprecision(2);
  cout << "Rata-rata Skor     : " << averageScore << " Poin\n";
  cout << "skor tertinggi    : " << maxScore << " Poin (" << topPlayer
       << ")\n";
  cout << "Skor Terendah      : " << minScore << " Poin (" << lowPlayer
       << ")\n";
}
// Akhir penampilan statistik balapan.

// Awal pengurutan dan penampilan leaderboard.
void urutkanLeaderboard() {
  cout << "\n========================================\n";
  cout << "\n--- PENGURUTAN LEADERBOARD ---\n";
  cout << "========================================\n";
  if (records.empty()) {
    cout << "[i] Data kosong, tidak ada yang diurutkan.\n";
    return;
  }

  cout << "Pilih Jenis Pengurutan:\n";
  cout << "1. Skor Tertinggi ke Terendah\n";
  cout << "2. Skor Terendah ke Tertinggi\n";
  int sortChoice = readInt("Pilihan (1-2): ", 1, 2);

  skorTertinggiDulu = (sortChoice == 1);
  cout << "[+] Leaderboard ditampilkan berdasarkan urutan skor.\n";
  cetakLeaderboard(skorTertinggiDulu);
}
// Akhir pengurutan dan penampilan leaderboard.
