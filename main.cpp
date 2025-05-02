#include <SDL2/SDL.h>  // SDL2 ka header for graphics
#include <iostream>    // Input-output ke liye
#include <vector>      // Vectors use karne ke liye
#include <random>      // Random number generator ke liye
#include <ctime>       // Time seed ke liye
#include <queue>       // Queue use karne ke liye (abhi isme priority queue use nahi ho rahi)
#include <algorithm>   // STL functions jaise remove, find etc.
#include <cmath>       // Math functions jaise abs()

// Screen aur grid ke dimensions
const int WIDTH = 600;          // Window width (600x600)
const int ROWS = 50;            // Grid rows aur columns (50x50)
const int GRID_SIZE = WIDTH / ROWS;  // Har cell ka size calculate kiya

// Kuch predefined colors (RGBA format)
SDL_Color WHITE = {255, 255, 255, 255};
SDL_Color GREY = {128, 128, 128, 255};
SDL_Color BLACK = {0, 0, 0, 255};
SDL_Color ORANGE = {255, 165, 0, 255};
SDL_Color TURQUOISE = {64, 224, 208, 255};
SDL_Color GREEN = {0, 255, 0, 255};
SDL_Color RED = {255, 0, 0, 255};
SDL_Color PURPLE = {128, 0, 128, 255};
SDL_Color LAVENDER = {230, 230, 250, 255};  // Path ke liye lavender color

// Node class har grid cell ko represent karta hai
class Node {
public:
    int row, col;              // Node ki position
    SDL_Rect rect;             // Drawing ke liye SDL rectangle
    SDL_Color color;           // Node ka color
    int gCost, hCost, fCost;   // A* ke costs
    Node* parent;              // Path tracing ke liye parent node

    // Constructor: Node initialize karo
    Node(int r, int c, int width, int height)
        : row(r), col(c), color(WHITE), gCost(0), hCost(0), fCost(0), parent(nullptr) {
        rect.x = col * width;  // X position calculate karo
        rect.y = row * height; // Y position calculate karo
        rect.w = width;        // Width set karo
        rect.h = height;       // Height set karo
    }

    // Node ka color set karne ka method
    void setColor(SDL_Color col) {
        color = col;
    }

