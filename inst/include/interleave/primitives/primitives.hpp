#ifndef R_INTERLEAVE_PRIMITIVES_H
#define R_INTERLEAVE_PRIMITIVES_H

#include "interleave/earcut/interleave.hpp"
#include "interleave/interleave.hpp"

#include "geometries/coordinates/dimensions.hpp"

namespace interleave {
namespace primitives {

  inline SEXP interleave_point( SEXP& lst, int stride ) {

    if( !Rf_isNewList( lst ) ) {
      Rcpp::stop("interleave - expecting a list");
    }

    // TODO:
    //
    // IF list, loop over each object
    // interleave
    // set strides, n_coordinates etc.
    // where the stride is the width of the matrix.
    // assume the stride is the same for each matrix.

    Rcpp::NumericVector coords = interleave::interleave( lst );
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
  }


  inline SEXP interleave_line( SEXP& obj, int stride ) {
    if( !Rf_isNewList( obj ) ) {
      Rcpp::stop("interleave - expecting a list");
    }

    Rcpp::List lst = Rcpp::as< Rcpp::List >( obj );

    if( Rf_length( lst ) == 0 ) {
      Rcpp::stop("interleave - empty list");
    }

    // this function will convert ANY input, and treat each matrix as its own linestring
    // so a multipolygon or polygon will have all rings as separate 'start_indices'
    // expects as input a list of geometry shapes
    R_xlen_t total_coordinates = 0;
    R_xlen_t n = Rf_length( lst );
    R_xlen_t i;

    Rcpp::List res_indices( n );
    Rcpp::List res_list( n );

    //Rcpp::IntegerVector geometry_coordinates( n );

    for( i = 0; i < n; ++i ) {

      SEXP g = lst[ i ];
      Rcpp::List dimension = geometries::coordinates::geometry_dimensions( g );
      Rcpp::IntegerMatrix dims = dimension["dimensions"];
      //Rcpp::Rcout << "dims: " << dims << std::endl;
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


} // primitives
} // interleave

#endif
