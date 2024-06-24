#include <iostream>
#include <map>
#include <algorithm> //sort
#include <fstream> //file
#include <vector> //vector<>
#include <functional> //function<>
#include <conio.h> //_getch()

using namespace std;

class WordDecryptor {
  protected:
    string filename_;
    static int cipher_;

    string Encr_Decr(const string& before_encr_decr, int coef) {
        string after_encr_decr;
        for (char c : before_encr_decr) {
            after_encr_decr += c + coef;
        }
        return after_encr_decr;
    }

    string Decrypt(const string& encryptedWord) {
        return Encr_Decr(encryptedWord, -cipher_);
    }

    string Encrypt(const string& decryptedWord) {
        return Encr_Decr(decryptedWord, cipher_);
    }
        
  public:
    WordDecryptor(const string& filename) : filename_(filename) {};

    vector<string> getDecryptedWords() {
        vector<string> decryptedWords;
        ifstream file(filename_);
        string encryptedWord;

        if (!file.eof()) {
            while (file >> encryptedWord) {
                decryptedWords.push_back(Decrypt(encryptedWord));
            }
            file.close();
        }
        else {
            cerr << "Не удалось открыть файл: " << filename_ << endl;
        }

        return decryptedWords;
    }



};

class File : public WordDecryptor {
  public:
    File(const string& filename) : WordDecryptor(filename) {};
    File() : File("") {};

    void addWordsToFile(initializer_list<string> words) {
        ofstream file(filename_, ios::app);
        if (!file.eof()) {
            for (string word : words) {
                if (!Search_Word(Encrypt(word))) {
                    file << Encrypt(word) << endl;
                    cout << "Слово " << word << " успешно добавлено!" << endl;
                }
                else {
                    cout << "Слово " << word << " уже есть!" << endl;
                }
            }
            file.close();
        }
        else {
            cerr << "Не удалось открыть файл " << filename_  << " для записи." << endl;
        }
    }

    void Request_addWordToFile() {
        cout << "Введите слово: ";
        string word;
        cin >> word;
        addWordToFile(word);
    }

    void addWordToFile(const string& word) {
        addWordsToFile({word});
    }

    bool Search_Word(const string& for_search) {
        ifstream file(filename_);
        string word;
        while (file >> word) {
            if (word == for_search) {
                file.close();
                return 1;
            }
        }
        file.close();
        return 0;
    }

    string GetWord() {
        return getDecryptedWords()[rand() % (getDecryptedWords().size())];
    }
};

class Letter {
  private:
    char value_;
    bool revealed_;

  public:
    Letter(char c) : value_(c), revealed_(false) {};

    char GetValue() const {
        return value_;
    }

    bool GetRevealed() const {
        return revealed_;
    }

    void ReverseRevealing() {
        revealed_ = !revealed_;
    }
};

class Word {
  private:
    vector<Letter> letters_;

  public:
    Word(const string& word) {
        for (auto ch : word) {
            letters_.push_back(ch);
        }
    }
    Word() : Word("") {};

    string ToString() const {
        string str;
        for (auto letter : letters_) {
            str += letter.GetValue();
        }
        return str;
    }

    int Length() const {
        return letters_.size();
    }

    vector<Letter>& GetLetters() {
        return letters_;
    }
};

class Leaderboard {
  private:
    static map<string, int> scores_;

  public:
    void AddScore(const string& username, int score) {
        scores_[username] = score;
    }

    void DisplayTopScores() {
        cout << "Топ пользователей:" << endl;
        for (const auto& score : scores_) {
            cout << score.first << ": " << score.second << endl;
        }
    }
};

class Statistics {
  private:
    Leaderboard leaderboard_;
    Word* word_;
    double time_start_;
    int  attempts_;
    static double time_spent_all_;
    static int attempts_all_;
    static int score_all_;
    static int count_words_all_;
    static int count_correct_words_;

  public:
    Statistics(Word* word) : word_(word), time_start_(clock()), attempts_(0) {};
    Statistics() = default;

    void ShowStatistics(bool isWon) {
        double time_spent = TimeSpent();
        int score = CountScore(time_spent);

        cout << "Загаданное слово: " << word_->ToString() << endl;
        cout << "Затрачено попыток: " << attempts_ << endl;
        cout << "Затрачено времени: " << time_spent << " секунд" << endl;

        if (isWon) {
            count_correct_words_++;
            cout << "Итоговый счёт: " << score << endl;
            score_all_ += score;
        }
        else {
            cout << "Вы проиграли, итоговый счёт: " << 0 << endl;
        }

        time_spent_all_ += time_spent;
        attempts_all_ += attempts_;
        count_words_all_++;
    }

