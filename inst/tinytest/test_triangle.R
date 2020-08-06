
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
expect_error( interleave:::rcpp_interleave_triangle( m ), "interleave - expecting a list" )

## single polygon
## This won't work for trinagulating, because tringulating needs a collection,
## not a single (sfg) geometry.
## for a single geometrh you would use earcut()
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
l <- list( m )
expect_silent( interleave:::rcpp_earcut( l ) )
expect_error( interleave:::rcpp_interleave_triangle( l ), "interleave - a list must only contain matrices")

## single multipolygon is treated as two POLYGONs
m1 <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
h <- matrix(c(0.25,0.25,0.25,0.75,0.75,0.75,0.75,0.25,0.25,0.25), ncol = 2, byrow = T)
m2 <- matrix(c(2,2,2,3,3,3,3,2,2,2), ncol = 2, byrow = T)
l <- list( list( m1, h ), list( m2 ) )
expect_silent( interleave:::rcpp_interleave_triangle( l ) )
## but it won't work for earcutting
expect_error( interleave:::rcpp_earcut( l ), "interleave - a list must only contain matrices" )

## testing what 'geometries::dimensions' give
## single polygon
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
l <- list( m )
dim <- geometries:::rcpp_geometry_dimensions( l )
expect_true( nrow( dim$dimensions ) == 1 )


## Two polygons
m1 <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
m2 <- matrix(c(0.25,0.25,0.25,0.75,0.75,0.75,0.75,0.25,0.25,0.25), ncol = 2, byrow = T)
l <- list( list( m1 ), list( m2 ) )
dim <- geometries:::rcpp_geometry_dimensions( l )
expect_true( nrow( dim$dimensions ) == 2 )

## polygon with hole
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
h <- matrix(c(0.25,0.25,0.25,0.75,0.75,0.75,0.75,0.25,0.25,0.25), ncol = 2, byrow = T)
l <- list( m, h )
dim <- geometries:::rcpp_geometry_dimensions( l )
expect_true( nrow( dim$dimensions ) == 2 )

## Two polygons, one with a hole
m1 <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
h <- matrix(c(0.25,0.25,0.25,0.75,0.75,0.75,0.75,0.25,0.25,0.25), ncol = 2, byrow = T)
m2 <- matrix(c(2,2,2,3,3,3,3,2,2,2), ncol = 2, byrow = T)
l <- list( list( m, h ), list( m2 ) )
dim <- geometries:::rcpp_geometry_dimensions( l )
expect_true( nrow( dim$dimensions ) == 2 ) ## because there are two separate objects



## polygon with hole
## - single 'sfg' type should error
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
h <- matrix(c(0.25,0.25,0.25,0.75,0.75,0.75,0.75,0.25,0.25,0.25), ncol = 2, byrow = T)
l <- list( m, h )
expect_error( interleave:::rcpp_interleave_triangle( l ), "interleave - a list must only contain matrices" )
## TODO: ^^ possibly change this error by checking for the internal object type inside each
## list element of the obj (in rcpp_interleave_trinagel)
##
## Nest it inside an 'sfc'
l <- list( list( m, h )  )
res <- interleave:::rcpp_interleave_triangle( l )
ec <- interleave:::rcpp_earcut( l[[1]] )
expect_equal( res$coordinates, ec$coordinates )


