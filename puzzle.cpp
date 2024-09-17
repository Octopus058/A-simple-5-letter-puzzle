#include <bits/stdc++.h>
#include "termcolor.hpp"
using namespace std;

const int GRID_SIZE = 7;
int hintCount = 0;

vector<vector<char>> AnswerGrid(GRID_SIZE, vector<char>(GRID_SIZE, ' '));
vector<vector<string>> colorGrid(GRID_SIZE, vector<string>(GRID_SIZE, "white"));
vector<vector<char>> PlayerGrid(GRID_SIZE, vector<char>(GRID_SIZE, ' '));
vector<vector<string>> PlayerColorGrid(GRID_SIZE, vector<string>(GRID_SIZE, "white"));
vector<string> words;
vector<string> currentWords;
map<string, string> wordColors;

void initGrid();
void isValidPlacement();
void fillGrid();
void initPlayerGrid();
void printGrid();
void printPlayerGrid();
bool checkWin();
void Welcome();
void Menu();
void playerInput();

// 从文件中读取单词
vector<string> fetchWords()
{
    vector<string> words;
    ifstream file("5words.txt");
    string word;

    // 先将所有单词读入到 vector 中
    while (getline(file, word))
    {
        words.push_back(word);
    }
    file.close();

    // 随机打乱 words 中的单词
    random_device rd;
    mt19937 g(rd());
    shuffle(words.begin(), words.end(), g);

    return words;
}

// 初始化网格
void initGrid()
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            AnswerGrid[i][j] = ' ';
        }
    }
}

// 检查单词是否可以被放置
bool isValidPlacement(const string &word, int row, int col, bool isHorizontal)
{
    // 如果单词水平，并且长度加上列的位置超过网格大小，则不能被放置
    if (isHorizontal && col + word.size() > GRID_SIZE)
        return false;
    // 同理
    if (!isHorizontal && row + word.size() > GRID_SIZE)
        return false;

    for (size_t i = 0; i < word.size(); i++)
    {
        if (isHorizontal)
        {
            // 如果当前位置已有字母，并且这个字母不是当前单词的字母，则不能被放置
            if (AnswerGrid[row][col + i] != ' ' && AnswerGrid[row][col + i] != word[i])
                return false;
        }
        else
        {
            if (AnswerGrid[row + i][col] != ' ' && AnswerGrid[row + i][col] != word[i])
                return false;
        }
    }
    return true;
}

// 填充网格
void fillGrid(const vector<string> &words)
{
    // 用不同的颜色填充单词
    vector<string> colors = {"red", "blue", "green", "yellow", "cyan", "purple"};
    int colorIndex = 0;
    for (int i = 0; i < 6; i++)
    {
        string word = words[i];
        currentWords.push_back(word);
        transform(word.begin(), word.end(), word.begin(), ::tolower); // 转换为小写
        bool placed = false;

        // 最多尝试1000次，如果1000次都没有找到合适的位置，就放弃
        for (int tries = 0; tries < 1000 && !placed; tries++)
        {
            int row = rand() % GRID_SIZE;
            int col = rand() % GRID_SIZE;
            bool isHorizontal = rand() % 2 == 0; // 随机选择横向或纵向

            if (isValidPlacement(word, row, col, isHorizontal))
            {
                string color = colors[colorIndex];
                colorIndex = (colorIndex + 1) % colors.size();

                wordColors[word] = color; // 存储单词的颜色

                // 将单词填入网格
                for (size_t i = 0; i < word.length(); i++)
                {
                    if (isHorizontal)
                    {
                        AnswerGrid[row][col + i] = word[i];
                        colorGrid[row][col + i] = color;
                    }
                    else
                    {
                        AnswerGrid[row + i][col] = word[i];
                        colorGrid[row + i][col] = color;
                    }
                }
                placed = true;
            }
        }
    }

    // 随机填入10个无关的字母
    for (int i = 0; i < 10; i++)
    {
        int row, col;
        do
        {
            row = rand() % GRID_SIZE;
            col = rand() % GRID_SIZE;
        } while (colorGrid[row][col] != "white"); // 如果这个位置已经被单词占用，就重新选择位置

        char letter = 'a' + rand() % 26; // 随机选择一个小写字母
        AnswerGrid[row][col] = letter;
    }
}