    double TimeSpent() {
        return (clock() - time_start_)/ CLOCKS_PER_SEC;
    }

    int CountScore(double time_spent) {
        if (attempts_ > 0 && time_spent > 0) {
            return static_cast<int>((word_->Length() * 10000) / (attempts_ * time_spent));
        }
        return 0;
    }

    void PlusAttempt() {
        attempts_++;
    }

    static void DisplayAllStat() {
        cout << "Количество угаданных слов: " << count_correct_words_ << " из " << count_words_all_<< endl;
        cout << "Всего затрачено попыток: " << attempts_all_ << endl;
        cout << "Всего затрачено времени: " << time_spent_all_ << " секунд" << endl;
        cout << "Конечный счёт: " << score_all_ << endl;
    }

    void DisplayLeaderBoard() {
        leaderboard_.DisplayTopScores();
    }

    void AddUser(const string& username, int score) {
        leaderboard_.AddScore(username, score);
    }
};

class Game {
  private:
    Statistics statistics_;
    Word secretWord_;
    int remainingGuesses_;
    bool isWon_;
    vector<char> usedLetters_;
    static vector<string> pictures_;
    int state_;

  public:
    Game() = default;

    void ChangeState(const string& wordToGuess) {
        secretWord_ = Word(wordToGuess);
        statistics_ = Statistics(&secretWord_);
        remainingGuesses_ = 6;
        isWon_ = false;
        state_ = 0;
        usedLetters_.clear();
    }

    void Guess(char& guess) {
        statistics_.PlusAttempt();
        if (find(usedLetters_.begin(), usedLetters_.end(), guess) != usedLetters_.end()) {
            cout << "Вы уже предлагали букву " << guess << ". Попробуйте другую." << endl;
            return;
        }

        usedLetters_.push_back(guess);

        bool isCorrect = false;
        for (Letter& letter : secretWord_.GetLetters()) {
            if (letter.GetValue() == guess) {
                letter.ReverseRevealing();
                isCorrect = true;
            }
        }

        if (!isCorrect) {
            remainingGuesses_--;
            state_++;
        }

        isWon_ = CheckWin();
    }

    bool CheckWin() {
        for (Letter& letter : secretWord_.GetLetters()) {
            if (!letter.GetRevealed()) {
                return false;
            }
        }
        return true;
    }

    void DisplayWord() {
        for (Letter& letter : secretWord_.GetLetters()) {
            if (letter.GetRevealed()) {
                cout << letter.GetValue() << " ";
            }
            else {
                cout << "_ ";
            }
        }
        cout << endl;
    }

    void DisplayHangman() {
        cout << pictures_[state_] << endl;
    }

    void DisplayStatus() {
        system("cls");
        cout << "Оставшиеся попытки: " << remainingGuesses_ << endl;

        cout << "Использованные буквы: ";
        DisplayUsedLetters();

        DisplayWord();
        DisplayHangman();

        if (isWon_) {
            cout << "Поздравляем! Вы угадали слово!" << endl;
            statistics_.ShowStatistics(true);
        }
        else if (remainingGuesses_ <= 0) {
            cout << "Игра окончена. Вы не смогли угадать слово." << endl;
            statistics_.ShowStatistics(false);
        }
    }

    void DisplayUsedLetters() {
        cout << "Использованные буквы: ";
        for (char& usedLetter : usedLetters_) {
            cout << usedLetter << " ";
        }
        cout << endl;
    }

    void ShowLeaderboard() {
        statistics_.DisplayLeaderBoard();
    }

    void StartGame(const string& word) {
        ChangeState(word);
        DisplayStatus();

        while (!isWon_ && remainingGuesses_ > 0) {
            char guess;
            cout << "Введите букву: ";
            cin >> guess;   
            Guess(guess);
            DisplayStatus();
        }
    }

    void ExitGame() {
        statistics_.DisplayAllStat();
        cout << "Выход из игры..." << endl;
        exit(0);
    }

    void AddUser(const string& username, int score) {
        statistics_.AddUser(username, score);
    }
};

class Button {
  private:
    string name_;
    bool IsSelected_;
    function<void()> onClick_;

  public:
    Button(string name, function<void()> onClick) : name_(name), onClick_(onClick), IsSelected_(false) {};

