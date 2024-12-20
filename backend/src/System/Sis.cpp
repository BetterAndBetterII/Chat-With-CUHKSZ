//
// Created by Wen on 24-11-27.
//

#include "../../include/System/Sis.h"
#include <iostream> //std::cerr
#include <regex> //get_grades() course_parser()
#include <filesystem> //destructor
#include <random> //login()

using std::string;
using std::vector;
using std::cout;
using std::endl;

SisSystem::SisSystem(const string& username, const string& password) : System(username, password){
    this->command_list = {
        "get_schedule",
        "get_course",
        "get_term_grades"
    };
}

SisSystem::~SisSystem(){}

//login function implementation
bool SisSystem::login(){
    if(is_login){
        return true;
    }

    //尝试登录
    if(handle){

        CURLcode res;

        //忽略登录过程返回的响应体（注释下行可把响应体打印到终端）
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, ignore_calback);
        //打印详细输出
        //curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
        //curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, debug_callback);

        // 启用自动cookie处理，指定cookie文件
        cookiefile = username + "sisCookies.txt";
        curl_easy_setopt(handle, CURLOPT_COOKIEJAR,  cookiefile.c_str());  // 保存cookies
        curl_easy_setopt(handle, CURLOPT_COOKIEFILE, cookiefile.c_str()); // 发送保存的cookies

        //logout
        //curl_easy_setopt(handle, CURLOPT_URL, "https://sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/?cmd=logout");
        //curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);
        //curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        //curl_easy_perform(handle);
        //curl_easy_setopt(handle, CURLOPT_URL, "https://sts.cuhk.edu.cn/adfs/ls/?wa=wsignout1.0");
        //curl_easy_perform(handle);
        //curl_easy_setopt(handle, CURLOPT_URL, "https://sts.cuhk.edu.cn/adfs/oauth2/logout");
        //curl_easy_perform(handle);

        //向sts.cuhk.edu.cn发送登录请求(POST)
        string url = string("https://sts.cuhk.edu.cn/adfs/oauth2/authorize?")
            + "response_type=" + "code" 
            + "&client_id=" + "3f09a73c-33cf-49b8-8f0c-b79ea2f3e83b" 
            + "&redirect_uri=" + "https://sis.cuhk.edu.cn/sso/dologin.html"
            + "&client-request-id=" + "e4ad901b-ac83-4ace-8413-0040020000e8";

        string strdata ="UserName=cuhksz\\" + username + "&Kmsi=true&AuthMethod=FormsAuthentication&Password=" + password ; //POST data
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, strdata.c_str());
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str() );

        res = curl_easy_perform(handle);

        char* final_url;
        curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &final_url);
        string code = string(final_url).substr(string(final_url).find("code=")+5);
        
        url = string("https://sis.cuhk.edu.cn/psp/csprd/?")
            + "cmd=" + "login" 
            + "&languageCd=" + "ENG"
            + "&code=" + code;

        //生成随机字符串用于第二阶段的post
        // 创建一个随机数生成器，使用随机设备作为种子
        std::random_device rd;
        std::mt19937 gen(rd()); // Mersenne Twister 随机数生成器
    
        // 定义一个均匀分布，范围为 0 到 25（因为大写字母的索引范围是 0 到 25）
        std::uniform_int_distribution<> distrib(0, 25);
    
        // 生成一个由 10 个大写字母组成的字符串
        std::string randomstring;
        for (int i = 0; i < 10; ++i) {
            // 使用生成器随机选择一个字母的索引
            char letter = alphabet[distrib(gen)];
            randomstring += letter;  // 将字母添加到结果字符串中
        }

        strdata = 
                string("timezoneOffset=") + "-480" +
                "&ptmode=" +  "f" +
                "&ptlangcd=" + "ENG" +
                "&ptinstalledlang=" + "ENG,ZHT,ZHS" +
                "&userid=" + "CUSZ_SSO_LOGIN" +
                "&pwd=" + randomstring +
                "&ptlangsel=" + "ENG";
            
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, strdata.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(handle);

        // 手动添加 Cookie
        curl_easy_setopt(handle, CURLOPT_COOKIE, "PS_DEVICEFEATURES=width:1728 height:1152 pixelratio:1.25 touch:0 geolocation:1 websockets:1 webworkers:1 datepicker:1 dtpicker:1 timepicker:1 dnd:1 sessionstorage:1 localstorage:1 history:1 canvas:1 svg:1 postmessage:1 hc:0");
        url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/s/WEBLIB_PTBR.ISCRIPT1.FieldFormula.IScript_StartPage?&";
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(handle);

        curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &final_url);

        if(res != CURLE_OK){
            std::cerr << "Login failed because:" << curl_easy_strerror(res) << endl;
        }
        else if(string(final_url).find("sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/h/?tab=STUDENT_HOMEPAGE")!=string::npos ){
            is_login = true;
            cout << "Login successfully!" << endl;
            return true;
        }
        else{
            std::cerr << "Username or Password incorrect!" << endl;
            return false;
        }

    }

    std::cerr << "Failed to initialize handle." << endl;
    return false;

}

