#include "param.h"
#include <tclap/CmdLine.h>
#include <string>

using namespace TCLAP;
using namespace std;

int point_gap = 2;
int w_val = 0;
int g_val = 0;
int noise_mean = 0;
int noise_sigma = 0;
int darkness1 = 0; 
int darkness2 = 0; 
int darkness3 = 0;
int points = 0;
int fast = 1;
int pen = 1;
int saver = 1;
int sides = 0;
string input1_image; 
string input2_image; 
int RUN = 1;
int lines = 1000;
int lines2 = 1000;
int NAILSCNT = 360;
void initParam( int argc, char **argv ){
  CmdLine cmd("Draws an image from straight lines.", ' ', "1.0");
  ValueArg<double> meanArg( "m", "mean", "Factor to resize by", false, 0, "double", cmd);
  ValueArg<double> sigmArg( "a", "sigma", "Factor to resize by", false, 0, "double", cmd);
  ValueArg<int> darknessArg1( "d", "darkness", "Darkness of lines. Integer from 1 to 255, with 255 being completely black.", false, 50, "int", cmd);
  ValueArg<int> darknessArg2( "f", "darkness2", "Darkness of lines. Integer from 1 to 255, with 255 being completely black.", false, 0, "int", cmd);
  ValueArg<int> darknessArg3( "x", "darkness3", "Darkness of lines. Integer from 1 to 255, with 255 being completely black.", false, 0, "int", cmd);
  ValueArg<int> sidesArg( "s", "sides", "Number of sides", false, 0, "int", cmd);
  ValueArg<int> pointsArg( "p", "points", "Number of points on outside", false, NAILSCNT, "int", cmd);
  ValueArg<int> linesArg( "n", "lines", "Number of lines to draw", false, 5000, "int", cmd);
  ValueArg<string> outputArg( "o", "output", "Output image", false, "..\\images\\out.png", "string", cmd);
  ValueArg<string> inputArg( "i", "input1", "Input image", false,  "..\\images\\skull4.png", "string", cmd);
  ValueArg<string> input2Arg( "j", "input2", "Input image", false, "..\\images\\skull4.png", "string", cmd);
  ValueArg<int> q1var( "q", "gap", "distance", false, 25, "int", cmd);
  ValueArg<int> w1var( "w", "jump", "distance", false, 0, "int", cmd);
  ValueArg<int> g1var( "g", "gen", "distance", false, 0, "int", cmd);
  ValueArg<int> e1var( "e", "fast", "distance", false, 0, "int", cmd);
  ValueArg<int> y1var( "y", "penalty", "distance", false, 1, "int", cmd);
  ValueArg<int> u1var( "u", "saver", "distance", false, 1, "int", cmd); 

  cmd.parse( argc, argv );

  input1_image        = inputArg.getValue();
  input2_image        = input2Arg.getValue();
  string output_image = outputArg.getValue();
  lines               = linesArg.getValue();
  points              = pointsArg.getValue();
  sides               = sidesArg.getValue();
  darkness1           = darknessArg1.getValue();
  darkness2           = darknessArg2.getValue();
  darkness3           = darknessArg3.getValue();
  noise_mean          = meanArg.getValue();
  noise_sigma         = sigmArg.getValue();
  point_gap           = q1var.getValue();
  w_val               = w1var.getValue();
  g_val               = g1var.getValue();
  fast                = e1var.getValue();
  pen                 = y1var.getValue();
  saver               = u1var.getValue();
}