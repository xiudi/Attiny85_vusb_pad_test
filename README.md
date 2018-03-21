# Attiny85_vusb_pad_test
start


更加专业一些的玩法：

1 XD002项目的所有程序部分是开源的。而且为了防止一些恶意宏，打字机功能在固件层级禁止了所有windows功能键组合。

（如有以下做法将不再提供售后服务，请玩家谨慎决定。）

2 XD002兼容Arduino（Digispark）。按住靠近usb的按键插入usb即可进入bootloader（需要micronucleus的驱动）。你将有6.5k的可用flash空间和512k的eeprom。当然一旦如此，原始固件将被覆盖。进入bootloader备份原始固件的方法和Digispark一样（都采用micronucleus作为bootloader，不过XD002版本高一些）。备份固件有少许失败概率。自写代码的朋友请确保自己的代码安全性，避免芯片烧毁。

3 XD002没有锁Reset。不需要高压解锁就可使用ISP。这里推荐avrdude作为上位机。相信用到这里的都有一定的avr单片机基础。这里要用到主板烧录夹这种工具。劣质的夹子有大概率烧毁芯片。建议先切断pcb上的跳线再上夹子（跳线在芯片这一面，断开ws2812的信号输入）。如果要魔改供电，要记得修改对应的fuse。

4 因为PCB太小，取出时必须断电，而且要注意防止静电击穿。



How to change key values and LED color:

1. Click on “Keyboard” button on the title
bar, select “XD002”. (Keyboard->XD002)

2. Click on the button icon that you want
to change, and then select the desire key value from the chart at lower right
corner. Click RGB on the title to change color config . If you want to use
Rainbow LED, select RGB->Rainbow on the title bar. Otherwise, choose
RGB->Fixed color.

3. XD002 provide two layers (layer 0 and
layer 1), you can click radio button on the title bar to alter the key values
of another layer.

4. Click on “Matrix” and select “Upload
Matrix” to save changes. 

 

How to alter  the printer content:

Input the text content in Print
Box.  The print box is at the lower left
corner. Click on “Printer” on the title
bar, select “Upload with GBK” to save the changes. Try to select “Upload with Unicode”
If print garbled Korea.





