#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May  9 21:43:50 2021

@author: nicholasdyette
"""
import sqlite3
weather_db = '/var/jail/home/team28/final_project/weatherdatabase.db'
def request_handler(request):
    conn = sqlite3.connect(weather_db)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS weather_table (user text, location text PRIMARY KEY, count int);''')
    if (request['method'] == 'POST'):
        if(('user' not in request['form']) or ('location' not in request['form']) or 
           ('count' not in request['form'])):
            return False
        else:
            c.execute('''INSERT INTO weather_table VALUES (?,?,?) ON DUPLICATE KEY UPDATE SET count = count + 1);''',
                      (request['form']['user'], request['form']['location'], 1))
    elif (request['method'] == 'GET'):
        if('user' not in request['args']):
            return False
        else:
            values = c.execute('''SELECT TOP 5 location FROM weather_table ORDER BY count ASC;''')
            ret = []
            for val in values:
                ret.append(val)
            return ret
        
    
    