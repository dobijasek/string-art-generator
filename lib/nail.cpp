#include "nail.h"
#include "util.h"
#include "drawLines.h"
#include <math.h>

std::vector<nail_t> createRectangle( int points, int w, int h )
{
  std::vector<nail_t> nail;
  int x,y,edge;
  int sizeEdge = 2 * ( w + h );

  // For every point on the outside
  for( int ps = 0; ps < points; ps++ )
  {
    // Get the two points
    int p = ( sizeEdge * ps ) / points;

    if( p < w )
    {
      x = p;
      y = 0;
      edge = 0;
    }
    else if( p < w + h )
    {
      x = w-1;
      y = p - w;
      edge = 1;
    }
    else if( p < 2 * w + h)
    {
      x = w-1 - ( p - w - h );
      y = h-1;
      edge = 2;
    }
    else
    {
      x = 0;
      y = h-1 - ( p - 2 * w - h );
      edge = 3;
    }
    nail.push_back({x,y,edge});
  }
  return nail;
}

std::vector<nail_t>  createPolygon( int points, int sides, int w, int h )
{
  std::vector<nail_t> nail;
  int x,y,edge;

  double pi, TAU;

  // Polygon
  if( sides > 2 )
  {
    pi = 3.14159265358979 / (double)sides;
    TAU = (double)sides / points;
  }
  // Ellipse
  else
  {
    TAU = 6.283185307 / points;
  }

  // For every point on the outside
  for( int ps = 0; ps < points; ps++ )
  {
    double theta = ps * TAU;

    if( sides > 2 )
    {
      double u = cos(pi)*cos(pi*(2*floor(theta)+1))-(2*theta-2*floor(theta)-1)*sin(pi)*sin(pi*(2*floor(theta)+1));
      double v = cos(pi)*sin(pi*(2*floor(theta)+1))+(2*theta-2*floor(theta)-1)*sin(pi)*cos(pi*(2*floor(theta)+1));

      u *= w/2;
      v *= h/2;

      x = (int)(u + w/2);
      y = (int)(v + h/2);
      edge = ps/(points/sides);
    }
    else
    {
      x = (int)(cos(theta)*w/2+w/2);
      y = (int)(sin(theta)*h/2+h/2);
      edge = ps;
    }

    x = std::min(x,w-1);
    y = std::min(y,h-1);
    // printf("%i %i %i %i\n",x,y,w,h);
    nail.push_back({x,y,edge});
  }
  return nail;
}

std::vector<nail_t>  generateNails(int points, int sides, int w, int h){
  std::vector<nail_t> nail;
  if( sides < 0 ){
    nail = createRectangle( points, w, h );
  } else {
    nail = createPolygon(  points, sides, w, h );
  }
  return nail;
}

int getNailsIndex(int start, int end, int points_count){
  return start<end ? (end*points_count+start) : (start*points_count+end);
}