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
SEXP rcpp_interleave_point( SEXP& obj, int stride ) {
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

  Rcpp::IntegerVector n_coordinates = Rcpp::rep( stride, total_coordinates );

  return Rcpp::List::create(
    Rcpp::_["coordinates"] = coords,
    Rcpp::_["start_indices"] = start_indices,
    Rcpp::_["n_coordinates"] = n_coordinates,
    Rcpp::_["stride"] = stride
  );

  return Rcpp::List::create();

}


// [[Rcpp::export]]
SEXP rcpp_interleave_triangle( Rcpp::List& obj ) {

  Rcpp::List lst_coords = interleave::earcut::earcut( obj );

  Rcpp::NumericVector nv = lst_coords["coordinates"];
  int stride = static_cast< int >( lst_coords["stride"] );

  std::size_t total_coordinates = nv.length() / stride;


  // start_indices is the start of each triangle
  // each triangle has 3 coordinates (where a coordinate vector is length stride )
  // so the start indices are 0 + (3 * stride ) + (3 * stride * 2 ) + (3 * stride * 3 ) + ...
  //
  R_xlen_t n_indices = total_coordinates / 3;
  Rcpp::IntegerVector start_indices( n_indices );
  R_xlen_t i;
  //
  for( i = 0; i < n_indices; ++i ) {
    start_indices[ i ] = i * stride * 3;
  }
  //
  // // as we're making triangles, there are always 3 coordinates PER triangle
  Rcpp::IntegerVector n_coordinates = Rcpp::rep( 3, n_indices );
  //
  return Rcpp::List::create(
    Rcpp::_["coordinates"] = nv,
    Rcpp::_["start_indices"] = start_indices,
    Rcpp::_["n_coordinates"] = n_coordinates,
    Rcpp::_["stride"] = stride
  );
}


// line
// the start indices are the coordinates of the first row of each matrix
// the stride is the width of the matrix
