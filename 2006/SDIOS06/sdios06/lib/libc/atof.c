#include <stdlib.h>

double atof(const char *nptr) {
  double tmp=strtod(nptr,0);
  return tmp;
}
