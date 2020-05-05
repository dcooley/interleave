#include "interleave/earcut/interleave.hpp"
#include <Rcpp.h>
#include "interleave/earcut/earcut.hpp"

// [[Rcpp::export]]
SEXP rcpp_earcut( SEXP& obj ) {
  return interleave::earcut::earcut( obj );
}
