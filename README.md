# IIC_Software
软件模拟IIC时序

开始信号：SCL在高电平模式时，SDA由高变低
停止信号：SCL在高电平时，SDA由低变高
应答信号：发送完一个数据之后，SCL在高电平时，SDA为低电平则接受应答成功

往AT24C02中写数据时序：
开始信号 -- 器件地址+写命令 -- 等待应答 -- 写入存储地址 -- 等待应答 --写入数据 -- 等待应答 -- 停止信号 -- 延时10ms（防止写数据时时序变化）

从AT24C02中读取数据时序：
开始信号 -- 器件地址+写命令 -- 等待应答 -- 写入存储地址 -- 等待应答 --开始信号 -- 读命令 -- 等待应答 -- 读取数据 -- 发送应答1 -- 停止信号
