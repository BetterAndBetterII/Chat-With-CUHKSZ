from lxml import etree

from event import CalendarEvent, CourseEvent, ContentEvent, AssignmentEvent, AnnouncementEvent, \
    ContentListEvent
from utils.utils import Login
from tqdm import tqdm

class BaseRetriever:
    def __init__(self, login: Login):
        self.login = login

    def retrieve(self, query):
        raise NotImplementedError("retrieve method must be implemented")


YEARS = "years"
MONTHS = "months"
WEEKS = "weeks"
DAYS = "days"

CALENDAR = "calendar"
COURSE = "course"
CONTENT = "content"
ANNOUNCEMENT = "announcement"
ASSIGNMENT = "assignment"
DISCUSSION = "discussion"


class CalendarRetriever(BaseRetriever):
    def retrieve(self, query: str) -> list[CalendarEvent]:
        """
        retrieve data from blackboard
        :param query: Default is None. Get all CalendarEvents
        :return: list of data (BaseEvent)
        """
        return self.get_calendar_data(2, YEARS)

    def _parse_calendar_data(self, data) -> list[CalendarEvent]:
        events = []
        for item in data:
            event = CalendarEvent(
                start=item["start"],
                end=item["end"],
                title=item["calendarName"],
                location=item["calendarNameLocalizable"],
                sub_title=item["title"],
                _id=item["id"],
                name=item["title"],
                description=item["eventType"],
                login=self.login
            )
            events.append(event)
        return events

    def get_calendar_data_period(self, start, end) -> list[CalendarEvent]:
        # timestamp in milliseconds
        params = {
            "start": start,
            "end": end,
            "course_id": "",
            "mode": "personal"
        }
        r = self.login.get("https://bb.cuhk.edu.cn/webapps/calendar/calendarData/selectedCalendarEvents", params=params)
        data = r.json()
        events = self._parse_calendar_data(data)
        return events

    def get_calendar_data(self, counts=1, _type=MONTHS) -> list[CalendarEvent]:
        import time
        now = int(time.time() * 1000)
        if _type == MONTHS:
            start = now - 1000 * 60 * 60 * 24 * 30 * counts
            end = now + 1000 * 60 * 60 * 24 * 30 * counts
            return self.get_calendar_data_period(start, end)
        elif _type == WEEKS:
            start = now - 1000 * 60 * 60 * 24 * 7 * counts
            end = now + 1000 * 60 * 60 * 24 * 7 * counts
            return self.get_calendar_data_period(start, end)
        elif _type == DAYS:
            start = now - 1000 * 60 * 60 * 24 * counts
            end = now + 1000 * 60 * 60 * 24 * counts
            return self.get_calendar_data_period(start, end)
        elif _type == YEARS:
            start = now - 1000 * 60 * 60 * 24 * 365 * counts
            end = now + 1000 * 60 * 60 * 24 * 365 * counts
            return self.get_calendar_data_period(start, end)
        else:
            raise ValueError("type must be one of 'years', 'months', 'weeks', 'days'")

    def get_ical_link(self) -> str:
        url = "https://bb.cuhk.edu.cn/webapps/calendar/calendarFeed/url"
        return self.login.get(url).text


class CourseRetriever(BaseRetriever):
    def retrieve(self, query: str) -> list[CourseEvent]:
        """
        retrieve courses from blackboard
        :param query: Default is None. Get all courses
        :return: list of courses (CourseEvent)
        """
        return self.get_course_list()

    def get_course_list(self) -> list[CourseEvent]:
        r = self.login.get("https://bb.cuhk.edu.cn/webapps/portal/execute/tabs/tabAction?tab_tab_group_id=_1_1")
        data = r.text
        courses = self._parse_course_data(data)
        return courses

    def _parse_course_data(self, data: str) -> list[CourseEvent]:
        courses = []
        for line in data.split("\n"):
            if "type=Course" in line:
                # <a href=" /webapps/blackboard/execute/launcher?type=Course&id=_10351_1&url="
                # target="_top">CHI1000:Chinese_L13L14L15L16</a>
                course_id = line.split("id=")[1].split("&")[0]
                course_name = line.split(">")[1].split("<")[0]
                course = CourseEvent(course_id, course_name, login=self.login)
                courses.append(course)
        return courses

    def get_course_by_title(self, title: str) -> CourseEvent | None:
        courses = self.get_course_list()
        for course in courses:
            if course.title == title:
                return course

        # blur search
        for course in courses:
            if title in course.title:
                return course
        return None


