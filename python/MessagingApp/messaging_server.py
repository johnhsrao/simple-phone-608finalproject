import sqlite3
import datetime
import requests
import json

messaging_db = '/var/jail/home/team28/final_project/python/MessagingApp/messages.db'
notif_db = '/var/jail/home/team28/final_project/python/Notification/notifications.db'

def insert_dict(d, msg, sender):
	output = dict()
	output['date'] = d[5:16]
	output['message'] = sender + ": " + msg
	return output

def request_handler(request):
	if request['method']=="POST":
		#use POST request to send messages and check old messages
		action = request['form']['action']

		if action == 'send':
		#if POST request is sending a message
		#POST request body should have arguments: action ('send'), sender, receiver, message
			try:
				user_sender = request['form']['sender']
				user_receiver = request['form']['receiver']
				message = request['form']['message']
			except Exception as e:
				return e
			#create two tables for each user ex. jhsraoSENT(for message jhsrao sent) and jhsraoRECEIVE
			sender_table = user_sender + 'SENT'
			receiver_table = user_receiver + 'RECEIVED'
			with sqlite3.connect(messaging_db) as c:
				#use formatted strings to handle table with variable name
				tableS_command = """CREATE TABLE IF NOT EXISTS {} (TIME_ timestamp, RECEIVER text, MESSAGE text);""".format(sender_table)
				#receiver tables have 1 extra column indicating whether the receiver has checked that message or not
				tableR_command = """CREATE TABLE IF NOT EXISTS {} (TIME_ timestamp, SENDER text, MESSAGE text, READ text);""".format(receiver_table)
				c.execute(tableS_command)
				c.execute(tableR_command)
				insertS_command = """INSERT INTO {} VALUES (?,?,?);""".format(sender_table)
				insertR_command = """INSERT INTO {} VALUES (?,?,?,?);""".format(receiver_table)
				c.execute(insertS_command, (datetime.datetime.now(), user_receiver, message))
				c.execute(insertR_command, (datetime.datetime.now(), user_sender, message, 'NO'))
			#insert into notification table
			with sqlite3.connect(notif_db) as cursor:
				cursor.execute("""CREATE TABLE IF NOT EXISTS message_table (TIME_ timestamp, USER text, SENDER text, NOTIFY text);""")
				cursor.execute("""INSERT INTO message_table VALUES(?,?,?,?);""", (datetime.datetime.now(), user_receiver, user_sender, 'YES'))
			#sending complete return confirmation to user
			return "Your message has been sent to {}!".format(user_receiver)


		if action == 'check':
		#user wants to check a message
		#POST request body should have arguments: action ('check'), other(the user wants to check), me(the user checking), num(how many messages to check)
			try:
				user_other = request['form']['other']
				user_me = request['form']['me']
				num_msg = int(request['form']['num'])
				response_size = int(request['form']['size'])
			except Exception as e:
				return e
			#check both user_me's sent and received table
			sent_table = user_me + 'SENT'
			received_table = user_me + 'RECEIVED'
			#check all unread messages/ on esp, if user is checking all unread messages, send POST request action=check, other=='UN_READ', num=(number of unread)
			if user_other == 'UN_READ':
				with sqlite3.connect(messaging_db) as c:
					#on esp, make sure use can only check UN_READ when prompted there are unread messages (server is not checking if user received table exists)
					unread_query = """SELECT * FROM {} WHERE READ = 'NO' ORDER BY TIME_ DESC;""".format(received_table)
					unread_messages = c.execute(unread_query).fetchall()
					#to_return = []
				#set notify to NO in notification table
				with sqlite3.connect(notif_db) as cursor:
					cursor.execute("""UPDATE message_table SET NOTIFY = 'NO' WHERE USER = ?;""", (user_me,))
				return_dict = dict()
				length = 0
				to_return = []
				for i in range(len(unread_messages)):
					if len(json.dumps(return_dict, indent=4)) < (response_size-50):
						return_dict[i+1] = insert_dict(unread_messages[i][0], unread_messages[i][2], unread_messages[i][1])
						length = i+1
						to_insert = [unread_messages[i][0], unread_messages[i][1]+ ": "+ unread_messages[i][2]]
						to_return.append(to_insert)
					else:
						break
				return_dict["length"] = length
				return_dict["end"] = "{} unread messages shown".format(length)
				update_status = """ UPDATE {} SET READ = 'YES' WHERE READ = 'NO' AND TIME_ = ?;""".format(received_table)
				with sqlite3.connect(messaging_db) as c:
					for msg in to_return:
						c.execute(update_status, (msg[0],))
				return json.dumps(return_dict, indent=4)
			#check message history with a specific user
			else:
				conn = sqlite3.connect(messaging_db)
				c = conn.cursor()
				#check if these tables exists
				queryS = """SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?;"""
				queryR = """SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?;"""
				# return 0 if table doesn't exist 1 otherwise
				tableS_count = c.execute(queryS, (sent_table,)).fetchone()
				tableR_count = c.execute(queryR, (received_table,)).fetchone()
				#if neither table exists there is no message history
				if tableS_count[0] == 0 and tableR_count[0] == 0:
					conn.commit()
					conn.close()
					return_dict = dict()
					return_dict['end'] = "There are no messages between you and {}".format(user_other)
					return_dict['length'] = 0
					return json.dumps(return_dict, indent=4)
				if tableR_count[0] != 0:
					check_received = """SELECT * FROM {} WHERE SENDER = ? ORDER BY TIME_ DESC;""".format(received_table)
					received_messages =  c.execute(check_received, (user_other,)).fetchall()
				else:
					received_messages = []
				
				if tableS_count[0] != 0:
					check_sent = """SELECT * FROM {} WHERE RECEIVER = ? ORDER BY TIME_ DESC;""".format(sent_table)
					sent_messages = c.execute(check_sent, (user_other,)).fetchall()
				else:
					sent_messages = []
				conn.commit()
				conn.close()
				#order messages sent and received by time and only return the num user specfied
				to_return = []
				return_dict = dict()
				length = 0
				if len(sent_messages) == 0 and len(received_messages) == 0:
					return_dict = dict()
					return_dict['end'] = "There are no messages between you and {}".format(user_other)
					return_dict['length'] = 0
					return json.dumps(return_dict, indent=4)
				for i in range(num_msg):
					if len(json.dumps(return_dict, indent=4)) < (response_size-50):
						if len(sent_messages) >= 1 and len(received_messages) >= 1:
							if sent_messages[0] > received_messages[0]:
								selected = sent_messages.pop(0)
								return_dict[i+1] = insert_dict(selected[0], selected[2], "Me")
								length = i + 1
								#to_insert = ['Me: '+ selected[2]]
								to_insert = [selected[0], "Me: "+ selected[2]]
								to_return.append(to_insert)
							elif received_messages[0] >= sent_messages[0]:
								selected = received_messages.pop(0)
								return_dict[i+1] = insert_dict(selected[0], selected[2], user_other)
								length = i + 1
								#to_insert = [user_other + ': '+selected[2]]
								to_insert = [selected[0], user_other + ": "+selected[2]]
								to_return.append(to_insert)
						elif len(sent_messages) >= 1:
							selected = sent_messages.pop(0)
							return_dict[i+1] = insert_dict(selected[0], selected[2], "Me")
							length = i + 1
							#to_insert = ['Me: '+ selected[2]]
							to_insert = [selected[0], "Me: "+ selected[2]]
							to_return.append(to_insert)
						elif len(received_messages) >= 1:
							selected = received_messages.pop(0)
							return_dict[i+1] = insert_dict(selected[0], selected[2], user_other)
							length = i + 1
							#to_insert = [user_other + ': '+selected[2]]
							to_insert = [selected[0], user_other + ": "+ selected[2]]
							to_return.append(to_insert)
						else:
							break
					else:
						break
				'''to_return is a nested list = [['sender:', TIME_, MESSAGE ], "These are the last num messages between you and user_other"\
				or "There are only x messages between you and user_other"]'''
				'''return_dict is a dictionary {(index):{date:(time), message: (sender: message)}, end: (end message), length: (number of messages)}'''
				return_dict['length'] = length
				with sqlite3.connect(messaging_db) as c:
					for msg in to_return:
						update_status = """ UPDATE {} SET READ = 'YES' WHERE READ = 'NO' AND TIME_ = ?;""".format(received_table)
						c.execute(update_status, (msg[0],))
				with sqlite3.connect(notif_db) as cursor:
					for msg in to_return:
						cursor.execute("""UPDATE message_table SET NOTIFY = 'NO' WHERE USER = ? AND TIME_ = ?;""",(user_me, msg[0]))
				if len(to_return) < num_msg:
					return_dict['end'] = "Only {} messages between you and {} shown".format(len(to_return), user_other)
				else:
					return_dict['end'] = "Last {} messages between you and {} shown".format(num_msg, user_other)
				return json.dumps(return_dict, indent=4)

	else:
		#use GET request to periodically check if there are unread messagese
		#these GET request are automatically sent by esp every x seconds, only needs one argument
		#server should return number of unread messages
		user = request['values']['receiver']
		user_table = user + 'RECEIVED'
		table_command = """CREATE TABLE IF NOT EXISTS {} (TIME_ timestamp, SENDER text, MESSAGE text, READ text);""".format(user_table)
		with sqlite3.connect(messaging_db) as c:
			c.execute(table_command)
			query = """SELECT * FROM {} WHERE READ = 'NO';""".format(user_table)
			num_unread = len(c.execute(query).fetchall())
		return num_unread