#include "layer.h"

# define FORCE_INLINE __attribute__((always_inline)) inline

FORCE_INLINE void BLEND(double &var,const double &val,const double &valIntensity) {
    // var = var*(1.0-valIntensity) + val*valIntensity;
    // var = var-var*valIntensity + val*valIntensity;
    var = valIntensity*(val-var) + var;
}

FORCE_INLINE void BLEND(std::vector<outData_t> &w,const std::vector<double> &cw_, double intensity) {
    BLEND(w[0].cdInsert,cw_[0],intensity);
    BLEND(w[1].cdInsert,cw_[1],intensity);
    BLEND(w[2].cdInsert,cw_[2],intensity);
    BLEND(w[3].cdInsert,cw_[3],intensity);
}

FORCE_INLINE void BLEND(std::vector<outData_t> &w,double cw, double intensity) {
    BLEND(w[0].cdInsert,cw,intensity);
    BLEND(w[1].cdInsert,cw,intensity);
    BLEND(w[2].cdInsert,cw,intensity);
    BLEND(w[3].cdInsert,cw,intensity);
}

FORCE_INLINE void BLEND(std::vector<double> &var,double val, double intensity) {
    BLEND(var[0],val,intensity);
    BLEND(var[1],val,intensity);
    BLEND(var[2],val,intensity);
    BLEND(var[3],val,intensity);
}

FORCE_INLINE void BLEND(double *var,double val, double intensity) {
    BLEND(var[0],val,intensity);
    BLEND(var[1],val,intensity);
    BLEND(var[2],val,intensity);
    BLEND(var[3],val,intensity);
}

FORCE_INLINE void BLEND(std::vector<double> &var,const std::vector<double> val, double intensity) {
    BLEND(var[0],val[0],intensity);
    BLEND(var[1],val[1],intensity);
    BLEND(var[2],val[2],intensity);
    BLEND(var[3],val[3],intensity);
}

std::vector<outData_t> computePixel_t::dataTestv(const std::vector<double> &cw_, double intensity, int layerIdxDraw, int layerIdxErase) const {
    std::vector<double> cdInsert(4,cb);
    double cdErase = cb;
    double contribI = 0;
    double contribE = 0;
    bool layerIdxDrawInitialised = 0;
    bool layerIdxEraseInit = 0;
    auto it = layer.begin();
    int lilast = 0;
    for(; it != layer.end(); it++) {
        int itli = it->lil->layerIdx;
        // if(it->lil->drop && itli!=layerIdxDraw) continue;
        lilast = itli;
        if( !layerIdxDrawInitialised && itli>=layerIdxDraw ){
            BLEND(cdInsert,cw_,intensity);
            // contribI = intensity;
            contribI = 1;
            layerIdxDrawInitialised = 1;
        }
        if( itli==layerIdxErase ){
            contribE = 1;
            layerIdxEraseInit = 1;
        } else {
            BLEND(cdErase,it->cw,it->intensity);
            BLEND(cdInsert,it->cw,it->intensity);
            // contribI *= 1.0-it->intensity;
            contribI += 1;
            contribE += 1;
        }
    }
    
    // symulacja insert() na końcu
    if(lilast<layerIdxDraw){
        if(layerIdxDrawInitialised){
            printf("failed layerIdxDrawInitialised\n");
        }
        BLEND(cdInsert,cw_,intensity);
        // contribI = intensity;
        contribI = 1;
        layerIdxDrawInitialised = 1;
    }

    // contribE = contribE*layerIdxEraseInit;
    // contribI = contribI*layerIdxDrawInitialised;
    std::vector<outData_t> w(4,{-1,-1,cs,-1,layerIdxDraw,layerIdxErase,0,0,*weight});
    for(int i=0;i<4;i++){
        w[i].cdInsert = cdInsert[i];
        w[i].cdErase = cdErase;
        w[i].contribE = contribE;
        w[i].contribI = contribI;
    }
    return w;
}


// #if defined(__AVX2__)
//     #pragma message("Compiling with AVX2 enabled")
// #else
//     #pragma message("AVX2 NOT enabled")
// #endif

FORCE_INLINE void BLEND_SIMD(__m256d &v_dst, const __m256d &v_src,const __m256d &v_intensity) {
    __m256d v_diff = _mm256_sub_pd(v_src, v_dst);
    // v_dst = _mm256_add_pd(v_dst, _mm256_mul_pd(_mm256_sub_pd(v_src, v_dst), v_int));
    v_dst = _mm256_fmadd_pd(v_diff, v_intensity, v_dst); // dst += diff*intensity
}

