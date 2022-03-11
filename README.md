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
* ## [pass](#012) # 
* ## [pass](#013) # 
* ## [pass](#014) # 
* ## [pass](#015) # 


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

  * ## [Concept](#0031) #
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

  * ## [Concept](#0041) #
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

  * ## [Concept](#0051) #
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
  * ## [替代方法](#0069) #




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


* Readers-Writers Problem：
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

<h2 id="0069">替代方法</h2> 

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
* phisycal adderss：memory真正看到的，也就是載入MBR(https://github.com/a13140120a/Computer_Organization_And_Architecture/blob/main/README.md#0041)的數值，而此數值通常是CPU發出來的address經由MMU轉換而來的
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
  * ## [ECC](#0111) #
  * ## [ECC](#0111) #
  * ## [ECC](#0111) #
  * ## [ECC](#0111) #
  * ## [ECC](#0111) #
  * ## [ECC](#0111) #
  * ## [ECC](#0111) #
  * ## [ECC](#0111) #
  * ## [ECC](#0111) #




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
  
<h2 id="0112">Direct Memory Access</h2> 

* programmed IO(PIO)：使用昂貴的general-purpose processor來監控資料傳輸的狀態，以及一次一個(或少許) byte傳送資料，非常浪費cpu資源。
* 為了改善上述那種狀況，於是有了DMA的技術，DMA是一種special-purpose的controler，當初始化「資料傳輸」的動作時，CPU會把一個「DMA command block(DMA指令區塊)」寫進記憶體，這個block可能包含了傳輸目的及來源的pointer，還有傳輸的資料長度，也可能包含了更複雜的資料(例如不連續的來源及目的address的列表)
* 這種方法又稱為scatter–gather(分散收集)的方法，這種方法允許通過單個DMA 指令執行多個傳輸，cpu只要把DMA command block的位址寫進去DMA controler裡面，cpu就可以去做其他的事情了，待DMA處理完IO之後會發出interrupt，然後cpu再接手處理。
* 大部分電腦(包含行動裝置)都會有一個簡單的DMA
* 為了防止user program修改存放DMA command那塊memory的內容，一定是存在kernel space，因為user不能去修改它。
* DMA執行IO時，會占用系統匯流排，此時可會影響CPU的效能，但根據實驗證明，整體效能還是比較佳的。
* VDMA(virtual direct memory access)技術可以讓DMA直接使用虛擬記憶體的位址。


<h2 id="0112">Application I/O Interface</h2> 

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

<h2 id="0112">Kernel I/O Subsystem</h2> 

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
  * kernel需要保存有關I/O使用的狀態信息。它通過各種kernel data structure來跟踪網絡連接、字符設備通信和其他 I/O 活動(例如第 13.1.2 節中討論的打開文件表結構)。 
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


<h2 id="0112">Transforming I/O Requests to Hardware Operations</h2> 

* 













