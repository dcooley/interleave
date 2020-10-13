
v <- 1:4
expect_error( interleave:::rcpp_interleave_line( v ), "interleave - expecting a list")


m <- matrix(1:4, ncol = 2)
expect_error( interleave:::rcpp_interleave_line( m ), "interleave - expecting a list")

expect_error( interleave:::rcpp_interleave_line( list() ), "interleave - empty list")

## interleave_line() should only accept LIST inputs
## matrix (e.g. LINESTRING)
m <- matrix(1:16, ncol = 2, byrow = T)
l <- list( m )
res <- interleave:::rcpp_interleave_line( l )
expect_equal( res$coordinates, 1:16 )
expect_equal( res$start_indices, 0 ) ## Only one line
expect_equal( res$stride, 2 )
expect_equal( sum( res$total_coordinates ), sum(sapply(l, nrow)) )
expect_equal( res$geometry_coordinates, sapply(l, nrow) )

## Two linestrings (or a polygon)
l <- list( m, m )
res <- interleave:::rcpp_interleave_line( l )
expect_equal( res$coordinates, c(1:16, 1:16) )
expect_equal( res$start_indices, c(0,8) ) ## two lines
expect_equal( res$stride, 2 )
expect_equal( sum( res$total_coordinates ), sum(sapply(l, nrow)) )
expect_equal( res$geometry_coordinates, sapply(l, nrow) )

## A mixture of a line and a polygon
m1 <- matrix(1:4, ncol = 2, byrow = T)
m2 <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
l <- list( m1, m2 )
res <- interleave:::rcpp_interleave_line( l )
expect_equal( res$coordinates, c(1:4, 0,0,0,1,1,1,1,0,0,0))
expect_equal( res$start_indices, c(0, 2) )
expect_equal( sum( res$total_coordinates ), sum(sapply(l, nrow)) )
expect_equal( res$geometry_coordinates, sapply(l, nrow) )


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

res <- interleave:::rcpp_interleave_line( l )

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
expect_equal( res$start_indices, c(0, 1, 4, 7, 10, 13, 16 ))
expect_true( res$total_coordinates == 19 )
expect_equal( res$geometry_coordinates, c(1,3,6,9) )


## Test all SF types return the same result
df <- data.frame(
  id1 = rep(1,10)
  , id2 = c( rep(1, 5), rep(2, 5) )
  , x = 1:10
  , y = 10:1
)

sf_pt <- sfheaders::sf_point(
  obj = df
  , x = "x"
  , y = "y"
)

sf_line <- sfheaders::sf_linestring(
  obj = df
  , x = "x"
  , y = "y"
  , linestring_id = "id1"
)

sf_polygon <- sfheaders::sf_polygon(
  obj = df
  , x = "x"
  , y = "y"
  , polygon_id = "id1"
  , close = FALSE
)

res_pt <- interleave:::rcpp_interleave_line( sf_pt$geometry )
res_ln <- interleave:::rcpp_interleave_line( sf_line$geometry )
res_poly <- interleave:::rcpp_interleave_line( sf_polygon$geometry )

expect_equal( res_pt$coordinates, res_ln$coordinates )
expect_equal( res_ln$coordinates, res_poly$coordinates )


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

