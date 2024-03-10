# 使用方法

## 开发环境

- OS: Ubuntu 22.04
- Language: C++ 17
- Compiler: GCC 11.4.0
- Debugger: GDB 12.1
- Build: CMake 3.22.1
- Dependent: 
    - fmt
    - protobuf
 
## 配置文件说明

基本的配置文件格式如下：

```json
{
    "thread": {},
    "log": {},
    "network": {},
    "registry": {},
    "database": {}
}
```

分别为**线程池配置**、**日志器配置**、**网络配置**、**注册中心配置**、**数据库配置**。

### 线程池配置

可配置参数如下：

| 可配置项        | 数据类型 | 默认参数             | 说明               |
| --------------- | -------- | -------------------- | ------------------ |
| init_thread_num | Number   | 根据硬件情况动态分配 | 初始线程数量       |
| max_thread_num  | Number   | 200                  | 最大线程数量       |
| max_task_num    | Number   | 1024                 | 最大任务数量       |
| dynamic_mode    | Boolean  | false                | 开启动态线程池模式 |

### 日志配置

可配置参数如下：

| 可配置项  | 数据类型 | 默认参数 | 说明                                                         |
| --------- | -------- | -------- | ------------------------------------------------------------ |
| level     | String   | info     | 全局日志等级                                                 |
| formatter | String   | %#       | 全局日志格式化                                               |
| default   | String   |          | 默认日志器的名称，如果不存在则保留原始的默认日志器 |
| terminal  | Array    |          | 终端字体样式属性设置                                             |
| logger    | Array    |          | 日志器                    |

可选的日志等级如下：

- `trace`：追踪级别
- `debug`：调试级别
- `info`：信息级别
- `warn`：警告级别
- `error`：错误级别
- `fatal`：故障级别(直接退出应用程序)
- `off`：关闭日志

可用的格式化选项如下：

| 格式化标志 |        格式化内容        |                             示例                             |
| :--------: | :----------------------: | :----------------------------------------------------------: |
|     %a     |        工作日简写        |                             Sun                              |
|     %A     |        工作日全称        |                            Sunday                            |
|     %b     |         月份简写         |                             Dec                              |
|     %B     |         月份全称         |                           December                           |
|     %c     |     标准的日期字符串     |                   Tue Aug 31 10:15:42 2021                   |
|     %C     |         日志名称         |                        default_logger                        |
|     %d     |         月/天/年         |                           12/25/21                           |
|     %D     | 十进制表示的每月的第几天 |                              25                              |
|     %e     |    十进制表示的毫秒数    |                             943                              |
|     %E     |          线程号          |                            33423                             |
|     %f     |    十进制表示的微秒数    |                            943265                            |
|     %F     |         年-月-日         |                          2020-12-25                          |
|     %g     |   自纪元时间以来的秒数   |                          1708769224                          |
|     %G     |        源文件名称        |                           main.cpp                           |
|     %h     |      24小时制的小时      |                              23                              |
|     %H     |      12小时制的小时      |                              11                              |
|     %i     |  源文件的完整或相对路径  |                 /home/user/project/main.cpp                  |
|     %I     |      源文件的所在行      |                             134                              |
|     %j     | 十进制表示的每年的第几天 |                             342                              |
|     %k     |    源文件名称:所在行     |                         main.cpp:134                         |
|     %K     |  完整源文件名称:所在行   |               /home/user/project/main.cpp:134                |
|     %l     |       日志级别简写       |                              I                               |
|     %L     |       日志级别全称       |                             info                             |
|     %m     |    十进制表示的分钟数    |                              44                              |
|     %M     |     十进制表示的月份     |                              5                               |
|     %n     |          换行符          |                              \n                              |
|     %N     |    十进制表示的纳秒数    |                          943265487                           |
|     %p     |  本地的AP或PM的等价显示  |                              AM                              |
|     %P     |          进程号          |                            13324                             |
|     %r     |     12小时制的时分秒     |                         08:30:45 AM                          |
|     %R     |   小时和分钟表示的时间   |                            14:30                             |
|     %s     |     十进制表示的秒数     |                              42                              |
|     %t     |        水平制表符        |                              \t                              |
|     %T     |     24小时制的时分秒     |                           14:35:50                           |
|     %v     |         日志内容         |                       some message...                        |
|     %y     |   不带世纪的十进制年份   |                              21                              |
|     %Y     |    带世纪的十进制年份    |                             2021                             |
|     %z     |         时区简写         |                             CST                              |
|     %%     |          百分号          |                              %                               |
|     %#     |         默认格式         | [2020-12-20 14:32:45.867] [logger_name] [info] [334523] xxxxx |

#### 终端字体属性配置

可配置参数如下：

| 可配置项   | 数据类型 | 默认参数 | 说明             |
| ---------- | -------- | -------- | ---------------- |
| level      | String   |          | 要设定的日志等级 |
| foreground | String   | Default  | 前景色           |
| background | String   | Default  | 背景色           |
| bold       | Boolean  | false    | 加粗             |
| italic     | Boolean  | false    | 斜体             |
| underline  | Boolean  | false    | 下划线           |

可选颜色如下：

- `default`：默认
- `black`：黑色
- `red`：红色
- `green`：绿色
- `yellow`：黄色
- `blue`：蓝色
- `purple`：紫色
- `cyan`：青色
- `white`：白色

