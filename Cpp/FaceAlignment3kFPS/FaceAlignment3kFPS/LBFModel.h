#pragma once

#include "common.h"

#include "numerical.hpp"

#include "opencv2/highgui/highgui.hpp"
using namespace cv;

struct ImageData {
  bool loadImage(const string &filename);
  bool loadPoints(const string &filename);
  cv::Mat img;  // resized grayscale image for training
  cv::Mat original;
  arma::vec pts;  // rescaled points
};

struct TrainingSample {
  vector<int> imgidx; // index vector of the training samples
  arma::mat truth;
  arma::mat guess;
};

class LBFModel
{
public:
  LBFModel(){}
  LBFModel(const string &modelfile) { load(modelfile); }
  ~LBFModel(){}

  bool train(const string &settingsfile);  
  bool test(const string &imagefile);
  bool batch_test(const string &settingsfile);

  bool load(const string &modelfile);
  bool save(const string &modelfile);

private:
  map<string, string> readSettingFile(const string &filename);
  vector<ImageData> loadInputImages(const map<string, string> &configs);

private:
  struct ModelParameters {
    int window_size;
    int T;  // number of stages
    int N;  // number of trees per stage
    int D;  // depth of decision trees

    void print() {
      cout << "window size = " << window_size << endl;      
      cout << "T = " << T << endl;
      cout << "N = " << N << endl;
      cout << "D = " << D << endl;
    }
  } params;
};