// 初始化玩家网格
void initPlayerGrid()
{
    // 创建一个随机数生成器
    std::default_random_engine generator;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            PlayerGrid[i][j] = ' ';
            PlayerColorGrid[i][j] = "";
        }
    }

    // 收集所有单词的字母
    map<string, vector<pair<int, int>>> wordCells;
    for (const auto &word : currentWords)
    {
        string color = wordColors[word]; // 获取单词的颜色
        for (int i = 0; i < GRID_SIZE; i++)
        {
            for (int j = 0; j < GRID_SIZE; j++)
            {
                if (AnswerGrid[i][j] != ' ' && colorGrid[i][j] == color)
                {
                    wordCells[word].push_back({i, j});
                }
            }
        }
    }

    // 为每个单词选择两个字母显示在玩家网格中
    for (const auto &word : currentWords)
    {
        shuffle(wordCells[word].begin(), wordCells[word].end(), generator);
        for (int i = 0; i < min(2, (int)wordCells[word].size()); i++)
        {
            int row = wordCells[word][i].first;
            int col = wordCells[word][i].second;
            PlayerGrid[row][col] = AnswerGrid[row][col];
            PlayerColorGrid[row][col] = colorGrid[row][col];
        }
    }

    // 收集所有无关的字母
    vector<pair<int, int>> unrelatedCells;
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (colorGrid[i][j] == "white")
            {
                unrelatedCells.push_back({i, j});
            }
        }
    }

    // 随机选择2个无关的字母显示在玩家网格中
    shuffle(unrelatedCells.begin(), unrelatedCells.end(), generator);
    for (int i = 0; i < min(2, (int)unrelatedCells.size()); i++)
    {
        int row = unrelatedCells[i].first;
        int col = unrelatedCells[i].second;
        PlayerGrid[row][col] = AnswerGrid[row][col];
        PlayerColorGrid[row][col] = colorGrid[row][col];
    }
}
// 打印网格
void printGrid()
{
    cout << "     ";
    for (int i = 0; i < GRID_SIZE; i++)
    {
        cout << i + 1 << "   ";
    }
    cout << endl;

    cout << "   ";
    for (int i = 0; i < GRID_SIZE; i++)
    {
        cout << "+---";
    }
    cout << "+" << endl;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        cout << " " << i + 1 << " ";
        for (int j = 0; j < GRID_SIZE; j++)
        {
            cout << "| ";
            // 根据颜色填充单词
            if (colorGrid[i][j] == "red")
            {
                cout << termcolor::red << AnswerGrid[i][j] << termcolor::reset;
            }
            else if (colorGrid[i][j] == "blue")
            {
                cout << termcolor::blue << AnswerGrid[i][j] << termcolor::reset;
            }
            else if (colorGrid[i][j] == "green")
            {
                cout << termcolor::green << AnswerGrid[i][j] << termcolor::reset;
            }
            else if (colorGrid[i][j] == "yellow")
            {
                cout << termcolor::yellow << AnswerGrid[i][j] << termcolor::reset;
            }
            else if (colorGrid[i][j] == "cyan")
            {
                cout << termcolor::cyan << AnswerGrid[i][j] << termcolor::reset;
            }
            else if (colorGrid[i][j] == "purple")
            {
                cout << termcolor::magenta << AnswerGrid[i][j] << termcolor::reset;
            }
            else
            {
                cout << AnswerGrid[i][j];
            }
            cout << " ";
        }
        cout << "|" << endl;

        cout << "   ";
        for (int j = 0; j < GRID_SIZE; j++)
        {
            cout << "+---";
        }
        cout << "+" << endl;
    }
}

// 打印玩家网格
void printPlayerGrid()
{
    cout << "     ";
    for (int i = 0; i < GRID_SIZE; i++)
    {
        cout << i + 1 << "   ";
    }
    cout << endl;

    cout << "   ";
    for (int i = 0; i < GRID_SIZE; i++)
    {
        cout << "+---";
    }
    cout << "+" << endl;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        cout << " " << i + 1 << " ";
        for (int j = 0; j < GRID_SIZE; j++)
        {
            cout << "| ";
            // 根据颜色填充单词
            string color = PlayerColorGrid[i][j];
            if (color == "red")
            {
                cout << termcolor::red << PlayerGrid[i][j] << termcolor::reset;
            }
            else if (color == "blue")
            {
                cout << termcolor::blue << PlayerGrid[i][j] << termcolor::reset;
            }
            else if (color == "green")
            {
                cout << termcolor::green << PlayerGrid[i][j] << termcolor::reset;
            }
            else if (color == "yellow")
            {
                cout << termcolor::yellow << PlayerGrid[i][j] << termcolor::reset;
            }
            else if (color == "cyan")
            {
                cout << termcolor::cyan << PlayerGrid[i][j] << termcolor::reset;
            }
            else if (color == "purple")
            {
                cout << termcolor::magenta << PlayerGrid[i][j] << termcolor::reset;
            }
            else
            {
                cout << PlayerGrid[i][j];
            }
            cout << " ";
        }
        cout << "|" << endl;

        cout << "   ";
        for (int j = 0; j < GRID_SIZE; j++)
        {
            cout << "+---";
        }
        cout << "+" << endl;
    }
}

