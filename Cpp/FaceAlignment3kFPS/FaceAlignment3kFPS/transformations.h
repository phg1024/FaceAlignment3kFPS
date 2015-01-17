#pragma once

#include "common.h"
#include "numerical.hpp"

namespace Transform {
  // similarity transformation matrix from p to q
  static arma::mat22 estimateSimilarityTransform(const arma::vec &p, const arma::vec &q) {
    assert(p.n_elem == q.n_elem);

    int n = p.n_elem / 2;
    assert(n>0);
    const int m = 2;

    //cout << "n = " << n << endl;

    mat pmat = p, qmat = q;
    pmat.reshape(2, n);
    pmat = trans(pmat);
    qmat.reshape(2, n);
    qmat = trans(qmat);

    mat mu_p = mean(pmat);
    mat mu_q = mean(qmat);

    mat dp = pmat - repmat(mu_p, n, 1);
    mat dq = qmat - repmat(mu_q, n, 1);

    double sig_p2 = sum(sum(dp % dp)) / n;
    double sig_q2 = sum(sum(dq % dq)) / n;

    mat sig_pq = trans(dq) * dp / n;

    double det_sig_pq = det(sig_pq);
    mat S = eye(m, m);
    if (det_sig_pq < 0) S(m - 1, m - 1) = -1;

    mat U, V;
    vec D;
    svd(U, D, V, sig_pq);

    /*
    cout << U << endl;
    cout << D << endl;
    cout << V << endl;
    */

    mat R = U * S * trans(V);
    //cout << R << endl;
    double s = trace(diagmat(D) * S) / sig_p2;
    vec t = trans(mu_q) - s * R * trans(mu_p);

    R = R * s;
    return R;
  }

  static arma::vec transformShape(const arma::vec &shape, const arma::mat22 &M) {
    int n = shape.n_elem / 2;
    mat smat = shape;
    smat.reshape(2, n);
    mat tsmat = M * smat;
    arma::vec res = reshape(tsmat, 1, shape.n_elem);
    return res;
  }
}