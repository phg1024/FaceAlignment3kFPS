#pragma once

#include "numerical.hpp"

namespace Transform {
  // similarity transformation matrix from S to S0
  arma::mat22 estimateSimilarityTransform(const arma::vec &S, const arma::vec &S0) {

  }

  arma::vec transformShape(const arma::vec &shape, const arma::mat22 &M) {

  }
}