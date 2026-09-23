#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <cmath>
#include <map>

using namespace cv;
using namespace std;

// Globalne parametry
const int MIN_DISTANCE = 25;
const int TOURNAMENT_SIZE = 10;
const double MUTATION_RATE = 0.2;
const int STAGNATION_THRESHOLD = 100000;
int populationSize = 1000;
int numGenerations = 10000000;
int numFunctions = 3000;
int displayInterval = 1;
int numPoints = 360;

// Struktura przechowująca parametry funkcji rysującej
struct DrawingFunction {
    Point p1, p2;
    int color;
};

// Funkcja rysująca wykres trendu fitness
void drawFitnessTrend(const vector<double>& fitnessHistory, Mat& trendImg) {
    int width = trendImg.cols;
    int height = trendImg.rows;

    trendImg = Scalar(255); // Białe tło

    double maxFitness = *max_element(fitnessHistory.begin(), fitnessHistory.end());
    double minFitness = *min_element(fitnessHistory.begin(), fitnessHistory.end());

    for (int i = 1; i < fitnessHistory.size(); ++i) {
        Point pt1((i - 1) * width / fitnessHistory.size(), height - (fitnessHistory[i - 1] - minFitness) * height / (maxFitness - minFitness));
        Point pt2(i * width / fitnessHistory.size(), height - (fitnessHistory[i] - minFitness) * height / (maxFitness - minFitness));

        line(trendImg, pt1, pt2, Scalar(0), 1); // Czarna linia
    }

    imshow("Fitness Trend", trendImg); // Wyświetl wykres
}

// Funkcje rysujące
void drawLine(Mat& img, Point p1, Point p2, int color) {
    line(img, p1, p2, Scalar(color), 1);
}

// Funkcja fitness ocenia, jak blisko obraz jest do obrazu docelowego
double fitness(const Mat& img, const Mat& target) {
    Mat diff;
    absdiff(img, target, diff);
    return sum(diff)[0];
}

// Funkcja do uzyskania czterech najczęstszych kolorów w obrazie
vector<int> getTopColors(const Mat& image) {
    map<int, int> colorCounts;
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            int color = image.at<uchar>(y, x);
            colorCounts[color]++;
        }
    }

    vector<pair<int, int>> sortedColors(colorCounts.begin(), colorCounts.end());
    sort(sortedColors.begin(), sortedColors.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second > b.second;
    });

    vector<int> topColors;
    for (int i = 0; i < 4 && i < sortedColors.size(); ++i) {
        topColors.push_back(sortedColors[i].first);
    }

    return topColors;
}

// Funkcja generująca punkty na okręgu wpisanym w obraz
vector<Point> generateCirclePoints(const Size& imgSize, int numPoints) {
    vector<Point> points;
    int radius = min(imgSize.width, imgSize.height) / 2;
    Point center(imgSize.width / 2, imgSize.height / 2);

    for (int i = 0; i < numPoints; ++i) {
        double angle = 2 * M_PI * i / numPoints;
        int x = center.x + radius * cos(angle);
        int y = center.y + radius * sin(angle);
        points.push_back(Point(x, y));
    }

    return points;
}

// Sprawdzanie, czy dwa punkty są oddalone co najmniej o MIN_DISTANCE
bool arePointsDistant(const Point& p1, const Point& p2, const vector<Point>& circlePoints) {
    int index1 = find(circlePoints.begin(), circlePoints.end(), p1) - circlePoints.begin();
    int index2 = find(circlePoints.begin(), circlePoints.end(), p2) - circlePoints.begin();
    int distance = abs(index1 - index2);
    return distance >= MIN_DISTANCE;
}

// Zastosowanie funkcji rysujących na obrazie
void applyDrawingFunctions(Mat& img, const vector<DrawingFunction>& funcs) {
    for (const auto& func : funcs) {
        drawLine(img, func.p1, func.p2, func.color);
    }
}

// Algorytm genetyczny: losowa inicjalizacja
vector<DrawingFunction> generateRandomIndividual(int numFunctions, const Size& imgSize, const vector<int>& colors, const vector<Point>& circlePoints) {
    vector<DrawingFunction> individual;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distColor(0, colors.size() - 1);
    uniform_int_distribution<> distPoint(0, circlePoints.size() - 1);

    for (int i = 0; i < numFunctions; ++i) {
        DrawingFunction func;
        Point p1, p2;

        do {
            p1 = circlePoints[distPoint(gen)];
            p2 = circlePoints[distPoint(gen)];
        } while (!arePointsDistant(p1, p2, circlePoints));

        func.p1 = p1;
        func.p2 = p2;
        func.color = colors[distColor(gen)];
        individual.push_back(func);
    }

    return individual;
}

// Krzyżowanie dwóch osobników
vector<DrawingFunction> crossover(const vector<DrawingFunction>& parent1, const vector<DrawingFunction>& parent2) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, parent1.size() - 1);

    vector<DrawingFunction> offspring;
    int crossoverPoint = dist(gen);

    offspring.insert(offspring.end(), parent1.begin(), parent1.begin() + crossoverPoint);
    offspring.insert(offspring.end(), parent2.begin() + crossoverPoint, parent2.end());

    return offspring;
}

