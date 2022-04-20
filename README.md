# Operation_System

* ## [Overview](#001) #
* ## [OS Structure](#002) #
* ## [Process Concept](#003)
* ## [Thread](#004) #
* ## [CPU Scheduling](#005) #
* ## [Synchronization](#006) #
* ## [Deadlock](#007) #
* ## [Memory Management](#008) #
* ## [Virtual Memory](#009) #
* ## [Mass-Storage Structure](#010) #
* ## [I/O Systems](#011) # 
* ## [File-System Interface](#012) # 
* ## [File-System Implementation](#013) # 
* ## [File-System Internals](#014) # 
* ## [Security](#015) # 
* ## [Protection](#016) # 
* ## [Virtual Machines](#017) # 
* ## [pass](#018) # 




****




<h1 id="001">Overview</h1> 

  * ## [Definition](#0011) #
  * ## [History](#0012) #
  * ## [Interrupt](#0013) #
  * ## [Timer](#0014) #
  * ## [Duel-Mode Operation](#0015) #
  * ## [Component 名詞定義](#0016) #
  * ## [Memory Access Architecture](#0017) #
 
 
 
 
<h2 id="0011">Definition</h2> 

  *  **Resource allocator**： 能有效的分配及管理資源，並且滿足公平性，即時性等要求。 
  *  **Control program**： 控制user program 的執行以及控制hardware，避免電腦因不當操作產生error 或崩潰。
  *  **Kernel**： 一個always running 的核心程式。
  *  作業系統是屬於event-driven(事件驅動)，也就是他依據命令、應用程式、IO devices以及interrupt來工作。 
<h2 id="0012">History</h2>   

  * 演進： Batch -> Multi-programming -> Time-sharing 
    * batch： 一次只能一個user 執行一個process
    * Multi-programming： 一次可以執行多個process，不會因為IO 而idle 在那邊，使用的是interrupt 的技術，但是沒有跟user 的interactive
    * Time-sharing： 可以來回在很多個process 之間切換，讓人產生好像一次執行多個process的錯覺，可以interactive。  
    * [更多詳細資訊](https://medium.com/@a131401203/%E4%B8%AD%E6%96%87%E7%B3%BB%E4%B9%8B%E4%BD%9C%E6%A5%AD%E7%B3%BB%E7%B5%B1%E6%BC%94%E9%80%B2%E5%8F%B2-610986e9ee3c)

<h2 id="0013">Interrupt</h2>   

* 對於傳統的電腦，IO的運作方式是驅動程式會先載入到控制器中適合的暫存器，然後controler會從暫存器的內容來決定要進行的動作。
  * ![cONTROLER](https://github.com/a13140120a/Operation_System/blob/main/imgs/controler.PNG)
* 現代OS大部分都是Interrupt-driven IO的方式執行IO
  * cpu 發出request 使IO devices工作，自己則可以去執行其他指令，等到controler執行完之後會通知cpu，然後cpu才會繼續處理
* 當CPU發生Interrupt時，會停止正在執行的工作，並依照Interrupt的種類轉換到一個的位址，而這個位址通常是interrupt service routine(ISR, 中斷服務常式，或稱中斷處理器interrupt handler)的起始位址。
* interrupt vector(中斷向量)：
  * 中斷向量是處理中斷的一種技術，它會通知中斷處理程序在哪裡找到 ISR的地址，所有的interrupt都會有一個number，通常是0~255，每個interrupt都會對應到一個interrupt vector。
  * 使用這項技術是為了避免「跳轉到ISR之後還要搜尋並確定是哪一種中斷」這種老舊的技術造成時間上的浪費。
  * 中斷向量表(interrupt vector table)通常位於memory的000000H/~0003FFH的前1024個bytes，它包含 256 個不同的中斷向量。每個向量長 4 個bytes，並且包含了ISR的起始位址
  * 中斷向量表中，前五個向量從8086到Pentium都是相同的，並且intel保留前32個中斷向量供其在各種微處理器系列成員中使用。最後224個向量可用作用戶中斷向量。
  * ISR的起始位址由段(CS內容)和偏移量(IP內容，IP就是intel的PC)組成
  * Hook就是利用安裝interrpt vector來達成的，使用此技術組譯器必須知道實體記憶體的絕對地址。
  * 下圖為intel 8086微處理器的interrupt vector table。
  * ![8086_Interrupts_vec_table](https://github.com/a13140120a/Operation_System/blob/main/imgs/8086_Interrupts_vec_table.jpg)
* interrupt chaining(中斷串鏈)：
  * 當interrupt vactor太多時(超過256個)，就會使用到interrupt chaining的技術
  * 每一個interript vector都指向一個由ISR組成的linked-list，**而不是直接指向ISR**。當有interrupt發生時，會先找到interrupt vector，然後再traverse整個list。
  * 這是在效率與節省空間之間折衷的方法。
* Interrupt Request Lines(中斷請求線)：大部分的cpu會有兩條Interrupt Request Lines, 一條是maskable, 一條是nonmaskable。
  * 當cpu偵測到Interrupt Request Line 發出interrupt時，會讀取interrupt的編號，跳轉到interrupt vector中相對應的routine
* Interrupt prioriy level：
  * priority較高的在處理的時候，priority較低的會被mask掉，相對的priority較低的在處理的時候若有priority較高的interrupt出現則會先被執行。
  * 通常會有一個priority arbiter(優先權仲裁器)的電路來決最高優先權的硬體
* interrupt grant(中斷同意線)：
  * priority arbiter決定好某硬體後,用來傳送中斷同意信號給該硬體
* 運作過程：
  * 當有事件發生時，process會透過interrupt request向priority arbiter發出中斷信號,並進入等待狀態
  * priority arbiter收到一個或多個中斷信號後,使用priority arbiter電路決定最高優先權的硬體
  * priority arbiter傳送中斷同意信號給最高優先權的硬體,並清除interrupt request上的中斷信號
  * 如果使用interrupt vector的話，當硬體收到中斷同意信號後再送出interrupt vector(中斷向量)給cpu
  * cpu到interrupt vector table(中斷向量表)被指定的欄位取出ISR起始位址並執行指定ISR

* 處理完interrupt之後執行return_from_interrupt並返回之前的狀態。
* 有心人士有可能會透過修改interrupt routine 或者是interrupt vector來達成偷取資料(或其他破壞行為)的目的
* Hardware 發出的**signal**導致interrupt, 而software 則是發出error，或是透過system call導致interrupt
* Software的interrupt也叫做trap或exception，software產生interrupt之後可以跳轉到自己定義的module當中
* [更多](#0112)


<h2 id="0014">Timer</h2>   

* 通常在mother board上
* 每隔一段時間會導致interrupt
* 避免cpu陷入無窮迴圈或卡死
* 用來implement Time-sharing
* 透過load-timer這個privileged instruction可以修改間隔時間

<h2 id="0015">Duel-Mode Operation</h2> 

* 為了防止user program摧毀硬體，OS會分成兩種mode： 分別是user mode跟kernel mode
* 有Hardware support的硬體會有一個bit, 當進入到kernel mode的時候OS會先把這個bit設為0，回到user mode之後會再設回1。
* 只要發生interrupt就代表進入kernel mode
* Privileged instruction只能在kernel mode 時候執行，在cpu設計的時候privieleged instruction就已經定義好了
* 一但在user mode執行priviledged instruction cpu會通知OS使OS產生錯誤訊息並阻擋執行。
* Privileged instruction只能藉由呼叫System Call之後OS觸發interrupt然後轉換到kernel mode執行。

<h2 id="0016">Component 名詞定義</h2>   

* CPU： 執行instruction的硬體,通常內含暫存器以及L1 cache
* Processor： 一個實體晶片，包含一個或多個CPU
* Core： CPU的基本計算單位，或CPU中執行指令和暫存器儲存資料的地方
* Muti-Core： 同一個CPU上包含多個Core
* ![DualCore](https://github.com/a13140120a/Operation_System/blob/main/imgs/DualCore.jpg)

<h2 id="0017">Memory Access Architecture</h2> 

* Uniform Memory Access (UMA)：
  * 適合[SMP](#0054)架構
  * 每個processor accesses memory的速度都是一樣的，程式師不用在意程式執行在哪個processor，因為都一樣
* Non-Uniform Memory Access (NUMA)：
  * 是一個hierarchy的架構，通常由許多SMP架構組成，可以建構更大的架構
  * 每個processor accesses memory的速度不一樣，local的比較快，remote access會較慢
  * 程式師需管理process run在哪個processor以提升效能
* ![NUMA](https://github.com/a13140120a/Operation_System/blob/main/imgs/NUMA-Architecture.png)

****




<h1 id="002">OS structure</h1> 

  * ## [Services](#0021) #
  * ## [System Call](#0022) #
  * ## [API](#0023) #
  * ## [loader and linker](#0024) #
  * ## [OS設計與製作](#0025) #
  * ## [Structure](#0026) #
  * ## [系統啟動](#0027) #
  * ## [除錯](#0028) #




<h2 id="0021">Services</h2>   

* 一套作業系統服務通常會提供下列功能：
  * UI(使用者介面：User Interface)：
    * GUI： 圖形化介面(graphical user interface)
    * CLI： 命令行介面(command-line interface)，
      CLI通常會有一個命令解譯器(command interperter)來解譯指令  
      shell就是一種command interperter  
      command interperter有分兩種，一種是本身含有執行指令的程式碼，另一種是利用尋找檔案的方式(UNIX系統)
    * touch-screen interface
  * Program execution
  * I/O operations
  * File systemmanipulation
  * communication： 
    * error detection： zero devision, over flow等等
  * resource allocation： 分配memory, cpu等等
  * accounting(keep track紀錄等等)
  * Protection ans Security
    * protection： 保護devices不會被互相干擾或者被user破壞
    * security： 保護電腦或系統不被有心人士不當使用

<h2 id="0022">System Call</h2>   

* System Call： 提供作業系統服務的介面，一般以C/C++或組合語言撰寫，其類型可分為以下幾種(以下的system call皆為通用的例子，每種OS的system call都不盡相同)：
  * process control： 
    * 執行中的process可能會正常終止(`end()`)或不正常終止(`abort()`)，
    * 也可能需要載入(`load()`)或執行(`execute()`)另一個process, 
    * 如果是mutiprocess的話就會有(`create_process()`)然後把原本程式在記憶體內的**image**保存起來，
    * 為了控制process，例如priority，或執行時間，我們必須要有(`get_process_attributes()`)或(`set_process_attributes()`)，
    * 我們可能想要終止程式(`terminate_process()`)
    * 或者等待事件發生(`wait_event()`)，等待時間到(`wait_time()`)，當事件發生時process應該發出訊號(`signal_event()`)。
    * mutiprocess的process之間含需要有能夠保護資源的lock，通常會包含(`acquire_lock`)和(`release_lock()`)
  * file management： 
    * 首先需要能夠建立(`create()`)和刪除檔案(`delete()`)
    * 建立了檔案之後需要開啟(`open()`)、讀(`read()`)寫(`write()`)及重定位(`reposition`)(檔案倒轉或跳到結尾)，之後還會需要關閉(`close()`)檔案。
    * 檔案的屬性包含檔案名稱、檔案類型、保護碼(protection)、帳號資訊等等
    * 為了此目的還需要取得檔案屬性(`get_file_attributes()`)，設定檔案(`set_file_attributes()`)
  * devices management
    * 裝置可以是實體的(硬碟或其他設備)或者抽象的(檔案)
    * 多使用者的系統需要先對裝置提出(`request()`)，使用完之後就必須將其釋放(`release()`)
    * 一旦request device之後，就可以做讀(`read()`)寫(`write()`)及重定位(`reposition`)，
    * 因為IO devices與檔案系統非常相似，有些作業系統(例如UNIX)會將兩個併在一起，一system call會同時用在檔案跟device上，將IO devices 定義成特殊的檔案或目錄
  * information maintenance：
    * 大多數系統都會有傳回時間(`time()`)和日期(`date()`)的system call
    * 也有一些事傳回系統版本、可用記憶體空間或磁碟空間等等
  * communication：
    * message-passing model：
      * 較慢，適合傳輸較小的數據，需要經過kernel傳遞到另一個process
      * 如同網路之間有hostname, IP address, Process之間也會有process name會轉換成 processID
      * 作業系統會根據id找到該process, 例如(`get_processid()`)以及網路的(`get_hostid()`)
      * id會傳入(`open()`)或者(`close()`)的system call，或傳入(`open_connection()`)或(`close_connection()`)的system call
      * 接受者的process通常會以(`accept_connection()`)
      * 大部分接收連接的process都是daemons(守護程序)，daemons是提供該用途的process
      * daemon 執行(`wait_for_connection()`)，並在連接時甦醒。
      * 通訊的來源稱為client(客戶端)，接收的daemon稱為server,兩者藉由(`read_message()`)及(`write_message()`)來溝通，並透過(`close_connection()`)關閉連接。
    * shared-memory model：
      * 較快，但會有sysnc的問題
      * 由OS分配一個可以共享的記憶體
      * process使用(`shared_memory_create()`)和(`shared_memory_attach()`)來產生和取得其他process的記憶體存取權(thread的話不用額外分配，預設就友共享記憶體空間)
    * 大多數作業系統同時擁有兩種模式
  * protection
    * 包括(`set_permission()`)以及(`get_permission()`)
    * 針對使用者則有(`allow_user()`)以及(`deny_user()`)

* Windows and UNIX system call example：
  | Class | Windows | UNIX |
  | --- | --- | --- |
  | process control | CreateProcess() | fork() |
  |  | ExitProcess() | exit() |
  |  | WaitForSingleObject() | wait() |
  | file management | CreateFile() | open() |
  |  | ReadFile() | read() |
  |  | WriteFile() | write() |
  |  | CloseHandle() | close() |
  | devices management | SetConsoleMode | ioctl() |
  |  | ReadConsole() | read() |
  |  | WriteConsole() | write() |
  | information maintenance | GetCurrentProcessID() | getpid() |
  |  | SetTimer() | alarm() |
  |  | Sleep() | sleep() |
  | communication | CreatePipe() | pipe() |
  |  | CreateFileMapping() | shm_open() |
  |  | MapViewOfFile() | mmap |
  | protection | SetFileSecurity() | chmod() |
  |  | InitializeSecurityDescriptor() | umask() |
  |  | SetSecurityDescriptorGroup() | chown() |

<h2 id="0023">API</h2>   

* API(application porgramming interface)： OS提供的程式設計介面，API會去呼叫System Call, 一個API可對應到0或多個System Call
  * Linux 的Api可以藉由man api_function查看：例如man read 可以查看read()這個api的資訊
  * UNIX系列的API叫做POSIX
  * Windows 就叫windows api
  * JAVA的API： Java Api(與jvm的interface)

* api與system call比較圖：
  ![api與system call比較圖](https://github.com/a13140120a/Operation_System/blob/main/imgs/sys_api.PNG)

* 傳遞參數至作業系統有三種方式：
  * 使用register
  * 使用存放在memory中的block或table
  * 使用stack

<h2 id="0024">loader and linker</h2>   

* 通常程式以二進制可執行檔(如a.out或 b.exe)的形式存在磁碟中
* 其編譯到載入的過程如下：
  * source code 被編譯成物件檔(目的檔)object file，object file可以被載入到實體記憶體的任何位置，而這種格式被稱為relocatabel object file(可重定位物件檔案)
  * 接下來linker會將這些relocatabel object file組合成單一個可執行(executable)檔
  * 在連結的期間可以指定包含其他的物件檔或程式庫(-l\*就是链接lib\*.a文件，-lm 就是連結C的標準數學函式庫libm.a(靜態函式庫)。  [詳細](https://bbs.csdn.net/topics/20023165))
  * loader 則會將二進制可執行檔載入到記憶體中執行
  * 動態連結函式庫(DLL)允許執行時動態的載入，windows為.dll檔，linux則為.so檔
  * ![compile](https://github.com/a13140120a/Operation_System/blob/main/imgs/c_compile.png)

* 物件檔與執行檔通常具有標準格式：
  * linux系統中，此標準格式為[ELF格式](https://zh.wikipedia.org/wiki/%E5%8F%AF%E5%9F%B7%E8%A1%8C%E8%88%87%E5%8F%AF%E9%8F%88%E6%8E%A5%E6%A0%BC%E5%BC%8F)
  * Windows中，則使用[PE格式](https://ithelp.ithome.com.tw/articles/10187490)
  * Linux當中可用file test.o來查看，以下可以看到test.o為elf 64位元可重定位檔，test為elf可執行檔(也可用readelf進行評估)
    ![file](https://github.com/a13140120a/Operation_System/blob/main/imgs/%E6%93%B7%E5%8F%960.PNG)
    
* 應用程式specify on OS，其原因如下：
  * 每個OS具有不同的二進制格式
  * CPU具有不同的指令集
  * Sustem call不同

* API 是在應用曾架構中指定某些功能，[ABI](https://zh.wikipedia.org/wiki/%E5%BA%94%E7%94%A8%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%8E%A5%E5%8F%A3)(應用二進位介面 application binary interface)則是用於指定底層詳細資訊，包括位址寬度，參數傳遞給System call的方法、執行時堆疊架構等等，一個ABI常見的樣貌即是調用約定(又稱呼叫約定 Calling Conventions)


<h2 id="0025">OS設計與製作</h2>   

* OS的設計目標大致上可分為以下兩種類：
  * user goal(使用者目的)： 就使用者而言，OS的設計必須easy to use, easy to learn, reliable, fast, safe
  * system goal(系統目的)： 而對於設計、維護OS的人，OS的設計必須要easy to design, implement, maintain, reliable, no error, efficient
* Policy and mechanism：
  * mechanism(機制)：決定如何做某些事
  * Policy(策略)：決定做什麼
  * 例如作為cpu保護的機制(mechanism)，[Timer](#0014)就是一種策略(Policy)
  * microkernel 為mechanism與Policy分離的極端例子
* 早期作業系統都是由組合語言撰寫，而現今大多都是以C/C++撰寫，少部分較瓶頸的常式(routine)則是用組合語言替換重構，以實現較好的效率。
* Android 使用一種以上的語言撰寫而成，其大部分系統程式庫都是以C/C++撰寫，而應用程式框架以及API則都是以Java撰寫而成。

<h2 id="0026">Structure</h2>   

* Simple OS Architecture(單一結構)：]
  * 又稱為緊密耦合(tightly coupled)系統，會因為更改一部分而對其他部分產生廣泛影響。
  * 所謂單一架構其實就是沒有架構，所有功能都放到單一的位址空間中的靜態二進制檔案當中
  * 缺點是難以design, 修改困難，漏洞多，優點是效能非常快，windows and UNIX Linux仍然存在此種架構
  * 原始UNIX系統為此架構
  * ![UNIX_Structure](https://github.com/a13140120a/Operation_System/blob/main/imgs/UNIX_Structure.jpg)
* Layered OS Architecture(分層架構)
  * 又稱為鬆散耦合(loosely coupled)，系統可分為許多具有特定功能的較小單獨元件
  * 速度較慢，但更好更改系統
  * 是模組化(Modular)的方法之一
  * 第0層為硬體，最高層為使用者介面
  * 非常好除錯與維護，因為每層只能使用較低層的服務
  * 分層系統常用於網路(如TCP/IP)
  * ![Layered_OS](https://github.com/a13140120a/Operation_System/blob/main/imgs/Layered_OS.jpg)
* Microkernel
  * kernel 的部分只保留一些基礎(如記憶體管理和process),其他都在使用者空間
  * 主要採用message passing 溝通
  * 優點是容易擴展，所有新的service 都加入使用者空間，所以不需修改核心，而當核心修改時，因為核心很小，所以不會牽涉到太多東西
  * 效能非常低弱，因為當兩個service 要溝通時必須要經過kernel, 產生許多複製訊息的動作
  * 最有名的Darwin是macOS 和IOS 的核心，Darwin是由兩個kernel所組成，其中一個是Mach
  * ![microkernel](https://github.com/a13140120a/Operation_System/blob/main/imgs/microkernelArchitecture.jpg)
* Modules(模組化)：
  * kernel提供主要服務，其他服務則在執行時被動態載入，就不必每次將新特性加入kernel都必須從新編譯
  * 比Layered OS更有彈性，因為任何module都可以呼叫其他module
  * 全部的module都在kernel裡面，所以速度不會太慢。
  * kernel裡面會有system table, 會有許多空的entry, 可以讓使用者insert
  * linux 也適用該技術，稱為[LKM](https://zh.wikipedia.org/wiki/%E5%8F%AF%E8%BC%89%E5%85%A5%E6%A0%B8%E5%BF%83%E6%A8%A1%E7%B5%84)
  * ![Solaris_Modules](https://github.com/a13140120a/Operation_System/blob/main/imgs/Solaris_Modules.jpg)

* 混和系統
  * 作業系統通常不會使用單一的系統結構，會組合不同架構，以強調性能、安全及可用性等問題
  * Linux是單核心，但模組化
  * Windows是單核心，但保留一些microkernel架構，支援subsystem，但同時也可以動態載入module

* macOS and IOS
  * user experience： 該層定義設備與使用者之間的互動，macOS使用的是[Aqua](https://zh.wikipedia.org/wiki/Aqua_(GUI))，IOS使用的則是[Springboard](https://zh.wikipedia.org/zh-tw/SpringBoard)
  * application frameworks： 這層包刮了用於開發MacOS應用程式的Cocoa 以及開發OS的Cocoa Touch framework，他們為Objected-C以及swift編程語言提供API。
  * core frameworks： 定義了支援圖形和多媒體的框架，包括Quicktime和OpenGL
  * Kernel enviroment： 這種環境也稱為Darwin, 包括Mach為核心和[BSD](https://zh.wikipedia.org/wiki/BSD) UNIX核心 
  * 這樣的結構可以直接利用各層的優點  
  * ![macOS_iOS](https://github.com/a13140120a/Operation_System/blob/main/imgs/macOS_and_iOS.jpg)  
  * Darwin是一個分層系統，由Mach微核心 和BSD UNIX核心組成，Mcah處理trap, BSD kernel處理POSIX system call
  * Darwin還提供了用於開發驅動程式和動態載入module的[kexts](https://support.apple.com/zh-tw/guide/deployment/depa5fb8376f/web)  
  * ![Darwin](https://github.com/a13140120a/Operation_System/blob/main/imgs/Darwin.jpg)

* Android：
  * 由google 領導開發
  * 開發人員使用Java程式語言開發，但不使用標準Java API, Java program被編譯成可在ART(Android Runtime)上執行的程式，針對有限的記憶體和cpu做了優化。
  * Java首先會先被編譯成.class檔，然後轉成可執行檔.dex檔
  * 一般的JVM會有JIT的機制，而Android的Java則是使用AOT(ahead-of-time提前編譯)技術
  * 開發人員還可以使用[JNI](https://developer.android.com/training/articles/perf-jni)技術，JNI允許跳過虛擬機直接存取硬體，缺點是不能移植到不同的硬體平台。
  * 包含[HAL](https://zh.wikipedia.org/wiki/%E7%A1%AC%E9%AB%94%E6%8A%BD%E8%B1%A1%E5%B1%A4)層，使開發人員可以在不同的硬體平台上移植程式。
  * 一般Linux使用的是GNU的C庫(glibc), 而Android則是用[Bionic libc](https://zh.wikipedia.org/wiki/Bionic_(%E8%BB%9F%E9%AB%94))函式庫
  * ![android_kernel](https://github.com/a13140120a/Operation_System/blob/main/imgs/Android_kernel_moudule.jpg)

* [WSL](https://hackmd.io/@billsun/Bkh8oAmGX?type=view)： 使windows 可以允許Linux的ELF執行檔在Windows 10 上執行。

<h2 id="0027">系統啟動</h2>  

* [啟動程式](https://zh.wikipedia.org/wiki/%E5%95%9F%E5%8B%95%E7%A8%8B%E5%BC%8F)：
  * BIOS的作用是初始化和測試硬體元件，以及從大容量儲存裝置（如硬碟）載入啟動程式，並由啟動程式載入作業系統
  * 許多BIOS程式都只能在特定電腦型號或特定主機板型號上執行。早年，BIOS儲存於ROM晶片上；現在的BIOS多儲存於快閃記憶體晶片上，這方便了BIOS的更新。
  * 最近許多電腦系統已經使用[UEFI](https://zh.wikipedia.org/wiki/%E7%B5%B1%E4%B8%80%E5%8F%AF%E5%BB%B6%E4%BC%B8%E9%9F%8C%E9%AB%94%E4%BB%8B%E9%9D%A2)取代[BIOS](https://zh.wikipedia.org/wiki/BIOS)
  * GRUB是一個用於UNIX系統的開源啟動程式，非常靈活，可在啟動時修改kernel參數，甚至可以選擇啟動不同的kernel，例如Linux的/proc/cmdline中的BOOT_INAGE是要載入到記憶體的核心映象檔的名稱，而root指定根檔案系統的唯一標識符
  * Android不使用GRUB，最常見的啟動程式是[LK](https://baike.baidu.hk/item/lk/623671)  [(LK詳細)](https://www.itread01.com/content/1502160009.html)

<h2 id="0028">除錯</h2>  

* core dump： 當程序運行的過程中異常終止或崩潰，作業系統會將程序當時的內存狀態記錄下來，保存在一個文件中，這種行為就叫做Core Dump
* crush dump： 當系統crash 的時候就要 collecting data for crash dump
* /proc是存在kernel中的偽檔案系統，/proc/2155將包含ID為2155的process 的訊息
* 可用`ps`查看process狀態，`top`查看process即時訊息，`vmstat`查看系統記憶體使用情況，`netstat`查看網路，`iostat`查看磁碟IO狀況
* `strace`可以查看任何process調用的system call,例如`strace ls`可查看ls調用的那些system call以及狀況，[gdb](https://jasonblog.github.io/note/gdb/gdbshi_yong_jiao_xue_ff1a_zi_dong_hua_ni_de_debug.html)指令可以原始碼除錯，`perf`為Linux性能工具包，`tcpdump`網路封包擷取。
* BCC： BCC是[eBPF](https://hackmd.io/@sysprog/linux-ebpf)工具的前端介面，而eBPF是擴展的[BPF](https://zh.wikipedia.org/wiki/BPF)，eBPF可動態插入正在運行的Linux系統，其指令可讀取特定事件(例如正在呼叫某個system call)，或者監視系統效能(例如IO時間)，BCC提供了python的前端介面，並嵌入了eBPF工具連接的C程式碼，而eBPF又與核心連接，BCC提供的許多工具均可用於特定應用程式，例如MySQL, Java或Python 程式

****




<h1 id="003">Process Concept</h1> 

  * ## [Process Creation](#0032) #
  * ## [Process Communication](#0033) #




<h2 id="0031">Concept</h2> 

* process： 執行中的程式(動態，存在memory)
* program： 未執行的程式(靜態，存在disk)
* process的活動可由當前的register以及Program Counter表示
* process的記憶體配置通常會包括以下四個section：
  * text section ：存放程式碼，大小固定
  * data section ：存放全域變數，大小固定，又分初始化及未初始化區域。
  * heap section ：process執行時動態分配的記憶體
  * stack section：呼叫函數時的臨時資料儲存(包括函數參數，return值以及區域變數)
  * [詳細](https://www.gushiciku.cn/pl/pwfP/zh-tw)
  * ![c_process_memory](https://github.com/a13140120a/Operation_System/blob/main/imgs/c_process_memory.png)
* `size [filename]`可以查看Linux 二進位檔的資訊，data顯示的是未初始化的資料，bss是已初始化的資料，dec跟hex分別表示十進位與16進位的三個階段的總和
* [objdump](https://wangchujiang.com/linux-command/c/objdump.html)是gcc工具，可查看執行檔的反組譯碼，符號表等資訊，反組譯出來的是[AT&T格式](https://www.itread01.com/content/1549963466.html)。
* process的狀態可分為以下幾種：
  * new：process正在產生中
  * ready：已經在memory裡面等待被執行
  * running：正在執行，interrupt之後會回到ready狀態
  * waiting：正在等待event發生，例如等待IO結束或收到signal
  * terminated：已完成
* PCB：每個作業系統都存在一個process control block(PCB)，其所記載之process相關資訊包括：
  * pointer to next PCB
  * process state
  * program counter
  * CPU register
  * Scheduling information：ex priority
  * memory management information：ex base register, limit register, page table, segment table
  * IO status information
  * accounting information： 包括帳號，cpu使用時間及數量，時限等等，開了多少file等等。
  * linux pcb是以C的[task_struct](http：//lienguang.com/task_struct/) 結構表示(位於/usr/src/linux-hwe-5.13-headers-5.13.0-27/include/linux/sched.h中定義)，所有的process是以Double Link List的task_struct表示，[請參考詳細說明](https://www.itread01.com/content/1548075798.html)。
* degree of multiprogramming： 目前在記憶體的process數量
* IO bound：較IO傾向的process
* CPU bound：較CPU傾向的process
* queue： 各種scheduling會放在queue裡面
  * Job queue：早期memory比較有限的時候，決定誰能夠load進memory
  * Ready queue：已經在memory，準備等待被執行
  * Device queue：sleep, interrupt或IO時會被踢進device queue，device queue通常不只一種
* swap in/out： virtual memory放進/岀disk
* context switch：當cpu要切換running process的時候需要把process的資訊記錄下來(如PC以及其他暫存器)，這種動作就叫context switch
* Linux 的systemd 這個process的ID永遠是1，是系統啟動時的第一個user process，然後會再create 許多子process
  * ![linux_tree](https://github.com/a13140120a/Operation_System/blob/main/imgs/linux_tree.png)
  * `ps -el`可以看到系統中所有process的完整訊息：
  * PPID代表parent pid(父process id), [NI及PRI代表的意義請看這裡](https://kknews.cc/zh-tw/tech/j85ngl.html), [其他](https://dywang.csie.cyut.edu.tw/dywang/linuxSystem/node77.html)
  * Linux的init 就是 systemd


<h2 id="0032">Process Creation</h2> 

* create process的時候，其resouece可能是：
  * child共享所有parent的資源(如file handle, io device handler)
  * child共享部分parent的資源
  * child不共享parent的資源
* create process的時候，其執順序可能是：
  * parent繼續與child同時執行
  * parent等child執行完之後才繼續執行
* create process的時候，新process 的記憶體空間有可能是：
  * 完全複製parent的process跟資料例如(UNIX的fork())：[example](https://github.com/a13140120a/Operation_System/blob/main/linux_create_process_example.c)
  * create的同時直接將program載入進去child。(例如windows的CreateProcess())：[example](https://github.com/a13140120a/Operation_System/blob/main/win_createprocess_example.cpp)
* parent會因為以下原因殺死child：
  * child使用超過分配的resource
  * 工作結束
  * parent結束
* 呼叫system call `exit()`來結束process
* 殭屍進程：child完成執行(通過exit()，或運行時發生致命錯誤或收到終止信號所致)，但在作業系統的進程表中仍然存在其PCB，其state處於terminated狀態的process，通常會發生在chhild需要保留表項以允許其parent讀取child的退出狀態，也就是在等待parent call `wait()`的process稱為zombie process(殭屍進程)，如果parent沒有呼叫wait就結束的話，child就會變成孤兒進程(orphan)，這時候Linux就會把該orphan設定為init(systemd)的child，並由init處理。

<h2 id="0033">Process Communication</h2> 

* Purpose：
  * information sharing
  * computation speedup： 多執行緒，multicore等等要加速運算就需要彼此溝通
  * modularity： 就像modular的kernel一樣，module, subsystem都需要彼此溝通
* google chrome是一個multiprocess的例子，其分成以下三個process
  * browser：process負責管理UI以及磁碟和網路IO
  * renderer(渲染器)：process負責處理呈現的網頁邏輯，包含HTML, JasaScript, 影像等邏輯，每個新的分頁都會有一個renderer
  * plug-in(插件)：例如flash, QuickTime
* IPC(interprocess communication)指的是process彼此之間的溝通
* producer生產資訊，comsumer消耗資訊。
* shared memory IPC：
  * 製作comsumer跟producer的共通buffer：
  * ![ipc_buffer](https://github.com/a13140120a/Operation_System/blob/main/imgs/ipc_buffer.PNG)
  * 
    ```c
    /* 如果沒有犧牲掉一個位置的話，當in==out會不知道是空了還是滿了。 */
    /* 製作buffer */
    /* 使用環狀陣列 */
    # define BUFFER_SIZE 10
    item buffer[BUFFER_SIZE]
    int in = out = 0;

    /* producer */
    whie(1){
        while(((in+1) % BUFFER_SIZE) == out); //buffer滿了則等待
        buffer[in] = nextProduced;
        in = (in+1) % BUFFER_SIZE;
    }

      /* comsumer */
    whie(1){
        while(in == out); //buffer空了則等待
        nextComsumed = buffer[out];
        out = (out+1) % BUFFER_SIZE;
    }
    ```
  * POSIX shared memory的範例：[producer](https://github.com/a13140120a/Operation_System/blob/main/posix_producer.c)，[consumer](https://github.com/a13140120a/Operation_System/blob/main/posix_comsumer.c)，gcc command`gcc -o consumer posix_producer.c -lrt`
* message passing IPC：
  * 至少具備以下兩種操作：
    ```
    send(message)

    receive(message)
    ```
  * process P跟process Q溝通，則communication link(通訊鏈) implementation可分為以下幾種：
    1. Direct or indirect communication：
       * Direct communication： 
         * Hard-coding, 較缺乏彈性
         * 又可分symmetric(對稱)與asymmetric(非對稱)
         * 如下所示，symmetric代表說雙方都要指定傳輸方向，不能修改。
         * 而asymmetric則是send方要指定，而receive方不用指定，可以接收任何process的message
         ```
         # symmetric
         send(P, message)
         receive(Q, message)
         
         # asymmetric
         send(P, message)
         receive(id, message)
         ```
       * indirect communication：
         * mailbox可能是為process或者OS所擁有，如果process是owner，那麼只有owner可以receive。
         * 藉由一個mailbox(或稱port)，
         * 每個mailbox都有一個id，只有雙方有共用的mailbox的時候才能溝通
         * 一個link可以和兩個以上的process相結合，
         * 假設P1,P2,P3共用一個mailbox, 當P1傳送訊息至A,而P2,P3都要執行來自A的receive()，有幾種方法可以決定誰可以收到訊息：1. 允許一個link最多只能與兩個process結合(簡化成1to1)。2. 同一時間只有一個process可以call mailbox的receive()。3. 為了選擇能接收的process而使用某種演算法。
    2. blocking(sychronous 同步) or nonblocking(asychronous 非同步)：
      * blocking send： 等待直到return
      * nonblocking send： 不等待直接return
      * blocking reveive： 等待直到接到訊息
      * nonblocking reveive： 不論有無都不等待
    3. buffer：
      * zero capacity：buffer size為零，此種情況下，producer必須等待直到comsumer收到資料。
      * bounded capacity：有限buffer size，當buffer滿時producer需等待，當buffer空時comsumer需等待。
      * unbounded capacity：無限buffer size，從不阻塞。
* Windows中提供的內部進程間通信方式為[ALPC](https://zh.wikipedia.org/wiki/%E6%9C%AC%E5%9C%B0%E8%BF%87%E7%A8%8B%E8%B0%83%E7%94%A8#%E5%AE%9E%E7%8E%B0)，只能於同一台機器中使用。
  * ![ALPC_Windows](https://github.com/a13140120a/Operation_System/blob/main/imgs/ALPC_Windows.jpg)
* Pipe：
  * 早期的UNIX系統所使用的IPC機制
  * Linux pipe：`ls|less`相等於Windows的`dir|more`
  * implement pipe時，有四個議題必須考慮：
    * 允許單向通信或雙向通訊?
    * 若允許雙向通訊，則是半雙工(同時間只能單方向)，還是全雙工(同時間可以雙向)
    * 通信的兩個process是否必須存在關係(parent-child)
    * 是否可以通過網路，或者必須在同一台機器上?
  * Ordinary pipe(普通管道)：
    * 必須是是parent-child的關係，所以只能備用在同一台機器上
    * UNIX 的 Ordinary pipe create方法就是`pipe(int fd[])`
    * producer從write end(寫入端)寫入，consumer從read end(讀取端)讀取
    * fd[0]表示write end，fd[1]表示read end
    * UNIX 的Ordinary pipe不能從產生這個pipe外的process存取
    * 基本上，UNIX的parent透過ordinary pipe和`fork()`出來的child溝通
    * UNIX把pipe當成一個特殊的檔案型態，可以使用普通的`read()`或`write()`來操作。
    * 完成通訊之後就會消失
    * [UNIX Ordinary pipe example](https://github.com/a13140120a/Operation_System/blob/main/unix_pipe.c)
  * Windows的Ordinary pipe叫做anonymous pipe(匿名管道)：
    * 如同UNIX是parent-child的關係
    * 可以使用普通的`readFile()`或`writeFile()`來操作。
    * create的API是`CreatePipe()`
    * windows anonymous pipe example： [parent](https://github.com/a13140120a/Operation_System/blob/main/win_pipe_parent.cpp), [child](https://github.com/a13140120a/Operation_System/blob/main/win_pipe_child.cpp)
  * Named Pipes(命名管道)：
    * 可以雙向，不需是parent-child的關係，可以多個process一起使用。
    * 完成通訊之後依然可以持續存在
    * UNIX的Named Pipes被稱為FIFO, Create的方法是`mkfifo()`,使用檔案系統的`read()``write()``open()``close()`來操作，並會持續存在直到被作業系統刪除，允許雙向通信但使用半雙工，如果要同時雙向傳輸，通常會建立兩個FIFO，必須在同一台機器上。
    * Windows的Named Pipes比UNIX更豐富，允許全雙工，可以在不同機器間傳輸，Create的方法是`CreateNamedPipe()`，client端可以使用`ConnectNamedPiped()`連接到Named Pipes，然後透過`ReadFile()`與`WriteFile()`還通信。

* Socket：
  * 由IP和port number組成
  * 分為TCP以及UDP兩種。
  * port 22為ssh, port21為FTP, port 80為http
  * 只能使用1024以上的port，因為1024以內都定義好了。
  * 127.0.0.1稱為loopback
  * 傳輸的data可以是任何東西，由programmer自行parse
  * Socket 運作方式：
    * ![Socket](https://github.com/a13140120a/Operation_System/blob/main/imgs/socket.PNG)
* PRC(Remote procedure call)
  * RPC的底層也是使用Socket實現
  * client端會有一個stub的process負責處理parsing，server端則稱為(skeleton)[wiki](https://zh.wikipedia.org/wiki/%E6%A1%A9_(%E8%AE%A1%E7%AE%97%E6%9C%BA))
  * 當client端呼叫一個remote procedure時，RPC會找到適當的stub, 並且mashaling(重排) parameter，封裝成[XDR(External Data Representation)](https://zh.wikipedia.org/wiki/%E5%A4%96%E9%83%A8%E6%95%B0%E6%8D%AE%E8%A1%A8%E7%A4%BA%E6%B3%95)格式，送到server端處理。
  * Windows的stub程式碼是由使用MIDL(微軟介面定義語言 Microsoft Interface Definition Language)所撰寫編譯而成
  * 遇到Pointer的傳輸時會有複雜的解決方法，其中可以參考微軟的[MIDL Pointers and RPC](https://docs.microsoft.com/en-us/windows/win32/rpc/pointers-and-rpc)
  * 概念圖：
    * ![RPC](https://github.com/a13140120a/Operation_System/blob/main/imgs/RPC.PNG)
* Android RPC：
  * Android OS中的binder framework包含龐大的IPC機制，其中就包含RPC
  * [Application components](https://www.techplayon.com/applications-component/)是Android app 的基本block，而Service就是其中一個Application components
  * 當Client端app呼叫`bindService()`時，該app的service(例如背景播放音樂)將會被bind(綁定)，而被綁定的service將會extend(Java的繼承) Service這個父類別`public class MyService extends Service {}`
  * 要提供RPC，`onBind()`必須要有一個返回介面，該介面由Java撰寫，並使用Android Interface Definition Language (AIDL Android介面定義語言)創建stub文件。
  * [詳細資訊](https://www.itread01.com/p/585471.html)

****




<h1 id="004">Thread</h1> 

  * ## [Mode](#0042) #
  * ## [Library](#0043) #
  * ## [Threading Issues](#0044) #
  * ## [OS Example](#0045) #




<h2 id="0041">Concept</h2> 

 * a lightweight process
 * 網頁伺服器是一個multithread 的例子(一個繼續監聽，一個處理request)
 * code section, data section(靜態或全域), OS resource(handle etc)是share的
 * thread有自己的ID, program counter, register set, stack
 * 好處：
   * Responsiveness：可以即時應對user
   * Resource sharing
   * Economy：相較create process可以不用消耗那麼多的時間，記憶體及其他資源
   * Scalability：可以隨意擴增執行緒數量。
* Concurrency(並行)是指藉由快速切換達到好像一起執行的錯覺，Parallelism(平行)才是真正一起執行
* Challenge：
  * Dividing tasks
  * Data spliting
  * Data dependency：(後面依賴前面的資料)
  * Balance
  * Testing and Debugging
* 分成user thread和kernel thread，user thread會傳給kernel thread去執行：
  * user thread：
    * 較快速
    * 例如Pthread(Linux, Mac), Win32 thread, Java thread
  * kernel thread
    * 因為較複雜，較多檢查以及保護機制，所以較慢


<h2 id="0042">Mode</h2> 
* 分成三種mode：
  * many to one：
    * 一個process 指有一個kernel thread(有很多user thread)
    * 優點是因為resource都sharing所以很快，缺點是一旦被blocking會導致系統停滯
    * ![many_to_one](https://github.com/a13140120a/Operation_System/blob/main/imgs/many_to_one.PNG)
  * one to one：
    * 一個thread被blocking的話，對其他thread沒有影響
    * 缺點是消耗太多資源，但現在電腦不缺資源，所以現代大多數OS(包括Linux和Windows)都是使用此種模式。
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/1to1.PNG)
  * Many to many：
    * 可以有many to one的resource sharing的快速的優點
    * 又可以有one to one那種不容易被blocking導致系統停滯的缺點
    * 難以實現，問題出在於如何mapping, 如果使用情境很單純，mapping的演算法可能會導致產生過多的overhead。
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/many_to_many.PNG)

<h2 id="0043">Library</h2> 

* 又分成asynchronous threading(非同步)和synchronous threading(同步)
  * asynchronous threading：不必等child thread結束，才能繼續
  * synchronous threading：必須等child thread結束，才能繼續
* [Pthread example](https://github.com/a13140120a/Operation_System/blob/main/pthread_exm.c), gcc command：`gcc -pthread -o pthread_exm pthread_exm.c`
* [Windows thread example](https://github.com/a13140120a/Operation_System/blob/main/win_createthread.c)

* Implicit Threading： 由compiler和runtime libraries負責管理與新增執行緒，並非由程式負責，分成以下幾種方法：
  * Thread pool
    * 先Create 一個thread，節省去產生一個thread的成本。
    * 可以把資源使用量控制在一定的範圍之內
    * Window threadpool： [QueueUserWorkItem](https://docs.microsoft.com/en-us/windows/win32/api/threadpoollegacyapiset/nf-threadpoollegacyapiset-queueuserworkitem)
    * Java threadpool
  * Fork-join： thread並不是在fork的時候被create，而是在task真正執行的時候才被create，library負責分配建立thread的數量，以及分配task給thread
    * java的fork join： `ForkJoinPool()`
  * [OpenMP](https://zh.wikipedia.org/wiki/OpenMP)：是一套支援跨平台共享記憶體方式的多執行緒並行的編程API，支援C,C++和Fortran
    * example：
    * ```c
      #include <omp.h>
      #include <studio.h>
      
      int main()
      {
          /* 當OpenMP遇到#program omp parallel時會產生和系統core數一樣多的thread */
          #program omp parallel
          {
              printf("i am a parallel region"); 
          }
          
          /* 當OpenMP遇到#program omp parallel for時會將回圈內的指令分給不同的thread進行平行運算 */
          #program omp parallel for
          for (i=0;i<N;i++)
              c[i] = a[i] + b[i]
              
          return 0;
      }
      ```

  * GCD(Grand Central Disoatch)：MacOS和IOS作業系統的一項技術
    * 和OpenM類似，允許開發人員區分出要平行執行的程式碼區塊
    * GCD會將區塊放進dispatch queue, 當一個區塊從queue pop出去時，會分配thread pool中的一個thread給這個區塊執行
    * 在GCD內部的thread pool由POSIX組成，並由GCD管理pool決定增減thread數量
    * GDC由libdispatch這個程式庫實現，並且以一直到FreeBSD作業系統當中
    * dispatch queue又分成serial和concurrency，serial一次只能pop一個區塊，而concurrency一次可以pop多個區塊並且可以同時執行，兩者都是以FIFO順序pop
      * concurrency又分成以下幾種類別：
        * QOS_CLASS_USER_INTERACTIVE：與USER互動
        * QOS_CLASS_USER_INITIATED：需要USER來啟動，但並不需要像USER_INTERACTIVE一樣快速回應
        * QOS_CLASS_UTILITY：需要較長時間才能完成，但不要求立即獲得結果
        * QOS_CLASS_BACKGROUND：對時間的敏感度不高，例如：信箱備份
    * 對於C/C++/Objective-C語言的簡單區塊範例：
      * ```C
        ^{ printf("hello block!"); }
        ```
    * 對於Swift而已，以下範例示範將task push進dispatch queue：
      * ```C
        let queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0)
        dispatch_async(queue, { printf("hello block!") })
        ```
  * [TBB(Intel threading building blocks)](https://en.wikipedia.org/wiki/Threading_Building_Blocks)：
    * 是一個 C++ template library，TBB提供任務排班、負載平衡以及支援快取。
    * 也提供平行迴圈、Atomicity操作、互斥鎖等豐富功能。
    * 範例：
    * ```C
      # c迴圈
      for (int i =0; i<n; i++)
          apply(v[i]);
      ```
    * ```C
      # 使用TBB
      parallel_for(size_t(0), n, [=](size_t) { apply(v[i]); });
      ```
      
<h2 id="0044">Threading Issues</h2> 
      
* fork and exec：
  * UNIX的`fork()`有兩種版本： 1. 複製process的所有thread(整個process)。 2. 指複製呼叫`fork()`的那個thread。
  * 而`exec()`則是會取代整個process(包括所有thread)，所以如果呼叫上2的`fork()`再呼叫`exec()`是沒有意義的。
* signal：
  * 所有的signal都遵循相同的過程： 由於event發生而產生 > 產生的signal被送到一個process > 送達後交給signal handler處理
  * signal又分成同步跟非同步，同步包括：記憶體存取違規或除以0，非同步包括ctrl+c或者 timer時間到了。通常非同步signal會被送到另一個process
  * signal handler 又分成default signal handler 以及 user-defined signal handler，default signal handler是在kernel執行，但有可能被user-defined所覆蓋。
  * signal有可能會被傳送到以下幾種選擇：
    * 把訊號傳到有應用的執行緒上。
    * 把訊號直接傳到所有的執行緒上。
    * 把訊號傳到行程中特定的執行緒中。
    * 選一個執行緒，負責接收所有的訊號。
  * [Windows APC](https://docs.microsoft.com/zh-tw/windows/win32/sync/asynchronous-procedure-calls)：windows沒有signal，但有APC，允許使用者執行緒設定一個函數, 當一個event發生時就呼叫此函數
* Thread cancellation： 意思是一個thread在完成工作之前結束他，例如多執行緒搜尋資料庫，其中一個找到，其他就都該結束，被取消的thread稱為target thread, target thread又可分成兩種：
  * Asynchronous cancellation(非同步取消)：立即終止，有可能會無法釋放所有資源
  * Deferred cancellation(延遲取消)：會執行到一個check point才取消，例如：ctrl+c，允許執行緒安全的被取消。
  * Pthread： `pthread_cancel(tid)`或`pthread_cancel(pthread_self())`，pthread支援[三種取銷模式](https://man7.org/linux/man-pages/man3/pthread_setcancelstate.3.html)
  * java的Thread cancellation只要把thread的interrupt狀態設成true就可以了`Thread worker; worker.interrupr()`，檢查：`while(!Thread.currentThread().isInterrupr())`
* TLS(Thread local storage)： 
  * 每個thread的各自的獨一無二的資料，不會干擾或影響到別人
  * local variable：只能在自己的function中被看見，而TLS中的資料可以被自己以外的function看見。
* Schedular Activations：
  * 在Many-to-Many跟Two-level通訊要求間，維持適當的kernel threads的數量，再分配給應用程式。
  * <p id="lwp">LWP(lightweight process)：在user thread跟kernel thread中間屬於kernel的資料結構</p>  
  
    * 提供虛擬執行緒的排程，
    * LWP看起來像是一個Virtual processor，應用程式可以排班一個process在其上執行，
    * 每個LWP連接到一個kernel thread，然後OS安排這些Kernel thread在實體processor執行，
    * 如果Kernel thread被block，LWP也會被block。
    * 一個應用程式可以要求任何數目的LWP，IO bound的應用程式通常需要多個LWP來執行，例如一次開啟五個檔案讀寫就需要五個LWP，因為他們可能都在Waiting，如果這時應用程式只有4個LWP，其中一個就必須等待。
    * LWP與普通行程的區別也在於它只有一個最小的執行上下文和排程程式所需的統計資訊，而這也是它之所以被稱為輕量級的原因

<h2 id="0045">OS Example</h2> 

* Windows thread
  * 使用One-to-One模組。
  * 每個執行緒包含：
    * 執行緒ID
    * 儲存處理器表現狀態的暫存器組
    * program counter
    * 一個user stack跟kernel stack
    * 由runtime libraries與dyanamic link libraries所使用的私有儲存區域
  * context： 儲存處理器表現狀態的暫存器組+stack+私有儲存區域
  * 在Windows中thread的主要資料結構：
    * ETHREAD：包含thread所屬process的指標，執行緒起始位址，還有一個指標指向KTHREAD。
    * KTHREAD：包含排班跟同步的資料，kernel stack跟指向TEB的指標。
    * TEB(Thread Envrionent block)：包含ID、use stack、TLS。 
* Linux Thread：
  * Linux沒有區分process跟thread，在Linux中，threads被稱作為tasks。
  * Linux not support mulltithreading，而是使用Pthread實現user-level的thread
  * `fork()`create a new process and **copy** assciated data of parent process
  * `clone()`create a new process and **link** assciated data of parent process(有些memory是share的)，clone 中第三個參數flag有以下幾種選擇
    * CLONE_FS： 共用檔案系統訊息
    * CLONE_VM：share相同的Memory space
    * CLONE_SIGHAND：共用signal handler的函數
    * CLONE_FILES：共用開啟的檔案
  * 當`clone()`沒有Flag設定，就與`fork()`相似，
  * task_struct(見3章PCB)結構中存在許多指標指向儲存這些資料的其他struct，如開啟檔案的linked-list、signal處理訊息和virtual memory的資料結構等，當`fork()`被呼叫時會clone這些data, 而`clone()`則會根據flag指向parent的儲存這些資料的struct，`clone()`技術也造就了虛擬化技術。

****




<h1 id="005">CPU Scheduling</h1> 

  * ## [Scheduling Algorithms](#0052) #
  * ## [Thread Scheduling](#0053) #
  * ## [Multi-Processor Scheduling](#0054) #
  * ## [Real-time Scheduling](#0055) #
  * ## [OS Example](#0056) #
  * ## [Evaluation Methods](#0057) #




<h2 id="0051">Concept</h2> 

* CPU burst： 一連串的instruction都在使用CPU的狀態
* IO burst ： 一連串的instruction都在做IO的狀態
* CPU bound： CPU burst較多的(process)
* IO bound ： IO 較多的(process)
* CPU 的Schedule發生在以下四種狀況  
  1.從running到waiting(例如發生IO)  
  2.從running到ready(例如發生interrupt)  
  3.從waiting到ready(例如IO結束)  
  4.terminates  
* non-preemptive： CPU 的Schedule只發生在上述1跟4的情況，non-preemptive的kernel結構較簡單，對於即時計算較差
* preemptive： CPU 的Schedule可以發生在上述所有的狀況，需要額外的方法防止race condition(競爭情況)
* Linux環境可使用[vmstat](https://charleslin74.pixnet.net/blog/post/434428316)來查看每秒context switch以及interrupt的數量，例如`vmstat 5 6`每五秒統計一次，總共統計六次。
* Linux環境可以利用`cat /proc/254/status`來查看pid=254的process狀態，最後兩行顯示voluntary-ctxt-switches(自願context switch，如IO)以及nonvoluntary-ctxt-switches(非自願，如被preempt)數量。
* ready queue並不一定是FIFO queue。

<h2 id="0052">Scheduling Algorithms</h2> 

* Scheduling Algorithms通常有以下幾種評估標準(Scheduling Criteria)：
  * CPU utilization：理論上0/~100%，實際上會介於40/~90%，可用[top](https://david50.pixnet.net/blog/post/45252072-%5B%E7%AD%86%E8%A8%98%5Dlinux---top%E8%B3%87%E8%A8%8A)查看
  * Throughput： (以系統的角度)每單位時間內完成的工作量，對於cpu scheduling而言就是每單位時間完成的process數量。
  * Turnaround Time： (以single job的角度)submission/~cpmpletion的時間
  * wait time：在ready queue的時間
  * response time： (對於interactive很重要)從submission到第一個cpu burst開始的時間。

* FCFS(First Come First Serve)：
  * 製作容易，average wait time通常很長，並且會因為cpu burst的變化而有極大差異
  * 不管是non-preemptive還是preemptive都不會改變結果，一樣都是先來先做完。
  * ![fcfs_scheduling](https://github.com/a13140120a/Operation_System/blob/main/imgs/fcfs_scheduling.PNG)
* SJF(Shortest Job First)：
  * 可以證明是最小wait time的演算法
  * non-preemptive跟preemptive會得到不同的結果
  * 無法預知下一個cpu burst的長度(但可以使用一些time serious的方式預測，例如[EMA](https://zh.wikipedia.org/wiki/%E7%A7%BB%E5%8B%95%E5%B9%B3%E5%9D%87#%E6%8C%87%E6%95%B8%E7%A7%BB%E5%8B%95%E5%B9%B3%E5%9D%87))
  * [詳細解說](https://www.youtube.com/watch?v=scp5vRE3yVw&list=PL9jciz8qz_zyO55qECi2PD3k6lgxluYEV&index=32)
* RR(Round robin)：
  * 每個process會得到一個固定的time quantum，通常是10/~100毫秒。
  * 時間越短，overhead越高，時間越長則就變成FCFS
* Priority Scheduling：
  * 每個process會有一個priority, priority越高則越先執行
  * Priority Scheduling也有分non-preemptive跟preemptive，non-preemptive的話在priority較高的process出現時，較低的會被preempt
  * priority可以是內部或外部定義的，內部的包含依據時間限制、記憶體需求、開啟檔案數量、IO burst與CPU burst的比例來計算priority，外部則是由作業系統的一些標準所定義，例如process的重要性等等。
  * 會有starvation的問題，解決的方法是aging，逐漸提高在系統中停留時間較長的process的priority。
  * SJF是按照cpu burst的長度來決定priority的Priority Scheduling
* multilevel queue：
  * 有多個queue, 每個queue有不同的Scheduling Algorithms，也有不同的Priority，通常會放不同類型的process
  * process不能在queue之間移動
  * 要定義如何選擇哪一個queue，通常有兩種做法：
    * 除非priority較高的queue空了，否則不會執行較低的queue
    * 每個queue分到不同比例的cpu時間，譬如priority較高的分到80%做RR Schdule，而較低的分到20%做FCFS等等。
  * ![multilevel_queue](https://github.com/a13140120a/Operation_System/blob/main/imgs/multilevel_queue.PNG)
* multilevel feedback queue：
  * 最通用的Scheduling Algorithm
  * process可以在queue之間移動
  * 會在run time的時候觀察process的behavior來判斷應該要如何在queue之間移動
  * 如果一個process使用cpu的時間太長，就會被排到較低priority的queue，讓IO bound和interative的process放在較高priority的queue
  * 而較低priority的process也會隨著時間增加而增加priority(aging)來避免starvation
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/multilevel_feedback_queue.PNG)
  * 上圖例子當中，最上層queue分配到的time quantum是8ms，如果8ms內未完工的process就會被安排到第二層的queue，這時候如果最上層queue裡面沒有任何process就會開始執行第二層的queue，如果還是執行不完，就會再被安排到最下層的queue，而為了避免starvation，如果在較低priority的queue待太久則會被安排到較高的queue
  * multilevel feedback queue必須決定以下幾種參數：
    * queue的數量
    * 每個queue的Algorithm
    * 何時把porcess提升到較高priority的queue
    * 降低高priority的process到較低queue的方法
    * 當process需要serve的時候該進入哪個queue

<h2 id="0053">Thread Scheduling</h2> 

* 分成user-level 和 kernel-level
* 因為當thread支援得時候,不是只有process要Scheduling, thread也要
* PCS：如果在Many-to-one 和many-to-many model，user-level就必須經過Scheduling，這個過程稱為process-contention scope (PCS)由programmar來做
* SCS：另外一個是由Kernel thread 來schedule，被稱為system-contention scope (SCS)由系統來做
* Pthread中允許process產生期間指定PCS和SCS
  * 預設情況下，對於一個擁有n個執行緒的程式，啟動多少LWP，由哪些LWP來控制哪些執行緒由作業系統來控制，這種狀態被稱為非繫結的。
  * 那麼繫結的含義就很好理解了，只要指定了某個執行緒“綁”在某個LWP上，就可以稱之為繫結的了。
  * `int pthread_attr_setscope(pthread_attr_t *attr, int scope);`的第二個參數scope是[繫結型別](https://www.796t.com/article.php?id=63046)，擁有兩個取值，PTHREAD_SCOPE_SYSTEM（繫結的）和PTHREAD_SCOPE_PROCESS（非繫結的）。
  * Linux採用了one-to-one的方案，這也就是說，Linux的執行緒永遠都是繫結，所以PTHREAD_SCOPE_PROCESS在Linux中不管用，而且會返回ENOTSUP錯誤。
  * 在many-to-many中，PTHREAD_SCOPE_PROCESS使用PCS排班法排班執行緒，將策略地排班user mode的thread在可用的LWP中，而LWP的數目是由library所控制，
  * PTHREAD_SCOPE_SYSTEM則是使用SCS排班法排班執行緒，將產生和連結一個LWP給每個user mode的thread，並使用1對1有效的map thread。
  * [wiki](https://zh.wikipedia.org/wiki/POSIX%E7%BA%BF%E7%A8%8B#%E6%95%B0%E6%8D%AE%E7%B1%BB%E5%9E%8B)
  * [EXAMPLE](https://github.com/a13140120a/Operation_System/blob/main/set_scope.c)，GCC：`gcc -o set_scope set_scope.c -lpthread`

<h2 id="0054">Multi-Processor Scheduling</h2> 

* 又分成asymmetric multiprocessing(非對稱)和symmetric multiprocessing(對稱)：
  * asymmetric multiprocessing(AMP)：其中一顆cpu為master，負責管理整台電腦上的所有processor，也因為所有的OS code都run在同一個cpu上，同一時間只會有一顆cpu去access system data structure，所以不會有synchronization的問題，scale能力很強，用於大型電腦當中，缺點是浪費一顆cpu不做運算，以及master可能是效能瓶頸。
  * symmetric multiprocessing(SMP)： 每個processor的功能都一樣，能自行排班，OS的code run在所有的processor上，同一時間有可能會有多個cpu去access system data structure，所以會有synchronization的問題，能夠scale的能力較弱。
  * SMP又可分為兩種策略：
    * all processes in common ready queue：所有cpu的排班程式去競爭一個ready queue，會有synchronization的問題
    * separate ready queues for each processor：可能會有load balancing不平衡的問題(例如一個cpu的queue滿載，另一個則空)。
* multicore processor將多個核心(core)放在同一個實體晶片上，每個core維持它的架構，因此對作業系統來說好像是一個獨立的processor
* ![processor_vs_os](https://github.com/a13140120a/Operation_System/blob/main/imgs/processor_vs_os.jpg)
* 當processor存取memory時，需耗費大量的時間，這種情況稱為**Memory Stall**(例如cache miss)
* ![MemStall](https://github.com/a13140120a/Operation_System/blob/main/imgs/MemStall.jpg)
* 核心多執行緒化
  * 核心多緒化，多個hardware threads被分配到一個core  
  * ![TwoThrdCore](https://github.com/a13140120a/Operation_System/blob/main/imgs/TwoThrdCore.jpg)
  * Intel使用hyper-threading(或稱simultaneous multithreading,SMT)來將hardware thread分配給一個core
  * 核心多執行緒化有兩種方法：
    * Coarse-grained(粗糙)：會執行一條執行緒一直到出現long-latency event(例如Memory stall)，才進行切換並清空(flush)管線，context switch代價較大。
    * fine-grained(精緻)：在更小的granularity上面switch thread，例如每個指令之間，fine-grained的架構設計包含thread切換的邏輯(例如一次儲存兩個thread的cache)，因此context switch代價較小。(註：granularity是指在並行計算中，對該任務執行的工作量（或計算量）的度量)
    * [參考](http：//ccckmit.wikidot.com/qa：osthread)
  * 實體的core同一時間只能執行一個hardware thread，因此需要兩種層級的排班，分別是**軟體執行緒**與**硬體執行緒**的排班
* Load balancing：
  * push migration： 某個task會會檢查不同processor上的loading，如果不平衡的話就把processes移動(適合整個系統的processor的loading都很低)。
  * pull migration： idle的processor去拉waiting task(適合整個系統的processor的loading都很高)
  * 兩種方法並不互斥，通常會一起使用這兩種方法。
* affinity：
  * process由一個processor轉移到另一個processor上需要將轉移前的processor cache清空，並將轉移後的processor cache填滿，這會造成很大的cost
  * Proccessor affinity：大部分SMP系統會嘗試讓一個process一直在同一個processor上執行，避免process由一個processor轉移到另一個processor上，也就是process對目前執行的processor有affinity(親和性)
  * soft affinity可以轉移到其他processor，hard affinity不行
  * 許多系統同時提供soft affinity和hard affinity
  * linux提供支援affinity的system call：`sched_setaffinity()`
* Load balancing會抵消affinity帶來的bonus
* heterogeneous multiprocessing(HMP)：
  * 對於某些系統而言，每個核心使用相同的instruction來執行，但他們有著不同的時脈以及電源管理，這樣的系統稱為HMP。
  * 例如ARM使用[big.LITTLE](https://zh.wikipedia.org/wiki/Big.LITTLE)，高性能的big core與較節能的LITTLE core結合，big core消耗更多的能量，因此只能使用較短的時間，LITTLE消耗較少的能量，所以能夠更長時間使用。
  * CPU scheduler可以將較低耗能(例如背景程式)分配給little，可以節省電量，如果行動裝置組於省電模式，則禁止使用big，windows允許支援HMP。


<h2 id="0055">Real-time Scheduling</h2> 

* 必須在deadline之前完成
* 又分成soft real-time和hard real-time，soft real-time如果miss deadline的話不會造成嚴重的後果，hard real-time必須保證一定要在deadline之前完成(如煞車系統)。
* Event latency： event發生到被處理的延遲時間。
* 影響real-time system的主要因素為interrupt latency(中斷延遲)以及dispatch latency(調度延遲)：
  * interrupt latency包含決定interrupt類型以及context switch的時間
  * dispatch latency包含以下兩個部分：
    * Preemption of any process running in the kernel.
    * Release by low-priority processes resources required by the high-priority process.  
* Real-time Scheduling的scheduler必須要support preemptive以及prioritybased scheduling，並要求週期p必須是固定的，但只能保證soft real-time。
* 設process的執行時間是t、deadline是d、週期時間是p，如果t < d < p，週期任務的頻率就是1/p，
* T1 = (0,4,10)=(Ready, Execution, Period)
* Rate-Monotonic (RM) Scheduling：
  * Shorter period, higher priority
  * priority是固定的
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/RM.PNG)
* Early Deadline First (EDF) Scheduler
  * Task’s priority is determined by deadline.
  * priority會變動
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/EDF.PNG)
* Proportional Share Scheduling：
  * 假設CPU時間總共有T個**shares**、process有N個shares(N < T)，這樣每個process都被分配到N/T時間，
  * 必須事前需要把資源比例分配好，例如總共有100個share，已經分配了85個share，若有新的process需要30個share，則拒絕進入系統。
* POSIX Real-Time Scheduling：
  * 設定執行緒排程演算法的介面是`pthread_attr_setschedpolicy()`，它的完整定義是：`pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);`
  * 第二個參數的選項有SCHED_RR、SCHED_FIFO和SCHED_OTHER(預設)
  * [EXAMPLE](https://github.com/a13140120a/Operation_System/blob/main/set_schedule_policy.c)，GCC：`gcc -o set_schedule_policy set_schedule_policy.c -lpthread`


<h2 id="0056">OS Example</h2> 

* Linux：
  * Linux系統的schedule是base on scheduling class，每個scheduling class會有不同的priority以及不同的algorithms以應對不同的需求。
  * 標準的Linux系統會有兩種scheduling class： default(CFS)和real-time，亦可自行加入新的scheduling class。
  * [CFS](https://blog.xuite.net/ian11832/blogg/23745751)於Linux kernel 2.6.23開始正式被採用，讓所有工作平等的被執行便是這排程的目標
  * CFS將每個thread的負載記錄下來，結合priority以及cpu使用率，較高priority但是IO bound的程式通常負載較低，使用此方法可以確保所有queue都有相近的負載
  * Linux也定義scheduling domain的階層系統，scheduling domain是根據他們共享的資源下去分組的，每個core可能擁有L1 Cache, core之間也可能共享L2 cache, 包含L2 cache的一組被分為domain0和domain1，而這兩個scheduling domain共享L3 cache，並且組成一個**NUMA node**，CFS最初只會在同一個domain裡面做thread的搬移(即domain0或domain1)，下一個層級的load balancing會發生在domain0跟domain1之間，如果整個系統處於忙碌狀態，則CFS不會在每隔Core之間進行load balancing。
* windows
  * 使用以priority為基礎的preemptive排班演算法來排班thread
  * 確保最高priority的thread將一直執行
  * windows kernel處理schedule的部分叫做dispatcher，被dispatcher選到的thread將一直執行直到被更高priority的thread preempt或是遇到IO、終止、或time quantum用完才會停止
  * 使用32層的priority技巧，variable class包含1/~15，real-time包含16/~31，priority為0的thread被用於記憶體管理。
  * dispatcher為每個priority提供一個queue，從最高到最低traverse，直到發現有ready的thread為止，如果沒有發現任何ready thread，會執行一個叫做idle thread的特殊thread。
  * process通常是NORMAL_PRIORITY_CLASS，除非父process是其他類別或者使用者特別設定。
  * 可使用`SetPriorityClass()`以及`SetThreadPriority()`，其API對應的priority請[參考](https://docs.microsoft.com/zh-tw/windows/win32/procthread/scheduling-priorities#base-priority)
  * 每個thread都會有一個基本優先權，預設是該class中NORMAL的數值。
  * 當thread的time quantum用完之後會被interrupt，並且如果此thread是variable class，他的priority會被降低，但不會被降到低於基本優先權，如果從waiting狀態中被釋放出來，就會提升priority，提升量取決於等待的是什麼，例如鍵盤IO將獲得大幅提升，若是磁碟IO則適量提升。
  * windows對於foreground process(前景)和background process(背景)有所區別，當一個process由background移動到foreground時，windows會增加其排班量(通常是3)
  * windows 7引入了[user-mode scheduling(UMS)](https://zhuanlan.zhihu.com/p/43652554)，允許user獨立於kernel外自行產生以及管理thread，對於大量產生 thread的process，在user mode排班會比在kernel mode排班更有效率，因為不需要介入kernel。
  * 早期版本的windows提供類似的特性，稱為[fiber](https://docs.microsoft.com/zh-tw/windows/win32/procthread/fibers)，[(fiber wiki)](https://zh.wikipedia.org/wiki/%E7%BA%96%E7%A8%8B)，允許user mode的thread被map到單一的kernel thread，fiber不能呼叫windows API，因為所有fiber(纖程)都必須分享他們執行之thread的**TEB**[(Thread Environment Block 執行緒環境區塊)](https://docs.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-teb)，如果windows api 將資訊狀態放入一個fiber的TEB，會讓此資訊被另一個fiber覆蓋，UMS藉由提供每個user thread自己的執行緒內容來克服此障礙。
  * Windows也支援multicore processor系統來進行thread在processor上的排程，使用的技術是建立[SMT Set(邏輯處理器集)](https://docs.microsoft.com/zh-tw/windows-hardware/drivers/debugger/-smt)，例如四核二緒(8 logic processor)包括四個SMT集：{0,1}{2,3}{4,5}{6,7}，Windows能夠維持同一個SMT set中logic processor上的執行緒執行，為了在不同processor之間load balancing，每個thread會分配給一個ideal processor(理想處理器)，每個process都會有一個初始種子值，用於標示屬於該process的thread的理想CPU，該process每建立一個thread，種子值的數字就會增加，藉此分散負載在不同的logic processor上，在SMT系統中，下一個ideal processor的增量會在下一個SMT set中，例如某個process的thread的ideal processor為0,2,4,6,0,2...，如果第二個process的種子值為1，則分配1,3,5,7,1,3....。


<h2 id="0057">Evaluation Methods</h2> 

* Deterministic modeling：依照performance metric去選擇、製作model，可能是要response time最小，還是wait time，或者是real time。
* Queueing model：設計演算法，分析並且證明，[little's law](https://wiki.mbalib.com/zh-tw/%E5%88%A9%E7%89%B9%E5%B0%94%E6%B3%95%E5%88%99)認為，系統中的平均存貨等於存貨單位離開系統的比率（亦即平均需求率）與存貨單位在系統中平均時間的乘積。
* Simulation：以應用程式的方式設計一個電腦系統的模型，用來驅動模型的資料產生可以是隨機分布、常態分布、指數分布、卜瓦松分布等等，並且收集LOG、workload等資料
* Implementation：即使是模擬，也沒辦法得到完全正確的評估，因此還需要Implementation。

****




<h1 id="006">Synchronization</h1> 

  * ## [Algorithm for Two Processes](#0061) #
  * ## [Peterson’s Solution for Two Processes](#0062) #
  * ## [Synchronization Hardware Support](#0063) #
  * ## [Mutex lock](#0064) #
  * ## [Semaphores](#0065) #
  * ## [Monitor](#0066) #
  * ## [Classical Problem](#0067) #
  * ## [Example](#0068) #
  * ## [Alternative Approaches](#0069) #


<h2>Concept</h2> 

* race condition：
  * [process communication](#0033)裡頭提到的producer與consumer的例子可以修改如下：
  * ```
    # define BUFFER_SIZE 10
    item buffer[BUFFER_SIZE]
    int in = out = 0 , count=0;

    /* producer */
    whie(1){
        while(((in+1) % BUFFER_SIZE) == out); //buffer滿了則等待
        buffer[in] = nextProduced;
        in = (in+1) % BUFFER_SIZE;
        count++;
    }

      /* comsumer */
    whie(1){
        while(in == out); //buffer空了則等待
        nextComsumed = buffer[out];
        out = (out+1) % BUFFER_SIZE;
        count--
    }
    ```
  * 出現問題：
  * ```
    /* producer */
    register1 = count
    register1 = register1 + 1
    count = register1

    /* consumer */
    register2 = count
    register2 = register2 - 1
    count = register2

    /* instruction interleaving */
    register1 = count                     {count=5}
    register1 = register1 + 1             {count=6}
    register2 = count  // context switch  {count=5}
    register2 = register2 - 1             {count=4}
    count = register1  // context switch  {count=6}
    count = register2  // context switch  {count=4}
    ``` 
* critical section： 設系統含有多個process，每個process都有一段critical section，critical section可能被存取或者更新，但同一時間內只能有一個process進入。
* remain section：除了critical section以外的程式碼區段。
* 解決critical section problem必須滿足以下三個條件：
  * mutual exclusion： 不能兩個process同時在critical section內
  * progress： 如果cirtical section裡面沒有任何process，此時有一個process想要進入(或本來就在waiting)，應該要可以進入，不應無限的搶先進入。
  * bounded waiting： 一個process已經在waiting進入critical section，而在這個process進入之前應該限制其他process不能無限的搶先進入(應限制其他process進入的次數)
* 可以disable interrupt來防止race condition，對於單核心還好，但對於多核心就會嚴重影響效能，也會影響電腦的clock interrupt


<h2 id="0061">Algorithm for Two Processes</h2> 

* initially turn = 0
* turn = i ⇒ Pi can enter its critical section

  ```c
   /* Process 0 */                  /* Process 1 */
  do {                             do {
    while (turn != 0) ;              while (turn != 1) ;
     /* critical section */           /* critical section */
    turn = 1;                        turn = 0;
     /*remainder section */            /* remainder section */
  } while (1)                      } while (1)
  ```
* Mutual exclusion：Yes 
* Progress：No，p0離開之後再進入會無法進入，這時p1可能也根本沒有想要進入。
* Bounded-Wait：Yes，p0離開之後一定要先讓p1進入才可以再進入。

<h2 id="0062">Peterson’s Solution for Two Processes</h2> 

* 多一個boolean flag[2]; //initially flag [0] = flag [1] = false //用來表示process想不想進去

  ```c
   /* Process 0 */                         /* Process 1 */
  do {                                    do {
    flag[0]=true;                           flag[1]=true;
    turn = 1 ;                              turn = 0 ;
    while (flag [1] && turn == 1 ) ;        while (flag [0] && turn == 0 ) ;
     /* critical section */                  /* critical section */
    flag [0] = FALSE ;                      flag [1] = FALSE ;
     /*remainder section */                  /* remainder section */
  } while (1)                             } while (1)
  ```
* Mutual exclusion：Yes 
* Progress：Yes，當p0想進去的時候，p1若已經離開，則flag[1]會設成flase，那麼p0就可以進入
* Bounded-Wait：Yes，當p0想進去的時候flag[0]會設成true，這時如果p1想要重複一直進入便會被擋住，反之亦然

<h2 id="0063">Synchronization Hardware Support</h2> 

* 因為是hardware，所以是Atomic
* memory barriers(或稱memory fences)：
  * 大多數現代電腦為了提高效能而採取亂序執行，這使得記憶體屏障成為必須。
  * Peterson’s Solution的亂序執行影響：
  * ![memory_barrier](https://github.com/a13140120a/Operation_System/blob/main/imgs/memory_barrier.PNG)
  * 最終會導致p1跟p2同時進入CS
  * 以下例子，預期的output是x=100：
    ```c
    boolean flag = false;
    int x = 0;
    
    /* p0 */
    while (!flag) ;
    print x;
    
    /* p1 */
    x=100;
    flag=true
    ```
  * 為了確保能夠獲得預期的output，我們加上memory_barrier
    ```c
    boolean flag = false;
    int x = 0;
    
    /* p0 */
    while (!flag) ;
      memory_barrier();
    print x;
    
    /* p1 */
    x=100;
    memory_barrier();
    flag=true
    ```
  * 這樣就能夠確保載入x之前先載入flag的值。
* test_and_set()：
  * ```
    boolean TestAndSet ( bool &lock) {
        bool value = *lock ;
        *lock = TRUE ;
        return value ;
    }
    ```
  * ```c
     /* Process 0 */                  /* Process 1 */
    do {                             do {
      while (TestAndSet(lock)) ;        while (TestAndSet(lock)) ;
      /* critical section */            /* critical section */
      lock = false;                     lock = false;
       /*remainder section */            /* remainder section */
    } while (1)                      } while (1)
    ```
  * Mutual exclusion：Yes 
  * Progress：Yes，任何一個process離開之後，其他正在waiting的process都可以進入
  * Bounded-Wait：No，假如p0先進到CS，p0結束後p1正在waiting，但p0結束馬上又搶先進到CS導致無限循環，p1 starvation(反之亦然)。
* compare_and_swap()：(CAS)
  * ```c
    int compare_and_swap(int *value, int expected, int new_value){
        int tmp = *value;
        if (*value == expected)
            *value = new_value
        return tmp;
    }
    ```
  * ```c
    lock = 0;

     /* Process 0 */                                  /* Process 1 */
    do {                                             do {
      while (compare_and_swap(&lock, 0, 1)!=0) ;       while (compare_and_swap(&lock, 0, 1)!=0) ;
      /* critical section */                            /* critical section */
      lock = 0;                                        lock = 0;
       /*remainder section */                           /* remainder section */
    } while (1)                                      } while (1)
    ```
  * Mutual exclusion：Yes 
  * Progress：Yes，任何一個process離開之後，其他正在waiting的process都可以進入
  * Bounded-Wait：No，假如p0先進到CS，p0結束後p1正在waiting，但p0結束馬上又搶先進到CS導致無限循環，p1 starvation(反之亦然)。
  * 滿足Bounded-Wait的有限等待互斥compare_and_swap()：(令有n個process)
  * ```c
    lock = 0;

    while (true){
      waiting[i] = true;
      key = 1;
      while (waiting[i] && key ==1)
        key = compare_and_swap(&lock, 0, 1); //if lock==0, then unblock, after that, lock will be 1, therefore, other cant not enter
      waiting[[i] = false;
        
        /* critical section */   
        
      j = (i+1) % n;  //start from next process
      while ((j!=i) && waitinf[j])  //search if any process is waiting
        j = (j+1) %n;
        
      if (j==i)  // if no one waiting
        lock=0;  // unlock
      else
        waiting[j] = false;  // if any process is waiting then unlock line 4, pi second time will block in line 4
        
        /*remainder section */  
        
    }
    ```

<h2 id="0064">Mutex lock</h2> 

* 這是一種programmer可以用的方式，利用`acquire()`和`release()`來取用，兩個function都必須是atomic，可以使用Mutex lock來保護CS。
* Mutex lock通常使用CAS來操作實現
* 主要缺點是busy waiting，也因為會一直重複執行(盤旋)while loop而又稱為spinlock(自旋鎖)
* 在SMP系統上，spinlock較優，而在單一processor系統上，則是使用進用preemption的方法會比較好。
* 當lock持續時間較短時，使用spin lock才會被用在kernel，而當lock必須被保持一段很長的時間時，mutex lock和Semaphore會比較適合。


<h2 id="0065">Semaphores</h2> 

* 一種用來解決synchronization problem的tool(可以比較好理解的比喻成一種類似int的資料型態，只能透過`wait()`跟`signal()`操作)
* 可以用來表示資源的數量，`wait()`跟`signal()`都是atomic
* 又分成couting semaphore 跟binary semaphore，
  * binary semaphore只能是0或1，性質類似mutex lock。
  * couting semaphore的值則可以不受限制。
* SMP必須提供代替鎖的技術來確保atomic，例如上述的`compare_and_swap()`與`test_and_set()`
* 又可以有兩種實作：
  * busy waiting的方法：會浪費cycle，但不會有context switch，所以如果waiting的時間小於兩次context switch的時間的話busy waiting是較好的選擇
    * wait()：
      ```c
      wait(S){
          while (S <=0); // busy waiting
          S--;
      }
      ```
    * signal()：
      ```c
      signal(S){
          S++;
      }
      ```
  * 使用queue的方法：不會浪費cycle，但會有context switch(system call)，system call通常會花費較大的cycle，使用情境於將會waiting很久的process。
    * 先定義semaphore：
      ```c
      typedef struct{
          int value;
          struct process * list;
      } semaphore;
      ```
    * wait()：
      ```c
      // S->list通常是PCB的list
      wait(semaphore *S){
          S->value--;
          if (S->value < 0)
            /* add this process to S->list */ ; 
            sleep();
      }
      ```
    * signal()：
      ```c
      signal(semaphore *S){
          S++;
          if (S->value <= 0)
          /* remove a process(P) from S->list */ ; 
          wakeup(P);
      }
      ```
* 假設p0具有程式碼s0，p1具有程式碼s1，我們要求s1必須在s0完成之後才可以執行，可以使用一個共同的semaphpre synch：
  ```c
  /* p0 */                  /* p1 */
  s0;                       wait(synch);
  signal(synch);            s1;
  ```
* 使用semaphore處理CS problem：
   ```c
   do {
     wait(mutex);
   
        /* critical section */  
     
     signal(synch);  
   
       /*remainder section */ 
   }while (1) 
   ```
  * Mutual exclusion：Yes 
  * Progress：Yes，任何一個process離開之後，其他正在waiting的process都可以進入
  * Bounded-Wait：Yes/No，假如p0先進到CS，p0結束後p1正在waiting，但p0結束馬上又搶先進到CS導致無限循環，p1 starvation(反之亦然)，但實作上其實進入critical section會先進入到一個queue，所以p0結束馬上又搶先到CS的話其實還是要等原本就在waiting的process執行完之後才能進去。
      
<h2 id="0066">Monitor</h2> 

* 因為semaphoer的使用非常的複雜，若是有出錯，有可能造成程式錯誤或者deadlock，為了可以讓programmer可以更方便的處理`wait()`及`signal()`的問題而誕生
* 是一種ADT(abstract data type, 抽象資料型態)
* 類似 OO(物件導向) Language的概念
* 屬於hight level的synchronization處理方式
* Monitor內部的local variable只能由內部的method存取，並確保同一時間只能有一個process在Monitor的內部活動(active)
* Monitor提供特殊的資料型態： condition variable來處理同步問題。
  * condition variable代表的就是某些event發生時他可以trigger其他process(或thread)，或者使一個process(或thread)wait一個event發生
  * condition variable可以透過`wait()`及`signal()`來操作，與semaphoer不同的是，若Monitor內部沒有任何process等待存取該condition variable，則`signal()`會沒有作用，與之相比semaphoer的`signal()`必定會改變semaphoer的狀態。
  * Pthread的condition variable有`broadcast()`方法，可以wakeup所有正在waiting的process(或thread)。
  * condition variable的`wait()`及`signal()`就像
  * 一旦Monitor內部的condition variable call了`wait()`等於是把process加到waiting queue裡面，並且變成inactive，外面的process就可以進入，或者內部本來在wait的process會active
  * `signal()`會dequeue並且wakeup 一個正在waiting的process
*  Hoare Monitor implementation using semaphore：
  *  for the process accessing the monitor：
      ```c
      semaphore mutex; // initially = 1  // mutex lock to control processes accessing the monitor
      semaphore next;  // initially = 0  // a counting semaphore "next" on which processes can suspend themselves after they are already "inside" the monitor
      int next_count = 0;                // number of inactive process that want to get into the monitor

      wait(mutex);

        /* body of process */

      if (next_count > 0)  // after done, if any process is inactive, 
        signal(next);      // up(next), unblock one of the inactive process
      else
        signal(mutex)      //if you are the only one then up mutex so someone else get in
      ```
    
  * for the wait(condition) and signal(condition) implementation using semaphore[參考解說](http：//learningnote321235.blogspot.com/2018/01/10214-hoare-monitor.html)和[參考2](https://wangwilly.github.io/willywangkaa/2018/08/04/Operating-System-Process-Synchronization-2/)：
    * x.wait()
      ```c
      semaphore x_sem;     // initially = 0
      int x_count = 0;     //number of process waiting for this condition,

      x_count++;           // number of process waiting for this condition increment by one
      if (next_count > 0)  // if any process is inactive, 
        signal(next);      // up(next), unblock one of the inactive process(including self)
      else
        signal(mutex)      // if you are the only one then up mutex so someone else get in
      wait(x_sem)          // wait(block) for someone to wake you up
      x_count--;           // you wakeup, so number of process waiting for this condition decrement by one
      ```
    * x.signal()
      ```c
      if (x_count > 0){    // if still processes waiting for this condition
        next_count++;      // number of inactive process increment by one because you wait(next) so you inactive
        signal(x_sem);     // wakeup(unblock) one of the process waiting for this condition
        wait(next);        // put yourself to sleep and get block(inactive) and join list of waiting processes, if a spot is available, down this
        next_count--;      // you wake up and(or) leave so number of inactive process decrement by one
      }
      ```
      
  * condition-wait(條件等待) Monitor with one resource：
    ```c
    monitor ResourceAllocator
    {
        boolean busy;
        condition x
        
        void acquire(int time){  // time為使用資源的最大時間，亦或者可以帶入優先權等等控制誰先恢復active
            if (busy)
                x.wait(time);
            busy = true;
        }
        
        void release(){
            busy = false;
            x.signal()
        }
        
        iniyialization_code(){
            busy = false;
        }
    }
    ```

* prioirty inersion： 
  * 假設有三個process，L, M, H，其prioirty L<M<H，假設H 想存取正在被L存取的Semaphore S，這時H必須等待L使用完S，然而此時M突然變得可執行，因此L被context switch到M，但S仍未被釋放，間接導致低優先權M，影響到較高優先權H，必須等待L。
  * 解決的方法是Priority-inheritance protocol，根據這個protocal，所有要存取資源的process必須繼承最高priority的process的priority，上述例子中，L將繼承H的priority，因此就可以阻止M把L preempt，當L使用完S之後priority會恢復到原本的樣子，並context switch到M。


<h2 id="0067">Classical Problem</h2> 

* bounded-buffer problem：
  * 設buffer有n個儲存空間
  * ```c
    semaphore mutex = 1; // 對buffer的互斥鎖
    semaphore empty = n; // buffer內現有的空格數(有多少空間)
    semaphore full = 0;  //buffer存放資料的格子數
    
    /* producer */
    while (true){
    
        /* produce an item */
        
        wait(empty); // wait if empty=0(means buffer full)
        wait(mutex);
        
        /* add item to the buffer */
        
        signal(mutex);
        signal(full); // because you put an item on buffer so increment full by one
    }
    
    /* consumer */
    while (true){
        wait(full); //wait if buffer full=0(means buffer empty)
        wait(mutex);
        
        /* remove item from the buffer */
        
        signal(mutex);
        signal(empty); // because you take an item from buffer so decrement empty by one
        
        /* cocnsume an item */
    }
    ```


* #### Readers-Writers Problem：
  * first RW problem：
    * reader不需要等待其他的reader結束(reader可以pass lock給下一個reader)，而writer要等待reader結束，有可能會造成writer starvation。
    * init：
      ```c
      semaphore rw_mutex = 1;  // reader跟writer共用
      semaphore mutex = 1;     // 用來保護read_count作加減
      int read_count = 0;      // 目前有多少個reader
      ```
    * reader：
      ```c
      while(true){
          wait(rw_mutex);  // wait if some one read or write
          
          /* **writing** */
          
          signal(rw_mutex);
      }
      
      ```
    * writer：
      ```c
      while(true){
          wait(mutex);           // protect read_count++ and if
          read_count++;
          if (read_count ==1)    // if only me
              wait(rw_mutex);    // if writer is writing, wait, else go on(if not only me, just go on, because someone else must be reading, so its impossible that a writer is writing)
          signal(mutex);         // protect read_count++ and if-condition
          
          /* **reading** */
          
          wait(mutex);           // protect read_count-- and if
          read_count--;
          if (read_count ==0)    // if no one else is using, let writer use.
              signal(rw_mutex);
          signal(mutex);         // protect read_count-- and if-condition
      }
      ```
  * second RW problem
    * 當writer ready時，就必須換writer使用，reader不得插隊，有可能會造成reader starvation。


* Dining Philosophers Example：
  * 每個人要吃飯時必須拿起左右兩支筷子
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/Dining_Philosophers.png)
  * 以下方法可以免於deadlock，但不能免於starvation：
    ```c
    monitor dp {
        enum {THINKING, HUNGRY, EATING} state[5]; //current state
        condition self[5]; //delay eating if can’t obtain chopsticks
        void pickup(int i){ // pickup chopsticks
            state[i] = HUNGRY;
            test(i);  // if neighbors are not eating, then set self=eating
            if (state[i]!=EATING)  // means neighbors are eating
                self[i].wait();
        }
        void putdown(int i){ // putdown chopsticks
            state[i] = THINKING;
            test((i+4) % 5);  // test left and if its hungry let it eat
            test((i+1) % 5);  // test right and if its hungry let it eat
        }
        void test(int i){ // try to eat
            if ( (state[(i + 4) % 5] != EATING) &&(state[(i + 1) % 5] != EATING)
              && (state[i] == HUNGRY) ) {
                //No neighbors are eating and Pi is hungry
                state[i] = EATING;
                self[i].signal();
            }
        }
        void init() {
        for (int i = 0; i < 5; i++)
        state[i] = thinking;
        }
    }
    ```

<h2 id="0068">Example</h2> 

* Windows synchronization：
  * windows kernel在單一處理器系統下存取share resource的時候會把所有存取此resource的interrupt都mask掉
  * 而多處理器系統下，windowd使用spinlock(自旋鎖)來保護較短的程式碼，且擁有spinlock的thread絕對不會被preempt。
  * 對於kernel space以外的thread synchronization，windows提供dispatcher object(分派器物件)，使用dispatcher object時，thread可以根據包括mutex lock、semaphore、event、counter來做同步。
    * windows 藉由thread獲得mutex lock來存取resource，並在結束時釋放。
    * event和condition variable類似，他可以在某個條件發生時等待一個thread。
    * counter用來通知一個thread的time quantum已經到了。
    * dispatcher object擁有兩個狀態：分別是**signaled state(信號狀態)**和**nonsignaled state(非信號狀態)**
    * signaled state表示thread可以取得此物件(釋放鎖)，反之亦然。
    * dispatcher object與thread之間的關係：當一個thread被一個nonsignaled state的dispatcher object阻擋時，thread的狀態會從ready變成waiting，並放入此object的waiting queue，當dispatcher object變成signaled state的時候，kernel會去檢查是否有任何thread在waiting queue，如果有，kernel就從裡面挑一個(或多個)把他的狀態從waiting設成ready，thread便可以恢復執行。
    * 對於mutex lock dispatcher object而言，如果thread試圖取得一個nonsignaled state的mutex lock dispatcher object時，該thread會被suspend，並放入該object的waiting queue中，當該object轉換到signaled state時，kernel只能選擇釋放一個於waiting queue中的thread，並獲得此mutex lock。
  * critical section object是使用者模式的mutex lock，並且在沒有kernel的干預下釋放。
    * critical section object在等待thread釋放物件時，會先使用spinlock，如果自旋的時間太長，則該thread將配置一個kernel的mutex lock並交出他的CPU
    * critical section非常有效率，因為kernel的mutex lock只有在發生race condition的時候才會被配置。

* Linux synchronization：
  * Linux提供一種簡單數學運算的不可分割指令：(單元整數)`atomic_t`，所有使用該類型的運算都不可中斷：
    * 此種方法特別有效率，因為不需要上鎖
    * ```c
      atomic_t counter;
      int value;
      ```
    *  | Atomic operation | Effect | 
       | --- | --- | 
       | atomic_set(&counter,5) | counter=5 |
       | atomic_add(&counter,10) | counter+=10 |
       | atomic_sub(&counter,4) | counter-=4 |
       | atomic_inc(&counter) | counter+=1 |
       | value=atomic_read(&counter) | calue=12 |
  * Linux 亦提供`mutex_lock()`來保護kernel內的critical section：
    * task在進入critical section之前必須呼叫函數`mutex_lock()`，並在離開之後呼叫`mutex_unlock()`
    * 如果目前lock無法取得，則呼叫`mutex_lock()`的task會被設為sleep，並在可以取得時被wakeup。
  * Linux kernel也提供spinlock和semaphore。
  * Linux中，如果該task已經獲得一個spinlock或mutex lock的話，除非釋放，否則不能再獲得一個相同的鎖。
  * Linux提供`preempt_disable()`和`preempt_enable()`來禁用和啟用preempt。
  * 如果kernel的task正擁有一個lock的時候，kernel是不可以preempt的，為了實現此機制，系統中的每個task都有一個thread-info的struct，包含一個preempt_count的計數器，當task獲得鎖的時候preempt_count+1，反之亦然，當preempt_count的值為0時，才可以被preempt或者被中斷。

* POSIX synchronization：
  * POSIX mutex lock：
    * `int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);`建立互斥鎖，第二個參數attr可以是以下值：  
      * PTHREAD_MUTEX_TIMED_NP，這是缺省值，也就是普通鎖。當一個線程加鎖以後，其余請求鎖的線程將形成一個等待隊列，並在解鎖後按優先級獲得鎖。這種鎖策略保證了資源分配的公平性。
      * PTHREAD_MUTEX_RECURSIVE_NP，嵌套鎖，允許同一個線程對同一個鎖成功獲得多次，並通過多次unlock解鎖。如果是不同線程請求，則在加鎖線程解鎖時重新競爭。
      * PTHREAD_MUTEX_ERRORCHECK_NP，檢錯鎖，如果同一個線程請求同一個鎖，則返回EDEADLK，否則與PTHREAD_MUTEX_TIMED_NP類型動作相同。這樣就保證當不允許多次加鎖時不會出現最簡單情況下的死鎖。
      * PTHREAD_MUTEX_ADAPTIVE_NP，適應鎖，動作最簡單的鎖類型，僅等待解鎖後重新競爭。
      ```c
      #include <pthread.h>
      
      pthread_mutex_t mutex;
      pthread_mutex_init(&mutex, NULL); //設定lock屬性，第二個參數為NULL代表使用預設參數
      
      pthread_mutex_lock(&mutex); //成功回傳0
      /* critical section */
      pthread_mutex_unlock(&mutex); //成功回傳0
      ```
  * POSIX semaphore：
    * 分成named跟unnamed。
    * create a named semaphore and lock：
      ```c
      include <semaphre.h>
      sem_t* sem;
      /* create a named semaphore and initialize to 1 */
      sem = sem_open("SEM",O_CREAT, 0666, 1);
      ```
    * create a named semaphore and lock：
      ```c
      include <semaphre.h>
      sem_t* sem;
      
      sem_init(&sem,0,1)  //第二個參數設定為 0 表示僅供目前的 process 及其 thread 使用。非 0 表示此 semaphore 與其他process共用，第三個參數表示initialize to 1
      ```
   * lock and unlock(named跟unnamed一樣)：
     ```c
      sem_wait(sem); //aquire
      /* critical section */
      sem_post(sem); //release
     ```
  * POSIX condition variable：
    * 初始化：
      ```c
      pthread_mutex_t mutex;
      pthread_cond_t cond;
      ```
    * wait跟signal的動作都必須要在critical section裡面，原因是會觸發condition variable的變數往往會需要被critical section保護，`pthread_cond_wait()`第二個參數lock在此function被呼叫的時候會被release掉，讓signal方可以進入critical section，雖然lock被release掉，但因為condition variable未被signal，所以其實會被卡住直到被叫醒，叫醒之後還必須要aquire才會繼續往下執行
      ```c
      /* wait */
      action() {
          pthread_mutex_lock(&mutex);
          if (x != 0) // x為觸發condition variable的變數，有race condition的可能
              pthread_cond_wait(cond, mutex); // release lock，被叫醒之後必須aquire才會繼續往下執行。
          pthread_mutex_unlock(&mutex);
          /* take_action */
      }
      
      /* signal */
      counter() {
          pthread_mutex_lock(&mutex)
          x--;
          if (x==0)
              pthread_cond_signal(cond);
          pthread_mutex_unlock(&mutex);
      }
      ```
* Java synchronization：
  * Java synchronized：
    * 較方便的機制，會自動幫我們控制鎖(使用者不必顧慮)
    * 當進入宣告為synchronized的method時，該thread會得到lock，這時候其他的thread 想要進來，則將自己block並且放到entry set
    * 當thread進入synchronized的method時，因為某些條件不滿足而卡在`wait()`時，會被放到wait set裡面，並觸發InterruptedException轉換到其他的thread，而其他的thread呼叫`signal()`，會從wait set裡面挑選(通常是FIFO)一個thread回到entry set並且與其他在entry set裡面的thread競爭鎖。
    * wait()要在同步方法(synchronized method)或同步區塊(synchronized block)中呼叫
       ```java
       /* synchronized block */
       synchronized(obj){
         //...
       }

       /* synchronized method */
       public synchronized void method() { 
         // ...
       }   
       ```
    * [其他請參考](https://www.jackforfun.com/java-synchronized)
    * Java monitor example：
    ```java
    public class BoundedBuffer<E>
    {
        private static final int BUFFER_SIZE = 5;

        private int count, in, out;
        private E[] buffer;

        public BoundedBuffer(){
            coint = 0;
            in    = 0;
            out   = 0;
            buffer = (E[]) new Object[BUDDER_SIZE];
        }

        /* Producer */
        public synchronized void insert(E item){ // method被宣告為synchronized
            while (count==BUFFER_SIZE){
                try{
                    wait(); // 暫停直到其他thread呼叫notify或notifyAll
                }catch(InterruptedException ie){}
            }

            buffer[in] = item;
            in = (in+1) % BUFFER_SIZE;
            count++;

            notify();
        }

        /* Consumer */
        public synchronized void remove(){
            E item;

            while(count ==0){
                try：{
                    wait();
                }catch(InterruptedException ie){}
            }

            item = buffer[out];
            out = (out+1) % BUFFER_SIZE;
            count--;

            notify();
            retuen item;
        }
    }
    ```
  * Java ReetrantLock：
    * 較靈活的鎖定機制(比起synchronized)
    * 分成NonfairSync(非公平鎖)以及FairSync(公平鎖)，可以透過修改ReetrantLock class內部的sync class來設定機制，[詳見](https://zhuanlan.zhihu.com/p/45305463)
    * FairSync會先去檢查waiting queue裡面有沒有thread在等待，沒有的話才會去aquire; 而NonfairSync則會直接去搶鎖。
    * default是NonfairSync，FairSync 的效率往往沒有 NonfairSync的效率高
    * 對於reader-writer proclem, ReetrantLock把每一個thread都鎖住，則該策略可能過於保守，所以Java API還提供ReetrantReadWriteLock，允許同時多個reader並行，但只有一個writer。
    * example：
      ```java
      Lock key = new ReetrantLock();
      key.lock(); //如果把lock()放在try子句裡面，那麼如果lock()發生異常就會跳到finally，這時因為沒有lock成功所以會觸發IllegalMonitorStateException並且覆蓋遠本的異常失敗原因。
      try{
          /* critical section */
      }
      finally{
          key.unlock(); // 避免try的時候發生錯誤而導致資源release不正常，所以不管有沒有拋出異常都要unlock
      }
      ```
  * Java Semaphore：
    * 允許負值
    * `acquire()`將拋出 InterruptedException
    * example：
      ```java
      try{
          sem.acquire()
          /* critical section */
      }catch(InterruptedException ie){ 
      }finally{
          sem.release();
      }
      ```
  * Java confition variable：
    * 因為需要提供互斥，所以要跟ReetrantLock關聯
    * Java不提供對命名的Condition variable的支援。
    * 其`await()`與`signal()`呼叫方法與上述提到的[Monitor](#0066)相同
    * Java synchronized的`wait()`和`signal()`操作僅適用於單一的Condition variable，而Condition可以允許通知特定thread。
    * example：
      ```java
      Lock key = ReetrantLock();  // create ReetrantLock
      Condition conVar = key.newCondition(); 
      Condition[] conVars = Condition[5];
      
      for (int i;i++;i<5)
          conVars[i] = key.newCondition(); // 透過ReetrantLock 建立Condition variable
      
      /* worker */
      public void doWork(int threadNumber)
      {
          lock.lock();
          
          try{
              /* if not my turn then wait */
              if (threadNumber != turn)
                  conVars[threadNumber].await();
          
              /*Do some work*/
          
              turn = (turn + 1) % 5;
              conVars[turn].signal();
          }catch(InterruptedException ie) {}
          finally{
              lock.unlock();
          }
      }
      ```

<h2 id="0069">Alternative Approaches</h2> 

* transaction memory：
  * 觀念源自於資料庫理念
  * memory transaction代表一連串單元記憶體讀寫操作
  * 如果在一個transaction中所有操作都完成，則commit，否則的話就終止並撤回
  * 鎖容易產生deadlock，且當擁有大量thread時，競爭會非常激烈，而該機制沒有鎖，所以也不會有deadlock
  * 可以用硬體或軟體實作，軟體叫做STM(software transactional memory)，硬體叫做HTM(hardware transactional memory)
  * example：
    ```c
    void update(){
        atomic{
            /* modify shared data */
        }
    }
    ```
* OpenMP：
  * thread的產生和管理由library管理而不是開發人員。
  * `#pragma omp parallel`之後的程式碼會確保thread不會發生race condition的狀況。
  * example：
    ```c
    void update(int value)
    {
         #pragma omp parallel
         {
             counter += value;
         }
    }
    ```
* Functional language：
  * c/c++、Java和C#被稱為命令式(imperative or procedural)語言。
  * 對於SMP系統強調並行與平行程式
  * 例如Scala或Erlang
  * Functional language和命令式語言的差別在於，Functional language不必維護狀態，變數一旦被定義，並且assign一個數值，就不可以被改變，也因為不允許可變，所以不會有race condition的情況發生。


****




<h1 id="007">Deadlock</h1> 

  * ## [Resource-Allocation Graph](#0071) #
  * ## [Handling Deadlocks](#0072) #




* 在正常的情況下，一個thread只能依照下列的順序來使用resource：
  1. request
  2. use
  3. release
  * 例如對devices有`request()`、`release()`、對 file則有`open()`和`close()`，對記憶體有`allocate`和`free()`等system call
* 活結(livelock)發生在一個thread連續嘗試失敗的操作，像是在走廊有兩個人面對面嘗試通過走廊，A往左邊移動，B往右邊移動，於是A又往右邊移動，B又往左邊移動，也就是當thread同時重試失敗的操作時，通常會發生。
  * 可以失敗後隨機選擇重試的運作來避免，乙太網路就是採用此方法(會退讓一段隨機的時間)。
  * 可用以下例子說明：
    ```c
    void *do_work_one(void *param)
    {
        int done = 0;
        while (!done){
            pthread_mutex_lock(&first_lock);
            if (pthread_mutex_trylock(&second_lock)){
                /* do some work */
                pthread_mutex_unlock(&second_lock);
                pthread_mutex_unlock(&first_lock);
                done=1;
            }
            else
                pthread_mutex_unlock(&first_lock);
        }
        pthread_exit(0);
    }
    
    void *do_work_two(void *param)
    {
        int done = 0;
        while (!done){
            pthread_mutex_lock(&second_lock);
            if (pthread_mutex_trylock(&first_lock)){
                /* do some work */
                pthread_mutex_unlock(&first_lock);
                pthread_mutex_unlock(&second_lock);
                done=1;
            }
            else
                pthread_mutex_unlock(&first_lock);
        }
        pthread_exit(0);
    }
    ```
* 要發生Deadlock必須要同時成立以下四種情況：
  * Mutual exclusion：only one process at a time can use a resource
  * Hold and wait：a process holding at least one resource is waiting to acquire additional resources held by other processes
  * No preemption： a resource can be released only voluntarily by the process holding it, after that process has completed its task
  * Circular wait：there exists a set {P0, P1, …, Pn} of waiting processes，p1在等p2，p2在等p3....pn在等p1。

<h2 id="0071">Resource-Allocation Graph</h2> 

* Resources types R1, R2, ..., Rm：E.g. CPU, memory pages, I/O devices, 
* Each resource type Ri has Wi instances：E.g. a computer has 2 CPUs
* 以下情形不會產生deadlock：
* ![non_deadlock](https://github.com/a13140120a/Operation_System/blob/main/imgs/non_deadlock.PNG)
* 以下情形會產生daedlock：
* ![deadlock_graph](https://github.com/a13140120a/Operation_System/blob/main/imgs/deadlock_graph.PNG)
* 以下情形，雖然有cycle，但沒有deadlock
* ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/deadlock2_graph.PNG)


<h2 id="0072">Handling Deadlocks</h2> 

* 對於Deadlock可以有以下三種方式處理：
  * 忽略此問題，假裝從未發生過deadlock：
    * 大多數的OS，例如windows 跟Linux使用此方法，
  * 允許系統出現Deadlock，出現了再來想辦法恢復(Deadlock Recovery)：
    * Process termination： 有兩種方法：一是全部kill掉，二是一次kill掉其中一個位於cycle裡面的process(問題是從哪一個process開始kill)直到deadlock解除。
    * Resource preemption： 選擇一個resource或process然後把它preempt(問題是要preempt哪一個resouce或process)，或者是rollback，(issue：要全部rollback還是要rollback到哪)，會不會老是砍掉同一個(starvation)
  * 使用某些方法防止或避免出現Deadlock，例如deadlock prevention和deadlock avoidance：
    * deadlock prevention是在設計時就要避免deadlock的發生，可以藉由讓四個發生Deadlock的必要條件不成立來達成
      * Mutual exclusion：不使用Mutual exclusion，例如read-only file就是一個很好的例子，但是許多resour本來就是不可共用的，例如printer、mutext lock
      * Hold and wait：有兩種方法可以確保不Hold and wait。1. 當一個process request a resource時，必須保證目前沒有hold任何resource。2. process執行之前必須先取得所有將會取得的resource(不切實際)，這兩種作法都有兩個主要的缺點，第一是resource utilization會非常低，因為許多resource會長期被分配而未使用，第二是starvation。
      * No preemption：當一個process正在waiting一個resource的時候，他的resource必須要可以被preempt(例如cpu，memory等等)，但是一些IO device就沒辦法使用此方法。
      * Circular wait：解決Circular wait的方法就是可以藉由把每個resource都設置一個total ordering{R1,R2,R3,R4}，並且當一個process想要取得Rk的時候必須要釋放所有Ri,(k<i)，讓所有的resource變成同一個方向，就不會有Circular wait，例如有一個process已經有R0跟R5，想要取得R3就必須要把R5 release。
    * deadlock avoidance則是在runtime的時候：
      * safe state：就是system 可以找到一個process的先後順序的safe sequence讓system可以絕對不會發生deadlock
      * unsafe state： 若找不到safe sequence，則有可能會發生deadlock
      * 分成single instance：使用[Resource-Allocation Graph(RAG)](https://www.youtube.com/watch?v=YM77tIHvYVM&list=PL9jciz8qz_zyO55qECi2PD3k6lgxluYEV&index=50)
      * 和multiple instance：使用[Banker’s algorithm](https://www.youtube.com/watch?v=YM77tIHvYVM&list=PL9jciz8qz_zyO55qECi2PD3k6lgxluYEV&index=51)
    * 某些資料庫系統使用第三種方法


****




<h1 id="008">Memory Management</h1> 

  * ## [Address Binding](#0081) #
  * ## [Contiguous Memory Allocation](#0082) #
  * ## [Non-Contiguous Memory Allocation(Paging)](#0083) #
  * ## [Non-Contiguous Memory Allocation(Segmentation)](#0084) #
  * ## [Swapping](#0085) #
  * ## [Example](#0086) #




<h2 id="0081">Address Binding</h2> 

* ![c_compile](https://github.com/a13140120a/Operation_System/blob/main/imgs/c_compile.png)

* [參考Computer_Organization_And_Architecture的memory部分](https://github.com/a13140120a/Computer_Organization_And_Architecture#007)
* [當編譯到執行](https://github.com/a13140120a/Computer_Organization_And_Architecture#0065)
* [Loader and Linker請參考](#0024)
* source檔中的位址通常是符號化的(symbolized)，而通常compiler 會將這些符號bind到一個可重定位的位址(例如離某個segment開始14個bytes的地方)，而lin
* 指令和資料至記憶體位置的連結(bind)可在以下任何步驟完成：
  * Compile time：
    * ![comepile_time](https://github.com/a13140120a/Operation_System/blob/main/imgs/compile_time.PNG)
    * 最早期的方式，在compile的時候就去assign phisycal address
    * 生成出來的是absolute code(絕對碼)，裡面的address都會被bind到真實且固定的phisycal address
    * 缺乏彈性，除非recompile否則無法搬動(因為virtual swapin再swapout回來的時候原來的空間可能已經被占用，所以需要搬動)，但是run-time的時候非常快，因為直接可以access address，不需要做額外運算
  * Load time：
    * ![load_time](https://github.com/a13140120a/Operation_System/blob/main/imgs/load_time.PNG)
    * 若程式在compile時不能確定在記憶體中的位置，則compiler必須產生relocatable code(可重定位程式碼)，並由loader來決定address的binding(例如.BS+0x14，BS由loader決定)。
    * 一但記憶體位置搬動，必須要reload(把程式kill掉)，沒辦法在runtime的時候搬移。
  * Execution time：
    * ![execution_time](https://github.com/a13140120a/Operation_System/blob/main/imgs/execution_time.PNG)
    * 必須要有特殊的硬體才可以(MMU)
    * 0x18為相對於此process的位址的偏移值
    * 上圖0x18其實是指0x2018，但是load到memory之後來是0x18，原因是因為會經由MMU轉換(CPU自己不知道)
    * compile出來與load到memory的都是所謂的logical-address(virtual address)
    * 是現在電腦大部分的作法
* logical address： CPU發送出來的address，
* phisycal adderss：memory真正看到的，也就是載入[MBR](https://github.com/a13140120a/Computer_Organization_And_Architecture/blob/main/README.md#0041)的數值，而此數值通常是CPU發出來的address經由MMU轉換而來的
* MMU(memory-managment unit 記憶體管理單元)：
  * ![MMU](https://github.com/a13140120a/Operation_System/blob/main/imgs/MMU.PNG)
  * 是一種硬體裝置
  * 會有一個relocation register裡面會儲存藥加上得值(例如14000)
  * context switch的時候會把memory base address load到relocation register裡面
* Dynamic Loading
  * 當一個routine被呼叫的時候，會先去找memory裡面有沒有，如果沒有的話才會load到memory裡面。
  * 可節省memory空間(例如當有大量的error handle code的時候)
  * 決定那些code是Dynamic Loading的是使用者，而不是OS
  * Example：
    ```c
    // dlopen()： open a library and prepare it for use
    // desym()： looks up the value of a symbol in a given(opened) library.
    // dlclose()： closes a DL library
    #include <dlfcn.h>
    
    int main() {
        double (*cosine)(double);
        void* handle = dlopen ("/lib/libm.so.6", RTLD_LAZY);
        cosine = dlsym(handle, "cos"); // 定義cosine是system的dynamic library裡面的cos這個function
        printf ("%f\n", (*cosine)(2.0));  // 當call到這行的時候才會把function load進memory
        dlclose(handle);
    }
    ```
* Static linking：
  * gcc在call linker之後會發現執行檔變大了，原因是因為加上了Static linking library(xx.lib)
  * link完之後就會把程式碼併在一起
  * 會在memory裡面造成duplicated code
  * Dynamic Loading + Static linking還是沒有解決duplicated code的問題
  * 優點是比Dynamic linking速度更快
* Dynamic linking：
  * 只有一份Code 放在記憶體裡面，並且可以share給everyone
  * 需要的時候再去用就好了
  * DLL(Dynamic linking library)又稱為shared libraries(共享程式庫)
  * compile的時候就必須決定是static library還是dynamic library
  * 需要透過OS才有辦法完成，因為會牽涉到Processes之間的共享空間
  * 當被call的時候OS會先去找看看有沒有在memory裡面，如果沒有的話才會load 進memory
  * 跟Dynamic Loading的差別在於Dynamic linking可以共享，只需一份
  * 相較Static linking較慢一點

<h2 id="0082">Contiguous Memory Allocation</h2> 

* 每個process都包含在一個記憶體中(physical)的單獨連續空間，這個空間稱為partition
* Partition又有分Fixed-partition allocation 和 Variable-size partition：
  * Fixed-partition allocation：因為每個partition都一樣大，所以會造成內部空間的浪費(就像機車停在汽車停車格一樣)
  * Variable-size partition： 記憶體在經過多次的填補與抽離之後，會造成hole，而為了解決新產生的process要塞進哪個hole，有以下三種方法：
    * First-fit：尋找第一個找到的合適的hole(速度較快，不用traverse整個list)
    * Best-fit：尋找可以填進去的所有hole裡面最小的hole(需要traverse整個list，但相對較節省空間)
    * Worst-fit：尋找可以填進去的所有hole裡面最大的hole(需要traverse整個list)
  * Fragmentation：
    * Internal fragmentation：一個partition內部出現沒有用到的memory空間稱為Internal fragmentation，只會發生在fixed-partition allocation，其中一個solution是把partition size切得更小。
    * External fragmentation：一個partition外部出現沒有用到的memory空間(hole)稱為External fragmentation，只會發生在variable-size allocation，其中一個solution是compaction(重新排列記憶體空間，把每個partition都貼在一起)

<h2 id="0083">Non-Contiguous Memory Allocation(Paging)</h2> 

* page： 把**logical address space**分成多個相同size的chunk 稱為page
* frame：把**physical memory**分成多個相同size的chunk 稱為frame
* 如果要 run 一個需要n個page的program, 則需要準備n個frames來裝。
* 允許**physical-address space**是不連續的
* 沒有external fragmentation，而且降低internal fragmentation(page size越小的話internal fragmentation越少)
* Dynamic linking的實作使用的技術就是共享的page。
* [詳細資料可以參考](https://github.com/a13140120a/Computer_Organization_And_Architecture/blob/main/README.md#0077)
* page number經過查詢page table之後會得到相對應的frame number：
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/page_map.jpg)
* page table是一個由OS mantain的structure，放在memory裡面，每個process都有它自己的page table，page table裡也只會有這個process的page而已，process不能隨意access自己以外的memory space。
* 由CPU產生出來的address都分成兩個部分： page number 和 page offset：
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/page_offset.png)
* N bits的process可以allocate 2的N次方的pages，也就是最多會有2的N次方乘以page size的memory size
* N bits的offset代表page size是2的N次方
* Physical addr = frame base addr + frame offset
* ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/page_table2.PNG)
* 例子：
  * 假設Page size是1KB(2^10)，Page2 maps到frame5，今有13個bits的logical address(p=2,d=20), physical addr就是5*(1KB)+20 =1,010,000,000,000+0,000,010,100=1,010,000,010,100
  * 假設logical address有32 bits,physical address有36 bits，則：
    * page size為4KB, 
    * Page table 有2的20次方個entries(有2的20次方個個page，也就是說需要20個bits來定位page)
    * offset有12個bit(4KB)
    * Max program memory就是2的32次方(4GB)的空間，
    * Total physical memory size： 2的36次方(64GB)
    * 有2的24次方個frames
* OS會有一個free-frame list(或frame table)來記錄frame是否可用，分配給哪一個page或哪一個process。

* Hardware Support
  * PTBR(page-table base register)： 通常會使用一個PTBR來指向page table的base addr(phisycal，因為不再做translation)，base addr的值會儲存在PCB裡面，等到Context switch的時候會把這個base addr load到PTBR裡面。
  * TLB(translation look-aside buffer)： 
    * 為了解決一次read要兩次access的問題，裡面儲存了少量的page tabale的entry
    * 是一個特殊的小型硬體快取記憶體
    * 由Associative memory所組成
    * 包含一個key跟一個value，Associative memory可以同時跟所有的key做比較
    * [wiki 典型的TLB](https://zh.wikipedia.org/wiki/%E8%BD%89%E8%AD%AF%E5%BE%8C%E5%82%99%E7%B7%A9%E8%A1%9D%E5%8D%80#%E5%85%B8%E5%9E%8B%E7%9A%84_TLB)
    * 加上TLB之後的Access memory流程圖：
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/page_table2.PNG)
    * 假設access TLB要20ns，access memory要100秒，有70% TLB hit-ratio，EMAT(Effective Memory-Access Time)就等於0.7x(20 + 100)+(1-0.70)x(20+100+100)=150ns
    * 如果TLB滿了，就必須要找到一個key來犧牲掉
    * Intel Core i7有128個entry的L1 instruction TLB跟64個entry的L1 data TLB，L1 cache miss之後會到L2，L2有512個entry的TLB。L2 cache miss之後就要到Memory裡面去找。
    * 發生context switch 的時候TLB有兩種Solution，第一種也就是現代電腦最普遍的做法，就是整個flush掉，第二種就是多加一個PID(ASID, address-space identifier 位址空間識別碼)的欄位，標記每個page是哪個process的。
  * Memory Protection：
    * 通常每個page都會有一個(或一些)欄位用來儲存一些protection bit，這些bit有可能可以用來判斷是不是read-only或者read/write等等，而protection bit中最常使用的就是所謂的valid-invalid bit，
    * valid-invalid bit代表目前的page是不是可用的，例如virtual memory技術裡面，當這個page是invalid的狀態時，那麼使用這個page就會產生page fault(因為swap in 到disk裡面)。
    * PTLR(page-table length register)用來表示page table的大小，以保護一個process在正確的範圍內存取memory。
  * Shared Pages：
    * 可以多個page map到同一個frame，避免不必要的記憶體空間浪費，DLL就是使用此項技術。

* Page Table Memory Structure
  * PTE：page table entry
  * 因為page table通常太大，沒辦法塞進單一個frame裡面，而通常page table又必須存在連續的記憶體空間，於是有多種solution出現。
  * Hierarchical Paging：
    * 把page table分成階層式的，缺點是要access的次數變多
    * 例如Two-level paging(32-bit address with 4KB (2^12) page size)
      * 12-bit offset (d) = 4KB (212) page size
      * 第一層有10-bit(inner page) ：1K (2^10) 個page table entries，代表一個inner page table有1K個page
      * 第二層有10-bit(outer page) ：1K (2^10) 個page table entries，代表一個outer page table有1K個inner page table
      * 要access三次
  * Hashed Page Table：
    * 每個entry由page number, frame number以及指向下一個entry的pointer所組成
    * virtual addr的page number部分被hash到hash table
    * 通常用於處理大於32 bit的virtual addr
    * 比較virtual addr的page number和hash table的page number，如果相同則命中，如果不相同則traverse這個entry的linked list
    * 與virtual addr的bit數量無關，所以更有彈性，如果使用到的page很少，就不會存在bucket，可以節省大量記憶體空間
    * 壞處是如果collision就要一直traverse link，而且pointer會浪費記憶體空間
    * 示意圖：
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/hash_table.png)
    * 改良後的page table：
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/better_hash_table.PNG)

  * Inverted Page Table：
    * 不使用每個process都有一個page table，而使用單一個frame table，節省記憶體空間，而且table的使用率是百分之百(不會有多餘的frame)
    * 每個access都必須要search整個frame table，可以使用hashing解決這個問題
    * 每個entry不能有多個pid跟page，所以無法共享實體frame。
    * 通常一個entry會由process id(PID)、page number、offset三項所組成
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/inverted_page_table.png)


<h2 id="0084">Non-Contiguous Memory Allocation(Segmentation)</h2> 

* 是Non-Contiguous Memory Allocation的variable size
* 是以使用者的角度去切割(page是以系統管理的角度)
* ![Segmentation-table-example](https://github.com/a13140120a/Operation_System/blob/main/imgs/Segmentation-table-example.jpg)
* Segmentation Table：
  * Logical address格式： [Segment, offset]，一個page的size可以是4KB等等，但這裡的offset必須要跟系統定義的能長到最大的physical addr的長度一致(例如4GB，23個bit)
  * 每個entry都必須要有：
    * Base (4 bytes)： Segment的base address
    * Limit (4 bytes)： Segment的長度
  * 會有STBR(Segment-table base register)：segmentation table的base addr
  * 還有STLR(Segment-table length register)：segmentation table的長度，用來檢查存取的位置是否超過segment的範圍
  * ![segmentationHardware](https://github.com/a13140120a/Operation_System/blob/main/imgs/segmentationHardware.jpg)
* Sharing of Segments： 因為要program在memory可能是非常複雜的，必須要知道在哪幾個page可以share，而且會有internel fragmentation的問題，所以比較上層的Segments sharing就比較親近programmer。
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/segmentation_share.PNG)
* Protection & Sharing
  * Protection bits associated with segments
    *  paging所提到的protection bit是一個例子，存取記憶體真正的protection通常是在segmentation5
    *  Read-only segment (code)
    *  Read-write segments (data, heap, stack)
  *  Code sharing occurs at segment level
    * Shared memory communication
    * Shared library
    * Share segment by having same base in two segment tables
* Segmentation with Paging：
  * Apply segmentation in logical address space
  * Apply paging in physical address space
  * ![Segmentation_with_page](https://github.com/a13140120a/Operation_System/blob/main/imgs/Segmentation_with_page.png)

<h2 id="0085">Swapping</h2> 

* Memory的content(page)在memory跟disk之間移動的動作叫做swap
* backing store(備份儲存體，或稱swap space)： 一塊用來儲存從memory swap到disk的儲存空間，通常在安裝OS的時候設定Swap space的大小，通常Swap space跟檔案系統是分開的，使用midterm scheduling。

* swap page示意圖：
  * ![swap_page](https://github.com/a13140120a/Operation_System/blob/main/imgs/swap_page.jpg)
* 如果memory addr的binding是在load time或者compile time的話，那麼當disk中的page swap回memory當中就必須要到相同的記憶體位址
* 但如果memory arrd的binding是在executing time的話，就可以swap到memory的任意位置
* 當一個process被swap的時候，必須要是Idle的狀態(不能是在正做IO的狀態)，因為如果IO寫回memory的時候剛好這個位址的page(或process)已經被swap掉(換成其他page或process)了，就會寫到別的page裡面去，有兩種解決辦法：
  * 一是swap 的時候不做IO，但是這樣會影響效能。
  * (較通用的作法)二是OS會allocate一個專門用來裝IO要寫回memory資料的buffer，當IO要寫回memory的時候會先寫回buffer，然後當page(或process)被swap回memory的時候才從buffer去拿。
* 標準swap 是swap整個process，但是這樣太缺乏效率而且沒有必要，
* 所以通常都是swap一個page而已，而swap page又稱為**page in** 跟**page out**。

* 行動裝置：
  * 行動裝置通常使用快取記憶體來代替空間較大的硬碟作為永久性儲存裝置
  * IOS系統通常在可用的主記憶體空間降到一定門檻的時候會強制要求應用程式主動放棄所配置的記憶體空間來取代Swap機制，這時唯讀資料(例如程式碼)會從系統中被移除，而已經被修改過的資料(例如stack)則不會被移除，然而，若還是不能夠釋放足夠的記憶體空間則該應用程式會被IOS OS系統終止。
  * Androi不支援swap，如果沒有足夠的記憶體空間可以使用的話，Android會終止一個process，在終止process之前，android會寫入該process的application state到快取記憶體裡面(行動裝置的永久儲存體)，因此可以快速的重啟應用程式。

<h2 id="0086">Example</h2> 

* Intel 32和64位元架構：
  * IA-32 架構：
    * IA-32 架構的記憶體管理被分成兩個元件，segmentation unit跟paging unit，cpu產生的logical address會先送到segmentation unit，然後產生一個linear address之後再送到paging unit產生physical address
    * 流程圖：
    * ![流程圖](https://github.com/a13140120a/Operation_System/blob/main/imgs/IA32_1.PNG)
    * IA-32 架構允許segment的大小最大可以到4GB，並且最多可以擁有16K個segment，其logical address由兩個部分組成：(selector, offset)
    * 段描述符表(descriptor table)：IA-32處理器把所有段描述符按順序組織成線性表放在主記憶體中，稱為段描述符表。
      * 分為三類：全局描述符表GDT、局部描述符表LDT、中斷描述符表IDT。
      * GDT和IDT在整個系統中只有一張，而每個任務都有自己私有的一張局部描述符表LDT。
      * GDTR全局描述符寄存器：48位，高32位存放GDT基址，低16為存放GDT限長。
      * LDTR局部描述符寄存器：16位，高13為存放LDT在GET中的索引值。
      * 目前使用的LDT, 其位址和limit存放在 LDTR暫存器中
      * 主記憶體的GDT, 其位址和limit存放在 GDTR暫存器中
    * 段選擇器(selector)：
      * 32位彙編中16位段寄存器([CS](https://iter01.com/560468.html)、DS、ES、SS、[FS](https://www.firbug.com/a/202106/196009.html)、GS)存放段描述符在段描述符表中的索引值、指示位TI(TI=0指示從全局描述表GDT中讀取描述符，TI=1指示從局部描述符中LDT中讀取描述符)、優先級(RPL用於特權檢查)。這些信息總稱段選擇器(段選擇子).
      * CS 暫存器具有另外一個重要的功能 ： 它包含一個2位元欄位, 用來指定CPU當前的特權等級
      * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/IA32_selector.PNG)
      * cpu從descriptor table 中找到段描述符(descriptor)之後加上offset得到linear address
      * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/IA32_discriptor_table.PNG)
    * 產生出來的linear address會再送到paging unit：
      * IA-32 架構允許兩種分頁大小，4KB與4MB，在4KB的page中，IA-32使用two-level的階層分頁法，32位元的linear address分割如下：
      * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/IA32_page.PNG)
      * 最高位的10個bit referece到outer table(IA32稱為page directiry)的entry(進入點)，而中間10個bit(1024個)為inner table的index，最低的12個位原則是4KB的page offset
      * CR3：是一種處理有關分頁目錄 (page directory)的控制暫存器，CR3中含有頁目錄表實體記憶體基地址，因此該暫存器也被稱為頁目錄基地址暫存器PDBR（Page-Directory Base address Register），[詳細內容](https://wanker742126.neocities.org/new/dos_asm/ap03.html)
      * 如果PSE(page size extensions, CR4的bit 4，在Pentium和以後的處理器才有)旗標設為1時，才可以使用4MB的分頁大小，那麼p1就會是page table的index，而剩下的22bit就是offset。否則就只能只用4KB的page，那麼p1跟p2就分別用來表示outer table跟inner table的index。[詳細](https://www.csie.ntu.edu.tw/~wcchen/asm98/asm/proj/b85506061/chap2/paging.html)
      * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/IA_32_2level_page.PNG)
    * [PAE(Physical Address Extension)](https://zh.wikipedia.org/wiki/%E7%89%A9%E7%90%86%E5%9C%B0%E5%9D%80%E6%89%A9%E5%B1%95#%E9%A1%B5%E8%A1%A8%E7%BB%93%E6%9E%84)技術使得32位元處理器可以處理大於4GB的實體位址空間。
    * 其他名詞：PDE(page directory entry, windows page table的outer table)
    * [參考資料](https://www.cntofu.com/book/46/linux_kernel/1217.md)
    * [其他參考資料](https://www.cnblogs.com/shiqi17/p/12650640.html)

  * x86-64 架構：
    * Intel一開始的64位元架是IA-64，但是這時候AMD也開始發展64位元架構，稱為 x86-64，它是以現有IA-32 指令集為基礎下去擴展和支援更大的logical 和physical address，當 x86-64架構出現時，原本AMD根據Intel架構發展晶片的情況反轉，變成Intel採取AMD的x86-64架構，後來AMD64與Intel64通稱為x86-64。
    * 64位元的address space支援2^64次方(16 exabytes)的記憶體定址，但實際上，目前x86-64只提供48位元的虛擬地址，使用四層的分頁結構，支援4KB、2MB或1GB的分頁，x86-64的virtial address大小是48個bit，有PAE的話則可以支援到52 bits(1024兆個bytes)。
    * ![x86_64_page_translation](https://github.com/a13140120a/Operation_System/blob/main/imgs/x86_64_page_translation.png)

* ARM架構：
  * ARM除了行動裝置以外，還提供嵌入式系統的架構，以下將描述64位元ARMv8的架構。
  * 雖然是64位元結構，但其實只有使用到48位元的位址，高位元的25個bit全為1的話代表使用kernel的virtual address，全為0的話代表使用的是user 的virtual space。
  * ARMv8句有三種不同的translation granules(轉換顆粒)： 4KB、16KB、64KB，每種translation granules提供不同的page大小，以及較大的連續記憶體部分稱為region(區域)
  * | ranslation granules大小 | page大小 | region大小 |
    | --- | --- | --- |
    | 4 KB | 4 KB | 2 MB |
    | 16 KB | 16 KB| 32 MB |
    | 64 KB | 64 KB | 512 MB |
  * ![ARMv8_page_translation](https://github.com/a13140120a/Operation_System/blob/main/imgs/ARMv8_page_translation.png)
  * 如果使用的是4KB的translation granules的話，那麼第0/~11個bit是page的offset，此時如果使用的region是2MB的話，那麼0/~20個bit就是region內的offset，但如果使用的region是1GB的話，那麼0/~29個bit就是region內的offset。
  * [詳細](https://www.cnblogs.com/-9-8/p/8406345.html)


****




<h1 id="009">Virtual Memory</h1> 

  * ## [Bcakground](#0091) #
  * ## [Demand Paging](#0092) #
  * ## [Demand Paging Performance](#0093) #
  * ## [Copy-on-Write](#0094) #
  * ## [Page Replacement](#0095) #
  * ## [Allocation of Frames](#0096) #
  * ## [Thrashing](#0097) #
  * ## [Memory Compression](#0098) #
  * ## [Allocating Kernel Memory](#0099) #
  * ## [Other Considerations](#00910) #
  * ## [Operating System Examples](#00911) #




<h2 id="0091">Bcakground</h2> 

* 一個process的執行，不需要把整個process都load 到memory裡面去，因為以下幾種情況：
  * 程式碼當中會有許多handle error的程式碼，但是這些程式碼並不是每次執行都會被使用到(或極少使用到)。
  * 當allocate一個array或list 的時候，我們可能allocate非常大(例如100 * 100)但實際上使用到的確只有10 * 10的空間。
  * 某些function很少被執行到。
* 而使用virtual memory有以下幾個好處：
  * 程式不再被實體記憶體所限制，可以使用超過實體記憶體的記憶體空間
  * 提高cpu/resourese的使用率(multi programming)
  * load program的速度較快
* VirtualMemory是意圖：
  * ![VirtualMemory](https://github.com/a13140120a/Operation_System/blob/main/imgs/VirtualMemory.jpg)
* virtual是屬於鬆散空間，因為這樣才有位置可以塞進成長的stack、heap或者DLL的code部分到自己的virtual memory：
  * ![process_between_dll](https://github.com/a13140120a/Operation_System/blob/main/imgs/process_between_dll.png)

<h2 id="0092">Demand Paging</h2> 

* 只有在需要用到的時候才把page load到memory
* **page fault**： 通常會有每個PTE會有一個valid-invalid bit的欄位來表示此page存在memory與否(limit用來保護違規存取)，如果不存在的話將會觸發**page fault**，並交由OS處理：
  * 可以參考Computer_Organization_And_Architecture的[page fault流程](https://github.com/a13140120a/Computer_Organization_And_Architecture/blob/main/README.md#0077)
  * 補充：發生page fault時會先檢查internel table(通常會再PCB裡面)，來決定是有效還是無效的reference，若是無效，便終止process，若是有效，才把page從disk load到memory當中，在這中途可能會context switch到另一個process，等待load的動作結束之後才會切換回來。
  * 
* pure demand paging：在沒有任何page在memory裡面的情況下執行process，簡單來說就是當execute一個process的時候可能就create一個page table、PCB就好。 
* 觸發page fault會使該instruction重新執行，然而，執行一個instruction的時候可能不只發生一個page fault，例如一個instruction的功能是把一段區域A的byte搬移到另一個區域B，這時候CPU在存取A的時候因為A區域橫跨了兩個page，於是在搬移到一半的時候觸發了page fault，這時候instruction重新執行，原來A的前半段部分已經被修改過了，解決這個問題有兩個方法：
  * 先嘗試存去兩個端點，如果有page fault被觸發，那就可以在修改之前先讓所有的page fault發生
  * 使用臨時的暫存器把修改過的區域記錄下來，如果發生page fault的話，就把舊的內容寫回記憶體中，並且處理完page fault之後重新執行一次instruction。
* swaper：把整個process存到disk裡面
* pager：把page存到disk裡面
* swaper跟pager不會衝突，例如swap要把這個process存到disk裡面，然後交給pager處理，pager再決定要把這個process的哪個page存到disk裡面。
* Free-Frame List： OS通常會有一個free-frame list，裡面裝著空白的frame，而OS通常會使用一種叫做「**zero-fill-on-demand**」的技術來把free-frame list裡面的frame在被配置之前填(清)零。

<h2 id="0093">Demand Paging Performance</h2> 

* 其EAT(有效存取時間) = (1-P) x ma(memory access time) + p x pft(page falt time)：
  * 假設ma = 200ns, pft = 8ms
  * 那麼EAT = (1 - p) * 200ns + p * 8ms = 200ns + 7,999,800ns x p
  * 從上述可以看到page fault 發生的機率是決定效能好壞的關鍵因素。
  * 如果p(page fault rate)是僅僅千分之一的話，效能就會下降40倍(速度為原來的40分之一)
  * 如果系統效能下降的程度要控制在10%以內，p必須要小於0.0000025才行。
  * 因為memory 其實有locality的特性，所以p通常都會非常小。

* Optimizations：
  * 儘管是在同一顆disk上面，系統對於swap space的存取速度還是會大於對file system控制範圍的存取速度，原因是因為file system在存取硬碟的時候需要做很多的檢查、以及控管，另一個原因是因為swap space 通常會有比較大的chunk size，這會增加一次access的資料量，進而增加存取速度，也因為如此，有些OS會在一開始就把整個process的image檔都load到swap space裡面。
  * 而一開始就把整個process的image檔都load到swap space裡面的方法也會造成一些缺點，其中一個缺點便是process的load time會特別久，所以另一種做法是： 需要的page(demand page)才會從file system讀出來到memory當中，當這個page要被swap到disk時，系統會先檢查有沒有modify過，如果有的話就存到swap space，如果沒有的話就直接覆寫掉該page(而不是swap到disk)，需要的話就再從file system載入，這種做法可以把減少系統對於swap space的使用量。
  * 像這種不會寫回file system(可能被overwrite或者swap到swap space)的memory page又稱為「Anonymous memory」(匿名記憶體)，這些page所存放的資料通常是stack、heap裡面的資料，也就是使用`malloc()`、`sbrk()`、`brk()`、`mmapp()`這些system call所配置出來的記憶體空間。[(sbrk()參考)](https://stackoverflow.com/questions/6988487/what-does-the-brk-system-call-do)

<h2 id="0094">Copy-on-Write</h2> 

* Copy-on-Write(寫入時複製)： 藉由共享page來取代複製，當有寫入的動作發生的時候，才複製出一份新的page讓其中一個process使用(parent or child)
* `fork()` system call使用此技巧，當呼叫`fork()`的時候，傳統的做法是，child會複製parent的memory content，然而，考慮到通常`fork()`之後會接著`exec()`、複製整個parent process的memory 空間是一件浪費時間與空間的事情，因此，我們可以讓parent與child最初共享parent的memory page，這些memory page會被標註為copy-on-write的page，這表示如果有其中一個process想要寫入共享的page，才會copy 一份新的page讓process寫入
* process1修改 page C之前：
  * ![copy_on_write_1](https://github.com/a13140120a/Operation_System/blob/main/imgs/copy_on_write_1.jpg)
* process1修改 page C之後：
  * ![copy_on_write_2](https://github.com/a13140120a/Operation_System/blob/main/imgs/copy_on_write_2.jpg)
* 有些UNIX版本(包括Linux、MacOS、BSD Linux)會提供`vfork()`system call，vfork的全名是"virtual memory fork"，使用`vfork()`時，parent process會被suspend，而child process會使用parent的memory space，因為`vfork()`在使用時不會有Copy-on-Write，所以child的任何改動，parent都是看的見的，通常`vfork()`使用完之後會馬上接著`exec()`，也因為不會發生分頁的複製，所以`vfork()`是一種非常有效率的process產生方式。



<h2 id="0095">Page Replacement</h2> 

* virtual memory的其中一項好處是可以增加degree of multiprogrammimg，假設今天配置了6個process，系統中有40個frame可以使用，每個process大小都是10個page，但實際用到的其實只有5個page，這時候就可以裝得下，但假設突然間這6個process都必須要使用完整的10個page，這樣子就over-allocating記憶體了。
* 當page fault發生時，可能會需要一個空的(free)frame來放這個「我們需要但是不在記憶體裡面的page」，而這時候如果系統上卻已經沒有空的frame，就必須選擇一個犧牲的欄位(稱為victim frame)，把這個victim frame swap 到disk，讓我們需要的page可以load到memory上面來。
* Basic Page Replacement：
  * modify bit(dirty bit)： page replacement需要兩個動作： swap out一個page到disk，跟swap in一個page到memory，這使得處理page fault的時間會double，因此，在硬體中通常每個page或frame會有一個dirty bit，如果這個page被寫過的話，dirty bit就會被設成1，當我們需要選擇一個page來swap進disk的時候，就可以先檢查這個bit，如果dirty bit是1的話，那我們才會寫回disk，如果不是的話就代表不會寫回disk，可以大幅減少處理page fault的時間。
* First-In-First-Out (FIFO) Algorithm：
  * 顧名思義，先進先出，最老的page會先被replace掉
  * 建立一個FIFO的queue，當要替換掉的時候就把最上面的page replace掉，而當一個page被load 進memory的時候就插入queue的尾端。
  * 假設 Reference string： 1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5
  * 一開始有3個free frames
  * 使用FIFO Algorithm會出現9個page fault：
  * ![FIFO](https://github.com/a13140120a/Operation_System/blob/main/imgs/FIFO.PNG)
* Belady's anomaly：
  * FIFO會發生的現象
  * 當frame數提高的時候page fault可能不會提升或甚至反而下降的現象。
  * 如下圖，當使用上述的Reference string，並把frame數提升到4個的時候，page fault反而上升到了10個
  * ![beladys](https://github.com/a13140120a/Operation_System/blob/main/imgs/beladys.PNG)
  * Belady's anomaly FIFO的page fault發生曲線圖(非與上述Reference string相同)：
  * ![beladys_plot](https://github.com/a13140120a/Operation_System/blob/main/imgs/beladys_plot.PNG)
* Optimal Page Replacement：
  * 把未來最長時間之內不會被用到的那一頁替換掉
  * 被證明是「一定是最好的演算法」
  * 可以看到使用相同的Reference string之下，只出現了6個page fault
  * 實際上是無法達成的，因為我們無法知道未來會出要那些page
  * 常常會用來被跟其他演算法比較，藉此評估效能。
  * 注意下圖並不是每個page真的在memory裡面搬來搬去，而是為了方便觀察而改變順序
  * ![optimal_algorithm](https://github.com/a13140120a/Operation_System/blob/main/imgs/optimal_algorithm.PNG)
* LRU Algorithm (Least Recently Used)：
  * 近來最少使用演算法，白話文就是最久未被使用演算法
  * 比起Optimal Page Replacement是使用「向前看」的方式，那麼LRU就是「向後看」(跟FIFO一樣都是向後看)
  * 一個有趣的地方是，如果我們把Reference string倒過來(5, 4, 3, 2, 1, 5, 2, 1, 4, 3, 2, 1)那麼page fault的次數也不會改變
  * 可以看到使用相同的Reference string之下，出現了8個page fault，雖然沒辦法跟optimal一樣，但是比FIFO還多。
  * ![LRU](https://github.com/a13140120a/Operation_System/blob/main/imgs/LRU.PNG)
  * LRU的實作方法有兩種：
    * Counter： 把time stamp copy 進去counter裡面，搜尋的時候只要搜尋最小的counter的值就可以了，需要注意overflow的問題。
    * stack： 美當一個page被access時，就把該page從stack中移出，並且放到最頂端，如此一來最頂端的page就是最近被用過的，那麼最底端的page就是要被犧牲的page，可以使用double linked list來實作，每當要移動一個page並放在頂端的時候，就需要更改六個pointer，使用hash map來儲存每個page在double linked list的位置，如此一來要搜尋該page就只需要order one的時間。
    * 無論是Counter或者是stack的方式來Iimplement LRU，硬體上除了TLB支外的部分，沒有硬體是辦不到的，每次access memory都必須要更改counter或者stack的值會讓系統效能，如果使用軟體的方式，那麼效能將會變成原來的十分之一，所以必須採用其他的方法，例如LRU-Approximation Page Replacement。
* LRU-Approximation Page Replacement：
  * 會有一個reference bit(參考位元)，當這個page被access(包括讀取和寫入)，這個reference會被硬體設定為1，雖然我們無法知道被使用的順序，但我們知道哪修被使用過，而哪修還沒被使用過，這種部份排班資訊可以使許多replacement algorithm盡量接近於LRU，基於這個reference，可以衍伸出下列兩種演算法。
  * Additional-Reference-Bits Algorithm：
    * 會為PTE保存一個8位元的reference bit位元組，每經過一段時間(例如100毫秒)的interrupt(clock interrupt)這個位元組就會產生中斷並把控制權交給OS，OS會把每一頁的reference bit向左shift到最高位元(most significant bit, msb)，然後把其他的位元向右shift一個bit，這樣這8個位元就代表過去這8段時間的這頁的使用紀錄，
    * 如果位移暫存器中的位元是00000000，那麼表示這八段時間內該頁都沒有被使用過，如果是11111111，代表這八段時間內該頁至少都被使用過一次
    * 如果某暫存器的值是11000100，代表上一次，上兩次以及上六次使用過
    * 如果某暫存器的值是01110000，表上上一次，上上上次以及上四次使用過
    * 這樣只需要比較大小，就知道誰最近最少被使用了，11000100上一次使用是前一次，01110000上一次使用是前兩次，所以01110000這頁會被替換掉
    * 如果有好幾個page的數值都是一樣的，我們可以把所有最小值的page都swap出去，或是使用FIFO的方法來選擇其中一個
  * Second-Chance Algorithm：
    * 又稱為clock page-replacement algorithm
    * 會有一個FIFO的queue(環狀的，因為搜尋到queue的尾端時，會再回到頂部重新開始搜尋)
    * 基本上是一種FIFO的演算法，可是當某個page被選出來之後，我們檢視他的reference bit，如果為0的話，則swap進disk，
    * 如果為1的話，我們就把他的reference bit清除為0，然後給他second chance，並且繼續往下搜尋整個queue，直到所有的page都被替換掉，或者被給予second chance。
    * 如果這個page經常被使用，那麼他的reference一直都會是1，代表他永遠不會被swap到disk裡面
    * 當每個page的reference bit都為1時，Second-Chance Algorithm就會退化成FIFO了(第一次search 整個queue的時候會把全部的page都設成0，然後挑最頂端的)。
    * ![Second_chance_algorithm](https://github.com/a13140120a/Operation_System/blob/main/imgs/Second_chance_algorithm.jpg)
  * Enhanced Second-Chance Algorithm：
    * 使用跟Second-Chance Algorithm相同的技巧，但會有一個reference bit(表示被access過)跟一個dirty bit(表示被修改過)
    * 有了這兩個bit可以得到以下四種類型：
    * 1.(0,0)：表示沒有被access過也沒有被修改過，為最佳替換的page
    * 2.(0,1)：表示沒有被access過但有被修改過(應該是指沒有被讀取但有被寫入過?)
    * 3.(1,0)：表示有被access過但沒有被修改過，可能很快會被再使用到
    * 4.(1,1)：表示有被access過而且有被修改過，最差候選page
    * 檢查這兩個bit，並且從1開始為最高優先swap到disk依序往下
    * 有可能要搜尋過好幾次整個queue才可以找到victim page
    * 這種方法傾向於選擇不曾被修改過的page，可以降低IO時間
* Stack Algorithm：
  * 一種類型的演算法
  * LRU跟Optimal都是屬於該類型的演算法
  * 只要屬於這類型的演算法都不會遭遇Belady's anomaly
  * 定義：the set of pages in memory for n frames is always a subset of the set of pages that would be in memory with n+1 frames，意思是，如果今天有n個frame，在同樣的reference string 的input之下，變成n+1個frame之後，原來的set還是會被include在裡面
  * FIFO不屬於stack algorithm
* Counting-Based Page Replacement：
  * 又分成LFU與MFU
  * 兩種演算法執行的時候的cost代價都很昂貴。
  * LFU Algorithm (least frequently used)：
    * 使用次數最少的那個page被swap掉
    * 缺點是，若是有一個page在process剛開始的時候大量用到，但之後就沒有再用到過了，可是因為counter的值很大，所以會一直留在memory當中
    * 有一種解決方法就是每經過一段時間把counter的值往右shift一個位置，造成使用次數會隨著時間指數下降。
  * MFU Algorithm (most frequently used)：
    * 使用次數最多的那個page被swap掉
    * 一些理論認為，使用次數最少的代表才開要開始使用，所以應該留在memory，反而把使用次數較多的替換掉。
* Page-Buffering Algorithms：
  * 是一種優化演算法的方法(演算法)，概念是： 系統會有一個free frame的庫存(free frames pool)
  * 當需要選擇一個victim page的時候(而這個victim page又因為有被modify過所以要先swap回disk，才可以把disk的page swap到這個frame上)，會先從free frame的庫存裡面挑一個出來，然後從disk裡面先把要用的page swap到這個frame裡面，之後再把原本被modify過的page寫回disk裡面，並且把這個frame清零後放進free frames的庫存裡面
  * 相較於原本的「先swap進disk，再swap回memory」，可以節省較多的時間(類似資源回收桶的概念)
  * 這個觀念的其中一種延伸是，幫被modify過的page保存一個table，每當這些page閒置的時候把他的content寫回disk，並且把dirty bit設為0，這樣就可以增加選出來的victim page未被修改過的機率了。
  * 另外一種延伸是，存進free frame的庫存的時候，不把內容全部清零，並且標上這個frame是屬於哪個page的，因為內容在swap進disk之後並不會更改，所以當如果某個那個page的時候，就可以直接從free frame庫存裡面去找看看有沒有，就不需要再去disk 做IO，可以節省較多時間。
* Applications and Page Replacement：
  * 一些應用程式，因為它們有特殊的特性和需求，應該盡可能地自己管理memory和disk，而不是依賴OS的Page Replacement或file system，通過使用raw IO(直接存取硬碟做IO而不透過file system)，一些應用程式可以更有效地運行。數據庫(data base)和數據倉庫(data warehouse)就是此類應用程式的示例。

* 作業系統通常區分兩種類型的page fault：主要(major)以及次要(minor)，(windows稱major為hard page fault，minor為soft page fault)，
  * 當使用該page，但該page不在記憶體上時，會觸發major page fault，
  * 而當該頁存在於memory中，但process沒有map到該頁的logical mapping的時候會發生minor page fault，而發生的原因有以下兩種：
    * process可以使用共享程式庫(dll)，在這種情況下，process只需要更新page table就可以了。
    * 第2種原因是，當系統回收(reclaim)page，並將其放在free frame list裡面的時候，這時該page的內容尚未被清空(之前使用的內容尚在，會有安全的疑慮)，這時只要把該page清空，然後重新分配給該process就可以了，第2種原因通常會發生在allocate heap的時候。
    * 可以使用`ps -eo min_flt,maj_flt,cmd` 來查看Linux系統當中major和minor的page fault數量以及啟動該process的command。
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/major_minor_page_fault_command.PNG)
    * [詳細參考](https://www.learnsteps.com/difference-between-minor-page-faults-vs-major-page-faults/)

<h2 id="0096">Allocation of Frames</h2> 

* 每個process都有一個最少配置frame的數量，若少於這個數量，process將無法執行。
* 舉例來說，如果使用[間接定址(indirect addressing)](https://github.com/a13140120a/Computer_Organization_And_Architecture/blob/main/README.md#%E5%AE%9A%E5%9D%80)，例如load可能會涉及第0個page，而第0個page又會去reference第6個page等等，這樣這個process至少需要三個frame才能夠執行。
* Allocation Algorithms：
  * Equal Allocation(同等分配)：每個process平均分到相同的frame，例如system當中有93個frame和5個process，那麼每個frame可以分到18個frame，而剩下的3個可以當作free frame的庫存，這種分配方法會造成較大的process可能沒有足夠的frame，或者較小的process分配到太多沒有用到的frame。
  * Proportional Allocation(比例分配)：按照每個process的大小去分配，例如有62個frame，分給兩個process，一個是10KB的小型程式，一個則是127KB的大型程式， 那麼總共的size是137KB，於是我們分給小process 10/137 * 62約等於四個frame，而大的process則獲得127/137 * 62約等於57個frame，如果有新的process近來或者舊的process執行完畢，都會改變系統配置給每個process的數量。
  * 當然，如果這個process的priority較高的話，可以多分配一些frame以增加其效能，反之亦然。

* Global versus Local Allocation
  * Local allocation：
    * 允許一個process可以從別的process中取得frame
    * 有可能因為frame數不足而導致延遲。
  * Global allocation：
    * 每個process只能使用分配給自己的frame
    * 這種方法允許高priority的process可以增加配給自己的frames數量。
    * 缺點是process不能控制自己的page fault rate，因為frame會被較高priority的process搶走
    * 因為會受到其他process的影響，所以執行同樣的process有可能會有不同的狀況出現(例如第一次執行只需要0.5秒，另一次執行則需4.3秒)
    * 通常會產生較好的系統效能，是較常用的方法
  * 系統中通常會有一種核心常式(kernel routine)，該routine會從所有process中回收page(reclaim page)，而這種常式通常被稱為**reaper(收割者)**，他們會應用上述的任何演算法，當可用記憶體數量到達最大的閾值(threshold)的時候系統會suspend routine，直到低於最低的閾值的時候會再resume routine，並且將回收回來的frame放到free frame list當中。
  * 如下圖，當達到最低直a的時候啟動reapers，直到達到最高點b的時候suspend reapers。
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/reaper.jpg)
  * 收割常式(reaper)，通常會使用一些如上述的類似LRU演算法，如果仍然無法將free frame list保持在最小閾值以上的話，這時候可能會暫停使用Second-Chance Algorithm，改成使用FIFO，
  * 或者更極端的例子： Linux 系統中，當記憶體數量降到極低時，OOM(out-of-memory killer)這個process會選擇一個process將其終止，每個process都會有所謂的OOM score，較高的score會增加該process被終止的機會，OOM score是根據process使用的記憶體百分比來計算，其中，pid為2500的process其OOM score可以使用`cat /proc/2500/oom_score`查看，reaper不僅可以reclaiming page，還可以隨著時間調整最大與最小閾值，而這些值管理者都可以根據需求調整預設值。


<h2 id="0097">Thrashing</h2> 

* 如果一個process花在paging的時間比花在executing的時間還多，這種現象就稱為Thrashing
* 如果一個process沒有足夠的frame，那執行中將會大量出現page fault，因為每個page都很忙碌，所以把這個page swap到disk之後馬上又須要把他swap回來。
* 當一個process突然進入到新的狀況，並且需要更多的frame，於是他開始產生page fault，然後從其他的process拿走frame，其他的process因為frame減少了，於是又開始大量的產生page fault，造成所有的process都在排隊等待frame，這時候cpu的queue就空了，空了之後cpu就認為cpu使用率變低，於是就增加degree of multiprogramming，讓更多的process進到cpu scheduling裡面，更多的process需要更多的frame，如此惡性循環，早成cpu使用率急遽下降
* ![Thrashing](https://github.com/a13140120a/Operation_System/blob/main/imgs/Thrashing.jpg)
* 使用Local page replacement(process不能搶奪其他process的frames)可以限制thrashing，但如果多個process都出現thrashing現象，仍然會阻塞IO queue，造成效能下降。
* locality model(局部區域模式)： 藉著觀察process上一段時間使用的page，來決定process目前需要多少個frame，所謂的locality就是指一組同時被使用的page，舉例來說，當一個function被呼叫的時候，這個process就到了一個新的locality，而當這個function return的時候，就又來開了這個locality。
* ![locality](https://github.com/a13140120a/Operation_System/blob/main/imgs/Locality.jpg)
* 如果我們分配的frame比locality的大小還少，那麼該process就會出現thrashing，而如何為每個locality定義他的frame數? 以下有兩種方法：
  * working-set model：
    * 定義一個參數∆，這個∆代表working-set window，也就是最近∆個page的reference，而在最近∆個page所組成的set就是working set。
    * 下圖中t1時間內的working set是{1,2,5,6,7}，t2的working set是{3,4}
    * ![WorkingSetModel](https://github.com/a13140120a/Operation_System/blob/main/imgs/WorkingSetModel.jpg)
    * D = ∑WSSi，WSSi是Wirking set size，D是process目前working set對frame的需求，如果需求大於可用frame數(D>m)的話，將會發生thrashing，那我們就必須選擇另一個process，把他的frame給這個thrashing的process，反之，如果D<m的話，那麼就可以把這個process多出來的frame拿給其他需要的process使用。
    * 要實現working-set model是非常困難的事情，因為每次的reference都必須把working-set window的一端加入新的紀錄，且另一端要移除舊的紀錄，就像working-set window在移動一樣
    * 另一種類似working-set window的方法是，假設∆為10000，我們可以每5000次的reference就觸發一次clock interrupt，並且把這5000次的每一個page的reference bit記錄下來(例如page 1在過去5000到10000次有被reference，還有過去10000次到15000次有被reference，那麼用兩個bit來表示就是11)，這樣如果產生了page fault，我們就可以去參考過去5000/~15000次的reference中(因為∆=10000)，當然我們可以增加用來記錄的bit數以及觸發clock interrupt的次數來增加精確度，可是更頻繁的unterrupt將會付出更高的代價。
  * Page-Fault Frequency：
    * 是一種更直接的控制方法
    * 當Page-Fault Frequency太高的時候，代表發生thrashing，代表需要更多的frame，反之亦然
    * 使用這種方法的cost會比working-set model要低很多，因為working-set model要記錄每個access，而Page-Fault Frequency只有發生page fault的時候才需要紀錄。

<h2 id="0098">Memory Compression</h2> 

* paging(in/out)的替代方法
* 當我們要replace一個page的時候，這個page有可能已經被modify過了，所以必須要先把原本的content寫回disk(或secondary storage)，另外一種方法，也就是Memory Compression，可以把多個modify過的frame壓縮到一個frame當中，這樣就不用一直存回disk(或secondary storage)當中了。
* 舉例來說，當free frame的數量低於某個閾值的時候，會觸發replacement的動作，假設選擇了第13, 3, 35, 26個frame放到free frame list裡面，通常這些page當中，如果有被修改過的話就會被寫入disk當中：
* ![memory_compression_before](https://github.com/a13140120a/Operation_System/blob/main/imgs/memory_compression_before.jpg)
* 而Memory Compression的做法則會把其中三個frame壓縮成一個frame，並存在compressed frame list裡面：
* ![memory_compression_after](https://github.com/a13140120a/Operation_System/blob/main/imgs/memory_compression_after.jpg)
* Windows 10和MacOS都支援Memory compression
* MacOS一開始會先使用Compression LRU page，如果記憶體不足的話才會用paging。
* 行動裝置不支援paging(in/out)，所以絕大部分行動裝置都是使用這種方式管理記憶體。
* 這種方法被證明確實可以減少記憶體空間，並且使用SSD當成backing store的paging更快速。


<h2 id="0099">Allocating Kernel Memory</h2> 

* kernel的記憶體空間不同於user program，不能輕易地被paging或切割，例如I/O buffering這種必須要直接與硬體溝通的地方，一定要是連續的實體記憶體空間
* 再者，有些kernel的structure不能隨意的被swap到disk當中，必須常駐於記憶體，且須小心地管理這些不同大小的structure，避免產生internal fragmentation，才可以給user更大的記憶體使用空間。
* 因此可以有以下兩種策略：
  * Buddy system(夥伴系統)：
    * 使用二的次方配置器(power-of-two allocator)來配置記憶體，配置出來的chunk都是二的冪次方(4KB, 8KB,16KB等等)。
    * chunk之間可以自由合併與切割。
    * 比如我們要裝一個21KB的structure，可以把一個256KB的連續實體記憶體切成兩分AL跟AR，再把AL切成BL跟BR，再把BL切成CL，於是變成32KB就可以符合21KB的structure
    * Buddy system的一個很好的優點是：只要把chunk的地址與chunk的大小進行xor運算就可以獲得Buddy的地址，因此可以非常快速的合併。
    * Buddy system的一個很明顯的缺點就是容易造成fragmentation，像是33KB的就只能使用64KB的page，浪費了31KB的空間。
    * ![BuddySystem](https://github.com/a13140120a/Operation_System/blob/main/imgs/BuddySystem.jpg)
  * Slab allocator(平板分配)：
    * 一個slab由一個或多個實體的連續page所組成(不能是半個或不到一個)
    * 一個cache由一個或多個slab所組成，每個cache只能裝一種object
    * 假設有一個object大小為2KB，且slab為12KB(由三個4KB的page所組成)，那麼如果一個cache需要裝6個object則需要有1個slab
    * cache是為了map slab跟object而存在的(因數與倍數的關係)。
    * slab 向 buddy system 去「批發」一些記憶體，加工切塊以後「零售」出去。
    * Slab allocator使用cache來儲存object，每當cache被產生的時候(等於一次產生多個object)，這些object會被標示成free，當配置器分配了其中一個object之後這個object便會被標示成used。
    * slab可能存在三種狀態：full, partial, empty。
    * 例如Linux的struct task_struct大概需要1.7KB的記憶體，當產生一個新的task的時候kernel會需要為這個structure配置一個1.7KB的記憶體，此時一個struct task_struct的object已經在在slab當中，可以大大增加速度。
    * 使用這個技術不會有fragmentation的問題，因為每個object的大小都是量身訂製的。
    * Linux使用SLUB取代SLAB，SLUB降低許多SLAB造成的overhead(負擔)。
    * 在Ubuntu上面可以用 sudo cat /proc/slabinfo or sudo slabtop 看到 slab caches的相關資訊。
      * OBJS指的是這個process調用了多少個object。
      * ACTIVE代表有多少object正在使用中，USE是active的比例。
      * SLABS指這個process使用多少個slab來管理，而OBJ/SLAB就是指一個slab擁有多少個object。
    * [參考資料](https://hackmd.io/@sysprog/linux-memory)
    * ![SlabAlloc](https://github.com/a13140120a/Operation_System/blob/main/imgs/SlabAlloc.jpg)


<h2 id="00910">Other Considerations</h2> 

* Preparing：
  * 在pure demand paging剛開始執行時，會發生大量的page fault，會影響CPU的效能，所以Prepaging的目的就是要減少process startup(或process context switch)的時候大量的page fault。
  * 其策略就是把同一時間內，該process需要的所有page都記錄下來。
  * 例如，我們可以使用working-set model，並且把每個process的working set用到的每個page都保存成一個表，假如我們需要暫時switch到其他的process的時候，就可以記住該process的working set，當這個process被恢復執行的時候，就可以把整個working set都載入到記憶體中(如果有足夠的frame的話)。
  * 但如果預先載入(preparing)的page根本就沒有用到的話，那麼這些cost就都白費了，假設有s個page被preparing了，而其中只有a(百分比例，0<a<1)部份被使用到，如果a很接近0的話，代表這些preparing都是白費工夫，但如果a很接近1的話，代表大部分都有被使用到，那就有達成效果
  * 預測分頁是非常困難的，但是檔案讀取是sequential的，因此是可以preparing的，Linux的`readahead()`system call可以將檔案的內容預讀到記憶體當中，以便後續的快速存取。
* Page Size：
  * 通常，計算機硬件的體系結構決定了頁面大小，所以作業系統設計師不太有選擇的機會，但假設要重新設計新的機器時，就必須要做決定。
  * 在過去的三十年左右，計算機設計的趨向於越來越大的page size，當前的頁面大小範圍高達4KB，即使在移動裝置上也是如此。
  * 沒有所謂「最好」的page size
  * IO的時間是由seek time(搜尋)，rotational latency(旋轉)以及搬移資料的時間所構成，與前兩者比較之下，搬移資料的時間簡直是小巫見大巫，譬如在每分鐘50MB的搬移速率來看，要搬512個byte也只需要0.01毫秒，而rotational latency可能高達3毫秒，seek time更高達5豪秒，因此就算整個page的大小增加為兩被，IO的時間也只不過是從8.01毫秒增加到8.02毫秒而已，但主要是讀入的量增加，page fault的次數也會減少(IO時間主要減少的原因)。
  * 當page size較大時：
    * 可以減少page table的size
    * 可以減少IO的時間，如上述
  * 當page size較小時：
    * 會增加page table的size
    * memory可以得到較佳的使用率(internel fragmentation較少)
* TLB reach：
 * TLB Reach代表TLB可存取的記憶體範圍
 * 增加TLB的大小可以增加hit ratio，但較大的TLB非常昂貴，而且很耗電。
 * 大小為(TLB size) * (page size)
 * 可以藉由增加page size來提高TLB reach(這樣就不用增加TLB的大小了)。
 * 但增加page size會使一些較小的process的internel fragmentation增加，解決辦法是是用多個page size
 * 例如提到的三種ARMv8的page size(64KB, 1GB, 2MB)，ARMv8中的每個PTE都包含一個contiguous bit的欄位，可以在單個PTE中map到三種可能排列方式:
   * 64KB：包括16個4KB的相鄰的page
   * 1GB：包括32個32MB的相鄰的page
   * 2MB：包括32個64KB的相鄰的page，或者128個16KB的相鄰的page。

* Inverted Page Tables:
  * 如之前介紹過的，使用frame table而非每個process一個的page table。
  * 但假如page不在memory裡面的話，就無法儲存在frame table了(因為不在任何一個frame上面，所以frame table裡面也不會有)。
  * 為了解決這個問題，我們可以使用一個externel table(每個process一個)，上面記載著所有不在memory裡面的page的資訊，記錄著他們在disk的哪個位置，不同於一般的page table，這個externel table平常的時候都是放在disk裡面，只有發生page fault的時候才會從disk裡面拿出來使用。
  * 因此，使用這種方式還是可以減少memory的使用量。

* Program Structure:
  * 因為陣列是以列(row)為主的方式存放資料的，如下兩個程式，若page size為128個bytes，第一個程式會產生128x128=16384個page fault，而第二個則只會產生128個:
  * ```c
    int i, j;
    int[128][128];
    for(j=0;j<128;j++)
        for(i=0;i<128;i++)
            data[i][j] = 0;
    ```
  * ```c
    int i, j;
    int[128][128];
    for(i=0;i<128;i++)
        for(j=0;j<128;j++)
            data[i][j] = 0;
    ```
  * 因此謹慎的設計資料結構可以增加locality，並可以降低page fault rate，例如stack可以有很好的locality，但是hash就是分散開來的，因此locality較差。
  * compiler 跟loader會優化這些動作。

* I/O Interlock and Page Locking:
  * 某些page因為某些原因而不能輕易地被page out到disk裡面，例如某個process正在做IO(把disk的內容搬到buffer上面)，當做到一半時候CPU就被context switch到另一個process，結果這個IO寫回memory的位置就不是原本的page，於是就會寫到別的page上面，所以這時候就需要lock。
  * kernel中的許多位置都是被所在記憶體當中的。


<h2 id="00911">Operating System Examples</h2> 

* Linux:
  * Linux使用demand paging並且從free frame list分配page
  * Linux使用類似LRU-Approximation Page Replacement的方法:
    * Linux有兩個page list，分別是active_list以及inactive_list，active_list包含正在使用的page，而inactive_list則包含有資格被回收的page。
    * 每個page都會有一個access bit
    * 每當該page被配置的時候就把access bit設為1，並將其增加到active_list的後面
    * 只要access active_list裡面的page的話，就會再把access bit設為1，並將該page移到list的尾端
    * 會定期重製active_list中的page的access bit 為0
    * 過程中最少使用的page將會位於active_list的前面，這個page就會被移動到inactive_list裡面
    * 如果在inactive_list裡面的page被access的話，又會被移動到active_list的後面
    * 當active_list變得比inactive_list大太多的時候，active_list前面的page將會移動到inactive_list
    * Linux 會定期call一個叫kswapd 的daemon，kswapd會檢察系統中的可用記憶體數量，如果低於特定的threshhol，kswapd就會把inactive_list中可用的page放到free list當中

* windows:
  * windows 32位元系統中，process預設的virtual address space為2GB，最多可以到3GB，並支援4GB的physical address space
  * windows 64位元系統中，process具有125TB的virtual address space， 並支援24TB的physical address space
  * windwos 10使用clustering的方式來implement虛擬記憶體，該方法不僅可以處理page fault，還可以處理page fault的前後多個page
    * 對於資料分頁(data page)，clustering包含3個page(包含前一個跟後一個page)
    * 對其於他的page，clustering則包含七個分頁(前三後三)
  * 當一個process建立時，windows會將其配置最少(working set minimum)50跟最多(working set maximum)345的working set，除非有設定hard working-set limit，否則都可以忽略這些值，如果有夠多的記憶體，則可能超過最大值，反之亦然。
  * windows 使用類似Second-Chance Algorithm的演算法，並結合Local allocation與Global allocation
  * 管理器維護一個free frame list，並且會有一個閾值，該閾值會顯示是否有足夠的memory可以使用，
  * 如果某個process在低於working set maximum的時候發生page fault，則從free frame list分配一個free frame給他
  * 如果如果某個process處於working set maximum的時候，還是發生了page fault，而且這時候有足夠的記憶體空間，那就會發給他一個frame，讓他超過working set maximum，
  * 但如果如果某個process處於working set maximum的時候，還是發生了page fault，這時候記憶體空間不足，則會使用Local的LRU來進行replace。
  * 當記憶體空間降到threshold以下，會觸發working set trimming這個全域(global)替換策略，將值恢復到threshold以上，這時候如果有process的分頁超過working set minimum的話，會回收他的一些page值到有足夠的記憶體空間，或者達到working set minimum為止。
  * 從較大的idle process開始(優先於較小的idle process)，必要時會將已經低於working set minimum的process繼續刪除page。


****




<h1 id="010">Mass-Storage Structure</h1> 

  * ## [Overview of Mass-Storage Structure](#0101) #
  * ## [Nonvolatile Memory Devices](#010) #
  * ## [Volatile Memory](#0103) #
  * ## [Secondary Storage Connection Methods](#0104) #
  * ## [Address Mapping](#0105) #
  * ## [HDD Scheduling](#0106) #
  * ## [ECC](#0107) #
  * ## [Storage Device Management](#0108) #
  * ## [Swap-Space Management](#0109) #
  * ## [RAID Structure](#01010) #
  * ## [Object Storage](#01011) #




<h2 id="0101">Overview of Mass-Storage Structure</h2> 

* 傳統HDD磁碟機具有以下結構:
  * ![HDD_DiskMechanism](https://github.com/a13140120a/Operation_System/blob/main/imgs/HDD_DiskMechanism.jpg)
  * 磁盤(platter)：一個或多個磁盤形式的盤片，磁盤的兩面皆有覆蓋磁性介質，較舊的硬碟不會使用最頂端的那面以及最底端的那面，因為這兩面較容易受到損壞。
  * 磁軌(track)：每個表面被分成許多稱為磁軌的同心環。
  * 磁柱(cylinder)：與盤片邊緣距離相同的所有磁軌的集合（即上圖中的所有表面的同一磁軌）稱為磁柱(cylinder)。
  * 磁區(sector)：每個磁軌進一步分為磁區，傳統上每個磁區包含512 bytes的數據，有些現代硬碟偶爾會使用更大的扇區大小，磁區還包括標頭(header)和標尾(trailer)，其中包括校驗信息(checksum information)。較大的磁區大小會減少標頭和標尾佔用的磁盤比例(較少的header和trailer)，但會增加Internal fragmentation。
  * 讀寫頭(或磁頭，read-write head)：硬碟由讀寫頭讀取數據，標準配置（如上圖所示）每個表面使用一個讀寫頭，每個讀寫頭在一個單獨的磁臂(arm)上，並由一個通用磁臂組件控制(arm assembly)，該組件將所有讀寫頭同時從一個磁柱移動到另一個磁柱。（每個表現都有獨立的讀寫頭，可以加快硬碟速度，但會涉及嚴重的技術難題。）
  * 傳統硬碟的存儲容量等於讀寫頭數量（即表面的數量）乘以每個表面的磁軌數，乘以每個磁軌的磁區數，乘以每個磁區的bytes數。
  * 硬碟透過 **head-sector-cylinder number** 來定位要讀寫的區塊。
* 當磁碟機運轉時，磁碟機馬達會以高速運轉，大多數驅動器每秒旋轉 60 到 250 次，以每分鐘轉數(rotation per minute, RPM)表示，例如 7200 rpm（每分鐘7500轉，每秒120轉）
* positioning time(稱定位時間，或稱隨機存取時間，random access time)是由以下兩部份所組成：
  * 搜尋時間(seek time)：是將讀寫頭從一個磁柱移動到另一個磁柱以及讀寫頭在移動後穩定下來所需的時間。這通常是過程中最慢的步驟，也是整體傳輸速率的主要瓶頸。
  * 旋轉延遲(rotational latency)：所需的sector旋轉到磁頭位置所需要的時間
* 傳輸速率(transfer rate)：即以電子方式將數據從磁盤移動到計算機所需的時間（一些人也可能使用transfer rate來指代整體傳輸速率，包括seek time和rotational latency以及電子數據傳輸時間。）
* 磁碟速度可透過硬碟controler裡面的buffer來提高效能。

* 硬碟磁頭在極薄的(以微米測量)一層空氣或其他氣體(氣墊)（如氦氣）上飛行，並且磁頭有與磁盤表面接觸的危險，雖然磁盤盤片塗有一層薄薄的保護層，但磁頭有時會損壞磁面，這種損壞就稱為head crash，head crash 通常無法修復，必須更換整個磁盤。

<h2 id="0102">Nonvolatile Memory Devices(NVM，非揮發性記憶體裝置)</h2> 

* NVM裝置通常由 controler 和 NAND快閃半導體晶片組成
* 硬碟、光碟與磁帶雖然也是非揮發性儲存裝置，但被歸類為現行NVM一般特指非機械式之電子類記憶體元件。
* USB、SSD、和有電池的DRAM都屬於NVM。
* 快閃記憶體（flash memory），是一種電子清除式可程式唯讀記憶體(EEPROM)的形式，又分為NOR與NAND兩型。
* 一些NVMs被設計成可以直接連線到system bus(例如PCIe)
* NAND NVM 的壽命不是以年為單位，而是以每天的驅動器寫入(DWPD, Drive Writes Per Day)為單位，例如，一個5 DWPD rating的 1TB NAND deiver(SSD)預計在保固期內每天有5TB的寫入數據而不會出現故障。 
* 優點(相較傳統磁碟):
  * 沒有移動性的元件，所以更reliable(較抗震，所以手機都是用此類型裝置做儲存)
  * 沒有seek time或rotational latency，所以較快
  * 較不耗電
  * 體積較小
* 缺點(相較傳統磁碟):
  * 較貴
  * 容量較小
  * 壽命較短

*  SSD 在寫入數據時將電子存儲在 NAND 單元上。使用 NAND 閃存，一個記憶單元經過寫入(Program)，可由邏輯1變成邏輯0，但無法再經由寫入將此單元回復到邏輯1，需經過抹除(Erase)才可回復邏輯1。因此，存儲或刪除新數據時無法覆蓋已存儲的數據。
*  寫入(Program)和抹除(Erase)操作SSD 的執行是在不同的單元中進行的，寫操作以頁(pages)為單位執行，而抹除操作以塊(blocks)為單位執行，
*  NVMs(SSD)由許多NABD晶片所組成，而每個晶片都有multiple data path，因此操作之間可以平行發生。
*  OS只負責簡單地讀取和寫入邏輯塊，而硬體設備則負責管理"如何完成"，然而，NVM設備的性能會根據其操作的演算法而有所不同，因此有必要簡要討論控制器的功能:
  * 如果某些block被一直重複Program/Erase，那麼block就會被很快磨損殆盡，因此應該防止重複寫入相同的block
  * 為了追蹤哪些block包含有效的資料，因此在快閃記憶體檔案系統中，會經由[快閃記憶體轉換層(Flash Translation Layer, FTL)](https://zh.wikipedia.org/wiki/%E5%BF%AB%E9%96%83%E8%A8%98%E6%86%B6%E9%AB%94%E8%BD%89%E6%8F%9B%E5%B1%A4)來進行對讀、寫、抹除操作的管理。
  *  Wear-leveling(磨損均衡)：是FTL提供的其中一種防止對某個區域的重複寫入操作的功能，通過將暴露於大量 P/E(Program/Erase) 的塊與空閒塊交換，使單元能夠被均勻地利用，從而允許用戶在給定條件下使用 SSD 更長時間。
  *  ![wear_leveling](https://github.com/a13140120a/Operation_System/blob/main/imgs/wear_leveling.PNG)
  *  由於 NAND 閃存不可能進行覆蓋，因此必須首先抹除現有數據才能將新數據寫入該單元，這會產生許多無效的頁(當想要更新page的時候會寫一份新的到free blok裡面)，並且會降低 SSD 的整體寫入性能，而且通常抹除數據比寫入數據需要更長的時間。
  *  為了緩解這種寫入性能的下降，實施稱為[垃圾收集 (GC, garbage collection)](https://zh.wikipedia.org/wiki/%E5%9E%83%E5%9C%BE%E5%9B%9E%E6%94%B6_(%E8%A8%88%E7%AE%97%E6%A9%9F%E7%A7%91%E5%AD%B8)) 的過程以在 SSD 內創建空閒塊。這項技術，通過將有效頁面收集到一個位置並抹除由無效頁面組成的塊來保護空閒塊。
  *  而這些有效頁面被收集到的地方就稱為over-povisioning(OP)，通常會暫總儲存量的20%。
  *  ![garbage_collection.PNG](https://github.com/a13140120a/Operation_System/blob/main/imgs/garbage_collection.PNG)
  *  [詳細白皮書](https://semiconductor.samsung.com/resources/white-paper/S190311-SAMSUNG-Memory-Over-Provisioning-White-paper.pdf)

<h2 id="0103">Volatile Memory</h2> 

* 例如DRAM
* 當儲存在RAM裝置上的資料在系統發生當機，關閉或斷電後將無法繼續保存資料
* RAM裝置允許使用者(Programmer)向檔案一樣操作他，得以在記憶體中暫時保管資料，在Linux有`/dev/ram`，windows則要透過第三方工具
* RAM可用做高速暫存空間，速度比NVM更快

<h2 id="0104">Secondary Storage Connection Methods</h2> 

* 儲存裝置都常會有一組線，稱為I/O匯流排(I/O bus)連接到電腦，常用的幾種匯流排技術有:
  * 進階技術連接(ATA, advanced technology attachment)
  * 串列進階技術連接(SATA, serial ATA)
  * 外部串列進階技術連接(Externel serial ATA)
  * 小型電腦系統介面(SCSI, small computer systems interface)
  * 串列式傳輸介面技術(SAS, serial attached SCSI)
  * 萬用串列匯流排(USB, universal serial bus)
  * 光纖通道(FC, fiberchannel)
* 資料經由控制器(controler)或主機匯流排轉接器(HBA, host bus adapter)，的特殊電子處理器在匯流排上進行傳遞

<h2 id="0105">Address Mapping</h2> 

* 儲存裝置被定址為一個一維的邏輯區塊(logical block)大型陣列。
* logical block是資料傳輸的最小單位
* logical block會被map到實體的sector或page(上述SSD的page單位)
* sector 0 通常為硬碟最外圈的第一條磁軌的第一個磁區
* [邏輯區塊位址（Logical Block Address，LBA）](https://zh.wikipedia.org/wiki/%E9%82%8F%E8%BC%AF%E5%8D%80%E5%A1%8A%E4%BD%8D%E5%9D%80): 是描述電腦儲存裝置上資料所在區塊的通用機制，一般用在像硬碟這樣的輔助記憶裝置。LBA可以意指某個資料區塊的位址或是某個位址所指向的資料區塊。現今電腦上所謂一個邏輯區塊通常是512或1024位元組。
* 將邏輯區塊號碼轉換成舊形式的(CHS(cylinders-heads-sectors，磁柱-磁頭-磁區)定址模式)磁碟位址是非常不容易的，其原因有三：第一，磁碟上可能會出現損毀的磁區，那麼logical就會map到其他的地方來代替這些損毀的磁區，第二，而且在某些硬碟上每個磁道的磁區數是會變動的，第三，map的方式通常會由製造商內部管理。
* 恆定線性速度(constant linear velocity, CLV)：每個磁軌的密度是相同的，因為外圈的磁軌有較大的長度，所以當磁頭從外側移到內圈的時候，會增加旋轉速度，這種方法常被用在CD-ROM或DVD-ROM當中。
* 恆角速度(constant angular velocity, CAV)：磁碟旋轉速度保持固定，但外圈的磁軌密度較低，內圈較高，以保持資料速度穩定。


<h2 id="0106">HDD Scheduling</h2> 

* 因為整個硬碟的讀取速度中，seek time是佔最大部份時間的，因此就需要有Scheduling去盡可能減少磁頭移動的距離
* 當OS發出一個IO request的時候，需要指定以下必要資訊:
  * 是input還是output?
  * 指向操作的檔案
  * 傳送的記憶體位置
  * 要傳送多少資料量?
* 頻寬(bandwidth): 傳送的位元組總數除以從第一個request到最後完成傳送所需的總時間
* HDD Scheduling的目標是公平、快速、最佳化。

* 假設現在IO request queue裡面有以下排列：98、183、37、122、14、124、65、67
* FCFS(First come, first serve)：
  * 假設起始地點為53，則順序53、98、183、37、122、14、124、65、67
  * 是公平的，但不能提供最快的服務
  * 最簡單的演算法
  * ![HDD_FCFS](https://github.com/a13140120a/Operation_System/blob/main/imgs/HDD_FCFS.PNG)
* SSTF(Shortest-Seek-Time-First)：
  * 假設起始地點為53，則順序53、65、67、37、14、98、122、124、183
  * 最短尋道時間演算法，尋找下一個最短時間的點
  * 可能造成starvation
  * ![HDD_SSTF](https://github.com/a13140120a/Operation_System/blob/main/imgs/HDD_SSTF.PNG)
* SCAN Scheduling
  * 假設起始地點為53，則順序53、37、14、65、67、98、122、124、183
  * 朝向某一個方向移動，直到碰到0或最大值才會迴轉
  * 這種方法不公平，例如要是剛好往右到37的時候突然出現36的request，那他回來要等很久才會被serve
  * ![HDD_SCAN](https://github.com/a13140120a/Operation_System/blob/main/imgs/HDD_SCAN.PNG)
* C-SCAN Scheduling
  * 假設起始地點為53，則順序53、65、67、98、122、124、183、199、0、14、37
  * 往同一個方向scan，到底了之後就從頭繼續往同一個方向scan
  * 這種方法很公平，因為最多每個request只會等一圈
  * ![HDD_CSCAN](https://github.com/a13140120a/Operation_System/blob/main/imgs/HDD_CSCAN.PNG)
* C-LOOK Scheduling
  * 假設起始地點為53，則順序53、65、67、98、122、124、183、199、14、37
  * 往同一個方向scan，到queue裡面的最後外面的request(不會碰到底)之後就從最裡面的request(不會回到0)繼續往同一個方向scan
  * C-SCAN Scheduling的優化
  * ![HDD_CLOOK](https://github.com/a13140120a/Operation_System/blob/main/imgs/HDD_CLOOK.PNG)

<h2 id="0107">ECC</h2> 

* [詳情請見](https://github.com/a13140120a/Computer_Organization_And_Architecture/blob/main/README.md#error-detecting-and-error-correcting)

<h2 id="0108">Storage Device Management</h2> 


* Drive Formatting, Partitions, and Volumes
  * 低階初始化(low-level formatting或physical formatting)：
    * HDDS和NVMs都必須先經過raw formatting，
    * raw formatting通常會在生產的工廠裡面就已經完成，
    * raw formatting會把磁軌劃分成多個磁區，並且會將一種特殊的資料結構寫入裝置，而這個資料結構通常會包含標頭(header)、資料區域(data area)和標尾(trailer)所組成，標頭和標尾包含磁區號碼和糾錯碼(ECC)。
    * 通常data area的大小會是512bytes 或 4KB，較大的sector會有較少的header和trailer資料，意味著較多的使用空間。
    * NVMs 會在在raw formatting的階段會初始化pages並建立FTL。
  * 在磁碟可以用來保存資料之前，作業系統仍然需要記錄自己在裝置上的資料結構，OS使用三個步驟來處理:
    * 第一個步驟是partition(分割)：
      * 將磁碟切成多個partition，即使要將該disk用作單個大partition也必須完成此步驟，那麼這樣就可以將partition information會寫在disk上的固定位置(例如通常partition table會寫在disk的開頭)。
      * 一個partition只能存在於一個disk上，無法跨disk。
      * 基礎的檔案系統管理中，通常一個 partition 只能作為一個 filesystem。但實際上，我們可以透過 RAID 的技術以及 LVM 的技術將不同的 partition/disk 整合成為一個大的檔案系統，而這些檔案系統又能具有硬體容錯的功能在。
      * Linux 可以使用[fdisk](https://blog.gtwang.org/linux/linux-add-format-mount-harddisk/)來管理partition，並且當partition被作業系統認到以後，會幫這個partition create 一個device entry(可以在/dev裡面看到，並且device entry（例如 [/etc/fstab(其實就是filesystem table)](https://codertw.com/%E7%A8%8B%E5%BC%8F%E8%AA%9E%E8%A8%80/483041/)）告訴OS將包含file system的每個partition掛載到的位置以及資訊。
)。
    * 第二個步驟是volume(卷)的建立與管理：
      * 有時候這個步驟是implicit的(意思就是例如當file system放進partition裡面的時候，這個volume就已經建立好了)
      * 在其他時候個步驟也可以是explicit的(例如自己手動建立 volume)。
      * volume(或logical drive)是一個具有單一file system的一個可訪問存儲區域，並且一個volume可以包含多個partition
      * volume代表一個命名的存儲區域(named area of storage)，使user和應用程式能夠訪問底層設備上的數據，每個volume都配置有特定的file system(例如NTFS)，並且還分配有一個系統唯一的名稱或編號來標識該volume。
      * 為了使計算機能夠訪問物理卷(physical volume)上的數據，OS必須首先掛載(mounting)該卷，然後，計算機才能夠讀取或修改底層磁盤上的數據
      * 術語partition和volume經常相互混淆，這是因為OS及其文檔經常互換使用這些術語，但是，volume和partition通常被認為是管理存儲的不同方法。
      * volume又分成physical volume與logical volume。
      * Linux的[lvm2](https://sc8log.blogspot.com/2017/03/linux-lvm-lvm.html)以及zfs可以提供管理的功能
      * [詳細資料](https://www.cnblogs.com/lijiaman/p/12885649.html)
    * 第三個步驟是logical formating(邏輯格式化)：
      * 或稱為建立一個檔案系統
      * 在這個步驟中，作業系統將初始化用的檔案系統的資料結構儲存到disk當中，而這些資料結構可能包含未使用和已配置空間的配置圖，以及一個起始的空目錄。
  * partition information還記載著那些partition包含了bootable file system(或OS啟動程式碼)，Linux中被標記為boot的partition會用來被建立檔案系統的根目錄。
  * 通常在一般電腦中，檔案系統由所有已掛載的volume所組成，例如windows 的C槽、D槽、E槽等等(C,D,E槽可以在同一個partition裡面)，Linux在開機時，會先掛載boot file system，才會掛載其他檔案系統。
  * 可以由windows的disk management(磁碟管理)中看到目前disk 的partitio狀況。
  * 為了提高效率，大多數檔案系統將chunk組合成更大的chunk，通常稱為cluster，設備I/O是通過chunk完成的，而檔案系統I/O是通過cluster完成的，有效地確保了I/O具有更多的sequential和更少的random access特徵，例如檔案系統會把檔案的內容放在檔案的metadata附近，以減少seek time。
  * 一些作業系統為一些特殊程式(例如資料庫)提供使用partition的能力(不需要檔案系統的data structures)，這些partition稱為raw disk，而對他的IO則稱為raw IO，跳過了了file system的緩衝區(buffer and cache)、檔案鎖定(file locking)、預取(prefetching)、空間分配(space allocation)、文件名(file name)和目錄(directories)等等功能，也代表programmer要自己處理這些問題。
  * Linux不支援raw IO，但可以藉由在`open()`system call使用`DIRECT`這個flag來達到類似的access。

* Boot Block
  * 當打開電源或重新啟動時，為了使電腦開始運作，必須有一個起始程式才能開始運作，而這個起始程式就叫做bootstrap program(靴帶式程式，或稱靴帶式載入器bootstrap loader)，通常會儲存在主機板的NVM快閃記憶體當中，並映射到已知的記憶體中位置(代表有可能被病毒感染)。
  * 有些電腦系統，把這個步驟分成兩個階段，一個非常簡單的bootstrap loader(存在motherboard的快閃記憶體裡)，會引導controler去讀取disk的boot block(此時還未載入任何驅動程式)，並載入一個更複雜的bootstrap program，然後再由後者載入核心，這個更複雜的載入程式式儲存在一個稱為boot block(啟動區塊)的partition當中，且通常會在disk上的固定位置。
  * 以windows為例子，windows會把其中一個partition標記為boot partition，(擁有boot partition的硬碟又稱為開機碟，boot disk or system disk)，這個partition包含了整個OS kernel跟驅動程式。
  * [MBR](https://topalan.pixnet.net/blog/post/24292326)放在硬碟機最開頭的第一個磁區，位於硬碟第0面，第0軌，第1磁區的位置，包含了啟動程式碼，並且還包含了一個硬碟分割區表，以及一個指定由哪個partition啟動的旗標(也就是指向boot partition的意思)。
  * 一旦系統從MBR中找到了boot partition，就會從該partition讀取第一個sector(又稱boot sector)，並載入kernel，然後載入各種subsystem以及service。
  * ![booting_from_disk](https://github.com/a13140120a/Operation_System/blob/main/imgs/booting_from_disk.jpg)
  * Linux 的default bootstrap loader是grub2。

* bad block:
  * 毀損區塊
  * 大部分硬碟從工廠出來的時候就會有一些bad block了。
  * 在較舊的disk當中(例如IDE)，bad block是由人工處理的(例如linux的badblocks)，其中一種方式是在磁碟格式化的時候掃描硬碟，以便發現bad block，然後把這些bad block mark起來，並且不會再使用這些block
  * 較複雜的硬碟(例如SCSI disk)在整個使用壽命期間都會不斷地更新資料，且他的low-level formatting也設置一些額外的磁碟備份(sector sparing)，或磁區轉換(sector forwarding)的機制。
    * 典型的磁區轉換: 當作業系統試圖讀取某塊資料，controler計算ECC值之後發現該磁區損毀，於是發出錯誤給作業系統，並且將該邏輯位址map到一個備份的磁區，往後存取該邏輯區塊時，都會改map到這個備份磁區，由於該方法可能會導致磁碟排班演算法失效，所以大部分磁碟在格式化之後，在每個磁柱隻中都會提供一些備份磁區，同時最好也提供一個備份磁柱，以減少備份磁區對演算法的影響。
    * 另外一種方法是使用磁區順延(sector slipping)，例如假設邏輯區塊17變成失效，就把原本的18從新label成17，19label成18，以此類推，但可想而知這種方法的cost非常大(需要花很多的時間)。
  * NVM的故障管理區域比HDD更簡單，因為沒有搜尋的順序問題，controler將maintaim一個壞的page的table去追蹤。

<h2 id="0109">Swap-Space Management</h2> 


* 現代系統都是swap 一個page而不是swap整個process
* swap space的設計對於系統swap page的吞吐量影響很大。
* Swap-Space Use:
  * 每個系統對於swap space的空間大小有著不同的看法，例如Solaris建議設置swap space等於許你記憶體超過可分頁實體記憶體的數量，而Linux則建議設置swap space為實體記憶體數量的兩倍，但現在大部分Linux都使用相當少的swap space。
  * 多分配swap space比少分配還要安全，因為少分配很有可能造成系統當機或process被kill掉。
  * 有些作業系統可以有多個swap space(例如Linux)，把swap space放在不同的儲存裝置上，可以分散IO造成的負擔(平行存取)，增加IO的頻寬。
* Swap-Space Location
  * swap space可以放在正常的檔案系統當中，也可以放在獨立的raw partition(原始磁碟分割)當中。
  * 放在檔案系統當中的話swap就是一個大型檔案。
  * 放在raw partition的效能會比放在檔案系統當中還要好。
  * 但放在檔案系統就會比較方便(不用管理)。
  * 有些作業系統比較有彈性，可以同時放raw partition以及file system當中，例如Linux允許管理員要使用哪一種形式
* An Example
  * 在下面顯示的 Linux 系統映射系統中，swap space的映射保存在memory中，其中每個條目對應於swap space中的 4KB的block，零表示空閒槽，非零表示有多少process map到該block（>1 僅用於共享頁面。）
  * ![LinuxSwapping](https://github.com/a13140120a/Operation_System/blob/main/imgs/LinuxSwapping.jpg)

<h2 id="0109">Storage Attachment</h2> 

* 主機附加儲存(host-attahed storage): "Host-Attached" 是指經由本地的IO port來進行傳輸，這些port使用多種技術，最常見的為SATA。
  * 為了使系統能有更多的儲存空間，可以透過多個USBFireWire或者Thunderbolt來連接多個儲存裝置。
  * 比較高級的工作站和server一般需要更多存儲空間或需要共享存儲，因此使用更複雜的I/O架構，例如光纖通道 (FC)
* NAS(network-attached storage)：[wiki](https://zh.wikipedia.org/wiki/%E7%BD%91%E7%BB%9C%E9%99%84%E6%8E%A5%E5%AD%98%E5%82%A8)
  * NAS是將儲存裝置透過檔案伺服器，與區域網路連接的儲存系統，只要能連上區域網路（LAN），便能存取儲存硬碟裡的資料。有了專門用於資料存取的「檔案伺服器」，企業內部的其他伺服器可以發揮正常的處理效率，大幅提升整體效能。相較於DAS，NAS的建置費用較高，但卻能滿足中大型企業存取大量文件資料的需求。
  * NAS可以是special-purpose 的儲存系統，也可以是為網絡上的其他主機提供存儲的通用計算機系統。
  * 客戶端通過RPC interface(例如UNIX和Linux系統的NFS或Windows的CIFS)來access NAS，該RPC通常是通過IP網絡上的TCP或UDP傳輸的(通常是同一個區網(LAN))，它將所有數據流量傳輸到客戶端。
* SAN(storage area network): [wiki](https://zh.wikipedia.org/wiki/%E5%AD%98%E5%82%A8%E5%8C%BA%E5%9F%9F%E7%BD%91%E7%BB%9C)
  * SAN是將儲存裝置從區域網路中獨立出來，成為另一個網路，並透過專屬的高速網路／光纖通道（FC）來連接各個伺服器與儲存系統。SAN可以帶來超高效的資料存取服務，其建置費用高且費時，須加裝光纖纜線、路由器、主匯流排介面卡（HBA）等裝置，適合業務上操作運算資料量大的企業。
  * SAN使用存儲協議而不是網絡協議。
  * SAN 的強大之處在於其靈活性，多個host和多個存儲陣列可以連接到同一個 SAN，並且存儲可以動態分配給host，例如如果host的磁盤空間不足，則可以將SAN 配置為為該host分配更多存儲空間，SAN使服務器集群可以共享相同的存儲，並使存儲陣列可以包括多個直接主機連接
  * SAN 通常比存儲陣列擁有更多port，而且成本更高。 SAN連接距離很短，通常沒有路由，因此NAS可以擁有比SAN更多的連接主機。 
  * 存儲陣列是特製的device，它包含存儲數據的硬碟和一個控制器來管理存儲，並允許跨網絡訪問存儲，控制器由 CPU、內存和實現陣列功能的software組成，包括網絡協議、用戶界面、RAID 保護、快照、複製、壓縮、重複數據刪除和加密。
  * 陣列可能僅包含SSD，從而獲得最佳性能但容量較小，或者可能包含 SSD 和 HDD 的混合(通常會使用SSD當作快取)。

* [詳細](https://www.itread01.com/content/1545690255.html)
* [ZFS](https://zh.wikipedia.org/wiki/ZFS)

<h2 id="01010">RAID Structure</h2> 

* redundant arrays of independent disks
* 將許多硬碟連接到計算機系統是較有經濟效益的(比起購買更大的昂貴的disk)。
* 如果硬碟以並行方式運行，則在系統中擁有大量硬碟可以提高讀取或寫入數據的速率。
* 此外，因為冗餘信息(redundant information)可以存儲在多個驅動器上，因此這種設置提供了提高了reliable(數據回復)的能力，我們存儲了通常不需要的額外信息，但可以在磁盤故障時用於重建丟失的信息。
* mirroring(鏡射)：複製每個磁碟，例如一台邏輯磁碟機若是由兩台時體磁碟機組成，而且每次寫入都會在兩台磁碟機上一起執行(寫入重複的資料)，這個結果稱為鏡像卷區(mirrored volume)，如果其中一台磁碟機壞了，可以從另一台磁碟機讀取資料，只有當兩台同時壞掉時，資料才會遺失。
* striping(條帶化)：條帶化技術就是一種自動的將 I/O 的負載均衡到多個物理磁盤上的技術，條帶化技術就是將一塊連續的數據分成很多小部分並把他們分別存儲到不同磁盤上去。例如，假設有多個disk，我們可以藉由在多顆disk間分割資料儲存來增進效能，例如將一個byte分散到八顆disk上，每顆disk都參與每次存取，因此速度便為八倍，又分成bit-level以及block-level
  * bit-level striping：將每個 byte 的bit拆分到多個disk上，幾乎不會使用
  * block-level striping：以block為單位，將每個file拆分成多個block，最常使用的striping
* mirroring提供了高可靠性，但價格昂貴，striping 提供了高數據傳輸率，但並沒有提高可靠性。
* RAID 的各種級別決定了reliable 以及速度的權衡:
  * RAID 0：容量最大，因為不用寫[parity](https://github.com/a13140120a/Computer_Organization_And_Architecture/blob/main/README.md#error-detecting-and-error-correcting)資料，也因此不用多餘的計算，因此速度也最快。
  * RAID 1：至少要兩顆disk，可以容許最多顆disk的損壞t(50%)，但同時也是最昂貴(需要最多顆disk可以儲存最少容量)。
  * RAID 3：會有一顆disk儲存parity資料，並將parity資料放在同一顆disk上(稱為同位碟)，以byte為單位做striping以及mirroring。
  * RAID 4：基本上跟RAID 3一樣，但RAID 4以block做striping以及mirroring，RAID 3與RAID 4的同位碟通常會是效能上的瓶頸，因為很多顆disk一起寫入代表很多顆disk一起製造parity並且寫入到同一顆disk
  * RAID 5：最少要三顆disk，與RAID 3、RAID 4不同的是，parity會分散寫入每顆disk，這項技術突破了RAID 3、RAID 4的瓶頸，RAID 5只能容忍一顆disk壞掉。
  * RAID 6：每顆硬碟會有兩個redundant，與RAID5 使用XOR來correcting的方式不同，RAID 6通常不使用同位位元，而使用一些複雜的數學技巧來計算Q(例如[Galois field](https://blogs.oracle.com/solaris/post/understanding-raid-6-with-junior-high-math))，RAID 6可以容忍兩顆disk損壞。
  * RAID 10：RAID 10是先分割資料再鏡像，再將所有硬碟分為兩組，視為以RAID 1作為最低組合，然後將每組RAID 1視為一個「硬碟」組合為RAID 0運作，當RAID 10有一個硬碟受損，其餘硬碟會繼續運作，RAID 10遠較RAID 01常用。
    * ![RAID_10](https://github.com/a13140120a/Operation_System/blob/main/imgs/RAID_10.png)
  * RAID 01：跟RAID 10的程式相反，是先鏡像再將資料到分割兩組硬碟。它將所有的硬碟分為兩組，每組各自構成為RAID 0作為最低組合，而將兩組硬碟組合為RAID 1運作，RAID 01只要有一個硬碟受損，同組RAID 0的所有硬碟都會停止運作，只剩下其他組的硬碟運作，可靠性較低。如果以六個硬碟建RAID 01，鏡像再用三個建RAID 0，那麼壞一個硬碟便會有三個硬碟離線，
    * ![RAID_01](https://github.com/a13140120a/Operation_System/blob/main/imgs/RAID_01.png)
  * JBOD(Just a Bunch Of Disks)：它的功能就跟它的全名一樣，「只是將多顆磁碟湊在一起」，當作一顆超大硬碟來用，並沒有striping或mirroring。
  * ![raid_Levels](https://github.com/a13140120a/Operation_System/blob/main/imgs/raid_Levels.jpg)

* implementation:
  * Volume-management的software可以於kernel層或系統軟體層 implement RAID，這種情況下hardware可以提供較少的功能。
  * 或者是RAID 可以在host bus-adapter(HBA)中implement，速度較快但會缺乏彈性。
  * 或者是在可以在存儲陣列的硬體(例如controler、hardware)中實現。
* Other features:
  * snapshot(快照)：上次更新前檔案系統的狀況記錄
  * Replication(複製)：不同站點之間自動複製寫入以實現冗餘和災難恢復。複製可以是同步的或異步的，在同步複製中，寫入local的block同時也必須寫入remote端，而在異步複製中，寫入內容被聚集在一起並定期寫入(譬如每天的幾點等等)。
  * hot spare drive(熱備援磁碟機)：RAID 中如果有disk壞掉，hot spare disk可以自動的重建壞掉的disk，不需等到人工替換的時候才做替換。
* Problem:
  * 雖然有如此多的保護措施，但RAID還是不保證資料永遠不會損毀，例如指向文件的poiter可能是錯誤的，或者file structure中的pointer可能是錯誤的，這會導致寫入時寫到不正確的位置，進而導致檔案損毀。
  * 或者是有一些process意外的overwrite了檔案系統的結構
  * RAID 的controller故障或是軟體上的bug都會導致資料損壞。
* ZFS：
  * Solaris ZFS檔案系統採用一種創新的方法來解決這些問題，而這種方法就是checksum。
  * ZFS的block不跟其checksum放在一起。
  * ZFS 的checksum 是跟指向block的pointer放在一起的。
  * ![ZFS_Checksums](https://github.com/a13140120a/Operation_System/blob/main/imgs/ZFS_Checksums.jpg)
  * inode 是一種儲存file system 的metadata的structure。
  * 如過資料被mirroring，並且存在一個帶有正確的checksum的block以及一個帶有錯誤checksum的block，ZFS會自動用好的block更新壞掉的block。
  * 相同地，指向inode的directory entry也會具有該inode的checksum，當訪問directory的時候ZFS會檢測到inode中的任何問題。
  * checksum計算和額外的block讀取修改寫入周期所產生的overhead並不明顯，因為 ZFS 的整體性能非常快。
  * [詳細資料](https://btrfs.wiki.kernel.org/index.php/Btrfs_design)
  * 然而，RAID的缺點是缺乏靈活性，假設有一個由二十個disk組成的存儲陣列，這些disk分為四組，每組五個驅動器(也就是有四個獨立的volume)，可能有些file system，無法放進其中一個volume，或者有一些只需要用到非常小的空間，那就會浪費volume裡面的空間。
  * 即使使用所有20個disk作為一個大型RAID時，可能會出現其他問題，例如，雖然我們在上面建裡多個不同大小的volume，然而，驅動器的使用和需求經常會隨著時間而變化，而且某些卷管理器不允許我們更改卷的大小，就算使用可以更改大小的卷管理器，但一些file system不允許空間增長或收縮。
  * 為了解決上述問題，ZFS 將檔案系統管理和卷管理結合，disk或partition通過多個RAID組合成一個儲存池(pool)(即多個disk或partition組合成RAID，多個RAID再組成成pool)，一個pool可以容納一個或多個 ZFS file system，整個pool的可用空間可供該pool中的所有file system使用，ZFS把記憶體管理的`malloc()` 和 `free()`的那套方法放在儲存空間的管理上，因此，無需在卷之間重新定位文件系統或調整捲大小，消除了上述的儲存限制，並且也提供管理者可以限制(控管)能夠增長的量，且隨時可以改變這些變量。
  * ![ZFS_Pools](https://github.com/a13140120a/Operation_System/blob/main/imgs/ZFS_Pools.jpg)
* 其他系統（如 Linux）具有捲管理器，允許邏輯上連接多個disk以創建大於disk的volume以容納大型檔案系統。


<h2 id="01011">Object Storage</h2> 

* 一般計算機系統使用file systems來儲存使用者資料，另一種方法是建立一個儲存池(storage pool)，並將所謂的"物件"(object)放進儲存池裡面。
* 與一般檔案系統不同的是，object無法在pool裡面被navigate(尋找)。
* object storage是computer-oriented而不是user-oriented，是專門設計來給program使用的。
* 一般的順序為:
  1. 在存儲池中創建一個object，並接收一個object ID。
  2. 在需要時通過object ID 訪問object。
  3. 通過object ID 刪除對象。 
* Object storage management software(例如Hadoop file system(HDFS)和Ceph)會決定要把object儲存在哪裡，並且提供對object的管理以及保護措施。
* 舉例來說，例如，HDFS可以在 N 台不同的計算機上存儲一個object的 N 個copies，這種方法的成本甚至可以低於存儲陣列。
* Hadoop cluster中的所有系統都可以訪問該對象，但擁有copy的系統會有較快的access速度，而其他系統需要網絡連接來讀取和寫入object(較慢)。因此，Object Storage通常用於大容量存儲，而不是高速random access。
* object storage具有高scalability，我們隨時都可以添加更多的系統(computer)進去。
* 每個object都具備self-describing(自我描述)的功能，包括對該object的內容的描述，內容沒有固定的格式，所以系統存儲的是非結構化數據。 
* Dropbox、Spotify、以及Facebook的相片都是使用object storage來保存資料、Amazon S3使object storage來保存檔案系統以及在雲端計算機上運行的客戶應用程式的data object。 

****




<h1 id="011">I/O Systems</h1> 

  * ## [Overview](#0111) #
  * ## [I/O Hardware](#0112) #
  * ## [Direct Memory Access](#0113) #
  * ## [Application I/O Interface](#0114) #
  * ## [Kernel I/O Subsystem](#0115) #
  * ## [Transforming I/O Requests to Hardware Operations](#0116) #
  * ## [STREAMS](#0117) #
  * ## [Performance](#0118) #




<h2 id="0111">Overview</h2> 

* 計算機的兩個主要工作是I/O和計算。在很多情況下，主要的工作是I/O，計算或處理只是其次。
* 由於 I/O 設備的功能和速度差異很大（鼠標、HDD、SSD等等），因此需要不同的方法來控制它們。
* 這些方法構成了內核的 I/O 子系統，它將內核的其餘部分與管理 I/O 設備的複雜性分開。 
* IO devices正在往兩個衝突的方向發展，一個是軟體與硬體的介面逐漸趨於標準化，另一個則是一些更新技術的IO devcie出現，而這些新的IO device與就的device的差異極大，以至於將它們整合到既存的系統變的很有挑戰。
* device-driver：為了封裝不同devices 之間的細節以及差異處，OS被設計為使用 device-driver modules(驅動程式模組)來為I/O subsystem提供統一的device access的interface。


<h2 id="0112">I/O Hardware</h2> 

* port：host 跟IO devices之間的連接點稱為port
* bus(匯流排)：是一組線路和一個嚴格定義的協議(protocal)，該協議定義了一組可以在線路上發送的消息。
* 下圖為典型的PCI匯流排，PCI匯流排負責處理較快速的devices，而expansion bus(擴充匯流排)則處理相對較慢的devices(例如鍵盤，serial port，USB port等等)。
* ![Typical_PCIe_Bus](https://github.com/a13140120a/Operation_System/blob/main/imgs/Typical_PCIe_Bus.jpg)
* 下圖可以看到北橋晶片(Northbridge)控制著速度較快的bus(例如PCIe bus)，而南橋晶片(Southbridge)則控制相對較慢的bus(例如一些expansion bus)。
* PCIe：
  * 是一種具有彈性的匯流排，可經由一個或多個lanes(通道)傳輸資料，一個lane由兩個signaling pairs組成，一個pair用於接收資料，另一個用於傳送，每個lane由四條線組成，每個lane都使用**全雙工位元組串流**(full-duplex bytes stream)，可以同時在雙向以八位元的位元組格式傳送封包。
  * 從物理上講，PCIe線路可以包含 1、2、4、8、12、16 或 32 個通道，由“x”前綴表示，例如，使用8個通道的PCIe 介面卡或連接器被指定為x8，此外，PCIe 已經經歷了多個“世代”，未來還會有更多。因此，假如一張卡可能是“PCIe gen3 x8”，這意味著它可以與第三代PCIe 配合使用並使用 8 個通道。(詳情[https://pcisig.com/](https://pcisig.com/))
* contrler(控制器)：可以操控port,bus,device的電子零件組合，通常會有獨立的晶片，以及一些memory。
  * serial-port controler(序列埠控制器)：一個簡單的控制器，他是電腦中的獨立晶片，控制在序列埠纜線上的訊號。
  * fibr channel (FC) bus controller(光纖通道控制器)：相較序列埠控制器之下較複雜，包含了microcode，processor，memory，能夠處理FC protocol的messages
  * 硬碟的背面會有一張電路板，這張電路板即為disk controller。

* Memory-Mapped I/O:
  * comtroler通常會有一個或多個用於數據和控制信號的暫存器，processor通過讀取和寫入這些暫存器中的bit patterns與控制器通信。
  * Port-mapped I/O：屬於最傳統的方法，使用特殊的I/O instructions(而非memory的instructions)來直接與I/O port溝通，，例如X86的IN, OUT，這些指令指定將bytes(或word)傳輸到I/O port address，而這種instructions通常都是Privileged Instructions(使用者無法直接操作，一定要透過OS)。
  * 下圖為PC 的常用 I/O port address。
  * ![IO_Ports](https://github.com/a13140120a/Operation_System/blob/main/imgs/IO_Ports.jpg)
  * Memory-Mapped I/O：controler的暫存器被map到一個memory的地址空間，process通過把資料寫入memory-mapped 的記憶體空間(region)來操作IO device，透過操作這個被map的記憶體空間來間接的操控IO devices，這種方法一次可以寫入較多的數據，例如，process 通過將資料寫入一個"map到圖形控制器(graphics controller)"的記憶體空間來顯示圖形在螢幕上，將數百萬bytes寫入圖形內存比發出數百萬條I/O指令要快，現今大多數IO devices都是使用這種IO方式。
  * I/O設備控制通常由四個暫存器組成：
    * data-in register(資料輸入暫存器)：device到host，host由data-in register讀區資料。
    * data-out register(資料輸出暫存器)：host到device，由host寫入要輸出的資料。
    * status regiter：包含了可以被host讀取的位元資料，這些位元包含「目前的指令使否完成」、「在data-in register的資料是否可以讀取」、以及「是否有裝置錯誤發生」等等。
    * control register：可由host寫入要執行的指令，或更改device moed。例如在serial port controler中的某個位元可以選擇全雙工或者半雙工通信，某個位元則可以啟動同位元檢查等等。
  * 資料暫存器通常為1至4個bytes，某些controler擁有FIFO晶片。

* Polling：
  * 使用busy waiting的方式
  * 基本的handshaking(握手)概念:
    * 假設使用兩個bit(status register裡面的busy位元以及command register的command-ready位元)
    1. host將重複讀取busy位元，直到busy被設為0
    2. host設定在command register中的write位元，並將資料寫入data-out register
    3. host設定command-ready的位元
    4. 當controler發現command-ready位元被設定完成，即立刻設定busy位元為1
    5. controler讀取指令，並且執行
    6. controler清除command-ready位元，並清除再status resigter中的error位元，表示IO已經成功，並清除busy位元。
  * 如上述，於步驟1當中，host就處於busy waiting(或polling)的狀態。
  * 如果等待的時間夠小，使用polling是較好的選擇
  * polling適合programmed IO
* Interrupt：
  * 如[之前提到](#0013)
  * 由於在許多情況下中斷處理受時間和資源限制，因此實作起來很複雜，系統經常將中斷管理分為第一級中斷處理程序（FLIH, first-leve interrupt handler）和第二級中斷處理程序（SLIH, second-level interrupt handler），FLIH 執行上下文切換、狀態存儲和處理操作的排隊柱列，而SLIH則執行request operation的處理 。
  * interrupt的技術也應用在許多OS的kernel，例如製作virtual memory的page fault、system call等等
  * interrupt也可以用來控制kernel的流程(control flow)，例如有一user process要讀取硬碟資料，其中的一個步驟是將從硬碟讀取並存在kernel buffer的資料copy到user program的space，另一個步驟則是啟動IO queue裡面的下一個IO request，我們可以藉由調整interrupt的piority來讓後者先執行，待另一個IO request開始執行IO之後再執行前者(把資料copy到user program space)。
  * interrupt技術可以允許devices通知cpu，如此就不需要為了完成IO動作而做多餘的等待
  * 但是會消耗context switch的時間，interrupt適合memory-mapped I/O
  * [補充](https://cihcih.medium.com/%E4%BD%9C%E6%A5%AD%E7%B3%BB%E7%B5%B1-interrupt-d2fced694da5)
  
<h2 id="0113">Direct Memory Access</h2> 

* programmed IO(PIO)：使用昂貴的general-purpose processor來監控資料傳輸的狀態，以及一次一個(或少許) byte傳送資料，非常浪費cpu資源。
* 為了改善上述那種狀況，於是有了DMA的技術，DMA是一種special-purpose的controler，當初始化「資料傳輸」的動作時，CPU會把一個「DMA command block(DMA指令區塊)」寫進記憶體，這個block可能包含了傳輸目的及來源的pointer，還有傳輸的資料長度，也可能包含了更複雜的資料(例如不連續的來源及目的address的列表)
* 這種方法又稱為scatter–gather(分散收集)的方法，這種方法允許通過單個DMA 指令執行多個傳輸，cpu只要把DMA command block的位址寫進去DMA controler裡面，cpu就可以去做其他的事情了，待DMA處理完IO之後會發出interrupt，然後cpu再接手處理。
* 大部分電腦(包含行動裝置)都會有一個簡單的DMA
* 為了防止user program修改存放DMA command那塊memory的內容，一定是存在kernel space，因為user不能去修改它。
* DMA執行IO時，會占用系統匯流排，此時可會影響CPU的效能，但根據實驗證明，整體效能還是比較佳的。
* VDMA(virtual direct memory access)技術可以讓DMA直接使用虛擬記憶體的位址。


<h2 id="0114">Application I/O Interface</h2> 

* 各種不同的hardeare的差異及細節被封裝在device driver，下圖說明kernel中與I/O相關的部分是如何在軟件層中構建的 
* ![Kernel_IO_Structure](https://github.com/a13140120a/Operation_System/blob/main/imgs/Kernel_IO_Structure.jpg)
* 我們必須找到devices的各個特徵，才能抽離IO的細節與差異，大多是device的特徵可以是以下：
  * Character-stream or block： 
    * 資料傳輸可以是以byte為單位，也可以是以block為單位
    * block-device interface：
      * 包括所有存取disk，以及block-oriented裝置時所需的功能，
      * 像`read()`、`write()`、`seek()`這種指令就已經掌握住區塊儲存裝置的核心特徵，因此應用程式並不需要知道low-level的細節是如何實現的。
      * raw IO(原始IO)：原始 I/O 直接發送到disk偏移量，完全繞過檔案系統。它已被某些應用程式（尤其是數據庫）使用，這些應用程序可以比檔案系統更好地管理buffer以及lock。缺點是軟件更複雜。根據 Oracle 的官方網站，原始I/O比對帶有檔案系統的 I/O性能提高了大約 5% 到 10%。
      * direct IO：屬於「完全繞過作業系統」、以及「完全使用作業系統」的折衷方案，讓使用者使用檔案系統，但繞過buffer和上鎖。
    * character-stream interface：
      * 對於鍵盤、滑鼠、數據機等等非常方便。
      * system call `get()`、`put()`等等，一次存取一個字元。
  * Sequential or random access： 序列化或隨機
  * Synchronous or asynchronous：
    * 同步設備與系統的其他方面協調，以可預測的response time執行數據傳輸。 異步設備表現出與其他計算機事件不規則或不可預測的response time。 
  * Sharable or dedicated(共用或指定)：sharable devices可以被多個thread或processor共用，dedicated devices則不行
  * Speed of operation：不同的devices之間有著不同的速度差異
  * Read–write, read only, write once：例如螢幕是write only，disk可以read-write。
  * ![DeviceCharacteristics](https://github.com/a13140120a/Operation_System/blob/main/imgs/DeviceCharacteristics.jpg)
* Back-door interfaces：大部分的會有escape(或稱back door)可以直接將某process的指令原封不動地傳給驅動程式，這種interface通常會有風險，因為可以直接通過許多安全機制的檢查。
  * 例如UNIX的`ioctl()`，這個system call讓應用程式可以直接存取任何驅動程式上的功能，`ioctl()`共有三個參數，第一個是裝置識別碼(device id)，它透過device將應用程式與驅動程式連接，第二個參數是一個整數，它可以選擇此裝置的指令，第三個參數是一個指標，可以指向記憶體中的任何資料結構，讓應用程式及驅動程式傳達信息(參數)。
* UNIX和Linux 中的device id分為"major"以及"minor"，第一個是裝置類型，第二個是裝置的instance，例如系統中有4顆SSD以下command，8代表是SSD，0123代表第幾顆SSD：
```
% ls -l /dev/sda*
[output:]
brw-rw---- 1 root disk 8, 0 Mar 16 09:18 /dev/sda
brw-rw---- 1 root disk 8, 1 Mar 16 09:18 /dev/sda1
brw-rw---- 1 root disk 8, 2 Mar 16 09:18 /dev/sda2
brw-rw---- 1 root disk 8, 3 Mar 16 09:18 /dev/sda3
```

* Network Devices：
  * 由於網絡 I/O 的性能和尋址特性與disk I/O有很大的不同，除了提供跟disk一樣的`read()`、`write()`、`seek()`以外，
  * 還有socket interface，顧名思義，就像插座一樣，program使用socket相關的system call來create 一個socket，那麼遠端的program就可以插入這個socket(插座)，反之亦然，透過連接發送和接收數據封包。
  * socket還提供了`select()`函數來管理一組socket，`select()`返回「有關哪些socket有正在等待接收的封包」以及「哪些socket有空間可以接受要發送的數據包」的信息。
  * `select()`的使用消除了網絡 I/O 所必需的polling和busy waiting。這些函數封裝了網絡的基本行為，極大地促進了可以使用任何底層網絡硬體和protocal stack的分散式應用程式的創建。
* Clocks and Timers
  * [Timer基本介紹](#0014)
  * 大多數計算機都有硬體時鐘和計時器，它們提供三種基本的功能：
    * 給出當前時間。
    * 給出經過的時間。
    * 設置定時器以在時間 T 觸發操作 X 
  * 這些函數被作業系統以及對時間敏感(time sensitive)的應用程序大量使用。 不幸的是，實現這些功能在各個作業系統之間沒有一個固定的標準。
  * 測量經過時間以及觸發操作的硬體又稱為programmable interval timer(PIT, 可編程間隔計時器)，該硬體可以設置為等待一定時間後產生中斷，也可以設置為執行一次或重複該過程產生週期性中斷。
  * scheduler 使用這種技術來觸發interrupt，讓OS可以multiprogramming 或Time-sharing。
  * Disk 的IO system使用這種技術來每隔一段時間把buffer的內容flush到disk內，網路subsystem使用它來取消由於擁塞或故障而進行得太慢的操作。
  * OS還可以為process提供使用Timer的interface。
  * 可以透過virtual clocks模擬比實際存在更多的定時器，為此，kernel（或計時器的device driver）維護其自己的routine和user requests所需的interrupt list，按最早時間優先順序排序(earliest-time-first order)，它將計時器設置為列表內最早的(所需要的interrupt)時間。當定時器中斷時，kernel向請求者發出信號並用下一個最早的時間重新加載定時器(重新開始計時)。 
  * 現代 PC 包括一個高性能事件計時器 (high-performance event timer，HPET)。
  * NTP(the network time protocol)：可以糾正有偏移的時間的協議。

* Blocking(阻塞) and Non-blocking(非阻塞) I/O：
  * IO devices通常是asynchronous的，因為他們需要的時間不一定，而且通常很久，但許多OS還是有提供Blocking的
  * Blocking：使用阻塞 I/O 時，當發出 I/O 請求時，進程將移至等待隊列，並在請求完成時移回就緒隊列，同時允許其他進程運行。
  * non-Blocking：對於非阻塞 I/O，無論請求的 I/O 操作是否（完全）發生，I/O 請求都會立即返回。
  * 應用程序編寫者可以與 I/O 重疊執行的方法有三種：
    * 使用multithread
    * 使用non blocking的system call
    * 使用asynchronous的system call
  * asynchronous與non-blocking的差異在於：
    * asynchronous：會在執行完之後透過variable、interrupt、或者[callback function](https://hackmd.io/@Heidi-Liu/note-javascript-callback)回傳。
    * non-blocking：會立刻回傳一些當下可用的值，譬如有沒有成功(回傳1或0)，或者回傳資料是否已全部傳達，部份傳達，或者尚未傳達等等
  * 同步與異步比較圖
  * ![sync_async_IO_Methods](https://github.com/a13140120a/Operation_System/blob/main/imgs/sync_async_IO_Methods.jpg)

* Vectored I/O：
  * 允許一個system call對多個位置進行多個IO操作
  * 例如UNIX的`ready()` system call
  * 多個單獨的緩衝區可以通過一個system call傳輸其內容，從而避免context switch和system call的overhead。 

<h2 id="0115">Kernel I/O Subsystem</h2> 

* I/O Scheduling：
  * 好的Scheduling I/O可以大大提高整體效率。priority也可以在request scheduling中發揮作用。
  * [上一章](#0106)的disk scheduling就是一個經典的I/O Scheduling的例子。
  * 當kernel support異步(asynchronous ) I/O 時，它必須能夠同時跟踪許多 I/O request。為此，OS可能會將waiting queue device-status table到設備狀態表(device-status table)。kernel管理這個表，其中包含每個I/O 設備的entry，如下圖所示，每個entry會有device的type、address、state(例如not functioning, idle或 busy)，如果devcie處於busy的狀態，那麼request會被加到queue裡面。
  * ![DeviceStatusTable](https://github.com/a13140120a/Operation_System/blob/main/imgs/DeviceStatusTable.jpg)
  * Buffering and caching可以使scheduling更靈活。

* Buffering：
  * 使用buffer有三個原因：
    * 一個原因是為了應對數據流的生產者(producer)和消費者(consumer)之間的速度不匹配，例如網路的速度大概比硬體慢了一千倍。
    * 第二個原因是為了具有不同數據傳輸大小的device做調整，例如網路封包是藉由分割成多小封包下去傳輸的，使用buffer就可以將多個小封包組合成一個完整的封包。
    * 第三個原因是**"copy semantics"**，假設process有一個希望寫入disk的buffer。它呼叫`write()` system call，提供指向buffer的pointer和指定要寫入的byte數。system call return後，如果process改變了buffer的內容就會造成錯誤，copy semantics就是藉由將控制權還給process之前將process的資料複製到kernel的buffer中。(disk寫入是從kernel buffer執行的，因此對process buffer的後續更改無效。)
    * 因為buffer之間的copy會造成額外的overhead，所以巧妙的運用virtual memory mapping和copy-on-write可以改善效能。
* Caching：
  * Buffering跟Caching的差別在於，buffer可能是系統上現存的唯一一份資料，可是cache是系統上速度更快的其他地方的複製資料
  * buffer不能隨便丟棄，cache可以。

* Spooling and Device Reservation：
  * spooling是關於慢速字元裝置（比如印表機）如何與計算機主機交換資訊的一種技術，通常稱為「假離線技術」。[詳細介紹(註：輸出井 output wells)](https://blog.csdn.net/william_munch/article/details/82865073)

* Error Handling：
  * I/O request失敗的原因有很多，可能是暫時的（緩衝區溢位），也可能是永久性的（硬碟損毀）。
  * I/O request通常會返回一個error bit（或更多）來表示錯誤訊息。 UNIX 系統還將全局變量errno設置了大約一百個明確定義的值，以表示已發生的特定錯誤。（完整列表請參見 errno.h，或 man errno。）
  * 某些device（例如 SCSI device）能夠提供有關錯誤的更詳細信息。

* I/O Protection：
  * privileged instructions(特權指令)：為了防止user執行非法 I/O，因此將所有I/O指令都為privileged instructions，因此，user不能直接發出 I/O 指令； 他們必須通過OS來完成。 
  * 如下圖，為了執行I/O，user program執行system call讓OS幫它執行I/O，OS在kernel mode下執行，檢查request是否有效，如果有效，則執行I/O。 然後返回result給用戶。 
  * ![SystemCall](https://github.com/a13140120a/Operation_System/blob/main/imgs/SystemCall.jpg)
  * memory-mapped region和I/O port的memory locations 必須受到memory管理系統的保護，但不能完全拒絕user process access這些區域。（例如，影片、遊戲和一些其他應用程式需要能夠直接access到 memory-mapped 的graphics controller memory以獲得最佳性能，但memory保護系統會限制access，以便一次只有一個process可以訪問內存的其中一些特定部分，例如部分對應於特定視窗的screen memory。)

* Kernel Data Structures：
  * kernel需要保存有關I/O使用的狀態信息。它通過各種kernel data structure來跟踪網絡連接、設備通信和其他 I/O 活動(例如[File Concept的open-file table](#0121))。 
  * UNIX 對各種entry(實體)的access都提供像是對檔案系統的access(也就是把所有的entry都是為檔案，user可以藉由這些檔案對entry進行操作)，包括user files、raw devices和process address space，而這些entry中的每一個都支持`read()`操作，但每種語法都不盡相同，
    * UNIX要讀取user file之前，kernel需要在決定是否執行硬碟I/O之前檢查buffer，然後需要確保請求大小是硬碟sector大小的倍數，並且在sector邊界上對齊。
    * 要讀取process image，只需從memory中複製資料。
    * UNIX 通過使用物件導向的技術將這些差異封裝在一個統一的structure中，利如下圖是一個open-file record，包含了dispatch table(分派表，調度表?)，表內包含了指向適當的routine的pointer。
    * ![UNIX_IO_struct](https://github.com/a13140120a/Operation_System/blob/main/imgs/UNIX_IO_struct.jpg)
  * Windows 使用message-passing的方式來實現IO：
  * IO request會被轉換成一個message(消息)，通過kernel發送到I/O manager ，然後發送到device driver，每個停留點都可能改變消息內容。
  * 對於output，消息包含要寫入的數據。 對於input，消息包含一個buffer來接收數據。
  * 與使用shared memory的技術相比，message-passing會增加開銷，但它簡化了I/O system的結構和設計並增加了靈活性。 


* Power Management：
  * 在移動裝置中，為了提供令人滿意的電池壽命，現代移動OS從頭開始設計，將電源管理作為關鍵功能，
  * Android 移動系統能夠最大限度地延長電池壽命的三個主要功能：power collapse(電源陷縮)、component-level power management(元件級電源管理)、wakelocks(喚醒鎖)。 
    * power collapse：power collapse使設備進入非常深度睡眠狀態的能力，通過關閉設備中的許多單獨組件（例如螢幕、喇叭和I/O system）來實現的，這樣它們就不會消耗電力，在這樣的狀態下，雖然 CPU 處於空閒狀態，但它可以很快地接收到中斷，喚醒並恢復之前的活動。
    * component-level power management：Android 構建了一個「device tree(裝置樹)」來表示手機的物理設備拓撲(topology)，來表示組件之間的關係，例如，在這樣的拓撲中，快閃記憶體和USB是I/O subsystem和system bus的子節點，借助這些信息，Android 可以管理手機各個組件的電源：如果某個組件未使用，則將其關閉。如果system bus上的所有組件都未使用，則將system bus關閉。如果整個device tree中的所有組件都未使用，系統可能會進入power collapse狀態。 
    * wakelocks：這是一個應用程式暫時防止系統進入power collapse的能力。 考慮使用者在玩遊戲、更新、觀看影片或等待網頁打開，在所有這些情況下，應用程式都需要一種方法來讓設備保持清醒，應用程式根據需要獲取和釋放喚醒鎖。當應用程序持有喚醒鎖時，kernel會阻止系統進入power collapse狀態，任務完成後將釋放喚醒鎖，讓系統進入power collapse狀態。 
  * [advanced configuratio and power interface (ACPI)](http://www.acpi.info)是具有許多功能的業界標準。它提供的代碼作為可被kernel呼叫的routines，用於設備狀態發現(例如熱插拔的時候，系統必須認的到devices)和管理、設備錯誤管理和電源管理。例如，當kernel需要靜止一個device時，它會調用設備的driver，driver再調用ACPI routine，然後與設備對話。 


<h2 id="0116">Transforming I/O Requests to Hardware Operations</h2> 

* 應用程式通過文件名讀取資料。
* DOS 使用冒號分隔符來指定特定設備（例如 C:、LPT: 等），冒號的前面代表"device name space"(裝置名稱空間)，後面則是"file-system name space"(檔案系統名稱空間)。
* UNIX 使用mount table(掛載表)將filename prefixes(前綴)（例如 /usr）map到特定的掛載設備。如果mount table中的多個entry與文件名的不同前綴匹配，則選擇匹配最長前綴的條目。（例如/usr/home與/usr兩者都存在於掛載表中並且都匹配所需的文件，則會選擇/usr/home。）
* 當 UNIX 在檔案系統目錄結構中查找此名稱時，它找到的不是inode編號，而是 <major, minor> 設備編號。major指出可被呼叫來處理此裝置的driver。minor則被傳遞給driver當成device table(設備表)中的索引。相應的device table entry紀載了controler的port address或memory-mapped address。
* 一系列的lookup tables和mapping使不同設備的access變得靈活。
* 某些作業系統具有隨時載入驅動程式的能力，在開機時，系統會先檢查硬體匯流排，來看看目前有哪裝置，接下來載入驅動程式，載入的方法有可能是開機時載入，或者是需要時才載入。
* 下圖說明一個blocking 的`read()` request的流程：
  1. process向先前打開的文件的file descriptor發出blocking read() system call。
  2. kernel檢查參數是否正確，如果buffer中已經有數據，則將數據返回給進程，完成I/O請求(盡量避免真的傳到底層做IO以節省時間)。
  3. 如果buffer中沒有數據，必須執行物理I/O。該process從running queue移到waiting queue中，並且I/O request被dispatch。 最終，I/O subsystem將request發送到driver。根據OS，request通過subroutine或kernel message發送。
  4. device driver allocates kernel buffer space來接收data和排班，然後driver會把指令寫到controler的register裡面
  5. device controler控制device開始執行IO
  6. driver polling狀態與資料，若由DMA controler處理，完成之後會發出interrupt
  7. interrupt handler接收interrupt並通知driver
  8. driver收到信號，並向IO subsystem發出完成信號，
  9. kernel將數據或返回碼傳送到process的address space，並將process從waiting queue移回ready queue。
  10. 將process移動到ready queue會解除對process的blocking。當process分配給CPU 時，process在system call完成時恢復執行。
* ![IO_LifeCycle](https://github.com/a13140120a/Operation_System/blob/main/imgs/IO_LifeCycle.jpg) 


<h2 id="0117">STREAMS</h2> 

* UNIX中的STREAMS機制在user program和設備驅動程式之間提供了全雙工連接通道，而且可以在其上添加modules，並且包含了「stream head」與「device end」。
* 戶進程與「stream head」互動。
* 設備驅動程式與「device end」互動。
* 每個module都有一個read queue跟write queue。
* 可以使用`ioctl()` system call將module推入STREAMS，(註：[ioctl函式介紹](https://blog.csdn.net/song_lee/article/details/104856385))
* STREAMS可以選擇support或不support flow control，在support的情況下，每個module將會有buffer存放資料，直到相鄰的module準備好接收它。如果沒有開啟flow control，那麼資料一準備好就被傳遞，當buffer滿了的時候，就有可能會溢位到相鄰的module的queue裡面(因為都是相鄰的)。
* STREAMS結構如下圖所示：
* ![STREAMS](https://github.com/a13140120a/Operation_System/blob/main/imgs/STREAMS.jpg)
* 當user process使用 `write()` 或 `putmsg()` system call 將data寫入devices，`write()` system call 將raw dara寫入STREAMS，而 `putmsg()`則允許user process設定(特製)message(消息)，無論user process使用何種system call，stream head都會將資料複製到message，並將其傳遞到queue中以供下一個module使用，這種消息複製一直持續到消息被複製到驅動程式端然後復製到設備為止，同樣地，user process使用`read()`或`getmsg()` system call從stream head讀取資料。 如果使用`read()`，則stream head從其相鄰的queue中獲取消息並將普通數據（非結構化的byte stream）返回給process。 如果使用 `getmsg()`，則會向process返回一條消息。 
* STREAMS I/O是asynchronous(或non-blocking)的(除了用user process和stream之間的interface這個階段以外)。
* 驅動程式必須接受並處理來自其設備的interrupt，如果相鄰module未準備好接受資料並且設備驅動程式的buffer已滿，則通常會丟棄資料，例如一個輸入緩衝區已滿的網卡。網卡必須丟棄消息。
* 使用 STREAMS 的好處是他提供了一個"模組化"(modular，類似[OS Structure裡面的模組化概念](0026))以及可以隨時增加數量的方法去撰寫驅動程式以及network protocal。
* module可以被不同的STREAM使用，也可以被不同的devices使用。System V UNIX 和 Solaris 使用 STREAMS 來實作socket。 
* 詳細請參考[使用ioctl控制STREAMS file](https://linux.die.net/man/3/ioctl)。


<h2 id="0118">Performance</h2> 

* I/O是整個系統性能的一個主要因素，並且可以給系統的其他主要組件（中斷處理、進程切換、內存訪問、匯流排競爭和設備驅動程式的CPU負載等等）帶來沉重的負載
* 儘管現代計算機每秒可以處理數千個中斷，但中斷處理是一項相對昂貴的任務。每個中斷都會導致系統執行狀態更改，執行中斷處理程序，然後恢復狀態。 如果busy waiting的周期數不太多，則programmed I/O 可能比產生interrupt的IO更有效。 I/O 完成通常會解除對process的block，這會增加context switch的overhead。
* 網絡流量也會給系統帶來沉重的負擔，以下是在telnet session中鍵入單個字符時發生的事件序列：
  * ![Intercomputer_communications](https://github.com/a13140120a/Operation_System/blob/main/imgs/Intercomputer_communications.PNG)
* 使用front-end processors(獨立前端處理器)可以減少主CPU的負擔，而terminal concentrator(終端集中器)可以在大型計算機的單個port上與數百個終端進行處理。
* 我們可以利用下列方法來改進IO的執行效率：
  * 漸少context switch的次數
  * 減少裝置與應用程式之間的傳輸時的記憶體複製次數
  * 藉由一次大量的傳輸配合少量cycle的polling，盡量降低interrupt發生的次數
  * 使用DMA來減輕CPU的負擔，並增加並行
  * 將處理的基本功能轉移到硬體之中，使controler的操作可以跟cpu及匯流排的操作並行。
  * 平衡CPU、記憶體子系統、匯流排與IO之效能，因為任一區域的額外負擔都會造成其他部份的閒置。
* 下圖分別代表在應用層、驅動程式、核心及硬體製作IO功能的比較
* ![Device_functionality_progression](https://github.com/a13140120a/Operation_System/blob/main/imgs/Device_functionality_progression.PNG)
  * 越往上，越有彈性
  * 越往下，月有效率，但開發成本越高，以及越抽象。

****




<h1 id="012">File-System Interface</h1> 

  * ## [File Concept](#0121) #
  * ## [Access Methods](#0122) #
  * ## [Directory Structure](#0123) #
  * ## [Protection](#0124) #
  * ## [Memory-Mapped Files](#0125) #




<h2 id="0121">File Concept</h2> 


* File Attributes：
  * Name：唯一人類看的懂的格式
  * Identifie：獨一無二的標籤，通常是一個數字，標識檔案系統中的文件，它是文件的非人類可讀名稱。
  * Type：不同類型文件的系統需要此信息。
  * Location：此信息是一個指向設備和該設備上文件位置的pointer。
  * Size：文件大小
  * Protection：access的控制信息決定了誰可以進行讀、寫、執行等等。
  * Timestamps and user identificatio：可以保留此信息以供創建、最後修改和最後使用。 這些數據可用於保護、安全和使用監控。 
  * 所有檔案的資訊都存在目錄(directory)結構(structure)中，這些structure也存在與檔案相同的儲存體上，目錄的entry是由檔案的名稱以及id所組成的，可透過檔案id再查詢到檔案的其他資訊。
  * 通常檔案跟目錄都會存在非揮發性裝置上，並且根據需求零碎地載入記憶體當中(像paging的概念)。

* open file information：
  * File-pointer：每個process都會有一個pointer指向現在的讀寫位置，因為每個process都不同，因此必須與硬碟上的檔案屬性分開。 
  * File-open count：OS會記錄每個打開的檔案到底被多少個process打開，當File-open count為0的時候代表開檔案已經沒有在使用，則會被OS從system-wide table(見下)移除。
  * Location of file：該訊息會指出檔案被保存在哪裡(可以是網路上，硬碟裡等等)，當檔案開啟後，這項資訊會保存在memory當中，這樣就不用每次操作都要從disk讀取目錄結構，再從目錄結構中搜尋到檔案的位置了。
  * Access right：訪問權限，此訊息會存在open-file table(見下)裡面。

* File Operations：
  * Creating a file：分成兩個步驟，首先要先allocate一個空間(見13章)，再來必須在目錄中建立該file的entry。
  * Opening a file：可以使用一個`open()`函數並返回一個句柄(handle)，這樣就不用每一次操作檔案都要進行查詢檔案名稱、定位檔案位置、檢查訪問權限等等一堆動作，`open()`還可以指定aceess的模式，例如創建、只讀、讀寫、僅附加等。
  * Writing a file：要寫入檔案，我們必須使用system call，同時指定打開的檔案句柄和要寫入檔案的資料，如果是sequential的話，系統必須保留一個指向檔案中下一次寫入的位置的"**write pointer**"。 每當發生寫入時，必須更新write pointer。 
  * Reading a file：要讀取檔案，我們使用system call來指定檔案句柄以及文件的下一個塊應該放在memory的哪裡。 同樣地，如果是sequential的話，系統需要保留一個"**read pointer**"，指向檔案中將發生下一次讀取的位置，每次發生讀取的動作，pointer就會更新，讀取和寫入操作都使用相同的poiter，可以節省空間並降低系統複雜性。 
  * Repositioning with a file：把打開的文件的pointer重新定位到給定值，此操作不須涉及任何IO，此操作又稱為[seek](http://codewiki.wikidot.com/c:system-calls:lseek)。
  * Deleting a file：刪除檔案必須先找到所有與其有關的directory entry，刪除entry之後再釋放檔案空間，如果有hard link的話，實際檔案內容不會被刪除，直到最後一個link被刪除。 
  * Truncating a file：使用者可能希望刪除檔案內容但保留其屬性，這種方法比起刪除並重新建立新的檔案還要方便的多。
  * 這七個基本操作為最基本且必要的操作。其他常見操作包括append(加到文件尾端)和rename，可以透過組合這些原始操作來達成，例如，我們可以通過create一個新的file然後從舊的file讀取並寫入新file來成為copy的動作。
  * 還有允許user獲取和設置檔案的各種屬性的操作。 例如，我們可能希望允許user確定檔案狀態（如檔案長度）和設置檔案屬性（如檔案的owner）的操作。 
  * open-file table：每個process會有一個table來記錄所有打開文件的相關信息，當請求檔案操作時，文件是通過該表的索引指定的，因此不需要搜索，當process關閉檔案時，OS從open-file table中刪除它的entry，可能會釋放鎖，`open()`通常會return指向open-file table的entry的pointer。
  * system-wide table：每個process的open-file table的entry又指向system-wide table的entry，通常system-wide table(原文書是寫open-file table，但我覺得不合理)都會有一個counter紀錄著多少個process打開了這個file，使用`close()`的時候這個counter會減1，當counter為0的時候，OS會從system-wide table刪除開檔案的entry。
  * `create()`和`delete()`是用來處理已經關閉的檔案而不是打開的檔案。 

* File locks：
  * 一些OS提供了鎖定了開啟中的檔案（或檔案的一部分）的功能，File locks 提供類似於[readers-writers-problem](https://github.com/a13140120a/Operation_System/edit/main/README.md#readers-writers-problem)中介紹的功能，File locks又分成讀取鎖(reader lock)以及寫鎖(writer lock)。
    * 讀取鎖類似於共享鎖(shared lock)，因為多個process可以同時獲取鎖。
    * 寫鎖的行為類似於互斥鎖(mutex lock)，一次只有一個process可以獲得這樣的鎖。
  * OS的鎖定機制又分成兩種：
    * mandatory(強制性)：使用強制鎖定，一旦process獲得mutex lock，OS將阻止任何其他進process access鎖定的檔案。例如，假設一個process在system.log上獲得了mutex lock。如果我們嘗試從另一個process（例如，文本編輯器）打開 system.log，將會被OS阻止，直到鎖被釋放。
    * advisory(建議性)：如果鎖定是建議性的，則OS不會阻止文本編輯器對system.log的訪access(但仍然會獲取鎖)，programmer必須自己決定開怎麼做。
  * 在Java API中，要取得所需要先取得FileChannel才能將檔案上鎖。其API是`FileLock lock(long begin, long end, Boolean shared)`，begin和end決定檔案中被上鎖的區域起始點跟結束位置，shared決定鎖能不能被共享，[範例](https://github.com/a13140120a/Operation_System/blob/main/file_lock.java)

* File Types：
  * 如果OS識別出檔案的類型，它就可以以合理的方式對檔案進行操作。例如，當你cat一個二進制形式的file(例如可執行檔)的時候，就會產生垃圾(一堆亂碼)，但是，如果OS知道該檔案是二進制形式的可執行檔，則該嘗試可能會成功。
  * 實現檔案類型的常用技術是將類型作為檔名的一部分包含在內。名稱分為兩部分——檔及副檔名，通常用句點分隔，如下圖所示。
  * ![CommonFileTypes](https://github.com/a13140120a/Operation_System/blob/main/imgs/CommonFileTypes.jpg)
  * 副檔名並不是必要的，user可以指定一個沒有副檔名的檔案，並且指定特定的應用程式來執行。
  * macOS中，每個檔案都有一個類型，例如 .app（用於應用程式）。 每個文件還有一個 creator 屬性，其中包含創建它的process的名稱，該屬性由OS在呼叫`create()`的時候設置，因此系統強制使用並支持該屬性。例如，由文字處理器生成的文件以文字處理器的名稱作為其創建者。 當用戶打開該檔案時，通過在代表該檔案的圖標上雙擊鼠標，自動調用文字處理器，並加載該檔案，準備進行編輯。 
  * UNIX 系統使用存儲在一些二進製文件開頭的"**magic number**"來指示文件中數據的類型（例如，shell script開頭的`#!`），使用text magic number可以告訴OS腳本是用哪種 shell 語言編寫的，並非所有文件都有magic number。另外，UNIX 也不記錄創建程序的名稱。 UNIX 確實允許副檔名提示，但既不強制也不依賴副檔名，它們主要用於幫助「user」確定檔案內容類型。
    * 關於[magic number](https://go-compression.github.io/reference/magic_numbers/)的解釋
    * [magic number表](https://en.wikipedia.org/wiki/List_of_file_signatures)

* File Structure：
  * 每個OS必須完全支援至少一種可執行檔案類型，以便OS可以載入和執行程式(例如service以及subsystem等等)。
  * OS支援的檔案格式越多，會增加OS的大小和復雜性。
  * UNIX將所有檔案視為一連串的8位元的byte，並且無視內部結構(留給使用該檔案的應用程式或者user自己決定)。（除了二進制可執行檔，因為OS必須知道如何載入和找到第一個可執行的指令。）


* Internal File Structure：
  * Disk通常以block(physical)為單位，每個physical block通常為512 bytes 或其兩倍的冪。（較大的硬碟使用較大的block，以將block的number的範圍保持在32位整數的範圍內。）
  * 通常會有一個適合的logic units的數量packing 到physical block裡面，例如，UNIX將所有file定義為簡單的byte stream，每個byte都可以通過其檔案的開頭（或結尾）的偏移量來單獨尋址，在這種情況下，logic unit大小為 1 byte，檔案系統會根據需要自動將bytes(也就是logic unit)packing到disk的physical block中（例如，每個block 512 bytes）。
  * 如果每個block是 512 bytes，那麼一個1949 bytes的檔案將被分配四個block。


<h2 id="0122">Access Methods</h2> 

* Sequential Access：
  * Sequential Access的概念就像對磁帶(tape)的操作，循序讀取。
  * 最常見的access方式，例如，文字編輯器和編譯器通常以這種方式存取檔案。
  * 可以同時在sequential-access以及random-access的devices上面使用
  * 讀取操作`read_next()`讀取文件的下一部分並自動將檔案指標+1，該指標及是目前I/O的位置，寫操作`write_next()`亦相同。
  * ![SequentialAccessFile](https://github.com/a13140120a/Operation_System/blob/main/imgs/SequentialAccessFile.jpg)
* Direct Access(random access)：
  * jump到任何block並讀取
  * Direct Access中我們使用讀取操作`read(n)`(其中 n 是block number)，而不是`read_next()`，以及`write(n)`而不是`write_next()`。 
  * data base通常屬於這種類型，當有query到達時，我們「**計算**」(例如使用hash)哪個block包含答案，然後直接讀取該塊以提供所需的信息。 
  * Sequential access可以模擬Direct Access，但是複雜而且低效。
  * ![Sequential_Access_Simulation](https://github.com/a13140120a/Operation_System/blob/main/imgs/Sequential_Access_Simulation.jpg)
* user提供給OS的block number通常是一個相對的"**relative block number**"，relative block number是相對於檔案開頭的索引，例如，檔案的第一個relative block number是 0，下一個是 1，依此類推，即使第一個block的絕對物理硬碟地址可能是 14703，而第二個塊的絕對物理硬碟地址可能是 3192。 使用relative block number允許OS決定文件應該放在哪裡，並有助於防止用戶訪問可能不屬於文件系統的部分 她的檔案，有些系統從 0 開始，有些從 1 開始。 

* Index Access：
  * 這種做法是在random access上面再加一層index
  * index包含指向各個block的pointer。 要在檔案中查找記錄，我們首先搜尋index，然後使用pointer直接訪問文件並找到所需的block。 
  * 對於大型檔案，index檔案本身可能會變得太大而無法保存在memory中。一種解決方案是為index檔案創建另一層index。主索引檔案包含指向二級索引文件的pointer，這些pointer指向實際的block。 
  * ![IndexAndRelativeFiles](https://github.com/a13140120a/Operation_System/blob/main/imgs/IndexAndRelativeFiles.jpg)

<h2 id="0123">Directory Structure</h2> 

* 目錄可以視為是一種把檔名轉換成file control blocks的符號表(symbol table)。
* 我們希望能夠插入項目(entry)、刪除項目、搜索指定名稱的項目以及列出目錄中的所有項目。
* 目錄結構應該包含以下操作：
  * Search for a file：搜索目錄結構以找到特定檔案的entry，並且相似的名稱可能表示檔案之間的關係，因此我們可能希望能夠找到名稱與特定模式匹配的所有文件。 
  * Create a file：建立新的檔案，並加到directory當中
  * Deleting a file：刪除檔案會在目錄結構中留下一個洞(hole)，並且文件系統可能具有對目錄結構進行碎片整合的方法。 
  * List a directory：列出目錄中的檔案以及列表中每個檔案的directiory entry的內容。 
  * Rename a file：重新命名檔案可能會更改其在目錄結構中的位置。 
  * Traverse the file system：為了reliable，定期保存整個檔案系統的內容和結構是個好主意

* Single-Level Directory
  * 最簡單的目錄結構，所有檔案都包含在同一目錄中，易於理解。 
  * 當文件數量增加或系統有多個user時，會有很大的限制。
  * 由於所有文件都在同一個目錄中，因此不能有相同名稱的檔案出現。系統有很多user的話，會導致檔案名稱的混淆，因為每個user的每個檔案都要取不同的名稱。
  * ![SingleLevel_dir](https://github.com/a13140120a/Operation_System/blob/main/imgs/SingleLevel_dir.jpg)

* Two-Level Directory
  * ![TwoLevel_dir_Structure](https://github.com/a13140120a/Operation_System/blob/main/imgs/TwoLevel_dir_Structure.jpg)
  * 正如我們所見，Single-Level Directory會導致不同user之間的檔名混淆。 標準解決方案是為每個用戶創建一個單獨的目錄。 
  * 每個user都有一個"**user file directory (UFD)**"，UFD僅列出該user的檔案，當user的程式執行時或user login時，將搜索系統的"**master file directory(MFD)**"。 MFD按使用者名或帳號進行索引，每個entry都指向該用戶的 UFD。 
  * 當user引用特定檔案時，OS僅搜索他自己的UFD，因此，不同user可能擁有同名的檔案，只要每個 UFD 中的所有檔名都是唯一的。 要為用戶創建或刪除文件，OS僅搜索該用戶的 UFD 以確定是否存在該名稱的另一個檔案。
  * 這種結構有效地將user之間隔離開來，當user完全獨立時，隔離是一個優勢，但當用戶想要合作時，隔離是一個劣勢
  * 有些系統不允許其他用戶訪問本地用戶文件：
  * 在允許的情況下，如果user A 希望訪問他自己的名為 test.txt 的測試文件，他可以簡單地引用 test.txt。 但是，要訪問user B（目錄名稱為 userb）的名為 test.txt 的文件，她可能必須引用/userb/test.txt，因此，用戶名和文件名定義了路徑名。每個系統都有自己的語法來命名用戶自己目錄以外的目錄中的文件。 
  * 作為系統loader、assembler、compiler、utility routines、libraries等的一部分提供的program通常被定義為文件，如果為了要執行這些文件而在每個UFD，都放上一份相同的檔案的話會非常浪費空間，因此通常會定義了一個特殊的用戶目錄來包含系統檔案（例如，user 0）。 每當指定要加載的檔案名時，OS首先搜索本地UFD。 如果找到該檔案，則使用該檔案。 如果沒有找到，系統會自動搜索檔案系統的特殊用戶目錄。


* Tree-Structured Directories
  * ![Tree_dir_Structure](https://github.com/a13140120a/Operation_System/blob/main/imgs/Tree_dir_Structure.jpg)
  * 最常見的目錄結構，會有一個根目錄，系統中的每個文件都有一個唯一的路徑名。
  * 在許多系統中，目錄只是另一個文件，但它以特殊方式處理，所有目錄都具有相同的內部格式，每個目錄的entry中的一個bit將entry定義為文件或子目錄(0或1)。 特殊system call用於創建和刪除目錄。
  * 在正常使用中，每個process都有一個當前目錄(current directory)。 當前目錄應該包含process會使用到的大部分檔案，如果需要不在當前目錄中的檔案，則通常必須指定路徑名或將當前目錄更改為保存該檔案的目錄，設計者可以提供一個system call，它將目錄名稱作為參數並使用它來重新定義當前目錄。
  * user登錄 shell 的初始當前目錄是在user process執行時或user login時指定的。
  * OS通常會有一個accounting file來保存指向用戶初始目錄的pointer
  * 從shell產生的其他process的當前目錄通常是父process(也就是shell)生成時的當前目錄。 
  * 在 UNIX 和 Linux 中，絕對路徑名從根（由初始"/"指定）開始，並沿著路徑向下到達指定檔案，給出路徑上的目錄名，並有相對路徑(relative path)與絕對路徑(absolute path)。
  * 可執行文件在許多系統中被稱為"二進製文件"，它們通常存儲在 bin 目錄中。
  * 處理刪除目錄的方法有兩種：
    * 除非它是空的，否則無法被刪除
    * 另一種方法，例如 UNIX rm 命令所採用的方法：當刪除一個目錄時，該目錄的所有檔案和子目錄也將被刪除

* Acyclic-Graph Directories
  * 當需要在目錄結構中的多個位置訪問相同的檔案時（例如，因為它們被多個user/process共享），Acyclic-Graph Directories就很有用。 
  * link：UNIX系統有一種特殊的directory entry叫做"**link**"，link實際上是指向另一個檔案或子目錄的pointer。
  * OS在遍歷目錄樹時會忽略這些link以保留系統的非循環結構。
  * ![AcyclicGraph_dir](https://github.com/a13140120a/Operation_System/blob/main/imgs/AcyclicGraph_dir.jpg)
  * Linux中包括兩種連結：硬連結(Hard Link)和軟連結(Soft Link),軟連結又稱為符號連結（Symbolic link），而硬連結又稱為非符號連結(non-symbolic link)。
    * 當我們建立了一個檔案的硬連結時，硬連結會使用和檔案相同的inode號，硬連結會保留一個count來記錄有多少硬連結連結到這個檔案，當硬連結為0時，會刪除開檔案並釋放硬碟空間，另外，硬連結只能應用於檔案，而不能應用於目錄，而且不能跨檔案系統（或分割槽）。
    * 在建立檔案的軟連結時，軟連結會使用一個新的inode，所以軟連結的inode號和檔案的inode號不同，軟連結的inode裡存放著指向檔案的路徑，刪除檔案，軟連結也無法使用了，因為檔案的路徑不存在了；當我們再次建立這個檔案時（檔名與之前的相同），軟連結又會重新指向這個檔案（inode號與之前的不同了）
    * [參考資料](https://codertw.com/%E7%A8%8B%E5%BC%8F%E8%AA%9E%E8%A8%80/561681/)
  * Windows 只支援 symbolic links(shortcuts，捷徑)。


* General Graph Directory：
  * 在General Graph Directory中，目錄的循環是允許存在的。
  * 當這樣的循環成為可能時，設計正確搜索和遍歷檔案系統的演算法就變得更加困難，設計不當的演算法可能會導致無限循環不斷地搜索並且永遠不會終止，一種解決方案是限制搜索期間將訪問(access)的目錄數量。 
  * 當我們試圖確定何時可以刪除檔案時，也存在類似的問題。對於Acyclic-Graph Directories結構，檔案的引用的count值為 0 意味著不再有對該檔案或目錄的引用，並且可以刪除該文件。但是，當循環存在時，即使無法再引用的目錄或文件，檔案的引用的count值也可能不為 0。
  * 在這種情況下，我們一般需要使用一種"**garbage collection**"的方法來確定最後一個引用何時被刪除，並且可以重新分配磁盤空間。
    * garbage collection遍歷整個檔案系統，標記可以訪問的所有內容。然後，把所有未標記到的內容刪除(或收集起來)，然而這種作法非常耗時。
  * 一個更簡單的方法是在目錄遍歷期間繞過link。
  * ![general_graph_direct_with_circle](https://github.com/a13140120a/Operation_System/blob/main/imgs/general_graph_direct_with_circle.PNG)


<h2 id="0124">Protection</h2> 

* access type：
  * 透過controll access，可以控制幾種不同類型的操作： 
    * Read
    * Write
    * Execute
    * Append
    * Delete
    * List：列出file
    * Attribute Change
  * 其他例如重新命名、複製和編輯檔案，這些higher-level functions也是由進行lower-level system call來實現。
  * 所有保護機制都有優缺點，例如，小型計算機系統可能不需要與大型公司計算機相同類型的保護。

* Access Control：
  * **access-control list(ACL)**：當user請求access特定檔案時，OS會檢查與ACL，如果符合權限則允許，否則將拒絕user access。
  * 這項技術會遇到一些難題，一是如果系統中有很多user，那麼list將會變的很大，二是目錄的entry將會變的有些大有些小，這讓空間難以管理。
  * 解決這項難題的其中一種辦法是，壓縮ACL的長度，許多系統識別出與每個文件相關的三類用戶：
    * Owner：檔案擁有者
    * Group：一個group裡面的所有user的權限是一樣的
    * Other：其他所有使用者
  * UNIX 系統定義了三個欄位，每個欄位為 3 bits(rwx)，其中 r 控制讀權限，w 控制寫權限，而 x 控制執行權限。
* Other Protection Approaches：
  * 或者我們可以藉由把文件或目錄加上密碼，來保護我們想要保護的資料。

<h2 id="0125">Memory-Mapped Files</h2> 

* 通過將disk block map到memory中的一個（或多個page）來間接操作file
* 寫入memory中的map檔案內容不一定是立即（同步）寫入disk的，當檔案關閉時才會根據memory的更改來更新檔案，並刪除存在virtual memory中的內容。
* 所有對這塊mapped memory的修改，都會馬上寫進swap space以免資料遺失，
* Solaris的檔案如果使用 `mmap()`system call，就會將檔案map到process的address space中，如果使用普通的system call（例如 `open()`、`read()` 和 `write()`）來開啟檔案，仍會對該檔案進行Memory-Mapped，不同的是，該檔案會被map到kernel space。所以無論文件如何打開，Solaris 都將使用Memory-Mapped來增加效率 
* ![MemMapFiles](https://github.com/a13140120a/Operation_System/blob/main/imgs/MemMapFiles.jpg)
* 多個process可以同時map同一個檔案來共享數據，如下圖所示：
* ![SharedMemMapd](https://github.com/a13140120a/Operation_System/blob/main/imgs/SharedMemMapd.jpg)
* 使用這種方法可以讓process進行溝通，請見[winsos API producer](https://github.com/a13140120a/Operation_System/blob/main/windowsAPI_mmap_producer.c)、[winsos API consumer](https://github.com/a13140120a/Operation_System/blob/main/windowsAPI_mmap_consumer.c)


****




<h1 id="013">File-System Implementation</h1> 

  * ## [File-System Structure](#0131) #
  * ## [File-System Operations](#013) #
  * ## [Directory Implementation](#0133) #
  * ## [Free-Space Management](#0134) #
  * ## [Efficiency and Performance](#0135) #
  * ## [Recovery](#0136) #
  * ## [Example: The WAFL File System](#0137) #




<h2 id="0131">File-System Structure</h2> 

* file control block(FCB)：(UNIX 檔案系統中就是一個 inode)包含有關檔案的信息，包括檔案內容的所有權、權限和位置。 
  * ![FileControlBlock](https://github.com/a13140120a/Operation_System/blob/main/imgs/FileControlBlock.jpg)
* disks(HDD)傳統硬碟有著可以隨意修改、覆寫block以及隨機存取的特性，而NVM裝置(例如SSD)則不能覆寫但還是越來越常使用於儲存資料。
* 為了提高效率，memory跟disk之間通常以block為單位傳輸資料，大小依驅動程式而不同，通常是512bytes或者4KB，NVM通常會有4KB的block。
* 如下圖所示，檔案系統本身會由許多不同的layer所組成：
  * ![LayeredFileSystem](https://github.com/a13140120a/Operation_System/blob/main/imgs/LayeredFileSystem.jpg)
  * The I/O control層：由 *驅動程式* 和 *interrupt handler(中斷處理器)* 所組成，負責memory與disk之間的信息傳遞，驅動程式就像一個translator(翻譯器)，它可以把由上層傳送過來指令(例如`retrieve block 123`)轉換成low-level的hardware-specific instructions(靈活度較小)，並傳送給硬體的controler。
  * basic file system層(在Linux中亦稱為block I/O subsystem)：這層會根據logical block addresses向下一層的驅動程式發送 *通用命令(generic commands)* ，IO scheduling亦是在這層進行的，該層還管理保存各種檔案系統、目錄和data block的buffer和cache，這些cache跟buffer用於保存經常使用的檔案系統的metadata以提高性能。
  * file-organizatio module層：這層負責管理檔案及其logical blocks，這層也包含了free-space manager。
  * logical file system層：管理metadata的information(像是UID, GID, mode, dates等等)，即除數據本身之外的有關檔案的所有內容，該層管理目錄結構和"檔案名稱與FCB的mapping"，並且透過FCB維護檔案結構，其中包含所有metadata以及用於在disk上搜尋data block number的信息。
  * 分層結構可以使code最小化，但分的越多層也會降低其效能
* FUSE 檔案系統將檔案系統實現為用戶級而不是核心級代碼來實現和執行檔案系統，從而為檔案系統的開發和使用提供了靈活性，使用者可以將新的檔案系統添加到各種不同的OS，並可以使用該檔案系統來管理自己的檔案。 
* UNIX 使用base-on Berkeley Fast File System(FFS) 的UNIX fil system(UFS)，儘管 Linux 支持超過 130 種不同的檔案系統，但標準的 Linux 檔案系統被稱為 *extended file system* ，最常見的版本是 ext3 和 ext4。


<h2 id="0132">File-System Operations</h2> 

* File system 由 *on-storage* 和 *in-memory* 的structure所維護：
  * On storage，包含了包括 *how to boot an operating system* 、 *the total number of blocks* 、*the number and location of free blocks* 、 *the directory structure* 以及檔案的information：
    * boot control block(per volume, 如果是開機碟或boot partition的話)：通常會在一個volume的第一個bolck，在 UFS 中，它被稱為 *boot block*。 在 NTFS 中，則被稱為 *partition boot sector*。 
    * volume control block(per volume)：該block記載了volume的一些詳細資料，包括block的數量(in volume)、block size、free-block數量、 free-block pointers、free-FCB數量、FCB pointers，volume control block 在UFS中稱為superblock，在NTFS中，它存儲在master file table中。
    * directory structure (per file system)：用於組織檔案，在UFS中，這包括檔名和相關的inode number，在NTFS中，它存儲在master file table。 
    * FCB (per-file)：包含了一個不會重複的id，在NTFS中，FCB存在master file table當中，並且這個table使用關聯是資料庫的結構，每個row一個file。
  * in-memory，同時用於管理檔案系統，以及增加檔案操作的效能，這些data在mount的時候載入，並且在unmount的時候丟棄：
    * in-memory mount table：包含每個mount起來的volume的訊息。
    * in-memory directory-structure(recently used)：把最近access過的directory訊息cache下來，就不用每次都去讀取硬碟，會包含一個指向volume table的pointer。
    * system-wide open-file table：包含每個open file的FCB的copy以及其他訊息。
    * per-process open-file table：包含指向system-wide open-file table的entry的pointer，以及該process已經open的file的其他訊息。
    * buffer在從檔案系統讀取或寫入檔案系統時保存file-system block。 
  * 當一個檔案被建立時，process會call logical file system，logical file system知道directory structures的格式，並且會幫它分配一個新的FCB（或者，如果檔案系統會在檔案系統建立時，建立所有的FCB，這時則從空閒FCB set中allocate一個FCB），然後將適當的目錄載入memory，用新的file name 更新它，再寫回檔案系統。
  * 有些作業系統(例如UNIX)，會將檔案跟目錄視為相同的東西，並保留一個"type"的欄位來標示這是目錄還是檔案，而windows等等其他OS則會把檔案跟目錄當作完全不同的東西，並且使用不同的system call。
  * 無論各種structure的差異有多大，logical file system都會call file-organization module去map上層的directory I/O到下層的storage block locations(也就是傳遞到basic file system和I/O control system)。(這邊應該是指logical file system會將各種structure的差異都封裝起來(藉由呼叫各種不同的file-organizatio module)傳遞到下層)

* Usage：
  * 當建立了一個檔案之後，現在它可以用於I/O，首先必須使用`open()`將檔名傳遞給logical file system層，`open()`會搜尋system-wide open-file table查看該檔案使否已經被其他process開啟，如果是的話則建立per-process open-file table的entry並指向system-wide open-file table的entry，
  * 如果沒有任何process在使用的話，則在目錄結構中搜索給定的檔名，而這邊部份的目錄結構通常會cache起來(最近用過的)以加速目錄的操作，
  * 找到檔案之後，其FCB就會被複製到memory中的system-wide open-file table中，該表不僅存儲FCB，還記錄了打開該file的process數量。
  * `open()`system call會返回per-process open-file table的entry的pointer，檔名通常不會記錄在這個table上，因為檔案打開之後就不再需要檔名了，而這個pointer在UNIX系統稱為 *file descriptor(檔案描述符)* ，在Windosw則稱為 *file handle(檔案句柄)* 。
  * 檔案被`open()`之後通常會被cache在memory當中，BSD UNIX system在可以保存disk I/O 的任何地方都使用cache，並且有高達85%的cache hit。
  * ![FileSystemStructures](https://github.com/a13140120a/Operation_System/blob/main/imgs/FileSystemStructures.jpg)

<h2 id="0133">Directory Implementation</h2> 

* 目錄分配和目錄管理演算法是設計上的一個很重要的方面，它影響效率、性能和可靠性。
* Linear List：
  * 實作目錄的最簡單方法是使用帶有指向data block的pointer的file name linear list。
  * ![os_directory_implementation_linear_list](https://github.com/a13140120a/Operation_System/blob/main/imgs/os_directory_implementation_linear_list.png)
  * 優點：實作起來非常容易
  * 缺點：建立檔案與刪除檔案必須要先traverse整個linked list(建立檔案需要搜尋是否有同名的檔案)，會讓效能下降，
    * 使用排序的list(sorted list)雖然可以增加搜尋效率，但會使新增及刪除節點變的更複雜
    * 使用平衡樹也許會更有幫助。
* Hash table：
  * 使用一個linear list來儲存每個directory entry，並且hash table把file name轉換成linear list的節點
  * ![os_directory_implementation_hash_table](https://github.com/a13140120a/Operation_System/blob/main/imgs/os_directory_implementation_hash_table.png)
  * 優點：可以大幅減少搜尋目錄的時間，而且插入及刪除也相對簡單。
  * 缺點：hash table的大小固定，缺乏靈活度，例如如果hash table裝了64個entry(等於使用mod 64)，因此，我們會需要一個新的hash function(變成mod 128之類的)。
    * 可以使用chained-overflow hash table來解決這個問題，每個hash entry指向的不是單一的directory entry，而是一個由directory entry組成的linked list，但使用這種方法如果發生collision的話會降低效能，需要traverse linked list，但仍然比Linear List的方法要快。


<h2 id="0133">Allocation Methods</h2> 

* 三種基本的secondary storage space allocation方法被廣泛的使用：*contiguous* 、 *linked*、 *indexed* ，儘管有些檔案系統同時支援這三種方法，但大多數都只支援一種。
* Contiguous Allocation：
  * ![file_ContiguousAllocation](https://github.com/a13140120a/Operation_System/blob/main/imgs/file_ContiguousAllocation.jpg)
  * 同時支援sequential 跟random access。
  * 分配的方法可以參考前面提到的[contiguous memory allocation](https://github.com/a13140120a/Operation_System/edit/main/README.md#contiguous-memory-allocation-1)
  * 優點：磁頭移動的距離最少
  * 缺點：會導致大量的external fragmentation，而且難以幫新的檔案allocate空間，因為不知道檔案會漲到多大。
    * 防止external fragmentation的方法之一是compact(把所有的檔案都重新往前擺放，消除hole)，但這樣會消耗非常可觀的時間
* Extent-Based File System：
  * 檔案剛建立時，會先分配一塊contiguous的space給它
  * 等到檔案長到space不夠用的時候，會給它一個link，指向另一塊contiguous的space，稱為 *extent*，某些系統允許使用者設定extent的大小。
  * [Symantec Veritas file system](https://en.wikipedia.org/wiki/Veritas_File_System)使用此項技術來做為性能的優化。
* Linked Allocation：
  * ![file_LinkedAllocation](https://github.com/a13140120a/Operation_System/blob/main/imgs/file_LinkedAllocation.jpg)
  * 優點：解決了contiguous allocation的空間分配問題，不會有external fragmentation或者檔案長到塞不下的問題，也不用compact。
  * 缺點：對於sequential-access還好，但對於random access的效能非常差，而且會有reliability的問題，因為只要中間有一個block斷掉(損毀)，整個檔案的後面資料都會損毀，還有一個缺點是每個block會浪費一個pointer的空間。
    * 一種折衷的方法是使用 *cluster* ，例如，檔案系統可以將四個block定義成一個cluster，每次操作都是以clustser為單位，代價是增加了internal fragmentation
    * reliability的問題可以使用doubly linked list來解決，但是會浪費更多的空間
* file-allocatio table (FAT)：
  * 早期MS-DOS使用的方法，每個volume的開頭會保留一個空間來儲存file-allocatio table，該表的每個entry的index是block number，而field儲存的則是指向下一個block的number。
  * FAT如下圖所示：
    * ![FAT_Table](https://github.com/a13140120a/Operation_System/blob/main/imgs/FAT_Table.jpg)
  * 如果該block未被分配，則儲存的field為0，要分配新的block給檔案只需要找到第一個為0的block就可以了。
  * 通常FAT會被cache在memory裡面(如果不這麼做的話會導致大量的磁碟讀取)。

* Indexed Allocation：
  * 將所有block的pointer蒐集起來放在一個 *index block* 裡面，如下圖所示：
    * ![file_Index_Allocation](https://github.com/a13140120a/Operation_System/blob/main/imgs/file_Index_Allocation.jpg)
  * 優點：這既解決了Contiguous Allocation的分配問題，也解決了Linked Allocation的random access的問題。
  * 缺點：如果file太小，則index block會有internal fragmentation的問題，如果file太大，則要許多個index block
  * 關於需要許多個index block的問題，以下有幾種解決方案：
    * Linked scheme：每個index block都包含下一個index block的pointer
      * ![index_block_Linked_scheme](https://github.com/a13140120a/Operation_System/blob/main/imgs/index_block_Linked_scheme.PNG)
    * Multilevel index：多層索引
      * ![index_block_Multilevel_index](https://github.com/a13140120a/Operation_System/blob/main/imgs/index_block_Multilevel_index.PNG)
    * Combined scheme：也就是UNIX檔案系統使用的方案，將index block的前 15 個pointer保留在檔案的inode中，  
                       前12個direct blocks直接指向data block(假設每個block 4KB, 這邊可以存48KB)，  
                       第13個指向indirect blocks(超過48KB的話才會到這裡，可以存4MB)，  
                       第14個指向double indirect block(可以存4GB)，  
                       第15個指向triple indirect block(4TB)，如下圖所示：  
      * ![combine_schema](https://github.com/a13140120a/Operation_System/blob/main/imgs/combine_schema.jpg)
 
 
<h2 id="0134">Free-Space Management</h2> 

* 追蹤free space有以下幾種方法：
* Bit Vector：
  * 使用一個bitmap或bit vector來記錄free block，一個bit代表一個block，1為free block，0為allocated
  * 例如，有block 2、3、4、5、8、9、10、11、12、13、17、18、25、26 和 27 是free，其餘的是allocated，則free-space bitmap將會是：
    * `001111001111110001100000011100000 ...`
  * 利用這種方法只要檢查每個word(字組)是否為0，只要不為0則代表找到free block
  * 第一個free block number的計算方式是：
    * `（每個word的位元數）×（為0的word的數量）+ 第一個不為0的word的第一個1的bit的offset `
  * 優點：實現方法簡單，在硬碟上找到第一個free block或 n 個連續的free block的速度很快。
  * 缺點：佔大量儲存空間，具有 4 KB 塊的 1 TB 硬碟需要 32 MB的空間來儲存bitmap。
* Linked List：
  * 將所有的free block串在一起。
  * 如果需要找到連續空間的化這種方法會很沒有效率，幸好絕大多數時候OS都只需要一個block
* Grouping：
  * 是Linked List的修改版，list中的第一個block用來存放n個free block(其實是n-1個)的number，最後一個block則用來存放另外n個free block的number
  * 使用這種方法可以快速找到大量的free block
* Counting：
  * 透過類似上述cluster的方法來實作free block list，list中的每個entry(或者說node)都包含了一個free block的number以及連續的free block的數量
  * 這些entry可以存在平衡樹中，而不是linked list中，以實現高效的查找、插入和刪除。
* Space Maps：
  * 在管理可用空間時，ZFS 使用多種技術來控制數據結構的大小並最大限度地減少管理這些結構所需的 I/O。
  * 首先，ZFS 建立 *metaslab* 以將設備上的空間劃分為可管理大小的塊。
  * 一個volume可能包含數百個metaslab，而每個metaslab都會有一個相關聯的 *space map* ，space map是一種會記錄block活動的log。
  * ZFS 使用計數演算法來存儲有free block的資訊，與傳統將計數結構寫入disk的方法不同，而是使用日 *log-structured file-system* 技術來記錄它們。
  * 當ZFS決定要從metaslab allocate或者free空間的時候，他會把相關的space map載入到存在記憶體中的平衡樹(balanced tree)結構當中，按偏移量索引，並且"replay"這個space map。
  * ZFS還透過將連續的free block結合成單一個entry來壓縮map
  * 本質上，log加balanced tree 就是free-list。 
  * [zfs 特色術語](https://docs.freebsd.org/zh-tw/books/handbook/zfs/#zfs-term)
  * [zfs 架構圖](https://people.freebsd.org/~gibbs/zfs_doxygenation/html/d1/d4d/structmetaslab.html)
  * [space map解說(英文)](https://sdimitro.github.io/post/zfs-lsm-flushing/)

 
<h2 id="0135">Efficiency and Performance</h2> 

* UNIX inode 是在volume上預先分配的，透過這種方法UNIX可以將檔案的data block放在檔案的inode附近，可以減少硬碟的seek time並提高檔案系統的效能，
* BSD UNIX可以隨著檔案大小的增加而改變cluster的大小，來減少internal fragmentation
* 有些檔案系統會記錄"後寫入日期"或"最後訪問日期"，這意味著任何時候只要打開檔案，也必須要跟著打開並修改他的FCB，這樣會造成效能下降。
* **unified buffer cache**：硬碟map到一個buffer，而這個buffer又map到另一個buffer，這種現象稱為 *double caching* ，這種現象會降低效能，下圖代表`read()`、`write()`system call分別使用double caching 以及unified buffer cache：
  * ![unified_buffer_cache](https://github.com/a13140120a/Operation_System/blob/main/imgs/unified_buffer_cache.jpg)
* sequential read/write的檔案不使用LRU演算法，因為最近使用的page將最後使用，或者可能永遠不會再使用
  * 可以使用 *free-behind* 或者 *read-ahead* 的技巧來來進行優化
  * free-behind：訪問完page的buffer後，應該立即釋放它們，因為它們可能不會再次被訪問
  * read-ahead：當從一個sequential的檔案中將一個block讀入buffer時，後面的幾個block也應該一起讀入，因為它們可能很快就會被訪問
* 小資料量的傳輸，當寫入disk時，因為cpu只需要將要寫入的資料放進memory就好，剩下的就由DMA或device controler來處理就好，而讀取要等待disk把資料讀出來，因此小資料量的傳輸其實是write的速度會大於read的速度，這與直覺相反。而大量的資料傳輸則是read的速度會大於write。


 
<h2 id="0136">Recovery</h2> 

* Consistency Checking：
  * OS可以使用一個 *consistency checker* (例如Linux的[fsck](https://blog.gtwang.org/linux/linux-fsck-examples/))來檢查目錄結構與data block之間的一致性，
  * 檔案系統實現的細節，例如分配演算法和free space管理演算法，決定了consistency checker可以檢測和糾正哪些類型的問題
* Log-Structured File Systems：
  * 基本思想是首先將metadata的更改按照順序寫入log，寫入的每個操作稱為 *transaction* ，寫入了之後system call就會return到user的process讓可以繼續執行，
  * 同一時間，這些修改的log會在檔案系統實際進行，隨著更改的進行，pointer會更新以指示哪些操作已完成，哪些操作仍未完成。
  * 這個log檔實際上是一個循環緩衝區(circular buffer)，循環緩衝區寫入其空間的末尾，然後從開頭繼續，並在進行時覆蓋舊值。
  * log檔裡面存的都是沒有完成的transaction，所以如果系統發生崩潰，這些log檔裡面的紀錄就是必須完成的transaction。
  * 使用metadata的log的另一個好處是，原本昂貴的同步隨機寫入metadata，變成了成本更低的同步循序寫入的metadata transaction log，而這些更改通過對適當結構的隨機寫入異步"replay"。
* Other Solutions：
  * WAFL檔案系統和 Solaris ZFS採用了Consistency Checking的另一種替代方法，這些系統永遠不會用新數據覆蓋舊的block，transaction(交易)將所有data和metadata更改寫入new block，當交易完成時，只指向舊的data block的pointer會指向新的data block，然後檔案系統可以刪除舊pointer和block，並使它們可以重使用。
  * ZFS 採用了一種更具創新性的維持Consistency的方法。 與WAFL一樣，它從不覆蓋block，而且更進一步提供所有metadata和data block的checksum。此解決方案（與 RAID 結合使用時）可確保數據始終正確。 因此 ZFS 沒有consistency checker。
  * Backup and Restore：
    * 為了最大限度地減少所需的複制，我們可以使用每個file的directory的entry的信息，例如，如果備份程序知道檔案的最後一次備份是什麼時候完成的，並且該檔按在目錄中的最後寫入日期表明該檔案自該日期以來沒有更改，則不需要再次復制該檔案，典型的備份計劃可能如下所示： 
      * 第 1 天。將檔案系統中的所有檔案複製到備份裝置，這稱為 *full backup* 。
      * 第 2 天。將自第 1 天以來更改的所有檔案複製到另一個備份裝置。這是一個 *incremental backup*
      * 第 3 天。將自第 2 天以來更改的所有檔案複製到另一個介質。
      * 第 N 天。將自第 N-1 天以來更改的所有檔案複製到另一個儲存裝置。然後返回第 1 天。 
    * 使用這種方法，我們可以通過使用full backup開始恢復並繼續執行每個incremental backup來恢復整個檔案系統。 
    * N 的值越大，為完全恢復必須讀取的資料量就越多。此備份週期的另一個優點是，我們可以通過從前一天的備份中檢索已刪除的文件來恢復該週期中意外刪除的任何檔案。 

<h2 id="0137">Example: The WAFL File System</h2> 

* WAFL(The write-anywhere file layout) 是一個針對隨機寫入進行了優化的檔案系統，由NetApp公司所開發。
* 主要是為了分散式系統所設計的，它可以通過 NFS、CIFS、iSCSI、ftp 和 http protocal向客戶端提供檔案，儘管它只是為NFS和CIFS設計的，當許多客戶端使用這些協議與檔案的server 通信時，server對隨機讀取/寫入的需求會非常大。
* WAFL 用於帶有"用於寫入的NVRAM緩衝"的檔案server。
* 每個inode包含16個pointer指向blocks(或indirect block)，而由這些block組成的inode則用來描述檔案
* 所有的inode都存在單一個檔案當中，每個檔案系統都有一個root inode，其結構如下所示：
  * ![WAFL](https://github.com/a13140120a/Operation_System/blob/main/imgs/WAFL.jpg)
* snapshot(快照)：可以在不同的時間點創建檔案系統的多個"只讀(read only)"副本，當資料發生損毀的時候就可以藉由快照來還原成拍攝快照的時候當下的狀態。
  * 為了拍攝快照，WAFL 創建了root inode 的副本。 之後的任何檔案或metadata更新都會轉到新的block，而不是覆蓋其現有的block。
  * 如果某個block被修改的話，新的root inode 指向這些被寫入而更改的metadata和data。同時，快照（舊的root inode）仍然指向舊的block(類似copy on write的概念)，其結構如下所示：
    * ![WAFLsnapshts](https://github.com/a13140120a/Operation_System/blob/main/imgs/WAFLsnapshts.jpg)
  * 每個block會有一個一連串的bit組成的bitmap，只要有一個snapshot正在使用這個block的話，這個bitmap就不會為0，當這個bitmap為0的時候，該block會被釋放，就可以被reuse
* clone(克隆)：較新版本的WAFL 實際上允許讀寫快照，稱為clone
  * 只讀快照捕獲檔案系統的狀態，而clone則引用該快照，對克隆的任何寫入都存儲在新的block中，並且克隆的pointer會更新以指向新塊，原始快照未修改，跟更新克隆之前一樣。
  * 克隆對於測試和升級很有用，因為原始版本保持不變，並且在測試完成或升級失敗時刪除克隆。
* replication：
  * 通過網路將一組數據複製和同步到另一個系統。首先，將 WAFL 檔案系統的快照複製到另一個系統。當在源系統上拍攝另一個快照時，只需發送新快照中包含的所有block，就可以相對容易地更新遠程系統。



****




<h1 id="014">File-System Internals</h1> 

  * ## [File Systems](#0141) #
  * ## [File-System Mounting](#0142) #
  * ## [Partitions and Mounting](#0143) #
  * ## [File Sharing](#0144) #
  * ## [Virtual File Systems](#0145) #
  * ## [Remote File Systems](#0146) #
  * ## [Consistency Semantics](#0147) #



<h2 id="0141">File Systems</h2> 

* 一個通用計算機系統可以有多個存儲設備，這些設備可以分割成partition，這些partition保存volume，而這些partition又保存file system。有了volume manager，一個volume也可以跨越多個partition。下圖顯示了一個典型的檔案系統組織。
  * ![typical_storage_device_organization](https://github.com/a13140120a/Operation_System/blob/main/imgs/typical_storage_device_organization.PNG)
* 例如，一個典型的 Solaris 系統可能有幾十個不同類型的檔案系統，如下圖中的檔案系統列表所示。
  * ![sloaris_file_system](https://github.com/a13140120a/Operation_System/blob/main/imgs/sloaris_file_system.PNG)
* 實際上還有許多特殊用途的檔案系統。考慮上面提到的 Solaris 示例中的檔案系統類型： 
  * tmpfs：在易揮發性記憶體儲存裝置(NVMs)中創建的"臨時"檔案系統，如果系統重新啟動或崩潰，其內容將被刪除
  * objfs：一個"虛擬"的檔案系統（本質上是一個看起來像檔案系統的kernel interface），它使調試器(debugger)能夠訪問[kernel symbols(可以在`/proc/ksyms`或`/proc/kallsyms`中看到)](https://www.oreilly.com/library/view/linux-device-drivers/0596000081/ch02s03.html)
  * [ctfs](https://docs.oracle.com/cd/E88353_01/html/E37851/ctfs-4fs.html)：一個虛擬檔案系統，它維護"[contract](https://docs.oracle.com/cd/E88353_01/html/E37852/contract-5.html#REFMAN5contract-5) information"以管理哪些process在系統啟動時啟動並且必須在操作期間繼續運行
  * lofs：一種"[loop back](http://liujunming.top/2018/01/17/%E5%9B%9E%E7%8E%AF%E8%AE%BE%E5%A4%87loopback-device/)" file system 
  * procfs：為了讓kernel debug的時候能夠比較方便，而創造出來的虛擬的檔案系統，procfs是"行程檔案系統 (process file system)"的縮寫，這個檔案系統通常被掛載到 `/proc` 目錄。由於 `/proc` 不是一個真正的檔案系統，它也就不占用儲存空間，只是占用有限的記憶體。
    * 用於通過核心存取process資訊，可以向user呈現kernel中的一些訊息，也可以當作一種從user space 向kernel發送訊息的管道，
    * 此外這些特殊檔案中，大多數建立的時間及日期屬性是當前系統時間和日期。
    * 事實上 ps, top這些SHELL命令就是從proc檔案系統中讀取信息的。[詳細command wiki](https://zh.wikipedia.org/wiki/Procfs#Linux)
  * ufs, zfs：通用的檔案系統。


<h2 id="0142">File-System Mounting</h2> 

* 檔案系統必須先掛載，然後才能供系統上的process使用，而目錄結構可以由多個包含檔案系統的volume構建而成，掛載(mount)這些volume才可以使其在檔案系統命名空間中可用。 

* mount point：file structure中要附加檔案系統的位置，通常，mount pointer必須是一個空目錄。
  * 例如，在 UNIX 系統上，包含user的home目錄的檔案系統可能掛載為 /home， 然後，要訪問該檔案系統中的目錄結構，我們可以在目錄名稱前加上 /home，如 /home/jane
  * 這麼一來如果在 /users 下掛載該檔案系統將產生路徑名 /users/jane，我們可以使用它來訪問同一目錄。 
* OS會驗證device是否包含有效的檔案系統(藉由驗證該目錄是否具有預期的格式)，然後OS在其目錄結構中註明該檔按系統安裝在指定的安裝點，使OS能夠遍歷其目錄結構，在檔案系統之間切換。
* masOS每當系統第一次遇到新的disk時（無論是在開機時還是在系統運行時），macOS都會在設備上搜索檔案系統。如果找到，它會自動將檔案系統掛載到 /Volumes 目錄下，並添加一個標有檔按系統名稱的文件夾圖標。然後，用戶可以單擊該圖標，從而顯示新安裝的檔案系統。
* Windows 系列OS maintains了一個"extended two-level directory structure"，幫每個device和volume分配了一個drive letter(例如C槽)，並且採用`drive- letter:∖path∖to∖file`類似這樣的路徑格式。
* UNIX，mount 命令是顯式(explicit)的。在開機時自動掛載的系統設備和mount point list，但其他掛載可以手動執行。


<h2 id="0143">Partitions and Mounting</h2> 

* 每個disk可以有很多個partitions，每個volume也可以有很多的partitions或disks，取決於OS或volume management的software。
* 一個partition如果是raw的，代表他沒有檔案系統在上面，swap space使用自己的格式，並且不適合用於有檔案系統的partition，大部分的database也選擇使用raw partition來增強自己的效能。
* 如果一個partition包含了bootable的partitino的話，那麼該partition也必須要包含boot information，這個boot information會有自己的格式，通常會是一連串連續的blocks，然後會從預定義的位置(譬如第一個bytes)load 到memory當中，而這一連串的blocks包含了一個image(映像檔)也就是我們所謂的bootstrap loader。
* 有些系統可以dual-booted，使用這種技術通常boot loader會知道哪些partition有哪些OS或File system(且就是所謂的 *root partition* )，如果boot loader沒辦法解讀file system的格式的話，那麼存在這個file system裡面的OS就沒辦法被啟動，這也就是為什麼某些OS不支援某些檔案系統的原因。
* boot loader會選擇一個有OS的partition來開機
* 當mount的時候OS會透過driver去檢查該該device是否包含有效的檔案系統格式。
* 如果格式無效，則必須檢查partition的consistency 並進行更正。
* 最後，OS會在其in-memory mount table記錄已掛載的檔案系統以及檔案系統的類型。
* Windows 將每個 volume掛載在單獨的名稱空間中，由字母和冒號表示，當一個process指定driver letter時，OS會找到適當的檔案系統指針並遍歷該設備上的目錄結構以找到指定的檔案或目錄。更高版本的 Windows 可以在現有目錄結構中的任何位置掛載檔案系統。
* 在 UNIX 上，檔案系統可以掛載在任何目錄中，掛載是通過在該目錄的in-memory的inode的copy中設定一個flag來實現的。
  * 該flag表明該目錄是一個mount point。然後會有一個field指向mount table中的一個entry，來記錄這個mount point掛載了哪個device。
  * mount table entry包含一個指向該設備上檔案系統[superblock](#0132)的pointer，該方案使OS能夠遍歷其目錄結構，在不同類型的檔案系統之間無縫切換。 

<h2 id="0144">File Sharing</h2> 

* 為了實現共享和保護，系統必須維護比單用戶系統所需的更多的檔案和目錄屬性。
* 大多數係統已經演變為使用檔案（或目錄）owner（或user）和group的概念。
* 給定檔案（或目錄）的owner和group ID 與其他檔案屬性一起存儲。當用戶請求對檔案進行操作時，可以將user ID 與owner屬性進行比較，以確定請求user是否是檔案的owner。同樣，可以比較Group ID。
* 如果有一個可以在系統之間移動的外部硬碟出現的話，當設備在它們之間移動時，必須注意確保系統之間的 ID 匹配，或者在發生此類移動時重置檔案所有權。


<h2 id="0145">Virtual File Systems</h2> 

* 每個檔案系統實現細節的方法都不相同，為了將基本system call的功能與細節隔離開來。因此，檔案系統實現由三個主要層組成，如下圖所示。 
* ![Virtual_FS](https://github.com/a13140120a/Operation_System/blob/main/imgs/Virtual_FS.jpg)
* 第一層是file system interface，base on `open()`、`read()`、`write()` 和 `close()` 以及檔案描述符。
* 第二層稱為虛擬檔案系統（VFS, virtual file system layer）層。 VFS 層有兩個重要功能：
  * 會有一個通用的interface來實現各種不同的細節。
  * 它為整個空間中的檔案提供了唯一標識符 (vnode)，包括橫跨所有不同類型的檔案系統（UNIX inode 僅在單個檔案系統中是唯一的。），kernel為每個activity node（檔案或目錄）maintain一個 vnode 結構。 
* VFS使用local file system來處理local requests，並且以呼叫NFS protocol procedures來處理remote的file system
* 檔案句柄由相關的 vnode 構成，並作為參數傳遞給這些prodedures。

* Linux VFS：
  * 四種主要的object types：
    * inode object：代表單個文件
    * file object：代表打開的文件
    * superblock object：代表整個file system
    * dentry object：代表一個directory entry
  * 對於這四種object types，每種類型的每個object都會有pointer指向一個 *function table* ，function table紀載了真正實現function的實際位址，
  * 例如，檔案對象的一些操作的縮寫 API 包括：
    * `int open(...)`：打開一個檔案。
    * `int close(...)`：關閉一個已經打開的檔案。
    * `ssize_t read(...)`：從檔案中讀取。
    * `ssize_t write()`：寫入檔案。
    * `int mmap(...)`：memory mapped file。 
  * 對於file object操作的完整定義在`struct file_operations`中定義(位於/usr/include/linux/fs.h)

<h2 id="0146">Remote File Systems</h2> 

* 前言：
  * 隨著網絡和檔案技術的發展，遠端檔案共享方式發生了變化。第一個實現的方法涉及通過諸如 ftp 之類的程式在機器之間手動傳輸檔案。 第二種主要方法使用分散式檔案系統 (DFS)，其中遠端目錄可從本地計算機看到。
  * 在某些方面，第三種方法，即World Wide Web，是對第一種方法的回歸。 需要瀏覽器來訪問遠端文檔案，並且使用單獨的操作（本質上是 ftp 的包裝）來傳輸檔案。

  * ftp 用於匿名訪問(Anonymous access)和經過身份驗證的訪問。匿名訪問允許用戶在沒有遠程系統帳戶的情況下傳輸檔案，World Wide Web幾乎完全使用匿名文件交換。
  * DFS 涉及訪問遠端檔案的機器和提供檔案的機器之間更緊密的集成。


* The Client– Server Model：
  * server聲明一個資源對client可用，並準確指定哪個資源（在這種情況下，是哪些檔案）以及哪些client。
  * 一個server可以為多個client提供服務，一個client可以使用多個server，這取決於實作細節。 
  * server通常在volume或目錄級別指定可用檔案，client可以通過網絡名稱或其他標識符（例如 IP 地址）來指定，但這些都可以被欺騙或模仿。
  * 一個安全的解決方案是通過加密密鑰對客戶端進行安全身份驗證。不幸的是，安全帶來了許多挑戰，包括確保客戶端和server的兼容性（它們必須使用相同的加密算法）和密鑰交換的安全性（再次截獲密鑰可能再次允許未經授權的訪問）。
  * 由於解決這些問題的難度，不安全的身份驗證(unsecure authentication)方法是最常用的。
  * 一旦遠端檔案系統被掛載，檔案操作的請求就會代表用戶通過網絡通過 DFS 協議發送到server。
  * 通常請求會與請求用戶的 ID 一起發送。
  * 如果請求被允許，則將檔案句柄返回給客戶端應用程式。

* Failure Modes：(不太能夠理解原文在說什麼)
  * 系統可以終止對崩潰的伺服器(server)的所有操作或延遲操作，直到伺服器再次可訪問。這些故障語義被定義和實現為遠端檔案系統協議的一部分。終止所有操作可能會導致用戶丟失數據和耐心。
  * 為了實現這種故障恢復，可以在客戶端和伺服器上維護某種狀態信息(state information )。在伺服器崩潰但必須識別它已遠程掛載導出的檔案系統並打開檔案的情況下，NFS 版本 3 採用了一種簡單的方法，實現了 *stateless DFS* (無狀態 DFS)。
  * 除非檔案系統已遠程掛載並且檔案先前已打開，否則不允許發生客戶端讀取或寫入檔案的請求。 NFS 協議攜帶定位適當檔案和執行請求操作所需的所有信息，但此種方法會缺乏安全性。


<h2 id="0147">Consistency Semantics</h2> 

* 一致性語義(Consistency Semantics)代表了評估任何支援檔案共享的檔案系統的重要標準。這些語義(semantics)指定係統的多個用戶如何同時訪問共享檔案。
* Consistency Semantics通常會直接在檔案系統implement。
* 我們假設用戶嘗試對同一檔案進行的一系列訪問（即讀取和寫入），而所有的操作都在 open()之後和 close()之前，那麼open() 和 close() 操作之間的一系列訪問構成了一個檔案的 *session(會話)*。
* UNIX file system使用下列consistency semantics：
  * 使用者對打開的檔案的寫入對打開該檔案的其他使用者立即可見。
  * 一種共享模式允許使用者將指向當前位置的pointer共享到檔案中，因此，如過一個使用者的pointer前進會影響所有共享的使用者。
* Andrew file system (OpenAFS) 使用下列consistency semantics：
  * 使用者對打開的檔案的寫入不會立即對打開相同檔案的其他使用者可見。
  * 關閉檔案後，對其所做的更改僅在稍後開始的session中可見。已打開的檔案實例不反映這些更改。 
* Immutable-Shared-Files Semantics：
  * 不可變的共享檔案，一旦一個檔案被它的creator宣告為共享的，它就不能被修改。
  * 不可變檔案有兩個關鍵屬性：其名稱不可重複使用，其內容不可更改。
  * 因此，不可變檔案的名稱表示檔案的內容是固定的。 
  * 這些語義在分散式系統中的實現很簡單，因為共享是有規律的（只讀）。 


<h2 id="0148">NFS</h2> 

* NFS 是一個廣泛使用、良好實現的客戶端-服務器網絡檔案系統的一個很好的例子，NFS 有多個版本，最新的是版本 4。以下我們描述版本 3。
* NFS 將一組互連的工作站視為一組具有獨立檔案系統的獨立機器。目標是允許這些檔案系統進行某種程度的共享。共享base on client– server 關係。 一台機器可能而且經常既是客戶端又是服務器。
* NFS 的設計目標之一是在不同機器、OS和網絡架構的異構環境中運行，NFS 規範獨立於這些媒體。這種獨立性是通過[使用外部數據表示 (external data representation, XDR)](#0033) 協議之上構建的 RPC primitive來實現的。


****




<h1 id="015">Security</h1> 

  * ## [The Security Problem](#0151) #
  * ## [Program Threats](#0152) #
  * ## [System and Network Threats](#0153) #
  * ## [Cryptography as a Security Tool](#0154) #




<h2 id="0151">The Security Problem</h2> 

* 系統的安全違規（或濫用）可分為惡意或意外，防止意外誤用比防止惡意誤用更容易。在大多數情況下，保護機制是避免事故的核心，以下列表包括幾種形式的意外和惡意行為：
  * Breach of confidentialit(機密的缺口)：這種類型的違規涉及未經授權的數據讀取（或信息盜竊）。通常，洩露機密是入侵者的目標。從系統或數據流中捕獲秘密數據，例如用於身份盜用的信用卡信息或身份信息，或未發布的電影或腳本，可能會直接為入侵者帶來金錢，並使被駭客入侵的機構感到尷尬。
  * Breach of integrity(完整的缺口)：這種違規行為涉及未經授權的數據修改。例如，此類攻擊可能導致將責任轉移給無辜方或修改重要商業或開源應用程序的源代碼。
  * Breach of availability(可用的缺口)：這種違規行為涉及未經授權的數據破壞。一些攻擊者寧願大肆破壞並獲得地位或吹噓的權利，也不願獲得經濟利益。網站污損是此類安全漏洞的常見示例。
  * Theft of service(服務盜竊)：此違規行為涉及未經授權使用資源。例如，入侵者（或入侵程序）可能會在充當文件服務器的系統上安裝守護程序。
  * Denial of service(DoS, 拒絕服務)：此違規行為涉及阻止系統的合法使用。拒絕服務 (DOS) 攻擊有時是偶然的。最初的 Internet worm(網路蠕蟲病毒) 在一個 bug 未能延緩其快速傳播時變成了 DOS 攻擊。

* 攻擊者在嘗試破壞安全時使用了幾種標準方法。最常見的是 *偽裝(masquerading)* ，其中一個交流參與者假裝是另一個人（或另一台電腦）。
* 另一種常見的攻擊是repeat捕獲的數據。 *重放攻擊(replay attack)* 包括對有效數據傳輸的惡意或欺詐性重複。有時重播包括整個攻擊，例如：重複轉賬請求。
* 另一種攻擊是 *中間人攻擊(man-in-the-middle attack)* ，其中攻擊者位於通信的數據流中，偽裝成接收者的發送者，反之亦然。在網絡通信中，中間人攻擊之前可能會發生 *會話劫持(session hijacking)* ，其中活動的通信會話被攔截。 
* 另一類廣泛的攻擊旨在* 提升權限(privilege escalation)* ，特權升級為攻擊者提供了比他們應該擁有的更多的特權。例如，包含執行的script或macro的電子郵件超出了電子郵件發件人的權限。上面提到的偽裝和消息修改通常用於提升權限。
* 絕對保護系統免受惡意濫用是不可能的，但是可以使肇事者的成本足夠高以阻止大多數入侵者。

* 為了保護一個系統，我們必須在四個層面上採取安全措施： 
  * Physical：對包含計算機系統的站點進行物理保護，以防止入侵者進入。
  * Network：大多數當代計算機系統(從伺服器到移動設備再到物聯網(IoT)設備)都是聯網的。網路為系統提供了一種訪問外部資源的方法，但也為未經授權訪問系統本身提供了潛在的載體，此外，現代系統中的計算機數據經常通過專用租用線路(private leased lines)、共享線路（shared lines，如 Internet）、無線連接(wireless connections)和撥號線路(dial-up lines)傳輸。
  * Operating system：作業系統及其內置的應用程式和服務集構成了一個龐大的代碼庫，其中可能包含許多漏洞。不安全的預設設置、錯誤配置和安全漏洞等等潛在問題。因此，作業系統必須保持最新（通過持續修補）和強化配置和修改以減少攻擊面並避免滲透。 *attack surface* 是攻擊者可以嘗試侵入系統的一組點。
  * Application：第三方應用程式也可能帶來風險，尤其是在它們擁有重要特權的情況下。一些應用程式本質上是惡意的，但即使是良性應用程式也可能包含安全漏洞。由於大量第三方應用程式及其不同的代碼庫，幾乎不可能確保所有此類應用程式都是安全的。 
  * ![four_layered_model_of_security](https://github.com/a13140120a/Operation_System/blob/main/imgs/four_layered_model_of_security.PNG)
* 還有一種攻擊手法稱為 *social engineering* ，social engineering使用欺騙手段說服人們放棄機密信息，social engineering的其中一個例子是網絡釣魚(phishing)，其中看起來合法的電子郵件或網頁會誤導用戶輸入機密信息，只需單擊瀏覽器頁面或電子郵件中的鏈接即可無意中下載惡意負載，而通常那台 PC 不是最終目標，而是一些更有價值的資源。從該受感染的系統開始，對 LAN 上的其他系統或其他用戶的攻擊隨之而來。 



<h2 id="0152">Program Threats</h2> 

* 入侵者常常會在入侵之後留下一個後門守護程序(back-door daemon)或遠程訪問工具（RAT, Remote Access Tool），即使原始漏洞被阻止，它也能提供信息或權限讓入侵者輕鬆訪問，以下將描述程式導致安全漏洞的常見方法。

* Malware：
  * 惡意軟件是旨在利用、禁用或破壞計算機系統的軟件。
  * 特洛斯木馬(Trojan horse)：程式於提供使用者存取權限的領域中執行，其他使用者可能會濫用這些權限，以秘密或惡意的方式進行，稱為木馬程式，這種程式宣稱提供一些良性功能(例如手電筒應用程式)，但同時也秘密的存取使用者的聯絡人訊息。
  * 特洛伊木馬的一個經典的變型，是模仿登入頁面(或程式)的"Trojan mule"，使用者於偽造的登入頁面上登入，第一次登入時會顯示輸入密碼錯誤，再次嘗試時才會成功，"假的登入畫面"把密碼存起來，顯示出登錄錯誤信息，退出，然後向使用者提供真正的登錄畫面。此時使用者的身分驗證密鑰以及密碼已經被攻擊者竊取。
  * 這種類型的攻擊可以通過讓OS在交互式會話(interactive session)結束時顯示使用資訊來破解，方法是要求一個不可捕獲的鍵序列來進入登錄畫面(例如Windows使用的 control-alt-delete 組合)。
  * 特洛伊木馬的另一個變形是spyware(間諜軟體)，這種軟體通常會附加在一些免費軟體或共享程式上(甚至有些會附加在商業軟體中)，這些間諜軟體可能會下載廣告，在存取某些節點時彈出瀏覽器視窗，或從使用者的系統捕獲資料，在windows上安裝看似無害的程式，可能會導致加載間諜軟體，取得受害者訊息及收件人位址列表，並從受害者的windows發送垃圾郵件，過程將一直持續直到受害者發現為止，而且百分之90通常都不會被發現，在大多數國家/地區，這種盜竊行為甚至不被視為犯罪。
  * 最近出現的的勒索軟體(Ransomware)會加密目標電腦上的部份或全部資料，藉以強迫使用者支付贖金，當然並不保證可以返回存取權。
  * 最小特權原則(The principle of least privilege)是指系統中的每個程式和每個特權使用者都應該使用完成工作所需最少特權來運作，在違反最小特權原則的情況下，會造成木馬及其他惡意軟體猖獗，Windows7 以及以下的版本使用者都是預設以管理員身分運作，這種情況下，作業系統本身的保護能力會沒有辦法啟動。
  * 另一種形式的惡意軟體中，設計者會在軟體中留下一個漏洞，只有設計者本身才能使用，稱為暗門（trap door）或後門(back door)，以便之後利用。
    * 使用back door的設計師可能偷偷地四捨五入錯誤，偶爾將半美分記入其帳戶來到用銀行，考慮到大型銀行執行的交易數量，此帳戶可能會增加大量金錢。
    * 暗門可以設定為僅在一組邏輯條件下運作，這種情況稱為「邏輯炸彈(logic bomb)」，這種類型的後門非常難以檢測，因為他們在被檢測到之前(通常是已經遭到破壞之後)，可以休眠長達數年，例如當一名網管人員的程式偵測到他不再受雇於該公司時，對其公司網路進行破壞性重置。
    * 聰明的暗門會包含在編譯器裡面，如以一來不管原始碼如何，都無法被搜尋到任何問題，2015年針對蘋果公司XCode編譯器套件(XCodeGhost)的惡意軟體，影響了所有未直接由Apple官方下載的XCode的軟體開發人員。
    * 要檢測到暗門必須對系統所有組件的所有程式碼進行檢查，這可能包含幾百萬行的程式碼，因次可以用「程式碼檢查(code review)」的機制來解決此漏洞問題，編寫程式碼的開發人員將其提交給程式碼資料庫，審查人員批准並提供註解，一旦程式碼被批准，該程式將被收納到程式碼資料庫進行編譯。
    * 一位好的程式開發人員通常是最好的程式碼審查者。

* Code Injection(程式碼注入)：
  * 大多數軟體並非惡意軟體，但由於程式碼注入攻擊(code-injection attack)，在其中修改或增加可執行程式碼，仍然可以對安全性構成嚴重威脅。
  * 程式碼注入攻擊幾乎都是由於不良或不安全的程式碼所造成的，通常發生在低階語言(例如C/C++)，他們允許透過指標進行直接記憶體存取，如果未正確處理緩衝區，可能會造成記憶體損壞。
  * 例如[緩衝區溢位](https://github.com/a13140120a/Operation_System/blob/main/overflow.c)就是一個是最簡單的例子，對於`strcpy()`函數不考慮所涉及的緩衝區大小而複製，僅在遇到"\0"(NULL)的時候才停止，溢位的結果取決於意味的長度和溢位的內容，因為編譯器會最佳化程式碼，因此對記憶體佈局的調整也會不同。
    * overflow造成core dump的執行結果：
      * ![overflow_coredump](https://github.com/a13140120a/Operation_System/blob/main/imgs/overflow_coredump.PNG)
    * 如果溢位非常小，則可能沒有任何影響，這是因為BUFFER_SIZE位元組的分配通常會填充(padding)到該架構指定的邊界(boundary, 通常為8或16位元)，padding是未使用的記憶體，因此從技術上來說，雖然有溢位發生，但沒有超出範圍，所以不會造成影響。
    * 如果溢位超出padding的範圍，則堆疊上的下一個自動變量會被溢位內容覆寫
    * 如果溢位大幅度超過padding的範圍的話，則當前的stack frame 都會被覆蓋掉，在frame 的最頂端的記憶體位址是函數的return address，攻擊者可以藉由覆寫該address來將程式導向其他的地方，包括攻擊者可以控制的記憶體範圍，然後執行程式碼注入，就可以以有效的process ID執行任意程式碼。
    * ![posible_outcome_of_buf_overflow](https://github.com/a13140120a/Operation_System/blob/main/imgs/posible_outcome_of_buf_overflow.PNG)
    * 如上述例子可見，容易被攻擊的函數包括`strcpy()`、`sprintf()`、`gets()`等等
    * 但即便是"size-aware"(會檢查size)的變數，在有限長度的整數運算時也可能存在漏洞，導致整數溢位。
    * [smashing the stack for fun and profit](http://phrack.org/issues/49/14.html)
  * [程式碼注入shellcode詳解](https://medium.com/@ktecv2000/%E7%B7%A9%E8%A1%9D%E5%8D%80%E6%BA%A2%E4%BD%8D%E6%94%BB%E6%93%8A%E4%B9%8B%E4%BA%8C-buffer-overflow-b0a33d43ba1d)：
  * ```c
    void func (void) {
        execvp(“/bin/sh”, “/bin/sh”, NULL);
    }
    ```
  * 使用 `execvp()` system call，此程式碼段創建一個 shell process。 如果被攻擊的程序以 root 權限運行，這個新創建的 shell 將獲得對系統的完全訪問權限。 當然，code segment可以做被攻擊進程權限允許的任何事情。 code segment接下來被編譯成它的彙編二進制操作碼形式，然後轉換成binary stream。 編譯後的形式通常被稱為 shellcode，因為它具有生成 shell 的經典功能，但該術語已發展到涵蓋任何類型的代碼，包括用於將新用戶添加到系統、重新啟動甚至連接的更高級代碼 通過網絡等待遠端指令（稱為"reverse shell"）。
  * 此類攻擊可能發生在預期用於與目標機器通信的協議中，因此很難檢測和預防，他們甚至可以繞過防火牆
  * 當溢出發生在 heap 中時，它們也可以被利用。在memory buffer被釋放之後繼續使用，以及過度釋放它們（調用 `free()` 兩次），也可能導致代碼注入。 


* Viruses and Worms：
  * 病毒跟蠕蟲(worm)的區別是，病毒需要人類的活動，蠕蟲可以靠網路進行複製，蠕蟲還會通過電子郵件將自己發送給用戶聯繫人列表中的其他人。
  * 病毒是由自身複製並設計成可以感染其他程式，其在系統中造成的破壞包括修改、破壞檔案，造成系統損毀和程式不正常動作。
  * 病毒非常特定於架構、作業系統和應用程式(移植性差)。
  * UNIX 和其他multiuser作業系統通常不易感染病毒，因為可執行程式受到作業系統的保護，不會被寫入。即使病毒確實感染了這樣的程式，它的能力通常也是有限的，因為系統的其他方面受到保護。
  * 有關病毒如何感染主機的示例：考慮 Microsoft Office 檔案。 這些文件可以包含 Office 套件（Word、PowerPoint 和 Excel）中的程序將自動執行的macros(巨集，大陸稱宏)（或 Visual Basic 程序），由於這些程式在使用者自己的帳戶下運行，因此巨集可以在很大程度上不受限制地運行（例如，隨意刪除檔案）。以下程式碼示例顯示了編寫 Visual Basic 巨集是多麼簡單，一旦打開包含巨集的檔案，蠕蟲就可以使用該巨集來格式化 Windows 計算機的硬碟：
  * ```c
    Sub AutoOpen()
    Dim oFS
    Set oFS = CreateObject(”Scripting.FileSystemObject”)
    vs = Shell(”c: command.com /k format c:”,vbHide)
    End Sub
    ```
  * 一旦病毒達到目標機器，一種稱為virus dropper(病毒注射器)的program就會在系統中注入病毒，virus dropper本身也是一個木馬程式，一旦病毒被載入就可以任意破壞，實際上有數千種病毒，但它們分為幾個主要類別：
    * 檔案(file)：病毒改變程式的起始，當程式執行時，跳轉到病毒的程式碼，等到病毒的程式碼執行完畢之後，在跳轉回到原本的程式。所以病毒的執行並不會引起人的注意，病毒檔案又稱為寄生病毒，因為他們其實沒有留下完整的檔案，但主程式依然具有功能。
    * 開機(boot)：開機病毒感染boot partition，於每次系統啟動和作業系統載入前執行，這些病毒也稱為記憶體病毒，因為他們不出現在檔案系統，下圖顯示一個開機病毒如何運作，開機病毒同樣可以感染韌體，例如網卡PXE和可擴展韌體接口(EFI)環境。
      * ![boot_sector_computer_virus](https://github.com/a13140120a/Operation_System/blob/main/imgs/boot_sector_computer_virus.PNG)
    * 巨集(marco)：大部分病毒以低階語言撰寫(例如組合語言或C語言)，巨集病毒以高階語言撰寫，如 Visual Basic ，當程式可以執行巨集時，病毒被觸發，例如巨集病毒可以藏在execel裡。
    * Rootkit：最初是為了描述UNIX系統提供能簡單從root存取權限的後門而建立的，現在的定義擴展為滲透到作業系統本身的病毒和惡意軟體，當rootkit感染作業系統時，可以接管系統所有的功能，包括自我偵測的能力，所以系統的任何方面都是不可信任的。
    * 原始碼(source code)：原始碼病毒會尋找原始碼並修改原始碼，使原始碼包含病毒，並幫助傳播。
    * 千面人病毒(Polymorphic)：千面人病毒可怕的地方，在於每當它們繁殖一次，就會以不同的病毒碼傳染到別的地方去。每一個中毒的檔案中，所含的病毒碼都不一樣，對於掃描固定病毒碼的防毒軟體來說，無疑是一個嚴重的考驗！如Whale病毒依附於.COM檔時，幾乎無法找到相同的病毒碼，而Flip病毒則只有2 byte的共同病毒碼（好像戴面具只剩兩個眼睛露出來）。
    * 加密(encrypted)：加密病毒包括解密程式與加密病毒一起，加密可以避免被偵測到，病毒執行前會先解密
    * 隱形(stealth)：此類病毒會企圖修改用來偵測病毒的系統部分來避免被偵測，例如他可以修改`read()`system call，這樣如果被修改的檔案被讀取，他只會返回修改前的程式碼，而不是被修改過後的程式碼
    * 複合型病毒(Multi-Partite Virus)：複合型病毒兼具開機型病毒以及檔案型病毒的特性。它們可以傳染 *.COM, *.EXE 檔，也可以傳染磁碟的開機系統區(Boot Sector)。由於這個特性，使得這種病毒具有相當程度的傳染力。一旦發病，其破壞的程度將會非常可觀！例如：台灣曾經流行的大榔頭(Hammer)，歐洲流行的Flip翻轉病毒皆是。
    * 裝甲(armored)：裝甲病毒由於經過程式碼混淆，被編寫成讓反病毒研究人員難以解開和理解。它也可以被壓縮以避免檢測和刪除。此外，virus dropper和其他完整檔案(的病毒)通常通過檔案屬性或不可見的檔名隱藏。
    * [病毒種類詳細說明](https://www.eduhk.hk/has/comp/compviru/class.htm#top)

<h2 id="0153">System and Network Threats</h2> 

* 程序威脅本身就構成嚴重的安全風險。 但是，當系統連接到網絡時，這些風險會增加幾個數量級。
* secure by default：例如，Solaris 10 從在安裝系統時預設啟用許多服務（FTP、telnet 等）的模型轉變為在安裝時禁用幾乎所有服務並且必須由系統管理員啟用。此類更改減少了系統的attack surface(即攻擊的方法數量)。
* zombie systems：獨立系統或設備已被駭客入侵，但仍繼續為其所有者服務，同時在所有者不知情的情況下被用於惡意目的，包括拒絕服務攻擊(DoS)和垃圾郵件中繼(spam relay)。
* 無線乙太網使攻擊者可以通過匿名加入公共網絡或"WarDriving"(戰爭駕駛?)來輕鬆發起攻擊——透過私人不受保護的網路來定位。
* Attacking Network Traffic(網路流量攻擊)：
  * sniffin(嗅探)：攻擊者可以選擇保持被動並攔截網絡流量，通常會獲取有關係統之間進行的會話類型或會話內容的有用信息。
  * spoofin(欺騙)攻擊者可以扮演更積極的角色，或者偽裝成一方，或者成為一個中間人，攔截並可能修改雙方之間的交易，如下圖所示：
  * ![spoofing](https://github.com/a13140120a/Operation_System/blob/main/imgs/spoofing.PNG)
* Denial of Service(DoS, 拒絕服務)：
  * 發起阻止合法使用的攻擊通常比闖入系統或設施更容易。 
  * 它們分為兩類：
    * 第一類攻擊占用系統絕大部分的資源，使系統上的所有資源變得不可用，例如，從網站點擊可以下載 Java 小程式，該小程式繼續使用所有可用的 CPU 時間或無限彈出窗口。
    * 第二類涉及破壞設備的網路，此類攻擊可能持續數小時或數天，導致使用目標設施的嘗試部分或全部失敗。攻擊通常會癱瘓目標網路，直到作業系統可被更新以降低他們的傷害。
  * 通常Dos攻擊無法被防止，更難預防和解決的是分散式拒絕服務 (DDoS, Distributed Denial-of-Service) 攻擊，這些攻擊利用多台殭屍電腦像一個目標發動攻擊，有時一個站點甚至不知道它正在受到攻擊。 很難確定係統減速是攻擊還是系統使用激增。
  * 如果某個身份驗證演算法在多次錯誤地嘗試訪問該帳戶後將其鎖定一段時間，那麼攻擊者可能會通過故意不正確地嘗試訪問所有帳戶來阻止所有身份驗證，亦可以誘導自動阻止某些類型的流量的防火牆在不應該阻止該流量時阻止該流量。這些例子表明程式設計師和系統管理員需要充分理解他們正在部署的演算法和技術。 
* Port Scanning(連接埠掃描)：
  * Port Scanning本身並不是一種攻擊，而是駭客檢測系統攻擊漏洞的一種手段。（安全人員也用來檢測不需要或不應該運行的服務。）
  * 有一些工具可以執行完整掃描功能。例如，[nmap](https://nmap.org/)，當它指向目標時，它將確定正在運行的服務，包括應用程式名稱和版本，它還可以識別主機作業系統，還可以提供有關防禦的資料，例如哪些防火牆在保護目標
  * 其他工具例如Metasploit則在連接埠掃描程式離開的地方進行讀取
  * 其他有關[Port Scanning的技術文章](http://phrack.org/issues/49/15.html#article)


<h2 id="0154">Cryptography as a Security Tool</h2> 

* 建立一個可以信任數據包的來源地址和目標地址的網路通常被認為是不可行的，因此，唯一的取代方法就是在某種程度上消除信任網路的需求，這就是密碼學(cryptography)的工作。
* 現代密碼學建立於密鑰(key)之上，發送者可以對其消息進行編碼，以便只有具有特定密鑰的計算機才能解碼該消息，密鑰的設計使得從它們用於生成的消息或任何其他公共信息中推導出它們在計算上是不可行的。因此，它們提供了一種更可靠的方式來約束消息的發送者和接收者。 
* 加密(encryption)：
  * 加密用來限制receiver(防止有人假裝成receiver)
  * 加密算法由以下組件組成： 
    * 鑰匙的集合K
    * 明文的集合M
    * 密文的集合C
    * 加密函數 E：K → (M → C)。 也就是說，對於每個 k ∈ K，Ek 是一個從明文中生成密文的函數。對於任何 k而言，E、Ek 都應該是有效的可計算函數。 通常，Ek 是從消息到密文的隨機映射。
    * 解密函數D：K → (C → M)。 也就是說，對於每個 k ∈ K，Dk 是一個從密文生成消息的函數。對於任何 k而言D、Dk 都應該是有效的可計算函數。 
  * 當給予密文c ∈ C時，只有當電腦擁有k時才能計算出m，且Ek(m)=c。
  * 加密演算法又可以分為對稱及非對稱：
    * 對稱加密演算法(Symmetric-key algorithm)：
      * 使用相同的鑰匙來加密和解密
      * 鑰匙在發送者與接送者之間交換
      * 過去幾十年，美國民間應用上最普遍的對稱加密演算法曾經是NIST(National Institute of Standards and Technology)所採用的DES(data-encryption standard, 資料加密標準)，DES採用64位元值和56位元鑰匙，DES一次處理一個由 "一連串bits組成的block" ，因此又被稱為區塊加密(block cipher)，如果使用相同的密鑰來加密大量數據，它就容易受到攻擊，而使用block cipher，如果使用相同的密鑰來加密大量數據，它就容易受到攻擊。
      * 使用DES是不安全的，因為他的鑰匙有可能被找出來，因此出現了三重DES(triple DES)，DES演算法被重複做三次，兩次加密和一次解碼，例如c = Ek3(Dk2(Ek1(m)))。
      * 後來DES被AES(advanced encryption standard，又稱為Rijndae，)所取代，並以成為標準，[詳情請見](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf)
      * block cipher本身是不安全的加密技術，尤其是他們不處理比他們規定還要常的區塊
      * 串流加密(stream cipher)被設計用於加密和解密位元組串流或位元，密鑰被輸入到"偽"隨機位生成器中，這是一種嘗試生成隨機位的演算法。輸入密鑰時生成器的輸出是keystream，keystream是一組無限位，可用於通過 XOR 操作加密明文stream。
    * 非對稱加密演算法(Asymmetric Encryption)：
      * 在非對稱加密算法中，加密和解密使用的是不同的鑰匙，準備接收加密通信的實體創建兩個密鑰，並將其中一個（稱為公鑰）提供給任何想要它的人。 任何發送者都可以使用該密鑰來加密通信，但只有密鑰創建者才能解密通信。
      * 其流程可以概述如下：
        * 愛麗絲與鮑伯事先互不認識，也沒有可靠安全的溝通管道，但愛麗絲現在卻要透過不安全的網際網路向鮑伯傳送資訊
        * 愛麗絲撰寫好原文，原文在未加密的狀態下稱之為明文 x
        * 鮑伯使用密碼學安全偽亂數生成器產生一對密鑰，其中一個作為公鑰為 c，另一個作為私鑰 d
        * 鮑伯可以用任何方法傳送公鑰 c 給愛麗絲，即使伊夫在中間竊聽到 c 也沒問題
        * 愛麗絲用公鑰 c 把明文 x 進行加密，得到密文 c(x)
        * 愛麗絲可以用任何方法傳輸密文 c(x) 給鮑伯，即使伊夫在中間竊聽到密文 c(x) 也沒問題
        * 鮑伯收到密文，用私鑰 d 對密文進行解密 d(c(x)) ，得到愛麗絲撰寫的明文 x
        * 由於伊夫沒有得到鮑伯的私鑰 d，所以無法得知明文 x
      * [RSA加密](https://ithelp.ithome.com.tw/articles/10215109)：
        * 找到[李永樂老師的視頻](https://www.youtube.com/watch?v=D_kMadCtKp8)覺得講解的很好。
* #### 認證(Authentication)：
  * 認證限制sender(防止有人假裝sender)
  * 認證演算法是由以下的元件所組成的：
    * 鑰匙的集合K
    * 訊息的集合M
    * 認證因子A(authenticators，即Sender的「所知道或所持有的事物」、或「所具備的特徵」，例如密碼、指紋、晶片、虹膜等等)
    * 函數 S：K → (M → A)。也就是說，對於每個 k ∈ K，Sk 是一個用於從消息中生成認證因子A的函數。對於任何 k而言，S 和 Sk 都應該是有效的可計算函數。
    * 函數 V : K → (M × A →{true, false})。也就是說，對於每個 k ∈ K，Vk 是一個用於驗證消息上的認證因子A的函數。對於任何 k而言，V 和 Vk 都應該是有效的可計算函數。 
  * 電腦可以產生一個認證因子a ∈ A，使得如果擁有k，則Vk(m,a) = true，因此擁有k的電腦可以產生認證因子a，也可以用來驗證a
  * k必須是不能從認證因子a導出來的。
  * 如果 Vk(m, a) = true，那麼我們知道 m 沒有被修改，並且消息的發送者有 k。
  * 因此，如果我們只與一台電腦共享 k，那麼我們就知道消息來自這台電腦。
  * 了解認證演算法之前必須要先知道雜湊函數(hash function)，
  * 認證有兩種主要不同的演算法， 
    * 第一種主要類型的身份驗證算法使用對稱加密(symmetric encryption)。
      * message-authentication code (MAC，消息驗證碼)：使用密鑰從message生成加密的checksum，該方法用來驗證較短的訊息
      * 雜湊函數(hash function)：雜湊函數會產生一個短且大小固定的資料區塊，稱為message digest(訊息摘要)或hash value(雜湊值)，常見的雜湊函數有輸出128位元雜湊值的MD5(現今已被認為是不安全的)，以及輸出160位元的雜湊值的SHA-1，這裡討論的雜湊函數必須避免碰撞，該方法適合用來驗證較長的訊息。
      * 計算 Sk 和 Vk 都需要 k，因此任何能夠計算其中一個的人都可以計算另一個。 
    * 第二種方法是使用digital-signature algorithm(數位簽章演算法)：
      * 此種方法產生的認證因子即稱為digital signatures(數位簽章)
      * 數位簽章可以讓任何人驗證訊息的真偽
      * 一個使用RSA數位簽章的例子，它和RSA加密的鑰匙是相反的(公鑰跟私鑰的演算法相反)
      * 程式碼簽章(Code signing)可以用來對程式碼進行數位簽章
      * 即使是公鑰的分發也需要一些小心。考慮中間人攻擊，想要接收加密消息的人發送了他的公鑰，但攻擊者也發送了他的"壞"公鑰（與她的私鑰匹配）。想要發送加密消息的人並不知道，因此使用壞密鑰對消息進行加密。攻擊者就可以輕鬆的解密它，數位簽章可以解決這個問題。

* Implementation of Cryptography：
  * 網路協定以階層式的方式組織，每一層對他的下面一層就像是一個client，例如，TCP(傳輸層)就像IP(網路層)的client等等。
  * 而密碼學可以用在任何一層上，
    * 例如，TLS在傳輸層提供了安全性
    * 而[IPSec](https://zh.wikipedia.org/wiki/IPsec) 則為網絡層提供了身份驗證器和加密封包的功能，IPSec常被用於[虛擬私人網路(virtual private network，VPN)](https://zh.wikipedia.org/wiki/%E8%99%9B%E6%93%AC%E7%A7%81%E4%BA%BA%E7%B6%B2%E8%B7%AF)當中。
  * 密碼學應用在較低層的話可以提供較多的保護(會連同上面層一起加密)。
* Transport Layer Security ([TLS](https://zh.wikipedia.org/wiki/%E5%82%B3%E8%BC%B8%E5%B1%A4%E5%AE%89%E5%85%A8%E6%80%A7%E5%8D%94%E5%AE%9A))：
  * 傳輸層安全性協定顧名思義就是於傳輸層保護資料，是由SSL演變而來的
  * TLS會建立一個安全的Session key(對話層鑰匙)，一旦對話結束，該鑰匙就會被遺忘，如果開啟了另一個通信，就需要產生新的鑰匙。
  * TLS 協議由客戶端 c 發起，用於與伺服器進行安全通信。在協議使用之前，假定服務器 s 已經從受信任的憑證頒發機構(certification authority) CA 獲得了一個數位憑證(certificate)，表示為certs。 該證書是一個包含以下內容的結構：
    * 伺服器的屬性attrs，例如伺服器例如它的唯一區別名稱及其通用 (DNS) 名稱
    * 伺服器的非對稱加密算法 E() 
    * 伺服器的public key
    * 憑證的有效期時間unterval
    * 由CA根據以上資料發給的數位簽章a，a由以上資料推算而來
  * 客戶端在使用此協定之前，假設已經從CA取得公共驗證演算法Vkca，在網頁中，使用者的瀏覽器是從其供應商提供的，其中包含某些認證機構的驗證算法和公鑰。使用者可以刪除這些或添加其他。
  * 其流程如下：
    * 當 c 連接到 s 時，它會向伺服器發送一個 28 bytes的隨機值 nc，伺服器以它自己的隨機值 ns 和它的憑證certs作為響應。
    * 客戶端驗證 VkCA(attrs, Eke, interval, a) = true 並確認當前時間在有效時間內。
    * 然後客戶端生成一個隨機的 46 bytes的 premaster secret(管理員密碼) pms 並將 cpms = Eke(pms) 發送到服務器。
    * 伺服器根據 pms = Dkd(cpms)反推出pms。現在客戶端和伺服器都擁有 nc、ns 和 pms，並且每個都可以計算共享的 48 bytes主密鑰 ms = H(nc, ns, pms)。
    * 只有伺服器和客戶端可以計算 ms，因為只有他們知道 pms。此外，ms 對 nc 和 ns 的依賴確保了 ms 是一個新的值，換言之，他不會使用在先前的協定執行上。
    * 此時，客戶端和服務器從 ms 計算以下密鑰：
      * 對稱加密密鑰 k𝖼𝗋𝗒𝗉𝗍cs加密從客戶端到伺服器的消息
      * 對稱加密密鑰 k𝖼𝗋𝗒𝗉𝗍sc於加密從伺服器到客戶端的消息
      * MAC 生成密鑰 k𝗆𝖺𝖼cs從客戶端到伺服器的消息上生成認證 
      * MAC 生成密鑰 k𝗆𝖺𝖼sc於在從伺服器到客戶端的消息上生成認證
    * 客戶端送出密文 c = Ek𝖼𝗋𝗒𝗉𝗍kcs(⟨m, Sk𝗆𝖺𝖼cs(m)⟩)
    * 伺服器在收到c之後使用⟨m, a⟩ = Dk𝖼𝗋𝗒𝗉𝗍cs(c)
    * 如果V𝗆𝖺𝖼(m, a) = true的話就接受 m，同理，當伺服器要送出訊息m時，會一起送出c = Ek𝖼𝗋𝗒𝗉𝗍sc(⟨m, Sk𝗆𝖺𝖼sc(m)⟩)
    * 而客戶根據⟨m, a⟩ = Dk𝖼𝗋𝗒𝗉𝗍sc(c)計算得出m，如果V𝗆𝖺𝖼(m,a) = true的話就接受 m
  * 該協議使伺服器能夠將其消息的接收者限制為能夠生成 pms 的客戶端，並將它接受的消息的發送者限制為同一客戶端。同樣，客戶端可以將其發送的消息的接收者和接收的消息的發送者限制為知道kd的一方（即可以解密cpms的一方）。
  * 在很多應用中，比如web交易，客戶端需要驗證知道kd的一方的身份。 這是certs的目的之一。 特別是，attrs 包含客戶端可以用來確定與它通信的伺服器的身份（例如，域名）的信息。 
  * IPSec VPN 與 TLS VPN：IPSec 非常適合對流量進行點對點加密——例如，在兩個公司辦公室之間。 TLS VPN 更靈活但效率不高，因此可以在遠端工作的員工和公司辦公室之間使用。 
  * [TLS詳解(全英)](https://www.comparitech.com/blog/information-security/tls-encryption/)

* Firewalling to Protect Systems and Networks：
  * Firewall(防火牆)是於可信任與不可信任的區域之間舉凡的電腦、路由器、process、應用程式等等
  * 一個network firewall限制的對於security domains的存取，並記錄(log)、監控(monitor)一切access。
  * network firewall可以把network 分成多個domain，舉一個最常用的例子，通常Internet(網際網路)會被分在untrusted domain，然後還會有一個半信任的network稱為[DMZ(demilitarized zone，非軍事區)](https://zh.wikipedia.org/wiki/DMZ)為另一個Domain，第三個domain則是在公司裡面的所有電腦。
    * 從Internet連線到DMZ或者從公司的電腦連線到網際網路是允許的，但是從Internet直接連線到公司的電腦是不被允許的(簡單的說就是所有來自Internet的存取都必須要透過DMZ)
    * 或者是在嚴格限制的條件下，DMA可以存取公司電腦，假如即便DMZ受到感染，也沒辦法access其他的公司電腦(降低傷害程度)。例如DMA上的web server可能只被允許對一個資料庫的server下query。
  * ![DomainSeparation](https://github.com/a13140120a/Operation_System/blob/main/imgs/DomainSeparation.jpg)
  * 防火牆本身不能防止以下攻擊：
    * tunneling(穿透)：帶有溢位攻擊的正常管道(譬如http是被允許的，但裡面含有overflow的程式碼)
    * spoofing：攻擊者假裝是其中一項經過認證的host(譬如藉由偽造IP)
    * DoS攻擊
  * 某些特殊的防火牆被開發用來保護電腦不受網路攻擊：
    * personal firewall：於軟體層的保護，用來限制使用者的電腦能夠與哪些host溝通，以及不能夠與哪些host溝通，這樣可以避免木馬程式外洩資料。
    * application proxy fire wall：application proxy fire wall本質上是網路上每個連接的中間人。網絡上的每台計算機都通過代理(proxy)建立連接，從而創建一個新的網絡連接。例如，如果user想要訪問外部網站，則封包在被轉發到請求的網站之前會通過 HTTP server進行過濾、監控、處理，來自網站的封包亦是，這種技術比傳統的防火牆更強的地方在於它不僅僅檢查封包的來源以及目的，還可以檢查整個封包的內容。
    * XML firewal：分析xml的內容，並阻擋未經允許或畸形(不完整，malformed)的xml格式。
    * System-call firewalls：檢查應用程式與kernel之間的溝通，監控system call的執行
* Other Solutions：
  * Address Space Layout Randomization (ASLR)：要使用程式碼注入攻擊的話，駭客必須了解目標的精準記憶體位址，ASLR技術會隨機分配諸如heap、stack的位址，雖然並不能完全阻擋關於此類的攻擊，但仍然可以增加攻擊的難度，此技術已經成為Windows, Linux, and macOS的標準功能之一.


****




<h1 id="016">Protevtion</h1> 

  * ## [Goals of Protection](#0161) #
  * ## [Protection Rings](#0162) #
  * ## [Domain of Protection](#0163) #
  * ## [Access Matrix](#0164) #
  * ## [Implementation of the Access Matrix](#0165) #
  * ## [Revocation of Access Rights](#0166) #
  * ## [Role-Based Access Control](#0167) #
  * ## [Mandatory Access Control (MAC)](#0168) #
  * ## [Language-Based Protection](#0169) #




<h2 id="0161">Goals of Protection</h2> 

* 為了防止用戶或程序惡意濫用系統
* 確保每個共享資源僅根據系統的policy使用，該policy可由系統設計者或系統管理員設置。
* 確保錯誤的程式造成的損害盡可能小。
* 基本上每個user都會有一個自己的account，並且都只能修改、更動自己的檔案
* root 帳戶不應用於正常的日常活動。即便系統管理員也應該有一個普通帳戶，並保留使用 root 帳戶僅用於需要 root 權限的任務。

<h2 id="0162">Protection Rings</h2> 

* 將所有執行的動作定義成一個同心圓，每一層都是一個 *Ring* ，最裡面的Ring(Ring 0)，擁有最高的權限。
* Ring之間的互動：
  * 系統剛開機時，將會啟動最高level的權限，將必要的程式碼初始化之後再降回到較低的ring，如果需要使用較高權限的ring的話，必須要透過一些特殊指令(這些指令通常稱為 *gate* )，他在ring之間提供接口，例如Intel cpu的 [SYSCALL](https://www.felixcloutier.com/x86/syscall)，[SYSCALL補充](https://stackoverflow.com/questions/10583891/is-syscall-an-instruction-on-x86-64)，此呼叫會將執行的動作由使用者模式轉換到核心模式。
  * 執行任何system call 或者產生trap 或 interrupt 都會使執行的動作轉換到更高權限的ring。
* Intel and ARM：
  * Intel 架構使用此模式，並且將user mode的程式碼放在ring3，kernel mode的程式碼放在ring 0，並使用EFLAGS 暫存器中的兩個bits來區分ring
  * Intel 提供一個特殊的ring(Ring -1)來提供虛擬機使用。
  * ARM一開始僅允許使用USR(for user) 和 SVC(for kernel or supervisor) mode,並且於ARMv7引入了TrustZone (TZ)，TZ是一個最高權限的執行環境(一個額外的Ring)，TZ可以access 硬體的加密功能(譬如[NFC](https://zh.wikipedia.org/wiki/%E8%BF%91%E5%A0%B4%E9%80%9A%E8%A8%8A) [Secure Element](https://nfc-forum.org/faq-items/what-is-a-secure-element/) 和晶片加密密鑰)，連kernel也必須透過TZ環境裡的 [Secure Monitor Call (SMC)](https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/The-System-Level-Programmers--Model/Exception-descriptions/Secure-Monitor-Call--SMC--exception?lang=en)才能夠請求加密和解密服務，與system call一樣，kernel只有透過register傳遞參數才能夠執行任務，如此一來就算kernel就算遭到攻擊，也無法從kernel memory中找到晶片密鑰。
  * ![trust_zone](https://github.com/a13140120a/Operation_System/blob/main/imgs/trust_zone.PNG)
  * 64-bit ARMv8 架構中，支援四種level的層級，稱為 *exception levels* ，分別是從EL0到EL3
    * user mode 於EL0中執行
    * kernel mode 於EL1中執行
    * [hypervisors(虛擬機管理程式)](#0171)於EL1中執行
    * 最後EL3就是TrustZone layer
    * 下圖為ARM Exception架構圖
    * ![ARM_Exception_architecture](https://github.com/a13140120a/Operation_System/blob/main/imgs/ARM_Exception_architecture.PNG)
    * [ARM_architecture參考](http://wiki.csie.ncku.edu.tw/embedded/ARMv8#%E4%BE%8B%E5%A4%96%E5%B1%A4%E7%B4%9A-exception-levels)


<h2 id="0163">Domain of Protection</h2> 

* 電腦系統可以被視為是一個process與objects的集合，其中objects包含hardware object(例如cpu, memory)以及software object(file,semaphore,program)
* 每個object只能由特定的方式操作，譬如cpu只能執行，memory可以讀寫，檔案可以建立、刪除、開啟關閉等等。
* need to know principle：一個process只被允許access他需要用到的objects
* Domain Structure：
  * protection domain：限制了一個process可以access的resources。
  * access right：一個process可以對這個object做的操作
  * 一個domain可以定義成<object, {access right set}>，例如D有一access right：<file F, {read, write}>，代表D有權對F做讀寫的操作。
  * Domain之間可以共用access right，下圖有三個domain，其中<O4,{PRINT}>被D2與D3所共用。
  * ![ThreeDomains](https://github.com/a13140120a/Operation_System/blob/main/imgs/ThreeDomains.jpg)
  * Domain與process之間的關係可以是動態，也可以是靜態的。建立動態Domain會比靜態更複雜。
    * 舉例來說，例如一個process分成兩個階段，一個階段只需要對file做讀取，另一個階段則必須要做寫入，則如果是靜態的話，在第一個階段就會同時擁有讀寫的權限，這違反了need to know principle。
    * 如果是動態的話，我們可以把視Domain為以下三種型態：
      * 視每個Domain為每個user，當使用者改變時，object的access right取決於user ID，代表改變了Domain，通常改變會發生在user log in/log out
      * 視每個Domain為每個process，object的access right取決於process ID，當process send message到其他的process並且等待response時，代表改變了Domain
      * 視每個Domain為每個procedure(function)，object的access right取決於local variables，當procedure被call或結束時，代表改變了Domain
    * dual-mode (kernel– user mode) model也是一個Domain的例子，當呼叫system call(privileged instructions)的時候就進到kernel mode(也就是kernel domain)，而一般(non-privileged instructions)就只能在系統分配給他的memory space(user domain)執行。

* UNIX Example：
  * UNIX系統的[檔案的特殊屬性 SUID/SGID/SBIT](https://dywang.csie.cyut.edu.tw/dywang/linuxsecurity/node39.html)
  * 例如 user 想要更改自己的密碼必須執行 /etc/passwd ，如果直接賦予user root的權利的話，萬一出現人為的意外，會造成很嚴重的後果，通過Set UID (SUID)，我們可以讓user在執行該檔案的時候擁有owner(這個例子也就是root)的權限，這樣就算發生了人為的意外，也可以將傷害降至最小。
* Android Application IDs Example：
  * 在 Android 中，為每個應用程式提供不同的user ID。安裝應用程式時，installd 這個deamon 為其分配一個不同的user ID (UID) 和group ID (GID)，以及一個私有數據目錄(/data/data/\<app-name\>)，其所有權被授予此 UID/GID 的組合。

* Android Application IDs Example：
  * 
  * 該機制通過修改kernel來擴展，以允許某些操作（例如 networking sockets）僅對特定 GID 的成員（例如，AID_INET，3003）。



<h2 id="0164">Access Matrix</h2> 

 
* 我們可以利用一個Access Matrix(存取矩陣)來表示Domain跟objects之間的關係，其中row代表domain，column則代表object，每個 entry 都代表一個access rights，entry access(i,j) 定義了在Domain Di 中執行的process可以對object Oj 執行的操作。
* 下圖中代表四個domain(D1\~D4)以及F1\~F1、一個printer之間的關係，可以看到於D1內可以對F1、F3讀取，於D2內可以使用printer....以此類推。
  * ![Access_Matrix](https://github.com/a13140120a/Operation_System/blob/main/imgs/Access_Matrix.jpg)
* switch：或者我們可以把 access matrix 中的每個 entry 都當成一個 object，並且加入一個switch的 access right，switch 允許在 Domain 之間轉換。
 * 下圖為上圖的access matrix 並且把 domain 視為 object 之後的結果：
 * ![Access_Matrix_switch](https://github.com/a13140120a/Operation_System/blob/main/imgs/Access_Matrix_switch.jpg)
* 允許對 access right 的改變必須添加額外三個動作：copy、owner、control。
* copy：我們可以在 entry 加上星號來表示這個entry可以被copy。
  * transfer：如果一個 access(i, j) 轉換到另一個 access(k, j) 之後原本的 access(i, j) 被移除的話，稱為 transfer 而不是 copy
  * limited copy：當 access(i, j) R* 被複製到 access(k, j) R 之後，這個複製出來的 R 不能擁有再複製的能力(也就是沒有星號)。
  * ![AccessMatrixCopy](https://github.com/a13140120a/Operation_System/blob/main/imgs/AccessMatrixCopy.jpg)
* owner：加入 owner right 之後，如果 access(i, j) 包含 owner right 的話則在 Di 中的 process 可以加入或移除 column j 中的任何 entry。
  * ![AccessMatrixOwner](https://github.com/a13140120a/Operation_System/blob/main/imgs/AccessMatrixOwner.jpg)
* control：既然 owner 允許 process 可以改變 column，那我們還需要一個可以改變 row 的機制，這個機制可以利用 switch 中的圖片下去修改，如果 process 於 Di 中則可以移除任何 access right from row j。
  * 如下圖所示，access(D2, D4) 包含control right，所以 D2 可以修改 D4 的 access right。
  * ![AccessMatrix_control](https://github.com/a13140120a/Operation_System/blob/main/imgs/AccessMatrix_control.jpg)

* 
<h2 id="0165">Implementation of the Access Matrix</h2> 

* Global Table：
  * 最簡單的方式就是使用global table，每個 entry 都已 \<domain, object, rights\> 的方式儲存。
  * 缺點是這個table可能會太大，以致於無法整個存在memory裡面，或者需要使用 virtual memory 的技術，後者則會導致 IO 造成效能低落。
  * 如果有一個 object 可以被每個 Domain 存取，那麼所有的 Domain 都必須要有一個 entry 包含這個 object。
* Access Lists for Objects：
  * access matrix 的每個 column 都可以用一個 access list 來製作，空白元素省略不記，list 中的每個項目都以 \<domain, rights-set\> 的形式儲存。
  * 系統每次 access object 都必須要 search 整個 access list 來確定自己有沒有資格存取，這會浪費很多時間。
  * 這種方法還可以衍伸出另一種方法：可以使用一個額外的 default set 的 list 來代表每個 Domain 所能夠獲得的 access right。
  * 能夠藉由先尋找 default set list 找不到的話再找 Access Lists 的方式來提升效率。
* Capability Lists for Domains：
  * 類似的方式，Capability Lists 代表了每個 row，每個 list 裡面的 element 都是一個 *capability* 。
  * Capability lists 雖然代表了一個 Domain，但不能經由 user process 直接存取 Capability。
  * 為了幫 Capability 提供保護，我們可以利用以下兩種方式來區別 Capability 跟其他的 data：
    * 使用一個 tag 來代表是否為 Capability(可以藉由硬體或韌體來達成)，當然 tag 本身也必須被保護，還能透過更多位元的 tag 來區別這是整數、浮點數、字元還是 Capability 等等。
    * 另一種做法是將 process address space 劃分成兩個部份，其中一個部份是 process 可以存取的，包含 process instructions 以及 data，另一個部份則儲存 Capability lists，只能透過 OS 來存取，前面提到的 segmented memory space 很適合這種做法。
  * 然而，如果想要取消一個 access right 的話，需要尋找整個系統的所有 Capability Lists 來做更動，這非常困難。
* A Lock-Key Mechanism：
  * 每個 object 都有一個獨特的 bit patterns list，稱為 locks，每個 domain 也都有一個獨特的 bit patterns list，稱為 keys
  * 只有 keys 符合 locks 的時候才被批准使用 resource。
  * user process 不能夠修改或查看 locks or keys。
  * 是前面兩種做法的折衷方案，key 可以自由的傳遞於 domain 之間，而且要取消 access right 只需要改動 object 的 key 即可。

<h2 id="0166">Revocation of Access Rights</h2> 

* 在動態保護系統(dynamic protection system)中，我們常常會需要取消一個 domain 對 object 的access right，這就衍生出了以下幾種問題：
  * Immediate versus delayed(立即和延後)：取消是立即發生的，或是延後發生的?如果是延後發生的，我們是否能夠察覺他是和時發生的?
  * Selective versus general(選擇性及一般性)：當一個 object 的 access right 被取消時，他是否會影響所有此 object 的access right?或者我們是否能夠使用某一集合的 users，他們對此 object 的 access right 被取消?
  * Partial versus total(部份或全部)：能否只有一部份集合對此 object 的 access right 被取消，或必須對此object 的全部 access right 都取消?
  * Temporary versus permanent(暫時性與永久性)：顧名思義

* access-list scheme 當中，取消相當容易，只要搜尋 list 並且將他 remove 就可以了，但在 Capability list 中就很麻煩，如上述所說，以下探討 implement revocation(取消) for capabilities 的 schema：
  * Reacquisition：capabilities 週期性的從 domain 中被刪除，當一個 process 想要使用 capabilities 並且發現已經被刪除了，該 process 會嘗試再次獲得 capabilities，如果 access 被 revoked(撤銷)，則無法再獲得 capabilities。
  * Back-pointers：每個 object 都維護一個由 pointer 組成的 list，這些 pointer 指向與 object 相關的 capabilities，如此一來當需要 revoked 的時候只要順著這個 pointer 依序更改 capabilities 就可以了，這種方法很普遍使用
  * indirection：capabilities 並不是直接指向 object 而是指向一個 global table，然後 table 中的每個 entry 再指向 object，如此一來當需要 revoked 的時候，只需要 search 該 table 並刪除其 entry 就可以了，當有人要 access 的時候，會發現這是一個 illegal table entry(非法存取)。
  * key：
    * 會有一個跟 capability 相關的 key，當 capabilities 被 create 的時候，該 key 也跟著被建立，這個 key 不能被任何 process 檢查或者修改，
    * 然後每個 object 會有一個相關的 master key，這個 per-object 的 master key 會對應到該 object 的所有 capabilities。
    * 使用`set-key`可以修改或定義一個 master key，當使用`set-key`來修改 master key 的值時，所有相關的 capability 都會失效
    * 當 capability 被 create 的時候，master key 的值會跟 capability 有關，如果 capability 被使用，則這個 capability 會被拿去跟 master key 做 match，如果 match 成功的話則 allow，反之亦然。
    * 這種 schema 不允許選擇性的 Revocation，但我們可以將每個 object 使用單一個 master key 改成每個 object 使用一個 key list ，則可以解決這個問題。
    * 可以將所有 key 組合到一個 global table 當中。只有當它的 key 與 table 的某個 key 匹配時才有效，使用這種方案，一個 key 可以與多個 object 關聯，每個 object 也可以與多個 key 相關聯，從而提供最大的靈活性。


<h2 id="0167">Role-Based Access Control</h2> 


* RBAC(Role-Based Access Control)模型是一套較MAC以及DAC更為中性且更具靈活性的存取控制技術。
* role：以 *role(角色)* 為基礎的存取控制（Role-based access control，RBAC），是資訊安全領域中，一種較新且廣為使用的存取控制機制，其不同於強制存取控制(MAC)以及自由選定存取控制(DAC)，直接賦予使用者權限，而是將權限賦予 *role(角色)* 。
* 用戶被分配了角色，或者可以根據分配給角色的密碼來扮演角色。通過這種方式，用戶可以扮演啟用特權的角色，允許用戶運行程序來完成特定任務。
* [wiki](https://zh.wikipedia.org/wiki/%E4%BB%A5%E8%A7%92%E8%89%B2%E7%82%BA%E5%9F%BA%E7%A4%8E%E7%9A%84%E5%AD%98%E5%8F%96%E6%8E%A7%E5%88%B6)


<h2 id="0168">Mandatory Access Control (MAC)</h2> 

* [DAC](https://zh.wikipedia.org/wiki/%E8%87%AA%E4%B8%BB%E8%AE%BF%E9%97%AE%E6%8E%A7%E5%88%B6)：
  * 在 UNIX 系統中，DAC 採用檔案權限的形式（可通過 chmod、chown 和 chgrp 設置），而 Windows（和一些 UNIX 變體）通過訪問控制列表 (ACL) 允許更精細的粒度的控制。
  * 但是 DAC 有兩個關鍵的弱點，一是 resource 的 owner 可以任意修改該 resource 的權限，另一個是 root 擁有無限的權限。
* [MAC](https://zh.wikipedia.org/wiki/%E5%BC%BA%E5%88%B6%E8%AE%BF%E9%97%AE%E6%8E%A7%E5%88%B6)：
  * 因此，需要一種更強大的保護形式。MAC 作為系統策略強制執行，即使是 root 用戶也無法修改（除非策略裡有明確允許修改系統）。 MAC 策略規則的權限比 root 的能力更強大，可用於使除其預期所有者之外的任何人都無法任意存取資源。
* 幾乎每個現代的作業系統都同時提供 MAC 和 DAC ，Solaris 的 MAC 是 Trusted Solaris 的一部分，而 FreeBSD 將 DAC 作為其 TrustedBSD 的一部分，Linux 是 SELinux 等等。
* MAC 的核心是標籤(label)的概念。標籤是分配給 object（檔案、設備等）的標識符(id)（通常是字符串）。標籤也可以應用於 subject（參與者，例如 process）。當 subject 請求對 object 執行操作時，作業系統會先標籤，確定是否允許 subject 對 object 執行操作。
* 舉個簡單的例子，考慮一組簡單的標籤，按照特權級別排序： "unclassified" 、"secret"和"top secret"。具有"secret"權限的用戶將能夠建立類似標記的 process，然後這些 process 將可以存取"unclissified"和"secret"的檔案，但不能存取"top secret"的檔案。
* 用戶及其 process 都不會意識到"top secret"檔案的存在，因為 OS 會將它們從所有檔案操作中過濾掉（例如，在列出目錄內容時不會顯示它們）。用戶 process 同樣會以這種方式受到保護，因此"unclissified"的 process 將無法查看或執行對"secret"（或"top secret"）process 的 IPC 請求。
* MAC labels 是前面描述的訪問矩陣的 implement。


<h2 id="0169">Capability-Based Systems</h2> 

* Linux 將 root 的權力切成不同的區域，每個區域都由 bitmasks 的一個 bit 表示
* 通常會使用三個 bitmasks，分別表示 capabilities 允許(permitted), 有效(effective), 和可繼承(inheritable)
* bitmask 可以用在每個 process 或 thread，此外，一旦撤銷(revoked)，就不能重新獲得 capability。
* 一般的順序是 process 或 thread 以允許的全部能力集(permitted capabilities set)開始，並在執行期間慢慢減少該集。 例如，在打開一個網絡埠後，一個 thread 可能會刪除該功能，這樣就不能再打開更多 port 了。
* Example： [Hydra、CAP system](https://www.cs.uic.edu/~jbell/CourseNotes/OperatingSystems/14_Protection.html)



<h2 id="0169">Other Protection Improvement Methods</h2> 

* System Integrity Protection (SIP)：
  * Apple 於 masOS 10.11 中一個新的保護機制，稱為System Integrity Protection (SIP)
  * Darwin-based 的 OS 使用這種機制來限制對系統檔案及資源的存取，就連 root 也不能隨意修改這些檔案及資源。
  * SIP 使用一個額外的檔案屬性來保護系統的二進制檔案以防止他們被檢查或修改。
  * 僅允許擁有程式碼簽章(code-signed)的核心擴展還有二進制檔案
  * 使用 SIP 的話 root 雖然還是系統裡面最強大的使用者，但比起以前，他的能力範圍會減少許多，root 仍然可以管理其他使用者的檔案、設定、process 等等，但無法隨意更動有關 OS 的部分。 
* system-call filtering：
  * system-call filtering 的其中一種方法是，向 kernel 添加檢查 system-call 的程式碼，將 caller 限制在安全的 system call set 裡面，例如可以為各個 process 構建特定的 system call config 檔。
    * Linux 機制 SECCOMP-BPF 就是這樣做的，它利用 [Berkeley Packet Filter (BPF) syntax](https://biot.com/capstats/bpf.html) 通過 Linux 專有的 `prctl()` 這個 system call 來加載自定義配置檔(profile)，這種過濾可以有效地過濾來自 run-time library 或者 loader 的 system call。
      * 註：`prctl()`是一個可以從各個層面檢查或更動 process 的 system call。
  * 另一種方法是連 system call 的參數也要檢查，因為即使是看似良性的 system call 也可能隱藏嚴重的漏洞，Linux 的 [futex()](https://ryan0988.pixnet.net/blog/post/181245363-linux%E4%B8%AD%E7%9A%84%E5%90%8C%E6%AD%A5%E6%9C%BA%E5%88%B6----futex) system call 就是這種情況，攻擊者利用 race condition 來覆蓋掉 kernel memory 以破壞系統。
* sandboxing：
  * sandboxing(沙盒)是指對一個 process 實施非常嚴格的限制的做法，這種被施加的限制在 process 被 create(執行`main()`或 `fork()`之前) 之前就已經先定義好了。
  * 在sandbox 裡面的 process 無法執行任何除了被允許以外的操作，也可以防止 process 與系統的其他部份溝通。
  * 有許多實現 sandbox 的方法，例如 Java 和 .net 在虛擬機施加沙盒限制，Android 將沙盒作為其強制訪問控制 (MAC) 策略的一部分，它利用 [SELinux](https://zh.wikipedia.org/zh-tw/%E5%AE%89%E5%85%A8%E5%A2%9E%E5%BC%BA%E5%BC%8FLinux) 策略增強了安全性。
* Code signing：
  * 系統如何相信一個 process 或 script ?通常，如果該 process 或 script 是 OS 的一部分，那麼它應該是可以信任的，但如果它改變了呢?如果它是被系統更改的，那它仍然是可以被信任的，否則就是不可以被信任的，這些不被信任的修改在執行之前必須要經過特殊許可（來自用戶或管理員）。
  * Code signing(程式碼簽章)是解決上述問題的最佳工具，它使用前面提到過的[authentication](https://github.com/a13140120a/Operation_System/edit/main/README.md#%E8%AA%8D%E8%AD%89authentication)的方式來保持程式碼的完整性以及真實性，以確認自作者創建它們以來它們沒有被更改。
  * 代碼簽名用於 OS 發行版、補丁和第三方工具等。某些 OS（包括 iOS、Windows 和 macOS）拒絕運行未通過程式碼簽章檢查的程式。
  * 它還可以通過其他方式增強系統功能。例如，Apple 可以禁用為現已過時的 iOS 版本編寫的所有程式，方法是在從 App Store 下載這些程序式停止對這些程式進行簽名。

<h2 id="0169">Language-Based Protection</h2> 

* OS kernel   層面的檢查，會對每次存取的嘗試都做檢查，這會對系統的效能造成很大的負擔，因此我們必須為其提供 hardware support 以降低每次檢查的開銷(overhead)，但我們很難去確保這些機制造成的額外開銷不會對效率及花費成本造成威脅。
* 隨著作業系統變得越來越複雜，特別是當它們試圖提供更高級別的用戶界面時，保護的目標也變得更加精細。
* 再者 resource 的使用取決於應用程式有許多不同的方法，
* 因此保護不再只是作業系統設計者關心的問題。它還應該可以作為應用程式設計者使用的工具，從而可以保護 subsystem 的資源免受篡改或錯誤的影響。

* Compiler-Based Enforcement
  * 在 Compiler-Based Enforcement(基於編譯器的保護)中，程式設計師在宣告 resource 時直接指定不同 resource 所需的保護。
  * 利用這種方法有以下幾種好處：
    * 保護只需簡單的被宣告，而不是一系列複雜的 procedure call。
    * 可以獨立於特定作業系統提供的支持來說明保護要求(簡單的說就是可以獨立於作業系統之外)。
    * 執行方式不需要由開發商(子系統的設計者)直接提供。
    * 宣告的表示法很自然，因為存取權限與數資料型態的概念密切相關。
  * 編程語言實現可以提供多種技術來實施保護，但其中任何一種都必須依賴於底層機器及其作業系統的某種程度的支持。例如，假設使用一種語言撰寫的程式碼以在 Cambridge CAP 系統上運行
  * 無論採用何種方式來實作，基於編譯器的保護都依賴於底層機器或者作業系統提供的保護機制，例如 Cambridge CAP 或 Hydra 系統，Cambridge CAP 系統中對所有 storage 都必須透過 capability 間接的來存取，這避免了 process 跑出了保護環境的範圍。
  * Compiler-Based Enforcement 和 kernel 提供的保護相比，有以下優點：
    * Security：kernel 提供的安全性比編譯器的安全性提供了更好的保護。Compiler-Based Enforcement 的安全性取決於編譯器本身的完整性，以及要求檔案在編譯後不被修改，kernel 有更好的手段來保護自己免受修改，以及保護對特定檔案的存去，如果有 hardware support，則保護更加強大。
    * Flexibility：kernel 提供的保護系統不能靈活地提供單個 programmer 所需的特定保護，儘管它可以提供 programmer 可以使用的功能，Compiler-Based Enforcement 容易更改和更新編譯器以更改所提供的保護服務或其細節。
    * Efficiency：最有效率的保護方法是直接提供hardware 或 microcode support，而就 software 而言，Compiler-Based 的系統具有以下優點：許多檢查可以在編譯時離線進行，而不是在執行期間(run-time)進行。
  * 將保護機制納入編程語言的概念還處於起步階段，仍有待充分發展。 然而，總體目標是提供以下三個功能：
    * 在 processes 之間安全有效地分配 capabilities ，尤其是一個 user process 應該只能訪問它被授予權限的資源。
    * 指定 process 可以對資源執行的操作類型，例如讀取或寫入。
    * 指定對資源執行操作的順序，例如先打開再讀取。
* Run-Time-Based Enforcement--Protection in Java
  * 因為 Java 被設計為在分散式環境中運行，所以 Java 虛擬機（JVM）具有許多內建的保護機制。
  * Java programs 是由 class 所組成的，每個class 又是由 data fields 和 functions (或 methods，可以操作 data field) 所組成的
  * Java 最新的特色之一是 support 透過網路 dynamically loading 不受信任的 class，也就是說它可以在同一個 JVM 中執行互相不信任的 class。
  * 也由於這些特色，Protection 變成了 Java 最重要的課題，同一個 JVM 中運行的 class 可能來自於不同的來源，而且可能不受信任，所以以 process 等級的保護是不夠的，必須提供更細度的保護機制，例如，是否應該允許打開檔案的請求通常取決於是哪個 class 發出請求。
  * 因此，此類保護決策是在 JVM 中處理的。 當 JVM load 一個 class 時，它會將該 class 分配給一個 protection domain，protection domain 提供該 class 的權限。
  * 這個 protection domain 取決於該 class 是從哪個 URL 來的，以及它的數位簽章，並且會有一個 configurable policy file(可配置的策略檔)來決定並授予 domain 權限，例如，從被信任的伺服器載入的 class 可能被放置在允許它們存取用戶主目錄中的檔案的 protection domain，而從不被信任的伺服器載入的 class 可能根本沒有檔案存取權限。
  * JVM 確定哪個 class 負責處理存取受保護資源的請求可能是一件很複雜的事，存取通常是通過 system libraries 或其他的 class 間接執行的，例如，考慮一個不允許打開網絡連接的 class，它可以調用 system libraries 來請求載入 URL 的內容。問題是，應該使用哪個 class 來確定是否應該允許連接，應用程式還是 system libraries？
  * Java 採用的理念是要求 library class 明確允許網絡連接，為了存取受保護的資源，一連串的 calling 中的一些發起 request 的 method 必須要 explicitly assert the privilege to access the resource
    * 註：[Java explicit](https://kknews.cc/zh-tw/education/r9apjro.html)、[Java assert](https://www.itread01.com/content/1547018948.html)
  * 當然，並不是每個 method 都可以 assert privilege。 只有當一個 method 的 class 在一個本身被允許行使特權的 protection domain 時，一個 method 才能 assert privilege。
  * stack inspection：此類 implementation 又稱為 *stack inspection* ，JVM 中的每個 thread 都有一個與正在進行的 method 相關的 stack，當 caller 不能被信任時，method 會被包在 doPrivileged() 的 block 中執行，以直接或間接的 access 受保護的資源。
  * doPrivileged() 是 AccessController 類別中的一個靜態方法，它通過一個帶有 run() 方法的類別來調用，當進入 doPrivileged() block 時，該方法的 stack frame 會被 "annotated(標註，會被 mark 成 privileged)"，然後 block 裡面的內容才被執行。
  * 當隨後通過此 method 或這個 method 調用的方法請求存取受保護的資源時，會有一個 `checkPermissions()` 來做 stack inspection 並決定這個 request 是否被允許存取資源。
  * stack inspection 會檢查 call 這個 method 的 thread，如果有找到 doPrivileged() 的 "annotation(標註)"則 return 並允許存取，如果找到了不允許存取的 stack frame 就觸發 AccessControlException，如果都沒有找到的話就取決於每個版本的 JVM 而決定要怎麼做。
  * Stack inspection 如下圖，在這裡，不受信任的小程式(applet)的 protection domain 中的 class 的 gui() 方法(method)執行兩個操作，首先是 get()，然後是 open()。
  * 前者是在 URL loader protection domain 中調用的一個類別的 get() 方法，它被允許打開到 lucent.com 的會話(session)，
  * 因此，不受信任的小程序的 get() 調用將成功：網絡庫中的 checkPermissions() 調用遇到 get() 方法的 Stack inspection，該方法在 doPrivileged block 中執行 open()。 
  * 但是，不受信任的小程序的 open() 調用會導致異常，因為 checkPermissions() 調用在遇到 gui() 方法的 Stack inspection 之前沒有找到 doPrivileged 註解。
  * ![Stack_Inspection](https://github.com/a13140120a/Operation_System/blob/main/imgs/Stack_Inspection.jpg)
  * 當然，要使堆棧檢查起作用，程序必須不能修改其自己的堆 stach frame 上的 annotation 或以其他方式操縱  Stack inspection。 這是 Java 與許多其他語言（包括 C++）之間最重要的區別之一，Java program 不能直接 access 記憶體，它只能操作它有 reference 的對象。reference 不能被偽造，並且只能通過定義良好的 interface 進行操作。
  * [參考資料](https://tech101.cn/2019/08/15/AccessController%E7%9A%84doPrivileged%E6%96%B9%E6%B3%95%E7%9A%84%E4%BD%9C%E7%94%A8)




****




<h1 id="017">Virtual Machines</h1> 

  * ## [Overview](#0171) #
  * ## [History](#0172) #
  * ## [Benefits and Features](#0173) #
  * ## [Building Blocks](#0174) #
  * ## [Types of VMs and Their Implementations](#0175) #
  * ## [Virtualization and Operating-System Components](#0176) #
  * ## [Examples](#0177) #
  * ## [Virtualization Research](#0178) #




<h2 id="0171">Overview</h2> 


* virtual machine: 在一台電腦裡面，把諸如 CPU, memory, disk, 網卡等硬體抽象化成許多不同的執行環境，造成每個執行環境都有好像自己有一台自己的電腦的錯覺
* virtualization(虛擬化)：會有一個 layer，這個 layer 會創造一個 virtual system ，讓 OS 或 appliaction 可以在上面 run。
* Host：用來執行 virtual machine 的底層硬體系統。
* [virtual machine manager (VMM，或稱 hypervisor，虛擬機管理器，或稱虛擬機監視器)](https://zh.wikipedia.org/wiki/Hypervisor)：VMM 會提供一個與 host 相同的 interface 來給 vm 使用，讓 vm 可以在上面 run。
* Guest：guest process 通常實際上就是一個 OS，在此，"作業系統"的定義變得模糊，例如 VMware ESX 是一個安裝在 hardware 的虛擬化軟體(virtualization software)，並且當電腦開機時會執行，而且也提供 service 給 application ，包括 scheduling、記憶體管理等功能，並且也提供額外的功能例如 migration(application 移植於多個系統之間，而這些 application 通常就是 OS)。

* VMM 通常有以下幾種：
  * type 0 hypervisors(類型 0 虛擬機管理器)：VMM 程式通常被嵌在 firmware 裡面，這種類型的 VMM 通常會在 mainframe 或一些大型系統中看到，例如 IBM LPARs and Oracle LDOMs 。
  * type 1 hypervisors(類型 1 虛擬機管理器)：這種主要目的是提供虛擬化的 *類作業系統軟體(Operating-system-like software)* 包括了 VMware ESX 、 Joyent SmartOS、Citrix XenServer。
    •	General-purpose operating systems：提供一些基本的 VMM functions，例如具有 [HyperV](https://blog.xuite.net/yh96301/blog/459512721) 的 Windows Server，還有 具有 [KVM](https://zh.wikipedia.org/wiki/%E5%9F%BA%E4%BA%8E%E5%86%85%E6%A0%B8%E7%9A%84%E8%99%9A%E6%8B%9F%E6%9C%BA) 的 Red Hat Linux，這些 General-purpose operating systems 也都屬於 type 0 hypervisors
  * type 2 hypervisors(類型 2 虛擬機管理器)：這種類型的 VMM 會 run 在一般的作業系統之上，例如 VMware Workstation、Fusion、Parallels Desktop、Oracle VirtualBox。
  •	Paravirtualization(準虛擬化)：一種會更動 guest OS 的技術，會把 guest OS 修改成可以以更加的效能配合 VMM 的樣子。
  * Programming-environment virtualization(編譯環境虛擬化)：這種技術並不提供　hardware vitualization，而是提供一個優化的虛擬系統（virtual system），例如 Oracle Java、Microsoft.Net
  * Emulators(模擬器)：這種技術允許一個專門為某種 hardware 撰寫的應用程式可以 run 在非常不一樣的硬體環境(例如cpu)之上。
  * Application containment：這種技術並不能稱為虛擬化技術，但是這種技術可以把應用程式跟 OS 隔離開來，讓應用程式更安全，以及更好的被管理。


<h2 id="0172">History</h2> 

* Virtual machines 最早出現在 1972 年的 IBM mainframe，例如 IBM VM/370 mainframe system 上面可以 run 多個 VM，他使用一種 *virtual disks(虛擬硬碟)* 的技術(在BMI叫做minidisks)，minidisk 有除了 size 以外其他各方面都跟真實 disk 一樣的特性，這種技術藉由 allocating 多個磁軌來假裝成一個 disk 來達成。
* 虛擬化力求以下幾點目標：
  * Fidelity(真實度)：VMM  必須提供跟原本的機器一模一樣的環境。
  * Performance：在虛擬環境中，效能必須只能有一點點的下降。
  * Safety：VMM 必須能完全的掌握系統資源


<h2 id="0173">Benefits and Features</h2> 

* isolation：
  * 優點是 VM 可以保護 host 不受傷害，因為就算 VM 中毒了，也很難入侵到 host 裡面。
  * 缺點就是資源共享，但我們可以藉由以下兩種方式實現資源共享：
    * 透過共享的 file-system volume 來共享檔案
    * 透過網路來共享資源
* freeze(or suspend)：
  * 許多 OS 都會有這種功能來用在 process 上面(也就是有 suspend process 的功能的意思)。
* snapshot
  * VMM 除了這項功能之外還提供 snapshots 跟 copy 的功能，這種功能可以用來建立一個跟原本一模一樣的新 VM 或者把當前狀態的 VM 從一台機器移動到另一台機器上面。
  * 之後就可以 *resume*，然後就可以恢復到拍 snapshit 時的狀態，
* clone：
  * clone 會複製一個一模一樣的 vm 出來，這對於測試升級很有用(版本控制)
* VM 使得開發人員開法方便，節省大量時間，因為修改核心程式碼可能會造成整個作業系統毀損，但用 vm 的話就可以藉由 snapshot 或 clone 來達到快速復原的目的。
* VM 也使得開發人員可以快速的把 program 移植到各種 OS ，同理，測試人員也可以在各種不同的 OS(VM) 上快速測試每個不同的環境。
* consolidation(合併)：這是一種在 datacenter 裡面很常見的技術，該技術會把兩個不同的 system(VM) run 在同一台機器上，這樣做的目的是把多個較輕量級的系統合併成一個大型的系統
* templating：VM 模板是虛擬機的 "master copy image"，包括 VM disks、虛擬設備和其他設置，一個 VM template 可以多次用於虛擬機 clone，template 建立後無法開機和編輯，這種設定使得任何人都不會意外編輯 template 虛擬機，這種方法為 VM clone 提供了更高的安全性，VM clone 並不會 link 到 VM template 並且是獨立的，如果要編輯 template，則應將 template 轉換為 VM，編輯 VM，然後將編輯後的 VM 轉換為新的 template。
  * 在大型虛擬環境中手動部署許多類似的 VM 可能是一項無聊的任務。 VMware 在 vSphere 虛擬環境中提供了多種虛擬機 (VM) clone 方法，其中一種是使用 VM template。
* live migration：此特色讓 guest os(或 process) 在 running 的時候也可以在實體機器之間移動，不須中斷並且一直保持 active 的狀態，如此一來，當 host hardware 需要維修或升級時，


<h2 id="0174">Building Blocks</h2> 

* virtual CPU (VCPU)：VCPU 並不執行指令，而是會顯示一個虛擬的 cpu 狀態，VMM 會 maintain 每個 guest 一個的 VCPU，當 guest 要做 context switch 的時候，存在 VCPU 裡面的資訊會被 load 出來，就像普通 OS 最 context switch 的時候會把 PCB load 出來一樣。
* Trap-and-Emulate：因為 vitrual machine 與底層 system 都有 user mode 跟 kernel mode，但整個 VM 都是 run 在真實系統的 user mode 上的，這時候就要有一種辦法讓 VM 的 kernel(例如system call, interrupt, privileged instruction)被轉換成 physical 的 kernel mode，轉換的流程如下：
  * 當 guest 的 kernel mode 嘗試執行 privileged instruction 的時候，會發出 trap 給 VMM，
  * 這時候 VMM 得到這個 trap 並模擬他的動作然後執行，
  * 執行完之後再把控制權還給 VM
  * ![trap_and_emulate](https://github.com/a13140120a/Operation_System/blob/main/imgs/trap_and_emulate.PNG)
* Binary Translation：
  * special instructions：有些 cpu 當初在設計的時候就沒有考慮要支援 vm，甚至有一些 privileged instruction 在 kernel mdoe 跟 user mode 執行是不一樣的動作，例如有一個 `popf`的指令，在 kernel mode 的時候會把 stack 的狀態 load 到所有的 register，而在 user mode 執行的時候則只會把其中一些 load  到 register 裡面，因為這個指令不會發出 trap，所以 Trap-and-Emulate 的方法就不能使用，x86 裡面還有許多這種指令，稱為 *special instructions*。
  * 其流程如下：
    * 如果 guest 的 VCPU 是處於 user mode，那麼 guest 就可以直接把 instruction run 在真實 cpu 上。
    * 如果 guest 的 VCPU 是處於 kernel  mode，那麼 VMM 會檢查 guest 接下來將要執行的幾個 instruscions ，並將他們轉換成可以達到相同效果的另一批 instructions(即轉譯)，例如更改 VCPU 的 flags。
  * ![binary_translation](https://github.com/a13140120a/Operation_System/blob/main/imgs/binary_translation.PNG)
  * 我們可以把每次轉譯(translation)的結果 cache 下來，藉此達到效能的增進。
* nested page tables (NPTs，巢狀分頁表)：
  * VMM maintain NPT 來代表每個 guest 的 page table 的狀態，就像 VCPU 來代表 guest 的 CPU 狀態一樣，當 guest run 在 CPU 上時，VMM 將指向適當 NPT 的 pointer 放入適當的 CPU register 中，以使該 table 成為 active page table。
  * 如果 guest 需要修改 page table（例如執行 page fault），則該操作必須被 VMM 攔截，並對 nested page tables 和 system page table 進行適當的更改。
  * NTB 會導致 cache miss 率上升，並且會降低效能
* Hardware Assistance：
  * 現今大多數 CPU 也都提供許多虛擬化技術，例如 AMD-V(AMD virtualization technology)，這項技術除了原本的 user mode 與 kerbel mode 之外，還額外定義了兩個 mode：host 和 guest
    * 當 VMM  開啟 host mode  的時候，會定義 guest virtual machine 的特徵，然後轉換到 guest mode ，並將控制權轉換到 這個 guest 的 OS 上。
    * 當轉換到 guest mode 時，guest OS 認為他是 run 在一個剛剛  host mode 已經事先定義好的 hardware 環境之上。當 guest 想要存取 virtualized resource 時，會將控制權轉換給 VMM 然後進行互動。
  * Intel 於2005年加入了 VT-x 技術，讓 binary translation 不再被需要了。
    * VT-x 技術提供了除了原本的 user mode 與 kerbel mode 之外的兩個 mode：root and nonroot，這兩個額外的 mode 相當於 AMD-V 的 host 和 guest。
  * VT-x 和 AMD-V 都提供了 VCPU 來記錄 guest 的 cpu 狀態，並且當 context switch 發生時拿來使用。
  * virtual machine control structures (VMCSs虛擬機控制結構)：除了上述提到的資料結構以外，還會有一個 VMCs 來記錄及管理 guest 和 host 的狀態，以及各種客戶執行控制(guest execution controls)、退出控制(exit controls)以及有關客戶退出回主機的原因的信息，例如，由於嘗試存取不可用 memory 而導致的 nested page tables violation  可能導致 guest 退出。
  * AMD 和 Intel 還解決了虛擬環境中的 memory management 的問題。借助 AMD 的 RVI 和 Intel 的 EPT "記憶體管理增強功能"，VMM 不再需要實作 software 層面上的 NPT，因為 nested page tables 被實作到 hardware level，如此一來便可以加速 virtual 到 physical 位址的轉換，但 TLB miss 導致的 penalty 將會更大，因為系統必須要 traverse 更多的 table(包括 guest and host page tables)
  * ##### IO 是另一個受到 hardware support 增益的部分，例如 有 hardware-assisted DMA 的 CPU(例如 Intel 有 VT-d 功能的 CPU)，VMM 會設置一個 protection domains 來告訴 CPU 那些 physical memory 屬於那些 guest，然後再 assigns I/O devices 給那個 protection domains，允許被 assigns 的記憶體區域可以直接被存取且不能存取其他區域。
  * 擁有 interrupt remapping feature 的 CPU 會自動將發往 guest 的 interrupt 傳遞給當前正在運行該 guest 的 thread，如此一來便不需要 VMM 進行干預，如果沒有 interrupt remapping 的話，惡意的 guest 可能會生成可用於控制主機系統的 interrupt。
  * ARM v8 (64-bit) 使用 EL2 層來讓 vm 可以 run 在上面。

<h2 id="0175">Types of VMs and Their Implementations</h2> 

* Type 0 Hypervisor：
  * 又有其他的稱呼，例如 "partitions" 或者 "domains"。
  * 這種 VMM 是燒在 firmware 裡面的，並且在開機時載入，然後他會把 guest images load 進來以在每個 partition 運行。
  * 通常這類型的虛擬機功能會比其他類型少，因為他是直接 implement 在 hardware 上面的，每個 guest 都認為自己擁有專用硬體，因為它確實如此，從而簡化了許多細節。
  * control partition：用來管理 share 的 access 以及授權 IO devices 的存取
  * 本質上，Type 0 Hypervisor 中的每個 guest OS 都是一個 host OS，其中提供了一些個可用的硬體，所以每個 OS 又可以有他的 guest OS，其他類型的 Hypervisor 是沒辦法提供這種虛擬化再虛擬化的功能。
* Type 1 Hypervisor：
  * 常常被用於 data centers，並且有了"the data-center operating system"的別名
  * 是一種"special-purpose operating systems"，並且因為 Type 1 Hypervisor 是直接 run 在硬體之上，所以它們不為運行程式提供 system call 和其他 interface，而是創建、運行和管理 guest OS，例如：VMware ESX
  * 除了在標準硬體上運行之外，它們還可以在 Type 0 Hypervisor 上運行，但不能在其他 Type 1 Hypervisor 上運行。
  * 因為 Type １ Hypervisor　也是一種 OS ，所以他們也提供　CPU scheduling, memory management, I/O management, protection, security。
  * 可以把多個 guest OS consolidate(合併，上面有提到)在一起，假設今天有10台機器，每台分別負擔 10%(因為可能需要不同的環境)，就可以濃縮成一台，其他台就可以用來做別的事情，
  * 負載降低之後，就可以把 guests 移動到負載較低的系統，並且不需要暫停或中斷服務。
  * 這類型的 Hypervisor 還提供 clone、snapshot 等服務，讓開法者可以更快速開發及測試，還有備份。
  * 一些其他類型的 Type 1 Hypervisor(例如 RedHat Enterprise Linux, Windows, Oracle Solaris)可以在提供一般作業系統服務的同時，也提供 VMM 的功能，不過也因為同時提供兩種服務，讓它們在　VMM 的功能上會比一般的 Type 1 Hypervisor 要少，這些 Hypervisor 把 guest OS 當成是一個 process 而已，並且會對special instruction 進行特殊處理。
* Type 2 Hypervisor：
  * 這些應用程式級的 VMM 中幾乎沒有 OS 參與。這種類型的 VMM 只是由 host 運行和管理的另一個 process，甚至 host 也不知道 VMM 內正在發生虛擬化。
  * 通常 user 會需要管理員權限才能使用 hardware assistance 的功能，如果 VMM run 在普通的 user 上的話，就會失去這項優勢，所以通常 Type 2 Hypervisor 會比其他類型的效能還要低。
  * 但這也有一些好處，Type 2 Hypervisor 在各種general-purpose OS 上運行，並且運行它們不需要對 host OS 進行任何更改。例如，學生可以使用 Type 2 Hypervisor 來測試非本地 OS，而無需替換本地 OS。事實上，在 Apple 筆記本電腦上，學生可以擁有 Windows、Linux、Unix 和不太常見的 OS 版本，所有這些版本都可用於學習和實驗。
 * Paravirtualization(準虛擬化，半虛擬化)：
   * 必須要修改 guest OS Kernel，並且增加Hypercall
   * 由於某些 Non-Virtualizable OS 指令不能被 hypervisor trap，所以 Guest OS 利用 Hypercall 呼叫 Hypervisor 對缺少的指令進行更換，這也是為何要修改 OS 的核心
   * Host OS則不用模擬CPU，Guest OS自行互相調配
   * [參考資料](http://wiki.csie.ncku.edu.tw/embedded/xvisor)
* Programming-Environment Virtualization：
  * 例如，Java 虛擬機(JVM)
  * 直譯語言（Interpreted language）也是透過這種技術運行的。
* Emulation：
  * 當需要非常不一樣的系統架構時，就可以使用模擬器(Emulation)
  * 舊有的程式可以透過模擬器來模擬新的指令，以延長程式的壽命
  * 缺點是速度非常慢，因為本質是就是使用軟體編寫了整個 cpu
  * 即便如此，該技術在遊戲圈還是非常受歡迎。
* Application Containment：
  * 在某些情況下，虛擬化的主要目標是提供一種隔離 application 管理其效能和 resource use，並建立簡單的方法來啟動、移動、停止它們。
  * 這種情況下不需要完整的虛擬化，可以改使用 application containment。
  * 例如 Oracle Solaris 10版的 containers 或 zones，它在 OS 及 應用程式之間多了一層虛擬層，每個 container 都有自己的 applications, network stacks, network address, ports, user accounts 等等 
  * CPU 跟 memory 等資源被劃分並且分給每個 container
  * 這種虛擬化非常輕量級，使用資源更少，啟動及銷毀速度更快。
  * FreeBSD 是第一個提供類似功能的作業系統(稱為 jails)
  * Linux 於2014年增加 LXC container 功能。[The source code for LXC](https://linuxcontainers.org/lxc/downloads)


<h2 id="0176">Virtualization and Operating-System Components</h2> 

* CPU Scheduling
  * VMM 會將 physical CPU 適當的分配給每個 guest OS，當有足夠多的 CPU 的時候，VMM 可以幫 guest 指定一個專用的 cpu，並且將這個 cpu 只排班給該 guest。
  * VMM 本身需要一些 CPU 週期來進行 guest 管理和 I/O 管理 在沒有足夠多的 cpu 的情況下，VMM 可以使用標準調度算法在每個 thread 上取得進展，例如，如果有 6 個 CPU 但是要分12個給 guest，VMM 可以按比例分配 CPU 資源，給每個 guest 認為它擁有的 CPU 資源的一半。 VMM 仍然可以將所有 12 個虛擬 CPU 呈現給 guest，但在將它們映射到物理 CPU 時，VMM 可以使用其排班程式適當地分配它們。
* Memory Management：
  * VMware ESX 使用多種 memory 管理方法。VMM 必須確定每個 guest 應該使用多少實際 memory。為此，VMM 首先評估每個 guest 的最大 memory szie。通用作業系統不希望系統中的 memory size 發生變化，接下來，VMM 根據為每個 guest 配置的 memory 和其他因素（例如過度使用和系統負載）計算每個 guest 的 real-memory allocation 。然後它使用下面列出的三種 low-level  機制從 guest 那裡回收 memory：
    * Double paging：guest 有自己的記憶體管理方法跟 page-replacement algorithms，VMM 也有自己的 page-replacement algorithms，並將 page 載入到客戶認為是 physical 的 backing store，會 swap out 兩次，因此會嚴重導致效能下降。
    * 讓 VMM 在每個 guest 中安裝 pseudo– device driver(偽設備驅動程式)，偽設備驅動程式使用設備驅動程式的 interface，在 kernel 看來是設備驅動程式，但實際上並不控制設備。相反，這是一種無需直接修改 kernel 即可添加 kernel-mode code的簡單方法。例如：安裝一個 balloon memory manager(balloon driver)，當 balloon driver 被告知要 allocate memory 時(稱為 inflates the balloon，膨脹氣球)，會分配記憶體，並且將分配到的 page 鎖定在 guest 的記憶體上(不會被 swap out)，對於 guest 來說，這些鎖定的 page 似乎會減少其可用的 physical memory，從而產生記憶體壓力，但是因為 guest 偵測到自己的 physical memory(它以為的) 正在減少，就會開始釋放一些其他的記憶體位置，同時，VMM 知道被 balloon driver 鎖定的 page 將永遠不會被使用，從 guest 中刪除這些 physical memory 並將它們分配給另一個 guest(來膨脹氣球)。同時，guest 也可以使用自己的 memory management 和 page-replacement algorithms 來管理可用記憶體，這是最有效的方法。如果整個系統內的記憶體壓力降低，VMM 將告訴 guest 內的 balloon driver 取消鎖定並釋放部分或全部記憶體，允許 guest 使用更多 page。
    * 第三種方法是，讓 VMM 確定同一個 page 是否已載入多次，然後將 page 的 copy 數減少為一個，並將該 page 的其他使用者 map 到該 copy。舉例來說，VMware 會隨機採樣 guest 的記憶體，並為每個採樣的 page 建立一個雜湊值，該雜湊值可以視為是該 page 的 thumbprint(縮略圖，指紋?)，並且儲存在一個 hash table 當中，每當採樣都會與 hash table 當中的雜湊值比較，如果 match 的話會再 byte by byte 的仔細檢查是否真的完全相同，如果完全相同的話，則釋放其中一個相同的 page，並且將釋放的 map 到另一個相同的 page，
* IO：
  * 虛擬化技術利用驅動程式可以 dynamically loaded and unloaded 的靈活性來向 guest OS 提供特定的虛擬化裝置。
  * VMM 在提供 guest  的 IO 方式上有很大的差異：
    * 一種方法是提供一個給某個 guest 專用的 IO device
    * 或者 VMM 具有將 guest map 到 IO device 的 driver
    * 第三種可能是 VMM 提供一個對 guest 來說很理想的驅動程式，在這種情況下 guest 看到的是 easy-to-control 的裝置，但事實上 VMM 提供的這個理想的驅動程式的 request 會再發送到更複雜的真實裝置裡的複雜的驅動程式，
  * 雖然指定一個 IO device 只能被一個特定的 guest 存取，會造成其他 guest 無法存取這個 device，但是這種直接存取的方式卻可以更少的讓 VMM 介入，因此也就會有更好的效能。
  * 類似 Intel 的 VT-d 的 hardware support 的技術可以讓 虛擬機的效能接近 host。
  * 網路的方面，運行 VMM 的 server 可能會具有數十個 IP 位址，並且由 VMM 充當網路封包的虛擬交換機給 guest。
    * bridging：客戶可以經由 IP 位址直接連到網路
    * NAT：NAT 位址對於 running guest 的 derver 是區域性的。
* Storage Management：
  * 即使是標準的多重啟動(standard multiboot)方法，將 boot disk 分割成多個 partition，在一個 partition 中安裝 boot manager，並在另一個 partition 中安裝彼此的 OS 還是不夠的，因為 partitino 有一些限制，會阻止它一次運作數十或數百個虛擬機，解決此問題的方法取決於 VMM 的類型。
    * Type 0 Hypervisor 通常允許對 root disk 進行分割，部分原因是這些系統往往比其他系統運行更少的 guest。或者， 可以是 control partition 可以充當 disk manager，並且該 disk manager 可以為其他 partition 提供磁碟空間（包括 boot disk）。
    * Type 1 Hypervisor 將 guest 的 root disk 跟配置資訊儲存於一個或多個 VMM 提供的檔案系統上
    * Type 2 Hypervisor 將相同的資訊儲存在 host 的檔案系統當中，一個 *disk image* 包含了所有 root disk 的內容，並被包含再一個 VMM 的檔案當中，這種方法簡化了移動和複製客戶機(guest)的動作。
  * guest 有時需要比其 root disk image 中可用的更多磁碟空間。例如，非虛擬化資料庫伺服器可能使用分佈在許多 disk 上的多個檔案系統來存儲資料庫的各個部分。虛擬化這樣的資料庫通常涉及建立多個檔案並讓 VMM 將這些檔案作為 disk 呈現給 guest。guest 然後照常執行，VMM 將來自 guest 的 disk I/O request 轉換為檔案 I/O 命令到正確的檔案。
  * physical-to-virtual (實體到虛擬，P-to-V)：這種轉換讀取物理系統 disk 的 block，並將它們存儲在 VMM 系統上的檔案中或 VMM 可以存取的共享存儲體上。
  * virtual-to- physical (虛擬到磁體，V-to-P)：可以將 guest 轉換成實體系統，這可以用於 debug(例如排除虛擬化的問題原因)。
* Live Migration
  * live migration(即時遷移)不能實現於一般的作業系統，而只能實現於 type 0 和 type 1 hypervisors，這種功能可以讓正在 running 的 guest 從一個系統移動到另一個系統當中，並且受到的影響非常的小，以至於 user 不會發現。
  * 通常 VMM 要 migrates 一個 guest 會有以下步驟：
    * source VMM 跟 target VMM 建立連線，並且確認允許傳送 guest。
    * target VMM 會建立一個新的 guest，包含了新的 VCPU、nested page table 以及 storage 的狀態。
    * source VMM 傳送所有的 read-only memory pages 到 target。
    * source 傳送所有的 read– write pages。
    * 重複上一個步驟，因為傳送的過程中一些 page 可能會被 modify，那就要再傳送一次。
    * 當上一個跟上上一個步驟的執行 cycle 少於一定的數量時，source VMM 會 freeze guest，然後傳送 VCPU’s final state, other state details, 還有 final dirty pages，並且告訴 target 可以開始 run guest 了，target 開始 run 之後會通知 source 然後 source 就會終止。
  * 一些 details and limitations：
    * physical MAC address 不能改變，但虛擬化的必須要可以，因此 migrate 的時候才不會需要重置網路，而較新的 switch 也會隨著 MAC 的變動跟著動態傳遞封包。
    * 另外一個限制是不傳輸磁碟狀態，因為磁碟容量太巨大了，disk 必須透過網路來存取才能解決這個問題，通常 NFS, CIFS 或者 iSCSI 用於存儲虛擬機映像和 guest OS(process) 需要存取的任何其他存儲體。
  * Live migration 這項技術可以為 database 帶來很多的好處，當一台機器的 loading 太重的時候就可以把一些 guest migrate 到另一個機器，這樣可以更好的冷卻機房，還有省電。

<h2 id="0177">Examples</h2> 

* VMware Workstation 和 Java 虛擬機通常可以在前面章節中討論的任何設計類型的作業系統之上運行。
* VMware Workstation 
  * 是一種流行的商業應用程式，它將 Intel x86 和兼容硬體抽象為獨立的虛擬機。
  * 屬於 Type 2 hypervisor
  * guest 看到的 "physical disk"其實就只是在 host 的 file system 上面的一個 file
  * VMware Workstation 架構如下圖：
    * ![vmware_workstation](https://github.com/a13140120a/Operation_System/blob/main/imgs/vmware_workstation.PNG)
* The Java Virtual Machine：
  * Java 屬於 programming-environment virtualization 的一個範例
  * 對於每個 Java class，編譯器生成一個與體系結構無關的bytecode output (.class檔)，該檔案將在任何 JVM 上運行。
  * JVM 由一個 class loader  和一個執行 .class檔的 Java 直譯器組成。
  * 當載入一個 class 後，驗證程式會檢查 .class 檔是否是有效的 Java bytecode，並且它不會 overflow 或 underflow stack。它還確 bytecode 不執行指標運算，這可能會造成違規存取記憶體。如果通過驗證，則由 Java 直譯器運行。 
  * JVM 還通過執行垃圾收集(garbage collection)來自動管理記憶體，從不再使用的 object 中回收記憶體並將其返回給系統。許多研究集中在垃圾收集算法上，以提高虛擬機中 Java 程式的性能。
  * JVM 可以在 host 作業系統（例如 Windows、Linux 或 macOS）之上的軟體層中實現，或者作為 Web 瀏覽器的一部分實現。或者，JVM 可以在專門設計用於運行 Java 程序的晶片上的硬體中實現。
  * 一種更快的技術是使用即時 (JIT) 編譯器。在這裡，第一次調用 Java 方法時，該方法的 bytecode 被轉換為主機系統的本地機器語言。然後把這些操作 cache 住，以便使用本機機器指令執行方法的後續調用，並且不需要重新編譯。在硬體中運行 JVM 可能比 JIT 更快。


<h2 id="0178">Virtualization Research</h2> 

* 函式庫作業系统（Library Operating System，LibOS）是應用程式的特殊需求，由某一高級程式語言將原本屬於作業系統 kernel 的某些資源管理功能，如檔案、硬碟 I/O、網路等，按照模組化的要求，以函式庫的形式提供给應用程式的特殊作業系统。[參考資料](https://getiot.tech/dictionary/libos.html)
* 虛擬化研究已經擴展到涵蓋機器虛擬化的許多其他用途，包括運行在 library operating systems (函式庫作業系統)上的 microservice(微服務)，以及在 embedded systems 上面的 secure partitioning of resources。
* unikernels：
  * 建立於 library operating systems 的一個專案，旨在提高這些環境中的效率和安全性。
  * 它是一種特製的 "machine image"，使用單一地址空間，可縮小已部署應用程式的 attack surface 和資源佔用。
  * 他們將應用程式、它調用的系統函式庫以及它使用的 kernel service 編譯成一個在虛擬環境中（甚至在裸機上）運行的二進製檔案。


****




<h1 id="018">Networks and Distributed Systems</h1> 

  * ## [Advantages of Distributed Systems](#0181) #
  * ## [Network Structure](#0182) #
  * ## [Network and Distributed Operating Systems](#0183) #
  * ## [DFS Naming and Transparency](#0184) #




<h2 id="0181">Advantages of Distributed Systems</h2> 

* Resource Sharing(資源共享)：可以共享各個節點之間的運算、檔案等等
* Computation Speedup(提高運算速度)：在處理大規模資料時，可以藉由多個節點的運算能力來達到加速運算的目的，並且也提供負載平衡(load balacing)。
* Reliability(可靠性)：分散式系統如果一個節點故障了，還有其他許多節點可以持續運作

<h2 id="0182">Network Structure</h2> 

* 一開始提出[OSI 七層網路模型](https://zh.wikipedia.org/wiki/OSI%E6%A8%A1%E5%9E%8B)，後來因為太複雜，所以現今大多使用[TCP/IP的五層(或四層)模型](https://zh.wikipedia.org/wiki/OSI%E6%A8%A1%E5%9E%8B)
  * ![OSI_VS_TCPIP](https://github.com/a13140120a/Operation_System/blob/main/imgs/OSI_VS_TCPIP.PNG)
* 網路又分成LAN(區域網路)以及WAN(廣域網路)，並且透過 router、switch、以及 hub 等等連線起來。
* 區域網路的部分大份都是使用乙太網路(Ethernet CSMA/CD，data link layer)協定。
* 廣域網路的 data link layer 則有 ATM(品質保證 QoS)、MPLS(speedup) 等等
* ARP 協定是一個通過解析網路層位址來找尋資料鏈路層位址的網路傳輸協定，在區域網路內的 host 或 router 利用 arp 封包來找尋不在 cache 的 MAC address。
* 傳輸層的 TCP 是 reliable 的且需要建連線，UDP 則相反。
* 應用層協定包含 FTP、SSH、SMPT、HTTP、Telnet 等等。


<h2 id="0183">Network and Distributed Operating Systems</h2> 

* Network Operating Systems：
  * 現今所有常用的作業系統都屬於網路作業系統(Network Operating Systems)，Network Operating Systems 是指可以經由遠端登入電腦操作以及存取資源。
  * 其功能包括遠端登入(如使用 ssh)、遠端檔案傳輸(如使用 FTP、[SFTP](https://ithelp.ithome.com.tw/articles/10223897))

* Distributed Operating Systems：
  *	Data Migration：假設 A 節點要存取 B 節點上的檔案，有兩種方法，一種是將整個檔案傳送到 A 節點，如此一來 access 便是本地，可以增加效率，或者只傳輸檔案中需要用到那個部分(這個方法比傳輸整個檔案更有效率)。
  *	Computation Migration：process P 可以藉由 RPC 來存取 A 節點上的檔案，或者 P 向 A 發送訊息，接著 A 建立 process Q，由 process Q 處理完之後將結果發送回給 Q(P跟Q可以同時進行)。
  * Process Migration：執行此方案有以下幾個原因：
    * Load balancing
    * Computation speedup
    * Hardware preference(首選硬體)：某些 process 可能更適合在某些 cpu 上面執行
    * Software preference(首選軟體)：某些 process  可能會需要使用到 某些節點才有的軟體
    * Data access：如果需要運算的資料很多，那麼遠端執行可能會比傳輸整個檔案到 local 再執行的效率高上許多。
* World Wide Web 提供 data migration 以及 process migration(java applet/javascript) 的功能


<h2 id="0184">Design Issues in Distributed Systems</h2> 

* Robustness(強健性)：
  * fault tolerant：系統必須要有容錯性(fault tolerant)
  * Failure Detection：可以使用心跳(heartbeat)程式，每相隔一段時間，向另一個節點發送訊息，如果超過某個預定的時間沒有收到訊息，就代表某節點可能發生故障
  * Reconfiguratio：例如如果路由中的某個節點發生故障，系統必須要可以快速的計算出其它的路徑，重新設定 router table。
  * Recovery from Failure
* Transparency(通透性)：
  * 通透性就是可以不必顧慮細節
  * 例如 transparent distributed system(通透的分散式系統)不應區分 local 或者 remote 的 resource。
* Scalability：
  * 必須要有可輕易擴展的能力

<h2 id="0184">Distributed File Systems</h2> 

* The Client– Server DFS Model：
  * 如 NFS、OpenAFS 等等。
* The Cluster-Based DFS Model：
  * 例如 Hadoop、GFS 等等。

<h2 id="0184">DFS Naming and Transparency</h2> 

* 命名(naming)即邏輯和實體之間的映射(mapping)
* 檔案名稱映射到磁碟區塊是經過多個階層的，例如使用者藉由檔案名稱"字串"來開啟檔案，檔案名稱右映射到較低層級的數字id，id又映射到磁碟區塊
* 在 transparent DFS 當中，只是多增加一層到上述的階層當中，在一般的檔案系統當，命名的 mapping 範圍是磁碟區塊，而 DFS 命名範圍還包含了磁碟所在的那台電腦。
* Naming Structures：
  * Location transparency(通透性)：檔案名稱不包括檔案實體儲存位置
  * Location independence(獨立性)：檔案的實體儲存位置更改時，無須更改檔名

****




<h1 id="019">Linux</h1> 

  * ## [Advantages of Distributed Systems](#0191) #
  * ## [Advantages of Distributed Systems](#0192) #
  * ## [Advantages of Distributed Systems](#0193) #
  * ## [Advantages of Distributed Systems](#0194) #




<h2 id="0181">Advantages of Distributed Systems</h2> 

* 











