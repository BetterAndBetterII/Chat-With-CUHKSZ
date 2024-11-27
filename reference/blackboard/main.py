from event import ContentEvent, AssignmentEvent
from login import BBLogin
from retriever import CalendarRetriever, CourseRetriever, ContentRetriever, AssignmentRetriever

if __name__ == '__main__':
    login = BBLogin("1*************8", "F*************1")

    # cal_events = CalendarRetriever(login).get_calendar_data()
    # for event in cal_events:
    #     print(event)
    #
    courses = CourseRetriever(login).get_course_list()
    # for event in courses:
    #     print(event)

    # content_retriever = ContentRetriever(login)
    # contents = content_retriever.get_content_list_by_course(courses[0])
    # for content in contents:
    #     print(str(content) + ' ' + content.path)

    # contents = content_retriever.get_content_list()
    # for content in contents:
    #     print(content)
    #

    assignments = AssignmentRetriever(login).get_assignment_list_by_course(courses[10])
    for assignment in assignments:
        assignment: AssignmentEvent
        print(str(assignment))
        print(assignment.path)
        print(assignment.get_due().strftime('%Y-%m-%d %H:%M:%S'))



