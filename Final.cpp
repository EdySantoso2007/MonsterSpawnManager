// ============================================================
// Studi Kasus 2: Monster Spawn Manager (C++ Console)
// Pattern: Factory Pattern, Game Loop Pattern, Singleton Pattern
// Kompilasi: g++ -std=c++17 monster_spawn_manager_komentar.cpp -o game
// ============================================================

// ---------- HEADER (library bawaan C++) ----------
#include <iostream>   // untuk cout (tampil teks) dan cin (baca input)
#include <memory>     // untuk unique_ptr (pointer pintar, otomatis hapus memori)
#include <string>     // untuk tipe data string
#include <random>     // untuk angka acak (damage & spawn acak)
#include <algorithm>  // untuk fungsi min() dan max()
#include <limits>     // untuk numeric_limits (membersihkan input yang salah)

using namespace std;  // supaya tidak perlu menulis "std::" di setiap kode

// ------------------------------------------------------------
// FUNGSI BANTU: menghasilkan angka acak antara lo sampai hi
// ------------------------------------------------------------
int randomRange(int lo, int hi) {                       // lo = batas bawah, hi = batas atas
    static mt19937 rng(random_device{}());              // generator acak, dibuat SEKALI saja (static)
    return uniform_int_distribution<int>(lo, hi)(rng);  // ambil angka acak dalam rentang lo..hi
}

// ============================================================
// SINGLETON PATTERN : GameManager
// Hanya ada SATU objek GameManager yang menyimpan data global.
// ============================================================

// enum class = daftar nilai tetap untuk status permainan
enum class GameState { Playing, Won, Lost, Quit };      // sedang main, menang, kalah, keluar

class GameManager {                                     // deklarasi kelas GameManager
private:                                                // bagian private: tidak bisa diakses dari luar
    int score;                                          // menyimpan total skor
    int killCount;                                      // menyimpan jumlah monster yang dibunuh
    GameState state;                                    // menyimpan status permainan saat ini

    // Constructor dibuat PRIVATE -> orang lain tidak bisa membuat GameManager dengan "GameManager gm;"
    GameManager() : score(0), killCount(0), state(GameState::Playing) {}  // nilai awal: 0, 0, Playing

public:                                                 // bagian public: bisa diakses dari luar
    GameManager(const GameManager&) = delete;           // larang copy objek (agar tetap satu)
    GameManager& operator=(const GameManager&) = delete;// larang assignment objek (agar tetap satu)

    static GameManager& getInstance() {                 // pintu satu-satunya untuk mengambil GameManager
        static GameManager instance;                    // dibuat pertama kali dipanggil, lalu dipakai terus
        return instance;                                // kembalikan referensi ke objek yang sama
    }

    void addScore(int s)       { score += s; }          // tambah skor sebesar s
    void addKill()             { ++killCount; }         // tambah kill count sebanyak 1
    void setState(GameState s) { state = s; }           // ubah status permainan

    int getScore() const       { return score; }        // ambil nilai skor (const = tidak mengubah data)
    int getKillCount() const   { return killCount; }    // ambil jumlah kill
    GameState getState() const { return state; }        // ambil status permainan

    string getStateText() const {                       // ubah status (enum) jadi teks untuk ditampilkan
        switch (state) {                                // cek nilai state
            case GameState::Playing: return "Playing";  // jika Playing -> teks "Playing"
            case GameState::Won:     return "Won";      // jika Won -> teks "Won"
            case GameState::Lost:    return "Lost";     // jika Lost -> teks "Lost"
            case GameState::Quit:    return "Quit";     // jika Quit -> teks "Quit"
        }
        return "?";                                     // cadangan jika tidak ada yang cocok
    }
};

