#include "header.h"

#include <cstdlib>
#include <ctime> 
#include <ncurses.h> 
#include <unistd.h>
#include <vector> 

//mendeklarasikan objek/enemy di balapan
enum EntityType {
  ENEMY_NORMAL,
  ENEMY_SWERVE_LEFT,
  ENEMY_SWERVE_RIGHT,
  ITEM_COIN,
  ITEM_SHIELD
};

//mendeklarasikan untuk variabel yang mengatur enemy
struct Entity {
  int lane;
  int y;
  EntityType type;
  int swerveCooldown;
  bool hasSwerved;
  float moveAccum; // mengatur kecepatan
};

// Awal penyimpanan atau pembaruan skor berdasarkan mode dan difficulty.
void saveOrUpdateScore(const string &name, int newScore, const string &mode,
                       const string &difficulty) {
  bool found = false;
  for (auto &rec : records) {
    if (rec.name == name && rec.mode == mode && rec.difficulty == difficulty) {
      found = true;
      if (newScore > rec.score) {
        rec.score = newScore;
        rec.timestamp = pengaturanWaktuLeaderboard();
      }
      break;
    }
  }

  if (!found) {
    skorPemain newRecord;
    newRecord.id = nextId++;
    newRecord.name = name;
    newRecord.score = newScore;
    newRecord.mode = mode;
    newRecord.difficulty = difficulty;
    newRecord.timestamp = pengaturanWaktuLeaderboard();
    records.push_back(newRecord);
  }
}
// Akhir penyimpanan atau pembaruan skor berdasarkan mode dan difficulty.

