
v <- 1:4
expect_error( interleave:::rcpp_interleave_line( v, 2 ), "interleave - expecting a list")


m <- matrix(1:4, ncol = 2)
expect_error( interleave:::rcpp_interleave_line( m, 2 ), "interleave - expecting a list")


## interleave_line() should only accept LIST inputs
## matrix (e.g. LINESTRING)
m <- matrix(1:16, ncol = 2, byrow = T)
l <- list( m )
res <- interleave:::rcpp_interleave_line( l, 2 )
expect_equal( res$coordinates, 1:16 )
expect_equal( res$start_indices, 0 ) ## Only one line
expect_equal( res$stride, 2 )
expect_equal( sum( res$total_coordinates ), sum(sapply(l, nrow)) )
expect_equal( res$n_coordinates, sapply(l, nrow) )

## Two linestrings (or a polygon)
l <- list( m, m )
res <- interleave:::rcpp_interleave_line( l, 2 )
expect_equal( res$coordinates, c(1:16, 1:16) )
expect_equal( res$start_indices, c(0,8) ) ## two lines
expect_equal( res$stride, 2 )
expect_equal( sum( res$total_coordinates ), sum(sapply(l, nrow)) )
expect_equal( res$n_coordinates, sapply(l, nrow) )

## A mixture of a line and a polygon
m1 <- matrix(1:4, ncol = 2, byrow = T)
m2 <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
l <- list( m1, m2 )
res <- interleave:::rcpp_interleave_line( l, 2 )
expect_equal( res$coordinates, c(1:4, 0,0,0,1,1,1,1,0,0,0))
expect_equal( res$start_indices, c(0, 2) )
expect_equal( sum( res$total_coordinates ), sum(sapply(l, nrow)) )
expect_equal( res$n_coordinates, sapply(l, nrow) )


### Every possible type of sfg, sfc or any nested set of matrices (or not nested)
### need to be interleaved

m1 <- matrix(1:6, ncol = 2, byrow = T)
m2 <- matrix(7:12, ncol = 2, byrow = T)
m3 <- matrix(13:18, ncol = 2, byrow = T)

l <- list(
  1:2 ## POINT
  , m1 ## LINESTRING
  , list( m1, m2 ) ## POLYGON / MULTILINESTRING
  , list( list( m1 ), list( m2, m3 ) ) ## MULTIPOLYGON
)

res <- interleave:::rcpp_interleave_line( l, 2 )

coords <- c(
  1:2
  , as.vector( t(m1) )
  , as.vector( t(m1) )
  , as.vector( t(m2) )
  , as.vector( t(m1) )
  , as.vector( t(m2) )
  , as.vector( t(m3) )
)

expect_equal( res$coordinates, coords )

## start_indices should be one for each matrix
expect_true( length( res$start_indices ) == 7)
expect_true( res$total_coordinates == 19 )
expect_equal( res$n_coordinates, c(1,3,3,3,3,3,3) )


###
# A list containing
# \itemize{
#   \item{dimensions - a 5-column matrix where \itemize{
#     \item{column 1 - the start-index of the geometry}
#     \item{column 2 - the end-index of the geometry}
#     \item{column 3 - the dimension of the geometry}
#     \item{column 4 - the nesting level of the geometry}
#     \item{column 5 - the SEXP type of the geometry}
#     }
#   }
#   \item{max_dimension - the maximum dimension in all the geometries}
#   \item{max_nest - the maximum nesting level in all the geometries}
# }
## Test the dimeions are correctly cacluated

## sfg POINT (vector) (1 'line')
v <- 1L:4L
dim <- geometries:::rcpp_geometry_dimensions( v )
expect_true( nrow( dim$dimensions ) == 1 )
expect_true( dim$max_dimension == 4 )
expect_true( dim$max_nest == 0 )


### sfc POINTs (2 'lines')
l <- list(1:4, 1:3)
dim <- geometries:::rcpp_geometry_dimensions( l )
expect_true( nrow( dim$dimensions ) == 2 )
expect_true( dim$max_dimension == 4 )
expect_true( dim$max_nest == 1 )

### sfg LINESTRING (matrix)
m <- matrix(1:6, ncol = 2)
dim <- geometries:::rcpp_geometry_dimensions( m )
expect_true( nrow( dim$dimensions ) == 1 )
expect_true( dim$max_dimension == 2 )
expect_true( dim$max_nest == 0 )

### sfg MULTILINESTRING
### sfg POLYGON
### sfc LINESTRINGs
l <- list( m )
dim <- geometries:::rcpp_geometry_dimensions( l )
expect_true( nrow( dim$dimensions ) == 1 )
expect_true( dim$max_dimension == 2 )
expect_true( dim$max_nest == 1 )

l <- list( m, m )
dim <- geometries:::rcpp_geometry_dimensions( l )
expect_true( nrow( dim$dimensions ) == 2 )
expect_true( dim$max_dimension == 2 )
expect_true( dim$max_nest == 1 )

### sfg MULTIPOLYGON
### sfc POLYGON
l <- list( list( m ) )
dim <- geometries:::rcpp_geometry_dimensions( l )
expect_true( nrow( dim$dimensions ) == 1 )
expect_true( dim$max_dimension == 2 )
expect_true( dim$max_nest == 2 )

