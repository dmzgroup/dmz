#ifndef DMZ_QT_MAP_UTIL_DOT_H
#define DMZ_QT_MAP_UTIL_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesConsts.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>
#include <math.h>

// From http://msdn2.microsoft.com/en-us/library/bb259689.aspx

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define CLIP(X, min, max) MIN(MAX(min, X), max)

static const dmz::Int32 MinLevelOfDetail = 2;
static const dmz::Int32 MaxLevelOfDetail = 18;
static const dmz::Float64 EarthRadius = 6378137.0;
static const dmz::Float64 MinLatitude = -85.05112878;
static const dmz::Float64 MaxLatitude = 85.05112878;
static const dmz::Float64 MinLongitude = -180.0;
static const dmz::Float64 MaxLongitude = 180.0;
static const dmz::Float64 E = 2.71828182845904523536;


namespace dmz {

   
   struct Point {
      
      Int32 x;
      Int32 y;
      
      Point () : x (0), y (0) {;}
      Point (const Int32 TheX, const Int32 TheY) : x (TheX), y (TheY) {;}
      Point (const Point &ThePoint): x (ThePoint.x), y (ThePoint.y) {;}
   };
   
   struct Coordinate {
      
      Float64 latitude;
      Float64 longitude;
      
      Coordinate () : latitude (0.0), longitude (0.0) {;}
      
      Coordinate (const Float64 TheLatitude, const Float64 TheLongitude) :
         latitude (TheLatitude),
         longitude (TheLongitude) {;}
         
      Coordinate (const Coordinate &Coord) :
         latitude (Coord.latitude),
         longitude (Coord.longitude) {;}
   };

   UInt32 map_size_tiles (Int32 levelOfDetail);
   UInt32 map_size_pixels (Int32 levelOfDetail);
   
   Float64 ground_resolution (Float64 latitude, Int32 levelOfDetail);
   
   Float64 map_scale (Float64 latitude, Int32 levelOfDetail, Int32 screenDPI);
   
   Point scale_pixelxy (const Point PixelXY, Int32 levelOfDetail, Int32 desiredLevelOfDetail);
   
   Coordinate pixelxy_to_latLon (const Point &PixelXY, Int32 levelOfDetail);
   Point latlong_to_pixelxy (const Coordinate Coord, Int32 levelOfDetail);
   
   Point pixelxy_to_tilexy(const Point &ThePoint);
   Point tilexy_to_pixelxy (const Point &ThePoint);
   
   String tilexy_to_quadkey (const Point &TheTile, Int32 levelOfDetail);
};


// const dmz::Int32 MinLevelOfDetail = 2;
// const dmz::Int32 MaxLevelOfDetail = 18;
// const dmz::Float64 EarthRadius = 6378137.0;
// const dmz::Float64 MinLatitude = -85.05112878;
// const dmz::Float64 MaxLatitude = 85.05112878;
// const dmz::Float64 MinLongitude = -180.0;
// const dmz::Float64 MaxLongitude = 180.0;
// const dmz::Float64 E = 2.71828182845904523536;


// Determines the map width and height (in tiles) at a specified level of detail.
inline dmz::UInt32
dmz::map_size_tiles (Int32 levelOfDetail) {

   return (1 << levelOfDetail);
}


// Determines the map width and height (in pixels) at a specified level of detail.
inline dmz::UInt32
dmz::map_size_pixels (Int32 levelOfDetail) {

   return (256 << levelOfDetail);
}


// Determines the ground resolution (in meters per pixel) at a specified latitude and level of detail.
inline dmz::Float64
dmz::ground_resolution (Float64 latitude, Int32 levelOfDetail) {
   
   latitude = CLIP (latitude, MinLatitude, MaxLatitude);
   return cos (latitude * Pi64 / 180.0) * 2.0 * Pi64 * EarthRadius / (Float64)map_size_pixels (levelOfDetail);
}


