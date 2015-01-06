#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include "common.h"

template <typename T>
class Data {
public:
  typedef T sample_t;
  Data(){}
  Data(const vector<sample_t> &samples, size_t nclasses):samples(samples), nclasses(nclasses){}
  
  size_t sampleCount() const { return samples.size(); }
  const sample_t& sample(idx_t idx) const{
    assert( idx < samples.size() );
    return samples[idx];
  }
  
  size_t classCount() const { return nclasses; }
  
private:
  size_t nclasses;
  vector<sample_t> samples;
};

template <typename T>
struct TreeNode
{
  typedef T sample_t;
  typedef typename sample_t::item_t item_t;
  typedef Data<sample_t> data_t;
  
  TreeNode(){}
  TreeNode(const set<int> &dimensions, const set<int> &samples):
  dimensions(dimensions), samples(samples){}
  
  void computeLabel(const data_t &data) {
    map<int, int> counter;
    for(auto sidx : samples) {
      auto &sample = data.sample(sidx);
      if( counter.find(sample.clabel) == counter.end() ) {
        counter.insert(make_pair(sample.clabel, 1));
      }
      else
        ++counter[sample.clabel];
    }
    
    pair<int, int> maxClass = make_pair(-1, 0);
    for(auto c : counter) {
      if( c.second > maxClass.second ) {
        maxClass = c;
      }
    }
    clabel = maxClass.first;
  }
  
  /// split the node with the given dimension and pivot value
  void split(int dim, double val, const data_t &data) {
    splittingDimension = dim;
    splittingValue = val;
    //cout << "sdim = " << dim << " sval = " << val << endl;
    
    set<int> left, right;
    for(auto sidx : samples) {
      auto &sample = data.sample(sidx);
      if( sample.data.f[dim] <= val ) {
        left.insert(sidx);
      }
      else {
        right.insert(sidx);
      }
    }
    set<int> new_dimensions = dimensions;
    new_dimensions.erase(dim);
    
    /*
    cout << left.size() << ", " << right.size() << endl;
    for(auto l : left) {
      cout << data.sample(l) << "; ";
    }
    cout << endl;
    for(auto l : right) {
      cout << data.sample(l) << "; ";
    }
    cout << endl;
    */
    
    leftChild = shared_ptr<TreeNode<T>>(new TreeNode<T>(new_dimensions, left));
    rightChild = shared_ptr<TreeNode<T>>(new TreeNode<T>(new_dimensions, right));
  }
  
  /// reference to samples
  bool isLeaf;
  int clabel;
  
  int splittingDimension;
  double splittingValue;
  
  set<int> dimensions;
  set<int> samples;
  
  shared_ptr<TreeNode<T>> leftChild, rightChild;
};

template <typename T>
class InfomationGainSplitter {
public:
  typedef T sample_t;
  typedef Data<sample_t> data_t;
  typedef TreeNode<sample_t> node_t;
  typedef typename sample_t::item_t item_t;
  
  static double computeInformationValue(const set<int> &samples, const data_t &data) {
    unordered_map<int, int> counter;
    for( auto sidx : samples ) {
      auto &sample = data.sample(sidx);
      if( counter.find(sample.clabel) == counter.end() )
      {
        counter.insert(make_pair(sample.clabel, 1));
      }
      else
        ++counter[sample.clabel];
    }
    size_t n = samples.size();
    double infoval = 0.0;
    for( auto c : counter ) {
      double p = c.second / (double)n;
      infoval -= p * log2(p);
    }
    return infoval;
  }
  
