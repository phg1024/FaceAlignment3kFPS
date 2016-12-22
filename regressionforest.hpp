#pragma once

template <typename TreeType>
struct RegressionForest {
  RegressionForest(){}

  void init(int N, int D, int Ndims, double threshold) {
    trees.resize(N);
    for (auto &t : trees) {
      t = TreeType(N, D, threshold);
    }
  }
  void train(const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &deltashape);

  int ntrees;
  vector<TreeType> trees;
};

template <typename TreeType>
void RegressionForest<TreeType>::train(const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &deltashape)
{
  for (int i = 0; i < ntrees; ++i) {
    trees[i].train(pixels, deltashape);
  }
}
