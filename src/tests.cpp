#include <Rcpp.h>
using namespace Rcpp;

#include "interleave/interleave.hpp"
#include "interleave/primitives/primitives.hpp"

// [[Rcpp::export(.test_interleave)]]
SEXP test_interleave( SEXP obj ) {
  return interleave::interleave( obj );
}

// [[Rcpp::export(.test_list_rows)]]
SEXP test_list_rows( SEXP obj ) {
  R_xlen_t total_size = 0;
  return interleave::utils::list_rows( obj, total_size );
}

// [[Rcpp::export(.test_list_element_count)]]
SEXP test_list_element_count( SEXP obj ) {
  R_xlen_t total_size = 0;
  int existing_type = 10;  // start with LGLSXP
  return interleave::utils::list_element_count( obj, total_size, existing_type );
}

// [[Rcpp::export(.test_unlist_list)]]
SEXP test_unlist_list( SEXP obj ) {
  return interleave::utils::unlist_list( obj );
}

// [[Rcpp::export(.test_interleave_primitive)]]
SEXP test_interleave_primitive( SEXP obj, int primitive_type ) {
  return interleave::primitives::interleave_primitive( obj, primitive_type );
}

// [[Rcpp::export(.test_interleave_triangle)]]
SEXP test_interleave_triangle( SEXP obj, Rcpp::List properties ) {
  return interleave::primitives::interleave_triangle( obj, properties );
}

// [[Rcpp::export(.test_subset_vector)]]
SEXP test_subset_vector( SEXP v, Rcpp::IntegerVector indices ) {
  return interleave::primitives::subset_vector( v, indices );
}
