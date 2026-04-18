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

text = 'I love C++ and C++ is powerful'
new_text = text.replace('C++', 'python')
print(new_text)
new_text = text.replace('C++', 'python', 4)
print(new_text)
text.replace('ans', '&')
print(text)

'''
split
'''
date_str = '2024-03-15'
date_list = date_str.split('-')
print(date_list)

fruits = ['apple', 'banana', 'orange']
result = '-'.join(fruits)
print(result)
words = ['Hello', 'Python', 'World']
words = '.'.join(words)
print(words)
