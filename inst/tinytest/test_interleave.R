
## interleave()

## matrix (e.g. a LINESTRING)
m1 <- matrix(1:20, ncol = 2, byrow = TRUE )
res <- interleave( m1 )
expect_equal(1:20, res)


## list of matrices (e.g. a POLYIGON)
m2 <- matrix(20:1, ncol = 2, byrow = TRUE )
l <- list( m1, m2 )
res <- interleave( l )
expect_equal(c(1:20,20:1) , res )

## nested list of matrices
l <- list( m1, list( list( m2 ) ) )
res <- interleave( l )
expect_equal(c(1:20,20:1) , res )
