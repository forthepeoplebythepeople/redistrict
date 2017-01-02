# redistrict
Customizable, Automated Hands-Free Redistricting

This project aims to provide tools for drawing political districts and analysing the characteristics of the resulting maps.  But we're not really far along, so let's talk about what we have and what think we should need and how to code it up.

Build what we have using the GNU make flow:  make all

## What we have

I am using the 2010 US Census Gazetteer data (https://www.census.gov/geo/maps-data/data/gazetteer2010.html) for National County Subdivisions.  The zipped data is located here:  http://www2.census.gov/geo/docs/maps-data/data/gazetteer/Gaz_cousubs_national.zip

This data has geographic centroids for each country subdivision, but no adjacency data.  Thus, we have to determine what we believe is adjacent in order to generate contiguous regions.  Eventually we can take this from the TIGER political shapefile data (link forthcoming), but for now we are going to estimate.

The estimate I'm using involves generating a non-euclidean Gabriel Graph (https://en.wikipedia.org/wiki/Gabriel_graph) using the centroids.  The make_adjacency script takes in the gazetteer file and the State Code in question and generates a new file, similar to the gazetteer, which has adjacency data annotated into it.

## What we want

TBD.  I'm still thinking about heuristics, cost functions, pareto-optimality, and other fancy words...
