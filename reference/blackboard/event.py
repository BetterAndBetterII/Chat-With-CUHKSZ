from datetime import datetime

import pytz
from lxml import etree

from utils.utils import Login


class BaseEvent:
    title: str
    id: str
    login: Login

    def __init__(self, title, _id, login):
        self.title = title
        self.id = _id
        self.login = login

    def __str__(self):
        pass


class CourseEvent(BaseEvent):
    def __init__(self, course_id, course_name, login):
        super().__init__(title=course_name, _id=course_id, login=login)

    def __str__(self):
        return f"{self.title}"


class CalendarEvent(BaseEvent):
    start: datetime
    end: datetime
    location: CourseEvent | str
    sub_title: str
    name: str
    description: str

    def __init__(self, start, end, title, location, sub_title, _id, name, description, login):
        self.start = start
        self.end = end
        self.location = location
        self.sub_title = sub_title
        self.name = name
        self.description = description
        super().__init__(title=title, _id=_id, login=login)

    def __str__(self):
        return f"{self.title}"


class ContentEvent(BaseEvent):
    course: CourseEvent
    path: str
    detail: str
    metadata: dict = {}

    def __init__(self, course: CourseEvent, content_id, content_name, path, detail="", metadata=None):
        if metadata is None:
            metadata = {
                "detail": detail
            }
        self.course = course
        self.path = path
        self.detail = detail
        self.metadata.update(metadata)
        super().__init__(title=content_name, _id=content_id, login=course.login)

    def get_detail(self) -> str:
        return self.metadata["detail"] if "detail" in self.metadata else self.detail

    def __str__(self):
        return f"{self.course} {self.title}"


class AnnouncementEvent(BaseEvent):
    course: CourseEvent
    metadata: {}

    def __init__(self, course: CourseEvent, announcement_id, announcement_name, metadata=None):
        if metadata is None:
            metadata = {}
        self.course = course
        self.metadata = metadata
        super().__init__(title=announcement_name, _id=announcement_id, login=course.login)

    def __str__(self):
        return f"{self.title}"

    def get_detail(self):
        return self.metadata.get("detail", "")


class AssignmentEvent(ContentEvent):
    def __init__(self, course: CourseEvent, assignment_id, assignment_name, path, metadata=None):
        if metadata is None:
            metadata = {}
        super().__init__(course=course, content_id=assignment_id, content_name=assignment_name, path=path,
                         metadata=metadata)
        self._get_detail()

    def _get_detail(self) -> None:
        url = (f'https://bb.cuhk.edu.cn/webapps/assignment/uploadAssignment?course_id={self.course.id}'
               f'&content_id={self.id}')
        r = self.login.get(url)
        if "Review Submission" in r.text:
            is_finished = True
        else:
            is_finished = False

        url_new_attempt = (f'https://bb.cuhk.edu.cn/webapps/assignment/uploadAssignment?action=newAttempt&'
                           f'course_id={self.course.id}&content_id={self.id}')
        r2 = self.login.get(url_new_attempt)
        r2 = etree.HTML(r2.text)
        # Get due date
        # //*[@id="metadata"]/div/div/div[1]/div[2]
        due_date = r2.xpath('//*[@id="metadata"]/div/div/div[1]/div[2]/text()')[0].strip()  # Sunday, March 10, 2024
        due_time = r2.xpath('//*[@id="metadata"]/div/div/div[1]/div[2]/span/text()')[0].strip()  # 11:59PM
        # parse datetime object beijing time
        # print(f'Due: {due_date} {due_time}')
        due = datetime.strptime(due_date + " " + due_time, "%A, %B %d, %Y %I:%M %p")
        due = due.astimezone(pytz.timezone('Asia/Shanghai'))

        # Get detail
        _li = r2.xpath('//*[@id="instructions"]')[0]
        # parse all text in li
        detail = _li.xpath("string(.)").strip()

        metadata = {
            "is_finished": is_finished,
            "due": due,
            "detail": detail
        }
        self.metadata = metadata

    def get_due(self) -> datetime:
        return self.metadata["due"]

    def is_finished(self) -> bool:
        return self.metadata["is_finished"]

    def __str__(self):
        return f"{self.course} {self.title}"


