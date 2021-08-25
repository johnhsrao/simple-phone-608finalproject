import os.path
import base64
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from apiclient import errors
from email.mime.text import MIMEText
#from email.mime.base import MIMEBase
#from email.mime.image import MIMEImage
#from email.mime.multipart import MIMEMultipart
#from email.mime.audio import MIMEAudio
#import mimetypes
import requests
import json

# If modifying these scopes, delete the file token.json.
SCOPES = ['https://www.googleapis.com/auth/gmail.readonly', 'https://www.googleapis.com/auth/gmail.send', 'https://www.googleapis.com/auth/gmail.modify']
notif_db = '/var/jail/home/team28/final_project/python/Notification/notifications.db'


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
    if request['method']=="POST":
        try:
            user_me = request['form']['user']
            response_len = int(request['form']['size'])
        except Exception as e:
            return e
    else:
        user_me = request['values']['user']
    service = get_service()
    messages = get_messages(service, user_me)
    return_dict = dict()
    if messages == None:
        if request['method'] == "POST":
            return_dict['num'] = 0
            return return_dict
        else:
            return "0"
    else:
        msg_count = len(messages)
        if request['method'] == "GET":
            return "{}".format(msg_count)
        else:
            msg_index = 1
            return_dict['More']='N'
            for message in messages:
                if len(json.dumps(return_dict, indent=4)) < response_len:
                    msg = service.users().messages().get(userId=user_me, id=message['id']).execute()
                    service.users().messages().modify(userId=user_me, id=message['id'], body={'removeLabelIds': ['UNREAD']}).execute()
                    headers = msg['payload']['headers']
                    return_dict[msg_index] = dict()
                    for header in headers:
                        if header['name'] == 'from':
                            return_dict[msg_index]['From'] = header['value']
                        elif header['name'] == 'subject':
                            return_dict[msg_index]['Subject'] = header['value']
                    #return_dict[msg_index]['Snippet'] = msg['snippet']
                    encoded = msg['payload']['body']['data']
                    decoded = base64.urlsafe_b64decode(encoded) 
                    return_dict[msg_index]['Message'] = decoded.decode('ascii')
                    return_dict['num'] = msg_index
                    msg_index += 1
                else:
                    return_dict['More']='Y'
                    break
            return json.dumps(return_dict, indent=4)

        
            



        