// ============================================================
// ENEMY (kelas dasar/abstrak) + Slime, Goblin, Orc
// ============================================================
class Enemy {                                           // kelas induk untuk semua monster
protected:                                              // protected: bisa diakses kelas turunan (Slime dll)
    string name;                                        // nama monster
    int hp;                                             // HP saat ini
    int maxHp;                                          // HP maksimal (HP awal)
    int attackPower;                                    // kekuatan serangan dasar
    int scoreValue;                                     // skor yang didapat jika monster mati

public:
    // Constructor: mengisi data monster saat dibuat
    Enemy(string n, int h, int atk, int sc)
        : name(move(n)), hp(h), maxHp(h), attackPower(atk), scoreValue(sc) {}  // maxHp = hp awal
    virtual ~Enemy() = default;                         // destructor virtual agar penghapusan lewat pointer induk aman

    void takeDamage(int dmg) { hp = max(0, hp - dmg); } // kurangi HP, tapi tidak boleh di bawah 0
    bool isAlive() const     { return hp > 0; }         // monster hidup jika HP lebih dari 0

    // virtual = boleh di-override oleh kelas turunan (tiap monster punya cara serang sendiri)
    virtual int attack() const { return attackPower; }  // serangan default = attackPower

    const string& getName() const { return name; }      // ambil nama monster
    int getHp() const             { return hp; }        // ambil HP sekarang
    int getMaxHp() const          { return maxHp; }     // ambil HP maksimal
    int getScoreValue() const     { return scoreValue; }// ambil skor hadiah
};

class Slime : public Enemy {                            // Slime turunan (inheritance) dari Enemy
public:
    Slime() : Enemy("Slime", 30, 5, 10) {}              // nama, HP 30, atk 5, skor 10
    int attack() const override { return randomRange(3, 6); }   // override: damage acak 3-6
};

class Goblin : public Enemy {                           // Goblin turunan dari Enemy
public:
    Goblin() : Enemy("Goblin", 50, 8, 25) {}            // nama, HP 50, atk 8, skor 25
    int attack() const override { return randomRange(5, 10); }  // damage acak 5-10
};

class Orc : public Enemy {                              // Orc turunan dari Enemy
public:
    Orc() : Enemy("Orc", 80, 12, 50) {}                 // nama, HP 80, atk 12, skor 50
    int attack() const override { return randomRange(8, 15); }  // damage acak 8-15
};

// ============================================================
// FACTORY PATTERN : EnemyFactory
// Semua monster dibuat di sini, bukan di kelas Game.
// ============================================================
enum class EnemyType { Slime, Goblin, Orc };            // daftar tipe monster yang bisa dibuat

class EnemyFactory {                                    // kelas pabrik monster
public:
    // static = bisa dipanggil tanpa membuat objek: EnemyFactory::createEnemy(...)
    static unique_ptr<Enemy> createEnemy(EnemyType type) {      // terima tipe, kembalikan monster
        switch (type) {                                 // cek tipe yang diminta
            case EnemyType::Slime:  return make_unique<Slime>();   // buat Slime
            case EnemyType::Goblin: return make_unique<Goblin>();  // buat Goblin
            case EnemyType::Orc:    return make_unique<Orc>();     // buat Orc
        }
        return nullptr;                                 // cadangan jika tipe tidak dikenal
    }

    // Membuat monster acak: Slime paling sering, Orc paling jarang
    static unique_ptr<Enemy> createRandomEnemy() {
        int r = randomRange(1, 100);                    // angka acak 1-100
        if (r <= 50)      return createEnemy(EnemyType::Slime);    // 50% kemungkinan Slime
        else if (r <= 80) return createEnemy(EnemyType::Goblin);   // 30% kemungkinan Goblin
        else              return createEnemy(EnemyType::Orc);      // 20% kemungkinan Orc
    }
};

// ============================================================
// PLAYER
// ============================================================
class Player {
private:
    int hp;                                             // HP player saat ini
    int maxHp;                                          // HP maksimal player
    int healLeft;                                       // sisa jatah heal

public:
    Player() : hp(100), maxHp(100), healLeft(3) {}      // awal: HP 100, jatah heal 3 kali

    int attack() const { return randomRange(12, 22); }  // damage serangan player acak 12-22

