#include "lineAA.h"
#include <mutex>
#include "pool.h"

// double square(double x){
//     return x*x;
// }

double constrain_(double val, double l, double h){
    return val>h ? h : val<l ? l : val;
}

double gammaCorrection(double val, double gamma){
    double valMax = 1.0;
    return std::pow(val/valMax,gamma);
}

class lineX_t
{
    public:
    const double x1,y1,x2,y2;
    const double dx,dy;
    const double len;
    const double one_len;
    const double halfThickness;
    const double halfWide;
    const double intensityLo;
    const double intensityHi;
    lineX_t(double x1, double y1, double x2, double y2
    ,double thickness, double wide, double intensityLo, double intensityHi)
    : x1(x1), y1(y1), x2(x2), y2(y2)
    , dx(x2-x1), dy(y2-y1), len(sqrt(dx*dx+dy*dy)), one_len(1.0/len)
    , halfWide(wide/2.0)
    , halfThickness(thickness/2.0)
    , intensityLo(intensityLo)
    , intensityHi(intensityHi){
    }

    inline double distanceFromLine(double x, double y) const {
        return fabs((y1-y)*dx-(x1-x)*dy)/sqrt(dx*dx+dy*dy);
        // return fabs((dx)*(y1-y)-(x1-x)*(dy))*one_len;
    }
    
    inline double distanceBetweenPoints(double x1, double y1, double x2, double y2) const {
        double dx_ = x2-x1;
        double dy_ = y2-y1;
        return sqrt(dx_*dx_+dy_*dy_);
    }

    // sprawdza czy punkt znajduje się między końcami odcinka
    inline bool isBetweenSegmentEnds(double x, double y) const {
        // Oblicz parametryczny współczynnik t
        double dx = x2-x1;
        double dy = y2-y1;
        double dotProduct = (x-x1)*dx + (y-y1)*dy;
        // kąt ostry - dot>0
        // kąt rozwarty - dot<0
        // kąt prosty - dot==0
        double segmentLengthSquared = dx*dx + dy*dy;
        // double cosTheta = dotProduct/sqrt(segmentLengthSquared);

        // Sprawdzamy czy t mieści się w zakresie [0, 1]
        return dotProduct >= 0 && dotProduct <= segmentLengthSquared;
    }

    inline double getDistance(double x, double y) const {
        if(isBetweenSegmentEnds(x,y)){
            double d0 = distanceFromLine(x,y);
            return d0;
        } else {
            double d1 = distanceBetweenPoints(x,y,x1,y1);
            double d2 = distanceBetweenPoints(x,y,x2,y2);
            return std::min(d1,d2);
        }
        return -1;
    }

    inline double distance2intensity(double distance) const {
        double intensity = 0;
        if(distance<0){
            intensity = 0;
        } else if(halfWide<halfThickness){
            intensity = mapf(distance, halfWide, halfThickness, intensityHi, intensityLo);
            intensity = constrain_(intensity, intensityLo, intensityHi);
            // double q1 = mapf(distance, halfWide, halfThickness, M_PI/2.0, 0);
            // q1 = constrain_(q1, 0, M_PI/2.0);
            // intensity = sin(q1);
        } else if(halfWide>=halfThickness){
            intensity = intensityHi;
        }
        return intensity;
    }

    struct xxx_t
    {
        double distance;
        double intensity;
        int matches;
    };

    xxx_t getIntensity(double x, double y) const {
        double distance = getDistance(x, y);
        double intensity = distance2intensity(distance);
        int matchesCnt = distance<=halfThickness;
        return {distance,intensity,matchesCnt};
    }

    xxx_t getIntensityGrid(double x, double y, int vsub,int ssMode, double range) const {
        double distanceSum = 0;
        double intensitySum = 0;
        int matchesCnt = 0;
        double gridSize = 0;
        auto ff = [&](double dx, double dy) {
            double distance = getDistance(x+dx, y+dy);
            if(distance<=halfThickness) {
                intensitySum += distance2intensity(distance);
                matchesCnt++;
            }
            gridSize += 1;
        };
        supersampling(vsub,ff,ssMode,range);
        double distanceMean = distanceSum/matchesCnt;
        double intensityMean = intensitySum/gridSize;
        return {distanceMean,intensityMean,matchesCnt};
    }
    
    // il!=ih
    double mapf(double i,double il,double ih,double ol,double oh) const {
        return (i-il)*(oh-ol)/(ih-il)+ol;
    }

    double y2x(double y) const {
        if (y1==y2)  return x1;
        if (y <= y1) return x1;
        if (y >= y2) return x2;
        return mapf(y,y1,y2,x1,x2);
    }

    void thruLineAA(int vsub, int ssMode, double range
        , const std::function<void(int x,int y, double intensity)>& f) const {
        int yMin = floor(y1-halfThickness*2);
        int yMax = ceil(y2+halfThickness*2);
        for(int y=yMin;y<yMax;y++) {
            int xOffset = 0;
            int xDir = 1;
            double xLine = y2x(y);
            while(1) {
                int x = round(xLine) + xOffset;
                auto q = getIntensityGrid(x,y,vsub,ssMode,range);
                if(q.matches>0){
                    f(x,y,q.intensity);
                } else {
                    if(xDir==1) {
                        xOffset = -1;
                        xDir = -1;
                        continue;
                    } else if(xDir==-1) {
                        break;
                    }
                }
                xOffset += xDir;
            }
        }
    }
};


std::vector<Pixel> generateSmoothLine(double x1, double y1, double x2, double y2
, double thickness, double wide, int vsub, double gc, double imin, int ssMode, double range) {
    if(y2<y1){
        std::swap(y2,y1);
        std::swap(x2,x1);
    }
    wide = wide>thickness ? thickness : wide;
    vsub = vsub<1 ? 1 : vsub;
    std::vector<Pixel> pixels;
    double lo = imin;
    double hi = 0.7;
    lineX_t line(x1,y1,x2,y2,thickness,wide,lo,hi);
    line.thruLineAA(vsub,ssMode,range, [&](int x,int y,double intensity){
        pixels.push_back({x,y,gammaCorrection(intensity,gc)});
    });

    // double max = DBL_MIN;
    // double min = DBL_MAX;
    // for(auto &q : pixels){
    //     if(q.alpha>max){
    //         max = q.alpha;
    //     }
    //     if(q.alpha<min){
    //         min = q.alpha;
    //     }
    // }
    // for(auto &q : pixels){
    //     q.alpha = mapf(q.alpha,min,max,0,1.0);
    // }
    return pixels;
}