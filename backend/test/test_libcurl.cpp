#include <iostream>
#include <curl/curl.h>

//    //WRITEFUNCTION用于处理返回数据
//    static size_t cb(char *data, size_t size, size_t nmemb, void *clientp)
//    {
//    size_t realsize = size * nmemb;
//    struct memory *mem = (struct memory *)clientp;
//    
//    char *ptr = realloc(mem->response, mem->size + realsize + 1);
//    if(!ptr)
//        return 0;  /* out of memory */
//    
//    mem->response = ptr;
//    memcpy(&(mem->response[mem->size]), data, realsize);
//    mem->size += realsize;
//    mem->response[mem->size] = 0;
//    
//    return realsize;
//    } 


void test_libcurl(){
    std::cout << "=====Test libcurl=====" << std::endl;
    std::string username = "";
    std::string password = "";
    std::cout << "Username:" << std::endl;
    std::cin >> username;
    std::cout << "Password:" << std::endl;
    std::cin >> password;

    //检查libcurl版本信息
    curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
    std::cout << "libcurl 版本: " << info->version << std::endl;

    // 检查是否支持 SSL
    if(info->features & CURL_VERSION_SSL) {
        std::cout << "支持 SSL: 是" << std::endl;
    } else {
        std::cout << "支持 SSL: 否" << std::endl;
    }

    // 检查是否支持 HTTP/2
    if(info->features & CURL_VERSION_HTTP2) {
        std::cout << "支持 HTTP/2: 是" << std::endl;
    } else {
        std::cout << "支持 HTTP/2: 否" << std::endl;
    }

    // 检查是否支持 HTTP3
    if(info->features & CURL_VERSION_HTTP3) {
        std::cout << "支持 HTTP/3: 是" << std::endl;
    } else {
        std::cout << "支持 HTTP/3: 否" << std::endl;
    }  

    //初始化curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init(); //初始化curl(handle)

    if(curl){
        CURLcode res;

        //设置返回数据的处理方法（直接打印到output.txt)
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L); //将响应头一并打印到output.txt(仅为调试方便，后期改用)
        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb); //自定义处理返回数据的方式
        FILE *logininfo = fopen("logininfo.txt", "w");
        FILE *output = fopen("annoucement.html", "w");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, logininfo);

        std::string strdata ="UserName=cuhksz\\" + username + "&Kmsi=true&AuthMethod=FormsAuthentication&Password=" + password ; //POST data
        const char* data = strdata.c_str();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        // 启用自动cookie处理，指定cookie文件
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");  // 保存cookies
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt"); // 发送保存的cookies
        curl_easy_setopt(curl, CURLOPT_URL, "https://sts.cuhk.edu.cn/adfs/oauth2/authorize?response_type=code&client_id=4b71b947-7b0d-4611-b47e-0ec37aabfd5e&redirect_uri=https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode&client-request-id=dd5ffbba-e761-453e-e58f-014001000089");

        res = curl_easy_perform(curl);  //在未定义CURLOPT_WRITEDATA时该语句默认自动打印Responce

        //尝试POST获取annoucement
        curl_easy_setopt(curl, CURLOPT_URL, "https://bb.cuhk.edu.cn/webapps/blackboard/execute/announcement?method=search&context=mybb&handle=my_announcements&returnUrl=/webapps/portal/execute/tabs/tabAction?tab_tab_group_id=_1_1&tabId=_1_1&forwardUrl=index.jsp");
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L); 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);

        res = curl_easy_perform(curl);  //post请求anouncement网页

        //关闭文件和curl_easy
        fclose(logininfo);
        fclose(output);
        curl_easy_cleanup(curl);

    }

    curl_global_cleanup();

}