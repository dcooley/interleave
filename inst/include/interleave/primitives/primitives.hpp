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

  template< int RTYPE >
  inline Rcpp::Vector< RTYPE > subset_vector(
    Rcpp::Vector< RTYPE >& v,
    Rcpp::IntegerVector& indices
  ) {
    return v[ indices ];
  }

  inline SEXP subset_vector(
    SEXP& v,
    Rcpp::IntegerVector& indices
  ) {
    switch( TYPEOF( v ) ) {
      case LGLSXP: {}
      case INTSXP: {
        Rcpp::IntegerVector iv = Rcpp::as< Rcpp::IntegerVector >( v );
        return subset_vector( iv, indices );
      }
      case REALSXP: {
        Rcpp::NumericVector nv = Rcpp::as< Rcpp::NumericVector >( v );
        return subset_vector( nv, indices );
      }
      case STRSXP: {
        Rcpp::StringVector sv = Rcpp::as< Rcpp::StringVector >( v );
        return subset_vector( sv, indices );
      }
      default: {
          Rcpp::stop("interleave - unknown vector type");
        }
    }
    return Rcpp::List::create(); // #nocov never reaches
  }

  // List properties will be a list of vectors, where each vector is the same length
  // and it is equal to the number of rows
  inline SEXP interleave_triangle(
      SEXP& obj,
      Rcpp::List properties,
      bool keep_indices = true
  ) {

    if( !Rf_isNewList( obj ) ) {
      Rcpp::stop("interleave - expecting a list");
    }

    Rcpp::List lst = Rcpp::as< Rcpp::List >( obj );

    bool shuffle_properties = properties.length() > 0;

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

    Rcpp::List res_coordinates( n );
    Rcpp::List res_start_indices( n );
    Rcpp::List res_indices( n );
    SEXP input_indices; // filled later if required
    Rcpp::IntegerVector property_indexes;
    //Rcpp::List res_original_geometry_index( n );
    //Rcpp::List res_properties( n );

    //R_xlen_t max_index = 0;
    R_xlen_t total_rows = 0;

    for( i = 0; i < n; ++i ) {

      // earcut() only works on
      // POLYGONs - list[[ mat1, mat2 ]]
      // and NOT  MULTIPOLYGONs - list[[ list[[ mat1, mat2 ]], list[[ mat3 ]] ]]
      // so each iteration can only use list[[ mat1, ..., matn ]] structures
      //

      Rcpp::List poly = lst[ i ];
      Rcpp::List lst_coords = interleave::earcut::earcut( poly );
      Rcpp::NumericVector nv = lst_coords["coordinates"];
      Rcpp::IntegerVector iv = lst_coords["indices"];

      //Rcpp::Rcout << "indices: " << iv << std::endl;

      int this_stride = static_cast< int >( lst_coords["stride"] );

      if( i == 0 ) {
        stride = this_stride;
      } else if ( stride != this_stride ) {
        Rcpp::stop("interleave - polygons have different strides");
      }

      // start_indices: "the index in the coordinate vector where each geometry starts,
      // divided by the stride"
      // - and each geometry is 3 sets of coordinates


      R_xlen_t n_coordinates = iv.length(); // nv.length() / this_stride;
      //int int_coords = static_cast< int >( n_coordinates );
      //Rcpp::IntegerVector original_index( int_coords, i );
      //Rcpp::IntegerVector original_index( n_coordinates );
      //std::fill( original_index.begin(), original_index.end(), i );

      // start indices is every 3rd coordinate?
      // 0, 2, 5, 8, 11, 14,
      R_xlen_t n_indices = n_coordinates / 3; // beach each triangle has 3 coordinates
      Rcpp::IntegerVector start_indices( n_indices );

      for( j = 0; j < n_indices; ++j ) {
        start_indices[ j ] = ( j + total_coordinates ) * 3;
      }

      res_coordinates[ i ] = nv; //lst_coords["coordinates"];
      res_start_indices[ i ] = start_indices;

      //Rcpp::Rcout << "total_coordinates: " << total_coordinates << std::endl;

      if( keep_indices || shuffle_properties ) {
        // don't need to do this if we don't have any properties to shuffle
        // max_index = max_index + Rcpp::max( iv );
        // -- can't use the 'max_index' because I can't guarantee the max index is used in a
        // -- closed polygon (as opposed to the 0th indexed element)
        // so we need to know the dimension of the input polygons
        R_xlen_t n_row = 0;
        for( j = 0; j < poly.length(); ++j ) {
          SEXP ring = poly[ j ];
          n_row = n_row + geometries::utils::sexp_n_row( ring );
        }

        Rcpp::IntegerVector origin_index = iv + total_rows;  // Need to add the number of coordinates in the original matrix/geometry
        total_rows = total_rows + n_row;

        //Rcpp::Rcout << "origin_index: " << origin_index << std::endl;

        res_indices[ i ] = origin_index;  // add the index of the position of the geometry
        //res_original_geometry_index[ i ] = original_index;
      }

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

    if( keep_indices || shuffle_properties ) {
      input_indices = interleave::utils::unlist_list( res_indices ); // can't unlist_list this if it's NULL
      property_indexes = Rcpp::as< Rcpp::IntegerVector >( input_indices );
    }

    // Shuffling properties
    if( shuffle_properties ) {
      // we can unlist all the input properties, then use the `input_index` + `geometry_index`
      // to give the correct row
      // but how should it be returned?
      // as a 'properties' object?

      R_xlen_t n_properties = properties.length();
      for( i = 0; i < n_properties; ++i ) {
        // each property will be a list of vectors
        Rcpp::List this_property = properties[ i ];
        SEXP p = interleave::utils::unlist_list( this_property );

        properties[ i ] = subset_vector( p, property_indexes );

      }

    // } else {
    //   res_indices = Rcpp::List::create();
    }


    return Rcpp::List::create(
      Rcpp::_["coordinates"] = interleave::utils::unlist_list( res_coordinates ),
      //Rcpp::_["start_indices"] = interleave::utils::unlist_list( res_start_indices ),
      Rcpp::_["input_index"] = input_indices,  // we don't even need to return these?? because we're sorting out the shuffling in the function
      // Rcpp::_["geometry_index"] = interleave::utils::unlist_list( res_original_geometry_index ),
      // Rcpp::_["n_coordinates"] = n_coordinates,
      Rcpp::_["properties"] = properties,
      Rcpp::_["stride"] = stride
    );
  }

  inline SEXP interleave_triangle(
      SEXP& obj
  ) {
    Rcpp::List lst;
    return interleave_triangle( obj, lst );
  }


} // primitives
} // interleave

#endif
