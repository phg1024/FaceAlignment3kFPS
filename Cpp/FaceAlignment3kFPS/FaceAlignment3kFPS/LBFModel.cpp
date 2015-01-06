#include "LBFModel.h"

#include "extras/tinyxml2/tinyxml2.h"

#include "utils.h"

bool LBFModel::train(const string &settingsfile)
{
  cout << "training model with setting file " << settingsfile << endl;
  auto trainingSetParams = readSettingFile(settingsfile);
  vector<ImageData> inputimages = loadInputImages(trainingSetParams);
  cout << "number of input images = " << inputimages.size() << endl;
  
  TrainingSample samples = generateTrainingSamples(inputimages);

  // train the model with samples and input images

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

TrainingSample generateTrainingSamples(const vector<ImageData> &inputimages) {
  // find out valid input images
  vector<int> validimages;
  validimages.reserve(inputimages.size());

  // perform face detection to get the bounding boxes


  // generate training samples
  TrainingSample samples;

  return samples;
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

    pts = vec(npoints * 2);

    for (int i = 0; i < npoints; ++i) {
      f >> pts(i * 2) >> pts(i * 2 + 1);
    }
    f.close();
  }
  catch (exception e) {
    return false;
  }
  return true;
}
