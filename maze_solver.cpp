#include <stdlib.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>

#define NOT_IN_QUEUE 0
#define IN_QUEUE 1
#define PROCESSED 2

class node {
 public:
  node() : coord{0, 0}, val{0}, weight{INFINITY}, state{NOT_IN_QUEUE} {};
  node(int x, int y)
      : coord{x, y},
        val{0},
        parent{NULL},
        weight{INFINITY},
        state{NOT_IN_QUEUE} {};
  node(int x, int y, int v)
      : coord{x, y},
        val{v},
        parent{NULL},
        weight{INFINITY},
        state{NOT_IN_QUEUE} {};
  node(int x, int y, int v, node *p)
      : coord{x, y},
        val{v},
        parent{p},
        weight{INFINITY},
        state{NOT_IN_QUEUE} {};
  node(int x, int y, int v, node *p, float w)
      : coord{x, y}, val{v}, parent{p}, weight{w}, state{NOT_IN_QUEUE} {};

  int coord[2];
  node *parent;
  float weight;
  int val, state;

  int x() const { return coord[0]; }
  int y() const { return coord[1]; }
};

inline float calcDistDist(node *a, node *b) {
  return (a->x() - b->x()) * (a->x() - b->x()) +
         (a->y() - b->y()) * (a->y() - b->y());
  // return fmaxf32(fabsf32(a->x() - b->x()), fabsf32(a->y() - b->y()));
}

int main(int argc, char const *argv[]) {
  std::string line;
  std::cin >> line;

  if (line != "P3") {
    std::cerr << "This program expects a P3 PPM file as input\n";
    exit(1);
  }

  int width;
  int height;
  int maxVal;

  std::cin >> width >> height;
  std::cin >> maxVal;

  if (maxVal > 0xff) {
    std::cerr << "Image maximum value too big!\n";
    exit(1);
  }

  node *maze = (node *)malloc(sizeof(node) * width * height);
  if (maze == NULL) {
    std::cerr << "Failed to alocate memory!\n";
    exit(1);
  }

  node *begin, *end;

  int r, g, b;
  for (unsigned int i = 0; i < width * height; i++) {
    std::cin >> r >> g >> b;
    maze[i].val = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
    maze[i].coord[0] = i % width;
    maze[i].coord[1] = i / width;
    maze[i].weight = INFINITY;
    if (maze[i].val == 0x00ff00) begin = &maze[i];
    if (maze[i].val == 0xff0000) end = &maze[i];
  }

  auto comp = [](node *a, node *b) { return a->weight > b->weight; };
  std::priority_queue<node *, std::vector<node *>, decltype(comp)>
      activeNodeQueue(comp);

  struct timespec start, stop;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  begin->weight = calcDistDist(begin, end);
  activeNodeQueue.push(begin);

  while (!activeNodeQueue.empty()) {
    node *currentNode = activeNodeQueue.top();
    if (currentNode->x() == end->x() && currentNode->y() == end->y())
      break;
    else {
      currentNode->state = 1;
      currentNode->val = 0xaaaa00;

      int x = currentNode->x();
      int y = currentNode->y();
      int xLowerLimit = (x - 1) >= 0 ? (x - 1) : 0;
      int xUpperLimit = (x + 1) < width ? (x + 1) : width;
      int yLowerLimit = (y - 1) >= 0 ? (y - 1) : 0;
      int yUpperLimit = (y + 1) < height ? (y + 1) : height;

      for (int i = yLowerLimit; i <= yUpperLimit; i++) {
        for (int j = xLowerLimit; j <= xUpperLimit; j++) {
          node *tmpNode = &maze[i * width + j];
          if (tmpNode->val != 0) {
            float estimatedDist =
                currentNode->weight + 1.0f + calcDistDist(tmpNode, end);
            if (estimatedDist < tmpNode->weight) {
              tmpNode->parent = currentNode;
              tmpNode->weight = estimatedDist;
              if (tmpNode->state == NOT_IN_QUEUE) {
                tmpNode->state = IN_QUEUE;
                activeNodeQueue.push(tmpNode);
              }
            }
          }
        }
      }
      activeNodeQueue.pop();
      currentNode->state = NOT_IN_QUEUE;
    }
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
  double timer_milisecs =
      ((stop.tv_sec * 1000 * 1000 * 1000 + stop.tv_nsec) -
       (start.tv_sec * 1000 * 1000 * 1000 + start.tv_nsec)) /
      (1000 * 1000);
  std::cerr << "Elapsed time " << timer_milisecs << "ms.\n";

  if (end->parent == NULL) std::cerr << "No path found!\n";

  node *currentNode = end->parent;
  while (currentNode != NULL) {
    currentNode->val = 0x0000ff;
    currentNode = currentNode->parent;
  }

  std::cout << "P3\n";
  std::cout << width << " " << height << "\n";
  std::cout << "255\n";
  for (unsigned int i = 0; i < height * width; i++) {
    int r = (maze[i].val >> 16) & 0xff;
    int g = (maze[i].val >> 8) & 0xff;
    int b = maze[i].val & 0xff;
    std::cout << r << " " << g << " " << b << "\n";
  }

  return 0;
}