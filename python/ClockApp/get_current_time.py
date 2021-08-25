### libraries

import requests
import json
from datetime import datetime

### http request handler

def get_request(request):
	# gets current utc time and timezone if specified
	try:
		response_dict = {}

		# get utc time
		time = str(datetime.utcnow())
		hour_and_min = time.split(" ")[1].split(":")
		response_dict["utc_hour"] = int(hour_and_min[0])
		response_dict["utc_minute"] = int(hour_and_min[1])
		sec = hour_and_min[2].split(".")
		response_dict["utc_second"] = int(sec[0])
		ms = sec[1].split("+")[0]
		ms = int(ms)//1000
		response_dict["utc_millisecond"] = ms

		# get timezone
		timezone_url = "http://api.openweathermap.org/data/2.5/weather?"
		get_timezone = False
		if "city" in request["values"]:
			city = request["values"]["city"]
			timezone_url = "".join([timezone_url, "q=", city])
			get_timezone = True
		elif "lat" in request["values"] or "lon" in request["values"]:
			if "lat" not in request["values"] or "lon" not in request["values"]:
				response_dict["timezone_found"] = False
			else:
				lat = request["values"]["lat"]
				lon = request["values"]["lon"]
				timezone_url = "".join([timezone_url, "lat=", lat, "&lon=", lon])
				get_timezone = True

		if get_timezone:
			timezone_url = "".join([timezone_url, "&appid=29f8a4a49873dff02420270cea336c09"])
			timezone_response = requests.get(timezone_url)
			timezone_response_dict = json.loads(timezone_response.text)
			if "message" in timezone_response_dict:
				response_dict["timezone_found"] = False
			else:
				response_dict["timezone"] = int(timezone_response_dict["timezone"])
				response_dict["timezone_found"] = True

		response = json.dumps(response_dict)
		return response
	except Exception as e:
		# return the exception for debugging
		# if hasattr(e, 'message'):
		# 	return e.message
		# else:
		# 	return e

		response_dict = {"error" : True}
		response = json.dumps(response_dict)
		return response

def request_handler(request):
    if request["method"] == "GET":
        return get_request(request)
    else:
        return "Only GET supported"