#### 日志器配置

可配置参数如下：

| 可配置项  | 数据类型 | 默认参数   | 说明       |
| --------- | -------- | ---------- | ---------- |
| name      | String   |            | 日志器名称 |
| formatter | String   | 全局格式化 | 格式化     |
| level     | String   | 全局等级   | 日志等级   |
| appender  | Array    |            | 日志输出地 |

#### 日志输出器配置

对于日志输出器，分别有三种形式：控制台(`console`)、文件(`file`)、旋转文件(`rotating_file`)，它们分别可以进行不同形式的配置。在配置时必须指定相对应的输出器类型，即 `type` 字段。

控制台可配置参数如下：

| 可配置项     | 数据类型 | 默认参数     | 说明               |
| ------------ | -------- | ------------ | ------------------ |
| type         | String   |            | 输出器类型，必须指定为`console`|
| level        | String   | 日志器等级   | 日志等级           |
| formatter    | String   | 日志器格式化 | 格式化             |
| multi_thread | Boolean  | true         | 是否支持多线程     |

文件可配置参数如下：

| 可配置项     | 数据类型 | 默认参数     | 说明                       |
| ------------ | -------- | ------------ | --------------------- |
| type         | String   |            | 输出器类型，必须指定为`file` |
| level        | String   | 日志器等级   | 日志等级                  |
| formatter    | String   | 日志器格式化 | 格式化                    |
| multi_thread | Boolean  | true         | 是否支持多线程           |
| filename     | String   |              | 日志文件名称，必须指定    |
| rewrite      | Boolean  | true         | 每次重启应用时是否覆写文件 |
| async        | Boolean  | false        | 是否支持异步输出         |

旋转文件可配置参数如下：

| 可配置项      | 数据类型 | 默认参数      | 说明            |
| ------------- | -------- | ------------- | ------------------- |
| type          | String   | rotating_file | 输出器类型，必须指定为`rotating_file` |
| level         | String   | 日志器等级    | 日志等级            |
| formatter     | String   | 日志器格式化  | 格式化              |
| multi_thread  | Boolean  | true          | 是否支持多线程         |
| filename      | String   |               | 日志文件名称，必须指定  |
| max_file_num  | int      |               | 最大文件数目，必须指定  |
| max_file_size | int      |               | 最大文件大小，单位为字节，必须指定 |
| async         | Boolean  | false         | 是否支持异步输出       |

> 如果日志文件名称为 `log.txt`，且最大文件数量为3，那么会按照如下顺序依次旋转输出日志：log.txt ==> log.1.txt ==> log.2.txt ==> log.txt

### 网络配置

网络配置主要用于配置服务器相关参数，其可配置参数如下：

| 可配置项    | 数据类型 | 默认参数  | 说明             |
| ----------- | -------- | --------- | ------------ |
| ip          | String   | 127.0.0.1 | 对客户端暴露的IP地址 |
| port        | Number   | 8000      | 端口号        |
| name        | String   | RpcProvider | 服务器名称     |
| reuse_port  | Boolean  | false     | 是否复用端口号   |
| loopback_onley | Boolean | false | 是否仅监听本地地址 |
| subloop_num | Number   | 3         | 子事件循环的数量 |

### 注册中心配置

注册中心配置主要用于配置注册中心的相关参数，便于RPC服务提供者和RPC服务请求者与注册中心进行信息交互。其可配置参数如下：

| 可配置项    | 数据类型 | 默认参数  | 说明             |
| ----------- | -------- | --------- | ------------ |
| ip          | String   | 127.0.0.1   | IP地址        |
| port        | Number   | 8888      | 端口号        |

### 数据库配置

数据库配置主要用于配置数据库连接池，其可配置参数如下：

| 可配置项    | 数据类型 | 默认参数  | 说明             |
| ----------- | -------- | --------- | ------------ |
| max_idle_interval | Number   | 50000 | 连接的最大空闲时间，单位ms |
| connect_timeout   | Number   | 1000  | 连接的最大超时时间，单位ms |
| mysql             | Object   |       | MySQL配置，不可为空 |
| redis             | Object   |       | Redis配置，可为空 |

> 不使用数据库连接池时请不要在配置文件中出现 `database` 字段，如果出现则表示启用数据库连接池，则必须配置 `mysql` 字段。

#### MySQL配置

`mysql` 字段主要用于配置 MySQL 数据库的相关配置，其可配置参数如下：

| 可配置项   | 数据类型 | 默认参数  | 说明             |
| ----------- | -------- | --------- | ------------ |
| init_size | Number  | 2           | 连接的初始数量 |
| max_size  | Number  | 20          | 连接的最大数量 |
| port      | Number  | 3306        | MySQL服务器端口号 |
| ip        | String  | "127.0.0.1" | MySQL服务器IP地址 |
| username  | String  | "root"      | 用户名 |
| password  | String  |             | 密码，必须配置 |
| database  | String  |             | 数据库名，必须配置 |

#### Redis配置

| 可配置项   | 数据类型 | 默认参数  | 说明             |
| ----------- | -------- | --------- | ------------ |
| init_size | Number  | 2           | 连接的初始数量 |
| max_size  | Number  | 20          | 连接的最大数量 |
| port      | Number  | 6379        | Redis服务器端口号 |
| ip        | String  | "127.0.0.1" | Redis服务器IP地址 |
