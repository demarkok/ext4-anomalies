# Ext4 anomalies

[![Build Status](https://travis-ci.com/demarkok/ext4-anomalies.svg?branch=master)](https://travis-ci.com/demarkok/ext4-anomalies)

The repository describes a bunch of anomalies we discovered in **ext4** file system.
The anomalies were observed on `Linux 5.0.0-38-generic, Ubuntu 18.04, ext4` and in **Travis CI** environment (see the label above).

---

## Anomaly #1: non-atomic `pwrite`
According to [POSIX.1-2017 (section 2.9.7)](https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_09_07), 
`pwrite` and `pread` system calls should be atomic (i.e. *each call shall either see all of the specified effects of the other call, or none of them*). But the following two-threaded program (taken from [StackOverflow](https://stackoverflow.com/questions/35595685/write2-read2-atomicity-between-processes-in-linux)) exhibit non-atomic behaviors (see `src/isatomic.cpp`)

Suppose that `buf = ["xxxxxx", "yyyyyy"]` and `file`  is a file descriptor

The first thread (the writer) alternatingly writes `xxxxxx` and `yyyyyy` into the file:
```python
i = 0
while True:
  pwrite(file, buf[i], SIZE, 0)
  i = (i + 1) % 2
```

The second thread (the reader) reads from the file and reports a problem in case something different from `xxxxxx` and `yyyyyy` was read:
```python
while True:
  rbuf = ...
  pread(file, rbuf, SIZE, 0)
  if rbuf != buf[0] and rbuf != buf[1]:
    ANOMALY_FOUND!
```
According to Posix, the second thread can read either `xxxxxx` or `yyyyyy` but in our environment we observed partial writes such as `yyyxxx`, `xyyyyy`, `yxxxxx`, etc.
