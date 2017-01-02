// structures.hpp
//
// A place to put strutures common to various scripts.
//

#ifndef _STRUCTURES_HPP_
#define _STRUCTURES_HPP_

#include <cmath>
#include <set>
#include <string>

namespace district {

  struct coord3 {
    typedef double value_type;

    value_type x;
    value_type y;
    value_type z;

    coord3(value_type = 0.0,
           value_type = 0.0,
           value_type = 0.0);
    coord3(const coord3&);
    ~coord3();

    coord3& operator=(const coord3&);
    coord3& operator+=(const coord3&);
    coord3 operator+(const coord3&) const;
    value_type dot(const coord3&) const;
    
  }; // end struct coord3
  
  class geo_coord {
  public:
    typedef double value_type;

  private:
    value_type _lat;  // These are in degrees.
    value_type _lon;
    coord3     _unit; // A dimensionless unit vector

    void _latlon2unit();

  public:
    geo_coord();
    geo_coord(value_type lat, value_type lon);
    geo_coord(const geo_coord&);
    ~geo_coord();

    geo_coord& operator=(const geo_coord&);

    value_type lat() const;
    value_type lon() const;
    const coord3& unit() const;
    
    void set(value_type lat, value_type lon);
    
  }; // end struct geo_coord
  
  // Used to represent one indivisible community.
  struct community {
    typedef std::set<community*> adjacency_type;
    
    size_t id;  // A FIPS-assigned code, probably.  Will need 64 bits,
                // probably...
    std::string name;
    size_t population;
    geo_coord centroid;
    adjacency_type adjacency;
    size_t district;  // Zero is unassigned.  We intend to assign.
    
  }; // end struct community

  
  //
  // Inline definitions for coord3 methods
  //
  inline coord3::coord3(value_type ex,
                        value_type wye,
                        value_type zee) :
    x(ex),
    y(wye),
    z(zee) {}

  inline coord3::coord3(const coord3& that) :
    x(that.x),
    y(that.y),
    z(that.z) {}

  inline coord3::~coord3() {}

  inline coord3& coord3::operator=(const coord3& that) {
    x = that.x;
    y = that.y;
    z = that.z;

    return(*this);
  }
  
  inline coord3& coord3::operator+=(const coord3& that) {
    x += that.x;
    y += that.y;
    z += that.z;

    return(*this);
  }

  inline coord3 coord3::operator+(const coord3& that) const {
    coord3 reply = *this;
    return(reply += that);
  }

  inline coord3::value_type coord3::dot(const coord3& that) const {
    return(x * that.x + y * that.y + z * that.z);
  }

  
  //
  // Inline definitions for geo_coord methods
  //
  inline geo_coord::geo_coord() :
    _lat(0.0), _lon(0.0),
    _unit(1.0, 0.0, 0.0) {}

  inline geo_coord::geo_coord(value_type lat, value_type lon) :
    _lat(lat), _lon(lon),
    _unit() { _latlon2unit(); }

  inline geo_coord::geo_coord(const geo_coord& that) :
    _lat(that._lat),
    _lon(that._lon),
    _unit(that._unit) {}

  inline geo_coord::~geo_coord() {}

  inline geo_coord& geo_coord::operator=(const geo_coord& that) {
    _lat = that._lat;
    _lon = that._lon;
    _unit = that._unit;
    return(*this);
  }

  inline void geo_coord::_latlon2unit() {
    value_type theta = 3.1415926536 * _lon / 180.0;
    value_type phi   = 3.1415926536 * _lat / 180.0;
    
    value_type costheta = std::cos(theta);
    value_type cosphi   = std::cos(phi);
    value_type sintheta = std::sin(theta);
    value_type sinphi   = std::sin(phi);
    
    _unit.x = costheta * cosphi;
    _unit.y = sintheta * cosphi;
    _unit.z = sinphi;
  }

  inline geo_coord::value_type geo_coord::lat() const { return(_lat); }
  inline geo_coord::value_type geo_coord::lon() const { return(_lon); }
  inline const coord3& geo_coord::unit() const { return(_unit); }

  inline void geo_coord::set(geo_coord::value_type lat, geo_coord::value_type lon) {
    _lat = lat;
    _lon = lon;
    _latlon2unit();
  }
  
} // end namespace

#endif
