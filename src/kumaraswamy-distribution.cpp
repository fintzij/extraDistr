#include <Rcpp.h>
using namespace Rcpp;


/*
*  Kumaraswamy distribution
*
*  Values:
*  x in [0, 1]
*
*  Parameters:
*  a > 0
*  b > 0
*
*  f(x)    = a*b*x^{a-1}*(1-x^a)^{b-1}
*  F(x)    = 1-(1-x^a)^b
*  F^-1(p) = 1-(1-p^{1/b})^{1/a}
*
*/

double pdf_kumar(double x, double a, double b) {
  if (x >= 0 && x <= 1)
    return a*b * pow(x, a-1) * pow(1-pow(x, a), b-1);
  else
    return 0;
}

double cdf_kumar(double x, double a, double b) {
  if (x >= 0 && x <= 1)
    return 1 - pow(1 - pow(x, a), b);
  else
    return 0;
}

double invcdf_kumar(double p, double a, double b) {
  return pow(1 - pow(1-p, 1/b), 1/a);
}

double logpdf_kumar(double x, double a, double b) {
  if (x >= 0 && x <= 1)
    return log(a) + log(b) + log(x)*(a-1) + log(1-pow(x, a))*(b-1);
  else
    return -INFINITY;
}


// [[Rcpp::export]]
NumericVector cpp_dkumar(NumericVector x,
                         NumericVector a, NumericVector b,
                         bool log_prob = false) {

  if (is_true(any(a <= 0)) || is_true(any(b <= 0)))
    throw Rcpp::exception("Values of a and b should be > 0.");

  int n  = x.length();
  int na = a.length();
  int nb = b.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, na, nb));
  NumericVector p(Nmax);

  for (int i = 0; i < Nmax; i++)
    p[i] = logpdf_kumar(x[i % n], a[i % na], b[i % nb]);

  if (!log_prob)
    for (int i = 0; i < Nmax; i++)
      p[i] = exp(p[i]);

  return p;
}


// [[Rcpp::export]]
NumericVector cpp_pkumar(NumericVector x,
                         NumericVector a, NumericVector b,
                         bool lower_tail = true, bool log_prob = false) {

  if (is_true(any(a <= 0)) || is_true(any(b <= 0)))
    throw Rcpp::exception("Values of a and b should be > 0.");

  int n  = x.length();
  int na = a.length();
  int nb = b.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, na, nb));
  NumericVector p(Nmax);
  double z;

  for (int i = 0; i < Nmax; i++)
    p[i] = cdf_kumar(x[i % n], a[i % na], b[i % nb]);

  if (!lower_tail)
    for (int i = 0; i < Nmax; i++)
      p[i] = 1-p[i];

  if (log_prob)
    for (int i = 0; i < Nmax; i++)
      p[i] = log(p[i]);

  return p;
}


// [[Rcpp::export]]
NumericVector cpp_qkumar(NumericVector p,
                         NumericVector a, NumericVector b,
                         bool lower_tail = true, bool log_prob = false) {

  if (is_true(any(p < 0)) || is_true(any(p > 1)))
    throw Rcpp::exception("Probabilities should range from 0 to 1.");
  if (is_true(any(a <= 0)) || is_true(any(b <= 0)))
    throw Rcpp::exception("Values of a and b should be > 0.");

  int n  = p.length();
  int na = a.length();
  int nb = b.length();
  int Nmax = Rcpp::max(IntegerVector::create(n, na, nb));
  NumericVector q(Nmax);

  if (log_prob)
    for (int i = 0; i < n; i++)
      p[i] = exp(p[i]);

  if (!lower_tail)
    for (int i = 0; i < n; i++)
      p[i] = 1-p[i];

  for (int i = 0; i < Nmax; i++)
    q[i] = invcdf_kumar(p[i % n], a[i % na], b[i % nb]);

  return q;
}


// [[Rcpp::export]]
NumericVector cpp_rkumar(int n,
                         NumericVector a, NumericVector b) {

  if (is_true(any(a <= 0)) || is_true(any(b <= 0)))
    throw Rcpp::exception("Values of a and b should be > 0.");

  double u;
  int na = a.length();
  int nb = b.length();
  NumericVector x(n);

  for (int i = 0; i < n; i++) {
    u = R::runif(0, 1);
    x[i] = invcdf_kumar(u, a[i % na], b[i % nb]);
  }

  return x;
}