    // Node draw karo renderer pe
    void draw(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    // fCost getter
    int getFCost() const { return fCost; }
};

// Color comparison ke liye overload
bool operator==(const SDL_Color& c1, const SDL_Color& c2) {
    return (c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b) && (c1.a == c2.a);
}
bool operator!=(const SDL_Color& c1, const SDL_Color& c2) {
    return !(c1 == c2);
}

// Manhattan distance function for heuristic
int getHeuristic(Node* a, Node* b) {
    return abs(a->row - b->row) + abs(a->col - b->col);
}

// A* algorithm ka implementation
std::vector<Node*> aStar(std::vector<std::vector<Node>>& grid, Node* start, Node* end, SDL_Renderer* renderer) {
    std::vector<Node*> openList;     // Open list
    std::vector<Node*> closedList;   // Closed list
    std::vector<Node*> path;         // Final path

    openList.push_back(start);  // Start node ko open list me daalo

    while (!openList.empty()) {
        Node* currentNode = openList[0];

        // Sabse kam fCost wala node dhoondo
        for (Node* node : openList) {
            if (node->getFCost() < currentNode->getFCost() ||
                (node->getFCost() == currentNode->getFCost() && node->hCost < currentNode->hCost)) {
                currentNode = node;
            }
        }

        // currentNode ko open se hata ke closed me daal do
        openList.erase(std::remove(openList.begin(), openList.end(), currentNode), openList.end());
        closedList.push_back(currentNode);

        // Agar goal mil gaya to path trace karo
        if (currentNode == end) {
            Node* temp = currentNode;
            while (temp != nullptr) {
                path.push_back(temp);
                temp = temp->parent;
            }
            std::reverse(path.begin(), path.end());  // Path reverse karo (start se end tak)
            break;
        }

        // 4 neighbors check karo (up, down, left, right)
        std::vector<Node*> neighbors;
        if (currentNode->row > 0) neighbors.push_back(&grid[currentNode->row - 1][currentNode->col]);
        if (currentNode->row < ROWS - 1) neighbors.push_back(&grid[currentNode->row + 1][currentNode->col]);
        if (currentNode->col > 0) neighbors.push_back(&grid[currentNode->row][currentNode->col - 1]);
        if (currentNode->col < ROWS - 1) neighbors.push_back(&grid[currentNode->row][currentNode->col + 1]);

        for (Node* neighbor : neighbors) {
            if (std::find(closedList.begin(), closedList.end(), neighbor) != closedList.end()) continue;
            if (neighbor->color == BLACK) continue;  // Wall ko ignore karo

            int tentativeGCost = currentNode->gCost + 1;
            bool inOpenList = std::find(openList.begin(), openList.end(), neighbor) != openList.end();

            if (!inOpenList || tentativeGCost < neighbor->gCost) {
                neighbor->gCost = tentativeGCost;
                neighbor->hCost = getHeuristic(neighbor, end);
                neighbor->fCost = neighbor->gCost + neighbor->hCost;
                neighbor->parent = currentNode;

                if (!inOpenList) {
                    openList.push_back(neighbor);
                }
            }

            neighbor->setColor(RED);  // Animate karo node ko
        }

        // Render sab nodes
        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
        SDL_RenderClear(renderer);
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < ROWS; j++) {
                grid[i][j].draw(renderer);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(30);  // Animation delay
    }

    // Final path lavender color me draw karo (animated)
    for (Node* node : path) {
        node->setColor(LAVENDER);
        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
        SDL_RenderClear(renderer);
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < ROWS; j++) {
                grid[i][j].draw(renderer);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(100);  // Animation delay for path
    }

    return path;
}

// Random maze generation function
void generateRandomMaze(std::vector<std::vector<Node>>& grid, Node* start, Node* end) {
    std::mt19937 gen(static_cast<unsigned int>(std::time(0)));
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Sab nodes ko white karo
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < ROWS; j++) {
            Node& node = grid[i][j];
            if (&node != start && &node != end) {
                node.setColor(WHITE);
            }
        }
    }

    // Randomly walls create karo
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < ROWS; j++) {
            Node& node = grid[i][j];
            if (&node != start && &node != end) {
                if (dis(gen) < 0.3) {
                    node.setColor(BLACK);
                }
            }
        }
    }
    std::cout << "Random maze generated." << std::endl;
}

int main(int argc, char* argv[]) {
    // SDL initialize karo
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // SDL window banao
    SDL_Window* window = SDL_CreateWindow("Pathfinding Algorithm Visualizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, WIDTH, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // SDL renderer banao
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Grid banate hain
    std::vector<std::vector<Node>> grid;
    for (int i = 0; i < ROWS; i++) {
        std::vector<Node> row;
        for (int j = 0; j < ROWS; j++) {
            row.emplace_back(i, j, GRID_SIZE, GRID_SIZE);
        }
        grid.push_back(row);
    }

    Node* start = nullptr;
    Node* end = nullptr;
    bool running = true;

    // Main loop
    while (running) {
        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
        SDL_RenderClear(renderer);

        // Event handle karo
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_m) {
                    generateRandomMaze(grid, start, end);  // Maze generate karo
                } else if (e.key.keysym.sym == SDLK_SPACE && start && end) {
                    std::vector<Node*> path = aStar(grid, start, end, renderer);
                    for (Node* node : path) {
                        node->setColor(LAVENDER);  // Path ko lavender me dikhao
                    }
                    std::cout << "Pathfinding complete!" << std::endl;
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int row = y / GRID_SIZE;
                int col = x / GRID_SIZE;
                Node& clickedNode = grid[row][col];

                // Start, end aur wall set karo
                if (!start) {
                    start = &clickedNode;
                    start->setColor(ORANGE);
                } else if (!end) {
                    end = &clickedNode;
                    end->setColor(TURQUOISE);
                } else if (clickedNode.color != ORANGE && clickedNode.color != TURQUOISE) {
                    clickedNode.setColor(BLACK);  // Wall bana do
                }
            }
        }

        // Grid draw karo
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < ROWS; j++) {
                grid[i][j].draw(renderer);
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