string SisSystem::get_schedule(){
    if (!login()) {
        return "Invalid username or password!";
    }

    string url = string("https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_SCHD_W.GBL?") +
    "FolderPath=PORTAL_ROOT_OBJECT.CO_EMPLOYEE_SELF_SERVICE.HCCC_ENROLLMENT.HC_SSR_SSENRL_SCHD_W_GBL"+
    "&IsFolder=false"+
    "&IgnoreParamTempl=FolderPath,IsFolder";

    string rawData = getRequest(url);

    //截取课表部分（否则xpath查询不到）
    size_t startpos = rawData.find("<table cellspacing='0' cellpadding='2' width='100%' class='PSLEVEL1GRIDNBO' id='WEEKLY_SCHED_HTMLAREA'>");
    if (startpos == std::string::npos) {
        std::cout << "Can not find class table! from page" << std::endl;
    }
    rawData = rawData.substr(startpos);
    size_t endpos = rawData.find("class='PSLEVEL3GRID'>&nbsp;</td></tr></table></div>");
    rawData = rawData.substr(0, endpos+45);
    rawData = "<html><head></head><body>"+rawData+"</body></html>";

    vector<vector<string>> table(16, vector<string>(8));
    table[0][0]="Time";table[0][1]="Monday";table[0][2]="Tuesday";table[0][3]="Wednesday";table[0][4]="Thursday";table[0][5]="Friday";table[0][6]="Saturday";table[0][7]="Sunday";
    
    for(int row = 2 ; row <= 16 ; row++){
        int item_counter = 0 ;
        if(row!=2){
            for(string item : table[row-1]){if(item!=""){item_counter++;}}
        }
        int table_index = 0;
        for(int col = 1 ; col <= 8-item_counter ; col++){
            std::stringstream xpath;
            xpath <<  "/html/body/table/tr[" << row << "]/td[" << col << "]/";
            while(table[row-1][table_index]!=""){table_index++;}
            //课程格
            if(xpathQuery(rawData, xpath.str()+"@class")[0]=="SSSWEEKLYBACKGROUND"){
                table[row-1][table_index]=xpathQuery(rawData, xpath.str()+"/span")[0];;
                if(xpathQuery(rawData, xpath.str()+"@rowspan")[0]=="2"){
                    table[row][table_index]="*";
                }
                if(xpathQuery(rawData, xpath.str()+"@rowspan")[0]=="3"){
                    table[row][table_index]="*";
                    table[row+1][table_index]="*";
                }
                if(xpathQuery(rawData, xpath.str()+"@rowspan")[0]=="4"){
                    table[row][table_index]="*";
                    table[row+1][table_index]="*";
                    table[row+2][table_index]="*";
                }
            }
            //时间格
            if(xpathQuery(rawData, xpath.str()+"@class")[0]=="SSSWEEKLYTIMEBACKGROUND"){
                table[row-1][table_index]=xpathQuery(rawData, xpath.str()+"/span")[0];
            }

            table_index+=1;            
            
        }

    }

    //返回string
    string final_result;
    for(int day=1 ; day<=7 ; day++){
        final_result+= table[0][day] + ":\n";
        for(int time_index = 1 ; time_index<=15 ; time_index++){
            if(table[time_index][day]!="*"&&table[time_index][day]!=""){
                final_result+=course_parser(table[time_index][day]);
            }
        } 
        final_result+="\n";
    }

    return final_result;
}

void SisSystem::printTable(const vector<vector<string>>& Vector)const{
    cout<< "[Table]" <<endl;
    string result;
    for(vector<string> rowvector : Vector){
        string rowstr="";
        for(string item : rowvector){
            if(item==""){
                //注意空值填入'None'
                rowstr+="None";
            }
            else{
                rowstr+=item;
            }
        }
        cout << rowstr << endl;

    }
}

