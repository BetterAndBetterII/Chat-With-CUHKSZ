import time
from datetime import timedelta

from booking.booker import Booker
from event import *
from login import BookingLogin
from retriever import FieldInfoRetriever
import json

if __name__ == '__main__':
    # username = "1******8"
    # password = "F****************1"
    # start_time = "2024-04-04 12:00"
    # end_time = "2024-04-04 13:00"
    # phone = "138********"
    # reason = "test"
    # details = "fun"

    # load config from ./config.json
    with open("config.json") as f:
        config = json.load(f)
        username = config["username"]
        password = config["password"]
        start_time = config["start_time"]
        end_time = config["end_time"]
        phone = config["phone"]
        reason = config["reason"]
        details = config["details"]

    start_time = datetime.strptime(start_time, "%Y-%m-%d %H:%M")
    end_time = datetime.strptime(end_time, "%Y-%m-%d %H:%M")

    login = BookingLogin(username, password)

    # booker.is_available(field, available_places[0], "2024-04-02 12:00", "2024-04-02 13:00")
    # booker.book(field, available_places[0], start_time, end_time)

    while True:
        try:
            print("Checking...")
            field = FieldInfoRetriever(login).retrieve("badminton",
                                                       start_time.strftime("%Y-%m-%d"),
                                                       (start_time + timedelta(days=1)).strftime("%Y-%m-%d"))
            available_places = field.is_available(start_time.strftime("%Y-%m-%d %H:%M"),
                                                  end_time.strftime("%Y-%m-%d %H:%M"))
            print(f"Available places: {available_places}")
            if len(available_places) == 0:
                print("No available place")
                break
        except Exception as e:
            print(f"Failed to retrieve field info, check your username and password...")
            time.sleep(1)
            continue

        booker = Booker(phone, reason, details, login)

        for place in available_places:
            try:
                res = booker.book(field, available_places[0], start_time, end_time)
                if res:
                    break
            except Exception as e:
                print(f"Booking {place} failed, retrying...")
                pass

        time.sleep(0.5)  # 请求的间隔时间
        
