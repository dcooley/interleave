#
# library(interleave)
# library(sf)
# library(geojsonsf)
#
# sf <- geojsonsf::geojson_sf("https://symbolixau.github.io/data/geojson/SA2_2016_VIC.json")
# # sf <- sf[1:16, ]
#
# sf <- sfheaders::sf_cast(sf, "POLYGON")
#
# res <- interleave:::rcpp_interleave_triangle(
#   sf$geometry, list()
# )
# str( res )