// GAME ENGINE NCURSES (CENTERED & LARGE LAYOUT)
int jalankansimulasi(const string &playerName, int difficulty) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);

  if (has_colors()) {
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);    // Mobil Pemain
    init_pair(2, COLOR_RED, COLOR_BLACK);     // Musuh Normal
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // Musuh Belok (Left & Right)
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Gold Coin [$]
    init_pair(5, COLOR_BLUE, COLOR_BLACK);    // Shield Item [S]
    init_pair(6, COLOR_WHITE, COLOR_BLACK);   // Marka Jalan
    init_pair(7, COLOR_GREEN, COLOR_BLACK);   // HUD / Sidebar
  }

  int numLanes = 4;
  int maxConcurrentEnemies = 2;
  int baseDelay = 80000;
  string diffLabel = "EASY";

  if (difficulty == 2) {
    numLanes = 5;
    maxConcurrentEnemies = 3;
    baseDelay = 65000;
    diffLabel = "MEDIUM";
  } else if (difficulty == 3) {
    numLanes = 6;
    maxConcurrentEnemies = 4;
    baseDelay = 45000;
    diffLabel = "EXTREME";
  }

  // Layout Tengah Layar
  int termY, termX;
  getmaxyx(stdscr, termY, termX);

  int GAME_HEIGHT = 22; // Arena Lebih Tinggi
  int laneWidth = 7;
  int GAME_WIDTH = (numLanes * laneWidth) + 3;
  int SIDE_WIDTH = 34;
  int totalLayoutWidth = GAME_WIDTH + 2 + SIDE_WIDTH;

  if (termY < GAME_HEIGHT + 4 || termX < totalLayoutWidth + 4) {
    endwin();
    cout << "[!] Terminal terlalu kecil untuk mode balapan.\n";
    cout << "    Minimal dibutuhkan lebar " << totalLayoutWidth + 4
         << " dan tinggi " << GAME_HEIGHT + 4 << " karakter.\n";
    return 0;
  }

  int startY = (termY - GAME_HEIGHT) / 2;
  int startX = (termX - totalLayoutWidth) / 2;
  if (startY < 0) startY = 0;
  if (startX < 0) startX = 0;

  WINDOW *gameWin = newwin(GAME_HEIGHT, GAME_WIDTH, startY, startX);
  WINDOW *sideWin = newwin(GAME_HEIGHT, SIDE_WIDTH, startY, startX + GAME_WIDTH + 2);

  vector<int> laneX; //mengatur posisi kordinat x agar konsisten dan berjarak satu sama lain
  for (int i = 0; i < numLanes; i++) {
    laneX.push_back(2 + (i * laneWidth));
  }
  
  int playerLane = numLanes / 2; 
  bool hasShield = false;
  int score = 0;
  int roadOffset = 0;
  bool gameOver = false;

  vector<Entity> entities;

  const char *playerSprite[3] = {
    "  ^  ",
    " |O| ",
    "[===]"
  };

  const char *enemyNormalSprite[3] = {
    "[XXX]",
    " |V| ",
    "  v  "
  };

  const char *enemySwerveLeftSprite[3] = { // Mobil Belok Kiri
    "[<<<]",
    " |?| ",
    "  v  "
  };

  const char *enemySwerveRightSprite[3] = { // Mobil Belok Kanan
    "[>>>]",
    " |?| ",
    "  v  "
  };

  while (!gameOver) {
    // --- SIDEBAR INTERFACE ---
    werase(sideWin);
    box(sideWin, 0, 0);
    wattron(sideWin, COLOR_PAIR(7) | A_BOLD);
    mvwprintw(sideWin, 1, 2, "=== ROADWAY TO HEAVEN ===");
    wattroff(sideWin, COLOR_PAIR(7) | A_BOLD);

    mvwprintw(sideWin, 3, 2, "Driver     : %s", playerName.c_str());
    mvwprintw(sideWin, 4, 2, "Difficulty : %s (%d Jalur)", diffLabel.c_str(), numLanes);
    mvwprintw(sideWin, 5, 2, "Skor       : %d Poin", score);

    if (hasShield) {
      wattron(sideWin, COLOR_PAIR(5) | A_BOLD);
      mvwprintw(sideWin, 7, 2, "Perisai    : [ AKTIF ]");
      wattroff(sideWin, COLOR_PAIR(5) | A_BOLD);
    } else {
      mvwprintw(sideWin, 7, 2, "Perisai    : [ NO SHIELD ]");
    }

    mvwprintw(sideWin, 9, 2, "--- KONTROL & ITEM ---");
    mvwprintw(sideWin, 10, 2, "[ A / <- ] : Kiri");
    mvwprintw(sideWin, 11, 2, "[ D / -> ] : Kanan");
    mvwprintw(sideWin, 12, 2, "[ Q ]      : Menyerah");

    wattron(sideWin, COLOR_PAIR(4));
    mvwprintw(sideWin, 15, 2, "[$] Coin   : +50 Bonus Poin");
    wattroff(sideWin, COLOR_PAIR(4));

    wattron(sideWin, COLOR_PAIR(5));
    mvwprintw(sideWin, 16, 2, "[S] Shield : Anti 1x Tabrakan");
    wattroff(sideWin, COLOR_PAIR(5));

    wattron(sideWin, COLOR_PAIR(3));
    mvwprintw(sideWin, 17, 2, "[<>] Swerve: Belok 1x (Magenta)");
    wattroff(sideWin, COLOR_PAIR(3));

    wrefresh(sideWin);

    // --- MENGGAMBAR ARENA BALAP ---
    werase(gameWin);
    box(gameWin, 0, 0);

    wattron(gameWin, COLOR_PAIR(6));
    for (int y = 1; y < GAME_HEIGHT - 1; y++) {
      if ((y + roadOffset) % 3 == 0) {
        for (int l = 1; l < numLanes; l++) {
          mvwprintw(gameWin, y, l * laneWidth + 1, "|");
        }
      }
    }
    wattroff(gameWin, COLOR_PAIR(6));

    // Spawn Musuh & Item
    if (static_cast<int>(entities.size()) < maxConcurrentEnemies && (rand() % 100 < 35)) {
      int spawnLane = rand() % numLanes;

      // Cek apakah jalur spawn sudah ditempati entitas lain di area spawn
      bool occupied = false;
      for (const auto &e : entities) {
        if (e.y < 4 && e.lane == spawnLane)
          occupied = true;
      }

      if (!occupied) {
        Entity newEnt;
        newEnt.lane = spawnLane;
        newEnt.y = -3;
        newEnt.swerveCooldown = rand() % 2 + 2;
        newEnt.hasSwerved = false;
        newEnt.moveAccum = 0.0f; // kecepatan

        int roll = rand() % 100;
        if (roll < 60) {
          newEnt.type = ENEMY_NORMAL;
        } else if (roll < 85) {
          if (spawnLane == 0) {
            newEnt.type = ENEMY_SWERVE_RIGHT;
          } else if (spawnLane == numLanes - 1) {
            newEnt.type = ENEMY_SWERVE_LEFT;
          } else {
            newEnt.type = (rand() % 2 == 0) ? ENEMY_SWERVE_LEFT : ENEMY_SWERVE_RIGHT;
          }
        } else if (roll < 95) {
          newEnt.type = ITEM_COIN;
        } else {
          newEnt.type = ITEM_SHIELD;
        }
        entities.push_back(newEnt);
      }
    }

    // Render Mobil Pemain
    int playerY = GAME_HEIGHT - 4;
    wattron(gameWin, COLOR_PAIR(1) | A_BOLD);
    for (int i = 0; i < 3; i++) {
      mvwprintw(gameWin, playerY + i, laneX[playerLane], "%s", playerSprite[i]);
    }
    wattroff(gameWin, COLOR_PAIR(1) | A_BOLD);

    // Mengatur Kecepatan
    int speedLevel = score/5000;
    if (speedLevel > 10) speedLevel = 10;
    float speedMult = 1.0f + (speedLevel * 0.2f); // Biar maks nya di 3x lipat makanya *0.2f

    // Update & Render Entitas
    for (size_t idx = 0; idx < entities.size(); idx++) {
      Entity &e = entities[idx];

      // LOGIKA BELOK (SWERVING)
      if (!e.hasSwerved && e.y >= 3 && e.y <= playerY - 4) {
        if (e.type == ENEMY_SWERVE_LEFT) {
          e.swerveCooldown--;
          if (e.swerveCooldown <= 0) {
            if (e.lane > 0) {
              e.lane--;
            }
            e.hasSwerved = true;
          }
        } else if (e.type == ENEMY_SWERVE_RIGHT) {
          e.swerveCooldown--;
          if (e.swerveCooldown <= 0) {
            if (e.lane < numLanes - 1) {
              e.lane++;
            }
            e.hasSwerved = true;
          }
        }
      }

      // Render Item & Musuh
      if (e.type == ITEM_COIN) {
        wattron(gameWin, COLOR_PAIR(4) | A_BOLD);
        if (e.y >= 1 && e.y < GAME_HEIGHT - 1)
          mvwprintw(gameWin, e.y, laneX[e.lane], " [$] ");
        wattroff(gameWin, COLOR_PAIR(4) | A_BOLD);
      } else if (e.type == ITEM_SHIELD) {
        wattron(gameWin, COLOR_PAIR(5) | A_BOLD);
        if (e.y >= 1 && e.y < GAME_HEIGHT - 1)
          mvwprintw(gameWin, e.y, laneX[e.lane], " [S] ");
        wattroff(gameWin, COLOR_PAIR(5) | A_BOLD);
      } else {
        int colorPair = (e.type == ENEMY_NORMAL) ? 2 : 3;
        const char **sprite = (e.type == ENEMY_NORMAL) ? enemyNormalSprite :
                              (e.type == ENEMY_SWERVE_LEFT) ? enemySwerveLeftSprite :
                              enemySwerveRightSprite;

        wattron(gameWin, COLOR_PAIR(colorPair) | A_BOLD);
        for (int i = 0; i < 3; i++) {
          int ey = e.y + i;
          if (ey >= 1 && ey < GAME_HEIGHT - 1) {
            mvwprintw(gameWin, ey, laneX[e.lane], "%s", sprite[i]);
          }
        }
        wattroff(gameWin, COLOR_PAIR(colorPair) | A_BOLD);
      }

      // Deteksi Tabrakan
      if (e.lane == playerLane && (e.y + 2 >= playerY && e.y <= playerY + 2)) {
        if (e.type == ITEM_COIN) {
          score += 50;
          entities.erase(entities.begin() + idx);
          idx--;
          continue;
        } else if (e.type == ITEM_SHIELD) {
          hasShield = true;
          entities.erase(entities.begin() + idx);
          idx--;
          continue;
        } else {
          if (hasShield) {
            hasShield = false;
            entities.erase(entities.begin() + idx);
            idx--;
            continue;
          } else {
            gameOver = true;
            break;
          }
        }
      }

      e.moveAccum += speedMult;
      int steps = static_cast<int>(e.moveAccum);
      e.moveAccum -= steps;
      e.y += steps;

      if (e.y >= GAME_HEIGHT - 1) {
        entities.erase(entities.begin() + idx);
        idx--;
        score += 15;
      }
    }

    wrefresh(gameWin);

    // Input Pergerakan Pemain
    int ch = getch();
    if ((ch == 'a' || ch == 'A' || ch == KEY_LEFT) && playerLane > 0) {
      playerLane--;
    }
    if ((ch == 'd' || ch == 'D' || ch == KEY_RIGHT) && playerLane < numLanes - 1) {
      playerLane++;
    }
    if (ch == 'q' || ch == 'Q') {
      break;
    }

    roadOffset++;
    score += 1;
    usleep(baseDelay); 
  }

  nodelay(stdscr, FALSE);
  clear();
  attron(COLOR_PAIR(2) | A_BOLD);
  mvprintw(startY + 6, startX + 5, "=========================================");
  mvprintw(startY + 7, startX + 5, "         ~ GAME OVER / TABRAKAN! ~       ");
  mvprintw(startY + 8, startX + 5, "=========================================");
  attroff(COLOR_PAIR(2) | A_BOLD);

  attron(COLOR_PAIR(7));
  mvprintw(startY + 10, startX + 7, "Driver     : %s", playerName.c_str());
  mvprintw(startY + 11, startX + 7, "Difficulty : %s", diffLabel.c_str());
  mvprintw(startY + 12, startX + 7, "Total Skor : %d Poin", score);
  attroff(COLOR_PAIR(7));

  mvprintw(startY + 15, startX + 7, "Tekan tombol APAPUN untuk melanjutkan...");
  refresh();
  getch();

  endwin();
  return score;
}
// Akhir menjalankan simulasi balapan.


