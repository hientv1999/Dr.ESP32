import smtplib
import sys

#1st = email
#2nd = gender
#3nd = DateContacted
#4nd = TimeContacted

#below to setup email login
server=smtplib.SMTP('smtp.gmail.com', 587)
server.starttls()
username = '@gmail.com'
password = ''
server.login(username,password)

#below to prepare customer info
sendto = str(sys.argv[1])
#gender = 'Mr. '
#if str(sys.argv[2]) == 'Female':
#    gender = 'Mrs. '
#customerName = sendto[0:sendto.index('@')].capitalize()

#below to compose the email
salutation = 'Dear Customer,\n\n'
subject='COVID 19 Infection Alert'
body = 'It came to our attention that you had the potential to be infected by one of our customers while you were visiting our store at ' + sys.argv[4] + ' on ' + sys.argv[3] + ' \n\nTherefore, we highly recommend you to get a COVID 19 test as soon as possible.\n\nPlease follow proper steps to reduce your chance of spreading the virus to the community such as staying at home for the next 14 days, avoiding visiting with older adults or those with medical conditions.\n\nBest regards,\n\nDr.ESP32'
msg = 'From: ' +username+ '\nTo: ' + sendto + '\n'
msg = msg + 'Subject:' + subject + '\n' + salutation + body 
server.sendmail(username,sendto,msg)
server.quit()
