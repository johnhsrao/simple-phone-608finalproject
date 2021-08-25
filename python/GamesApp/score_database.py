import requests
import datetime
import sqlite3
import json

db = '/var/jail/home/team28/final_project/python/GamesApp/scoredatabase.db'

def request_handler(request):

	if request["method"] == "POST":
		best_score = 0
		
		conn = sqlite3.connect(db)  # connect to that database (will create if it doesn't already exist)
		c = conn.cursor()  # move cursor into database (allows us to execute commands)
		outs = ""
		c.execute('''CREATE TABLE IF NOT EXISTS score_table (user text, score real, level real, timing timestamp);''') # run a CREATE TABLE command
		
		if 'score' not in request['form']:
			return "missing a value"
		
		try:
			score = int(request['form']['score'])
			user = request['form']['user']
			level = int(request['form']['level'])
			
		except ValueError:
			# return e here or use your own custom return message for error catch
			#be careful just copy-pasting the try except as it stands since it will catch *all* Exceptions not just ones related to number conversion.
			return "Error: score is not a number"

		c.execute('''INSERT into score_table VALUES (?,?,?,?);''', (user, score, level, datetime.datetime.now()))
		
		things = c.execute('''SELECT score FROM score_table WHERE user = ? AND level = ?;''',(user, level)).fetchall()
		
		for i in range(len(things)):
			if things[i][0] > best_score:
				best_score = things[i][0]
				
		conn.commit() # commit commands
		conn.close() # close connection to database
		
		if score == best_score:
			return "You beat your high score!"
		else:
			return "Your high score was {}".format(best_score)
		
	elif request['method'] == "GET":
		conn = sqlite3.connect(db)  # connect to that database (will create if it doesn't already exist)
		c = conn.cursor()  # move cursor into database (allows us to execute commands)
		outs = ""
		c.execute('''CREATE TABLE IF NOT EXISTS score_table (user text, score real, timing timestamp);''') # run a CREATE TABLE command
		things = c.execute('''SELECT * FROM score_table ORDER BY score DESC;''').fetchone()
		second = c.execute('''SELECT * FROM score_table WHERE user != ? ORDER BY score DESC;''',(things[0],)).fetchone()
		third = c.execute('''SELECT * FROM score_table WHERE user != ? AND user != ? ORDER BY score DESC;''',(things[0], second[0])).fetchone()
		scores_and_users = {"user1": things[0], "score1": things[1], "user2": second[0], "score2": second[1], "user3": third[0], 'score3': third[1]}
		

		conn.commit() # commit commands
		conn.close() # close connection to database

		# try:
		# 	place = int(request['values']['place'])
			
		# except ValueError:
		# 	# return e here or use your own custom return message for error catch
		# 	#be careful just copy-pasting the try except as it stands since it will catch *all* Exceptions not just ones related to number conversion.
		# 	return "Error: place is not a number"

		return json.dumps(scores_and_users, indent=4)


	else:
		return 'request not supported. You need to change that.'