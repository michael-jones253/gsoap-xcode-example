//
//  main.cpp
//  mj_calc_server
//
//  Created by Michael Jones on 27/03/2015.
//  Copyright (c) 2015 Michael Jones. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <future>
#include <forward_list>
#include <chrono>
#include <mutex>

#include "soapcalcService.h"
#include "ServiceFactory.h"
#include "calc.nsmap"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

namespace  {
    int serveClient(calcService* calc) {
        auto ret = int{};
        
        // process RPC request
        ret = calc->serve();
        if (ret != SOAP_OK) {
            calc->soap_stream_fault(std::cerr);
            return ret;
        }
        
        cerr << "Request served." << endl;
        
        return ret;
    }
}

int main(int argc, const char * argv[]) {
    
    calcService calc;
    int m, s; // master and slave sockets
    
    // m = soap_bind(&soap, "machine.genivia.com", 18083, 100);
    const int port = 80;
    const int backlog = 5;
    m = calc.bind("localhost", port, backlog);
    if (m < 0) {
        calc.soap_stream_fault(std::cerr);
        return -1;
    }
    cerr << "Socket connection successful: master socket: " << m << endl;
    
    forward_list<future<int>> handles;
    mutex cleanupMutex;
    
    auto cleanupTask = [&]{
        while (true) {
            auto shouldSleep = bool{};
            {
                future<int> waitHandle{};
                lock_guard<mutex> guard{ cleanupMutex };
                
                auto beforeBegin = handles.before_begin();
                auto it = beforeBegin;
                it++;
                
                if (it != handles.end()) {
                    waitHandle = move(*it);
                    
                    // auto fres = it->get();
                    handles.erase_after(beforeBegin);
                    cerr << "child removed!!!!!" << endl;
                }
                else {
                    shouldSleep = true;
                }
                
                if (waitHandle.valid()) {
                    auto res = waitHandle.get();
                    cerr << "child cleaned!!!!! " << res << endl;
                }
            }
            
            if (shouldSleep) {
                sleep_for(seconds(1));
            }
            
        }
    };
    
    auto cleanupHandle = async(launch::async, cleanupTask);
    
    for (int i = 1; ; i++) {
        s = calc.accept();
        if (s < 0) {
            calc.soap_stream_fault(std::cerr);
            break;
        }
        
        stringstream errStr;
        errStr << i << ": accepted connection from IP: "
        << ((calc.ip >> 24) & 0xFF) << "."
        << ((calc.ip >> 16)&0xFF) << "."
        << ((calc.ip >> 8)&0xFF) << "."
        << ((calc.ip)&0xFF)
        << ", socket: " << s << ends;
        
        cerr << errStr.str() << endl;
        
        auto child = ServiceFactory::Copy(calc);
        
        auto task = [=](unique_ptr<calcService> service)->int {
            stringstream childStr;
            childStr << "Running child service: " << service.get() << " on socket: " << service->socket;
            cerr << childStr.str() << endl;
            auto ret = serveClient(service.get());

            return ret;
        };
        
        auto handle = async(launch::async, task, move(child));
        
        lock_guard<mutex> guard{ cleanupMutex };
        handles.push_front(move(handle));
        
        /* Must not do this or it will close the child socket
         * calc.destroy();
         */
    }
    
    // Shouldn't be needed destructor takes care of things.
    calc.destroy();
}

int calcService::add(double a, double b, double *result)
{ *result = a + b + 100; // Add one for fun!
    return SOAP_OK;
}

int calcService::sub(double a, double b, double *result)
{ *result = a - b;
    return SOAP_OK;
}

int calcService::mul(double a, double b, double *result)
{ *result = a * b;
    return SOAP_OK;
}

int calcService::div(double a, double b, double *result)
{ if (b)
    *result = a / b;
else
{ char *s = (char*)soap_malloc(this, 1024);
    sprintf(s, "<error xmlns=\"http://tempuri.org/\">Can't divide %f by %f</error>", a, b);
    return soap_senderfault("Division by zero", s);
}
    return SOAP_OK;
}

int calcService::pow(double a, double b, double *result)
{ *result = ::pow(a, b);
    if (soap_errno == EDOM)	/* soap_errno is like errno, but compatible with Win32 */
    { char *s = (char*)soap_malloc(this, 1024);
        sprintf(s, "Can't take the power of %f to %f", a, b);
        sprintf(s, "<error xmlns=\"http://tempuri.org/\">Can't take power of %f to %f</error>", a, b);
        return soap_senderfault("Power function domain error", s);
    }
    return SOAP_OK;
}

