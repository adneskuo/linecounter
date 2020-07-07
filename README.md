# wcc - linecounter

wcc.exe is a Windows console application.

You can enter the C/C++ source code and get the total number of lines and the number of lines excluding comments, in a format similar to 'wc -l' format.

example)
```
$ wcc *.cpp
   1044     672 A.cpp
    496     351 B.cpp
    110      75 C.cpp
   1650    1098 total
```

### Note
Strings enclosed in double quotation marks are not checked to parse comments.
