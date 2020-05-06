#' Interleave
#'
#' @param x object to interleave
#'
#' @examples
#'
#' m1 <- matrix(1:20, ncol = 2, byrow = TRUE )
#' interleave( m1 )
#'
#' ## list of matrices
#' m2 <- matrix(20:1, ncol = 2, byrow = TRUE )
#' l <- list( m1, m2 )
#'
#' interleave( l )
#'
#' ## nested list of matrices
#' l <- list( m1, list( list( m2 ) ) )
#' interleave( l )
#'
#'
#' @export
interleave <- function( x ) rcpp_interleave( x )
