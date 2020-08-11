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

      R_xlen_t n_geometries = dims.nrow();
      R_xlen_t n_coordinates = dims( n_geometries - 1, 1 );

      // n_coordinates is the total number of coordinates for the given sfg
      // this is what the data needs to be expanded by.

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

  inline SEXP interleave_triangle( SEXP& obj ) {
    if( !Rf_isNewList( obj ) ) {
      Rcpp::stop("interleave - expecting a list");
    }

    Rcpp::List lst = Rcpp::as< Rcpp::List >( obj );

    // TODO:
    // - accept any nested depth (list of lists of matrices)
    // - loop over list (sfc) and triangulate each polygon
    // - unlist the lists at the end

    // I Need the index of the original coordinate so that
    // I can reference the correct row of data for each coordinate
    // right.
    R_xlen_t total_coordinates = 0;
    R_xlen_t n = lst.length();
    R_xlen_t i, j;
    int stride;

    Rcpp::List res_indices( n );
    Rcpp::List res_list( n );

    for( i = 0; i < n; ++i ) {

      // earcut() only works on
      // POLYGONs - list[[ mat1, mat2 ]]
      // and NOT  MULTIPOLYGONs - list[[ list[[ mat1, mat2 ]], list[[ mat3 ]] ]]
      // so each iteration can only use list[[ mat1, ..., matn ]] structures
      //

      Rcpp::List poly = lst[ i ];
      Rcpp::List lst_coords = interleave::earcut::earcut( poly );
      Rcpp::NumericVector nv = lst_coords["coordinates"];

      int this_stride = static_cast< int >( lst_coords["stride"] );

      if( i == 0 ) {
        stride = this_stride;
      } else if ( stride != this_stride ) {
        Rcpp::stop("interleave - polygons have different strides");
      }

      // start_indices: "the index in the coordinate vector where each geometry starts,
      // divided by the stride"
      // - and each geometry is 3 sets of coordinates

      R_xlen_t n_coordinates = nv.length() / this_stride;

      // start indices is every 3rd coordinate?
      // 0, 2, 5, 8, 11, 14,
      R_xlen_t n_indices = n_coordinates / 3; // beach each triangle has 3 coordinates
      Rcpp::IntegerVector start_indices( n_indices );

      for( j = 0; j < n_indices; ++j ) {
        start_indices[ j ] = ( j + total_coordinates ) * 3;
      }

      res_list[ i ] = lst_coords["coordinates"];
      res_indices[ i ] = start_indices;

      total_coordinates = total_coordinates + n_coordinates;
    }


    // earcut works on a list of matrices
    // so if I have a MULTIPOLYGON I need to remove one depth level
    // if I have a LINESTRING I need to nest it to a MULTILINESTRING (i.e. POLYGON)
    // so I need the 'nest()' function of geometries for this.
    // Rcpp::List lst_coords = interleave::earcut::earcut( lst );
    //return lst_coords;

    // Rcpp::NumericVector nv = lst_coords["coordinates"];
    // int stride = static_cast< int >( lst_coords["stride"] );
    //
    // total_coordinates = nv.length() / stride;


    // start_indices is the start of each triangle
    // each triangle has 3 coordinates (where a coordinate vector is length stride )
    // so the start indices are 0 + (3 * stride ) + (3 * stride * 2 ) + (3 * stride * 3 ) + ...
    //
    // R_xlen_t n_indices = total_coordinates / 3;
    // Rcpp::IntegerVector start_indices( n_indices );
    //
    // //
    // for( i = 0; i < n_indices; ++i ) {
    //   start_indices[ i ] = i * 3;
    // }
    //
    // // as we're making triangles, there are always 3 coordinates PER triangle
    // Rcpp::IntegerVector n_coordinates = Rcpp::rep( 3, n_indices );
    //
    return Rcpp::List::create(
      Rcpp::_["coordinates"] = interleave::utils::unlist_list( res_list ),
      Rcpp::_["start_indices"] = interleave::utils::unlist_list( res_indices ),
      // Rcpp::_["n_coordinates"] = n_coordinates,
      Rcpp::_["stride"] = stride
    );
  }


} // primitives
} // interleave

#endif
