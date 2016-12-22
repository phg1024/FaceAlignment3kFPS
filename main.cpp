#include "common.h"
#include "LBFModel.h"

void printHelp() {
  cout << "usage: " << endl;
  cout << "train model: FaceAlignment3kFPS -train [training setting file] -output [model file]" << endl;
  cout << "single test: FaceAlignment3kFPS -test [image file] -model [model file]" << endl;
  cout << "batch tests: FaceAlignment3kFPS -batch_test [test setting file] -model [model file]" << endl;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printHelp();
  }
  else {
    unordered_map<string, string> args;
    for (int i = 1; i < argc; ++i) {
      string arg = argv[i];
      if (arg[0] == '-') {
        args[arg] = argv[i + 1];
      }
    }
    for (auto p : args) {
      cout << p.first << ": " << p.second << endl;
    }

    if (args.find("-train") != args.end()) {
      // train a model
      LBFModel model;
      model.train(args["-train"]);
      model.save(args["-output"]);
    }    
    else if (args.find("-test") != args.end()) {
      // single test 
      LBFModel model(args["-model"]);
      model.test(args["-test"]);
    }
    else if (args.find("-batch_test") != args.end()) {
      // batch test
      LBFModel model(args["-model"]);
      model.batch_test(args["-batch_test"]);
    }
  }  
  return 0;
}