# wcc - linecounter

wcc.exe is a Windows console application.

You can enter the C/C++ or Python source code and get the total number of lines and the number of lines excluding comments, in a format similar to 'wc -l' format.

example)
```
$ wcc *.cpp
   1044     672 A.cpp
    496     351 B.cpp
    110      75 C.cpp
   1650    1098 total
```

You can use a -t option, it outputs the total lines and lines_x_comments only.
```
$ wcc -t *.cpp
   1650    1098
```

For python code, the --python option is useful.
```
$ wcc -t --python *.py
   2500    1722
```

### Note
Strings enclosed in double quotation marks are not checked to parse comments.