    void click() {
        onClick_();
    }

    void display() {
        if (IsSelected_) {
            cout << "\033[1;33m" << name_ << "\033[0m" << endl; 
        }
        else {
            cout << name_ << endl;
        }
    }

    void select() { IsSelected_ = true; }
    void deselect() { IsSelected_ = false; }
};

class Menu {
  private:
    vector<Button> buttons_;
    string title_;
    Game* game_;
    int currentSelection_;

  public:
    Menu(string title, initializer_list<Button> buttons, Game* game) : title_(title), buttons_(buttons), game_(game), currentSelection_(0) {};

    void display() {
        cout << title_ << endl;
        buttons_[currentSelection_].select();
        for (int i = 0; i < buttons_.size(); ++i) {
            cout << i + 1 << ". ";
            buttons_[i].display();
        }
        Input();
    }

     void clickButton(int index) {
         if (index >= 0 and index < buttons_.size()) {
             buttons_[index].click();
         } else {
             throw "Error";
         }
    }

    void changeSelection(int input) {
        buttons_[currentSelection_].deselect();

        if (input == 'W' || input == 'w') {
            currentSelection_ = (currentSelection_ > 0) ? currentSelection_ - 1 : buttons_.size() - 1;
        }
        else if (input == 'S' || input == 's') {
            currentSelection_ = (currentSelection_ + 1) % buttons_.size();
        }
        else if (input == 13) {
            clickButton(currentSelection_); 
        }

        buttons_[currentSelection_].select();
    }

    void Input() {
        int input;
        while ((input = _getch()) != 13) {
            changeSelection(input);
            system("cls");
            display();
        }

        system("cls");
        changeSelection(input);
    }
};

class CommonMenu {
  private:
    Game game_;
    File file_;
    Menu mainMenu;
    Menu settingsMenu;

  public:
    CommonMenu(Game game, File file) :
        game_(game), file_(file),
        mainMenu("Виселица!", { Button("Начать игру", [this] { StartGame_Menu(); }),
                                Button("Настройки", [this] { displaySettingsMenu(); }),
                                Button("Выход", [this] { game_.ExitGame(); }) }, &game_),
        settingsMenu("Настройки", { Button("Ввести новое слово в словарь", [this] { Request_addWordToFile_Menu(); }),
                                    Button("Рейтинг", [this] { game_.ShowLeaderboard(); }),
                                    Button("Вернуться в главное меню", [this] { displayMainMenu(); }) }, &game_) {};

    void displayMainMenu() {
        mainMenu.display();
    }

    void displaySettingsMenu() {
        settingsMenu.display();
    }

    void Request_addWordToFile_Menu() {
        file_.Request_addWordToFile();
        displaySettingsMenu();
    }

    void StartGame_Menu() {
        game_.StartGame(file_.GetWord());
        displayMainMenu();
    }

    void ExitGame_Menu() {
        game_.ExitGame();
        /*game_.AddUser()*/
    }
};

class User {
  private:
    string nickname_;
    CommonMenu menu_;

  public:
    User(const string& nickname, const string& file) : nickname_(nickname), menu_(Game(), File(file)) {};

    void Start() {
        menu_.displayMainMenu();
    }
};


vector<string> Game::pictures_ = {
        "  +---+\n  |   |\n      |\n      |\n      |\n      |\n=========",
        "  +---+\n  |   |\n  O   |\n      |\n      |\n      |\n=========",
        "  +---+\n  |   |\n  O   |\n  |   |\n      |\n      |\n=========",
        "  +---+\n  |   |\n  O   |\n /|   |\n      |\n      |\n=========",
        "  +---+\n  |   |\n  O   |\n /|\\  |\n      |\n      |\n=========",
        "  +---+\n  |   |\n  O   |\n /|\\  |\n /    |\n      |\n=========",
        "  +---+\n  |   |\n  O   |\n /|\\  |\n / \\  |\n      |\n========="
};

map<string, int> Leaderboard::scores_;
double Statistics::time_spent_all_ = 0;
int Statistics::attempts_all_ = 0;
int Statistics::score_all_ = 0;
int Statistics::count_words_all_ = 0;
int Statistics::count_correct_words_ = 0;
int WordDecryptor::cipher_ = 5;

int main() {
    srand(time(0));
    setlocale(LC_ALL, "Russian");

    User user("lox", "words.txt");
    user.Start();

    return 0;
}
