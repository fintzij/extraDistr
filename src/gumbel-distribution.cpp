#include <Rcpp.h>
using namespace Rcpp;


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
  double z = (x-mu)/sigma;
  return 1/sigma * exp(-(z+exp(-z)));
}

double cdf_gumbel(double x, double mu, double sigma) {
  double z = (x-mu)/sigma;
  return exp(-exp(-z));
}

double invcdf_gumbel(double p, double mu, double sigma) {
  return mu - sigma * log(-log(p));
}


// [[Rcpp::export]]
NumericVector cpp_dgumbel(NumericVector x,
                          NumericVector mu, NumericVector sigma,
                          bool log_prob = false) {

  if (is_true(any(sigma <= 0)))
    throw Rcpp::exception("Values of sigma should be > 0.");

  double z;
  int n  = x.length();
  int nm = mu.length();
  int ns = sigma.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, nm, ns));
  NumericVector p(Nmax);

  for (int i = 0; i < Nmax; i++)
    p[i] = pdf_gumbel(x[i % n], mu[i % nm], sigma[i % ns]);

  if (log_prob)
    for (int i = 0; i < Nmax; i++)
      p[i] = log(p[i]);

  return p;
}


// [[Rcpp::export]]
NumericVector cpp_pgumbel(NumericVector x,
                          NumericVector mu, NumericVector sigma,
                          bool lower_tail = true, bool log_prob = false) {

  if (is_true(any(sigma <= 0)))
    throw Rcpp::exception("Values of sigma should be > 0.");

  double z;
  int n  = x.length();
  int nm = mu.length();
  int ns = sigma.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, nm, ns));
  NumericVector p(Nmax);

  for (int i = 0; i < Nmax; i++)
    p[i] = cdf_gumbel(x[i % n], mu[i % nm], sigma[i % ns]);

  if (!lower_tail)
    for (int i = 0; i < Nmax; i++)
      p[i] = 1-p[i];

  if (log_prob)
    for (int i = 0; i < Nmax; i++)
      p[i] = log(p[i]);

  return p;
}


// [[Rcpp::export]]
NumericVector cpp_qgumbel(NumericVector p,
                          NumericVector mu, NumericVector sigma,
                          bool lower_tail = true, bool log_prob = false) {

  if (is_true(any(p < 0)) || is_true(any(p > 1)))
    throw Rcpp::exception("Probabilities should range from 0 to 1.");
  if (is_true(any(sigma <= 0)))
    throw Rcpp::exception("Values of sigma should be > 0.");

  int n  = p.length();
  int nm = mu.length();
  int ns = sigma.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, nm, ns));
  NumericVector q(Nmax);

  if (log_prob)
    for (int i = 0; i < n; i++)
      p[i] = exp(p[i]);

  if (!lower_tail)
    for (int i = 0; i < n; i++)
      p[i] = 1-p[i];

  for (int i = 0; i < Nmax; i++)
    q[i] = invcdf_gumbel(q[i % n], mu[i % nm], sigma[i % ns]);

  return q;
}


// [[Rcpp::export]]
NumericVector cpp_rgumbel(int n,
                          NumericVector mu, NumericVector sigma) {

  if (is_true(any(sigma <= 0)))
    throw Rcpp::exception("Values of sigma should be > 0.");

  double u;
  int nm = mu.length();
  int ns = sigma.length();
  NumericVector x(n);

  for (int i = 0; i < n; i++) {
    u = R::runif(0, 1);
    x[i] = invcdf_gumbel(u, mu[i % nm], sigma[i % ns]);
  }

  return x;
}

