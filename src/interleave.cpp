#include "interleave/earcut/interleave.hpp"
#include "interleave/primitives/primitives.hpp"

#include <Rcpp.h>

#include "interleave/earcut/earcut.hpp"
#include "interleave/interleave.hpp"




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
SEXP rcpp_interleave_point( SEXP& lst, int stride ) {
  return interleave::primitives::interleave_point( lst, stride );
}


// [[Rcpp::export]]
SEXP rcpp_interleave_line( SEXP& lst, int stride ) {
  return interleave::primitives::interleave_line( lst, stride );
}

// for each element inside obj (for triangles)
// need to cast to a POLYGON
// OR do I say interleave_triangle() will ONLY work on POLYGONs (list(mat,mat)) objects?




/*
 * interleave triangle
 *
 * Accepts a list of polygons
 * p1 <- list(mat1, hole1)
 * p2 <- list(mat2)
 * p3 <- list(mat3)
 * list( p1, p2, p3, ... )
 *
 * but NOT multipolygons, nor a single polygon (e.g. sfg_POLYGON)
 * it MUST be a collection of POLYGONS
 *
 * You can use sfheaders::sfc_cast() to convert inputs to POLYGONS
 *
 */
// [[Rcpp::export]]
SEXP rcpp_interleave_triangle( SEXP& obj ) {
  return interleave::primitives::interleave_triangle( obj );
}


// line
// the start indices are the coordinates of the first row of each matrix
// the stride is the width of the matrix
