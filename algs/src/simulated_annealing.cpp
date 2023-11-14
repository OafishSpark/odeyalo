#include "blanket.h"


float Blanket::Evenity(const Color &color) {
  float temp = colors_[color.id] - color.num;
  temp = temp / num_of_cols_ / num_of_rows_;
  temp -= static_cast<float>(colors_[color.id]) / sum_;
  temp = std::abs(temp);
  return temp;
}

float Blanket::EvenCriteria(bool mean) {
  float answer{0};
  if (mean) {
    for (const auto &color : gr_color_set_) {
      answer += Evenity(color);
    }
    for (const auto &color : le_color_set_) {
      answer += Evenity(color);
    }
    if (answer <= 10e-5) {
      return 0.0;
    }
    return answer / colors_.size();
  } else { // max evenity
    for (const auto &color : gr_color_set_) {
      float temp = Evenity(color);
      if (temp > answer && std::abs(temp) > 10e-5) {
        answer = temp;
      }
    }
    for (const auto &color : le_color_set_) {
      float temp = Evenity(color);
      if (temp > answer && std::abs(temp) > 10e-5) {
        answer = temp;
      }
    }
    return answer;
  }
}


const std::vector<int> Steps {2, 4, 6, 8};

float Sequence(std::vector<int> seq) {
  float answer = 0.0;
  for (const auto& step: Steps) {
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
  return answer / pow(float(seq.size()), 4);
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
  return answer;
}


std::vector<std::pair<int, int>> Masks{
    {2, 1},
    {3, 2},
    {4, 2},
    {5, 3}
};

float Blanket::MasksCriteria() { 
    float answer{0};

    int t = num_of_cols_ > num_of_rows_ ? num_of_rows_ : num_of_cols_;
    
    if (t > 10) {
      int c = colors_.size(); 
      Masks.push_back({t / 2, t / c + 1});
      Masks.push_back({t / 4, t / c / 2 + 1});
    }

    for (const auto& mask: Masks) {
        auto[dim, max_similar] = mask;
    
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
                for (int& color: colors) {
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
  answer += EvenCriteria();
  answer += SequenceCriteria(); 
  answer += MasksCriteria();
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

const float coef_rand_elem_change = 1.5;

void Blanket::NewState() {
  int num_rand_elem = std::floor(coef_rand_elem_change * (num_of_cols_ + num_of_rows_));
  for (int iv{0}; iv < num_rand_elem; ++iv) {
    // change rand elem to free color
    int rand_x = (std::rand() % num_of_rows_);
    int rand_y = (std::rand() % num_of_rows_);
    
    std::vector<Color>* color_set;
    if ((rand_x + rand_y) % 2 == 0) {
      color_set =  &gr_color_set_;  
    } else {
      color_set = &le_color_set_;
    }
    
    int& elem = color_matrix_[rand_x][rand_y];
    bool ch = false;
    for (auto& color: *color_set) {
      if (color.num > 0) {
        if (color.id != elem) {
          int temp = elem;
          elem = color.id;
          if (CheckCorrectnessElem(rand_x, rand_y)) {
            --color.num;
            ch = true;
            break;
          }
          elem = temp;
        }
      }
    }
    if (ch) {
      for (auto& color: *color_set) {
        if (elem == color.id) {
          ++color.num;
        }
      }
    }

    // swap random elem from one group
    rand_x =  (std::rand() % num_of_rows_);
    rand_y =  (std::rand() % num_of_cols_);
  }
}

/*
  std::vector<std::vector<int>> old_state = color_matrix_;
  NewState();
  float energy_new = Energy()
*/

const int num_iters = 500;

void Blanket::RandomizeColoring() {
  std::srand(std::time(nullptr));
  float energy = Energy();
  for (int iv{0}; iv < num_iters; ++iv) {
    std::vector<std::vector<int>> old_state = color_matrix_;
    NewState();
    float energy_new = Energy();
    if (energy_new < energy) {
      energy = energy_new;
      continue;
    } else {
      float energy_delta = energy_new - energy;
      float prob = ((double) rand() / (RAND_MAX));
      if (prob > (energy_delta / energy) * std::log10(8 * (iv / num_iters) + 2)) {
        energy = energy_new;
        continue;
      } else {
        color_matrix_ = old_state;
      }
    }
  }
}


void Blanket::PrintStat() {
  std::cout << "Критерий неравномерности: " << EvenCriteria() << std::endl;
  std::cout << "Критерий длины подпоследовательности: " << SequenceCriteria() << std::endl;
  std::cout << "Масочный критерий: " << MasksCriteria() << std::endl;
}