class ContentListEvent(ContentEvent):
    contents: list[ContentEvent]
    contents_num: int

    def __init__(self, course: CourseEvent, content_id, content_name, path):
        self.contents_num = 0
        self.contents = []
        super().__init__(course=course, content_id=content_id, content_name=content_name, path=path)
        self.recursive_get_content_data()

    def add_content(self, content: ContentEvent):
        self.contents.append(content)
        self.contents_num += 1

    def recursive_get_content_data(self):
        url = (f'https://bb.cuhk.edu.cn/webapps/blackboard/content/listContent.jsp?course_id={self.course.id}'
               f'&content_id={self.id}&mode=reset')
        r = self.login.get(url=url)
        _html = etree.HTML(r.text)
        # //*[@id="content_listContainer"]
        if len(_html.xpath('//*[@id="content_listContainer"]')) <= 0:
            return
        for _li in _html.xpath('//*[@id="content_listContainer"]')[0]:
            # li's id is "contentListItem:_435903_1"
            _li_id = _li.xpath("@id")[0]
            _content_id = _li_id.split(":")[1]
            # '//*[@id="contentListItem:_424214_1"]/img'
            _type = _li.xpath("img/@src")[0].split("/")[-1].split("_")[0]
            if _type == "folder":
                # '//*[@id="anonymous_element_8"]/a/span'
                div = _li.xpath("div[1]")
                _title = div[0].xpath("h3/a/span/text()")[0]
                _content = ContentListEvent(self.course, _content_id, _title, self.path + "/" + _title)
                self.add_content(_content)
            elif _type == "document":
                div = _li.xpath("div[1]")
                _title = div[0].xpath("h3/span[2]/text()")[0]
                # '//*[@id="contentListItem:_434678_1"]/div[2]/div[2]/div/span'
                try:
                    _detail = _li.xpath("div[2]/div[2]/div/span/text()")[0]
                except IndexError:
                    # raise ValueError(f"Detail not found for {_title}, _li: {etree.tostring(_li)}")
                    _detail = ""
                _content = ContentEvent(self.course, _content_id, _title, self.path + "/" + _title, _detail)
                self.add_content(_content)
            elif _type == "assignment":
                div = _li.xpath("div[1]")
                _title = div[0].xpath("h3/a/span/text()")[0]
                _content = AssignmentEvent(self.course, _content_id, _title, self.path + "/" + _title)
                self.add_content(_content)
            elif _type == "file":
                # //*[@id="anonymous_element_8"]/a/span
                div = _li.xpath("div[1]")
                _title = div[0].xpath("h3/a/span/text()")[0]
                _content = FileEvent(self.course, _content_id, _title, self.path + "/" + _title)
                self.add_content(_content)
            elif _type == "image":
                div = _li.xpath("div[1]")
                _title = div[0].xpath("h3/span[2]/text()")[0]
                _content = FileEvent(self.course, _content_id, _title, self.path + "/" + _title)
                self.add_content(_content)
            elif _type == "panopto":
                div = _li.xpath("div[1]")
                _title = div[0].xpath("h3/a/span/text()")[0]
                _detail = "Panopto Video"
                _content = ContentEvent(self.course, _content_id, _title, self.path + "/" + _title, _detail)
                self.add_content(_content)
            elif _type == "discussion":
                div = _li.xpath("div[1]")
                _title = div[0].xpath("h3/a/span/text()")[0]
                _detail = "Discussion"
                _content = ContentEvent(self.course, _content_id, _title, self.path + "/" + _title, _detail)
                self.add_content(_content)
            else:
                try:
                    div = _li.xpath("div[1]")
                    _title = div[0].xpath("h3/a/span/text()")[0]
                    _detail = _type
                    _content = ContentEvent(self.course, _content_id, _title, self.path + "/" + _title, _detail)
                    self.add_content(_content)
                except IndexError:
                    div = _li.xpath("div[1]")
                    _title = div[0].xpath("h3/span[2]/text()")[0]
                    _detail = _type
                    _content = ContentEvent(self.course, _content_id, _title, self.path + "/" + _title, _detail)
                    self.add_content(_content)
                except Exception:
                    raise ValueError(f"Unknown content type: {_type} when "
                                     f"parsing Content at {self.course}, path: {self.path}")

    def get_all_contents(self) -> list[ContentEvent]:
        _all = []
        for child in self.contents:
            if isinstance(child, ContentListEvent):
                _all.extend(child.get_all_contents())
            else:
                _all.append(child)
        return _all

    def __str__(self):
        return f"{self.course} {self.title} Folder"


class FileEvent(ContentEvent):
    def __init__(self, course, content_id, content_name, path, detail="", metadata=None):
        super().__init__(course, content_id, content_name, path, detail=detail, metadata=metadata)

    def __str__(self):
        return f"{self.course} {self.title}"
