http://blog.csdn.net/bonchoix/article/details/8292564
无论在游戏还是简单的动画程序当中，定时器是一个必不可少的元素，用来正确地控制游戏帧数、动画的播放。最简单的实现定时器的方法通过在主循环中定义两个静态计时用的变量即可实现。但在学习过程中，定时器的使用相当频繁，因此设计一个具备基本功能的方便使用的定时器类很有必要。这次就来创建一个简单但精度很高的定时器。

​       这个定时器支持暂时、开始、重置等基本操作，程序中每一帧调用一次计数，并随时返回距离上一次计数之间的差值。这个差值就是控制游戏、动画更新的最重要的数据。

​       这个定时器用到了Windows中高精度的计时函数：QueryPerformanceCounter和QueryPerformanceFrequency，原型如下：



1. BOOL QueryPerformanceCounter(  LARGE_INTEGER* lpPerformanceCount );  
2. BOOL QueryPerformanceFrequency(  LARGE_INTEGER* lpFrequency );  

​       这个定时函数以计数为单位，不同的机器两次计数之间的时间差可能不一样，因此有了第二个函数，返回一秒内计数的个数，求倒数即是两次计数之间的时间差（秒）。第个函数返回一个长整型数，通过传递参数地址来获得。详细说明参见MSDN，参考该定时器类的实现也可以很好的理解它们的使用。

​       下面是这个定时器的类定义：

**[cpp]** [view plain](http://blog.csdn.net/bonchoix/article/details/8292564#) [copy](http://blog.csdn.net/bonchoix/article/details/8292564#) 

1. class Timer  
2. {  
3. public:  
4. ​    Timer();  
5.   
6. ​    float TotalTime() const;    //返回定时器从开始计时到现在的总时间（不包含暂停时间），单位：秒  
7. ​    float DeltaTime() const;    //返回本次计时与上次计时之间的时间差，单位：秒  
8.   
9. ​    void Start();           //暂停后恢复计时  
10. ​    void Stop();            //暂停  
11. ​    void Reset();           //重置，开始计时，定时器开始工作时调用  
12. ​    void Tick();            //计时一次，每一帧调用一次  
13.   
14. private:  
15. ​    float   m_secsPerCount;     //系统有关，两个系统计数器之间的时间差值，单位：秒  
16. ​                    //这个变量是定时器把时间单位转换为“秒”的基准  
17. ​    float   m_deltaTime;        //距离上次计时之间的时间差，单位：秒  
18.   
19. ​    __int64 m_baseTime;     //定时器开始工作的起始时间点  
20. ​    __int64 m_stopTime;     //如果当前为暂停状态，即指暂时的起始时刻  
21. ​    __int64 m_pausedTime;       //暂停的总时间累计  
22. ​    __int64 m_currTime;     //本次计时的时间点  
23. ​    __int64 m_prevTime;     //上次计时的时间点  
24.   
25. ​    bool    m_stopped;      //是否暂停  
26. };  

​       注释已经十分详细，应该很容易明白各成员函数及变量的作用。

​       在构造函数中，主要任务是获得跟机器相关的变量，及m_secsPerCount，相临两次计数之间的时间差，以秒为单位。

​       在定时器开始工作前，需要先调用Reset，重置定时器，使定时器处于相应的工作状态，且设置好起始的参数。

​       其次，在程序每一帧中调用Tick来计时，更新定时器。

​       然后可以通过调用如TotalTime、DeltaTime函数来获得相关的时间值。其中DeltaTime()是最重要的函数，其返回的时间值是更新游戏每一帧及动画的核心参数。

​       通过调用Stop和Start来暂停和恢复定时器。

 

​       该定时器设计设计就这么简单，各成员函数的定义及详细的注释见源代码，通过源代码也可以更深入地理解用到的两个Windows定时函数。