### 2023夏季学期大作业（贪吃蛇）

**成员组成: Luoxx, Zhaoxx**


#### <font color = red>附加功能概要 </font>
1. 框架的初步改变
2. 实现奖励机制（加2分，概率50%）
3. 实现加速[^speed]机制（按同蛇运动方向实现加速，反向取消加速）
4. 实现障碍[^障碍]添加，初始为3个，随难度增加而递增
5. 实现生命功能，初始生命为2条，一个生命果实加一分和一条生命
6. **双人模式**[^double]：字母操作第一条蛇，方向键操作第二条蛇
7. 优化初始化界面
8. 实现帮助界面（help）：介绍建筑的基本作用
9. 实现暂停，`space, Esc, Enter`三个键位可以实现游戏暂停
10. 实现游戏计时：计算游戏进行时间

  
[^speed]:只限于单人模式，加速模式会于界面左上角显示
[^障碍]:只限于单人模式，碰撞时处于加速状态则分数减一，否则生命减一
[^double]:此模式下没有障碍和加速功能,最终活着的蛇胜利，同时死亡则分数高者胜


#### 编译环境要求

源代码放于[GitHub](https://github.com/Walker-luo/SnakeGame "SnakeGame"), 主要部分是Windows和Linux分支（main分支主要记录进度）


提供2种环境的选择（`Windows和Linux`）
* `Windows`[^Windows]:codeblocks配置好`"curse.h"`[^set]后可以编译运行（不提供可执行文件）
* `Linux`[^Linux]：需要安装[PDCurses库](https://pdcurses.org/ "PDCurses")，提供2个链接学习相关函数：[链接1](https://pdcurses.org/docs/MANUAL.html) , [链接2](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html)
 可以使用以下命令安装此库：`sudo apt-get install libncurses-dev`
    *下载文件后进入文件所在输入 `make` 后 输入`./snakegame`即可运行游戏*
  
[^set]:配置文件也和源代码一同放置
[^Windows]:提供源代码，`curse.h`，codeblocks配置教程
[^Linux]: 提供源代码，Makefile
