#pragma once
#include <cmath>
#include <float.h>

class metricX
{
public:
    double N   = 0;
    double SA  = 0;
    double SB  = 0;
    double SA2 = 0;
    double SB2 = 0;
    double SAB = 0;
    double SE  = 0;
    double SE2 = 0;

    // Dodanie nowych próbek
    void add(double a, double b, double n = 1.0) {
        double e = fabs(a-b);
        N   += n;
        SA  += pow(a,1);
        SA2 += pow(a,2);
        SAB += a*b;
        SB  += pow(b,1);
        SB2 += pow(b,2);
        SE  += pow(e,1);
        SE2 += pow(e,2);
    }

    // Usunięcie próbek
    void sub(double a, double b, double n = 1.0) {
        double e = fabs(a-b);
        N   -= n;
        SA  -= a;
        SA2 -= a*a;
        SAB -= a*b;
        SB  -= b;
        SB2 -= b*b;
        SE  += e;
        SE2 += e*e;
    }

    // Średnie
    double meanA() const {
        return N > 0 ? SA/N : 0.0;
    }

    double meanB() const {
        return N > 0 ? SB/N : 0.0;
    }

    // Wariancja i odchylenie standardowe
    double varA() const {
        if (N <= 0) return 0.0;
        double v = (SA2/N) - (SA/N) * (SA/N);
        return v > 0 ? v : 0.0;
    }

    double varB() const {
        if (N <= 0) return 0.0;
        double v = (SB2/N) - (SB/N) * (SB/N);
        return v > 0 ? v : 0.0;
    }

    double stddevA() const {
        return sqrt(varA());
    }

    double stddevB() const {
        return sqrt(varB());
    }

    // Kowariancja
    double covAB() const {
        return N > 0 ? (SAB/N) - (SA/N) * (SB/N) : 0.0;
    }

    // MSE/RMSE
    double mse() const {
        return N > 0 ? (SA2 + SB2 - 2 * SAB)/N : 0.0;
    }

    double rmse() const {
        return sqrt(mse());
    }

    // Normalized MSE
    double nmse() const {
        double v = varA();
        return v > 0 ? mse()/v : 0.0;
    }

    // Cosine similarity
    double cosine() const {
        double den = sqrt(SA2 * SB2);
        return den != 0 ? SAB/den : 0.0;
    }

    // Pearson/NCC
    double pearson() const {
        // double num = covAB();
        // double den = sqrt(varA() * varB());
        // return den != 0 ? num/den : 0.0;

        double num = N*SAB - SA*SB;
        double den = sqrt((N*SA2 - SA*SA)*(N*SB2 - SB*SB));
        return den != 0 ? num/den : 0.0;
    }

    // SSIM (populacyjna, dla okna)
    double SSIM(double C1, double C2) const {
        if (N <= 0) return 0.0;
        double muA = meanA();
        double muB = meanB();

        double sigmaA2 = varA();
        double sigmaB2 = varB();
        double sigmaAB = covAB();

        double num = (2 * muA * muB + C1) * (2 * sigmaAB + C2);
        double den = (muA * muA + muB * muB + C1) * (sigmaA2 + sigmaB2 + C2);
        return den != 0 ? num/den : 0.0;
    }

    double SSIM23(double C1, double C2) const {
        if (N <= 0) return 0.0;
        const double winArea = N;  // dla Twojej wersji BEZ okien

        double muA = meanA();
        double muB = meanB();

        double sigmaA2 = varA();
        double sigmaB2 = varB();
        double sigmaAB = covAB();

        double stdA = stddevA();
        double stdB = stddevB();

        double denomStd = stdA * stdB + 1e-12;

        // --- LUMINANCE ---
        double L = (2 * muA * muB + C1) /
                    (muA * muA + muB * muB + C1);

        // --- CONTRAST (umowne rozbicie czynnika CS) ---
        double C = (2 * stdA * stdB + C2) /
                    (sigmaA2 + sigmaB2 + C2 + 1e-12);

        // --- STRUCTURE (umowne rozbicie czynnika CS) ---
        double s_ab = sigmaAB / denomStd;  // "czysta korelacja"
        double S  = (s_ab + C2 / (2 * denomStd)) /
                    (1.0  + C2 / (2 * denomStd));

        // --- FINALNY SSIM ---
        double SSIM = C*S;

        return SSIM;
    }
};
