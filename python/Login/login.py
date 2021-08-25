import sqlite3
import requests

user_db = '/var/jail/home/team28/final_project/python/Login/user_info.db'

def request_handler(request):
	if request['method']=="POST":
		try:
			username = request['form']['username']
			passcode = request['form']['passcode']
		except Exception as e:
			return "Username and passcode must be specified"
		with sqlite3.connect(user_db) as c:
			c.execute("""CREATE TABLE IF NOT EXISTS authorized_users(USER text, PASS text);""")
			selected = c.execute("""SELECT * FROM authorized_users WHERE USER = ?;""", (username,)).fetchone()
		if not selected:
			return 'This username does not exist'
		if selected[1] == passcode:
			return 'True'
		else:
			return "Username and passcode do not match our record"
	else:
		return "Invalid request type"