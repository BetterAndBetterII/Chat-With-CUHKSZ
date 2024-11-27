import requests
import urllib3.contrib.pyopenssl
from requests import Session


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


class BookingLogin(Login):
    def login(self, username, password) -> Session:
        urllib3.contrib.pyopenssl.inject_into_urllib3()
        _session = requests.Session()

        def stage1(_session: Session):
            response_type = "code"
            client_id = "caf5aded-3f28-4b64-b836-4451312e1ea3"
            redirect_uri = "https://booking.cuhk.edu.cn/sso/code"
            client_request_id = "f8739f9e-124f-4096-8b34-0140020000bb"
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
            if not ("booking.cuhk.edu.cn" in r.url):
                raise ValidationError("Username or password incorrect!")

        stage1(_session)
        print("Login successfully!")
        return _session
