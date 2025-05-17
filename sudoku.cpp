#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <string>

using namespace std;
#define N 9


class Solver { 
  public:
    bool isSafe(int grid[N][N], int row, int col, int num) {
        // Check row & column
        for (int x = 0; x < N; x++) {
            if (grid[row][x] == num || grid[x][col] == num)
                return false;
        }
        // Check the 3x3 sub-grid
        int startRow = row - row % 3, startCol = col - col % 3;
        for (int i = 0; i < 3; i++){
            for (int j = 0; j < 3; j++){
                if (grid[startRow + i][startCol + j] == num)
                    return false;
            }
        }
        return true;
    }
    
    bool solveSudoku(int grid[N][N]) {
        for (int row = 0; row < N; row++){
            for (int col = 0; col < N; col++){
                if (grid[row][col] == 0) {  // Empty cell found
                    for (int num = 1; num <= 9; num++){
                        if (isSafe(grid, row, col, num)) {
                            grid[row][col] = num;
                            if (solveSudoku(grid))
                                return true;
                            grid[row][col] = 0;  // backtrack
                        }
                    }
                    return false;
                }
            }
        }
        return true;  // Puzzle solved
    }
    
    void solve(int grid[N][N]) {
        solveSudoku(grid);
    }
 };


class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    
public:
    Button(float x, float y, float width, float height, const string &btnText, const sf::Font &font) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(sf::Color(100, 100, 200));
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);
        
        text.setFont(font);
        text.setString(btnText);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        
        // Center text properly
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width/2.0f,
                      textBounds.top + textBounds.height/2.0f);
        text.setPosition(x + width/2.0f, y + height/2.0f);
    }
    
    void draw(sf::RenderWindow &window) {
        window.draw(shape);
        window.draw(text);
    }
    
    bool isClicked(sf::Vector2i mousePos) {
        return shape.getGlobalBounds().contains(mousePos.x, mousePos.y);
    }
};


// Sudoku Class (Modified with Static Random Generator)

class Sudoku { 
public:
    int grid[N][N];
    static mt19937 gen;  // Static random generator
    
    Sudoku() {
        generatePuzzle(25);  // Generate a puzzle with 25 clues
    }
    
    void draw(sf::RenderWindow &window, const sf::Font &font) {
        const float cellSize = 60.f;
        sf::Vector2f offset(50, 50);
        
        sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
        cell.setOutlineThickness(1);
        cell.setOutlineColor(sf::Color::Black);
        
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                cell.setPosition(offset.x + j * cellSize, offset.y + i * cellSize);
                cell.setFillColor(sf::Color::White);
                window.draw(cell);
                if (grid[i][j] != 0) {
                    sf::Text text;
                    text.setFont(font);
                    text.setString(to_string(grid[i][j]));
                    text.setCharacterSize(30);
                    text.setFillColor(sf::Color::Black);
                    text.setPosition(offset.x + j * cellSize + 15, offset.y + i * cellSize + 10);
                    window.draw(text);
                }
            }
        }
        
        // Draw thick lines for 3x3 boxes
        sf::RectangleShape line;
        line.setFillColor(sf::Color::Black);
        // Horizontal lines
        for (int i = 0; i <= 3; i++){
            line.setSize(sf::Vector2f(cellSize * N, 3));
            line.setPosition(offset.x, offset.y + i * 3 * cellSize);
            window.draw(line);
        }
        // Vertical lines
        for (int i = 0; i <= 3; i++){
            line.setSize(sf::Vector2f(3, cellSize * N));
            line.setPosition(offset.x + i * 3 * cellSize, offset.y);
            window.draw(line);
        }
    }
    
    void generatePuzzle(int numClues) {
        createSolvedGrid();
        removeCells(81 - numClues);
    }
    
private:
    void createSolvedGrid() {
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                grid[i][j] = 0;
            }
        }
        fillDiagonalBoxes();
        Solver solver;
        solver.solve(grid);
    }
    
    void fillDiagonalBoxes() {
        for (int box = 0; box < 3; box++){
            vector<int> nums = {1,2,3,4,5,6,7,8,9};
            shuffle(nums.begin(), nums.end(), gen);  // Use static gen
            for (int i = 0; i < 3; i++){
                for (int j = 0; j < 3; j++){
                    grid[box * 3 + i][box * 3 + j] = nums[i * 3 + j];
                }
            }
        }
    }
    
    void removeCells(int count) {
        vector<pair<int,int>> cells;
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                cells.push_back({i, j});
            }
        }
        shuffle(cells.begin(), cells.end(), gen);  // Use static gen
        for (int k = 0; k < count; k++){
            grid[cells[k].first][cells[k].second] = 0;
        }
    }
};

// Initialize static random generator
mt19937 Sudoku::gen = mt19937(random_device()());


int main() {
    sf::RenderWindow window(sf::VideoMode(700, 700), "Sudoku Solver");
    
    // Load font once
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Error loading font! Place arial.ttf in executable directory." << endl;
        return -1;
    }

    Sudoku sudoku;
    Solver solver;
    
    // Position buttons at bottom center
    Button newPuzzleButton(200, 620, 150, 50, "New Puzzle", font);
    Button solveButton(400, 620, 150, 50, "Solve", font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (newPuzzleButton.isClicked(mousePos)) {
                    sudoku = Sudoku();
                }
                if (solveButton.isClicked(mousePos)) {
                    solver.solve(sudoku.grid);
                }
            }
        }
        
        window.clear(sf::Color(200, 200, 200));
        sudoku.draw(window, font);
        newPuzzleButton.draw(window);
        solveButton.draw(window);
        window.display();
    }
    return 0;
}