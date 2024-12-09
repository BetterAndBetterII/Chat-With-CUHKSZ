from event import ContentListEvent, ContentEvent, CourseEvent, AnnouncementEvent
from login import BBLogin
from retriever import CourseRetriever, ContentRetriever, AssignmentRetriever, AnnouncementRetriever

PROMPT = "blackboard# "

print("Welcome to the Blackboard CLI!")

# while True:
#     print("Please enter your username and password to log in.")
#     print(PROMPT + "Username: ", end="")
#     username = input().strip()
#     print(PROMPT + "Password: ", end="")
#     password = input().strip()
#
#     try:
#         login = BBLogin(username, password)
#         break
#     except Exception as e:
#         print("Login failed:", e)
#         print("Please try again.")
login = BBLogin("1*************8", "F*************1")
content_retriever = ContentRetriever(login)
course_retriever = CourseRetriever(login)
assignment_retriever = AssignmentRetriever(login)
announcement_retriever = AnnouncementRetriever(login)


def print_tree(_parent, _children: list[ContentListEvent | ContentEvent | CourseEvent | str],
               recursive=False, depth: int = 1) -> None:
    for child in _children:
        if isinstance(child, ContentListEvent):
            print("    " * depth + child.title)
            if recursive:
                print_tree(child, child.contents, recursive, depth + 1)
        elif isinstance(child, CourseEvent):
            print("    " * depth + str(child))
            root_content_lists: list[ContentListEvent] = content_retriever.get_root_content_list_by_course(child)
            if recursive:
                print_tree(child, root_content_lists, recursive, depth + 1)
        elif isinstance(child, ContentEvent):
            print("    " * depth + child.title)
            if not child.get_detail() == "" and recursive:
                print_tree(child, [child.get_detail()], recursive, depth + 1)
        elif isinstance(child, AnnouncementEvent):
            print("    " * depth + child.title)
            if not child.get_detail() == "" and recursive:
                print_tree(child, child.get_detail().split("\n"), recursive, depth + 1)
        else:
            print("    " * depth + str(child))


def recursively_print_retrieve_content_list(_path: str, root_content_list: ContentListEvent, recursive) -> None:
    # MUST: path starts with "/", and first part is the title of the root_content_list
    # path="/assignments/assignment1"
    # root_content_list.title = assignments
    contents = root_content_list.contents
    path_parts = _path.split("/")
    if len(path_parts) == 1:
        print_tree(root_content_list, [content for content in contents], recursive)
        return
    else:
        for content in contents:
            if content.title == path_parts[1]:
                if isinstance(content, ContentListEvent):
                    # is a folder, print contents
                    recursively_print_retrieve_content_list("/".join(path_parts[1:]), content, recursive)
                else:
                    # is a file, print detail
                    print(content.get_detail())
                return


def ls(_path: str, _login: BBLogin, recursive: bool = False) -> None:
    if _path == "/":
        print("/")
        global course_retriever
        courses = course_retriever.get_course_list()
        print_tree("/", [course.title for course in courses], recursive)
    elif _path.startswith("/"):
        if _path.endswith("/"):
            _path = _path[:-1]
        print(_path + "/")
        _path_parts = _path.split("/")
        course_title = _path_parts[1]
        course_retriever = CourseRetriever(_login)
        course = course_retriever.get_course_by_title(course_title)
        if course is None:
            print("Course not found!")
            return
        root_content_lists = content_retriever.get_root_content_list_by_course(course)
        # 只查询课程下的根目录
        if len(_path_parts) == 2:
            print_tree(course.title, [root_content_list.title for root_content_list in root_content_lists], recursive)
            return

        # 递归查询目录下内容
        for root_content_list in root_content_lists:
            if root_content_list.title == _path_parts[2]:
                recursively_print_retrieve_content_list("/".join(_path_parts[2:]), root_content_list, recursive)
                return
        print("Find no content with the given path!")
    else:
        print("Invalid path!")


def assignments(_is_finished: str, _is_detail: str, _course_title: str):
    if _course_title == "":
        __assignments = assignment_retriever.get_assignment_list()
    else:
        course = course_retriever.get_course_by_title(_course_title)
        if course is None:
            print("Course not found!")
            return
        __assignments = assignment_retriever.get_assignment_list_by_course(course)

    if _is_finished == "-unfinished":
        __assignments = [assignment for assignment in __assignments if not assignment.is_finished()]
    elif _is_finished == "-finished":
        __assignments = [assignment for assignment in __assignments if assignment.is_finished()]
    print("Total " + str(len(__assignments)) + " assignments" +
          (" (unfinished) " if _is_finished == "-unfinished" else "") +
          (" (finished) " if _is_finished == "-finished" else "") +
          (" (all) " if _is_finished == "-all" else "") +
          f"in {_course_title if _course_title else 'all courses'}")
    if _is_detail == "-details":
        for assignment in __assignments:
            print(str(assignment))
            print("    Finished" if assignment.is_finished() else "    Unfinished")
            print("    DUE: " + assignment.get_due().strftime("%Y-%m-%d %H:%M:%S"))
            print(assignment.get_detail())
    else:
        for assignment in __assignments:
            print(str(assignment))


