### libraries

import requests
import sqlite3
import json

### database path

favorites_db = '/var/jail/home/team28/final_project/python/ClockApp/favorites.db'

### http request handler

def get_request(request):
    # gets favorite cities and their timezones
    response_dict = {}
    try:
        user = request["values"]["user"]

        # get cities
        with sqlite3.connect(favorites_db) as c:
            # create table if not created
            c.execute('''CREATE TABLE IF NOT EXISTS favorite_cities (user TEXT, city TEXT);''')

            city_rows = c.execute('''SELECT * FROM favorite_cities WHERE user = ?;''', (user,)).fetchall()

        # get timezone
        cities = []
        timezones = []
        for city_row in city_rows:
            city = city_row[1]
            cities.append(city)
            timezone_url = "".join(["http://api.openweathermap.org/data/2.5/weather?q=", city, "&appid=29f8a4a49873dff02420270cea336c09"])
            timezone_response = requests.get(timezone_url)
            timezone_response_dict = json.loads(timezone_response.text)
            timezones.append(int(timezone_response_dict["timezone"]))
        response_dict["favorite_cities"] = cities
        response_dict["favorite_timezones"] = timezones

        response = json.dumps(response_dict)
        return response
    except Exception as e:
        # return the exception for debugging
        # if hasattr(e, 'message'):
        # 	return e.message
        # else:
        # 	return e

        return "False"

def post_request(request):
    # insert or delete a favorite city
    try:
        user = request["form"]["user"]
        city = request["form"]["city"]
        delete = request["form"]["delete"]

        with sqlite3.connect(favorites_db) as c:
            # create table if not created
            c.execute('''CREATE TABLE IF NOT EXISTS favorite_cities (user TEXT, city TEXT);''')

            if delete == "True":
                city_row = c.execute('''SELECT * FROM favorite_cities WHERE user = ? AND city = ?;''', (user, city)).fetchone()
                if city_row:
                    c.execute('''DELETE FROM favorite_cities WHERE user = ? AND city = ?;''', (user, city))
                    return "True"
                return "False"
            else:
                city_rows = c.execute('''SELECT * FROM favorite_cities WHERE user = ?;''', (user,)).fetchall()
                city_row = c.execute('''SELECT * FROM favorite_cities WHERE user = ? AND city = ?;''', (user, city)).fetchone()
                if len(city_rows) < 5 and city_row == None:
                    c.execute('''INSERT INTO favorite_cities VALUES(?,?);''', (user, city))
                    return "True"
                return "False"
    except Exception as e:
        # return the exception for debugging
        # if hasattr(e, 'message'):
        # 	return e.message
        # else:
        # 	return e

        return "False"

def request_handler(request):
    if request["method"] == "GET":
        return get_request(request)
    elif request["method"] == "POST":
        return post_request(request)
    else:
        return "Only GET and POST supported"