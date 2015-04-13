//
//  main.cpp
//  mj_calc_xcode
//
//  Created by Michael Jones on 27/03/2015.
//  Copyright (c) 2015 Michael Jones. All rights reserved.
//

#include <iostream>
#include <getopt.h>
#include <string>

#include "soapcalcProxy.h"
#include "calc.nsmap"

using namespace std;

string const standaloneServer = "http://localhost:18083";
string const cgiServer = "http://localhost/cgi-bin/mj_calc_cgi";
string const gsoapServer = "http://localhost/soap";
string const eduServer = "http://websrv.cs.fsu.edu/~engelen/calcserver.cgi";

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    /* options descriptor */
    static struct option longopts[] = {
        { "cgi", no_argument, nullptr, 'c'},
        { "gsoap", no_argument,           nullptr,     'g' },
        { "edu", no_argument, nullptr, 'e'},
        { "add",   required_argument,      nullptr,           'a' },
        { "sub",   required_argument,      nullptr,           's' },
        { "multiply",   required_argument,      nullptr,           'm' },
        { "div",   required_argument,      nullptr,           'd' },
        { "power",   required_argument,      nullptr,           'p' },
        { NULL,    0,                 nullptr,           0 }
    };

  if (argc < 4)
  { fprintf(stderr, "Usage: [add|sub|mul|div|pow] num num\n");
    exit(0);
  }
    
    char const* server = standaloneServer.c_str();
    
    double a{};
    double b{};
    double result{};
    char operation{};
    
    do {
        auto ch = getopt_long(argc, (char* const *)(argv), "gcea:s:m:d:p:", longopts, nullptr);

        switch (ch) {
            case 'a':
            case 's':
            case 'm':
            case 'd':
            case 'p':
                operation = ch;
                a = stod(optarg);
                
                if (optind >= argc) {
                    throw runtime_error("Missing value");
                }
                
                if (argc > (optind + 1)) {
                    throw runtime_error("Too many arguments");
                }
                
                b = stod(argv[optind]);
                break;
                
            case 'c':
                server = cgiServer.c_str();
                
            case 'g':
                server = gsoapServer.c_str();
                break;
                
            case 'e':
                server = eduServer.c_str();
                
            case -1:
                throw runtime_error("Not enough arguments");
                
            default:
                throw runtime_error("Unknown command");
                break;
        }
    } while (operation == 0);
    
  calcProxy calc;
  calc.soap_endpoint = server;
  switch (operation) {
      case 'a':
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
    
    // FIX ME catch and print usage:
    // { fprintf(stderr, "Usage: [add|sub|mul|div|pow] num num\n");

  return 0;
}

