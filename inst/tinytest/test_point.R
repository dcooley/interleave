

## For points, each row of each matrix inside each list is its own POINT

v <- 1:2
expect_error( interleave:::rcpp_interleave_point(v), "interleave - expecting a list")

## matrix (LINESTRING)
m <- matrix(1:4, ncol = 2, byrow = T)
expect_error( interleave:::rcpp_interleave_point(m), "interleave - expecting a list")

m <- matrix(1:4, ncol = 2, byrow = T)
l <- list( m )
res <- interleave:::rcpp_interleave_point(l)
expect_equal( res$coordinates, 1:4 )
expect_equal( res$start_indices, c(0:1) )
expect_equal( res$stride, 2 )

## 2 matrices in a list (MULTILINESTRING / POLYGON )
m1 <- matrix(1:4, ncol = 2, byrow = T)
m2 <- matrix(5:8, ncol = 2, byrow = T)
l <- list( m1, m2 )
res <- interleave:::rcpp_interleave_point(l)
expect_equal( res$coordinates, 1:8 )
expect_equal( res$start_indices, c(0:3) )
expect_equal( res$stride, 2 )

## list of list of two matrices
l2 <- list( l )
res <- interleave:::rcpp_interleave_point(l)
expect_equal( res$coordinates, 1:8 )
expect_equal( res$start_indices, c(0:3) )
expect_equal( res$stride, 2 )

expect_equal(
  interleave:::rcpp_interleave_point( l )$coordinates
  , interleave:::rcpp_interleave_point( l2 )$coordinates
)

expect_equal(
  interleave:::rcpp_interleave_point( l )$start_indices
  , interleave:::rcpp_interleave_point( l2 )$start_indices
)

expect_equal(
  interleave:::rcpp_interleave_point( l )$total_coordinates
  , interleave:::rcpp_interleave_point( l2 )$total_coordinates
)

expect_equal(
  interleave:::rcpp_interleave_point( l )$stride
  , interleave:::rcpp_interleave_point( l2 )$stride
)

## Geometry coordiantes are the coordinates of the original geometries
## so they should be different
expect_false(
  all(
    interleave:::rcpp_interleave_point( l )$geometry_coordinates ==
      interleave:::rcpp_interleave_point( l2 )$geometry_coordinates
  )
)

## Matrices with un-equal strides
m1 <- matrix(1:4, ncol = 2, byrow = T)
m2 <- matrix(1:6, ncol = 3, byrow = T)
l <- list( m1, m2 )
res <- interleave:::rcpp_interleave_point(l)



