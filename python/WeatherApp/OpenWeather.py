#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue May 11 16:47:01 2021

@author: nicholasdyette
"""
import requests


def request_handler(request):
    coords = False
    if(request['method'] != 'GET'):
        return "Request must be GET request"
    elif('city' not in request['args']):
        coords = True
        if('lat' not in request['args'] or 'lon' not in request['args']):
            return "Args must contain city or coordinates"
    
    base_url = "http://api.openweathermap.org/data/2.5/weather?"
    if(coords):
        complete_url = base_url + "lat=" + request['values']['lat'] + "&lon=" + request['values']['lon'] + "&units=imperial&appid=5ac53954359c633f54cbab998255d20f"
    else:
        complete_url = base_url + "q=" + request['values']['city'] + "&units=imperial&appid=5ac53954359c633f54cbab998255d20f"
    r = requests.get(complete_url)
    response = r.json()
    ret = ''
    cod = response['cod']
    print(cod)
    if(cod == '404'):
        return "ERROR"
    temp = response['main']['temp']
    humidity = response['main']['humidity']
    description = response['weather'][0]['description']
    ret += str(cod) + '&' + str(temp) + '&' + str(humidity) + '&' + description + '&'
    return ret
    

    

get = {'method': 'GET', 'args': ['city'], 'values': {'city': 'Boston'}}
print(request_handler(get))