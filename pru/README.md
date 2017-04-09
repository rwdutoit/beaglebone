Follow steps on:

https://github.com/ZeekHuge/BeagleScope

with additional/replacemnt step from:

https://www.zeekhuge.me/post/ptp_blinky/

mkdir /usr/share/ti/cgt-pru/bin

$ ln -s /usr/bin/clpru /usr/share/ti/cgt-pru/bin/clpru
$ ln -s /usr/bin/lnkpru /usr/share/ti/cgt-pru/bin/lnkpru


export PRU_CGT=/usr/share/ti/cgt-pru
If you want this to be done automatically when you start a terminal on bbb, just add the above line to ‘~/.bash.rc’. Once all this is done, you can test your setup: 




Kernel module message:

tail -f /var/log/kern.log