string SisSystem::course_parser(const string& courseinfo)const{
    string result;
    std::regex pattern("^([A-Z]{3}\\s[0-9]{4}\\s-\\s[LT]\\d+)(Lecture|Tutorial)(\\d{1,2}:\\d{2}(AM|PM) - \\d{1,2}:\\d{2}(AM|PM))(.+)");
    std::smatch match;
    if (std::regex_search(courseinfo, match, pattern)) {
        //std::cout << "Matched" << std::endl;
        if (match.size() == 7) { // 有 6 个捕获组，match[0] 是整个匹配内容
            string course_name = match[1];
            string type = match[2];
            string time = match[3];
            string location = match[6];

            result += "Course: " + course_name + "\n" 
                + "Type: " + type + "\n"
                + "Duration: " + time + "\n"
                + "Location: " + location + "\n";
        }
    } else {
        std::cout << "No match found." << std::endl;
    }
    return result;
}

string SisSystem::parse_ICSID(const std::string& text)const{
    string ICSID = "";
    std::istringstream stream(text);
    string line;
    
    while (std::getline(stream, line)) {
        size_t pos = line.find("name='ICSID' id='ICSID'");
        if (pos != std::string::npos) {
            size_t valuePos = line.find("value='");
            if (valuePos != std::string::npos) {
                valuePos += 7; // move past "value='"
                size_t endPos = line.find("'", valuePos);
                if (endPos != std::string::npos) {
                    ICSID = line.substr(valuePos, endPos - valuePos);
                }
            }
        }
    }
    
    return ICSID;
}

string SisSystem::parse_ICStateNum(const std::string& text)const{
    string ICStateNum = "";
    std::istringstream stream(text);
    string line;
    
    while (std::getline(stream, line)) {
        size_t pos = line.find("name='ICStateNum' id='ICStateNum'");
        if (pos != std::string::npos) {
            size_t valuePos = line.find("value='");
            if (valuePos != std::string::npos) {
                valuePos += 7; // move past "value='"
                size_t endPos = line.find("'", valuePos);
                if (endPos != std::string::npos) {
                    ICStateNum = line.substr(valuePos, endPos - valuePos);
                }
            }
        }
    }

    return ICStateNum;

}

