#include "interleave/earcut/interleave.hpp"
#include <Rcpp.h>
#include "interleave/earcut/earcut.hpp"
#include "interleave/interleave.hpp"

//#include "geometries/coordinates/coordinates.hpp"
#include "geometries/coordinates/dimensions.hpp"


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
SEXP rcpp_interleave_point( SEXP& obj, int stride ) {
  // TODO:
  //
  // IF list, loop over each object
  // interleave
  // set strides, n_coordinates etc.
  // where the stride is the width of the matrix.
  // assume the stride is the same for each matrix.

  Rcpp::NumericVector coords = interleave::interleave( obj );
  R_xlen_t total_coordinates = coords.length() / stride;
  Rcpp::IntegerVector start_indices( total_coordinates );
  R_xlen_t i;

  for( i = 0; i < total_coordinates; ++i ) {
    start_indices[ i ] = i * 1;
  }

  Rcpp::IntegerVector n_coordinates = Rcpp::rep( stride, total_coordinates );

  return Rcpp::List::create(
    Rcpp::_["coordinates"] = coords,
    Rcpp::_["start_indices"] = start_indices,
    // Rcpp::_["n_coordinates"] = n_coordinates,
    Rcpp::_["stride"] = stride
  );

  //return Rcpp::List::create();

}


// [[Rcpp::export]]
SEXP rcpp_interleave_line( Rcpp::List& obj, int stride ) {

  // this function will convert ANY input, and treat each matrix as its own linestring
  // so a multipolygon or polygon will have all rings as separate 'start_indices'
  // expects as input a list of geometry shapes
  R_xlen_t total_coordinates = 0;
  R_xlen_t n = obj.length();
  R_xlen_t i;

  Rcpp::List res_indices( n );
  Rcpp::List res_list( n );

  //Rcpp::IntegerVector geometry_coordinates( n );

  for( i = 0; i < n; ++i ) {

    SEXP g = obj[ i ];
    Rcpp::List dimension = geometries::coordinates::geometry_dimensions( g );
    Rcpp::IntegerMatrix dims = dimension["dimensions"];
    //Rcpp::IntegerMatrix coords = geometries::coordinates::coordinate_indices( g );

    R_xlen_t n_geometries = dims.nrow();
    R_xlen_t n_coordinates = dims( n_geometries - 1, 1 );

    //Rcpp::Rcout << "n_coordinates " << n_coordinates << std::endl;

    // n_coordinates is the total number of coordinates for the given sfg
    // this is what the data needs to be expanded by.
    //n_coordinates = n_coordinates + 1;
    //geometry_coordinates[ i ] = n_coordinates;

    Rcpp::IntegerVector start_indices = dims( Rcpp::_, 0 );
    start_indices = start_indices + total_coordinates;

    res_indices[ i ] = start_indices;
    res_list[ i ] = interleave::interleave( g );

    total_coordinates = total_coordinates + n_coordinates;

  }


  //return Rcpp::List::create();

  // // TODO
  // // safely handle this conversion
  // Rcpp::List lst = Rcpp::as< Rcpp::List >( obj );
  // Rcpp::IntegerMatrix coordinate_indices = geometries::coordinates::coordinate_indices( lst );
  //
  // Rcpp::IntegerVector start_indices = coordinate_indices( Rcpp::_, 0 );
  // R_xlen_t n_geometries = coordinate_indices.nrow();
  // R_xlen_t n_coordinates = coordinate_indices( n_geometries - 1, 1 );
  //
  // Rcpp::NumericVector nv = interleave::interleave( obj );
  //
  return Rcpp::List::create(
    Rcpp::_["coordinates"] = interleave::utils::unlist_list( res_list ),
    Rcpp::_["start_indices"] = interleave::utils::unlist_list( res_indices ),
    //Rcpp::_["n_coordinates"] = geometry_coordinates,
    Rcpp::_["stride"] = stride
  );

}


// [[Rcpp::export]]
SEXP rcpp_interleave_triangle( Rcpp::List& obj ) {

  // TODO:
  // - accept any nested depth (list of lists of matrices)
  // - loop over list (sfc) and triangulate each polygon
  // - unilst the lists at the end

  // I Need the index of the original coordinate so that
  // I can reference the correct row of data for each coordinate
  // right.
  R_xlen_t total_coordinates = 0;
  R_xlen_t n = obj.length();
  R_xlen_t i;

  Rcpp::List res_indices( n );
  Rcpp::List res_list( n );

  for( i = 0; i < n; ++i ) {

  }


  // earcut works on a list of matrices
  // so if I have a MULTIPOLYGON I need to remove one depth level
  // if I have a LINESTRING I need to nest it to a MULTILINESTRING (i.e. POLYGON)
  // so I need the 'nest()' function of geometries for this.
  Rcpp::List lst_coords = interleave::earcut::earcut( obj );

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
