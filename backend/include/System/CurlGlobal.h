#ifndef CURLGLOBAL_H
#define CURLGLOBAL_H

#include <mutex>
#include <stdexcept>

/*
使用libcurl需要进行全局初始化`curl_global_init();`
利用CurlGlobal类来避免bb，sis，library等实例多次
初始化/清除的问题
*/

class CurlGlobal {
public:
    CurlGlobal();
    ~CurlGlobal();

private:
    inline static int init_count = 0; // 记录初始化的次数
    inline static std::mutex init_mutex; // 确保线程安全
};

#endif // CURLGLOBAL_H
