

## For points, each row of each matrix inside each list is its own POINT

v <- 1:2
res <- interleave:::rcpp_interleave_point(v, 2)
expect_equal( v, res$coordinates )
expect_equal( res$start_indices, 0 )
expect_equal( res$stride, 2 )

# v <- 1:2
# res <- interleave:::rcpp_interleave_point(v, 3) ## testing incorrect stride
# expect_equal( v, res$coordinates )
# expect_equal( res$start_indices, 0 )
# expect_equal( res$stride, 2 )

v <- 1:4
res <- interleave:::rcpp_interleave_point(v, 4)
expect_equal( v, res$coordinates )
expect_equal( res$start_indices, 0 )
expect_equal( res$stride, 4 )

## matrix (LINESTRING)
m <- matrix(1:4, ncol = 2, byrow = T)
res <- interleave:::rcpp_interleave_point(m, 2)
expect_equal( res$coordinates, 1:4 )
expect_equal( res$start_indices, c(0:1) )  ##start_indices - the index in the coordinates vector where each geometry starts, divided by the stride.
expect_equal( res$stride, 2 )


## matrix in a list (MULTILINESTRING)
m <- matrix(1:4, ncol = 2, byrow = T)
l <- list( m )
res <- interleave:::rcpp_interleave_point(l, 2)
expect_equal( res$coordinates, 1:4 )
expect_equal( res$start_indices, c(0:1) )
expect_equal( res$stride, 2 )

## 2 matrices in a list (MULTILINESTRING / POLYGON )
m1 <- matrix(1:4, ncol = 2, byrow = T)
m2 <- matrix(5:8, ncol = 2, byrow = T)
l <- list( m1, m2 )
res <- interleave:::rcpp_interleave_point(l, 2)
expect_equal( res$coordinates, 1:8 )
expect_equal( res$start_indices, c(0:3) )
expect_equal( res$stride, 2 )

## list of list of two matrices
l <- list( l )
res <- interleave:::rcpp_interleave_point(l, 2)
expect_equal( res$coordinates, 1:8 )
expect_equal( res$start_indices, c(0:3) )
expect_equal( res$stride, 2 )

