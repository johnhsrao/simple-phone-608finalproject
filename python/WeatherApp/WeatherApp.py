#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May  9 21:43:50 2021

@author: nicholasdyette
"""
import sqlite3
weather_db = '/var/jail/home/team28/final_project/python/WeatherApp/realweatherdatabase.db'
def request_handler(request):
    conn = sqlite3.connect(weather_db)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS weather_table (user text, location text, count int);''')
    if (request['method'] == 'POST'):
        if(('user' not in request['form']) or ('location' not in request['form']) or 
           ('count' not in request['form'])):
            return "POST Request missing user, location, or count"
        else:
            users = c.execute('''SELECT user FROM weather_table;''').fetchall()
            valueset = set()
            for user in users:
                us = user[0]
                locations = c.execute('''SELECT location FROM weather_table WHERE user = ?;''',(us,)).fetchall()
                for location in locations:
                    valueset.add((us, location[0]))
            if((request['form']['user'], request['form']['location']) in valueset):
                c.execute('''UPDATE weather_table SET count = count + 1 WHERE location = ? AND user = ?;''',(request['form']['location'], request['form']['user']))
                conn.commit()
                conn.close()
                return "Updated Prexisting Value"
            else:
                c.execute('''INSERT into weather_table VALUES (?,?,?);''',(request['form']['user'], request['form']['location'], 1))
                conn.commit()
                conn.close()
                return "Inserted new value"
    elif (request['method'] == 'GET'):
        if('user' not in request['args']):
            return "GET Request missing user"
        else:
            values = c.execute('''SELECT location FROM weather_table WHERE user = ? ORDER BY count DESC LIMIT 5;''', (request['values']['user'],))
            ret = ''
            for val in values:
                if val[0] != '' and val[0] != ' ':
                    ret += val[0] + '&'
            return ret  
        



# post = {'method': 'POST', 'args': [], 'values': {}, 'content-type': 'application/x-www-form-urlencoded', 'is_json': False,
# 'data': b'user=jack&location=Boston&count=1', 'form': {'user': 'jack', 'location': 'Boston', 'count': '1'}}
# get = {'method': 'GET', 'args': ['user'], 'values': {'user': 'jack'}}
# print(request_handler(get))
