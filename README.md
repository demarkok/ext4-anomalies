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
`pwrite` and `pread` system calls should be atomic (i.e. *each call shall either see all of the specified effects of the other call, or none of them*). But the following two-threaded program (taken from [StackOverflow](https://stackoverflow.com/questions/35595685/write2-read2-atomicity-between-processes-in-linux)) exhibits non-atomic behaviors:

Suppose that `buf = ["xxxxxx", "yyyyyy"]` and `file`  is a file descriptor

The first thread (the writer) alternatingly writes `xxxxxx` and `yyyyyy` into the file:
```python
# THREAD 1

i = 0
while True:
  pwrite(file, buf[i], 6, 0)
  i = (i + 1) % 2
```

The second thread (the reader) reads from the file and reports a problem in case something different from `xxxxxx` and `yyyyyy` was read:
```python
# THREAD 2

while True:
  rbuf = ...
  pread(file, rbuf, 6, 0)
  if rbuf != buf[0] and rbuf != buf[1]:
    ANOMALY_FOUND!
```
According to Posix, the second thread can read either `xxxxxx` or `yyyyyy` but in our environment we observed partial writes such as `yyyxxx`, `xyyyyy`, `yxxxxx`, etc.

**(see `src/na_pwrite.cpp`)**

## Anomaly #2: non-atomic `write`
According to [POSIX.1-2017 (section 2.9.7)](https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_09_07), `write` and `read` system calls also should be atomic. And they are, until they access the same regular file via different [`file`](https://elixir.bootlin.com/linux/v5.5.7/source/include/linux/fs.h#L935) structures (e.g. when the file descriptors are aquired in two different `open` calls). That's because `write` and `read` [aquire `file->f_pos_lock`](https://elixir.bootlin.com/linux/v5.5.7/source/fs/file.c#L801). This feature was added in Linux v3.14 to guarantee atomicity w.r.t. file offset (`f_pos`) accesses, since `write` and `read` change it (see [Linus Torvalds' email](https://lkml.org/lkml/2014/3/3/533)).

The following two-threaded program exhibits non-atomic reads.

Suppose that the main thread opens the same file twice and aquire two descriptors `fd1` and `fd2`, then it initialize the file with `xxxxxx`, resets the offsets to 0 and runs the *writer* and the *reader* threads in parallel:

```python
# MAIN THREAD

fd1 = open("file.txt", ...)
fd2 = open("file.txt", ...)

while True:
  pwrite(fd1, "xxxxxx", 6, 0)
  lseek(fd1, 0, SEEK_SET)
  lseek(fd2, 0, SEEK_SET)
  RUN_WRITER_THREAD()
  RUN_READER_THREAD()
  JOIN()
```
The writer writes `yyyyyy` to the file via `fd1`:

```python
# THREAD 1 (WRITER)

write(fd1, "yyyyyy", 6)
```

The reader reads from the file via `fd2` and reports a problem in case of reading an intermediate value:
```python
# THREAD 2 (READER)

rbuf = ...
read(fd2, rbuf, 6)

if rbuf != "xxxxxx" and rbuf != "yyyyyy":
    ANOMALY_FOUND!
```

**(see `src/na_write.cpp`)**
