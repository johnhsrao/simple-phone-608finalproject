import requests
import datetime
import sqlite3

db = '/var/jail/home/team28/final_project/final_project/python/GamesApp/score_database.py'

def request_handler(request):
    
    return request
    if request["method"] == "POST":
        best_score = 0
        
        conn = sqlite3.connect(db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        outs = ""
        c.execute('''CREATE TABLE IF NOT EXISTS score_table (user text, score real, timing timestamp);''') # run a CREATE TABLE command
        
        if 'score' not in request['form']:
            return "missing a value"
        
        try:
            score = int(request['form']['score'])
            user = request['form']['user']
            
        except ValueError:
            # return e here or use your own custom return message for error catch
            #be careful just copy-pasting the try except as it stands since it will catch *all* Exceptions not just ones related to number conversion.
            return "Error: score is not a number"
        return "work til here"
        c.execute('''INSERT into score_table VALUES (?,?,?);''', (user, score, datetime.datetime.now()))
        
        things = c.execute('''SELECT score FROM score_table WHERE user = ?;''',(user,)).fetchall()
        
        outs = "Things:\n"
        for i in range(len(things)):
            
            if things[i] > best_score:
                best_score = things[i]
                
        conn.commit() # commit commands
        conn.close() # close connection to database
        
        if score == best_score:
            return "You beat your high score!"
        else:
            return "Your high score was {}".format(best_score)
        
    elif request['method'] == "GET":
        return "in get request"
        conn = sqlite3.connect(db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        outs = ""
        c.execute('''CREATE TABLE IF NOT EXISTS score_table (user text, score real, timing timestamp);''') # run a CREATE TABLE command
        things = c.execute('''SELECT * FROM score_table ORDER BY score ASC;''').fetchone()
        second = c.execute('''SELECT * FROM score_table WHERE user NOT IN ? ORDER BY score ASC;''',(things[0],)).fetchone()
        third = c.execute('''SELECT * FROM score_table WHERE user NOT IN ? and user NOT IN ? ORDER BY score ASC;''',(things[0], second[0])).fetchone()
        scores_and_users = {things[0]: things[1], second[0]: second[1], third[0]: third[1]}
        

        conn.commit() # commit commands
        conn.close() # close connection to database
        return scores_and_users

    else:
        return 'request not supported. You need to change that.'