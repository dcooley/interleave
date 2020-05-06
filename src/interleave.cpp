#include "interleave/earcut/interleave.hpp"
#include <Rcpp.h>
#include "interleave/earcut/earcut.hpp"
#include "interleave/interleave.hpp"

// [[Rcpp::export]]
SEXP rcpp_earcut_sfg( Rcpp::List& polygon ) {
  return interleave::earcut::earcut( polygon );
}

// [[Rcpp::export]]
SEXP rcpp_earcut_sfc( Rcpp::List& polygons ) {

  // TODO
  // count n_coordinates up front, and get the 'stride',
  // then fill a vector as we go
  // or
  // fill the list, then 'unlist'

  R_xlen_t n = polygons.length();
  R_xlen_t i;
  Rcpp::List res( n );
  for( i = 0; i < n; ++i ) {
    Rcpp::List polygon = polygons[i];
    res[ i ] = interleave::earcut::earcut( polygon );
  }
  return res;
}


// [[Rcpp::export]]
SEXP rcpp_interleave( SEXP& obj ) {
  return interleave::interleave( obj );
}