def search(_keyword: str):
    contents = content_retriever.get_content_list()
    announcements = announcement_retriever.get_announcement_list()
    for content in contents:
        if _keyword.lower() in (content.title + content.get_detail() + str(content.metadata)).lower():
            print(content.title)
            if not content.get_detail() == "":
                print("    " + content.get_detail())
            print("    Path: " + content.path)
    for announcement in announcements:
        if _keyword.lower() in (announcement.title + announcement.get_detail()).lower():
            print(announcement.title)
            if not announcement.get_detail() == "":
                print("    " + announcement.get_detail())
            print("    Course: " + announcement.course.title)


current_path = "/"
while True:
    command = input(PROMPT + current_path + "> ").strip()

    command_parts = command.split()
    if len(command_parts) > 0:
        command_name = command_parts[0]
        command_args = command_parts[1:]
    else:
        print("Invalid command!")
        continue

    if command_name == "exit":
        break
    elif command_name == "help":
        print("Commands:")
        print("  exit: Exit the program")
        print("  help: Print this help message")
        print("  ls [<path>] [-r(recursive)]: List the contents of the specified directory")
        print("  cd <path>: Change the current directory")
        print("  assignments [-unfinished/-finished/-all] [-details/-no-details] "
              "[<Course Title>]: List all assignments in a "
              "course")
        print("  announcements [-details/-no-details] [<Course Title>]: List all announcements")
        print("  search '<keyword>': Search for a keyword in the current directory")
    elif command_name == "ls":
        if len(command_args) > 2:
            print("Invalid command! Too many arguments")
            continue

        # path = command_args[0] if len(command_args) == 1 else current_path
        path = current_path
        if len(command_args) == 0:
            ls(path, login)
            continue

        if len(command_args) == 1:
            if command_args[0].lower() == "-r":
                ls(path, login, recursive=True)
                continue
            else:
                path = command_args[0]
                ls(path, login)
                continue
        elif len(command_args) == 2:
            if command_args[1].lower() == "-r":
                ls(command_args[0], login, recursive=True)
                continue
            elif command_args[0].lower() == "-r":
                ls(command_args[1], login, recursive=True)
                continue
        else:
            print("Invalid command! Too few arguments")
            continue

    elif command_name == "cd":
        if len(command_args) != 1:
            print("Invalid command! Exactly one argument is required")
            continue

        path = command_args[0]
        if path == "./":
            continue

        if path == "..":
            if current_path == "/":
                print("Invalid path!")
                continue
            if current_path.endswith("/"):
                current_path = "/".join(current_path.split("/")[:-2])
            else:
                current_path = "/".join(current_path.split("/")[:-1])
            continue

        if path.startswith("."):
            if current_path == "/":
                if path.endswith("/"):
                    current_path = current_path + path[1:-1]
                else:
                    current_path = path[1:]
            else:
                if path.endswith("/"):
                    current_path = current_path + path[1:-1]
                else:
                    current_path = current_path + path[1:]
            continue

        if not path.startswith("/"):
            print("Invalid path!")
            continue

        if path.endswith("/"):
            path = path[:-1]

        current_path = path
    elif command_name == "assignments":
        if len(command_args) == 0:
            _assignments = assignment_retriever.get_assignment_list()
            print_tree("/", [assignment.title for assignment in _assignments], recursive=True)

        elif len(command_args) == 1:
            if not command_args[0].lower() in ["-unfinished", "-finished", "-all"]:
                print("Invalid command! Invalid argument")
                continue
            assignments(command_args[0].lower(), "-no-details", current_path.split("/")[1])

        elif len(command_args) == 2:
            if not command_args[0].lower() in ["-unfinished", "-finished", "-all"]:
                print("Invalid command! Invalid argument")
                continue
            if not command_args[1].lower() in ["-details", "-no-details"]:
                print("Invalid command! Invalid argument")
                continue
            assignments(command_args[0].lower(), command_args[1].lower(), current_path.split("/")[1])

        elif len(command_args) == 3:
            if not command_args[0].lower() in ["-unfinished", "-finished", "-all"]:
                print("Invalid command! Invalid argument")
                continue
            if not command_args[1].lower() in ["-details", "-no-details"]:
                print("Invalid command! Invalid argument")
                continue
            assignments(command_args[0].lower(), command_args[1].lower(), command_args[2])

        else:
            print("Invalid command! Too many arguments")
            continue
    elif command_name == "announcements":
        if len(command_args) == 0:
            _announcements = announcement_retriever.get_announcement_list()
            print_tree("/", [announcement.title for announcement in _announcements])
        elif len(command_args) == 1:
            if not command_args[0].lower() in ["-details", "-no-details"]:
                print("Invalid command! Invalid argument")
                continue
            _announcements = announcement_retriever.get_announcement_list()
            print_tree("/", [announcement.title for announcement in _announcements], recursive=True)
        elif len(command_args) == 2:
            if not command_args[0].lower() in ["-details", "-no-details"]:
                print("Invalid command! Invalid argument")
                continue
            course = course_retriever.get_course_by_title(command_args[1])
            if course is None:
                print("Course not found!")
                continue
            _announcements = announcement_retriever.get_announcement_list_by_course(course)
            print_tree(course.title + "/Announcements/", [announcement.title for announcement in _announcements]
                       , recursive=True)
        else:
            print("Invalid command! Too many arguments")
    elif command_name == "search":
        if len(command_args) == 0:
            print("Invalid command! At least one argument is required")
            continue
        if len(command_args) >= 1:
            search(" ".join(command_args).replace("'", "").replace('"', ""))
            continue
        print("Invalid command! Too many arguments")
    else:
        print("Invalid command!")
        continue