class ContentRetriever(BaseRetriever):
    def retrieve(self, query: str) -> list[ContentEvent]:
        return self.get_content_list()

    def get_root_content_list_by_course(self, courses: CourseEvent | list[CourseEvent]) -> list[ContentListEvent]:
        if isinstance(courses, CourseEvent):
            courses = [courses]
        root_contents = []
        for course in tqdm(courses):
            url = f'https://bb.cuhk.edu.cn/webapps/blackboard/execute/modulepage/view?course_id={course.id}'
            r = self.login.get(url=url)
            data = r.text
            # print(data)
            root_contents.extend(self.parse_content_data(data, course))

        return root_contents

    def get_content_list_by_course(self, courses: CourseEvent | list[CourseEvent]) -> list[ContentEvent]:
        root_contents = self.get_root_content_list_by_course(courses)

        _all = []
        for root_content in root_contents:
            _all.extend(root_content.get_all_contents())
        return _all

    def get_content_list(self) -> list[ContentEvent]:
        courses = CourseRetriever(self.login).get_course_list()
        contents = self.get_content_list_by_course(courses)
        return contents

    @staticmethod
    def parse_content_data(data: str, course: CourseEvent) -> list[ContentListEvent]:
        root_contents = []
        # etree parse html, li element with href contains "content_id"
        html = etree.HTML(data)
        _li_list = html.xpath("//li")
        if len(_li_list) <= 0:
            return []
        for element in _li_list:
            href = element.xpath("a")
            if len(href) <= 0:
                continue
            href_str = href[0].get("href")
            if href and "content_id" in href_str:
                content_id = href_str.split("content_id=")[1].split("&")[0]
                title = href[0].xpath("span/text()")[0]
                content = ContentListEvent(course, content_id, title, path=course.title + "/" + title)
                root_contents.append(content)

        # print(f'Get {len(root_contents)} Folders in {course.title}!')

        return root_contents


class AssignmentRetriever(BaseRetriever):
    def retrieve(self, query: str) -> list[AssignmentEvent]:
        return self.get_assignment_list()

    def get_assignment_list_by_course(self, courses: CourseEvent | list[CourseEvent]) -> list[AssignmentEvent]:
        all_contents = ContentRetriever(self.login).get_content_list_by_course(courses)
        all_assignments = []
        for content in all_contents:
            if isinstance(content, AssignmentEvent):
                all_assignments.append(content)
        return all_assignments

    def get_assignment_list(self) -> list[AssignmentEvent]:
        return self.get_assignment_list_by_course(CourseRetriever(self.login).get_course_list())


class AnnouncementRetriever(BaseRetriever):
    def retrieve(self, query: str) -> list[AnnouncementEvent]:
        return self.get_announcement_list()

    def get_announcement_list_by_course(self, courses: CourseEvent | list[CourseEvent]) -> list[AnnouncementEvent]:
        if isinstance(courses, CourseEvent):
            courses = [courses]
        all_announcements = []
        for course in courses:
            url = (f'https://bb.cuhk.edu.cn/webapps/blackboard/execute/announcement?'
                   f'method=search&context=mybb&course_id={course}&viewChoice=2')
            r = self.login.get(url=url)
            data = r.text
            # print(data)
            all_announcements.extend(self._parse_announcement_data(data, course))
        return all_announcements

    def get_announcement_list(self) -> list[AnnouncementEvent]:
        courses = CourseRetriever(self.login).get_course_list()
        announcements = []
        for course in courses:
            announcements += self.get_announcement_list_by_course(course)
        return announcements

    @staticmethod
    def _parse_announcement_data(data: str, course: CourseEvent) -> list[AnnouncementEvent]:
        announcements = []
        # etree parse html, li element with href contains "content_id"
        html = etree.HTML(data)
        for element in html.xpath("//li"):
            href = element.xpath("@href")
            if href and "announcement_id" in href[0]:
                # <a href="/webapps/blackboard/content/listContent.jsp?
                # course_id=_11467_1&content_id=_123237_1&mode=reset"
                # target="_top">Assignment 1</a>
                announcement_id = href[0].split("announcement_id=")[1].split("&")[0]
                title = element.xpath("a/text()")[0]
                announcement = AnnouncementEvent(course, announcement_id, title)
                announcement.metadata["detail"] = element.xpath("string(.)")
                announcements.append(announcement)

        return announcements