    // return true jika heal berhasil, false jika jatah habis
    bool heal() {
        if (healLeft <= 0) return false;                // jatah habis -> gagal
        --healLeft;                                     // kurangi jatah heal
        hp = min(maxHp, hp + 30);                       // tambah 30 HP, tapi tidak lebih dari maxHp
        return true;                                    // heal berhasil
    }

    void takeDamage(int dmg) { hp = max(0, hp - dmg); } // player terkena damage, HP minimal 0
    bool isAlive() const     { return hp > 0; }         // player hidup jika HP > 0

    int getHp() const        { return hp; }             // ambil HP sekarang
    int getMaxHp() const     { return maxHp; }          // ambil HP maksimal
    int getHealLeft() const  { return healLeft; }       // ambil sisa jatah heal
};

// ============================================================
// GAME LOOP PATTERN : Game
// Loop utama: processInput() -> update() -> render()
// ============================================================
enum class Action { None, Attack, Heal, Skip, Quit };   // daftar aksi yang bisa dipilih player

class Game {
private:
    Player player;                                      // objek player
    unique_ptr<Enemy> currentEnemy;                     // monster yang sedang dilawan (bisa kosong)
    Action pendingAction;                               // aksi yang dipilih player pada turn ini
    string log;                                         // pesan kejadian turn ini (untuk ditampilkan)
    static const int WIN_KILLS = 10;                    // target kill untuk menang

    // Memunculkan monster baru lewat FACTORY
    void spawnMonster() {
        currentEnemy = EnemyFactory::createRandomEnemy();            // minta pabrik membuat monster acak
        log += ">> " + currentEnemy->getName() + " muncul!\n";       // catat ke log
    }

    // ---------- INPUT (tahap 1 game loop) ----------
    void processInput() {
        pendingAction = Action::None;                   // reset aksi
        while (pendingAction == Action::None) {         // ulangi sampai input valid
            cout << "Pilih aksi: [1] Attack  [2] Heal  [3] Skip  [0] Keluar\n> ";  // tampilkan menu
            int choice;                                 // tempat menyimpan pilihan
            if (!(cin >> choice)) {                     // jika gagal membaca angka
                if (cin.eof()) { pendingAction = Action::Quit; break; }  // input berakhir -> keluar game
                cin.clear();                            // hapus status error cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n');     // buang sisa input yang salah
                cout << "Input tidak valid.\n";         // beri tahu pengguna
                continue;                               // ulangi dari awal while
            }
            switch (choice) {                           // cek angka yang dimasukkan
                case 1: pendingAction = Action::Attack; break;   // 1 = serang
                case 2: pendingAction = Action::Heal;   break;   // 2 = heal
                case 3: pendingAction = Action::Skip;   break;   // 3 = lewati giliran
                case 0: pendingAction = Action::Quit;   break;   // 0 = keluar
                default: cout << "Pilihan tidak ada.\n";         // angka lain -> ulangi
            }
        }
    }

