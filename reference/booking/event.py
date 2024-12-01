from datetime import datetime

import pytz
from lxml import etree

from login import Login


class BaseEvent:
    title: str
    login: Login

    def __init__(self, title, login):
        self.title = title
        self.login = login

    def __str__(self):
        pass


class TypeEvent(BaseEvent):
    def __init__(self, type, title, path, login):
        self.type = type
        self.path = path
        super().__init__(title=title, login=login)

    def __str__(self):
        return f"{self.title}"


class BookEvent:
    def __init__(self, placeID, startTime, endTime, booker, reason):
        self.placeID = placeID
        self.startTime: datetime = startTime
        self.endTime: datetime = endTime
        self.booker = booker
        self.reason = reason

    def __str__(self):
        return f"Booker: {self.booker}, Start Time: {self.startTime}, End Time: {self.endTime}, Reason: {self.reason}, PlaceID: {self.placeID}"


class FieldEvent(BaseEvent):
    """
    场地的信息：场地名称、场地ID
    场地已经预定的信息：预定时间、预定人
    场地空闲的信息：空闲时间
    场地是否已开放预定
    """
    places: dict[str, str]

    def __init__(self, field_id, field_name, book_info: list[BookEvent], places, login):
        self.field_id = field_id
        self.field_name = field_name
        self.book_info = book_info
        self.places = places
        super().__init__(title=field_name, login=login)

    def is_available(self, start_time: str, end_time: str) -> list[str]:
        """
        判断场地是否空闲
        :param start_time: "%Y-%m-%d %H:%M"
        :param end_time: "%Y-%m-%d %H:%M"
        :return:
        """
        start_time = datetime.strptime(start_time, "%Y-%m-%d %H:%M")
        end_time = datetime.strptime(end_time, "%Y-%m-%d %H:%M")

        # group book info by placeID
        book_info_dict = {}
        for placeID in self.places:
            book_info_dict[placeID] = []
        for book in self.book_info:
            book_info_dict[book.placeID].append(book)

        available_place = [_ for _ in self.places]
        for placeID in book_info_dict:
            book_info = book_info_dict[placeID]
            if not self.__is_available(start_time, end_time, book_info):
                available_place.remove(placeID)
        return available_place

    def __is_available(self, start_time: datetime, end_time: datetime, book_info: list[BookEvent]):
        for book in book_info:
            if (book.startTime < start_time < book.endTime or book.startTime < end_time < book.endTime
                    or start_time < book.startTime < end_time or start_time < book.endTime < end_time):
                return False
        return True

    def __str__(self):
        return f"{self.title}"
