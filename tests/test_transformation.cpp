#include <iostream>
using namespace std;

#define CATCH_CONFIG_MAIN
#include "../extras/Catch/single_include/catch.hpp"

#include "../transformations.h"

TEST_CASE("Tests for transformation estimation", "[Transform]") {
  SECTION( "Simple Case" ) {
    Eigen::VectorXd p(8), q(8);

    p << 1., 0.,
         0., 1.,
         -1, 0,
         0, -1;

    q << 0., 1.,
        -1., 0.,
         0, -1,
         1,  0;

    auto R = Transform::estimateSimilarityTransform(p, q);
    Eigen::Matrix2d R_ref;
    R_ref << 0, -1, 1, 0;
    REQUIRE( R == R_ref );
  }

  SECTION( "Complex Case" ) {
    Eigen::VectorXd p(30), q(30);

    p <<
    0.9266,    0.4774,
    0.0949,    0.3055,
    0.3754,    0.5163,
    0.5460,    0.7070,
    0.1117,    0.8136,
    0.9045,    0.3158,
    0.6333,    0.3113,
    0.9054,    0.3450,
    0.6306,    0.6663,
    0.0142,    0.8611,
    0.3165,    0.7618,
    0.1119,    0.8758,
    0.6295,    0.8712,
    0.0607,    0.1728,
    0.6740,    0.8502;

    q <<
    0.8317,    1.0184,
    0.8777,    0.7956,
    0.8213,    0.8708,
    0.7702,    0.9165,
    0.7416,    0.8001,
    0.8750,    1.0125,
    0.8762,    0.9399,
    0.8672,    1.0128,
    0.7811,    0.9391,
    0.7289,    0.7740,
    0.7555,    0.8550,
    0.7250,    0.8002,
    0.7262,    0.9388,
    0.9133,    0.7865,
    0.7318,    0.9508;

    auto R = Transform::estimateSimilarityTransform(p, q);
    Eigen::Matrix2d R_ref;
    R_ref << 0, -cos(37), cos(37), 0;
    R_ref *= 0.35;

    REQUIRE( (R - R_ref).norm() < 1e-4 );
  }
}
