import requests
import urllib3.contrib.pyopenssl
from requests import Session

from utils.utils import ValidationError, Login

"""
OAuth:
1. 向sts.cuhk.edu.cn发送登录请求，输入用户名和密码页面（response_type, client_id, redirect_uri）(GET)
2. 输入页面向sts.cuhk.edu.cn发送登录请求，获取code（response_type, client_id, redirect_uri, client-request-id）
   （表单数据：UserName, Password, Kmsi, AuthMethod）(POST)
   响应：302重定向到sts.cuhk.edu.cn，但GET方法
   set-cookie有效
3. 重定向到sts.cuhk.edu.cn，获取code（GET）
   响应：302重定向到https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode?code=...
   set-cookie有效
4. 重定向到https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode?code=...
   验证成功，重定向到https://bb.cuhk.edu.cn/webapps/portal/execute/defaultTab
5. 此时已经登录成功，可以访问https://bb.cuhk.edu.cn/webapps/portal/execute/defaultTab
"""


class BBLogin(Login):
    def login(self, username, password) -> Session:
        urllib3.contrib.pyopenssl.inject_into_urllib3()
        _session = requests.Session()

        def stage1(_session: Session):
            response_type = "code"
            client_id = "4b71b947-7b0d-4611-b47e-0ec37aabfd5e"
            redirect_uri = "https://bb.cuhk.edu.cn/webapps/bb-SSOIntegrationOAuth2-BBLEARN/authValidate/getCode"
            client_request_id = "b956ea95-440d-4aa8-88c0-0040020000bb"
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
            # print(r.url)
            if not ("bb.cuhk.edu.cn:443" in r.url):
                raise ValidationError("Username or password incorrect!")

        stage1(_session)
        print("Login successfully!")
        return _session
