//
// Created by Sergiovan on 18-Mar-17.
//

#ifndef ISLANG_COMPAT_H
#define ISLANG_COMPAT_H

#include <string>

namespace compat {
    void init();
    bool isRelativePath(std::string path);
}

#endif //ISLANG_COMPAT_H