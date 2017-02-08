Python Jinja2 Templating Engine

http://jinja.pocoo.org/docs/2.9/api/#

    $ pip install Jinja2

Create file test.html

    Hello!
    {% set a = 'test'%}
    {% print a %}
    {% print b %}

$ python

>>> 
>>> from jinja2 import Environment, FileSystemLoader, select_autoescape
>>> env = Environment(
>>>     loader=FileSystemLoader('./'),
>>>     autoescape=select_autoescape(['html', 'xml'])
>>> )
>>> template = env.get_template('test.html')
>>> 
>>> d = dict(b='hellasdf')
>>> template.render(d)
u'Hello!\n\ntest\nhellasdf'
>>> print template.render(d)
Hello!

test
hellasdf
>>> 
