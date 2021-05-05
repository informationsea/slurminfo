slurminfo
=========

Show [SLURM](https://slurm.schedmd.com/) status as human friendly.

Example
-------

```
=================== Node List ===================
Partition: main        Nodes:   1  Status: Up
  Node: calcnode  State: Alloc  Job:  6  Mem: 7.4G/0.6G/7.6G  CPU: 6/6  Load:0.05
    Job: 121_865       CPU: 1 MEM:  0.1G User: testuser     Time: 00:05:27      Name: test.sh
    Job: 121_867       CPU: 1 MEM:  0.1G User: testuser     Time: 00:04:10      Name: test.sh
    Job: 121_868       CPU: 1 MEM:  0.1G User: testuser     Time: 00:03:31      Name: test.sh
    Job: 121_869       CPU: 1 MEM:  0.1G User: testuser     Time: 00:03:13      Name: test.sh
    Job: 121_870       CPU: 1 MEM:  0.1G User: testuser     Time: 00:01:26      Name: test.sh
    Job: 121_871       CPU: 1 MEM:  0.1G User: testuser     Time: 00:00:41      Name: test.sh

================= Job List =================
 ----------------- Running Jobs ----------------- 
     # Job ID          CPU   Memory  Partition Node      Running Time  User        Name
R    0 121_865           1     0.1G  main      calcnode      00:05:27  testuser    test.sh
R    1 121_867           1     0.1G  main      calcnode      00:04:10  testuser    test.sh
R    2 121_868           1     0.1G  main      calcnode      00:03:31  testuser    test.sh
R    3 121_869           1     0.1G  main      calcnode      00:03:13  testuser    test.sh
R    4 121_870           1     0.1G  main      calcnode      00:01:26  testuser    test.sh
R    5 121_871           1     0.1G  main      calcnode      00:00:41  testuser    test.sh
   Total: 6 jobs
 ----------------- Pending Jobs ----------------- 
     # Job ID          CPU   Memory  Partition    Wait Time  User       Reason                               Name
P    0 121_872-900       1     0.1G  main      22d 01:14:40  testuser   required resources not available     test.sh
P    1 121_359           1     0.1G  main      15d 03:30:38  testuser   required resources not available     test.sh
P    2 121_358           1     0.1G  main      15d 03:30:38  testuser   required resources not available     test.sh
P    3 121_357           1     0.1G  main      15d 03:30:38  testuser   required resources not available     test.sh
P    4 121_355           1     0.1G  main      15d 03:30:38  testuser   required resources not available     test.sh
P    5 121_352           1     0.1G  main      15d 03:30:38  testuser   required resources not available     test.sh
P    6 121_351           1     0.1G  main      15d 03:30:38  testuser   required resources not available     test.sh
P    7 1158_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P    8 1159_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P    9 1160_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P   10 1161_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P   11 1162_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P   12 1163_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P   13 1164_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P   14 1165_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P   15 1166_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
P   16 1167_1-900        1     0.1G  main      16d 01:42:20  testuser   higher priority jobs exist           test.sh
   Total: 17 jobs

=================== Partition List ===================
           Node        Total   Total  Executable Jobs(CPU)  Executable Jobs(Memory)   Executable Jobs(GPU)
Name       Avail Total   CPU   Mem(G) 1CPU 4CPU 8CPU 24CPU   4G  16G  64G  80G 170G      1 board  8 board
main           1     1     6        7    0    0    0     0    0    0    0    0    0            0        0

================= User List =================
                Running             Pending             Blocked             Error
User            Jobs   CPU  MEM(G)  Jobs   CPU  MEM(G)  Jobs   CPU  MEM(G)  Jobs   CPU  MEM(G)
testuser           6     6     0.6    17    17     1.7     0     0     0.0     0     0     0.0
```

Usage
-----

```
Usage: slurminfo [-dh?] [type]

Options:
  -d         : detail mode
  -h -?      : show this help
  -u username: show data for a user (only for normal mode. use * for all users)

Types in normal mode:
 * partitions
 * jobs
 * nodes
 * users

Types in detail mode:
 * partitions
 * jobs
 * nodes
 * slurmd
 * reservations
```

How to build
------------

1. Install compiler
2. Install SLURM development files
3. Run `make`

License
-------

GPL 3 or later
