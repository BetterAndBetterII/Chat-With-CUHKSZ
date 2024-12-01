import urllib3.contrib.pyopenssl

from utils.utils import *
from utils.utils import get_legacy_session


class LibraryLogin(Login):
    def login(self, username, password) -> Session:
        urllib3.contrib.pyopenssl.inject_into_urllib3()
        _session = get_legacy_session()
        print("Login successfully!")
        return _session


class LibrarySearch:
    def __init__(self, login_session: Login):
        self.login = login_session
    def search(self, keyword):
        # acTriggered=false&blendFacetsSeparately=false&citationTrailFilterByAvailability=true&disableCache=false&getMore=0&inst=86CUHKSZ_INST&isCDSearch=false&lang=en&limit=10&newspapersActive=false&newspapersSearch=false&offset=0&otbRanking=false&pcAvailability=false&q=any,contains,chinese&qExclude=&qInclude=&rapido=false&refEntryActive=false&rtaLinks=true&scope=MyInst_and_CI&searchInFulltextUserSelection=false&skipDelivery=Y&sort=rank&tab=Everything&vid=86CUHKSZ_INST:86CUHKSZ
        param = {
            'acTriggered': 'false',
            'blendFacetsSeparately': 'false',
            'citationTrailFilterByAvailability': 'true',
            'disableCache': 'false',
            'getMore': '0',
            'inst': '86CUHKSZ_INST',
            'isCDSearch': 'false',
            'lang': 'en',
            'limit': '10',
            'newspapersActive': 'false',
            'newspapersSearch': 'false',
            'offset': '0',
            'otbRanking': 'false',
            'pcAvailability': 'false',
            'q': f'any,contains,{keyword}',
            'qExclude': '',
            'qInclude': '',
            'rapido': 'false',
            'refEntryActive': 'false',
            'rtaLinks': 'true',
            'scope': 'MyInst_and_CI',
            'searchInFulltextUserSelection': 'false',
            'skipDelivery': 'Y',
            'sort': 'rank',
            'tab': 'Everything',
            'vid': '86CUHKSZ_INST:86CUHKSZ'
        }
        url = 'https://cuhksz.primo.exlibrisgroup.com.cn/primaws/rest/pub/pnxs'
        response = self.login.get(url, params=param)
        json_res = response.json()
        docs_list = json_res['docs']
        results = list()
        for doc in docs_list:
            info = {
                'title': doc['pnx']['display']['title'][0],
                'publisher': doc['pnx']['display']['publisher'][0],
                'type': doc['pnx']['display']['type'][0],
            }
            if 'creator' in doc['pnx']['display']:
                info['creator'] = doc['pnx']['display']['creator'][0]
            if 'subject' in doc['pnx']['display']:
                info['subject'] = doc['pnx']['display']['subject'][0]
            results.append(info)
        return results


if __name__ == '__main__':
    session = LibraryLogin('', '')
    search = LibrarySearch(session)
    res = search.search('chinese')
    print(res)
