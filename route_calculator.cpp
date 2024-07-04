# include <iostream>
# include <vector>
# include <queue>
# include <unordered_set>
# include <cmath>
# include <algorithm>

// Dimensiones del tablero
const int boardHeight = 10;
const int boardWidth = 10;
std::vector<std::vector<int>> board(boardHeight, std::vector<int>(boardWidth, 0));

// Obstáculos predefinidos
std::vector<std::pair<int, int>> obstacles = {
    {1, 1}, {1, 2}, {2, 6},
    {3, 1}, {3, 2}, {3, 3},
    {4, 5}, {5, 6}, {9, 1},
    {6, 1}, {6, 2}, {6, 7},
    {8, 1}, {8, 6}, {9, 4}
};

// Estructura del nodo para el algoritmo A*
struct Node {
    int x, y;
    int g, h, f;
    Node* parent;

    Node(int x, int y, int g, int h, Node* parent = nullptr)
        : x(x), y(y), g(g), h(h), f(g + h), parent(parent) {}
};

// Función de heurística (distancia de Manhattan)
int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Agregar obstáculos al tablero
void addObstaclesToBoard(std::vector<std::vector<int>>& board, const std::vector<std::pair<int, int>>& obstacles) {
    for (const auto& obs : obstacles) {
        if (obs.first < boardWidth && obs.second < boardHeight) {
            board[obs.second][obs.first] = 1;
        } else {
            std::cout << "Obstacle (" << obs.first << ", " << obs.second << ") no esta dentro del rango.\n";
        }
    }
}

// Imprimir el tablero
void printBoard(const std::vector<std::vector<int>>& board) {
    std::cout << "  ";
    for (int i = 0; i < boardWidth ; ++i) {
        std::cout << i << " ";
    }
    std::cout << "\n";
    for (int y = 0; y < boardHeight; ++y) {
        std::cout << y << " ";
        for (int x = 0; x < boardWidth; ++x) {
            if (board[y][x] == 0)
                std::cout << ". ";
            else if (board[y][x] == 1)
                std::cout << "# ";
            else if (board[y][x] == 2) 
                std::cout << "* ";
            else if (board[y][x] == 5)
                std::cout << "I ";
            else if (board[y][x] == 6)
                std::cout << "F ";
        }
        std::cout << '\n';
    }
}

// Verificar si las coordenadas son válidas
bool validCoordinates(int x, int y, const std::vector<std::vector<int>>& board) {
    return x >= 0 && x < boardWidth && y >= 0 && y < boardHeight && board[y][x] == 0;
}

// Agregar obstáculos ingresados por el usuario
void addObstaclesUser(std::vector<std::pair<int, int>>& obstacles) {
    int x, y;
    while (true) {
        std::cout << "Ingrese coordenadas para los obstaculos (o -1 -1 para finalizar): ";
        std::cin >> x >> y;
        if (x == -1 && y == -1) break;
        obstacles.emplace_back(x, y);
    }
}

// Obtener los vecinos de un nodo
std::vector<Node*> getNeighbors(Node* current, int endX, int endY, const std::vector<std::vector<int>>& board) {
    std::vector<Node*> neighbors;
    std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    for (const auto& dir : directions) {
        int newX = current->x + dir.first;
        int newY = current->y + dir.second;
        if (newX >= 0 && newX < boardWidth && newY >= 0 && newY < boardHeight && board[newY][newX] != 1) {
            int g = current->g + 1;
            int h = heuristic(newX, newY, endX, endY);
            neighbors.push_back(new Node(newX, newY, g, h, current));
        }
    }
    return neighbors;
}

// Reconstruir el camino desde el nodo final al inicial
void reconstructPath(Node* endNode, std::vector<std::vector<int>>& board) {
    Node* current = endNode;
    board[current->y][current->x] = 6;
    current = current->parent;

    while (current->parent != nullptr) {
        board[current->y][current->x] = 2;
        current = current->parent;
    }
}

// Algoritmo A*
bool aStar(int startX, int startY, int endX, int endY, std::vector<std::vector<int>>& board) {
    auto cmp = [](Node* left, Node* right) { return left->f > right->f; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> openSet(cmp);
    std::unordered_set<int> closedSet;

    Node* startNode = new Node(startX, startY, 0, heuristic(startX, startY, endX, endY));
    openSet.push(startNode);

    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();

        if (current->x == endX && current->y == endY) {
            reconstructPath(current, board);
            return true;
        }

        int currentHash = current->y * boardWidth + current->x;
        closedSet.insert(currentHash);

        for (Node* neighbor : getNeighbors(current, endX, endY, board)) {
            int neighborHash = neighbor->y * boardWidth + neighbor->x;
            if (closedSet.find(neighborHash) != closedSet.end()) {
                delete neighbor;
                continue;
            }

            openSet.push(neighbor);
        }
    }

    return false;
}

int main() {
    addObstaclesToBoard(board, obstacles);
    std::cout << "El mapa se ve asi actualmente:\n";
    printBoard(board);

    int startX, startY, endX, endY;
    addObstaclesUser(obstacles);
    addObstaclesToBoard(board, obstacles);
    while (true) {
        std::cout << "\nIngrese las coordenadas de inicio: ";
        std::cin >> startX >> startY;
        std::cout << "Ingrese las coordenadas del final: ";
        std::cin >> endX >> endY;

        if (validCoordinates(startX, startY, board) && validCoordinates(endX, endY, board)) {
            break;
        } else {
            std::cout << "\nCoordenadas fuera de rango o en un obstáculo. Vuelva a intentarlo.\n";
        }
    }

    board[startY][startX] = 5;
    board[endY][endX] = 6;

    std::cout << "\nEl inicio es en: " << startX << " " << startY;
    std::cout << "\nEl final es en: " << endX << " " << endY << "\n";

    if (aStar(startX, startY, endX, endY, board)) {
        std::cout << "Ruta encontrada:\n";
    } else {
        std::cout << "No se pudo encontrar una ruta.\n";
    }
    printBoard(board);

    return 0;
}
