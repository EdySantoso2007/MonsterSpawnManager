#define main game_main
#include "Final.cpp"
#undef main
#include <type_traits>
#include <map>
int main() {
    cout << "=== UJI FACTORY PATTERN ===\n";
    for (EnemyType t : {EnemyType::Slime, EnemyType::Goblin, EnemyType::Orc}) {
        auto e = EnemyFactory::createEnemy(t);
        cout << "createEnemy -> " << e->getName() << "  HP " << e->getHp() << "/" << e->getMaxHp()
             << "  skor " << e->getScoreValue() << "\n";
    }
    map<string,int> c; const int N = 10000;
    for (int i = 0; i < N; ++i) c[EnemyFactory::createRandomEnemy()->getName()]++;
    cout << "createRandomEnemy x" << N << ":\n";
    for (auto n : {"Slime","Goblin","Orc"})
        cout << "  " << n << " : " << c[n] << " (" << c[n]*100.0/N << "%)\n";

    cout << "\n=== UJI SINGLETON PATTERN ===\n";
    GameManager& a = GameManager::getInstance();
    GameManager& b = GameManager::getInstance();
    cout << "Akses 1 -> alamat " << &a << "\n";
    cout << "Akses 2 -> alamat " << &b << "\n";
    cout << "Alamat sama? " << (&a == &b ? "YA" : "TIDAK") << "\n";
    a.addScore(10); a.addKill();
    cout << "Lewat akses 1: addScore(10), addKill()\n";
    cout << "Dibaca akses 2: score=" << b.getScore() << ", kill=" << b.getKillCount() << "\n";
    b.setState(GameState::Won);
    cout << "Akses 2 setState(Won) -> akses 1 getState(): " << a.getStateText() << "\n";
    cout << "Bisa di-copy?          " << (is_copy_constructible<GameManager>::value ? "ya" : "tidak (delete)") << "\n";
    cout << "Bisa di-assign?        " << (is_copy_assignable<GameManager>::value ? "ya" : "tidak (delete)") << "\n";
    cout << "Bisa dibuat langsung?  " << (is_default_constructible<GameManager>::value ? "ya" : "tidak (private)") << "\n";
}
