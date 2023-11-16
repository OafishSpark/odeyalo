#include "blanket.h"

float Blanket::Uniformity(const Color &color) {
  float temp = colors_[color.id] - color.num;
  assert(temp >= 0);
  temp = temp / num_of_cols_ / num_of_rows_;
  temp -= float(colors_[color.id]) / sum_;
  temp = std::abs(temp);
  return temp;
}

float Blanket::UniformCriteria(bool mean) {
  float answer{0};
  if (mean) {
    for (const auto &color : gr_color_set_) {
      answer += Uniformity(color);
    }
    for (const auto &color : le_color_set_) {
      answer += Uniformity(color);
    }
    if (answer <= 10e-5) {
      return 0.0;
    }
    return answer / colors_.size();
  } else { // max uniformity
    for (const auto &color : gr_color_set_) {
      float temp = Uniformity(color);
      if (temp > answer && std::abs(temp) > 10e-5) {
        answer = temp;
      }
    }
    for (const auto &color : le_color_set_) {
      float temp = Uniformity(color);
      if (temp > answer && std::abs(temp) > 10e-5) {
        answer = temp;
      }
    }
    return answer;
  }
}

float Blanket::AllColorsToUseCriteria() {
  float answer{0};
  for (const auto &color : gr_color_set_) {
    answer += int(colors_[color.id] - color.num == 0);
  }
  for (const auto &color : le_color_set_) {
    answer += int(colors_[color.id] - color.num == 0);
  }
  return answer;
}

const std::vector<int> Steps{2, 4, 6, 8};

float Sequence(std::vector<int> seq) {
  float answer = 0.0;
  for (const auto &step : Steps) {
    int subseq_len{0};
    if (step >= seq.size()) {
      return answer;
    }
    for (int iv{0}; iv < seq.size() - step - (seq.size() % step); iv += step) {
      int eq_count{0};
      for (int jv{iv}; jv < iv + step; ++jv) {
        if (seq[jv] != seq[jv + step]) {
          answer += std::pow(float(subseq_len), 8 / float(step));
          subseq_len = 0;
          break;
        }
        ++eq_count;
      }
      if (eq_count == step) {
        ++subseq_len;
      }
    }
    answer += std::pow(float(subseq_len), 8 / float(step));
  }
  return answer / pow(float(seq.size()), 3);
}

float Blanket::SequenceCriteria() {
  float answer = 0.0;
  for (int iv{0}; iv < num_of_rows_; ++iv) {
    answer += Sequence(color_matrix_[iv]);
  }
  for (int iv{0}; iv < num_of_cols_; ++iv) {
    std::vector<int> seq(num_of_rows_);
    for (int jv{0}; jv < num_of_rows_; ++jv) {
      seq[jv] = color_matrix_[jv][iv];
    }
    answer += Sequence(seq);
  }
  for (int iv{0}; iv < num_of_cols_; ++iv) {
    std::vector<int> seq(num_of_rows_);
    for (int jv{0}; jv < num_of_rows_; ++jv) {
      seq[jv] = color_matrix_[jv][(iv + jv) % num_of_cols_];
    }
    answer += Sequence(seq);
  }
    for (int iv{0}; iv < num_of_cols_; ++iv) {
    std::vector<int> seq(num_of_rows_);
    for (int jv{0}; jv < num_of_rows_; ++jv) {
      seq[jv] = color_matrix_[jv][(iv - jv) % num_of_cols_];
    }
    answer += Sequence(seq);
  }
  return answer;
}

std::vector<std::pair<int, int>> Masks{{2, 1}, {3, 2}, {4, 2}, {5, 3}};

float Blanket::MasksCriteria() {
  float answer{0};

  int t = num_of_cols_ > num_of_rows_ ? num_of_rows_ : num_of_cols_;

  // if (t > 10) {
  //   int c = colors_.size();
  //   Masks.push_back({t / 2, t / c + 1});
  //   Masks.push_back({t / 4, t / c / 2 + 1});
  // }

  for (const auto &mask : Masks) {
    auto [dim, max_similar] = mask;

    if (dim >= num_of_cols_ || dim >= num_of_rows_) {
      return answer;
    }

    std::vector<int> colors(colors_.size(), 0);

    for (int iv{0}; iv < num_of_rows_ - dim; ++iv) {
      for (int jv{0}; jv < num_of_cols_ - dim; ++jv) {
        int similar{max_similar};
        for (int ind = iv; ind < iv + dim; ++ind) {
          for (int jnd = jv; jnd < jv + dim; ++jnd) {
            ++colors[color_matrix_[ind][jnd]];
          }
        }
        for (int &color : colors) {
          if (color > similar) {
            similar = color;
          }
          color = 0;
        }
        answer += float(similar - max_similar) / dim / dim;
      }
    }
    answer /= (num_of_cols_ - dim) * (num_of_rows_ - dim);
  }
  return answer;
}

float Blanket::Energy() {
  float answer = 0.0;
  answer += 1000 * UniformCriteria(true);
  answer += 1000 * AllColorsToUseCriteria();
  answer += SequenceCriteria();
  if (to_mask) {
    answer += MasksCriteria();
  }
  return answer;
}

bool Blanket::CheckCorrectnessElem(int iv, int jv) {
  int elem = color_matrix_[iv][jv];
  if (iv > 0) {
    if (color_matrix_[iv - 1][jv] == elem) {
      return false;
    }
  }
  if (iv < num_of_rows_ - 1) {
    if (color_matrix_[iv + 1][jv] == elem) {
      return false;
    }
  }
  if (jv > 0) {
    if (color_matrix_[iv][jv - 1] == elem) {
      return false;
    }
  }
  if (jv < num_of_cols_ - 1) {
    if (color_matrix_[iv][jv + 1] == elem) {
      return false;
    }
  }
  return true;
}

