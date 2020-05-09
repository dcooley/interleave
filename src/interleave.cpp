#include "interleave/earcut/interleave.hpp"
#include <Rcpp.h>
#include "interleave/earcut/earcut.hpp"
#include "interleave/interleave.hpp"

// [[Rcpp::export]]
SEXP rcpp_earcut( Rcpp::List& polygon ) {
  return interleave::earcut::earcut( polygon );
}


// [[Rcpp::export]]
SEXP rcpp_interleave( SEXP& obj ) {
  return interleave::interleave( obj );
}


// [[Rcpp::export]]
SEXP rcpp_interleave_point( SEXP& obj, R_xlen_t stride ) {
  // TODO:
  //
  // IF list, loop over each object
  // interleave
  // set strides, n_coordinates etc.
  // where the stride is the width of the matrix.
  // assume the stride is the same for each matrix.

  Rcpp::NumericVector coords = interleave::interleave( obj );
  std::size_t total_coordinates = coords.length() / stride;
  Rcpp::IntegerVector start_indices( total_coordinates );
  R_xlen_t i;

  for( i = 0; i < total_coordinates; ++i ) {
    start_indices[ i ] = i * stride;
  }

  return Rcpp::List::create();

}

// line
// the start indices are the coordinates of the first row of each matrix
// the stride is the width of the matrix
