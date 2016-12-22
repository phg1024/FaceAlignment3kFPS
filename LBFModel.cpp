#include "LBFModel.h"

#include "extras/tinyxml2/tinyxml2.h"

#include "facedetector.h"
#include "utils.h"

bool LBFModel::train(const string &settingsfile)
{
  cout << "training model with setting file " << settingsfile << endl;
  auto trainingSetParams = readSettingFile(settingsfile);
  vector<ImageData> inputimages = loadInputImages(trainingSetParams);
  cout << "number of input images = " << inputimages.size() << endl;

  TrainingSample samples = generateTrainingSamples(inputimages);

  // train the model with samples and input images
  trainModel(inputimages, samples);

  return true;
}

map<string, string> LBFModel::readSettingFile(const string &filename)
{
  tinyxml2::XMLDocument doc;
  doc.LoadFile(filename.c_str());
  auto root = doc.FirstChild();

  map<string, string> trainingSetParams;
  auto child = root->FirstChild();
  while (child != nullptr) {
    if (string(child->ToElement()->Name()) != "ModelParameters")
      trainingSetParams[child->ToElement()->Name()] = child->ToElement()->GetText();
    else {
      // parse model parameters
      params.window_size = stoi(child->FirstChildElement("windowsize")->GetText());
      params.T = stoi(child->FirstChildElement("T")->GetText());
      params.N = stoi(child->FirstChildElement("N")->GetText());
      params.D = stoi(child->FirstChildElement("D")->GetText());
    }
    child = child->NextSibling();
  }

  for (auto p : trainingSetParams) {
    cout << p.first << ": " << p.second << endl;
  }

  params.print();
  return trainingSetParams;
}

vector<ImageData> LBFModel::loadInputImages(const map<string, string> &configs) {
  cout << "loading input images ..." << endl;
  string path = configs.at("path");
  string prefix = configs.at("prefix");
  string imgext = configs.at("imgext");
  string ptsext = configs.at("ptsext");
  int digits = stoi(configs.at("digits"));
  int npoints = stoi(configs.at("pointcount"));
  int nimgs = stoi(configs.at("imagecount"));

  vector<ImageData> data;
  data.reserve(nimgs);
  for (int i = 1; i <= nimgs; ++i) {
    string imgfile, ptsfile;
    string idxstr = padWith(toString(i), '0', digits);
    imgfile = path + prefix + idxstr + imgext;
    ptsfile = path + prefix + idxstr + ptsext;

    ImageData d;
    bool valid = true;
    valid &= d.loadImage(imgfile);
    valid &= d.loadPoints(ptsfile);

    if (valid) {
      data.push_back(d);
    }
  }
  return data;
}

TrainingSample LBFModel::generateTrainingSamples(vector<ImageData> &inputimages) {
  // find out valid input images
  vector<pair<int, FaceDetector::BoundingBox>> validSamples;
  validSamples.reserve(inputimages.size());

  // perform face detection to get the bounding boxes
  const double CUTOFF = 0.75;
  for (int imgidx = 0; imgidx < inputimages.size();++imgidx) {
    auto &img = inputimages[imgidx];
    auto boxes = FaceDetector::detectFace(img.img);

    // test if the box is valid
    for (auto &box : boxes) {
      int count = 0;
      for (int pidx = 0; pidx < img.pts.rows()/2; ++pidx) {
        double x = img.pts(pidx * 2), y = img.pts(pidx * 2 + 1);
        if (box.isInside(x, y)) ++count;
      }
      double perc = (double)count / (double)(img.pts.rows() / 2);
      if (perc > CUTOFF) {
        validSamples.push_back(make_pair(imgidx, box));
        break;
      }
    }
  }

  cout << "Total number of valid input images = " << validSamples.size() << endl;

  // scale the valid samples properly
  const int wsize = params.window_size;
  for (int i = 0; i < validSamples.size(); ++i) {
    auto sample = validSamples[i];
    int idx = sample.first;
    auto box = sample.second;
    // scale the image
    double bsize = box.size();
    double scale = wsize/bsize;
    cv::Mat regimg;
    /// resize the cutout image
    cv::resize(inputimages[idx].img, regimg, Size(0, 0), scale, scale);
    inputimages[idx].img = regimg;
    inputimages[idx].pts *= scale;
  }

  // generate training samples
  const int oversamples = 20;
  int N = oversamples * validSamples.size();
  int Lfp = inputimages.front().pts.rows();

  TrainingSample samples;
  samples.imgidx.resize(N);
  samples.truth.resize(N, Lfp);
  samples.guess.resize(N, Lfp);

  std::random_device rd;
  std::default_random_engine e1(rd());
  std::uniform_int_distribution<int> uniform_dist(0, validSamples.size());

  for (int i = 0, sidx = 0; i < validSamples.size(); ++i) {
    // create random samples
    for (int j = 0; j < oversamples; ++j, ++sidx) {
      auto sample = validSamples[uniform_dist(e1)];
      int idx = sample.first;
      auto box = sample.second;

      samples.imgidx[sidx] = idx;
      samples.truth.row(sidx) = inputimages[validSamples[i].first].pts;
      samples.guess.row(sidx) = inputimages[idx].pts;
    }
  }
  return samples;
}