  static bool splitTest(const node_t &node, const data_t &data) {
    /// no more dimension available
    //if( node.dimensions.empty() ) return false;
    
    /// simple purity test
    set<int> classes;
    for( auto sidx : node.samples ) {
      classes.insert(data.sample(sidx).clabel);
    }
    if( classes.size() == 1 ) return false;
    
    
    return true;
  }
  static pair<int, double> computeSplittingDimensionAndValue(const node_t &node, const data_t &data) {
    pair<int, double> dim_val = make_pair(-1, 0.0);
    double maxInfoGain = 0.0;
    
    double oinfo = computeInformationValue(node.samples, data);
    
    /// for each potential dimension, find the optimal splitting point and compute info gain
    //for(auto didx : node.dimensions) {
    for(int didx=0;didx<item_t::ndims();++didx){
      /// take all values
      vector<typename item_t::value_t> values(node.samples.size());
      int vidx = 0;
      for(auto sidx : node.samples) {
        auto &sample = data.sample(sidx);
        values[vidx] = sample.data.f[didx];
        ++vidx;
      }
      
      std::sort(values.begin(), values.end());
      
      /// test all possible splitting points
      for(int i=1;i<values.size();++i) {
        typename item_t::value_t candidate = 0.5 * (values[i] + values[i-1]);
        
        /// split all candidates into two groups
        set<int> left, right;
        for(auto sidx : node.samples) {
          auto &sample = data.sample(sidx);
          if( sample.data.f[didx] <= candidate ) {
            left.insert(sidx);
          }
          else {
            right.insert(sidx);
          }
        }
        
        double pleft = left.size() / (double) node.samples.size();
        double pright = right.size() / (double) node.samples.size();
        double linfo = computeInformationValue(left, data);
        double rinfo = computeInformationValue(right, data);
        //cout << candidate << ", " << linfo << ", " << rinfo << endl;
        
        double infogain = oinfo - pleft*linfo - pright*rinfo;
        
        if( infogain > maxInfoGain ) {
          //cout << "infogain = " << infogain << endl;
          
          maxInfoGain = infogain;
          dim_val = make_pair(didx, candidate);
        }
      }
    }
    return dim_val;
  }
};

template <typename T>
class GiniImpuritySplitter {
public:
  typedef T sample_t;
  typedef Data<sample_t> data_t;
  typedef TreeNode<sample_t> node_t;
  typedef typename sample_t::item_t item_t;
  
  static double computeImputiryValue(const set<int> &samples, const data_t &data) {
    unordered_map<int, int> counter;
    for( auto sidx : samples ) {
      auto &sample = data.sample(sidx);
      if( counter.find(sample.clabel) == counter.end() )
      {
        counter.insert(make_pair(sample.clabel, 1));
      }
      else
        ++counter[sample.clabel];
    }
    size_t n = samples.size();
    double impurity = 0.0;
    for( auto c : counter ) {
      double p = c.second / (double)n;
      impurity += p * (1-p);
    }
    return impurity;
  }
  
  static bool splitTest(const node_t &node, const data_t &data) {
    
    /// no more dimension available
    //if( node.dimensions.empty() ) return false;
    
    /// simple purity test
    set<int> classes;
    for( auto sidx : node.samples ) {
      classes.insert(data.sample(sidx).clabel);
    }
    if( classes.size() == 1 ) return false;
    
    return true;
  }
  static pair<int, double> computeSplittingDimensionAndValue(const node_t &node, const data_t &data) {
    pair<int, double> dim_val = make_pair(-1, 0.0);
    double minImpurity = data.classCount();
    
    /// for each potential dimension, find the optimal splitting point and compute info gain
    ///for(auto didx : node.dimensions) {
    for(int didx = 0; didx < item_t::ndims(); ++didx ) {
      /// take all values
      vector<typename item_t::value_t> values(node.samples.size());
      int vidx = 0;
      for(auto sidx : node.samples) {
        auto &sample = data.sample(sidx);
        values[vidx] = sample.data.f[didx];
        ++vidx;
      }
      
      std::sort(values.begin(), values.end());
      
      /// test all possible splitting points
      for(int i=1;i<values.size();++i) {
        typename item_t::value_t candidates = 0.5 * (values[i] + values[i-1]);
        
        /// split all candidates into two groups
        set<int> left, right;
        for(auto sidx : node.samples) {
          auto &sample = data.sample(sidx);
          if( sample.data.f[didx] <= candidates ) {
            left.insert(sidx);
          }
          else {
            right.insert(sidx);
          }
        }
        
        double pleft = left.size() / (double) node.samples.size();
        double pright = right.size() / (double) node.samples.size();
        double linfo = computeImputiryValue(left, data);
        double rinfo = computeImputiryValue(right, data);
        double impurity = pleft*linfo + pright*rinfo;
        
        if( impurity < minImpurity ) {
          //cout << "impurity = " << impurity << endl;
          minImpurity = impurity;
          dim_val = make_pair(didx, candidates);
        }
      }
    }
    return dim_val;
  }
};

