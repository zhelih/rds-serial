#!/usr/bin/python3
import smtplib

from email.mime.text import MIMEText

def SendMail(subj, message):
    msg = MIMEText(message)
    msg['Subject'] = subj
    msg['From'] =  'noreply@butt7500g-3018.ie.tamu.edu'
    msg['To'] = 'lykhovyd@tamu.edu'
    msg['Cc'] = 'tsumiman@tamu.edu'

    s = smtplib.SMTP('localhost')
    s.send_message(msg)
    s.quit()
