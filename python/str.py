index = 5
str = "photo.jpg"
name = str[:index]
suffix = str[index:]
print(f'name:{name}, suffix:{suffix}')

message = 'welcome to Python programming, Python is great'
sub_str = 'Python'
position = message.find(sub_str)
if position == -1:
    print(f'{sub_str} not in: {message}')
else:
    print(f'{sub_str} in: {message}, index is {position}')
