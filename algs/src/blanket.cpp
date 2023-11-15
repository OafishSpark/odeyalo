#include "blanket.h"

Blanket::Blanket(int num_of_rows, int num_of_cols)
    : num_of_cols_(num_of_cols), num_of_rows_(num_of_rows) {
  assert(num_of_rows > 0);
  assert(num_of_cols > 0);
  // if (num_of_cols_ < 20 || num_of_rows_ < 20) {
  //   num_iters /= 50;
  //   to_mask = true;
  // }
  color_matrix_ = std::vector<std::vector<int>>(
      num_of_rows, std::vector<int>(num_of_cols, 0));
}

bool CompareColors(Color fst, Color scnd) {
  return fst.num > scnd.num ? true : false;
}

void Blanket::Coloring(std::vector<int> &colors_orig) {
  int k = colors_orig.size();
  
  colors_ = colors_orig;
  for (auto &color_n : colors_) {
    sum_ += color_n;
  }

  std::vector<Color> colors(k);
  for (int iv{0}; iv < k; ++iv) {
    colors[iv] = {iv, colors_orig[iv]};
  }

  std::sort(colors.begin(), colors.end(), CompareColors);

  std::vector<Color> color_set_1(k), color_set_2(k);
  int sum1{0}, sum2{0}; // sums of potential colors for in color sets
  int k1{0}, k2{0};     // numbers of types of colors in sets

  for (int iv{0}; iv < k; ++iv) {
    if (sum1 < sum2) {
      color_set_1[k1] = colors[iv];
      ++k1;
      sum1 += colors[iv].num;
    } else {
      color_set_2[k2] = colors[iv];
      ++k2;
      sum2 += colors[iv].num;
    }
  }

  color_set_1.resize(k1);
  color_set_2.resize(k2);

  std::vector<Color> &gr_color_set = sum1 > sum2 ? color_set_1 : color_set_2;
  std::vector<Color> &le_color_set = sum1 <= sum2 ? color_set_1 : color_set_2;
  int &le_sum = sum1 <= sum2 ? sum1 : sum2;

  int le_size = num_of_cols_ * num_of_rows_ / 2;
  if (le_sum < le_size) {
    int diff = le_size - le_sum;
    assert(diff > 0);
    assert(diff < gr_color_set.back().num);

    // if assert below goes wrong then it is impossible to color the blanket
    // with these colors
    if (gr_color_set.back().num > num_of_rows_ * num_of_cols_ / 2 + 1) {
      throw std::invalid_argument(
          "It is impossible to color blanket with such a colors");
    }
    le_color_set.push_back({gr_color_set.back().id, diff});
    gr_color_set.back().num -= diff;
  }

  auto gr_color_it = gr_color_set.begin();
  auto le_color_it = le_color_set.end();
  --le_color_it;
  for (int iv{0}; iv < num_of_rows_; ++iv) {
    for (int jv{0}; jv < num_of_cols_; ++jv) {
      if ((iv + jv) % 2 == 0) {
        while (gr_color_it->num == 0) {
          ++gr_color_it;
        }
        color_matrix_[iv][jv] = gr_color_it->id;
        --gr_color_it->num;
      } else {
        while (le_color_it->num == 0) {
          --le_color_it;
        }
        color_matrix_[iv][jv] = le_color_it->id;
        --le_color_it->num;
      }
    }
  }
  gr_color_set_ = gr_color_set;
  le_color_set_ = le_color_set;
  if_colored = true;
}

void Blanket::Print() {
  for (const auto &row : color_matrix_) {
    for (const auto &elem : row) {
      std::cout << elem << " ";
    }
    std::cout << "\n";
  }
}

std::pair<int, int> Blanket::CheckIfCorrectBlanket() const {
  std::pair<int, int> answer({-1, -1});
  for (int iv{0}; iv < num_of_rows_ - 1; ++iv) {
    for (int jv{0}; jv < num_of_cols_ - 1; ++jv) {
      if (color_matrix_[iv][jv] == color_matrix_[iv + 1][jv] or
          color_matrix_[iv][jv] == color_matrix_[iv][jv + 1]) {
        return {iv, jv};
      }
    }
  }
  return answer;
}
