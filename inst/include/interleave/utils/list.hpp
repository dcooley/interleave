#ifndef R_INTERLEAVE_UTILS_LIST_H
#define R_INTERLEAVE_UTILS_LIST_H

// adapted from colourvalues & sfheaders

#include <Rcpp.h>

#include "geometries/utils/sexp/sexp.hpp"

namespace interleave {
namespace utils {

  inline void validate_list( SEXP& obj ) {
    if( Rf_inherits( obj, "data.frame" ) ) {
      Rcpp::stop("interleave - expecting a list input");
    }

    if( !Rf_isNewList( obj ) ) {
      Rcpp::stop("interleave - expecting a list input");
    }
  }

  inline int vector_type( int new_type, int existing_type ) {

    // can't change from STRSXP
    if( existing_type == 16 ) {
      return existing_type;
    }

    std::vector< int > valid_types{10, 13, 14, 16};
    bool new_is_valid = ( std::find( valid_types.begin(), valid_types.end(), new_type ) != valid_types.end() );
    bool existing_is_valid = ( std::find( valid_types.begin(), valid_types.end(), existing_type ) != valid_types.end() );

    if( new_type == existing_type && new_is_valid ) {
      return existing_type;
    }

    // convert new type up to existing type
    if( new_type < existing_type && existing_is_valid ) {
      return existing_type;
    }

    if( new_type > existing_type && new_is_valid ) {
      return new_type;
    }

    if( new_type > existing_type && !new_is_valid ) {
      return 16;
    }

    if( existing_is_valid ) {
      return existing_type;
    }

    return 16;
  }


  /*
   * list rows
   *
   * Counts the number of rows in each list element
   * The output list is the same size as the input, where all elements are the
   * count of number of rows
   */
  inline Rcpp::List list_rows(
      SEXP obj,
      R_xlen_t& total_size
  ) {

    validate_list( obj );
    Rcpp::List lst = Rcpp::as < Rcpp::List > ( obj );

    R_xlen_t n = lst.size();
    Rcpp::List res( n ); // create a list to store the size corresponding to each list element
    R_xlen_t i;
    for( i = 0; i < n; i++ ) {
      SEXP inner_obj = lst[ i ];
      switch( TYPEOF( inner_obj ) ) {
        case VECSXP: {
          if( Rf_isNewList( inner_obj ) && !Rf_inherits( inner_obj, "data.frame" ) ) {
            res[ i ] = list_rows( inner_obj, total_size );
            break; // break is inside the 'if', because a data.frame needs to fall through to 'default'
          }
        }
        default: {
          R_xlen_t n_rows = geometries::utils::sexp_n_row( inner_obj );  // sexp_n_row is non-recursive
          res[ i ] = n_rows;
          total_size += n_rows;
        }
      }
    }
    return res;
  }

  /*
   * list element count
   *
   * Returns a list containing the number of values inside each list element
   *
   */
  inline Rcpp::List list_element_count(
      SEXP obj,
      R_xlen_t& total_size,
      int& existing_type
  ) {

    validate_list( obj );
    Rcpp::List lst = Rcpp::as < Rcpp::List > ( obj );

    R_xlen_t n = lst.size();
    Rcpp::List res( n ); // create a list to store the size corresponding to each list element
    R_xlen_t i;
    for( i = 0; i < n; i++ ) {
      SEXP inner_obj = lst[ i ];
      switch( TYPEOF( inner_obj ) ) {
        case VECSXP: {
          if( Rf_isNewList( inner_obj ) && !Rf_inherits( inner_obj, "data.frame" ) ) {
            res[ i ] = list_element_count( inner_obj, total_size, existing_type );
            break;
          }
        }
        default: {
          R_xlen_t n_elements = geometries::utils::sexp_n_row( inner_obj ) * geometries::utils::sexp_n_col( inner_obj );
          int new_type = TYPEOF( inner_obj );
          existing_type = vector_type( new_type, existing_type );
          res[ i ] = n_elements;
          total_size += n_elements;
        }
      }
    }
    return res;
  }

