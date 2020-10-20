
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

## Only numeric values can be interleaved
err <- "interleave - can not interleave this type of object"
expect_error( interleave:::.test_interleave( T ), err )
expect_error( interleave:::.test_interleave( raw() ), err )
expect_error( interleave:::.test_interleave( complex() ), err )

expect_equal( interleave:::.test_interleave( 1:4 ), 1:4 )
expect_equal( interleave:::.test_interleave( list(1:4) ), 1:4 )

## Test that data.frames are not supported
df <- data.frame(
  x = 1:5
  , y = 5:1
)
expect_error( interleave( df ), "interleave - data.frames are currently not supported")

