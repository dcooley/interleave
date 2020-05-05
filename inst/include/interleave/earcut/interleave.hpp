#ifndef R_INTERLEAVE_EARCUT_H
#define R_INTERLEAVE_EARCUT_H

#include <RcppCommon.h>
#include <array>

namespace Rcpp {

template < typename T, std::size_t S > SEXP wrap( std::array< T, S >& Point );
template < typename T, std::size_t S > SEXP wrap( std::vector< std::array< T, S > >& Polygon );
template < typename T, std::size_t S > SEXP wrap( std::vector< std::vector< std::array< T, S > > >& Polygons );

namespace traits {

template < typename T, std::size_t S > class Exporter< std::array< T, S > >;
template < typename T, std::size_t S > class Exporter< std::vector< std::array< T, S > > >;
template < typename T, std::size_t S > class Exporter< std::vector< std::vector< std::array< T, S > > > >;

} // traits
} // Rcpp

#include <Rcpp.h>

namespace Rcpp {

template <typename T, std::size_t S >
SEXP wrap( std::array< T, S >& Point ) {
  Rcpp::NumericVector v( S );
  std::size_t i;
  for( i = 0; i < S; ++i ) {
    v[i] = Point[i];
  }
  return v;
}

template < typename T, std::size_t S >
SEXP wrap( std::vector< std::array< T, S > >& Polygon ) {
  R_xlen_t n = Polygon.size();
  Rcpp::NumericMatrix mat( n, S );
  R_xlen_t i;
  for( i = 0; i < n; ++i ) {
    std::array< T, S > pt =  Polygon[ i ];
    Rcpp::NumericVector nv = Rcpp::wrap( pt );
    mat( i, Rcpp::_ ) = nv;
  }
  return mat;
}

template < typename T, std::size_t S >
SEXP wrap( std::vector< std::vector< std::array< T, S > > >& Polygons ) {
  R_xlen_t n = Polygons.size();
  Rcpp::List lst( n );
  R_xlen_t i;
  for( i = 0; i < n; ++i ) {
    std::vector< std::array< T, S > > polygon = Polygons[ i ];
    lst[ i ] = Rcpp::wrap( polygon );
  }
  return lst;
}

namespace traits {

template < typename T, std::size_t S >
class Exporter< std::array< T, S > > {
  typedef typename std::array< T, S > Point;

  const static int RTYPE = Rcpp::traits::r_sexptype_traits< T >::rtype;
  Rcpp::Vector< RTYPE > vec;

public:
  Exporter( SEXP x ) : vec( x ) {
    if( TYPEOF( x ) != RTYPE ) {
      throw std::invalid_argument("interleave - invalid R object for creating a Point");
    }
  }

  Point get() {
    if( vec.length() != S ) {
      Rcpp::stop("interleave - each point in the polygon must have the correct size");
    }
    //Point x({ vec[0], vec[1] });
    Point x(S);
    std::size_t i;
    for( i = 0; i < S; ++i ) {
      x[i] = vec[i];
    }
    return x;
  }

};

template < typename T, std::size_t S >
class Exporter< std::vector< std::array< T, S > > > {
  typedef typename std::vector< std::array< T, S > > Polygon;

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
      x[i] = Rcpp::as< std::array< T, S > >( v );
    }
    return x;
  }
};

template< typename T, std::size_t S >
class Exporter< std::vector< std::vector< std::array< T, S > > > > {
  typedef typename std::vector< std::vector< std::array< T, S > > > Polygons;

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
      x[i] = Rcpp::as< std::vector< std::array< T, S > > >( mat );
    }
    return x;
  }
};

} // traits
} // Rcpp

#include "interleave/earcut/earcut.hpp"

using Coord = double;

template< size_t S>
using Point = std::array< Coord, S >;

template< typename Point >
using Polygon = std::vector< Point >;

template< typename Polygon >
using Polygons = std::vector< Polygon >;

namespace interleave {
namespace earcut {

inline Rcpp::NumericVector earcut(
    SEXP& polygon
) {

  //size_t stride = 2;
  Polygons< size_t > polyrings = Rcpp::as< Polygons< std::size_t > >( polygon );
  std::vector< double> coords = ::earcut::earcut< uint32_t >( polyrings );
  return Rcpp::NumericVector::create();
  // std::vector< double > coords = ::earcut::earcut< uint32_t >( polyrings );
  // return Rcpp::wrap( coords );
}

} // earcut
} // interleave





#endif
