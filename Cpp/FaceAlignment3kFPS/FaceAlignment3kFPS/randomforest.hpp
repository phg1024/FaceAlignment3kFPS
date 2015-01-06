#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H

#include "common.h"

template<typename DecisionTreeType>
class RandomForest
{
public:
  typedef DecisionTreeType tree_t;
  typedef typename DecisionTreeType::data_t data_t;
  typedef typename DecisionTreeType::sample_t sample_t;
  typedef typename DecisionTreeType::item_t item_t;
  
  RandomForest(){}
  RandomForest(int n);
  
  void train(const data_t& data);
  vector<int> classify(const data_t &data);
  int classify_item(const item_t &item, int nclasses);
  
protected:
  data_t sampleWithReplacement(const data_t &data);
  
private:
  vector<tree_t> trees;
};

template<typename DecisionTreeType>
RandomForest<DecisionTreeType>::RandomForest(int n) {
  trees.resize(n);
}

template<typename DecisionTreeType>
typename RandomForest<DecisionTreeType>::data_t RandomForest<DecisionTreeType>::sampleWithReplacement(const typename RandomForest::data_t &data) {
  size_t n = data.sampleCount();
  // Seed with a real random value, if available
  std::random_device rd;
  
  // Choose a random mean between 1 and 6
  std::default_random_engine e1(rd());
  std::uniform_int_distribution<idx_t> uniform_dist(0, n-1);
  
  vector<sample_t> samples;
  for (int i=0; i<(int)n; ++i) {
    idx_t idx = uniform_dist(e1);
    samples.push_back(data.sample(idx));
  }
  
  return data_t(samples, data.classCount());
}

template<typename DecisionTreeType>
void RandomForest<DecisionTreeType>::train(const typename RandomForest::data_t &data) {
  int tidx = 0;
  for(auto &t : trees) {
    /// generate a sampling of the data
    data_t sampled_data = sampleWithReplacement(data);
    cout << "traing tree #" << tidx++ << endl;
    t.train(sampled_data);
  }
}

template<typename DecisionTreeType>
vector<int> RandomForest<DecisionTreeType>::classify(const typename RandomForest::data_t &data) {
  vector<int> clabs(data.sampleCount());
  for(int i=0;i<data.sampleCount();++i) {
    auto &sample = data.sample(i);
    clabs[i] = classify_item(sample.data, data.classCount());
  }
  return clabs;
}

template<typename DecisionTreeType>
int RandomForest<DecisionTreeType>::classify_item(const typename RandomForest::item_t &item, int nclasses) {
  vector<int> counter(nclasses, 0);
  for(auto &t : trees) {
    int clab = t.classify_item(item);
    ++counter[clab];
  }
  
  for(auto x : counter) cout << x << '\t';
  cout << endl;
  
  return (int)distance(counter.begin(), max_element(counter.begin(), counter.end()));
}

#endif // RANDOMFOREST_H
