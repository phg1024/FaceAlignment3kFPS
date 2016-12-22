#pragma once

#include "common.h"
#include "numerical.hpp"

namespace Transform {
  // similarity transformation matrix from p to q
  static Eigen::Matrix2d estimateSimilarityTransform(const Eigen::VectorXd &p, const Eigen::VectorXd &q) {
    assert(p.rows() == q.rows());

    int n = p.rows() / 2;
    assert(n>0);
    const int m = 2;

    //cout << "n = " << n << endl;

    Eigen::Map<const Eigen::MatrixXd> pmatT(p.data(), 2, n);
    Eigen::Map<const Eigen::MatrixXd> qmatT(q.data(), 2, n);

    Eigen::MatrixXd pmat = pmatT.transpose();
    Eigen::MatrixXd qmat = qmatT.transpose();

    Eigen::MatrixXd mu_p = pmat.colwise().mean();
    Eigen::MatrixXd mu_q = qmat.colwise().mean();

    Eigen::MatrixXd dp = pmat - mu_p.replicate(n, 1);
    Eigen::MatrixXd dq = qmat - mu_q.replicate(n, 1);

    double sig_p2 = dp.squaredNorm() / n;
    double sig_q2 = dq.squaredNorm() / n;

    Eigen::MatrixXd sig_pq = dq.transpose() * dp / n;

    double det_sig_pq = sig_pq.determinant();
    Eigen::MatrixXd S = Eigen::MatrixXd::Identity(m, m);
    if (det_sig_pq < 0) S(m - 1, m - 1) = -1;

    Eigen::MatrixXd U, V;
    Eigen::VectorXd D;

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(sig_pq);

    D = svd.singularValues();
    U = svd.matrixU();
    V = svd.matrixV();

    /*
    cout << U << endl;
    cout << D << endl;
    cout << V << endl;
    */

    Eigen::MatrixXd R = U * S * V.transpose();
    //cout << R << endl;
    double s = (Eigen::Matrix2d::Identity() * D * S).trace() / sig_p2;

    Eigen::VectorXd t = mu_q.transpose() - s * R * mu_p.transpose();

    R = R * s;
    return R;
  }

  static Eigen::VectorXd transformShape(const Eigen::VectorXd &shape, const Eigen::Matrix2d &M) {
    int n = shape.rows() / 2;
    Eigen::MatrixXd smat = Eigen::Map<const Eigen::MatrixXd>(shape.data(), 2, n);
    Eigen::MatrixXd tsmat = M * smat;
    Eigen::VectorXd res = Eigen::Map<Eigen::VectorXd>(tsmat.data(), shape.rows(), 1);
    return res;
  }
}
