# Ext4 anomalies

[![Build Status](https://travis-ci.com/demarkok/ext4-anomalies.svg?branch=master)](https://travis-ci.com/demarkok/ext4-anomalies)

The repository describes a bunch of anomalies we discovered in **ext4** file system.
The anomalies were observed on `Linux 5.0.0-38-generic, Ubuntu 18.04, ext4` and in **Travis CI** environment (see the label above).

---
## Build
```(bash)
make
./test.sh
```
---

## Anomaly #1: non-atomic `pwrite`
According to [POSIX.1-2017 (section 2.9.7)](https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_09_07), 
`pwrite` and `pread` system calls should be atomic (i.e. *each call shall either see all of the specified effects of the other call, or none of them*). But the following two-threaded program (taken from [StackOverflow](https://stackoverflow.com/questions/35595685/write2-read2-atomicity-between-processes-in-linux)) exhibit non-atomic behaviors (see `src/isatomic.cpp`)

Suppose that `buf = ["xxxxxx", "yyyyyy"]` and `file`  is a file descriptor

The first thread (the writer) alternatingly writes `xxxxxx` and `yyyyyy` into the file:
```python
i = 0
while True:
  pwrite(file, buf[i], 6, 0)
  i = (i + 1) % 2
```

The second thread (the reader) reads from the file and reports a problem in case something different from `xxxxxx` and `yyyyyy` was read:
```python
while True:
  rbuf = ...
  pread(file, rbuf, 6, 0)
  if rbuf != buf[0] and rbuf != buf[1]:
    ANOMALY_FOUND!
```
According to Posix, the second thread can read either `xxxxxx` or `yyyyyy` but in our environment we observed partial writes such as `yyyxxx`, `xyyyyy`, `yxxxxx`, etc.

## Anomaly #2: non-atomic `write`
According to [POSIX.1-2017 (section 2.9.7)](https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_09_07), `write` and `read` system calls also should be atomic. And they are, until they access the same regular file via different [`file`](https://elixir.bootlin.com/linux/v5.5.7/source/include/linux/fs.h#L935) structures (e.g. when the file descriptors are aquired in two different `open` calls). That's because `write` and `read` calls [aquire `file->f_pos_lock`](https://elixir.bootlin.com/linux/v5.5.7/source/fs/file.c#L801). This feature was added in Linux v3.14 to guarantee atomicity w.r.t. file offset (`f_pos`) accesses, since `write` and `read` change it (see [Linus Torvalds' email](https://lkml.org/lkml/2014/3/3/533)).
