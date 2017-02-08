Python - Capturing output from a system call

```python
from subprocess import check_output
a = check_output('ls *.py', shell=True)
print a
```

```text
test1.py
test2.py
...
```

_see: https://docs.python.org/3/library/subprocess.html#subprocess.check_output_
