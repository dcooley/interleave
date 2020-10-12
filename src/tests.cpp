#include <Rcpp.h>
using namespace Rcpp;

#include "interleave/utils/list.hpp"

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

