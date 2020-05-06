#ifndef R_INTERLEAVE_EARCUT_H
#define R_INTERLEAVE_EARCUT_H

#include <RcppCommon.h>
#include <array>

namespace Rcpp {

// template < typename T > SEXP wrap( const std::vector< T >& Point );
template < typename T > SEXP wrap( const std::vector< std::vector< T > >& Polygon );
template < typename T > SEXP wrap( const std::vector< std::vector< std::vector< T > > >& Polygons );

namespace traits {

// template < typename T > class Exporter< std::vector< T > >;
template < typename T > class Exporter< std::vector< std::vector< T > > >;
template < typename T > class Exporter< std::vector< std::vector< std::vector< T > > > >;

} // traits
} // Rcpp

#include <Rcpp.h>

namespace Rcpp {

// template <typename T >
// SEXP wrap( const std::vector< T >& Point ) {
//   R_xlen_t n = Point.size();
//   Rcpp::NumericVector v( n );
//   std::size_t i;
//   for( i = 0; i < n; ++i ) {
//     v[i] = Point[i];
//   }
//   return v;
// }

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

// template < typename T >
// class Exporter< std::vector< T > > {
//   typedef typename std::vector< T > Point;
//
//   const static int RTYPE = Rcpp::traits::r_sexptype_traits< T >::rtype;
//   Rcpp::Vector< RTYPE > vec;
//
// public:
//   Exporter( SEXP x ) : vec( x ) {
//     if( TYPEOF( x ) != RTYPE ) {
//       throw std::invalid_argument("interleave - invalid R object for creating a Point");
//     }
//   }
//
//   Point get() {
//     R_xlen_t n = vec.size();
//     // Rcpp::Rcout << "S: " << S << std::endl;
//     // if( vec.length() != S ) {
//     //   Rcpp::stop("interleave - each point in the polygon must have the correct size");
//     // }
//     //Point x({ vec[0], vec[1] });
//     Point x(n);
//     std::size_t i;
//     for( i = 0; i < n; ++i ) {
//       x[i] = vec[i];
//     }
//     return x;
//   }
//
// };

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
    Rcpp::Rcout << "nrow: " << n_row << std::endl;
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

// using Coord = double;
//
// template< size_t S>
// using Point = std::array< Coord, S >;
//
// template< typename Point >
// using Polygon = std::vector< Point >;
//
// template< typename Polygon >
// using Polygons = std::vector< Polygon >;

namespace interleave {

namespace earcut {

inline Rcpp::NumericVector earcut(
    Rcpp::List& polygon
) {

  using Point = std::vector< double >;
  using Polygon = std::vector< Point >;
  using Polygons = std::vector< Polygon >;

  Polygons polyrings = Rcpp::as< Polygons >( polygon );
  std::vector< double > coords = ::earcut::earcut< uint32_t >( polyrings );
  return Rcpp::wrap( coords );
  return Rcpp::NumericVector::create();
}

} // earcut
} // interleave





#endif
