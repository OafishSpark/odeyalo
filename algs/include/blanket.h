#ifndef BLANKET_H
#define BLANKET_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include <cstdlib>
#include <ctime>
#include <cmath>

struct Color {
  int id;  // number to identify color
  int num; // number of colors which are potential to use
};

bool CompareColors(Color fst, Color scnd);

class Blanket {
  const int num_of_rows_ = 0;
  const int num_of_cols_ = 0;
  std::vector<std::vector<int>> color_matrix_;

  std::vector<int> colors_;
  int sum_{0};
  std::vector<Color> gr_color_set_, le_color_set_;

  bool if_colored = false;

  float Evenity(const Color&);
  float EvenCriteria(bool mean=false);

  float SequenceCriteria();
  
  // Simplified KNN on Mask
  float MasksCriteria();

  float Energy();

  bool CheckCorrectnessElem(int iv, int jv);
  void NewState();

public:
  explicit Blanket(int, int);

  std::pair<int, int> CheckIfCorrectBlanket() const;

  void Coloring(std::vector<int> &);

  void RandomizeColoring();

  void Print();

  void PrintStat();
};

#endif // BLANKET_H

/* TODO:
    OK 1) First approximation
    
    OK 2) Check correctness
    
    OK 3) Random criterias
    OK 3.1) Evenity
    OK 3.2) Masks
    OK 3.3) Sequences
    
    4) Simulated annealing randomizing
    4.1) Function of changing state
    4.1.1) Random element changing (high prob)
    4.1.2) Random even noncrossing square changing (lowest prob)
    4.1.3) Random element enter (highest prob)
    4.2) Energy Function
    4.3) Temperature function
*/
