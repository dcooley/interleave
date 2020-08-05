#include "interleave/earcut/interleave.hpp"
#include "interleave/primitives/primitives.hpp"

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


// There will be three functions whose outputs are desigend
// for WebGL applications
//
// interleave_point
// - converts any shape into a "POINT" object
//
// interleave_line
// - converts any shape into a "LINE" object
//
// interleave_triangle
// - converts any shape into a "TRIANGLE" object
//
//

// [[Rcpp::export]]
SEXP rcpp_interleave_point( SEXP& lst, int stride ) {
  return interleave::primitives::interleave_point( lst, stride );
}


// [[Rcpp::export]]
SEXP rcpp_interleave_line( SEXP& lst, int stride ) {
  return interleave::primitives::interleave_line( lst, stride );
}


// [[Rcpp::export]]
SEXP rcpp_interleave_triangle( Rcpp::List& lst ) {

  if( !Rf_isNewList( lst ) ) {
    Rcpp::stop("interleave - expecting a list");
  }

  // TODO:
  // - accept any nested depth (list of lists of matrices)
  // - loop over list (sfc) and triangulate each polygon
  // - unilst the lists at the end

  // I Need the index of the original coordinate so that
  // I can reference the correct row of data for each coordinate
  // right.
  R_xlen_t total_coordinates = 0;
  R_xlen_t n = lst.length();
  R_xlen_t i;

  Rcpp::List res_indices( n );
  Rcpp::List res_list( n );

  for( i = 0; i < n; ++i ) {

  }


  // earcut works on a list of matrices
  // so if I have a MULTIPOLYGON I need to remove one depth level
  // if I have a LINESTRING I need to nest it to a MULTILINESTRING (i.e. POLYGON)
  // so I need the 'nest()' function of geometries for this.
  Rcpp::List lst_coords = interleave::earcut::earcut( lst );

  Rcpp::NumericVector nv = lst_coords["coordinates"];
  int stride = static_cast< int >( lst_coords["stride"] );

  total_coordinates = nv.length() / stride;


  // start_indices is the start of each triangle
  // each triangle has 3 coordinates (where a coordinate vector is length stride )
  // so the start indices are 0 + (3 * stride ) + (3 * stride * 2 ) + (3 * stride * 3 ) + ...
  //
  R_xlen_t n_indices = total_coordinates / 3;
  Rcpp::IntegerVector start_indices( n_indices );

  //
  for( i = 0; i < n_indices; ++i ) {
    start_indices[ i ] = i * 3;
  }
  //
  // // as we're making triangles, there are always 3 coordinates PER triangle
  Rcpp::IntegerVector n_coordinates = Rcpp::rep( 3, n_indices );
  //
  return Rcpp::List::create(
    Rcpp::_["coordinates"] = nv,
    Rcpp::_["start_indices"] = start_indices,
    // Rcpp::_["n_coordinates"] = n_coordinates,
    Rcpp::_["stride"] = stride
  );
}


// line
// the start indices are the coordinates of the first row of each matrix
// the stride is the width of the matrix