  // collapse a vector into a list
  // where line_ids gives the start and end indexes of v to use
  template < int RTYPE >
  inline Rcpp::List fill_list(
      Rcpp::Vector< RTYPE >& v,
      Rcpp::IntegerMatrix& line_positions
  ) {
    R_xlen_t n = line_positions.nrow();  // nrow should also be the row of the final sf object we are creating
    Rcpp::List res( n );
    R_xlen_t i;

    for( i = 0; i < n; ++i ) {
      R_xlen_t start = line_positions(i, 0);
      R_xlen_t end = line_positions(i, 1);
      Rcpp::IntegerVector elements = Rcpp::seq( start, end );
      res[ i ] = v[ elements ];
    }
    return res;
  }

  // TODO - handle dates and factors??
  inline Rcpp::List fill_list(
      SEXP& v,
      Rcpp::IntegerMatrix& line_positions
  ) {
    switch( TYPEOF( v ) ) {
    case LGLSXP: {
      Rcpp::LogicalVector lv = Rcpp::as< Rcpp::LogicalVector >( v );
      return fill_list( lv, line_positions );
    }
    case INTSXP: {
      Rcpp::IntegerVector iv = Rcpp::as< Rcpp::IntegerVector >( v );
      return fill_list( iv, line_positions );
    }
    case REALSXP: {
      Rcpp::NumericVector nv = Rcpp::as< Rcpp::NumericVector >( v );
      return fill_list( nv, line_positions );
    }
    case STRSXP: {
      Rcpp::StringVector sv = Rcpp::as< Rcpp::StringVector >( v );
      return fill_list( sv, line_positions );
    }
    default: {
      Rcpp::stop("interleave - unknown column type");
    }
    }
    return Rcpp::List::create(); // #nocov
  }


  template< int RTYPE >
  inline void unlist_list(
      SEXP obj,
      const Rcpp::List& lst_sizes,
      Rcpp::Vector< RTYPE >& values,
      int& list_position
  ) {

    validate_list( obj );
    Rcpp::List lst = Rcpp::as< Rcpp::List >( obj );

    // - iterate through original list
    // - extract each element and insert into 'values'
    R_xlen_t n = lst.size();
    Rcpp::List res( n );
    R_xlen_t i;
    for( i = 0; i < n; ++i ) {
      switch( TYPEOF( lst[ i ] ) ) {
      case VECSXP: {
        unlist_list< RTYPE >( lst[ i ], lst_sizes[ i ], values, list_position );
        break;
      }
      default: {
        Rcpp::IntegerVector n_elements = Rcpp::as< Rcpp::IntegerVector >( lst_sizes[ i ] );
        if( n_elements[0] == 0 ) { break; }
        int end_position = list_position + n_elements[0] - 1;

        Rcpp::IntegerVector elements = Rcpp::seq( list_position, end_position );
        values[ elements ] = Rcpp::as< Rcpp::Vector< RTYPE > >( lst[ i ] );

        list_position = end_position + 1;
        break;
      }
      }
    }
  }

  /*
   * Unlist list
   *
   * Converts a list where each element is a vector, into a single vector.
   * Currently only supports lists of vectors (including nested list / vectors )
   * (matrices get coerced to vectors)
   */
  inline SEXP unlist_list( SEXP obj ) {

    validate_list( obj );
    Rcpp::List lst = Rcpp::as< Rcpp::List >( obj );

    R_xlen_t total_size = 0;
    int existing_type = 10;
    int position = 0;
    Rcpp::List lst_sizes = list_element_count( lst, total_size, existing_type );

    switch( existing_type ) {
      case LGLSXP: {
        Rcpp::Vector< LGLSXP > v( total_size );
        unlist_list< LGLSXP >( lst, lst_sizes, v, position );
        return v;
      }
      case INTSXP: {
        Rcpp::Vector< INTSXP > v( total_size );
        unlist_list< INTSXP >( lst, lst_sizes, v, position );
        return v;
      }
      case REALSXP: {
        Rcpp::Vector< REALSXP > v( total_size );
        unlist_list< REALSXP >( lst, lst_sizes, v, position );
        return v;
      }
      case VECSXP: {
        Rcpp::stop("interleave - data.frames currently not supported");
      }
      default: {
        Rcpp::Vector< STRSXP > v( total_size );
        unlist_list< STRSXP >( lst, lst_sizes, v, position );
        return v;
      }
    }

    Rcpp::stop("interleave - couldn't unlist this object");
    return lst; // #nocov - never reaches

  }

} // utils
} // interleave

#endif

