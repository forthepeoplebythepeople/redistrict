// make_adjacency.cpp
//

// The US Census data does not contain actual geospatial boundaries.  Let's
// read the data and infer adjacency by generating a gabriel graph based on the
// centroids of the communities.

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "structures.hpp"

typedef std::vector<redistrict::community> community_set_type;

bool read_data(const char*, const char*, community_set_type&);
void gen_gilbert_graph(community_set_type&);
void write_data(std::ostream&, const community_set_type&);

int main(int argc, char* argv[]) {
  if(argc != 3) {
    std::cerr << "Usage:  make_adjacency <datafile> <state>" << std::endl;
    return(1);
  }
  
  community_set_type communities;

  if(!read_data(argv[1], argv[2], communities)) {
    return(2);
  }

  gen_gilbert_graph(communities);
  
  write_data(std::cout, communities);
  return(0);
}


bool read_data(const char* filename, const char* state,
               community_set_type& communities) {
  std::ifstream in(filename);

  if(!in) {
    std::cerr << "Unable to read " << filename << std::endl;
    return(false);
  }

  std::string line;

  // Read and discard the first line.  It is a key.  We're going to cheat and
  // hard-code the correct fields.  Someone feel free to read this correctly.
  if(!std::getline(in, line)) {
    // An empty file isn't an error -- it's just empty
    return(true);
  }

  while(std::getline(in, line)) {
    // The TAB-delimited fields we want are:
    //  1 - GEOID
    //  3 - NAME
    //  5 - POP10
    //  11 - INTPTLAT
    //  12 - INTPTLON

    size_t fieldId = 0;
    size_t startIdx = 0;
    bool complete = false;
    redistrict::geo_coord::value_type latCache = 0.0;
    
    while(startIdx < line.size()) {
      size_t endIdx = line.find('\t', startIdx);
      
      switch(fieldId) {
      case 0:   // USPS
        if(line.compare(startIdx, endIdx - startIdx, state) != 0) {
          endIdx = std::string::npos;  // Not going to use this state
        }
        break;
        
      case 1:   // GEOID
        communities.push_back(redistrict::community());
        communities.back().id = std::atoll(line.c_str() + startIdx);
        break;
        
      case 3: // NAME
        communities.back().name.assign(line, startIdx, endIdx - startIdx);
        break;
        
      case 5: // POP10
        communities.back().population = std::atoll(line.c_str() + startIdx);
        break;
        
      case 11: // INTPTLAT
        latCache = std::atof(line.c_str() + startIdx);
        break;
        
      case 12: // INTPTLON
        communities.back().centroid.set(latCache, std::atof(line.c_str()
                                                            + startIdx));
        complete = true;
        break;
        
      default:
        ; // Nothing else happens
        
      }
      
      if(endIdx == std::string::npos) { break; }
      ++fieldId;
      startIdx = endIdx + 1;
    }

    if(fieldId != 0 && complete == false) {
      std::cerr << "Found and skipping an incomplete line.  It looks like:\n  "
                << line << std::endl;
    }
  }

  return(true);
}

void gen_gilbert_graph(community_set_type& communities) {
  // This is far from optimal at O(n**3).  I think there is an O(n log n)
  // algorithm somewhere, but this should suffice for our dataset.

  for(community_set_type::iterator alphaIter = communities.begin();
      alphaIter != communities.end(); ++alphaIter) {

    std::cerr << "  Looking at:  " << alphaIter->name << std::endl;
    
    community_set_type::iterator betaIter = alphaIter;
    for(++betaIter; betaIter != communities.end(); ++betaIter) {

      // Take the two vectors on the unit surface sphere and construct the
      // summation vector.  This intersects the surface sphere at a point
      // precisely between the locations.  The summation vector is not
      // normalized.

      redistrict::coord3 nonnormalCenter
        = alphaIter->centroid.unit() + betaIter->centroid.unit();

      bool gilbertValid = true;  // Assume valid until we prove otherwise

      double limitingProd = nonnormalCenter.dot(alphaIter->centroid.unit());

      for(community_set_type::const_iterator gammaIter = communities.begin();
          gammaIter != communities.end(); ++gammaIter) {
        if(gammaIter == alphaIter || gammaIter == betaIter) { continue; }

        // The dot product between vectors A and B is equal to mag(A) * mag(B)
        // * cos(theta), where mag() is the magnitude (length) of the vector,
        // and theta is the angle between the vectors.  For the following
        // calculation, the magnitude of every unit vector is 1.0.  Also,
        // cos(theta) varies between 1.0 (for colinear vectors) and 0.0 (for
        // antipodal vectors).  Thus, we can determine if this edge is a member
        // of the gilbert graph by comparing the limiting product against every
        // other dot product.  If any dot product is larger than the limiting
        // product, this edge is not a gilbert graph edge, and should be
        // discarded.

        if(nonnormalCenter.dot(gammaIter->centroid.unit()) > limitingProd) {
          gilbertValid = false;
          break;
        }
      }

      // Keep the edge
      if(gilbertValid) {
        alphaIter->adjacency.insert(&*betaIter);
        betaIter->adjacency.insert(&*alphaIter);
      }
    }
  }
}

void write_data(std::ostream& out,
                const community_set_type& communities) {
  out << "GEOID\tNAME\tPOP10\tINTPTLAT\tINTPTLON\tDISTRICT\tADJACENCY\n";
  
  for(community_set_type::const_iterator cIter = communities.begin();
      cIter != communities.end(); ++cIter) {
    out << cIter->id << '\t'
        << cIter->name << '\t'
        << cIter->population << '\t'
        << cIter->centroid.lat() << '\t'
        << cIter->centroid.lon() << '\t'
        << cIter->district << '\t';
    for(redistrict::community::adjacency_type::const_iterator aIter
          = cIter->adjacency.begin();
        aIter != cIter->adjacency.end(); ++aIter) {
      if(aIter != cIter->adjacency.begin()) { out << ':'; }
      out << (*aIter)->id;
    }
    out << "\n";
  }
}

