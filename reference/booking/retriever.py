import json

from lxml import etree

from event import *
from login import Login
from tqdm import tqdm


class BaseRetriever:
    def __init__(self, login: Login):
        self.login = login


field_dict = {
    "badminton": "1097",
}


class FieldInfoRetriever(BaseRetriever):
    def retrieve(self, field: str, start_time, end_time) -> FieldEvent:
        """
        retrieve courses from blackboard
        :param end_time:
        :param start_time:
        :param field: badminton
        :return: list of courses (CourseEvent)
        """
        if field in field_dict:
            return self.get_field_details(field_dict.get(field), start_time, end_time)

    def get_field_places(self, field_id) -> dict[str, str]:
        url = "https://booking.cuhk.edu.cn/a/field/client/main"
        data = {
            "id": field_id,
            "bookType": "0",
            "personTag": "Student",
        }
        r = self.login.post(url, data=data)
        data = r.text
        place_list = self._parse_field_places(data)
        return place_list

    def get_field_details(self, field_id, start_time, end_time) -> FieldEvent:
        url = "https://booking.cuhk.edu.cn/a/field/book/bizFieldBookField/eventsV1"
        params = {
            "ftId": field_id,
            "startTime": start_time,
            "endTime": end_time,
            "reBookMainId": "",
            "jsonStr": "[]",
            "fitUseStr": ""
        }
        r = self.login.get(url, params=params)
        data = r.text
        info_list = self._parse_field_data(data)

        return FieldEvent(field_id, field_id, info_list, self.get_field_places(field_id), self.login)

    def get_field_details_by_field_event(self, start_time, end_time, field_event) -> FieldEvent:
        url = "https://booking.cuhk.edu.cn/a/field/book/bizFieldBookField/eventsV1"
        params = {
            "ftId": field_event.field_id,
            "startTime": start_time,
            "endTime": end_time,
            "reBookMainId": "",
            "jsonStr": "[]",
            "fitUseStr": ""
        }
        r = self.login.get(url, params=params)
        data = r.text
        info_list = self._parse_field_data(data)
        field_event.book_info += info_list
        return field_event

    @staticmethod
    def _parse_field_data(data: str) -> list[BookEvent]:
        res = json.loads(data)
        event_list = res.get("event")
        lock_event_list = res.get("lockEvent")

        info_list = []

        for event in event_list:
            start_time = datetime.strptime(event.get("startTime"), "%Y-%m-%d %H:%M:%S")
            end_time = datetime.strptime(event.get("endTime"), "%Y-%m-%d %H:%M:%S")
            booker = event.get("userName")
            reason = event.get("theme")
            placeID = event.get("fId")
            if not placeID:
                continue
            info_list.append(BookEvent(placeID, start_time, end_time, booker, reason))

        for lock_event in lock_event_list:
            start_time = datetime.strptime(lock_event.get("startTime"), "%Y-%m-%d %H:%M")
            end_time = datetime.strptime(lock_event.get("endTime"), "%Y-%m-%d %H:%M")
            booker = "Locked"
            reason = lock_event.get("reasons")
            placeID = lock_event.get("fId")
            if not placeID:
                continue
            info_list.append(BookEvent(placeID, start_time, end_time, booker, reason))

        return info_list

    @staticmethod
    def _parse_field_places(data: str) -> dict:
        places = {}
        # //*[@id="fieldSelect"]/option[2]
        html = etree.HTML(data)
        for element in html.xpath("//*[@id='fieldSelect']/option"):
            name = element.xpath("text()")[0]
            place_id = element.xpath("@value")[0]
            if place_id == "":
                continue
            places[place_id] = name
        return places