    // ---------- UPDATE (tahap 2 game loop) ----------
    void update() {
        GameManager& gm = GameManager::getInstance();   // ambil satu-satunya GameManager (Singleton)
        log.clear();                                    // kosongkan log turn sebelumnya

        if (pendingAction == Action::Quit) {            // jika player memilih keluar
            gm.setState(GameState::Quit);               // ubah status jadi Quit
            return;                                     // hentikan update
        }

        if (!currentEnemy) spawnMonster();              // jika belum ada monster, munculkan satu

        // 1) Jalankan aksi player
        switch (pendingAction) {
            case Action::Attack: {                      // aksi serang
                int dmg = player.attack();              // hitung damage player
                currentEnemy->takeDamage(dmg);          // kurangi HP monster
                log += "Kamu menyerang " + currentEnemy->getName() +
                       " sebesar " + to_string(dmg) + " damage.\n";  // catat ke log
                break;
            }
            case Action::Heal:                          // aksi heal
                if (player.heal())                      // coba heal, true jika berhasil
                    log += "Kamu heal +30 HP. Sisa heal: " +
                           to_string(player.getHealLeft()) + "\n";   // catat heal berhasil
                else
                    log += "Heal habis! Turn terbuang.\n";            // jatah heal habis
                break;
            case Action::Skip:                          // aksi skip
                log += "Kamu melewati giliran.\n";      // tidak ada yang dilakukan
                break;
            default: break;                             // aksi lain: abaikan
        }

        // 2) Cek apakah monster mati
        if (!currentEnemy->isAlive()) {                 // HP monster sudah 0?
            log += currentEnemy->getName() + " mati! +" +
                   to_string(currentEnemy->getScoreValue()) + " score.\n";  // catat kematian
            gm.addScore(currentEnemy->getScoreValue()); // tambah skor ke GameManager
            gm.addKill();                               // tambah kill count
            currentEnemy.reset();                       // hapus monster dari memori

            if (gm.getKillCount() >= WIN_KILLS) {       // sudah mencapai target kill?
                gm.setState(GameState::Won);            // status menang
                return;                                 // selesai
            }
            spawnMonster();                             // munculkan monster berikutnya
            return;                                     // monster baru tidak langsung menyerang
        }

        // 3) Monster masih hidup -> membalas serangan
        int dmg = currentEnemy->attack();               // hitung damage monster
        player.takeDamage(dmg);                         // kurangi HP player
        log += currentEnemy->getName() + " menyerangmu sebesar " +
               to_string(dmg) + " damage.\n";           // catat ke log

        if (!player.isAlive())                          // HP player habis?
            gm.setState(GameState::Lost);               // status kalah
    }

    // ---------- RENDER (tahap 3 game loop) ----------
    void render() const {
        GameManager& gm = GameManager::getInstance();   // ambil GameManager untuk data skor
        cout << "\n----------------------------------------\n";  // garis pemisah
        cout << log;                                    // tampilkan kejadian turn ini
        cout << "----------------------------------------\n";
        cout << "Player HP : " << player.getHp() << "/" << player.getMaxHp()
             << "  (Heal: " << player.getHealLeft() << ")\n";    // tampilkan HP player & sisa heal
        if (currentEnemy)                               // hanya tampil jika ada monster
            cout << "Monster   : " << currentEnemy->getName() << "  HP "
                 << currentEnemy->getHp() << "/" << currentEnemy->getMaxHp() << "\n";  // info monster
        cout << "Score     : " << gm.getScore()
             << " | Kill: " << gm.getKillCount() << "/" << WIN_KILLS
             << " | State: " << gm.getStateText() << "\n";       // info global dari GameManager
        cout << "----------------------------------------\n\n";
    }

public:
    Game() : pendingAction(Action::None) {}             // constructor: aksi awal = None

    void run() {                                        // fungsi yang menjalankan seluruh game
        GameManager& gm = GameManager::getInstance();   // ambil GameManager
        cout << "=== MONSTER SPAWN MANAGER ===\n";      // judul game
        cout << "Kalahkan " << WIN_KILLS << " monster untuk menang!\n";  // info tujuan

        spawnMonster();                                 // munculkan monster pertama
        render();                                       // tampilkan kondisi awal

        // ===== GAME LOOP =====
        while (gm.getState() == GameState::Playing) {   // ulangi selama status masih Playing
            processInput();                             // 1. input: baca pilihan player
            update();                                   // 2. update: proses logika game
            render();                                   // 3. render: tampilkan hasilnya
        }

        // Setelah loop selesai, tampilkan hasil akhir
        switch (gm.getState()) {
            case GameState::Won:  cout << "SELAMAT! Kamu menang!\n"; break;   // menang
            case GameState::Lost: cout << "GAME OVER. Kamu kalah.\n"; break;  // kalah
            default:              cout << "Game dihentikan.\n"; break;        // keluar manual
        }
        cout << "Skor akhir: " << gm.getScore()
             << " | Total kill: " << gm.getKillCount() << "\n";   // tampilkan hasil akhir
    }
};

// ============================================================
// FUNGSI main: titik awal program
// ============================================================
int main() {
    Game game;          // buat objek Game
    game.run();         // jalankan game loop
    return 0;           // program selesai tanpa error
}