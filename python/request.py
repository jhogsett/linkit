import requests

r = requests.get('https://www.google.com') #, auth=('user', 'pass'))

print r.status_code
print r.headers['content-type']
