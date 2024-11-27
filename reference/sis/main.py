import os
import random
import string
import time
from datetime import datetime

import pandas as pd
import requests
import urllib3
from requests import Session
import urllib3.contrib.pyopenssl
from lxml import html as hp
from lxml import etree

# 完整打印
pd.set_option('display.max_columns', None)
pd.set_option('display.max_rows', None)
pd.set_option('display.width', None)
# 单元格折行
pd.set_option('display.expand_frame_repr', False)


class ValidationError(Exception):
    def __init__(self, message):
        self.message = message

    def __str__(self):
        return self.message


class Login:
    _session: Session
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) '
                      'Chrome/79.0.3945.88 Safari/537.36',
        'Connection': 'close'
    }

    def __init__(self, username, password):
        self._session = self.login(username, password)

    def get_session(self) -> Session:
        return self._session

    def login(self, username, password) -> Session:
        pass

    def get(self, url, **kwargs):
        return self._session.get(url, headers=self.headers, **kwargs)

    def post(self, url, **kwargs):
        return self._session.post(url, headers=self.headers, **kwargs)

    @property
    def session(self):
        return self._session


class SISLogin(Login):
    def login(self, username, password) -> Session:
        urllib3.contrib.pyopenssl.inject_into_urllib3()
        _session = requests.Session()

        def stage1(_session: Session):
            response_type = "code"
            client_id = "3f09a73c-33cf-49b8-8f0c-b79ea2f3e83b"
            redirect_uri = "https://sis.cuhk.edu.cn/sso/dologin.html"
            client_request_id = "e4ad901b-ac83-4ace-8413-0040020000e8"
            params = {
                "response_type": response_type,
                "client_id": client_id,
                "redirect_uri": redirect_uri,
                "client-request-id": client_request_id
            }
            data = {
                "UserName": "cuhksz\\" + username,
                "Password": password,
                "Kmsi": "true",
                "AuthMethod": "FormsAuthentication"
            }
            url = "https://sts.cuhk.edu.cn/adfs/oauth2/authorize"
            r = _session.post(url, headers=self.headers, params=params, data=data, allow_redirects=True)
            parse_params = r.url.split("?")[1]
            param_dict = dict([param.split("=") for param in parse_params.split("&")])
            code = param_dict["code"]
            params = {
                "cmd": 'login',
                "languageCd": "ENG",
                "code": code,
            }
            # timezoneOffset=-480&ptmode=f&ptlangcd=ENG&ptinstalledlang=ENG%2CZHT%2CZHS&userid=CUSZ_SSO_LOGIN&pwd=YYYLOADKYN&ptlangsel=ENG
            data = {
                "timezoneOffset": "-480",
                "ptmode": "f",
                "ptlangcd": "ENG",
                "ptinstalledlang": "ENG,ZHT,ZHS",
                "userid": "CUSZ_SSO_LOGIN",
                "pwd": ''.join(random.choices(string.ascii_uppercase, k=10)),
                "ptlangsel": "ENG"
            }
            url = "https://sis.cuhk.edu.cn/psp/csprd/"
            r = _session.post(url, headers=self.headers, params=params, data=data, allow_redirects=True)
            # PS_DEVICEFEATURES=width:1728 height:1152 pixelratio:1.25 touch:0 geolocation:1 websockets:1 webworkers:1 datepicker:1 dtpicker:1 timepicker:1 dnd:1 sessionstorage:1 localstorage:1 history:1 canvas:1 svg:1 postmessage:1 hc:0
            cookies = r.cookies
            cookies.set("PS_DEVICEFEATURES",
                        "width:1728 height:1152 pixelratio:1.25 touch:0 geolocation:1 websockets:1 webworkers:1 datepicker:1 dtpicker:1 timepicker:1 dnd:1 sessionstorage:1 localstorage:1 history:1 canvas:1 svg:1 postmessage:1 hc:0")
            start_url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/s/WEBLIB_PTBR.ISCRIPT1.FieldFormula.IScript_StartPage?&"
            _session.cookies = cookies
            r = _session.get(start_url, headers=self.headers, allow_redirects=True)
            # print(r.url)
            if not ('sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/h/?tab=STUDENT_HOMEPAGE' in r.url):
                raise ValidationError("Username or password incorrect!")
        _session.get('https://sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/?cmd=logout', allow_redirects=True)
        _session.get('https://sts.cuhk.edu.cn/adfs/ls/?wa=wsignout1.0', allow_redirects=True)
        _session.get('https://sts.cuhk.edu.cn/adfs/oauth2/logout', allow_redirects=True)
        stage1(_session)
        print("Login successfully!")
        return _session