void Blanket::EnterRandomElement() {
  int x = rand() % num_of_rows_;
  int y = rand() % num_of_cols_;
  std::vector<Color> *colors;
  if ((x + y) % 2 == 0) {
    colors = &gr_color_set_;
  } else {
    colors = &le_color_set_;
  }
  int ind = rand() % colors->size();
  int temp = color_matrix_[x][y];
  for (int iv{ind}; iv < colors->size(); ++iv) {
    if ((*colors)[ind].num != 0) {
      color_matrix_[x][y] = (*colors)[ind].id;
      if (CheckCorrectnessElem(x, y)) {
        (*colors)[ind].num -= 1;
        for (auto &col : (*colors)) {
          if (col.id == temp) {
            col.num += 1;
            return;
          }
        }
      } else {
        color_matrix_[x][y] = temp;
      }
    }
  }
}

void Swap(int &a, int &b) {
  int t = a;
  a = b;
  b = t;
}

void Blanket::RandomSwap() {
  if (num_of_cols_ == 1 || num_of_rows_ == 1) {
    return;
  }
  int x = rand() % num_of_rows_;
  int y = rand() % num_of_cols_;
  int x1 = rand() % num_of_rows_;
  int y1 = rand() % num_of_cols_;
  if (x == x1 && y == y1) {
    if (x1 == 0) {
      x1 += 1;
    } else {
      x1 -= 1;
    }
    if (y1 == 0) {
      y1 += 1;
    } else {
      y1 -= 1;
    }
  } else if ((x + y) % 2 != (x1 + y1) % 2) {
    if (x1 == 0) {
      x1 += 1;
    } else {
      x1 -= 1;
    }
  }
  assert((x + y) % 2 == (x1 + y1) % 2);
  if (color_matrix_[x][y] == color_matrix_[x1][y1]) {
    return;
  }
  Swap(color_matrix_[x1][y1], color_matrix_[x][y]);
  if (CheckCorrectnessElem(x, y) && CheckCorrectnessElem(x1, y1)) {
    return;
  }
  Swap(color_matrix_[x1][y1], color_matrix_[x][y]);
}

void Blanket::RandomSquareSwap() {
  int t = num_of_cols_ < num_of_rows_ ? num_of_cols_ : num_of_rows_;
  if (t < 5) {
    return;
  }
  int k = rand() % (t - 4) + 4;
  int h = t / std::pow(2, std::floor(std::log2(k)));
  if (h == 0) {
    return;
  }
  int x = rand() % (num_of_rows_ - h);
  int y = rand() % (num_of_cols_ - h);
  int x1 = rand() % (num_of_rows_ - h);
  int y1 = rand() % (num_of_cols_ - h);
  if (std::abs(x1 - x) <= h + 1) {
    if (std::abs(y1 - y) <= h) {
      y1 = y - h - 1;
      if (y1 < 0) {
        y1 = y + h + 1;
      }
      assert(y1 >= 0);
      assert(y1 <= num_of_cols_ - h);
      assert(abs(y1 - y) > h);
    }
  }
  if ((x + y) % 2 != (x1 + y1) % 2) {
    if (x1 == 0) {
      x1 += 1;
    } else {
      x1 -= 1;
    }
  }
  assert((x + y) % 2 == (x1 + y1) % 2);
  for (int iv{0}; iv < h; ++iv) {
    for (int jv{0}; jv < h; ++jv) {
      Swap(color_matrix_[x + iv][y + jv], color_matrix_[x1 + iv][y1 + jv]);
      if (CheckCorrectnessElem(x + iv, y + jv) && CheckCorrectnessElem(x1 + iv, y1 + jv)) {
        continue;
      }
      Swap(color_matrix_[x + iv][y + jv], color_matrix_[x1 + iv][y1 + jv]);
      return;
    }
  }
}


void Blanket::NewState() {
  std::srand(std::time(nullptr));
  int num_rand_elem =
      std::floor(coef_rand_elem_change * (num_of_cols_ + num_of_rows_));
  for (int iv{0}; iv < num_rand_elem; ++iv) {
    EnterRandomElement();
    RandomSwap();
  }
  RandomSquareSwap();
}

void Blanket::RandomizeColoring() {
  std::srand(std::time(nullptr));
  float energy = Energy();
  for (int iv{0}; iv < num_iters; ++iv) {
    std::vector<std::vector<int>> old_state = color_matrix_;
    auto old_gr_colors = gr_color_set_;
    auto old_le_colors = le_color_set_;
    NewState();
    float energy_new = Energy();
    if (energy_new < energy) {
      energy = energy_new;
      continue;
    } else {
      float energy_delta = energy_new - energy;
      float rand_val = float(rand() % 100) / 100;
      float temperature = 0.01 * float(num_iters - iv) / num_iters;
      float max_prob = std::exp(-energy_delta / temperature);
      if (rand_val < max_prob) {
        energy = energy_new;
        continue;
      } else {
        color_matrix_ = old_state;
        gr_color_set_ = old_gr_colors;
        le_color_set_ = old_le_colors;
      }
    }
  }
}

void Blanket::PrintStat() {
  std::cout << "U " << UniformCriteria() << std::endl;
  std::cout << "S " << SequenceCriteria() << std::endl;
  std::cout << "M " << MasksCriteria() << std::endl;
}
