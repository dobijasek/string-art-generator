#include "layer.h"
#include "benchmark.h"

bool fcmp(double a, double b){
    return fabs(a-b)<=0.0002;
}

void tst(double a, double b){
    if(!fcmp(a,b)){
        printf("FAIL: %f %f\n",a,b);
    } else {
        printf("PASS\n");
    }
}

int main(){
    double cs = 0;
    double cb = 10;
    double weight = 1;
    
    int cwl[] = {4,6,2,6,8};
    int il[] = {1,2,3,4,5};
    std::vector<double> colors{13,20,66,64};
    {
        computePixel_t cp(cs,cb,&weight);
        for(int i=0;i<5;i++){
            lil_t *ll = new lil_t{0,0,0,nullptr,nullptr,il[i],0};
            ll->outlier = 0;
            cp.insert(ll,cwl[i],0.8);
        }
        double c = cp.color();
        tst(c,7.4707);

        auto sr = cp.dataTestv3(colors,0.8,3,3).at(0);
        tst(sr.cdInsert,7.8227);
        tst(sr.cdErase,7.5936);

        sr = cp.dataTestv3(colors,0.8,5,3).at(0);
        tst(sr.cdInsert,8.7187);
        tst(sr.cdErase,7.5936);

        sr = cp.dataTestv3(colors,0.8,6,3).at(0);
        tst(sr.cdInsert,11.9187);
        tst(sr.cdErase,7.5936);

        sr = cp.dataTestv3(colors,0.8,10,3).at(0);
        tst(sr.cdInsert,11.9187);
        tst(sr.cdErase,7.5936);

        sr = cp.dataTestv3(colors,0.8,2,3).at(0);
        tst(sr.cdInsert,7.6435);
        tst(sr.cdErase,7.5936);

        sr = cp.dataTestv3(colors,0.8,1,3).at(0);
        tst(sr.cdInsert,7.5974);
        tst(sr.cdErase,7.5936);

        sr = cp.dataTestv3(colors,0.8,0,3).at(0);
        tst(sr.cdInsert,7.5974);
        tst(sr.cdErase,7.5936);

        benchmark_t b1,b2,b3;
        int iters = 10000000;
        b1.start();
        for(int i=0;i<iters;i++){
            cp.dataTestv(colors,0.8,0,3);
        }
        b1.stop();
        b2.start();
        for(int i=0;i<iters;i++){
            cp.dataTestv2(colors,0.8,0,3);
        }
        b2.stop();
        b3.start();
        for(int i=0;i<iters;i++){
            cp.dataTestv3(colors,0.8,0,3);
        }
        b3.stop();
        printf("%i\n%i\n%i\n",b1.millis(),b2.millis(),b3.millis());
    }
    
}