#pragma once

#include "common.h"

#include "numerical.hpp"
#include "regressiontree.hpp"
#include "regressionforest.hpp"
#include "utils.h"
#include "transformations.h"

#include "opencv2/highgui/highgui.hpp"
using namespace cv;

struct ImageData {
  bool loadImage(const string &filename);
  bool loadPoints(const string &filename);
  cv::Mat img;  // resized grayscale image for training
  cv::Mat original;
  arma::vec pts;  // rescaled points, x1 y1 x2 y2 ... xn yn
};

struct TrainingSample {
  vector<int> imgidx; // index vector of the training samples, N
  arma::mat truth;    // N x Lfp matrix
  arma::mat guess;    // N x Lfp matrix
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
  TrainingSample generateTrainingSamples(vector<ImageData> &inputimages);
  void trainModel(vector<ImageData> &imgdata, TrainingSample &samples);

private:
  struct ModelParameters {
    ModelParameters() :Ndims(500), Npixels(400){}

    int window_size;
    int T;  // number of stages
    int N;  // number of trees per stage
    int D;  // depth of decision trees
    int Ndims;
    int Npixels;

    void print() {
      cout << "window size = " << window_size << endl;      
      cout << "T = " << T << endl;
      cout << "N = " << N << endl;
      cout << "D = " << D << endl;
    }
  } params;

  // the model

  // the trees
  typedef RegressionTree<> tree_t;
  typedef RegressionForest<tree_t> forest_t;
  struct PixelOffset {
    double x, y;
  };
  struct LandmarkMappingFunction {
    arma::mat locations; // pixel offsets w.r.t. the landmark in mean shape
    forest_t forest;
  };
  typedef vector<LandmarkMappingFunction> MappingFunction;
  struct Stage {
    MappingFunction phi;  // feature mapping function
    arma::mat W;          // weighting matrix
  };
  vector<MappingFunction> stages;
};