//-- pemilihan dificulty
// Awal pemilihan tingkat kesulitan balapan.
int chooseDifficulty() {
  cout << "\n--- PILIH TINGKAT KESULITAN ---";
  cout << "\n1. Easy    (4 Jalur, Santai)";
  cout << "\n2. Medium  (5 Jalur, Ada Musuh Belok 1x)";
  cout << "\n3. Extreme (6 Jalur, Sangat Cepat & Padat)";
  return readInt("\nPilih Difficulty (1-3): ", 1, 3);
}
// Akhir pemilihan tingkat kesulitan balapan.

//-- pemilihan mode single player
// Awal menjalankan mode single player.
void modeSinglePlayer() {
  cout << "\n--- MODE SINGLE PLAYER ---\n";
  string name = readString("Masukkan Nama Driver: ");
  int diff = chooseDifficulty();

  string diffLabel = (diff == 1) ? "EASY" : (diff == 2) ? "MEDIUM" : "EXTREME";
  int score = jalankansimulasi(name, diff);
  saveOrUpdateScore(name, score, "Single", diffLabel);
  cout << "\n[+] Data skor " << name << " (" << score << " Poin - " << diffLabel
       << ") telah diperbarui di Leaderboard!\n";
}
// Akhir menjalankan mode single player.