// Determines the map scale at a specified latitude (in degrees), level of detail, and screen resolution.
inline dmz::Float64
dmz::map_scale (Float64 latitude, Int32 levelOfDetail, Int32 screenDPI) {
   
   return ground_resolution (latitude, levelOfDetail) * screenDPI / 0.0254;
}


// Scales a pixel location to a new level of detail
inline dmz::Point
dmz::scale_pixelxy (const Point PixelXY, Int32 levelOfDetail, Int32 desiredLevelOfDetail) {
   
   Point newPoint (PixelXY);
   
   Int32 theLevelOfDetailDifference = desiredLevelOfDetail - levelOfDetail;

   if (theLevelOfDetailDifference > 0) {
      
      Float64 theFactor = pow (2, theLevelOfDetailDifference);
      newPoint.x = PixelXY.x * theFactor;
      newPoint.y = PixelXY.y * theFactor;
   }

   return newPoint;
}


// Converts a pixel location to coordinates at a given level of detail 
inline dmz::Coordinate
dmz::pixelxy_to_latLon (const Point &PixelXY, Int32 levelOfDetail) {
   
   const Int32 TheMapSize (map_size_pixels (levelOfDetail));

   Float64 y = PixelXY.y;
   y = pow (E, (0.5 - ((y - 0.5) / TheMapSize)) * (4.0 * Pi64));
   y = -(1.0 - y) / (1.0 + y);
   y = asin (y) / (Pi64 / 180);

   Coordinate coord (fmod ((PixelXY.x - 0.5) / TheMapSize * 360.0, 360.0) - 180.0, y);
	
   coord.latitude = CLIP (coord.latitude, MinLatitude, MaxLatitude);
   coord.longitude = CLIP (coord.longitude, MinLongitude, MaxLongitude);

   return (coord);
}


// Converts a point from latitude/longitude WGS-84 coordinates (in degrees) into pixel XY coordinates at a specified level of detail.
inline dmz::Point
dmz::latlong_to_pixelxy (const Coordinate Coord, Int32 levelOfDetail) {
   
   const Float64 TheLatitude = CLIP (Coord.latitude, MinLatitude, MaxLatitude);
   const Float64 TheLongitude = CLIP (Coord.longitude, MinLongitude, MaxLongitude);

   Float64 x = (TheLongitude + 180.0) / 360.0; 
   Float64 sinLatitude = sin (TheLatitude * Pi64 / 180.0);
   Float64 y = 0.5 - log ((1.0 + sinLatitude) / (1.0 - sinLatitude)) / (4.0 * Pi64);

   const Int32 TheMapSize = map_size_pixels (levelOfDetail);
   
   Point pixelXY (
      CLIP (x * TheMapSize + 0.5, 0.0, TheMapSize - 1.0),
      CLIP (y * TheMapSize + 0.5, 0.0, TheMapSize - 1.0));
   
   return (pixelXY);
}


// Converts pixel XY coordinates into tile XY coordinates.
inline dmz::Point
dmz::pixelxy_to_tilexy(const Point &ThePoint) {
   
   Point newPoint (ThePoint.x / 256, ThePoint.y / 256);
   return (newPoint);
}


inline dmz::Point
dmz::tilexy_to_pixelxy (const Point &ThePoint) {
   
   Point newPoint (ThePoint.x * 256, ThePoint.y * 256);
   return (newPoint);
}


// Generates string key for a tile at given coordinate and level of detail
inline dmz::String
dmz::tilexy_to_quadkey (const Point &TheTile, Int32 levelOfDetail) {

   String quadKey;
   
   for (int i = levelOfDetail; i > 0; i--) {
      
      char digit = '0';
      const int mask = 1 << (i - 1);
      
      if ((TheTile.x & mask) != 0) {
         
         digit++;
      }

      if ((TheTile.y & mask) != 0) {
         
         digit++;
         digit++;
      }
      
      quadKey << digit;
   }

   return (quadKey);
}


#endif // DMZ_QT_MAP_UTIL_DOT_H

