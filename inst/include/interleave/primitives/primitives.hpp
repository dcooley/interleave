#ifndef R_INTERLEAVE_PRIMITIVES_H
#define R_INTERLEAVE_PRIMITIVES_H

#include "interleave/earcut/interleave.hpp"
#include "interleave/interleave.hpp"

#include "geometries/utils/sexp/sexp.hpp"
#include "geometries/coordinates/dimensions.hpp"

namespace interleave {
namespace primitives {

  const int INTERLEAVE_POINT  = 1;
  const int INTERLEAVE_LINE   = 2;

  // Converts any sfc input into either interleved POINTs or LINEs
  //
  // for POINTs, each sfg will be interleaved, and the start_indices will
  // be a sequence from 1 to total-coordinates, by = stride
  //
  // for LINEs, each sfg will be interleaved, and the start indices will
  // be the number of coordinates in each input matrix
  //
  inline SEXP interleave_primitive( SEXP& obj, int primitive_type ) {
    if( !Rf_isNewList( obj ) ) {
      Rcpp::stop("interleave - expecting a list");
    }

    Rcpp::List lst = Rcpp::as< Rcpp::List >( obj );

    if( Rf_length( lst ) == 0 ) {
      Rcpp::stop("interleave - empty list");
    }

    R_xlen_t i;
    R_xlen_t total_size = 0;

    Rcpp::List dimension = geometries::coordinates::geometry_dimensions( obj );
    Rcpp::IntegerMatrix dims = dimension[ "dimensions" ];
    R_xlen_t n_geometries = dims.nrow();  // the number of sfg objects
    int stride = dimension[ "max_dimension" ];

    //TODO: validate the stride is the same for all geometries
    // Rcpp::IntegerVector strides = dims( Rcpp::_, 2 );
    //validate_stride( strides );

    // for storing the number of coordinates PER sfg
    // this becomes our 'repeats' vector
    Rcpp::IntegerVector geometry_coordinates( n_geometries );
    //geometry_coordinates[]
    for( i = 0; i < n_geometries; ++i ) {
      geometry_coordinates[ i ] = dims( i, 1 ) - dims( i, 0 ) + 1;
    }

    // TODO:
    // n_coordinates is actually the difference between the start_indices
    // so we don't need to calculate it twice?
    // but we do need to know how many 'ring's there are
    // so we can create the 'start_indices()' object
    Rcpp::List lst_sizes = interleave::utils::list_rows( lst, total_size );
    Rcpp::IntegerVector n_coordinates = interleave::utils::unlist_list( lst_sizes );
    R_xlen_t n = n_coordinates.length(); // the total number of coordinates

    R_xlen_t total_coordinates = Rcpp::sum( n_coordinates );

    // for POINT types, the start_indices is the sequence from 0 to n_coordinates
    Rcpp::IntegerVector start_indices;

    if( primitive_type == INTERLEAVE_POINT ) {

      start_indices = Rcpp::seq( 0, total_coordinates - 1 );

    } else if ( primitive_type == INTERLEAVE_LINE ) {
      Rcpp::IntegerVector si( n );
      si[0] = 0; // always starts at index 0
      for( i = 1; i < n; ++i ) {
        si[ i ] = si[ i - 1 ] + n_coordinates[ i - 1 ];
      }
      start_indices = si;
    } else {
      Rcpp::stop("interleave - unknown primitive type");
    }

    return Rcpp::List::create(
      Rcpp::_["coordinates"] = interleave::interleave( obj ),
      Rcpp::_["start_indices"] = start_indices,
      Rcpp::_["geometry_coordinates"] = geometry_coordinates,
      Rcpp::_["total_coordinates"] = total_coordinates,
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
      Rcpp::List list_properties
  ) {

    interleave::utils::validate_list( obj );

    Rcpp::List properties = Rcpp::clone( list_properties );

    Rcpp::List lst = Rcpp::as< Rcpp::List >( obj );

    bool shuffle_properties = properties.length() > 0;

    // I Need the index of the original coordinate so that
    // I can reference the correct row of data for each coordinate
    R_xlen_t total_coordinates = 0;
    R_xlen_t n = lst.length();
    R_xlen_t i, j;
    int stride;

    Rcpp::List res_coordinates( n );
    Rcpp::List res_indices( n );

    R_xlen_t n_properties = properties.length();
    Rcpp::List res_properties( n_properties );

    SEXP input_indices; // filled later if required
    Rcpp::IntegerVector property_indexes;

    // stores how many coordinates make up each geometry after earcutting
    // (i.e, the sum of all the triangle coordiantes)
    Rcpp::IntegerVector geometry_coordinates( n );

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

      int this_stride = static_cast< int >( lst_coords["stride"] );

      if( i == 0 ) {
        stride = this_stride;
      } else if ( stride != this_stride ) {
        Rcpp::stop("interleave - polygons have different strides");
      }

      R_xlen_t n_coords = iv.length(); // nv.length() / this_stride;

     // R_xlen_t n_indices = n_coords / 3; // because each triangle has 3 coordinates
      geometry_coordinates[ i ] = n_coords;

      res_coordinates[ i ] = nv;

      //if( shuffle_properties ) {
        // I'm making it always return 'input_indices', regardless if there are
        // properties or not

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

      res_indices[ i ] = origin_index;  // add the index of the position of the geometry
      //}

      total_coordinates = total_coordinates + n_coords;
    }


    input_indices = interleave::utils::unlist_list( res_indices ); // can't unlist_list this if it's NULL

    if( shuffle_properties ) {
      property_indexes = Rcpp::as< Rcpp::IntegerVector >( input_indices );
    }

    // Shuffling properties
    if( shuffle_properties ) {
      // we can unlist all the input properties, then use the `input_index` + `geometry_index`
      // to give the correct row
      // but how should it be returned?
      // as a 'properties' object?

      for( i = 0; i < n_properties; ++i ) {
        // each property will be a list of vectors
        Rcpp::List this_property = properties[ i ];
        SEXP p = interleave::utils::unlist_list( this_property );

        res_properties[ i ] = subset_vector( p, property_indexes );
      }

    }

    // start_indices: "the index in the coordinate vector where each geometry starts,
    // divided by the stride"
    // - and each geometry is 3 sets of coordinates
    // we get the start_indices after ear-cutting, as they are the starts of the triangles
    Rcpp::NumericVector coordinates = interleave::utils::unlist_list( res_coordinates );
    R_xlen_t n_start_indices = coordinates.length() / stride;
    Rcpp::IntegerVector start_indices( n_start_indices );
    for( i = 0; i < n_start_indices; ++i ) {
      start_indices[ i ] = i * 3;
    }

    return Rcpp::List::create(
      Rcpp::_["coordinates"] = coordinates,
      Rcpp::_["start_indices"] = start_indices,
      Rcpp::_["input_index"] = input_indices,
      Rcpp::_["geometry_coordinates"] = geometry_coordinates,
      Rcpp::_["properties"] = res_properties,
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