//-- pemilihan mode duel player
// Awal menjalankan mode duel dua pemain.
void modePlayerDuel() {
  cout << "\n--- MODE 2-PLAYER DUEL RACE ---\n";
  string player1 = readString("Masukkan Nama Player 1: ");
  string player2 = readString("Masukkan Nama Player 2: ");
  int diff = chooseDifficulty();
  string diffLabel = (diff == 1) ? "EASY" : (diff == 2) ? "MEDIUM" : "EXTREME";

  cout << "\n[!] Player 1 (" << player1 << "), siap-siap! Tekan Enter...";
  cin.get();
  int score1 = jalankansimulasi(player1, diff);

  cout << "\n[!] Player 2 (" << player2 << "), siap-siap! Tekan Enter...";
  cin.get();
  int score2 = jalankansimulasi(player2, diff);

  cout << "\n========================================\n";
  cout << "          HASIL PERTANDINGAN DUEL        \n";
  cout << "========================================\n";
  cout << player1 << " : " << score1 << " Poin\n";
  cout << player2 << " : " << score2 << " Poin\n";
  cout << "----------------------------------------\n";

  if (score1 > score2) {
    cout << "🏆 PEMENANG: " << player1 << "!\n";
  } else if (score2 > score1) {
    cout << "🏆 PEMENANG: " << player2 << "!\n";
  } else {
    cout << "🤝 HASIL SERI / DRAW!\n";
  }
  // simpan data pemain
  saveOrUpdateScore(player1, score1, "Duel", diffLabel);
  saveOrUpdateScore(player2, score2, "Duel", diffLabel);
}
// Akhir menjalankan mode duel dua pemain