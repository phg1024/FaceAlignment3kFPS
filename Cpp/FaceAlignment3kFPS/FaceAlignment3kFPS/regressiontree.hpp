#pragma once

#include "common.h"
#include "numerical.hpp"

struct RegressionTreeTrainingSample {
  arma::vec feature;
  arma::vec2 output;
};

struct RegressionTreeNode {
  vector<int> samples;
  int splitDim;
  double splitValue;
  arma::vec2 output;

  bool isLeaf() const {
    return lchild == nullptr && rchild == nullptr;
  }
  shared_ptr<RegressionTreeNode> lchild, rchild;
};

template <typename InputType=arma::vec, typename OutputType=arma::vec2, typename NodeType=RegressionTreeNode>
class RegressionTree {
public:
  typedef InputType input_t;
  typedef OutputType output_t;
  typedef NodeType node_t;

  RegressionTree(){}
  RegressionTree(int N, int D, double threshold) :ndims(N), maxDepth(D){}


  void train(const vector<RegressionTreeTrainingSample> &samples, double randomness = 0.0);
  OutputType predict(const InputType &sample);
  vector<bool> localBinaryFeature(const InputType &sample);

protected:
  shared_ptr<NodeType> trainSubTree(const vector<int> &samples, const vector<RegressionTreeTrainingSample> &data, double randomness = 0.0);
  pair<double, double> findBestSplittingPoint(const vector<int> &samples, int d, const vector<RegressionTreeTrainingSample> &data);
  bool stopSplitting(const vector<int> &samples, const vector<RegressionTreeTrainingSample> &data, arma::vec2 &meanval);

private:
  int ndims;
  int maxDepth;
  double threshold; // threshold for stop splitting
  shared_ptr<NodeType> root;
};

template <typename InputType/*=arma::vec*/, typename OutputType/*=arma::vec2*/, typename NodeType/*=RegressionTreeNode*/>
bool RegressionTree<InputType, OutputType, NodeType>::stopSplitting(const vector<int> &samples, const vector<RegressionTreeTrainingSample> &data, arma::vec2 &meanval)
{
  assert(samples.size() >= 1);
  if (samples.size() == 1) return true;
  else {
    // compute the mean value of all samples
    for (int i = 0; i < samples.size(); ++i) {
      meanval += data[samples[i]].output;
    }
    meanval /= samples.size();

    double errval = 0;
    for (int i = 0; i < data.size(); ++i) {
      arma::vec2 diff = data[samples[i]].output - meanval;
      errval += diff(1)*diff(1) + diff(2)*diff(2);
    }
    return errval / samples.size();
  }
}

template <typename InputType/*=arma::vec*/, typename OutputType/*=arma::vec2*/, typename NodeType/*=RegressionTreeNode*/>
pair<double, double> RegressionTree<InputType, OutputType, NodeType>::findBestSplittingPoint(const vector<int> &samples, int d, const vector<RegressionTreeTrainingSample> &data)
{
  // get all data
  int nsamples = samples.size();

  struct IndexValuePair {
    IndexValuePair(int idx, double val) :idx(idx), val(val){}
    int idx;
    double val;
  };
  vector<IndexValuePair> values(nsamples);
  for (int i = 0; i < nsamples; ++i) {
    values[i] = IndexValuePair(i, data[samples[i]].feature(d));
  }

  // sort the values
  std::sort(values.begin(), values.end(), [](const IndexValuePair &a, const IndexValuePair &b){
    return a.val < b.val;
  });

  // find the best splitting point
  OutputType leftSum, rightSum;
  leftSum = data[samples[0]].output;
  for (int i = 1; i < nsamples; ++i) {
    rightSum += data[samples[i]].output;
  }

  auto computeError = [&](int startIdx, int endIdx, const arma::vec2 &meanval) {
    double errval = 0;
    for (int i = startIdx; i <= endIdx; ++i) {
      arma::vec2 diff = data[samples[values[startIdx].idx]].output - meanval;
      errval += diff(1)*diff(1) + diff(2)*diff(2);
    }
    return errval;
  }

  double best_error = FLT_MAX;
  double split_point = 0;
  for (int i = 0; i < nsamples-1; ++i) {
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

template <typename InputType/*=arma::vec*/, typename OutputType/*=arma::vec2*/, typename NodeType/*=RegressionTreeNode*/>
shared_ptr<NodeType> RegressionTree<InputType, OutputType, NodeType>::trainSubTree(const vector<int> &samples, const vector<RegressionTreeTrainingSample> &data, double randomness /*= 0.0*/)
{
  // test if further splitting is necessary
  arma::vec2 meanval;
  if (stopSplitting(samples, data, meanval)) {
    // no splitting needed, just create a node here
    shared_ptr<NodeType> node = make_shared(new NodeType);
    node->samples = samples;
    node->output = meanval;
    return node;
  }
  else {
    // get a subset of available dimensions
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, ndims - 1);

    int avail_dims = int(ndims * (1.0 - randomness));
    set<int> dims;
    while (dims.size() < avail_dims) {
      int d = distribution(generator);
      dims.insert(d);
    }

    // find the best splitting dimension and split value
    double best_error = FLT_MAX;
    double best_split = 0;
    int best_dim = -1;
    for (auto d : dims) {
      auto res = findBestSplittingPoint(samples, d, data);
      if (res.second < best_error) {
        best_error = res.second;
        best_split = res.first;
        best_dim = d;
      }
    }

    // split the samples into two sub sets, and build the tree recursively
    vector<int> lset, rset;
    lset.reserve(samples.size());
    rset.reserve(samples.size());
    for (int i = 0; i < samples.size(); ++i) {
      if (data[samples[i]].feature(best_dim) < best_split) lest.push_back(samples[i]);
      else rest.push_back(samples[i]);
    }

    shared_ptr<NodeType> node = make_shared(new NodeType);
    node->samples = samples;
    node->splitDim = best_dim;
    node->splitVal = best_split;
    node->lchild = trainSubTree(lset, data, randomness);
    node->rchild = trainSubTree(rset, data, randomness);
    return node;
  }
}

template <typename InputType/*=arma::vec*/, typename OutputType/*=arma::vec2*/, typename NodeType/*=RegressionTreeNode*/>
void RegressionTree<InputType, OutputType, NodeType>::train(const vector<RegressionTreeTrainingSample> &samples, double randomness)
{
  int n = sample.size();
  vector<int> indices(n);
  for (int i = 0; i < n; ++i) indices[i] = i;
  root = trainSubTree(indices, samples, randomness);
}

template <typename InputType/*=arma::vec*/, typename OutputType/*=arma::vec2*/, typename NodeType/*=RegressionTreeNode*/>
OutputType RegressionTree<InputType, OutputType, NodeType>::predict(const InputType &sample)
{

}
