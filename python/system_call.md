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
