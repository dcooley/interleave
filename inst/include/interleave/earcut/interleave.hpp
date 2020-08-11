#ifndef R_INTERLEAVE_EARCUT_H
#define R_INTERLEAVE_EARCUT_H

#include <RcppCommon.h>
#include <array>

namespace Rcpp {

template < typename T > SEXP wrap( const std::vector< std::vector< T > >& Polygon );
template < typename T > SEXP wrap( const std::vector< std::vector< std::vector< T > > >& Polygons );

namespace traits {

template < typename T > class Exporter< std::vector< std::vector< T > > >;
template < typename T > class Exporter< std::vector< std::vector< std::vector< T > > > >;

} // traits
} // Rcpp

#include <Rcpp.h>

namespace Rcpp {

template < typename T >
SEXP wrap( const std::vector< std::vector< T > >& Polygon ) {
  R_xlen_t n = Polygon.size();
  R_xlen_t col = Polygon[0].size();
  Rcpp::NumericMatrix mat( n, col );
  R_xlen_t i;
  for( i = 0; i < n; ++i ) {
    std::vector< T > pt =  Polygon[ i ];
    Rcpp::NumericVector nv = Rcpp::wrap( pt );
    mat( i, Rcpp::_ ) = nv;
  }
  return mat;
}

template < typename T >
SEXP wrap( const std::vector< std::vector< std::vector< T > > >& Polygons ) {
  R_xlen_t n = Polygons.size();
  Rcpp::List lst( n );
  R_xlen_t i;
  for( i = 0; i < n; ++i ) {
    std::vector< std::vector< T > > polygon = Polygons[ i ];
    lst[ i ] = Rcpp::wrap( polygon );
  }
  return lst;
}

namespace traits {

template < typename T >
class Exporter< std::vector< std::vector< T > > > {
  typedef typename std::vector< std::vector< T > > Polygon;

  const static int RTYPE = Rcpp::traits::r_sexptype_traits< T >::rtype;
  Rcpp::Matrix< RTYPE > mat;

public:
  Exporter( SEXP x ) : mat( x ) {
    if( TYPEOF( x ) != RTYPE ) {
      throw std::invalid_argument("interleave - invalid R object for creating a Polygon");
    }
  }

  Polygon get() {
    R_xlen_t n_row = mat.nrow();
    Polygon x( n_row );
    R_xlen_t i;
    for( i = 0; i < n_row; ++i ) {
      Rcpp::Vector< RTYPE > v = mat( i, Rcpp::_);
      x[i] = Rcpp::as< std::vector< T > >( v );
    }
    return x;
  }
};

template< typename T >
class Exporter< std::vector< std::vector< std::vector< T > > > > {
  typedef typename std::vector< std::vector< std::vector< T > > > Polygons;

  const static int RTYPE = Rcpp::traits::r_sexptype_traits< T >::rtype;
  Rcpp::List lst;

public:
  Exporter( SEXP x ) : lst( x ) { }
  Polygons get() {
    R_xlen_t n = lst.size();
    Polygons x( n );
    R_xlen_t i;
    for( i = 0; i < n; ++i ) {
      if( !Rf_isMatrix( lst[ i ] ) ) {
        Rcpp::stop("interleave - a list must only contain matrices");
      }
      Rcpp::Matrix< RTYPE > mat = lst[ i ];
      x[i] = Rcpp::as< std::vector< std::vector< T > > >( mat );
    }
    return x;
  }
};

} // traits
} // Rcpp

#include "interleave/earcut/earcut.hpp"

namespace interleave {
namespace earcut {

  inline SEXP earcut(
      Rcpp::List& polygon
  ) {

    // triangluated interleaved gives a
    using Point = std::vector< double >;
    using Polygon = std::vector< Point >;
    using Polygons = std::vector< Polygon >;
    Polygons polyrings = Rcpp::as< Polygons >( polygon );

    return ::earcut::earcut< uint32_t >( polyrings );
  }

} // earcut
} // interleave





#endif
