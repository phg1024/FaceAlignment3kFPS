#pragma once

template <typename TreeType>
struct RegressionForest {
  typedef typename TreeType::input_t input_t;
  RegressionForest(){}
  RegressionForest(int N, double randomness) :ntrees(N), randomness(randomness){
    trees.resize(N);
  }

  void init(int N, int D, double threshold) {
    for (auto &t : trees) {
      t = TreeType(N, D, threshold);
    }
  }
  void train(const vector<input_t> &samples);

  int ntrees;
  double randomness;
  vector<TreeType> trees;
};

template <typename TreeType>
void RegressionForest<TreeType>::train(const vector<input_t> &samples)
{
  for (int i = 0; i < ntrees; ++i) {
    trees[i].train(samples, randomness);
  }
}
