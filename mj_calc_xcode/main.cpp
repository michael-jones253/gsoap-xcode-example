//
//  main.cpp
//  mj_calc_xcode
//
//  Created by Michael Jones on 27/03/2015.
//  Copyright (c) 2015 Michael Jones. All rights reserved.
//

#include <iostream>

#include "soapcalcProxy.h"
#include "calc.nsmap"

// const char server[] = "http://websrv.cs.fsu.edu/~engelen/calcserver.cgi";
// const char server[] = "http://localhost/cgi-bin/mj_calc_cgi";
const char server[] = "localhost";


int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";

  if (argc < 4)
  { fprintf(stderr, "Usage: [add|sub|mul|div|pow] num num\n");
    exit(0);
  }
  double a, b, result;
  a = strtod(argv[2], NULL);
  b = strtod(argv[3], NULL);
  calcProxy calc;
  calc.soap_endpoint = server;
  switch (*argv[1])
  { case 'a':
      calc.add(a, b, &result);
      break;
    case 's':
      calc.sub(a, b, &result);
      break;
    case 'm':
      calc.mul(a, b, &result);
      break;
    case 'd':
      calc.div(a, b, &result);
      break;
    case 'p':
      calc.pow(a, b, &result);
      break;
    default:
      fprintf(stderr, "Unknown command\n");
      exit(0);
  }
  if (calc.error)
    calc.soap_stream_fault(std::cerr);
  else
    printf("result = %g\n", result);
  return 0;
}