// 检查是否赢得游戏
bool checkWin()
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (colorGrid[i][j] != "white" && PlayerGrid[i][j] != AnswerGrid[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

// 欢迎界面
void Welcome()
{
    cout << "Welcome to the Word Puzzle Game developed by Oct0pu5!" << endl;
    cout << "You will be provided with a grid of letters. You need to find the hidden words(in a row or a column) in the grid." << endl
         << endl;
    cout << "Notice:" << endl;
    cout << "   1. The color of each letter in a word is the same (except for overlapping cases, randomly display one color)." << endl;
    cout << "   2. The  word is case-insensitive." << endl;
    cout << "   3. The word is placed horizontally or vertically." << endl;
    cout << "Here are some tips:" << endl;
    cout << "   1. You can enter the number of row and column and the letter to fill the grid." << endl;
    cout << "   2. You can also enter '+' to pause the game, enter the menu and 'R/r' to resume." << endl;
    cout << "   3. You can enter 'hint' in the menu to see the hints." << endl;
    cout << "   4. You can enter 'exit' in the menu to exit the game." << endl;
    cout << "Please press S/s to start the game." << endl;
Z:
    string input;
    cin >> input;
    if (input == "S" || input == "s")
    {
        return;
    }
    else
    {
        cout << "Invalid input, please input again." << endl;
        goto Z;
    }
}

// 菜单
void Menu()
{
    cout << endl
         << "Enter 'hint' to see the hints." << endl;
    cout << "Enter 'exit' to see the answer and exit the game." << endl;
    cout << "Enter 'R/r' to resume the game." << endl
         << endl;

    string input;
    cin >> input;
    if (input == "hint")
    {
        // 最多给3个提示
        if (hintCount < 3)
        {
            int randomIndex = rand() % currentWords.size();
            cout << "Hint: " << currentWords[randomIndex] << endl;
            hintCount++;
        }
        else
        {
            cout << "You have used all your hints." << endl;
        }
        Menu();
    }
    else if (input == "exit")
    {
        cout << endl;
        printGrid();

        cout << "The words are: ";
        for (int i = 0; i < currentWords.size(); i++)
        {
            cout << currentWords[i] << " ";
        }
        cout << endl
             << endl;
        cout << "Thanks for playing!" << endl;
        system("pause");
        exit(0);
    }
    else if (input == "R" || input == "r")
    {
        cout << "Game Resumed!" << endl;
        printPlayerGrid();
        playerInput();
    }
    else
    {
        cout << "Invalid input, please input again." << endl;
        Menu();
    }
}

// 玩家输入
void playerInput()
{
    if (checkWin())
    {
        cout << "Congratulations! You have won the game!" << endl;
        return;
    }

    int row, col;
    char letter;

    while (true)
    {
        string input;

        cout << "Please enter the letter (Enter '+' to pause):" << endl;
    A:
        cin >> input;

        if (input == "+")
        {
            Menu();
        }
        if (isalpha(input[0])) // 检查输入是否为字母
        {
            letter = input[0];
        }
        else
        {
            cout << "Invalid input. Please enter a letter." << endl;
            goto A;
        }

        cout << "Please enter the row and column:" << endl;
        int ret = scanf("%d %d", &row, &col);

        while ((getchar()) != '\n')
            ;

        // 将行列从1-based转换为0-based
        row--;
        col--;

        // 检查输入是否有效
        if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE)
        {
            break;
        }
        else
        {
            cout << "Invalid row or column. Please input again." << endl;
        }
    }

    // 更新玩家网格
    PlayerGrid[row][col] = letter;

    // 如果玩家输入的字母与答案网格中的字母匹配，更新玩家颜色网格
    if (AnswerGrid[row][col] == letter)
    {
        PlayerColorGrid[row][col] = colorGrid[row][col];
    }

    printPlayerGrid();
    playerInput();
}

// 主函数
int main()
{
    srand(time(NULL));
    initGrid();
    vector<string> words = fetchWords();
    fillGrid(words);
    initPlayerGrid();

    Welcome();
    printPlayerGrid();
    playerInput();

    system("pause");
    return 0;
}