string SisSystem::get_course(string course_code, string term, string openOnly){

    if (!login()) {
        return "Invalid username or password!";
    }

    //get ICSID
    string url=string("https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.CLASS_SEARCH.GBL?") + 
        "PORTALPARAM_PTCNAV=HC_CLASS_SEARCH&EOPP.SCNode=HRMS&EOPP.SCPortal=EMPLOYEE&EOPP.SCName=HCCC_SS_CATALOG&EOPP.SCLabel=Class%20Search%20%2f%20Browse%20Catalog&EOPP.SCPTfname=HCCC_SS_CATALOG&FolderPath=PORTAL_ROOT_OBJECT.CO_EMPLOYEE_SELF_SERVICE.HCCC_SS_CATALOG.HC_CLASS_SEARCH&IsFolder=false&PortalActualURL=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2fEMPLOYEE%2fHRMS%2fc%2fSA_LEARNER_SERVICES.CLASS_SEARCH.GBL&PortalContentURL=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2fEMPLOYEE%2fHRMS%2fc%2fSA_LEARNER_SERVICES.CLASS_SEARCH.GBL&PortalContentProvider=HRMS&PortalCRefLabel=Class%20Search&PortalRegistryName=EMPLOYEE&PortalServletURI=https%3a%2f%2fsis.cuhk.edu.cn%2fpsp%2fcsprd%2f&PortalURI=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2f&PortalHostNode=HRMS&NoCrumbs=yes&PortalKeyStruct=yes";
    string response = getRequest(url);
    
    string ICSID = parse_ICSID(response);
    string ICStateNum = parse_ICStateNum(response);
    char* escaped_icsid = curl_easy_escape(handle, ICSID.c_str(), static_cast<int>(ICSID.length()));

    url=("https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.CLASS_SEARCH.GBL");
    string data = 
        string("ICAJAX=1") +
        "&ICNAVTYPEDROPDOWN=0" +
        "&ICType=Panel" +
        "&ICElementNum=0" +
        "&ICStateNum="+ ICStateNum + //提交表单序号
        "&ICAction=CLASS_SRCH_WRK2_SSR_PB_CLASS_SRCH" +
        "&ICXPos=0" +
        "&ICYPos=0" +
        "&ResponsetoDiffFrame=-1" +
        "&TargetFrameName=None" +
        "&FacetPath=None" +
        "&ICFocus=" +
        "&ICSaveWarningFilter=0" +
        "&ICChanged=-1" +
        "&ICAutoSave=0" +
        "&ICResubmit=0" +
        "&ICSID=" + escaped_icsid +
        "&ICActionPrompt=false" +
        "&ICTypeAheadID=" +
        "&ICBcDomData=undefined" +
        "&ICFind=" +
        "&ICAddCount=" +
        "&ICAPPCLSDATA=" +
        "&#ICDataLang=ENG" +
        "&DERIVED_SSTSNAV_SSTS_MAIN_GOTO$7$=9999" +
        "&CLASS_SRCH_WRK2_INSTITUTION$31$=CUSZ1" +
        "&CLASS_SRCH_WRK2_STRM$35$=" + term.substr(0, 4) +
        "&SSR_CLSRCH_WRK_SUBJECT$0=" + course_code.substr(0, 3) +
        "&SSR_CLSRCH_WRK_SSR_EXACT_MATCH1$1=E" +
        "&SSR_CLSRCH_WRK_CATALOG_NBR$1=" + course_code.substr(3, 4) +
        "&SSR_CLSRCH_WRK_ACAD_CAREER$2=" + term.substr(4, 2) +
        "&SSR_CLSRCH_WRK_SSR_OPEN_ONLY$chk$3="+ openOnly +
        "&SSR_CLSRCH_WRK_OEE_IND$chk$4=N" +
        "&DERIVED_SSTSNAV_SSTS_MAIN_GOTO$8$=9999" +
        "&ptus_defaultlocalnode=PSFT_HR" +
        "&ptus_dbname=CSPRD" +
        "&ptus_portal=EMPLOYEE" +
        "&ptus_node=HRMS" +
        "&ptus_workcenterid=" +
        "&ptus_componenturl=https://sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.CLASS_SEARCH.GBL";

    string rawData = postRequest(url, data);
    curl_free(escaped_icsid);

    vector time = xpathQuery(rawData, "//*[@id[starts-with(.,'MTG_DAYTIME$')]]");
    vector location = xpathQuery(rawData, "//*[@id[starts-with(.,'MTG_ROOM$')]]");
    vector instructor = xpathQuery(rawData, "//span[@id[starts-with(.,'MTG_INSTR$')]]");
    vector duration = xpathQuery(rawData, "//*[@id[starts-with(.,'MTG_TOPIC$')]]");
    vector section = xpathQuery(rawData, "//a[@id[starts-with(.,'DERIVED_CLSRCH_SSR_CLASSNAME_LONG$')]]");
    vector enrollment_total = xpathQuery(rawData, "//*[@id[starts-with(.,'SSR_CLS_DTL_WRK_ENRL_TOT$')]]");
    vector capacity = xpathQuery(rawData, "//*[@id[starts-with(.,'SSR_CLS_DTL_WRK_ENRL_CAP$')]]");

    std::stringstream final_result ;
    
    string openstatus;
    if(openOnly=="Y"){openstatus="Yes";}
    else{openstatus="No";}
    final_result << "Search Result: " "(coursecode: "<< course_code << ")(Term: " << term << ")(Open course only: " << openstatus << ")\n\n";
    
    for(size_t i = 0 ; i < instructor.size() ; i++ ){
        final_result<< "Section: " << section[i] <<"\n";
        final_result<< "Enrollment Total/Enrollment Capacity: " << enrollment_total[i] << "/" << capacity[i] << "\n";
        final_result<< "Days&Times: " << time[i] <<"\n";
        final_result<< "Instructor: " << instructor[i] << "\n";
        final_result<< "Location: " << location[i] <<"\n";
        final_result<< "MeetingDates: " << duration[i] <<"\n";
        final_result<<"\n";

    }

    return final_result.str();
}

