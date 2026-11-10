#ifndef HEADER_H
#define HEADER_H

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// 1. STRUKTUR DATA UTAMA
struct skorPemain {
  int id;
  string name;
  int score;
  string mode;       // "Single" atau "Duel"
  string difficulty; // "EASY", "MEDIUM", "EXTREME"
  string timestamp;  // Format: YYYY-MM-DD HH:MM
};

// DEKLARASI VARIABEL GLOBAL
extern vector<skorPemain> records;
extern int nextId;

// 2. PROTOTIPE FUNGSI - HELPER & MAIN (main.cpp)
string pengaturanWaktuLeaderboard();
int readInt(const string &pesan, int minAngka, int maxAngka);
string readString(const string &pesan);
void tampilanMenu();

// 3. PROTOTIPE FUNGSI - GAME ENGINE (game_engine.cpp)
void saveOrUpdateScore(const string &name, int newScore, const string &mode,
                       const string &difficulty);
int jalankansimulasi(const string &playerName, int difficulty);
void modeSinglePlayer();
void modePlayerDuel();

// 4. PROTOTIPE FUNGSI - DATA MANAGER (data_manager.cpp)
int cariIndexData(int id);
void tampilkanLeaderboard();
void cariPengemudi();
void ubahDataPengemudi();
void hapusDataPengemudi();
void tampilkanStatistikBalapan();
void urutkanLeaderboard();

// 5. PROTOTIPE FUNGSI - FILE I/O (file_io.cpp)
void saveData();
void loadData();

#endif
