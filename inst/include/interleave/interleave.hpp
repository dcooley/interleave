#ifndef R_INTERLEAVE_H
#define R_INTERLEAVE_H

#include <Rcpp.h>
#include "interleave/utils/list.hpp"

namespace interleave {

template < int RTYPE >
inline SEXP interleave( Rcpp::Matrix< RTYPE >& mat ) {

  R_xlen_t nrow = mat.nrow(), ncol = mat.ncol();
  R_xlen_t len = nrow * ncol;
  R_xlen_t len2 = len - 1;
  Rcpp::Vector< RTYPE > res( len );
  R_xlen_t i, j;
  for( i = 0, j = 0; i < len; ++i, j += nrow ) {
    if (j > len2) j -= len2;
    res[i] = mat[j];
  }
  return res;
}

inline SEXP interleave( SEXP& sfg ) {

  switch( TYPEOF ( sfg ) ) {
  case INTSXP: {
    if( Rf_isMatrix( sfg ) ) {
    Rcpp::IntegerMatrix im = Rcpp::as< Rcpp::IntegerMatrix >( sfg );
    return interleave( im );
  } else {
    return sfg; // it's already a vector
  }
  }
  case REALSXP: {
    if( Rf_isMatrix( sfg ) ) {
    Rcpp::NumericMatrix im = Rcpp::as< Rcpp::NumericMatrix >( sfg );
    return interleave( im );
  } else {
    return sfg; // it's already a vector
  }
  }
  case VECSXP: {
    if( Rf_isNewList( sfg ) ) {
    Rcpp::List lst = Rcpp::as< Rcpp::List >( sfg );
    R_xlen_t n = lst.size();
    R_xlen_t i;
    Rcpp::List res( n ); // store interleaved vectors in the same nested-list structure

    for( i = 0; i < n; ++i ) {
      SEXP sfg = lst[ i ];
      res[ i ] = interleave( sfg );
    }

    return interleave::utils::unlist_list( res );
  }
  }
  default: {
    Rcpp::stop("interleave - can not interleave this type of object");
  }
  }
  return Rcpp::List::create();
}

} // interleave


#endif
