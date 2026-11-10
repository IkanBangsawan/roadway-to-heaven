#include "header.h"

// Definisi variabel global
vector<skorPemain> records;
int nextId = 1;

// Mendapatkan string tanggal & waktu saat ini
string pengaturanWaktuLeaderboard() {
  time_t waktuSekarang = time(0);
  tm *ltm = localtime(&waktuSekarang);
  ostringstream oss;
  oss << 1900 + ltm->tm_year << "-" << setfill('0') << setw(2)
      << 1 + ltm->tm_mon << "-" << setfill('0') << setw(2) << ltm->tm_mday
      << " " << setfill('0') << setw(2) << ltm->tm_hour << ":" << setfill('0')
      << setw(2) << ltm->tm_min;
  return oss.str();
} 

// Validasi input angka integer (Anti-Crash)
int readInt(const string &pesan, int minAngka, int maxAngka) {
  while (true) {
    cout << pesan;
    string baris;
    getline(cin, baris);

    stringstream ss(baris);
    int angka;
    char sisa;

    if (ss >> angka && !(ss >> sisa) && angka >= minAngka &&
        angka <= maxAngka) {
      return angka;
    }
    cout << "[!] Tidak boleh kosong. Masukkan angka antara " << minAngka << " - "
         << maxAngka << ".\n";
  }
}

// Validasi input teks string (Tidak Boleh Kosong)
string readString(const string &pesan) {
  while (true) {
    cout << pesan;
    string teks;
    getline(cin, teks);

    size_t first = teks.find_first_not_of(" \t\r\n");
    if (first != string::npos) {
      size_t last = teks.find_last_not_of(" \t\r\n");
      return teks.substr(first, last - first + 1);
    }
    cout << "[!] Nama tidak boleh kosong! Silakan masukkan nama pengemudi.\n";
  }
}

// Tampilan Menu Utama Program
void tampilanMenu() {
  cout << "\n+--------------------------------------------------------+\n";
  cout << "|                 ROADWAY TO HEAVEN v1.0                   |\n";
  cout << "+----------------------------------------------------------+\n";
  cout << "| 1. Mode Single Player                                    |\n";
  cout << "| 2. Mode 2-Player Duel                                    |\n";
  cout << "| 3. Tampilkan Leaderboard                                 |\n";
  cout << "| 4. Cari Pengemudi                                        |\n";
  cout << "| 5. Ubah Data pengemudi                                   |\n";
  cout << "| 6. Hapus Data pengemudi                                  |\n";
  cout << "| 7. Statistik Game                                        |\n";
  cout << "| 8. Urutkan Leaderboard (Sorting)                         |\n";
  cout << "| 9. Simpan Data ke File (.txt)                            |\n";
  cout << "| 0. Keluar Aplikasi                                       |\n";
  cout << "+----------------------------------------------------------+\n";
}

int main() {
  srand(static_cast<unsigned int>(time(0))); // Seed random generator
  loadData();                                 // Auto-load data lama jika ada

  int choice;
  do {
    tampilanMenu();
    choice = readInt("Pilih Menu (0-9): ", 0, 9);

    switch (choice) {
    case 1:
      modeSinglePlayer();
      break;
    case 2:
      modePlayerDuel();
      break;
    case 3:
      tampilkanLeaderboard();
      break;
    case 4:
      cariPengemudi();
      break;
    case 5:
      ubahDataPengemudi();
      break;
    case 6:
      hapusDataPengemudi();
      break;
    case 7:
      tampilkanStatistikBalapan();
      break;
    case 8:
      urutkanLeaderboard();
      break;
    case 9:
      saveData();
      break;
    case 0:
      saveData(); // Auto-save saat keluar
      cout << "\nTerima kasih telah menggunakan Roadway To Heaven CLI!\n";
      break;
    }

    if (choice != 0) {
      cout << "\nTekan Enter untuk kembali ke menu utama...";
      string pause;
      getline(cin, pause);
    }
  } while (choice != 0);

  return 0;
}