string SisSystem::get_grades(string term){

    if (!login()) {
        return "Invalid username or password!";
    }

    //get ICSID
    string url=string("https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_GRADE.GBL?") + 
        "FolderPath=PORTAL_ROOT_OBJECT.CO_EMPLOYEE_SELF_SERVICE.HCCC_ENROLLMENT.HC_SSR_SSENRL_GRADE_GBL&IsFolder=false&IgnoreParamTempl=FolderPath%2cIsFolder&PortalActualURL=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2fEMPLOYEE%2fHRMS%2fc%2fSA_LEARNER_SERVICES.SSR_SSENRL_GRADE.GBL&PortalContentURL=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2fEMPLOYEE%2fHRMS%2fc%2fSA_LEARNER_SERVICES.SSR_SSENRL_GRADE.GBL&PortalContentProvider=HRMS&PortalCRefLabel=View%20My%20Grades&PortalRegistryName=EMPLOYEE&PortalServletURI=https%3a%2f%2fsis.cuhk.edu.cn%2fpsp%2fcsprd%2f&PortalURI=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2f&PortalHostNode=HRMS&NoCrumbs=yes&PortalKeyStruct=yes";
    string response = getRequest(url);
    
    string ICSID = parse_ICSID(response);
    string ICStateNum = parse_ICStateNum(response);
    char* escaped_icsid = curl_easy_escape(handle, ICSID.c_str(), static_cast<int>(ICSID.length()));

    vector termlist = xpathQuery(response, "//span[@id[starts-with(.,'TERM_CAR$')]]");

    // 使用 std::find 查找 term 是否存在
    auto it = std::find(termlist.begin(), termlist.end(), term);

    if (it == termlist.end()) {
        return term + " not found.\n";
    }

    std::map<string, size_t> term_index;
    for(size_t i = 0 ; i< termlist.size() ; i++){
        term_index[termlist[i]]=i;
    }

    url=("https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_GRADE.GBL");
    string data = 
        string("ICAJAX=1") +
        "&ICNAVTYPEDROPDOWN=0" +
        "&ICType=Panel" +
        "&ICElementNum=0" +
        "&ICStateNum=" + ICStateNum +
        "&ICAction=DERIVED_SSS_SCT_SSR_PB_GO" +
        "&ICXPos=0" +
        "&ICYPos=0" +
        "&ResponsetoDiffFrame=-1" +
        "&TargetFrameName=None" +
        "&FacetPath=None" +
        "&ICFocus=" +
        "&ICSaveWarningFilter=0" +
        "&ICChanged=-1" +
        "&ICAutoSave=0" +
        "&ICResubmit=0" +
        "&ICSID=" + escaped_icsid +
        "&ICActionPrompt=false" +
        "&ICTypeAheadID=" +
        "&ICBcDomData=undefined" +
        "&ICFind=" +
        "&ICAddCount=" +
        "&ICAPPCLSDATA=" +
        "&#ICDataLang=ENG" +
        "&DERIVED_SSTSNAV_SSTS_MAIN_GOTO$7$=9999" +
        "&SSR_DUMMY_RECV1$sels$3$$0=" + std::to_string(term_index[term]) +
        "&DERIVED_SSTSNAV_SSTS_MAIN_GOTO$8$=9999" +
        "&ptus_defaultlocalnode=PSFT_HR" +
        "&ptus_dbname=CSPRD" +
        "&ptus_portal=EMPLOYEE" +
        "&ptus_node=HRMS" +
        "&ptus_workcenterid=" +
        "&ptus_componenturl=https://sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_GRADE.GBL" ;

    string rawData = postRequest(url, data);

    curl_free(escaped_icsid);

    vector coursename = xpathQuery(rawData, "//a[@id[starts-with(.,'CLS_LINK$')]]");
    vector units = xpathQuery(rawData, "//*[@id[starts-with(.,'STDNT_ENRL_SSV1_UNT_TAKEN$')]]");
    vector gradings = xpathQuery(rawData, "//span[@id[starts-with(.,'GRADING_BASIS$')]]");
    vector grades = xpathQuery(rawData, "//*[@id[starts-with(.,'STDNT_ENRL_SSV1_CRSE_GRADE_OFF$')]]");
    vector grade_points = xpathQuery(rawData, "//*[@id[starts-with(.,'STDNT_ENRL_SSV1_GRADE_POINTS$')]]");
    string gpa = xpathQuery(rawData, "//*[@id='STATS_CUMS$12']")[0];

    std::stringstream final_result ;
    
    final_result << "Term: " << term << "\n\n" ;
    
    for(size_t i = 0 ; i < coursename.size() ; i++ ){
        final_result << coursename[i] <<"\n";
        final_result<< "Units: " << units[i] << " Grading: " << gradings[i] << "\n";
        final_result<< "Grade: " << grades[i] <<" Grade_points: " << grade_points[i] << "\n";
        final_result<<"\n";
    }

    final_result << "Cumulative GPA: " << gpa << "\n";

    return final_result.str();

}