//
//  ServiceFactory.h
//  mj_calc_xcode
//
//  Created by Michael Jones on 27/03/2015.
//  Copyright (c) 2015 Michael Jones. All rights reserved.
//

#ifndef __mj_calc_xcode__ServiceFactory__
#define __mj_calc_xcode__ServiceFactory__

#include <memory>
#include <stdio.h>

#include "soapH.h"

class ServiceFactory {
public:
    template <class T>
    static std::unique_ptr<T> Copy(T const & original) {
        return std::unique_ptr<T>( new T((soap&)original));
    }
};

#endif /* defined(__mj_calc_xcode__ServiceFactory__) */
