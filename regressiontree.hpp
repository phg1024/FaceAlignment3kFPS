#pragma once

#include "common.h"
#include "numerical.hpp"

struct RegressionTreeNode {
  vector<int> samples;
  int m, n;
  double splitVal;
  Eigen::Vector2d output;

  bool isLeaf() const {
    return lchild == nullptr && rchild == nullptr;
  }
  shared_ptr<RegressionTreeNode> lchild, rchild;
};

template <typename InputType=Eigen::VectorXd, typename OutputType=Eigen::Vector2d, typename NodeType=RegressionTreeNode>
class RegressionTree {
public:
  typedef InputType input_t;
  typedef OutputType output_t;
  typedef NodeType node_t;

  RegressionTree(){}
  RegressionTree(int N, int D, double threshold) :ndims(N), maxDepth(D){}


  void train(const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &ds);
  OutputType predict(const InputType &sample);
  vector<bool> localBinaryFeature(const InputType &sample);

protected:
  shared_ptr<NodeType> trainSubTree(const vector<int> &samples, const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &ds);
  pair<double, double> findBestSplittingPoint(const vector<int> &samples, int m, int n, const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &ds);
  bool stopSplitting(const vector<int> &samples, const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &ds, Eigen::Vector2d &meanval);

private:
  int ndims;
  int maxDepth;
  double threshold; // threshold for stop splitting
  shared_ptr<NodeType> root;
};

template <typename InputType, typename OutputType, typename NodeType>
bool RegressionTree<InputType, OutputType, NodeType>::stopSplitting(const vector<int> &samples, const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &ds, Eigen::Vector2d &meanval)
{
  assert(samples.size() >= 1);
  if (samples.size() == 1) return true;
  else {
    // compute the mean value of all samples
    for (int i = 0; i < samples.size(); ++i) {
      meanval += ds.row(samples[i]);
    }
    meanval /= samples.size();

    double errval = 0;
    for (int i = 0; i < samples.size(); ++i) {
      Eigen::Vector2d diff = Eigen::Vector2d(ds.row(samples[i])) - meanval;
      errval += diff.norm();
    }
    return errval / samples.size();
  }
}

template <typename InputType, typename OutputType, typename NodeType>
pair<double, double> RegressionTree<InputType, OutputType, NodeType>::findBestSplittingPoint(const vector<int> &samples, int m, int n, const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &ds)
{
  // get all data
  int nsamples = samples.size();

  struct IndexValuePair {
    IndexValuePair(){}
    IndexValuePair(int idx, double val) :idx(idx), val(val){}
    int idx;
    double val;
  };
  vector<IndexValuePair> values(nsamples);
  for (int i = 0; i < nsamples; ++i) {
    values[i] = IndexValuePair(i, pixels(samples[i], m) - pixels(samples[i], n));
  }

  // sort the values
  std::sort(values.begin(), values.end(), [](const IndexValuePair &a, const IndexValuePair &b){
    return a.val < b.val;
  });

  // find the best splitting point
  OutputType leftSum, rightSum;
  for (int i = 0; i < nsamples; ++i) {
    rightSum += ds.row(samples[i]);
  }

  auto computeError = [&](int startIdx, int endIdx, const Eigen::Vector2d &meanval) {
    double errval = 0;
    for (int i = startIdx; i <= endIdx; ++i) {
      Eigen::Vector2d diff = Eigen::Vector2d(ds.row(samples[values[startIdx].idx])) - meanval;
      errval += diff.norm();
    }
    return errval;
  };

  double best_error = numeric_limits<double>::max();
  double split_point = 0;
  for (int i = 0; i < nsamples-1; ++i) {
    leftSum += ds.row(i);
    rightSum -= ds.row(i);

    // left error
    int leftCount = i + 1;
    int rightCount = nsamples - leftCount;
    double left_error = computeError(0, i, leftSum / leftCount);
    double right_error = computeError(i+1, nsamples-1, rightSum / rightCount);
    double cur_error = left_error + right_error;
    if (cur_error < best_error) {
      best_error = cur_error;
      split_point = (values[i].val + values[i+1].val)*0.5;
    }
  }

  return make_pair(split_point, best_error);
}

template <typename InputType, typename OutputType, typename NodeType>
shared_ptr<NodeType> RegressionTree<InputType, OutputType, NodeType>::trainSubTree(const vector<int> &samples, const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &ds)
{
  // test if further splitting is necessary
  Eigen::Vector2d meanval;
  if (stopSplitting(samples, pixels, ds, meanval)) {
    // no splitting needed, just create a node here
    shared_ptr<NodeType> node(new NodeType);
    node->samples = samples;
    node->output = meanval;
    return node;
  }
  else {
    // get a subset of available dimensions
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, pixels.cols()-1);

    set<pair<int,int>> dims;
    while (dims.size() < ndims) {
      int m = distribution(generator);
      int n = distribution(generator);
      if (m != n) {
        dims.insert(make_pair(m, n));
      }
    }

    // find the best splitting dimension and split value
    double best_error = numeric_limits<double>::max();
    double best_split = 0;
    pair<int, int> pix_pair;
    for (auto d : dims) {
      auto res = findBestSplittingPoint(samples, d.first, d.second, pixels, ds);
      if (res.second < best_error) {
        best_error = res.second;
        best_split = res.first;
        pix_pair = d;
      }
    }

    // split the samples into two sub sets, and build the tree recursively
    vector<int> lset, rset;
    lset.reserve(samples.size());
    rset.reserve(samples.size());
    for (int i = 0; i < samples.size(); ++i) {
      if (pixels(samples[i], pix_pair.first) - pixels(samples[i], pix_pair.second)  < best_split) lset.push_back(samples[i]);
      else rset.push_back(samples[i]);
    }

    shared_ptr<NodeType> node(new NodeType);
    node->samples = samples;
    node->m = pix_pair.first; node->n = pix_pair.second;
    node->splitVal = best_split;
    node->lchild = trainSubTree(lset, pixels, ds);
    node->rchild = trainSubTree(rset, pixels, ds);
    return node;
  }
}

template <typename InputType, typename OutputType, typename NodeType>
void RegressionTree<InputType, OutputType, NodeType>::train(const Eigen::MatrixXd &pixels, const Eigen::MatrixXd &ds)
{
  int n = pixels.rows();
  vector<int> indices(n);
  for (int i = 0; i < n; ++i) indices[i] = i;
  root = trainSubTree(indices, pixels, ds);
}

template <typename InputType, typename OutputType, typename NodeType>
OutputType RegressionTree<InputType, OutputType, NodeType>::predict(const InputType &sample)
{

}
