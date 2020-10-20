#' Interleave
#'
#' Converts matrices and lists of matrices into a vector. The elements of the vector
#' are taken from the matrices one row at a time.
#'
#'
#' @param x object to interleave
#'
#' @examples
#'
#' ## matrix (this is equivalent to a LINESTRING in spatial structures)
#' m1 <- matrix(1:20, ncol = 2, byrow = TRUE )
#' interleave( m1 )
#'
#' ## This is the same as transposing and removing the 'dim' attribute
#' tm <- t(m1)
#' attr( tm, "dim" ) <- NULL
#' all( interleave( m1 ) == tm )
#'
#' ## list of matrices (this is equivalent to a POLYGON in spatial structures)
#' m2 <- matrix(20:1, ncol = 2, byrow = TRUE )
#' l <- list( m1, m2 )
#' interleave( l )
#'
#' ## nested list of matrices
#' l <- list( m1, list( list( m2 ) ) )
#' interleave( l )
#'
#' @return vector of interleaved values
#' @export
interleave <- function( x ) rcpp_interleave( x )
