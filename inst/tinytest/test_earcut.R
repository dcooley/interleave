
## single polygon
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
l <- list( m )
ec <- interleave:::rcpp_earcut( l )
## Testing this is the same as supplying two triangles as LINEs
## expected:
t1 <- matrix(c(0,1,0,0,1,0), ncol = 2, byrow = T)
t2 <- matrix(c(1,0,1,1,0,1), ncol = 2, byrow = T)
tr <- interleave:::rcpp_interleave_line( list(t1, t2) )
expect_equal( ec$coordinates, tr$coordinates )
expect_equal( tr$start_indices, c(0,3))

### how does earcut work with M(Z) dimensions?
m <- matrix(c(0,0,1,0,1,1,1,1,3,1,0,4,0,0,1), ncol = 3, byrow = T)
l <- list( m )
ec <- interleave:::rcpp_earcut( l )
## the 'Z' coordinate is kept as-is on each coordinate

## what does earcutting triangles do?
m <- matrix(c(0,0,1,0,1,1,1,1,3,1,0,4,0,0,1), ncol = 3, byrow = T)
l <- list( m )
ec <- interleave:::rcpp_earcut( l )
tris <- matrix( ec$coordinates, ncol = 3, byrow = T)

interleave:::rcpp_earcut( list( tris ) )
ec
