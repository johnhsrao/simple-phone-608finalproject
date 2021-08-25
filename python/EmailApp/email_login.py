import sqlite3
import datetime
import requests
import json

email_db = '/var/jail/home/team28/final_project/python/EmailApp/emailInfo.db'
authorized_accounts = {'608team28@gmail.com':'demoaccount'}

def request_handler(request):
	if request['method']=="POST":
		try:
			user = request['form']['user']
			email = request['form']['email']
			passcode = request['form']['passcode']
		except Exception as e:
			return e
		if email not in authorized_accounts.keys():
			return "You need to give authorization with this email first"
		else:
			verified = False
			with sqlite3.connect(email_db) as c:
				c.execute("""CREATE TABLE IF NOT EXISTS acct_user(USER text, ACCT text, PASSCODE text);""")
				account_info = c.execute("""SELECT * FROM acct_user WHERE ACCT = ? AND USER = ?;""", (email,user)).fetchone()
				if not account_info:
					verified_pass = authorized_accounts[email]
					if verified_pass == passcode:
						verified = True
						c.execute("""INSERT INTO acct_user VALUES (?,?,?);""",(user, email, verified_pass))
				else:
					if passcode == account_info[2]:
						verified = True
			if verified == True:
				return "True"
			else:
				return "Please check your password"
	else:
		try:
			user = request['values']['user']
		except Exception as e:
			return e
		with sqlite3.connect(email_db) as c:
			c.execute("""CREATE TABLE IF NOT EXISTS acct_user(USER text, ACCT text, PASSCODE text);""")
			accounts = c.execute("""SELECT (ACCT) FROM acct_user WHERE USER = ?;""", (user,)).fetchall()
		return_dict=dict()
		if accounts:
			num = 0
			for i in range(len(accounts)):
				return_dict[i+1] = str(accounts[i][0])
				num += 1
				if i > 2:
					break
			return_dict['num'] = num
			return json.dumps(return_dict, indent = 4)
		else:
			return_dict['num'] = 0
			return json.dumps(return_dict, indent = 4)