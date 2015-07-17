#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H
#include <iostream>
#include <string>
#include <vector>
#include "QTSource.h"
#include <cstring>
#include <string>
#include <fstream>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <uuid/uuid.h>
#include <cctype>
using namespace std;
template <typename T> void delete_arr_if_not_null(T * a){
    if (a != NULL){
        delete [] a;
    }
}

namespace commutil {
    namespace const_data {
        extern int uuid_str_length;
    }
    inline QString generate_uuid(){
        uuid_t uuid;
        const int uuid_length = 3;
        const int byte_length = 5;
        uuid_generate(reinterpret_cast<unsigned char *>(&uuid));
        char tmp_str[byte_length];
        memset(tmp_str, 0, sizeof(tmp_str));
        QString res("s_");
        for(int i = 0; i < uuid_length;++i){
            sprintf(tmp_str,"%x",uuid[i]);
            res.append(tmp_str);
        }
        return res;
    }
    inline bool check_digit(const char * str){
        int l = strlen(str);
        for(int i = 0; i < l; ++i){
            if(!isdigit(str[i]))
                return false;
        }
        return true;
    }
    inline bool check_str(const char * str){
        int l = strlen(str);
        char forbid_chars[1] = {'/'};
        int forbid_chars_len = 1;
        for(int i = 0; i < l; ++i){
            for(int j = 0; j < forbid_chars_len; ++j){
                if(forbid_chars[j] == str[i])
                    return false;
            }
        }
        return true;
    }
    void print();
}

#endif
