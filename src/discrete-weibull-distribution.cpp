#include <Rcpp.h>
#include "shared.h"

using std::pow;
using std::sqrt;
using std::abs;
using std::exp;
using std::log;
using std::floor;
using std::ceil;
using std::sin;
using std::cos;
using std::tan;
using std::atan;
using Rcpp::IntegerVector;
using Rcpp::NumericVector;
using Rcpp::NumericMatrix;


/*
*  Discrete Weibull distribution
*
*  Values:
*  x >= 0
*
*  Parameters:
*  0 < q < 1
*  beta
*
*  f(x)    = q^x^beta - q^(x+1)^beta
*  F(x)    = 1-q^(x+1)^beta
*  F^-1(p) = ceiling(pow(log(1-p)/log(q), 1/beta) - 1)
*
*  Nakagawa and Osaki (1975), "The Discrete Weibull Distribution",
*  IEEE Transactions on Reliability, R-24, pp. 300-301.
*
*/

double pdf_dweibull(double x, double q, double beta) {
  if (ISNAN(x) || ISNAN(q) || ISNAN(beta))
    return NA_REAL;
  if (q <= 0.0 || q >= 1.0 || beta <= 0.0) {
    Rcpp::warning("NaNs produced");
    return NAN;
  }
  if (!isInteger(x) || x < 0.0)
    return 0.0;
  return pow(q, pow(x, beta)) - pow(q, pow(x+1.0, beta));
}

double cdf_dweibull(double x, double q, double beta) {
  if (ISNAN(x) || ISNAN(q) || ISNAN(beta))
    return NA_REAL;
  if (q <= 0.0 || q >= 1.0 || beta <= 0.0) {
    Rcpp::warning("NaNs produced");
    return NAN;
  }
  if (x < 0.0)
    return 0.0;
  return 1.0 - pow(q, pow(x+1.0, beta));
}

double invcdf_dweibull(double p, double q, double beta) {
  if (ISNAN(p) || ISNAN(q) || ISNAN(beta))
    return NA_REAL;
  if (q <= 0.0 || q >= 1.0 || beta <= 0.0 || p < 0.0 || p > 1.0) {
    Rcpp::warning("NaNs produced");
    return NAN;
  }
  if (p == 0.0)
    return 0.0;
  return ceil(pow(log(1.0 - p)/log(q), 1.0/beta) - 1.0);
}


// [[Rcpp::export]]
NumericVector cpp_ddweibull(
    const NumericVector& x,
    const NumericVector& q,
    const NumericVector& beta,
    const bool& log_prob = false
  ) {

  std::vector<int> dims;
  dims.push_back(x.length());
  dims.push_back(q.length());
  dims.push_back(beta.length());
  int Nmax = *std::max_element(dims.begin(), dims.end());
  NumericVector p(Nmax);

  for (int i = 0; i < Nmax; i++)
    p[i] = pdf_dweibull(x[i % dims[0]], q[i % dims[1]], beta[i % dims[2]]);

  if (log_prob)
    p = Rcpp::log(p);

  return p;
}


// [[Rcpp::export]]
NumericVector cpp_pdweibull(
    const NumericVector& x,
    const NumericVector& q,
    const NumericVector& beta,
    const bool& lower_tail = true,
    const bool& log_prob = false
  ) {

  std::vector<int> dims;
  dims.push_back(x.length());
  dims.push_back(q.length());
  dims.push_back(beta.length());
  int Nmax = *std::max_element(dims.begin(), dims.end());
  NumericVector p(Nmax);

  for (int i = 0; i < Nmax; i++)
    p[i] = cdf_dweibull(x[i % dims[0]], q[i % dims[1]], beta[i % dims[2]]);

  if (!lower_tail)
    p = 1.0 - p;
  
  if (log_prob)
    p = Rcpp::log(p);
  
  return p;
}


// [[Rcpp::export]]
NumericVector cpp_qdweibull(
    const NumericVector& p,
    const NumericVector& q,
    const NumericVector& beta,
    const bool& lower_tail = true,
    const bool& log_prob = false
  ) {

  std::vector<int> dims;
  dims.push_back(p.length());
  dims.push_back(q.length());
  dims.push_back(beta.length());
  int Nmax = *std::max_element(dims.begin(), dims.end());
  NumericVector x(Nmax);
  NumericVector pp = Rcpp::clone(p);

  if (log_prob)
    pp = Rcpp::exp(pp);
  
  if (!lower_tail)
    pp = 1.0 - pp;

  for (int i = 0; i < Nmax; i++)
    x[i] = invcdf_dweibull(pp[i % dims[0]], q[i % dims[1]], beta[i % dims[2]]);

  return x;
}


// [[Rcpp::export]]
NumericVector cpp_rdweibull(
    const int& n,
    const NumericVector& q,
    const NumericVector& beta
  ) {

  double u;
  int nq = q.length();
  int nb = beta.length();
  NumericVector x(n);

  for (int i = 0; i < n; i++) {
    u = rng_unif();
    x[i] = invcdf_dweibull(u, q[i % nq], beta[i % nb]);
  }

  return x;
}

