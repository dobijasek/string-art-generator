
#include "drawLines.h"
#include "find.h"
#include "param.h"
#include <tclap\ArgException.h>
#include "gui.h"
#include "pool.h"
#include "Instrumentor.h"
#include <signal.h>
extern std::mutex dstMutex;
void signal_callback_handler(int signum) {
  printf("Exit signal: %i\n",signum);
  dstMutex.unlock();
  Instrumentor::Get().EndSession();
  TASKER.kill();
  guiDestroy();
  printf("CLOSING\n");
  exit(signum);
}

char buf[100000];
int main( int argc, char **argv )
{  
  try
  {
    
    // setvbuf(stdout, buf, _IOFBF, 100000);
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler); // FIXME: nadal wyrzuca 'called without an active exception' po zamknięciu w środku obliczeń
    signal(SIGABRT, signal_callback_handler);
    signal(SIGBREAK, signal_callback_handler);
    initParam(argc,argv);
    drawLines();
  }
  catch (TCLAP::ArgException &e)
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
  }
  return 0;
}

// TODO: redukcja kolorów
// TODO: dithering
// aplikowanie transparency do dst daje ładny obraz ale tylko dla dst, rysujac to bez transparency brakuje kolorów
// IDEA: generować sekwencję po czym zostawić np 100 ostatnich, generować ponownie pod nimi... aż to wypełnienia lub naprzemiennie w zaleznosci gdzie lepszy score