void LBFModel::trainModel(vector<ImageData> &imgdata, TrainingSample &samples)
{
  int Lfp = imgdata.front().pts.rows();
  int Nfp = Lfp / 2;
  int nsamples = samples.guess.rows();

  // compute a meanshape as reference shape
  Eigen::VectorXd meanshape = samples.truth.colwise().mean();
  Eigen::Vector2d leftPupil = extractPoint(meanshape, 37) + extractPoint(meanshape, 38) + extractPoint(meanshape, 40) + extractPoint(meanshape, 41);
  Eigen::Vector2d rightPupil = extractPoint(meanshape, 43) + extractPoint(meanshape, 44) + extractPoint(meanshape, 46) + extractPoint(meanshape, 47);
  double ref_dist = (leftPupil - rightPupil).norm();

  for (int t = 0; t < params.T; ++t) {
    // compute the transformation from guess shape to the meanshape
    vector<Eigen::Matrix2d> M(nsamples);
    vector<Eigen::Matrix2d> invM(nsamples);
    for (int i = 0; i < nsamples; ++i) {
      M[i] = Transform::estimateSimilarityTransform(samples.guess.row(i), meanshape);
      invM[i] = M[i].inverse();
    }

    // compute the deltashape
    Eigen::MatrixXd deltashape = samples.truth - samples.guess;

    // find local binary features for each landmark
    MappingFunction phi;
    for (int l = 0; l < Nfp; ++l) {
      LandmarkMappingFunction lbf;

      // sample 500 locations around each landmark in the meanshape space, the range of sampling is determined by cross-validation
      // i.e. for 10 discrete radius, the trees are grown and then applied on the validation set.
      // the radius can be 0.25, 0.225, 0.20, 0.175, 0.15, 0.125, 0.10, 0.075, 0.05, 0.025. (normalized by the distance between pupils)

      double radius[] = { 0.25, 0.225, 0.20, 0.175, 0.15, 0.125, 0.10, 0.075, 0.05, 0.025 };

      const int Nlocations = params.Npixels;
      double radius_t = radius[t];
      // sample the locations
      // FIXME implement randn
      //lbf.locations = arma::randn(500, 2);
      lbf.locations *= (radius_t * ref_dist);

      // get the pixel values by transforming back to the image space
      Eigen::MatrixXd pixels(nsamples, Nlocations);
      // XXX

      Eigen::MatrixXd ds = deltashape.block(0, l * 2, deltashape.rows(), 2);

      // grow N trees for this landmark, and compute the the local binary feature
      lbf.forest.init(params.N, params.D, params.Ndims, 0.05);
      lbf.forest.train(pixels, ds);

      phi.push_back(lbf);
    }

    // global linear regression on the training data using LBFs

    // collect the feature vectors

    // deltaS matrix

    // linear regression with regularization

    // update the guess shapes
  }
}

bool LBFModel::batch_test(const string &settingsfile)
{
  cout << "batch test with setting file " << settingsfile << endl;

  return true;
}

bool LBFModel::test(const string &imgfile)
{
  cout << "test with image file " << imgfile << endl;

  return true;
}

bool LBFModel::load(const string &modelfile)
{
  cout << "loading model file " << modelfile << endl;

  return true;
}

bool LBFModel::save(const string &modelfile)
{
  cout << "saving model to file " << modelfile << endl;

  return true;
}

bool ImageData::loadImage(const string &filename)
{
  try {
    cout << "loading image " << filename << endl;
    original = imread(filename.c_str(), CV_LOAD_IMAGE_UNCHANGED);
    img = imread(filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    cout << "image size = " << img.cols << "x" << img.rows << endl;
    return img.cols > 0 && img.rows > 0;
  }
  catch (exception e) {
    return false;
  }
}

bool ImageData::loadPoints(const string &ptsfile)
{
  cout << "loading points " << ptsfile << endl;
  try {
    ifstream f(ptsfile);
    if (!f.good()) return false;
    string version_tag;
    f >> version_tag;
    std::getline(f, version_tag);
    string pointcount_tag;
    int npoints;
    f >> pointcount_tag >> npoints;
    cout << pointcount_tag << npoints << endl;
    f.ignore();
    string dummy;
    std::getline(f, dummy);

    pts = Eigen::VectorXd(npoints * 2);

    for (int i = 0; i < npoints; ++i) {
      f >> pts[i * 2] >> pts[i * 2 + 1];
    }
    f.close();
  }
  catch (exception e) {
    return false;
  }
  return true;
}