def parse_ICSID(text):
    ICSID = ""
    for line in text.split("\n"):
        if "name='ICSID' id='ICSID'" in line:
            ICSID = line.split("value='")[1].split("'")[0]
    return ICSID


def parse_courses_table(text, table_xpath):
    html = hp.fromstring(text)
    table = html.xpath(table_xpath)[0]
    # title
    title = table.xpath(".//td[@class='PSLEVEL3GRIDLABEL']")[0]
    # header
    header = table.xpath(".//th[@class='PSLEVEL3GRIDCOLUMNHDR']")
    header = [th.text if th.text is not None else th[0].text for th in header]
    # body
    body = table.xpath(".//td[@class='PSLEVEL3GRIDWBO']")
    rows = []

    def parse_element(td):
        if td.xpath(".//img"):
            if td.xpath(".//img")[0].attrib['alt'] == 'Validate Result':
                if "ERROR" in td.xpath(".//img")[0].attrib['src']:
                    return "Error"
                elif "SUCCESS" in td.xpath(".//img")[0].attrib['src']:
                    return "Success"
            return td.xpath(".//img")[0].attrib['alt']
        elif td.xpath(".//a"):
            return td.xpath(".//a")[0].text if td.xpath(".//a")[0].text.strip() else ""
        elif td.xpath(".//span"):
            return td.xpath(".//span")[0].text if td.xpath(".//span")[0].text.strip() else ""
        elif td.text.strip():
            return td.text
        else:
            return ""

    for i in range(len(body) // len(header)):
        row = body[i * len(header):(i + 1) * len(header)]
        row = [parse_element(td) for td in row]
        rows.append(row)
    df = pd.DataFrame(rows, columns=header)
    print(df)
    return df


def parse_result_table(text, table_xpath):
    html = hp.fromstring(text)
    table = html.xpath(table_xpath)[0]
    # header
    header = table.xpath(".//th[@class='PSLEVEL1GRIDCOLUMNHDR']")
    header = [th.text if th.text is not None else th[0].text for th in header]
    # body
    body = table.xpath(".//td[@class='PSLEVEL1GRIDODDROW']") + table.xpath(".//td[@class='PSLEVEL1GRIDEVENROW']")
    rows = []

    def parse_element(td):
        if td.xpath(".//img"):
            if td.xpath(".//img")[0].attrib['alt'] == 'Validate Result':
                if "ERROR" in td.xpath(".//img")[0].attrib['src']:
                    return "Error"
                elif "SUCCESS" in td.xpath(".//img")[0].attrib['src']:
                    return "Success"
            return td.xpath(".//img")[0].attrib['alt']
        elif td.xpath(".//a"):
            return td.xpath(".//a")[0].text if td.xpath(".//a")[0].text.strip() else ""
        elif td.xpath(".//span"):
            return td.xpath(".//span")[0].text if td.xpath(".//span")[0].text.strip() else ""
        elif td.xpath(".//div"):
            return td.xpath(".//div")[0].text if td.xpath(".//div")[0].text and td.xpath(".//div")[
                0].text.strip() else ''.join(td.xpath(".//div")[0].itertext()).strip()
        elif td.text.strip():
            return td.text
        else:
            return ""

    for i in range(len(body) // len(header)):
        row = body[i * len(header):(i + 1) * len(header)]
        row = [parse_element(td) for td in row]
        rows.append(row)
    df = pd.DataFrame(rows, columns=header)
    print(df)
    return df


def verify_code():
    # validate
    now_timestamp = int(datetime.now().timestamp() * 1000)
    randval = str(random.randint(10000, 99999))
    url = f"https://sis.cuhk.edu.cn/vcode.png?randval={randval}&bogus={now_timestamp}"
    code_pic = login.get(url, allow_redirects=True)
    with open("code.png", "wb") as f:
        f.write(code_pic.content)
    # https://sis.cuhk.edu.cn/ValidationServlet?HK_InputValue=67463&bogus=1725518579841
    params = {
        "HK_InputValue": "",
        "bogus": str(now_timestamp)
    }
    url = "https://sis.cuhk.edu.cn/ValidationServlet"
    val_res = None
    for i in range(len(randval)):
        wait_time = i * 100 + random.randint(0, 100)
        params["HK_InputValue"] = randval[:i + 1]
        params["bogus"] = str(now_timestamp + wait_time)
        time.sleep((wait_time - i * 100) / 1000)
        val_res = login.get(url, params=params, allow_redirects=True)  # 验证验证码
    if val_res and "Y" not in val_res.text:
        raise ValidationError("验证码错误")
    else:
        print("验证码正确")
    return randval


def parse_request_id(text):
    target = 'document.location'
    for line in text.split("\n"):
        if target in line:
            # 0001299832
            return line.split("ENRL_REQUEST_ID=")[1][:10]


if __name__ == '__main__':
    student_num = "123090873"
    password = "Ds-anying123"
    start_time = "2024-04-04 12:00"
    wait_seconds = 0.1
    env = False
    try:
        if env:
            student_num = os.getenv("STUDENT_NUM")
            password = os.getenv("PASSWORD")
    except KeyError:
        student_num = input("Please input your student number: ")
        password = input("Please input your password: ")
        os.environ["STUDENT_NUM"] = student_num
        os.environ["PASSWORD"] = password
    term = "2410"
    login = SISLogin(student_num, password)
    # url = 'https://sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/h/?tab=STUDENT_HOMEPAGE'
    # res = login.get(url, allow_redirects=True)  # 登录主页
    # print(res)
    # ?PORTALPARAM_PTCNAV=HC_SSR_SSENRL_CART_GBL&EOPP.SCNode=HRMS&EOPP.SCPortal=EMPLOYEE&EOPP.SCName=HCCC_ENROLLMENT&EOPP.SCLabel=Enrollment&EOPP.SCPTfname=HCCC_ENROLLMENT&FolderPath=PORTAL_ROOT_OBJECT.CO_EMPLOYEE_SELF_SERVICE.HCCC_ENROLLMENT.HC_SSR_SSENRL_CART_GBL&IsFolder=false&PortalActualURL=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2fEMPLOYEE%2fHRMS%2fc%2fSA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL&PortalContentURL=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2fEMPLOYEE%2fHRMS%2fc%2fSA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL&PortalContentProvider=HRMS&PortalCRefLabel=Enrollment%3a%20%20Add%20Classes&PortalRegistryName=EMPLOYEE&PortalServletURI=https%3a%2f%2fsis.cuhk.edu.cn%2fpsp%2fcsprd%2f&PortalURI=https%3a%2f%2fsis.cuhk.edu.cn%2fpsc%2fcsprd%2f&PortalHostNode=HRMS&NoCrumbs=yes&PortalKeyStruct=yes"
    params = {
        "PORTALPARAM_PTCNAV": "HC_SSR_SSENRL_CART_GBL",
        "EOPP.SCNode": "HRMS",
        "EOPP.SCPortal": "EMPLOYEE",
        "EOPP.SCName": "HCCC_ENROLLMENT",
        "EOPP.SCLabel": "Enrollment",
        "EOPP.SCPTfname": "HCCC_ENROLLMENT",
        "FolderPath": "PORTAL_ROOT_OBJECT.CO_EMPLOYEE_SELF_SERVICE.HCCC_ENROLLMENT.HC_SSR_SSENRL_CART_GBL",
        "IsFolder": "false",
        "PortalActualURL": "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL",
        "PortalContentURL": "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL",
        "PortalContentProvider": "HRMS",
        "PortalCRefLabel": "Enrollment:  Add Classes",
        "PortalRegistryName": "EMPLOYEE",
        "PortalServletURI": "https://sis.cuhk.edu.cn/psp/csprd/",
        "PortalURI": "https://sis.cuhk.edu.cn/psc/csprd/",
        "PortalHostNode": "HRMS",
        "NoCrumbs": "yes",
        "PortalKeyStruct": "yes"
    }
    url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL"
    res = login.get(url, params=params, allow_redirects=True)  # 进入选课页面
    ICSID = parse_ICSID(res.text)

    data = {
        "ICAJAX": "1",
        "ICNAVTYPEDROPDOWN": "0",
        "ICType": "Panel",
        "ICElementNum": "0",
        "ICStateNum": "1",
        "ICAction": "DERIVED_SSS_SCT_SSR_PB_GO",
        "ICXPos": "0",
        "ICYPos": "0",
        "ResponsetoDiffFrame": "-1",
        "TargetFrameName": "None",
        "FacetPath": "None",
        "ICFocus": "",
        "ICSaveWarningFilter": "0",
        "ICChanged": "-1",
        "ICAutoSave": "0",
        "ICResubmit": "0",
        "ICSID": ICSID,
        "ICActionPrompt": "false",
        "ICTypeAheadID": "",
        "ICBcDomData": "undefined",
        "ICFind": "",
        "ICAddCount": "",
        "ICAPPCLSDATA": "",
        "#ICDataLang": "ENG",
        "DERIVED_SSTSNAV_SSTS_MAIN_GOTO$7$": "9999",
        "SSR_DUMMY_RECV1$sels$3$$0": "3",
        "DERIVED_SSTSNAV_SSTS_MAIN_GOTO$8$": "9999",
        "ptus_defaultlocalnode": "PSFT_HR",
        "ptus_dbname": "CSPRD",
        "ptus_portal": "EMPLOYEE",
        "ptus_node": "HRMS",
        "ptus_workcenterid": "",
        "ptus_componenturl": "https://sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL"
    }
    url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL"
    res = login.post(url, data=data, allow_redirects=True)  # 加载购物车

    params = {
        "Page": "SSR_SSENRL_CART",
        "Action": "A",
        "ACAD_CAREER": "UG",
        "EMPLID": student_num,
        "INSTITUTION": "CUSZ1",
        "STRM": term,
        "TargetFrameName": "None"
    }
    url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL"
    res = login.get(url, params=params, allow_redirects=True)  # 加载选课页面
    parse_courses_table(res.text, '//*[@id="SSR_REGFORM_VW$scroll$0"]')
    # ICSID = parse_ICSID(res.text)

    randval = verify_code()
    print('睡眠20s')
    time.sleep(20)
    if start_time:
        while datetime.now().timestamp() < datetime.strptime(start_time, "%Y-%m-%d %H:%M").timestamp():
            pass
        time.sleep(wait_seconds)
    print('开始选课')
    start_time = datetime.now()

    data = {
        "ICAJAX": "1",
        "ICNAVTYPEDROPDOWN": "0",
        "ICType": "Panel",
        "ICElementNum": "0",
        "ICStateNum": "3",
        "ICAction": "DERIVED_REGFRM1_LINK_ADD_ENRL$82$",
        "ICXPos": "0",
        "ICYPos": "0",
        "ResponsetoDiffFrame": "-1",
        "TargetFrameName": "None",
        "FacetPath": "None",
        "ICFocus": "",
        "ICSaveWarningFilter": "0",
        "ICChanged": "-1",
        "ICAutoSave": "0",
        "ICResubmit": "0",
        "ICSID": ICSID,
        "ICActionPrompt": "false",
        "ICTypeAheadID": "",
        "ICBcDomData": "undefined",
        "ICFind": "",
        "ICAddCount": "",
        "ICAPPCLSDATA": "",
        "#ICDataLang": "ENG",
        "DERIVED_SSTSNAV_SSTS_MAIN_GOTO$7$": "9999",
        "DERIVED_REGFRM1_CLASS_NBR": "",
        "DERIVED_REGFRM1_SSR_CLS_SRCH_TYPE$249$": "06",
        "HK_AudioFlag": "N",
        "HK_ImgFlag": "Y",
        "hk_value": "Y\n",
        "inputValue": randval,
        "DERIVED_SSTSNAV_SSTS_MAIN_GOTO$8$": "9999",
        "ptus_defaultlocalnode": "PSFT_HR",
        "ptus_dbname": "CSPRD",
        "ptus_portal": "EMPLOYEE",
        "ptus_node": "HRMS",
        "ptus_workcenterid": "",
        "ptus_componenturl": "https://sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL"
    }
    url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_CART.GBL"
    res = login.post(url, data=data, allow_redirects=True)  # Step 1，确认购物车+验证码
    print('购物车确认成功' if res.status_code == 200 else '购物车确认失败')

    request_id = parse_request_id(res.text)
    if request_id is None:
        raise ValidationError("Request ID not found")

    params = {
        "Page": "SSR_SSENRL_ADD_C",
        "Action": "U",
        "ACAD_CAREER": "UG",
        "EMPLID": student_num,
        "ENRL_REQUEST_ID": request_id,
        "INSTITUTION": "CUSZ1",
        "STRM": term,
        "TargetFrameName": "None"
    }
    # add course info
    url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_ADD.GBL"
    res = login.get(url, params=params, allow_redirects=True)  # Step 2，添加课程信息（不等待响应直接进行下一步）
    if 'too frequent' in res.text or 'no longer available' in res.text:
        print('选课失败')
    print('添加课程信息成功' if res.status_code == 200 else '添加课程信息失败')

    data = {
        "ICAJAX": "1",
        "ICNAVTYPEDROPDOWN": "0",
        "ICType": "Panel",
        "ICElementNum": "0",
        "ICStateNum": "5",
        "ICAction": "DERIVED_REGFRM1_SSR_PB_SUBMIT",
        "ICXPos": "0",
        "ICYPos": "0",
        "ResponsetoDiffFrame": "-1",
        "TargetFrameName": "None",
        "FacetPath": "None",
        "ICFocus": "",
        "ICSaveWarningFilter": "0",
        "ICChanged": "-1",
        "ICAutoSave": "0",
        "ICResubmit": "0",
        "ICSID": ICSID,
        "ICActionPrompt": "false",
        "ICTypeAheadID": "",
        "ICBcDomData": "undefined",
        "ICFind": "",
        "ICAddCount": "",
        "ICAPPCLSDATA": "",
        "#ICDataLang": "ENG",
        "DERIVED_SSTSNAV_SSTS_MAIN_GOTO$7$": "9999",
        "DERIVED_SSTSNAV_SSTS_MAIN_GOTO$8$": "9999",
        "ptus_defaultlocalnode": "PSFT_HR",
        "ptus_dbname": "CSPRD",
        "ptus_portal": "EMPLOYEE",
        "ptus_node": "HRMS",
        "ptus_workcenterid": "",
        "ptus_componenturl": "https://sis.cuhk.edu.cn/psp/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_ADD.GBL"
    }
    url = "https://sis.cuhk.edu.cn/psc/csprd/EMPLOYEE/HRMS/c/SA_LEARNER_SERVICES.SSR_SSENRL_ADD.GBL"
    # time.sleep(10)
    res = login.post(url, data=data, allow_redirects=True)  # Step 4，提交选课
    xml = etree.XML(res.text.encode('utf-8'))
    if len(xml.xpath('//FIELD')) == 1:
        if 'too frequent' in ''.join(xml.itertext()) or 'no longer available' in ''.join(xml.itertext()):
            print(f"选课失败{'，操作过于频繁' if 'too frequent' in ''.join(xml.itertext()) else ''}")
            print(request_id)
            exit()
    df = parse_result_table(xml.xpath('//FIELD')[3].text, '//*[@id="SSR_SS_ERD_ER$scroll$0"]')
    all_status = df['Status'].unique()
    print('选课成功' if not 'Error' in all_status else '选课失败')
    print(f"耗时：{(datetime.now() - start_time).total_seconds()}s")
    print(request_id)
