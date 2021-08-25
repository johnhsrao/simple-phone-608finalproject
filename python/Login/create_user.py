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
		conn = sqlite3.connect(user_db)
		c = conn.cursor()
		c.execute("""CREATE TABLE IF NOT EXISTS authorized_users(USER text, PASS text);""")
		selected = c.execute("""SELECT * FROM authorized_users WHERE USER = ?;""", (username,)).fetchone()
		if selected:
			conn.commit()
			conn.close()
			return "This username already exists"
		else:
			c.execute("""INSERT INTO authorized_users VALUES (?,?);""",(username, passcode))
			conn.commit()
			conn.close()
			return 'True'
	else:
		return "Invalid request type"