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
    num_of_colors = 5;
    colors = std::vector<int>({100, 100, 100, 100, 100});
    num_of_rows = 15;
    num_of_cols = 15;

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

    std::cout << "Blanket current state is:\n";
    blanket.Print();

    auto [x_wrong, y_wrong] = blanket.CheckIfCorrectBlanket();
    std::cout << "Incorrect Element: (" << x_wrong << ", " << y_wrong << ")\n";
    std::cout << "If both nums in the line above are equal to -1, then blanket "
                "is correct\n";

    blanket.PrintStat();

    std::cout << "\nRandom Coloring!\n";
    blanket.RandomizeColoring();

    std::cout << "Blanket current state is:\n";
    blanket.Print();

    auto [x_w, y_w] = blanket.CheckIfCorrectBlanket();
    std::cout << "Incorrect Element: (" << x_w << ", " << y_w << ")\n";
    std::cout << "If both nums in the line above are equal to -1, then blanket "
                "is correct\n";

    blanket.PrintStat();

    std::cout << "\nThat is all, folks\n";
    return 0;
  } catch (std::invalid_argument &e) {
    std::cout << e.what() << std::endl;
    return -1;
  }
}
