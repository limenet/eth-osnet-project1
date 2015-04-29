# eth-osnet-project1
Repo for Project 1 of the Operating Systems & Networks taught at ETH Zürich in Spring 2015

## Terminal

```bash
# terminal 1
linus@ubuntu-VirtualBox:~/Desktop/eth-osnet-project1$ ./reliable 6667 -w 5 -d localhost:6666
# terminal 2
linus@ubuntu-VirtualBox:~/Desktop/eth-osnet-project1$ ./reliable 6666 -w 5 -d localhost:6667

# check ports etc.
$ sudo netstat -upa | grep reliable
```