FORCE_INLINE void BLEND_SIMD(double* dst, const double* src, double intensity) {
    __m256d v_dst = _mm256_loadu_pd(dst);
    __m256d v_src = _mm256_loadu_pd(src);
    __m256d v_int = _mm256_set1_pd(intensity);
    BLEND_SIMD(v_dst,v_src,v_int);
    _mm256_storeu_pd(dst, v_dst);
}

FORCE_INLINE void BLEND_SIMD(double* dst, double src, double intensity) {
    __m256d v_dst = _mm256_loadu_pd(dst);
    __m256d v_src = _mm256_set1_pd(src);
    __m256d v_int = _mm256_set1_pd(intensity);
    BLEND_SIMD(v_dst,v_src,v_int);
    _mm256_storeu_pd(dst, v_dst);
}

FORCE_INLINE void BLEND_SIMD(__m256d &v_dst, const __m256d &v_src, double intensity) {
    __m256d v_int = _mm256_set1_pd(intensity);
    BLEND_SIMD(v_dst,v_src,v_int);
}

FORCE_INLINE void BLEND_SIMD(__m256d &v_dst, double src, double intensity) {
    __m256d v_src = _mm256_set1_pd(src);
    __m256d v_int = _mm256_set1_pd(intensity);
    BLEND_SIMD(v_dst,v_src,v_int);
}
extern double intesityRng;
std::vector<outData_t> computePixel_t::dataTestv3(
    const std::vector<double> &cw_, 
    double intensity, 
    int layerIdxDraw, 
    int layerIdxErase
) const {
    // intensity *= intesityRng;
    // double cb2 = layer.begin()
    alignas(32) double cdInsert[4] = {cb, cb, cb, cb};  // SoA SIMD-ready
    const __m256d v_cw = _mm256_loadu_pd(cw_.data());
    __m256d v_cdInsert = _mm256_loadu_pd(cdInsert);
    double cdErase = cb;
    double contribI = 0;
    double contribE = 0;
    bool layerIdxDrawInitialised = false;
    bool layerIdxEraseInit = false;
    auto it = layer.begin();
    int layerIdx = -1;
    for (; it != layer.end(); ++it) {
        // if(it->lil->drop && it->lil->layerIdx!=layerIdxErase) continue;
        if(it->lil->drop) continue;
        layerIdx = it->lil->layerIdx;
        if (!layerIdxDrawInitialised && layerIdx >= layerIdxDraw) {
            BLEND_SIMD(v_cdInsert, v_cw, intensity);
            contribI += 1;
            layerIdxDrawInitialised = true;
        }

        if (layerIdx == layerIdxErase) {
            // contribE = 1;
            layerIdxEraseInit = true;
        } else {
            BLEND_SIMD(v_cdInsert, it->cw, it->intensity);
            BLEND(cdErase,it->cw,it->intensity);
            contribI += 1;
            contribE += 1;
        }
    }
    if (layerIdx < layerIdxDraw) {
        BLEND_SIMD(v_cdInsert, v_cw, intensity);
        contribI += 1;
        layerIdxDrawInitialised = true;
    }
    _mm256_storeu_pd(cdInsert, v_cdInsert);
    // contribI = 1;
    int q = layerIdx<=layerIdxDraw ? layer.size()==1?-1:1 : 0;
    q = layerIdx<=layerIdxDraw;
    int q2 = layer.size()==1 && layerIdx==layerIdxErase;
    // q2 = layer.size();
    // q = contribI;
    // q2 = contribI;
    std::vector<outData_t> w(4, {-1, -1, cs, -1, layerIdxDraw, layerIdxErase, (double)q, (double)q2, *weight});
//    if(contribI>0) printf("%i %f\n",w.size(),contribI);
    for (int i = 0; i < 4; i++) {
        w[i].cdInsert = cdInsert[i];
        w[i].cdErase = cdErase;
        // w[i].contribE = layer.size()-1;
        // w[i].contribI = layer.size();
    }

    return w;
}

double computePixel_t::color() const {
    double cd = cb;
    auto it = layer.begin();
    for(; it != layer.end(); ++it) {
        double intensity = it->intensity;
        // intensity = 0.5;
        BLEND(cd,it->cw,intensity);
    }
    return cd;
}

double computePixel_t::color2() const {
    double cd = cb;
    auto it = layer.begin();
    for(; it != layer.end(); ++it) {
        double intensity = it->intensity;
        intensity = 0.5;
        BLEND(cd,it->cw,intensity);
    }
    return cd;
}