// Mutacja osobnika
void mutate(vector<DrawingFunction>& individual, const Size& imgSize, const vector<int>& colors, const vector<Point>& circlePoints) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distColor(0, colors.size() - 1);
    uniform_int_distribution<> distPoint(0, circlePoints.size() - 1);
    uniform_real_distribution<> mutationRate(0.0, 1.0);

    for (auto& func : individual) {
        if (mutationRate(gen) < MUTATION_RATE) {
            Point p1, p2;
            do {
                p1 = circlePoints[distPoint(gen)];
                p2 = circlePoints[distPoint(gen)];
            } while (!arePointsDistant(p1, p2, circlePoints));

            func.p1 = p1;
            func.p2 = p2;
            func.color = colors[distColor(gen)];
        }
    }
}

// Selekcja turniejowa
int tournamentSelection(const vector<pair<double, int>>& fitnessScores) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, TOURNAMENT_SIZE - 1);

    vector<int> tournament;
    for (int i = 0; i < TOURNAMENT_SIZE; ++i) {
        tournament.push_back(dist(gen) % fitnessScores.size());
    }

    sort(tournament.begin(), tournament.end(), [&fitnessScores](int a, int b) {
        return fitnessScores[a].first < fitnessScores[b].first;
    });

    return tournament[0];
}

int main() {
    // Załaduj obraz docelowy w skali szarości
    Mat target = imread("target_image.jpg", IMREAD_GRAYSCALE);
    if (target.empty()) {
        cerr << "Błąd ładowania obrazu docelowego!" << endl;
        return -1;
    }

    // Uzyskaj cztery najczęstsze kolory z obrazu docelowego
    vector<int> topColors = getTopColors(target);
    if (topColors.size() != 4) {
        cerr << "Nie udało się znaleźć czterech kolorów w obrazie!" << endl;
        return -1;
    }

    Size imgSize = target.size();
    vector<Point> circlePoints = generateCirclePoints(imgSize, numPoints);

    // Inicjalizuj populację
    vector<vector<DrawingFunction>> population;
    for (int i = 0; i < populationSize; ++i) {
        population.push_back(generateRandomIndividual(numFunctions, imgSize, topColors, circlePoints));
    }

    // Utwórz okna do wyświetlania
    namedWindow("Podglad obrazu", WINDOW_AUTOSIZE);
    namedWindow("Fitness Trend", WINDOW_AUTOSIZE);

    // Algorytm genetyczny
    vector<double> fitnessHistory; // Historia najlepszych wyników fitness
    double previousBestFitness = numeric_limits<double>::max();
    int stagnationCount = 0;

    Mat fitnessTrendImg(400, 600, CV_8UC1); // Obraz dla wykresu fitness

    for (int generation = 0; generation < numGenerations; ++generation) {
        vector<pair<double, int>> fitnessScores;
        for (int i = 0; i < populationSize; ++i) {
            Mat img = Mat::zeros(imgSize, CV_8UC1); // Obraz w skali szarości
            applyDrawingFunctions(img, population[i]);
            double score = fitness(img, target);
            fitnessScores.push_back({score, i});
        }

        // Sortowanie wg wyników fitness
        sort(fitnessScores.begin(), fitnessScores.end());

        // Zapisz najlepszy wynik do historii
        fitnessHistory.push_back(fitnessScores[0].first);

        // Wyświetlanie co X generacji
        if (generation % displayInterval == 0) {
            Mat bestImg = Mat::zeros(imgSize, CV_8UC1);
            applyDrawingFunctions(bestImg, population[fitnessScores[0].second]);
            imshow("Podglad obrazu", bestImg);
            waitKey(1);

            // Rysuj wykres fitness
            drawFitnessTrend(fitnessHistory, fitnessTrendImg);
            waitKey(1);
        }

        // Tworzenie nowej populacji
        vector<vector<DrawingFunction>> newPopulation;

        for (int i = 0; i < populationSize / 2; ++i) {
            newPopulation.push_back(population[fitnessScores[i].second]); // Elitarna selekcja
        }

        while (newPopulation.size() < populationSize) {
            int parent1Idx = tournamentSelection(fitnessScores);
            int parent2Idx = tournamentSelection(fitnessScores);
            vector<DrawingFunction> offspring = crossover(population[parent1Idx], population[parent2Idx]);
            mutate(offspring, imgSize, topColors, circlePoints);
            newPopulation.push_back(offspring);
        }

        population = newPopulation;

        // Sprawdź stagnację
        if (fitnessScores[0].first >= previousBestFitness) {
            stagnationCount++;
        } else {
            stagnationCount = 0;
        }
        previousBestFitness = fitnessScores[0].first;

        if (stagnationCount >= STAGNATION_THRESHOLD) {
            cout << "Algorytm uległ stagnacji. Kończenie..." << endl;
            break;
        }

        // Wyświetl najlepszy wynik
        cout << "Generacja " << generation << " - Najlepszy wynik: " << fitnessScores[0].first << endl;
    }

    // Zapisz najlepszy wynikowy obraz
    Mat bestImg = Mat::zeros(imgSize, CV_8UC1);
    applyDrawingFunctions(bestImg, population[0]);
    imwrite("best_result.jpg", bestImg);

    // Wyświetl najlepszy wynik końcowy
    imshow("Podglad obrazu", bestImg);
    waitKey(0);

    return 0;
}
