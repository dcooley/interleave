

## Test that interleaving triangles only accept list( list(matrix) ) inputs
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
expect_error( interleave:::rcpp_interleave_triangle( m, list() ), "interleave - expecting a list" )

## single polygon
## This won't work for triangulating because it needs a collection,
## not a single (sfg) geometry.
## for a single geometry you would use earcut()
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
l <- list( m )
expect_silent( interleave:::rcpp_earcut( l ) )
expect_error( interleave:::rcpp_interleave_triangle( l, list() ), "interleave - a list must only contain matrices")


## single multipolygon is treated as two POLYGONs
m1 <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
h <- matrix(c(0.25,0.25,0.25,0.75,0.75,0.75,0.75,0.25,0.25,0.25), ncol = 2, byrow = T)
m2 <- matrix(c(2,2,2,3,3,3,3,2,2,2), ncol = 2, byrow = T)
l <- list( list( m1, h ), list( m2 ) )
expect_silent( interleave:::rcpp_interleave_triangle( l, list() ) )
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
expect_error( interleave:::rcpp_interleave_triangle( l, list() ), "interleave - a list must only contain matrices" )
## TODO: ^^ possibly change this error by checking for the internal object type inside each
## list element of the obj (in rcpp_interleave_trinagle)
##
## Nest it inside an 'sfc'
l <- list( list( m, h )  )
res <- interleave:::rcpp_interleave_triangle( l, list() )
ec <- interleave:::rcpp_earcut( l[[1]] )
expect_equal( res$coordinates, ec$coordinates )


## Testing Z(M) coordinates remain on the correct triangle
m <- matrix(c(0,0,1,0,1,2,1,1,3,1,0,4,0,0,1), ncol = 3, byrow = T)
sf <- sfheaders::sf_polygon(m)

res <- interleave:::rcpp_interleave_triangle( sf$geometry, list() )
res_mat <- matrix( res$coordinates, ncol = 3, byrow = T)

expect_equal( m[res$input_index + 1, ], res_mat )


## Testing on a non-closed matrix
m <- matrix(c(0,0,1,0,1,2,1,1,3,1,0,4), ncol = 3, byrow = T)
sf <- sfheaders::sf_polygon(m)
res1 <- interleave:::rcpp_interleave_triangle( sf$geometry, list() )

m <- matrix(c(0,0,1,0,1,2,1,1,3,1,0,4,0,0,1), ncol = 3, byrow = T)
sf <- sfheaders::sf_polygon(m)
res2 <- interleave:::rcpp_interleave_triangle( sf$geometry, list() )

expect_equal(res1, res2)

## Test the input index refers to the row of the geometry, not the separate rings (outer or hole)
m1 <- matrix(c(0,0,1,0,1,2,1,1,3,1,0,4,0,0,1), ncol = 3, byrow = T)
m2 <- matrix(c(0.5,0.5,0.5,0.5,0.75,0.5,0.75,0.75,0.5,0.75,0.5,0.5,0.5,0.5,0.5), ncol = 3, byrow = T)
mat <- rbind(m1, m2)

poly <- sfheaders::sfg_polygon(list(m1,m2))
res <- interleave:::rcpp_interleave_triangle( list(poly), list() )

res_mat <- matrix( res$coordinates, ncol = 3, byrow = T)

expect_equal( mat[ res$input_index + 1, ], res_mat )

## Testing 2-polygons have the correct input_index returned
## TODO: re-implement this test when 0.3.0 is on sfheaders
# m1 <- matrix(c(0,0,1,0,1,2,1,1,3,1,0,4,0,0,1), ncol = 3, byrow = T)
# m2 <- matrix(c(0.5,0.5,0.5,0.5,0.75,0.5,0.75,0.75,0.5,0.75,0.5,0.5,0.5,0.5,0.5), ncol = 3, byrow = T)
#
# ## This rquires sfheaders v0.3.0
# poly <- sfheaders:::rcpp_sfg_polygons( list(m1, m2), "XY", FALSE )
# res <- interleave:::rcpp_interleave_triangle( poly, list() )
#
# res_mat <- matrix( res$coordinates, ncol = 3, byrow = T )
# mat <- rbind(m1, m2)
# expect_equal( mat[ res$input_index + 1, ], res_mat )
#
#
# ## Testing 3-polygons have the correct input_index returned
# m1 <- matrix(c(0,0,1,0,1,2,1,1,3,1,0,4,0,0,1), ncol = 3, byrow = T)
# m2 <- matrix(c(0.5,0.5,0.5,0.5,0.75,0.5,0.75,0.75,0.5,0.75,0.5,0.5,0.5,0.5,0.5), ncol = 3, byrow = T)
# m3 <- matrix(c(2,2,1,2,3,1,3,3,1,3,2,1,2,2,1), ncol = 3, byrow = T)
#
# poly <- sfheaders:::rcpp_sfg_polygons( list(m1, m2, m3), "XY", FALSE )
# res <- interleave:::rcpp_interleave_triangle( poly, list() )
#
# res_mat <- matrix( res$coordinates, ncol = 3, byrow = T )
# mat <- rbind(m1, m2, m3)
# expect_equal( mat[ res$input_index + 1, ], res_mat )


## Testing properties are shuffled with indexes
m <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
p1 <- letters[1:5]
p2 <- letters[5:1]
p <- list( p1, p2)
l <- list( list( m ) )

res <- interleave:::rcpp_interleave_triangle( l, p )

expect_equal( p1[ res$input_index + 1 ], res$properties[[1]]  )
expect_equal( p2[ res$input_index + 1 ], res$properties[[2]]  )

