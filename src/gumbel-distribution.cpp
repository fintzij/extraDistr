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
 *  Gumbel distribution
 *
 *  Values:
 *  x
 *
 *  Parameters:
 *  mu
 *  sigma > 0
 *
 *  z       = (x-mu)/sigma
 *  f(x)    = 1/sigma * exp(-(z+exp(-z)))
 *  F(x)    = exp(-exp(-z))
 *  F^-1(p) = mu - sigma * log(-log(p))
 *
 */

double pdf_gumbel(double x, double mu, double sigma) {
  if (ISNAN(x) || ISNAN(mu) || ISNAN(sigma))
    return NA_REAL;
  if (sigma <= 0.0) {
    Rcpp::warning("NaNs produced");
    return NAN;
  }
  if (!R_FINITE(x))
    return 0.0;
  double z = (x-mu)/sigma;
  return exp(-(z+exp(-z)))/sigma;
}

double cdf_gumbel(double x, double mu, double sigma) {
  if (ISNAN(x) || ISNAN(mu) || ISNAN(sigma))
    return NA_REAL;
  if (sigma <= 0.0) {
    Rcpp::warning("NaNs produced");
    return NAN;
  }
  double z = (x-mu)/sigma;
  return exp(-exp(-z));
}

double invcdf_gumbel(double p, double mu, double sigma) {
  if (ISNAN(p) || ISNAN(mu) || ISNAN(sigma))
    return NA_REAL;
  if (sigma <= 0.0 || p < 0.0 || p > 1.0) {
    Rcpp::warning("NaNs produced");
    return NAN;
  }
  return mu - sigma * log(-log(p));
}


// [[Rcpp::export]]
NumericVector cpp_dgumbel(
    const NumericVector& x,
    const NumericVector& mu,
    const NumericVector& sigma,
    bool log_prob = false
  ) {

  int n  = x.length();
  int nm = mu.length();
  int ns = sigma.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, nm, ns));
  NumericVector p(Nmax);

  for (int i = 0; i < Nmax; i++)
    p[i] = pdf_gumbel(x[i % n], mu[i % nm], sigma[i % ns]);

  if (log_prob)
    p = Rcpp::log(p);

  return p;
}


// [[Rcpp::export]]
NumericVector cpp_pgumbel(
    const NumericVector& x,
    const NumericVector& mu,
    const NumericVector& sigma,
    bool lower_tail = true, bool log_prob = false
  ) {

  int n  = x.length();
  int nm = mu.length();
  int ns = sigma.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, nm, ns));
  NumericVector p(Nmax);

  for (int i = 0; i < Nmax; i++)
    p[i] = cdf_gumbel(x[i % n], mu[i % nm], sigma[i % ns]);

  if (!lower_tail)
    p = 1.0 - p;
  
  if (log_prob)
    p = Rcpp::log(p);

  return p;
}


// [[Rcpp::export]]
NumericVector cpp_qgumbel(
    const NumericVector& p,
    const NumericVector& mu,
    const NumericVector& sigma,
    bool lower_tail = true, bool log_prob = false
  ) {

  int n  = p.length();
  int nm = mu.length();
  int ns = sigma.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, nm, ns));
  NumericVector q(Nmax);
  NumericVector pp = Rcpp::clone(p);

  if (log_prob)
    pp = Rcpp::exp(pp);
  
  if (!lower_tail)
    pp = 1.0 - pp;

  for (int i = 0; i < Nmax; i++)
    q[i] = invcdf_gumbel(pp[i % n], mu[i % nm], sigma[i % ns]);

  return q;
}


// [[Rcpp::export]]
NumericVector cpp_rgumbel(
    const int n,
    const NumericVector& mu,
    const NumericVector& sigma
  ) {

  double u;
  int nm = mu.length();
  int ns = sigma.length();
  NumericVector x(n);

  for (int i = 0; i < n; i++) {
    u = rng_unif();
    x[i] = invcdf_gumbel(u, mu[i % nm], sigma[i % ns]);
  }

  return x;
}

