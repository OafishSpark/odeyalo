#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "blanket.h"

int main(int argc, char *argv[]) {
  try {
    // init stage
    int num_of_colors{0};
    std::vector<int> colors(0);
    int num_of_rows{0}, num_of_cols{0};

    // input stage
    if (argc > 4) {
      num_of_rows = std::stoi(argv[1]);
      num_of_cols = std::stoi(argv[2]);
      num_of_colors = std::stoi(argv[3]);
      colors.resize(num_of_colors);
      assert(argc == 4 + num_of_colors);
      for (int iv{0}; iv < num_of_colors; ++iv) {
        colors[iv] = std::stoi(argv[3 + iv]);
      }
    } else {
      num_of_colors = 1;
      colors = std::vector<int>(1, 1);
      num_of_rows = 1;
      num_of_cols = 1;
    }
    // aproximation stage
    int sum{0};
    for (const auto &elem : colors) {
      sum += elem;
    }
    if (sum < num_of_rows * num_of_cols) {
      throw std::invalid_argument(
          "It is impossible to color blanket with such a colors");
    }

    Blanket blanket(num_of_rows, num_of_cols);
    
    blanket.Coloring(colors);

    blanket.Print();

    auto [x_wrong, y_wrong] = blanket.CheckIfCorrectBlanket();
    assert(x_wrong == -1 && y_wrong == -1);

    blanket.PrintStat();

    blanket.RandomizeColoring();

    blanket.Print();

    auto [x_w, y_w] = blanket.CheckIfCorrectBlanket();
    assert(x_w == -1 && y_w == -1);

    blanket.PrintStat();
    return 0;
  } catch (std::invalid_argument &e) {
    std::cout << e.what() << std::endl;
    return -1;
  }
}
