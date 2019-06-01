//[[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>

// function to change list into vector
arma::colvec vecrobustm(Rcpp::List a) {
  int size = a.size();
  arma::colvec a1(size);
  for (int i = 0; i < size; i++) {
    a1[i] = a[i];
  }
  return a1;
}

// function to use rlm_m function in R code
// original code from MASS package
// R function has been added this to this package due to trouble pulling from MASS
// future developments should look into putting this into rcpp
Rcpp::List robustm(arma::dmat &x, arma::vec &y) {
  Rcpp::Environment pkg = Rcpp::Environment::namespace_env("modeLLtest");
  Rcpp::Function f = pkg["rlm_m"];
  return f(x, y);
}

// function to use gamma() function from base R
double gamm(double &x) {
  Rcpp::Environment pkg = Rcpp::Environment::base_env();
  Rcpp::Function f = pkg["gamma"];
  return Rcpp::as<double>(f(x));
}

// function to find the cvlls for m method of estimating robust regression
// [[Rcpp::export]]
Rcpp::List cvll_rlm_m(arma::dmat &x, arma::colvec &y, int n_row, int n_col) {

  int n = n_row - 1;
  arma::dmat yv;
  arma::dmat xv;
  arma::rowvec rowyi;
  arma::rowvec rowxi;
  Rcpp::List rlm;
  arma::colvec coef;
  arma::colvec resid;
  double sig;
  Rcpp::List cvll_rlm(n_row);

  // variables for t density function
  double gam_a;
  double gamm_a;
  double gam_b;
  double gamm_b;
  double dst_a;
  Rcpp::NumericVector dst_b;
  double dst_c;
  Rcpp::NumericVector dst_d;
  arma::colvec dst_e;
  double dst_f;
  double dst_g;

  // set up for t density function
  int df = n - n_col;
  gam_a = (df + 1.0) / 2.0;
  gamm_a = gamm(gam_a);
  gam_b = df / 2.0;
  gamm_b = gamm(gam_b);

  for (int i = 0; i < n_row; i++) {
    yv = y.row(i); // define obs i before change y
    rowyi = y.row(i);
    y.shed_row(i); // leaves out observation i
    xv = x.row(i); // define obs i before change x
    rowxi = x.row(i);
    x.shed_row(i); // leaves out observation i but changes x

    // model
    rlm = robustm(x, y);
    coef = vecrobustm(rlm("coefficients"));
    resid = vecrobustm(rlm("residuals")); // residuals
    sig = arma::as_scalar(vecrobustm(rlm("s"))); // scale param

    // t density function - PDF for RR
    dst_a = sig * std::sqrt(df * M_PI);
    dst_b = pow( resid, 2 );
    dst_c = pow( sig, 2 );
    dst_d = df + dst_b / dst_c;
    dst_e = pow( dst_d / df, - (df + 1) / 2 );
    dst_f =  dst_a * gamm_b;
    dst_g = gamm_a / dst_f;
    cvll_rlm[i] = log(dst_g * dst_e);

    // cleaning up
    y.insert_rows(i, rowyi); // add y back in
    x.insert_rows(i, rowxi); // add x back in
    }

  return cvll_rlm;
}


