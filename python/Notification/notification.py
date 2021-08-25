import sqlite3
import datetime
import requests
import json
import os.path
import base64
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from apiclient import errors
from email.mime.text import MIMEText

notif_db = '/var/jail/home/team28/final_project/python/Notification/notifications.db'
email_db = '/var/jail/home/team28/final_project/python/EmailApp/emailInfo.db'
SCOPES = ['https://www.googleapis.com/auth/gmail.readonly', 'https://www.googleapis.com/auth/gmail.send', 'https://www.googleapis.com/auth/gmail.modify']

def get_service():
    """Shows basic usage of the Gmail API.
    Lists the user's Gmail labels.
    """
    creds = None
    # The file token.json stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists('/var/jail/home/team28/final_project/python/EmailApp/token.json'):
        creds = Credentials.from_authorized_user_file('/var/jail/home/team28/final_project/python/EmailApp/token.json', SCOPES)
    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file('/var/jail/home/team28/final_project/python/EmailApp/credentials.json', SCOPES)
            creds = flow.run_local_server(port=0)
        # Save the credentials for the next run
        with open('/var/jail/home/team28/final_project/python/EmailApp/token.json', 'w') as token:
            token.write(creds.to_json())

    service = build('gmail', 'v1', credentials=creds)
    return service

def get_messages(service, user):
    results = service.users().messages().list(userId=user, labelIds=['INBOX'], q="is:unread").execute()
    messages = results.get('messages', [])
    if not messages:
        return None
    else:
        return messages

def request_handler(request):
    if request['method'] == "GET":
        try:
            user_me = request['values']['user']
        except Exception as e:
            return e
        #get email accounts associated with this user
        with sqlite3.connect(email_db) as ec:
            ec.execute("""CREATE TABLE IF NOT EXISTS acct_user(USER text, ACCT text, PASSCODE text);""")
            accounts = ec.execute("""SELECT (ACCT) FROM acct_user WHERE USER = ?;""", (user_me,)).fetchall()
        #get all unread emails of associated email accounts
        return_dict = {'messageNum': 0, 'mailNum': 0}
        if len(accounts) > 0:
            serv = get_service()
            #msg_dict = dict()
            msg_count = 0
            #check with notification db
            with sqlite3.connect(notif_db) as cursor:
                cursor.execute("""CREATE TABLE IF NOT EXISTS mail_table (ID text, USER text, SENDER text, NOTIFY text);""")
                for acct in accounts:
                    messages = get_messages(serv, acct[0])
                    if messages:
                        #msg_dict[acct[0]]['msg'] = message
                        #msg_count += len(message)
                        #msg_dict[acct[0]]['ids'] = []
                        #msg_dict[acct[0]]['sender'] = []
                        for message in messages:
                            curr_id = message['id']
                            #msg_dict[acct[0]]['ids'].append(curr_id)
                            msg = serv.users().messages().get(userId=acct[0], id=curr_id).execute()
                            headers = msg['payload']['headers']
                            for header in headers:
                                if header['name'] == 'from':
                                    curr_sender = header['value']
                                    #msg_dict[acct[0]]['sender'].append(curr_sender)
                                    existing = cursor.execute("""SELECT * FROM mail_table WHERE ID = ? AND USER = ? AND SENDER = ?;""", (curr_id, user_me, curr_sender)).fetchall()
                                    if len(existing) == 0:
                                        cursor.execute("""INSERT INTO mail_table VALUES(?,?,?,?);""", (curr_id, user_me, curr_sender, 'YES'))
                mail_info = cursor.execute("""SELECT (SENDER) FROM mail_table WHERE USER = ? AND NOTIFY = 'YES';""",(user_me,)).fetchall()
                cursor.execute("""UPDATE mail_table SET NOTIFY = 'NO' WHERE USER = ?;""", (user_me,))
                if len(mail_info) != 0:
                    return_dict['mailNum'] = len(mail_info)
                    if len(mail_info) == 1:
                        return_dict["mailFrom"] = mail_info[0][0]
        #check message notification db
        with sqlite3.connect(notif_db) as c:
            c.execute("""CREATE TABLE IF NOT EXISTS message_table (TIME_ timestamp, USER text, SENDER text, NOTIFY text);""")
            message_info = c.execute("""SELECT (SENDER) FROM message_table WHERE USER = ? AND NOTIFY = 'YES';""",(user_me,)).fetchall()
            c.execute("""UPDATE message_table SET NOTIFY = 'NO' WHERE USER = ?;""", (user_me,))
        if len(message_info) != 0:
            return_dict['messageNum'] = len(message_info)
            if len(message_info) == 1:
                return_dict["messageFrom"] = message_info[0][0]

        return json.dumps(return_dict, indent=4)

    else:
        return "invalid request type"



