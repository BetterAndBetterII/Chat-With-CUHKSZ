import json

from lxml import etree

from booking.event import FieldEvent


class Booker:
    def __init__(self, telephone, reason, details, login):
        self.telephone = telephone
        self.reason = reason
        self.details = details
        self.login = login

    def book(self, field_event: FieldEvent, placeID, start_time, end_time):
        info = self._get_info(start_time, end_time, placeID)
        # "success":true
        data = {
            "id": info["id"],
            "user.id": info["userId"],
            "serialNo": "",
            "userOrgId": info["userOrgId"],
            "status": "",
            "approvalFlag": "0",
            "bizFieldBookField.id": info["bizFieldBookField.id"],
            "bizFieldBookField.FId": placeID,
            "bizFieldBookField.BId": info["bizFieldBookField.BId"],
            "bizFieldBookField.theme": self.reason,
            "submitTime": "",
            "isNewRecord": "true",
            "extend1": field_event.field_id,
            "extend2": "",
            "extend3": "",
            "extend4": "",
            "extend5": "",
            "userJob": "",
            "userGrp": "STUDENTS",
            "userMobile": "",
            "bizFieldBookField.extend3": "",
            "bizFieldBookField.extend4": "",
            "bizFieldBookField.extend5": "",
            "userTag": "Student",
            "bookType": "0",
            "fitBook": "false",
            "user.name": info["userName"],
            "userOrgName": info["userOrgName"],
            "userEmail": info["userEmail"],
            "userPhone": self.telephone,
            "theme": self.reason,
            "bizFieldBookField.startTime": start_time,
            "bizFieldBookField.endTime": end_time,
            "bizFieldBookField.joinNums": "2",
            "bizFieldBookField.needRep": "0",
            "bizFieldBookField.extend1": "0",
            "bizFieldBookField.useDesc": self.details,
        }
        url = "https://booking.cuhk.edu.cn/a/field/book/bizFieldBookMain/saveData?reBookMainId="
        r = self.login.post(url, data=data)
        if r.status_code != 200:
            raise Exception("Booking failed")
        if not json.loads(r.text).get("success"):
            # print(json.loads(r.text).get("success"))
            raise Exception("Booking failed")
        print("Booking successful")
        return True

    def _get_info(self, start_time, end_time, placeID):
        param = {
            "fId": placeID,
            "bizFieldBookField.startTime": start_time,
            "bizFieldBookField.endTime": end_time,
            "repFlag": 0,
            "bookType": 0,
            "userTag": "Student",
            "approvalFlag": 0,
            "extend2": "",
            "bookedNum": 0,
            "fitBook": "false",
            "isDeptAdmin": "false",
            "adMost": 1
        }
        url = "https://booking.cuhk.edu.cn/a/field/client/bookForm"
        r = self.login.get(url, params=param)
        data = r.text
        info = self._parse_info(data)
        return info

    def is_available(self, field_event: FieldEvent, placeID, start_time, end_time):
        info = self._get_info(start_time, end_time, placeID)
        print(info)

    def _parse_info(self, data):
        html = etree.HTML(data)
        # //*[@id="id"]
        id = html.xpath("//*[@id='id']/@value")[0]
        # //*[@id="userId"]
        userId = html.xpath("//*[@id='userId']/@value")[0]
        # //*[@id="userOrgId"]
        userOrgId = html.xpath("//*[@id='userOrgId']/@value")[0]
        # //*[@id="bizFieldBookField.id"]
        bizFieldBookField_id = html.xpath("//*[@id='bizFieldBookField.id']/@value")[0]
        # //*[@id="bizFieldBookField.BId"]
        bizFieldBookField_BId = html.xpath("//*[@id='bizFieldBookField.BId']/@value")[0]
        # //*[@id="userName"]
        userName = html.xpath("//*[@id='userName']/@value")[0]
        # //*[@id="userOrgName"]
        userOrgName = html.xpath("//*[@id='userOrgName']/@value")[0]
        # //*[@id="userEmail"]
        userEmail = html.xpath("//*[@id='userEmail']/@value")[0]
        # //*[@id="bizFieldBookMainForm"]/div/div/div/div[2]/div[6]/div/div/div/input
        field_type = html.xpath("//*[@id='bizFieldBookMainForm']/div/div/div/div[2]/div[6]/div/div/div/input/@value")[0]
        # //*[@id="bizFieldBookMainForm"]/div/div/div/div[2]/div[7]/div/div/div/input
        field_name = html.xpath("//*[@id='bizFieldBookMainForm']/div/div/div/div[2]/div[7]/div/div/div/input/@value")[0]

        return {
            "id": id,
            "userId": userId,
            "userOrgId": userOrgId,
            "bizFieldBookField.id": bizFieldBookField_id,
            "bizFieldBookField.BId": bizFieldBookField_BId,
            "userName": userName,
            "userOrgName": userOrgName,
            "userEmail": userEmail,
            "field_type": field_type,
            "field_name": field_name
        }