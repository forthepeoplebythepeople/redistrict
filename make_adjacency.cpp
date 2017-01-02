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

typedef std::vector<district::community> community_set_type;

bool read_data(const char*, community_set_type&);
void write_data(std::ostream&, const community_set_type&);

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cerr << "Usage:  make_adjacency <datafile>" << std::endl;
    return(1);
  }
  
  community_set_type communities;

  if(!read_data(argv[1], communities)) {
    return(2);
  }

  // Do something in a moment...

  write_data(std::cout, communities);
  return(0);
}


bool read_data(const char* filename,
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
    district::geo_coord::value_type latCache = 0.0;
    
    while(startIdx < line.size()) {
      size_t endIdx = line.find('\t', startIdx);
      
      switch(fieldId) {
      case 1:   // GEOID
        communities.push_back(district::community());
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
      
      ++fieldId;
      if(endIdx == std::string::npos) { break; }
      startIdx = endIdx + 1;
    }

    if(fieldId != 0 && complete == false) {
      std::cerr << "Found and skipping an incomplete line.  It looks like:\n  "
                << line << std::endl;
    }
  }

  return(true);
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
    for(district::community::adjacency_type::const_iterator aIter
          = cIter->adjacency.begin();
        aIter != cIter->adjacency.end(); ++aIter) {
      if(aIter != cIter->adjacency.begin()) { out << ':'; }
      out << (*aIter)->id;
    }
    out << "\n";
  }
}