## Testing a multi-row sf object with multi-row list-property columns are correctly shuffled
m1 <- matrix(c(0,0,0,1,1,1,1,0,0,0), ncol = 2, byrow = T)
m2 <- matrix(c(0,0,0,2,2,2,2,0,0,0), ncol = 2, byrow = T)
m1 <- cbind(1, m1)
m2 <- cbind(2, m2)
ls <- rbind(m1,m2)

sf <- sfheaders::sf_polygon(
  obj = ls
  , polygon_id = 1
)

p1 <- letters[1:5]
p2 <- letters[21:25]
p <- list( p1, p2)

sf$prop <- p
sf$prop2 <- list(p2, p1)

res <- interleave:::rcpp_interleave_triangle( sf$geometry, list( sf$prop ) )

## the number of properties must match the number of coordinates
expect_true(
  ( length(res$coordinates) / res$stride ) == length( res$properties[[1]] )
)

res <- interleave:::rcpp_interleave_triangle( sf$geometry, sf[, c("prop2"), drop = FALSE ] )
res <- interleave:::rcpp_interleave_triangle( sf$geometry, sf[, c("prop","prop2") ] )

expect_equal( c(p1,p2)[ res$input_index + 1 ], res$properties[[1]] )

## input object is not updated by-reference
expect_equal( p[[1]], letters[1:5] )
expect_equal( p[[2]], letters[21:25] )

## multiple SFGs with properties

df <- data.frame(
  id = c(1,1,1,1,1,2,2,2,2,2)
  , x = c(0,0,1,1,0, 2,2,3,3,2)
  , y = c(0,1,1,0,0, 2,3,3,2,2)
)

p1 <- letters[1:5]
p2 <- letters[5:1]

sf <- sfheaders::sf_polygon( df, polygon_id = "id")
sf$val <- list(p1,p2)


res <- interleave:::rcpp_interleave_triangle( sf$geometry, list( sf$val ) )
## properties need to be in a list to reflect the columns of a data.frame

expect_equal(
  res$properties[[1]]
  , c(p1,p2)[ res$input_index + 1 ]
)

## Testing if the properties have different lengths
df <- data.frame(
  id = c(1,1,1,1,1, 2,2,2,2,2)
  , x = c(0,0,1,1,0, 2,2,3,3,2)
  , y = c(0,1,1,0,0, 2,3,3,2,2)
)

p1 <- letters[1:4]
p2 <- letters[4:1]

sf <- sfheaders::sf_polygon( df, polygon_id = "id")
sf$val <- list(p1,p2)

expect_error( interleave:::rcpp_interleave_triangle( sf$geometry, list( sf$val ) ), "index error" )


df <- data.frame(
  id = c(1,1,1,1,1, 2,2,2,2,2)
  , x = c(0,0,1,1,0, 2,2,3,3,2)
  , y = c(0,1,1,0,0, 2,3,3,2,2)
)

sf <- sfheaders::sf_polygon( df, polygon_id = "id")
properties <- list(
  x = 1:4
  , y = 1:3
)

expect_error( interleave:::.test_interleave_triangle( sf$geometry, properties ), "index error" )

l <- list(
  matrix(1:16, ncol = 2)
  , matrix(1:16, ncol = 4)
)

## TODO - better handle of this error?
expect_error( interleave:::.test_interleave_triangle( list( l ), list() ), "upper value must be greater than lower value" )

## tringale with XYZ coords
df <- data.frame(
  x = c(0,0,1,1,0, 2,2,3,3,2)
  , y = c(0,1,1,0,0, 2,3,3,2,2)
  , z = 1:10
)


p1 <- letters[1:10]

m1 <- as.matrix( df )
m2 <- as.matrix( df[, c("x","y") ])

res1 <- interleave:::rcpp_interleave_triangle( list( list( m1 ) ) , list( p1 ) )
res2 <- interleave:::rcpp_interleave_triangle( list( list( m2 ) ) , list( p1 ) )

expect_true( ( length( res1$coordinates ) / res1$stride) == ( length( res2$coordinates ) / res2$stride )  )

## start indices shouldn't change
expect_equal( res1$start_indices, res2$start_indices )

## input indexes shouldn't change
expect_equal( res1$input_index, res2$input_index )

## properties shouldn't change
expect_equal( res1$properties, res2$properties )

## n-coordinates shouldn't change
expect_equal( res1$geometry_coordinates, res2$geometry_coordinates )

expect_true( res1$stride != res2$stride )
expect_true( res1$stride == res2$stride + 1)


## Testing interleave triangle and primitive return the same structure object

df <- data.frame(
  id = c(1,1,1,1,1, 2,2,2,2,2)
  , x = c(0,0,1,1,0, 2,2,3,3,2)
  , y = c(0,1,1,0,0, 2,3,3,2,2)
)

p1 <- letters[1:5]
p2 <- letters[5:1]

sf <- sfheaders::sf_polygon( df, polygon_id = "id")

res_tri <- interleave:::rcpp_interleave_triangle( sf$geometry, list() )
res_line <- interleave:::rcpp_interleave_line( sf$geometry )

expect_true( "coordinates" %in% names( res_line ) )
expect_true( "coordinates" %in% names( res_tri ) )
expect_true( "start_indices" %in% names( res_line ) )
expect_true( "start_indices" %in% names( res_tri ) )
expect_true( "geometry_coordinates" %in% names( res_line ) )
expect_true( "geometry_coordinates" %in% names( res_tri ) )
expect_true( "stride" %in% names( res_line ) )
expect_true( "stride" %in% names( res_tri ) )

expect_true( "total_coordinates" %in% names( res_line ) )
expect_true( !"total_coordinates" %in% names( res_tri ) )

expect_true( "input_index" %in% names( res_tri ) )
expect_true( "properties" %in% names( res_tri ) )

expect_true( !"input_index" %in% names( res_line ) )
expect_true( !"properties" %in% names( res_line ) )