template <typename SampleType, class SplittingStrategy = InfomationGainSplitter<SampleType>,
class NodeType = TreeNode<SampleType>>
class DecisionTree
{
public:
  typedef SampleType sample_t;
  typedef typename sample_t::item_t item_t;
  typedef Data<SampleType> data_t;
  typedef NodeType node_t;
  typedef SplittingStrategy strategy_t;
  
  DecisionTree(){}
  
  void train(const data_t &data);
  vector<int> classify(const data_t &data);
  int classify_item(const item_t &item);
  
private:
  shared_ptr<node_t> root;
};

template <typename SampleType, class NodeType, class SplittingStrategy>
void DecisionTree<SampleType, NodeType, SplittingStrategy>::train(const typename DecisionTree::data_t &data) {
  set<int> dimensions;
  set<int> samples;
  size_t n = data.sampleCount();
  int ndims = data_t::sample_t::item_t::ndims();
  for(int i=0;i<n;++i) { samples.insert(i); }
  for(int i=0;i<ndims;++i) { dimensions.insert(i); }
  
  root = shared_ptr<node_t>(new node_t(dimensions, samples));
  
  queue<shared_ptr<node_t>> Q;
  Q.push(root);
  
  /// build a decision tree by recursively splitting tree node until convergence is reached
  
  /// build the tree in BFS manner
  while( !Q.empty() ) {
    shared_ptr<node_t> cur = Q.front();
    Q.pop();
    
    //cout << cur->samples.size() << endl;
    
    /// do we need to further split this node?
    bool needSplit = strategy_t::splitTest(*cur, data);
    
    if( needSplit ) {
      /// choose a dimension for splitting
      pair<int, double> dim_val = strategy_t::computeSplittingDimensionAndValue(*cur, data);
      int sdim = dim_val.first;
      double sval = dim_val.second;
      
      /// split the node and repeat the process at those nodes
      cur->split(sdim, sval, data);
      Q.push(cur->leftChild);
      Q.push(cur->rightChild);
    }
    else {
      /// otherwise, the node is good enough, label this node
      cur->computeLabel(data);
      cur->isLeaf = true;
    }
  }
}

template <typename SampleType, class NodeType, class SplittingStrategy>
int DecisionTree<SampleType, NodeType, SplittingStrategy>::classify_item(const typename DecisionTree::item_t &item) {
  shared_ptr<node_t> node = root;
  while( true ) {
    if( node->isLeaf ) return node->clabel;
    
    int dim = node->splittingDimension;
    if( item.f[dim] < node->splittingValue ) {
      node = node->leftChild;
    }
    else {
      node = node->rightChild;
    }
  }
}

template <typename SampleType, class NodeType, class SplittingStrategy>
vector<int> DecisionTree<SampleType, NodeType, SplittingStrategy>::classify(const typename DecisionTree::data_t &data) {
  vector<int> clabs;
  for(int i=0;i<data.sampleCount();++i) {
    auto &sample = data.sample(i);
    auto &item = sample.data;
    
    clabs.push_back(classify_item(item));
  }
  return clabs;
}
#endif // DECISIONTREE_H
