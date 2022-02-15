# Operation_System

* ## [Overview](#001) #
* ## [OS Structure](#002) #
* ## [Process Concept](#003)
* ## [Thread](#004) #
* ## [CPU Scheduling](#005) #
* ## [Synchronization](#006) #
* ## [Storage Management](#005) #
* ## [Protection and Security](#006) #
* ## [Distributed Systems](#007) #
* ## [Special Purpose Systems](#008) #
* ## [Case Studies](#009) # 
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

  *  **Resource allocator**: 能有效的分配及管理資源，並且滿足公平性，即時性等要求。 
  *  **Control program**: 控制user program 的執行以及控制hardware，避免電腦因不當操作產生error 或崩潰。
  *  **Kernel**: 一個always running 的核心程式。
  *  作業系統是屬於event-driven(事件驅動)，也就是他依據命令、應用程式、IO devices以及interrupt來工作。 
<h2 id="0012">History</h2>   

  * 演進: Batch -> Multi-programming -> Time-sharing 
    * batch: 一次只能一個user 執行一個process
    * Multi-programming: 一次可以執行多個process，不會因為IO 而idle 在那邊，使用的是interrupt 的技術，但是沒有跟user 的interactive
    * Time-sharing: 可以來回在很多個process 之間切換，讓人產生好像一次執行多個process的錯覺，可以interactive。  
    * [更多詳細資訊](https://medium.com/@a131401203/%E4%B8%AD%E6%96%87%E7%B3%BB%E4%B9%8B%E4%BD%9C%E6%A5%AD%E7%B3%BB%E7%B5%B1%E6%BC%94%E9%80%B2%E5%8F%B2-610986e9ee3c)

<h2 id="0013">Interrupt</h2>   

* 對於傳統的電腦，IO的運作方式是驅動程式會先載入到控制器中適合的暫存器，然後controler會從暫存器的內容來決定要進行的動作。
  * ![cONTROLER](https://github.com/a13140120a/Operation_System/blob/main/imgs/controler.PNG)
* 現代OS大部分都是Interrupt-driven IO的方式執行IO
  * cpu 發出request 使IO devices工作，自己則可以去執行其他指令，等到controler執行完之後會通知cpu，然後cpu才會繼續處理
* 當CPU發生Interrupt時，會停止正在執行的工作，並依照Interrupt的種類轉換到一個的位址，而這個位址通常是interrupt service routine(中斷服務常式)的起始位址
  * 又或者會有一個interrupt vector處理，cpu會依照這個vector跑去相對應的routine處理interrupt，通常位於記憶體的錢100個位置
  * 有心人士有可能會透過修改interrupt routine 或者是interrupt vector來達成偷取資料(或其他破壞行為)的目的
  * 當ID devices的數量多於interrupt vector的位址時，當發生interrupt時cpu會去traverse，直到找到相對應的routine，處理完之後執行return_from_interrupt並返回之前的狀態。
* 大部分的cpu會有兩條Interrupt Request Lines, 一條是maskable, 一條是nonmaskable。
  * 當cpu偵測到Interrupt Request Line 發出interrupt時，會讀取interrupt的編號，跳轉到interrupt vector中相對應的routine
  * 通常Interrupt 會有prioriy level, priority較高的在處理的時候，priority較低的會被mask掉，相對的priority較低的在處理的時候若有priority較高的interrupt出現則會先被執行。
* Hardware 發出的**signal**導致interrupt, 而software 則是發出error，或是透過system call導致interrupt
* Software的interrupt叫做trap或exception，software產生interrupt之後可以跳轉到自己定義的module當中


<h2 id="0014">Timer</h2>   

* 通常在mother board上
* 每隔一段時間會導致interrupt
* 避免cpu陷入無窮迴圈或卡死
* 用來implement Time-sharing
* 透過load-timer這個privileged instruction可以修改間隔時間

<h2 id="0015">Duel-Mode Operation</h2> 

* 為了防止user program摧毀硬體，OS會分成兩種mode: 分別是user mode跟kernel mode
* 有Hardware support的硬體會有一個bit, 當進入到kernel mode的時候OS會先把這個bit設為0，回到user mode之後會再設回1。
* 只要發生interrupt就代表進入kernel mode
* Privileged instruction只能在kernel mode 時候執行，在cpu設計的時候privieleged instruction就已經定義好了
* 一但在user mode執行priviledged instruction cpu會通知OS使OS產生錯誤訊息並阻擋執行。
* Privileged instruction只能藉由呼叫System Call之後OS觸發interrupt然後轉換到kernel mode執行。

<h2 id="0016">Component 名詞定義</h2>   

* CPU: 執行instruction的硬體,通常內含暫存器以及L1 cache
* Processor: 一個實體晶片，包含一個或多個CPU
* Core: CPU的基本計算單位，或CPU中執行指令和暫存器儲存資料的地方
* Muti-Core: 同一個CPU上包含多個Core
* ![DualCore](https://github.com/a13140120a/Operation_System/blob/main/imgs/DualCore.jpg)

<h2 id="0017">Memory Access Architecture</h2> 

* Uniform Memory Access (UMA):
  * 適合[SMP](#0054)架構
  * 每個processor accesses memory的速度都是一樣的，程式師不用在意程式執行在哪個processor，因為都一樣
* Non-Uniform Memory Access (NUMA):
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

* 一套作業系統服務通常會提供下列功能:
  * UI(使用者介面:User Interface):
    * GUI: 圖形化介面(graphical user interface)
    * CLI: 命令行介面(command-line interface)，
      CLI通常會有一個命令解譯器(command interperter)來解譯指令  
      shell就是一種command interperter  
      command interperter有分兩種，一種是本身含有執行指令的程式碼，另一種是利用尋找檔案的方式(UNIX系統)
    * touch-screen interface
  * Program execution
  * I/O operations
  * File systemmanipulation
  * communication: 
    * error detection: zero devision, over flow等等
  * resource allocation: 分配memory, cpu等等
  * accounting(keep track紀錄等等)
  * Protection ans Security
    * protection: 保護devices不會被互相干擾或者被user破壞
    * security: 保護電腦或系統不被有心人士不當使用

<h2 id="0022">System Call</h2>   

* System Call: 提供作業系統服務的介面，一般以C/C++或組合語言撰寫，其類型可分為以下幾種(以下的system call皆為通用的例子，每種OS的system call都不盡相同):
  * process control: 
    * 執行中的process可能會正常終止(`end()`)或不正常終止(`abort()`)，
    * 也可能需要載入(`load()`)或執行(`execute()`)另一個process, 
    * 如果是mutiprocess的話就會有(`create_process()`)然後把原本程式在記憶體內的**image**保存起來，
    * 為了控制process，例如priority，或執行時間，我們必須要有(`get_process_attributes()`)或(`set_process_attributes()`)，
    * 我們可能想要終止程式(`terminate_process()`)
    * 或者等待事件發生(`wait_event()`)，等待時間到(`wait_time()`)，當事件發生時process應該發出訊號(`signal_event()`)。
    * mutiprocess的process之間含需要有能夠保護資源的lock，通常會包含(`acquire_lock`)和(`release_lock()`)
  * file management: 
    * 首先需要能夠建立(`create()`)和刪除檔案(`delete()`)
    * 建立了檔案之後需要開啟(`open()`)、讀(`read()`)寫(`write()`)及重定位(`reposition`)(檔案倒轉或跳到結尾)，之後還會需要關閉(`close()`)檔案。
    * 檔案的屬性包含檔案名稱、檔案類型、保護碼(protection)、帳號資訊等等
    * 為了此目的還需要取得檔案屬性(`get_file_attributes()`)，設定檔案(`set_file_attributes()`)
  * devices management
    * 裝置可以是實體的(硬碟或其他設備)或者抽象的(檔案)
    * 多使用者的系統需要先對裝置提出(`request()`)，使用完之後就必須將其釋放(`release()`)
    * 一旦request device之後，就可以做讀(`read()`)寫(`write()`)及重定位(`reposition`)，
    * 因為IO devices與檔案系統非常相似，有些作業系統(例如UNIX)會將兩個併在一起，一system call會同時用在檔案跟device上，將IO devices 定義成特殊的檔案或目錄
  * information maintenance:
    * 大多數系統都會有傳回時間(`time()`)和日期(`date()`)的system call
    * 也有一些事傳回系統版本、可用記憶體空間或磁碟空間等等
  * communication:
    * message-passing model:
      * 較慢，適合傳輸較小的數據，需要經過kernel傳遞到另一個process
      * 如同網路之間有hostname, IP address, Process之間也會有process name會轉換成 processID
      * 作業系統會根據id找到該process, 例如(`get_processid()`)以及網路的(`get_hostid()`)
      * id會傳入(`open()`)或者(`close()`)的system call，或傳入(`open_connection()`)或(`close_connection()`)的system call
      * 接受者的process通常會以(`accept_connection()`)
      * 大部分接收連接的process都是daemons(守護程序)，daemons是提供該用途的process
      * daemon 執行(`wait_for_connection()`)，並在連接時甦醒。
      * 通訊的來源稱為client(客戶端)，接收的daemon稱為server,兩者藉由(`read_message()`)及(`write_message()`)來溝通，並透過(`close_connection()`)關閉連接。
    * shared-memory model:
      * 較快，但會有sysnc的問題
      * 由OS分配一個可以共享的記憶體
      * process使用(`shared_memory_create()`)和(`shared_memory_attach()`)來產生和取得其他process的記憶體存取權(thread的話不用額外分配，預設就友共享記憶體空間)
    * 大多數作業系統同時擁有兩種模式
  * protection
    * 包括(`set_permission()`)以及(`get_permission()`)
    * 針對使用者則有(`allow_user()`)以及(`deny_user()`)

* Windows and UNIX system call example:
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

* API(application porgramming interface): OS提供的程式設計介面，API會去呼叫System Call, 一個API可對應到0或多個System Call
  * Linux 的Api可以藉由man api_function查看:例如man read 可以查看read()這個api的資訊
  * UNIX系列的API叫做POSIX
  * Windows 就叫windows api
  * JAVA的API: Java Api(與jvm的interface)

* api與system call比較圖:
  ![api與system call比較圖](https://github.com/a13140120a/Operation_System/blob/main/imgs/sys_api.PNG)

* 傳遞參數至作業系統有三種方式:
  * 使用register
  * 使用存放在memory中的block或table
  * 使用stack

<h2 id="0024">loader and linker</h2>   

* 通常程式以二進制可執行檔(如a.out或 b.exe)的形式存在磁碟中
* 其編譯到載入的過程如下:
  * source code 被編譯成物件檔(目的檔)object file，object file可以被載入到實體記憶體的任何位置，而這種格式被稱為relocatabel object file(可重定位物件檔案)
  * 接下來linker會將這些relocatabel object file組合成單一個可執行(executable)檔
  * 在連結的期間可以指定包含其他的物件檔或程式庫(-l\*就是链接lib\*.a文件，-lm 就是連結C的標準數學函式庫libm.a(靜態函式庫)。  [詳細](https://bbs.csdn.net/topics/20023165))
  * loader 則會將二進制可執行檔載入到記憶體中執行
  * 動態連結函式庫(DLL)允許執行時動態的載入，windows為.dll檔，linux則為.so檔
  * ![compile](https://github.com/a13140120a/Operation_System/blob/main/imgs/c_compile.png)

* 物件檔與執行檔通常具有標準格式:
  * linux系統中，此標準格式為[ELF格式](https://zh.wikipedia.org/wiki/%E5%8F%AF%E5%9F%B7%E8%A1%8C%E8%88%87%E5%8F%AF%E9%8F%88%E6%8E%A5%E6%A0%BC%E5%BC%8F)
  * Windows中，則使用[PE格式](https://ithelp.ithome.com.tw/articles/10187490)
  * Linux當中可用file test.o來查看，以下可以看到test.o為elf 64位元可重定位檔，test為elf可執行檔(也可用readelf進行評估)
    ![file](https://github.com/a13140120a/Operation_System/blob/main/imgs/%E6%93%B7%E5%8F%960.PNG)
    
* 應用程式specify on OS，其原因如下:
  * 每個OS具有不同的二進制格式
  * CPU具有不同的指令集
  * Sustem call不同

* API 是在應用曾架構中指定某些功能，[ABI](https://zh.wikipedia.org/wiki/%E5%BA%94%E7%94%A8%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%8E%A5%E5%8F%A3)(應用二進位介面 application binary interface)則是用於指定底層詳細資訊，包括位址寬度，參數傳遞給System call的方法、執行時堆疊架構等等，一個ABI常見的樣貌即是調用約定(又稱呼叫約定 Calling Conventions)


<h2 id="0025">OS設計與製作</h2>   

* OS的設計目標大致上可分為以下兩種類:
  * user goal(使用者目的): 就使用者而言，OS的設計必須easy to use, easy to learn, reliable, fast, safe
  * system goal(系統目的): 而對於設計、維護OS的人，OS的設計必須要easy to design, implement, maintain, reliable, no error, efficient
* Policy and mechanism:
  * mechanism(機制):決定如何做某些事
  * Policy(策略):決定做什麼
  * 例如作為cpu保護的機制(mechanism)，[Timer](#0012)就是一種策略(Policy)
  * microkernel 為mechanism與Policy分離的極端例子
* 早期作業系統都是由組合語言撰寫，而現今大多都是以C/C++撰寫，少部分較瓶頸的常式(routine)則是用組合語言替換重構，以實現較好的效率。
* Android 使用一種以上的語言撰寫而成，其大部分系統程式庫都是以C/C++撰寫，而應用程式框架以及API則都是以Java撰寫而成。

<h2 id="0026">Structure</h2>   

* Simple OS Architecture(單一結構):]
  * 又稱為緊密耦合(tightly coupled)系統，會因為更改一部分而對其他部分產生廣泛影響。
  * 所謂單一架構其實就是沒有架構，所有功能都放到單一的位址空間中的靜態二進制檔案當中
  * 缺點是難以design, 修改困難，漏洞多，優點是效能非常快，windows and UNIX Linux仍然存在此種架構
  * 原始UNIX系統為此架構
    ![https://github.com/a13140120a/Operation_System/blob/main/imgs/2_12_UNIX_Structure.jpg]
* Layered OS Architecture(分層架構)
  * 又稱為鬆散耦合(loosely coupled)，系統可分為許多具有特定功能的較小單獨元件
  * 速度較慢，但更好更改系統
  * 是模組化(Modular)的方法之一
  * 第0層為硬體，最高層為使用者介面
  * 非常好除錯與維護，因為每層只能使用較低層的服務
  * 分層系統常用於網路(如TCP/IP)
  * ![Laered_OS](https://github.com/a13140120a/Operation_System/blob/main/imgs/2_13_Laered_OS.jpg)
* Microkernel
  * kernel 的部分只保留一些基礎(如記憶體管理和process),其他都在使用者空間
  * 主要採用message passing 溝通
  * 優點是容易擴展，所有新的service 都加入使用者空間，所以不需修改核心，而當核心修改時，因為核心很小，所以不會牽涉到太多東西
  * 效能非常低弱，因為當兩個service 要溝通時必須要經過kernel, 產生許多複製訊息的動作
  * 最有名的Darwin是macOS 和IOS 的核心，Darwin是由兩個kernel所組成，其中一個是Mach
    ![microkernel](https://github.com/a13140120a/Operation_System/blob/main/imgs/2_14_microkernelArchitecture.jpg)
* Modules(模組化):
  * kernel提供主要服務，其他服務則在執行時被動態載入，就不必每次將新特性加入kernel都必須從新編譯
  * 比Layered OS更有彈性，因為任何module都可以呼叫其他module
  * 全部的module都在kernel裡面，所以速度不會太慢。
  * kernel裡面會有system table, 會有許多空的entry, 可以讓使用者insert
  * linux 也適用該技術，稱為[LKM](https://zh.wikipedia.org/wiki/%E5%8F%AF%E8%BC%89%E5%85%A5%E6%A0%B8%E5%BF%83%E6%A8%A1%E7%B5%84)
    ![Solaris_Modules](https://github.com/a13140120a/Operation_System/blob/main/imgs/2_15_Solaris_Modules.jpg)

* 混和系統
  * 作業系統通常不會使用單一的系統結構，會組合不同架構，以強調性能、安全及可用性等問題
  * Linux是單核心，但模組化
  * Windows是單核心，但保留一些microkernel架構，支援subsystem，但同時也可以動態載入module

* macOS and IOS
  * user experience: 該層定義設備與使用者之間的互動，macOS使用的是[Aqua](https://zh.wikipedia.org/wiki/Aqua_(GUI))，IOS使用的則是[Springboard](https://zh.wikipedia.org/zh-tw/SpringBoard)
  * application frameworks: 這層包刮了用於開發MacOS應用程式的Cocoa 以及開發OS的Cocoa Touch framework，他們為Objected-C以及swift編程語言提供API。
  * core frameworks: 定義了支援圖形和多媒體的框架，包括Quicktime和OpenGL
  * Kernel enviroment: 這種環境也稱為Darwin, 包括Mach為核心和[BSD](https://zh.wikipedia.org/wiki/BSD) UNIX核心 
  * 這樣的結構可以直接利用各層的優點  
  <br>
  
    ![macOSiOS](https://github.com/a13140120a/Operation_System/blob/main/imgs/2_16macOS%2BiOS.jpg)  
  * Darwin是一個分層系統，由Mach微核心 和BSD UNIX核心組成，Mcah處理trap, BSD kernel處理POSIX system call
  * Darwin還提供了用於開發驅動程式和動態載入module的[kexts](https://support.apple.com/zh-tw/guide/deployment/depa5fb8376f/web)  
    ![D](https://github.com/a13140120a/Operation_System/blob/main/imgs/2_17Darwin.jpg)

* Android:
  * 由google 領導開發
  * 開發人員使用Java程式語言開發，但不使用標準Java API, Java program被編譯成可在ART(Android Runtime)上執行的程式，針對有限的記憶體和cpu做了優化。
  * Java首先會先被編譯成.class檔，然後轉成可執行檔.dex檔
  * 一般的JVM會有JIT的機制，而Android的Java則是使用AOT(ahead-of-time提前編譯)技術
  * 開發人員還可以使用[JNI](https://developer.android.com/training/articles/perf-jni)技術，JNI允許跳過虛擬機直接存取硬體，缺點是不能移植到不同的硬體平台。
  * 包含[HAL](https://zh.wikipedia.org/wiki/%E7%A1%AC%E9%AB%94%E6%8A%BD%E8%B1%A1%E5%B1%A4)層，使開發人員可以在不同的硬體平台上移植程式。
  * 一般Linux使用的是GNU的C庫(glibc), 而Android則是用[Bionic libc](https://zh.wikipedia.org/wiki/Bionic_(%E8%BB%9F%E9%AB%94))函式庫
  * 
    ![android_kernel](https://github.com/a13140120a/Operation_System/blob/main/imgs/2_18Android.jpg)

* [WSL](https://hackmd.io/@billsun/Bkh8oAmGX?type=view): 使windows 可以允許Linux的ELF執行檔在Windows 10 上執行。

<h2 id="0027">系統啟動</h2>  

* [啟動程式](https://zh.wikipedia.org/wiki/%E5%95%9F%E5%8B%95%E7%A8%8B%E5%BC%8F):
  * BIOS的作用是初始化和測試硬體元件，以及從大容量儲存裝置（如硬碟）載入啟動程式，並由啟動程式載入作業系統
  * 許多BIOS程式都只能在特定電腦型號或特定主機板型號上執行。早年，BIOS儲存於ROM晶片上；現在的BIOS多儲存於快閃記憶體晶片上，這方便了BIOS的更新。
  * 最近許多電腦系統已經使用[UEFI](https://zh.wikipedia.org/wiki/%E7%B5%B1%E4%B8%80%E5%8F%AF%E5%BB%B6%E4%BC%B8%E9%9F%8C%E9%AB%94%E4%BB%8B%E9%9D%A2)取代[BIOS](https://zh.wikipedia.org/wiki/BIOS)
  * GRUB是一個用於UNIX系統的開源啟動程式，非常靈活，可在啟動時修改kernel參數，甚至可以選擇啟動不同的kernel，例如Linux的/proc/cmdline中的BOOT_INAGE是要載入到記憶體的核心映象檔的名稱，而root指定根檔案系統的唯一標識符
  * Android不使用GRUB，最常見的啟動程式是[LK](https://baike.baidu.hk/item/lk/623671)  [(LK詳細)](https://www.itread01.com/content/1502160009.html)

<h2 id="0028">除錯</h2>  

* core dump: 當程序運行的過程中異常終止或崩潰，作業系統會將程序當時的內存狀態記錄下來，保存在一個文件中，這種行為就叫做Core Dump
* crush dump: 當系統crash 的時候就要 collecting data for crash dump
* /proc是存在kernel中的偽檔案系統，/proc/2155將包含ID為2155的process 的訊息
* 可用`ps`查看process狀態，`top`查看process即時訊息，`vmstat`查看系統記憶體使用情況，`netstat`查看網路，`iostat`查看磁碟IO狀況
* `strace`可以查看任何process調用的system call,例如`strace ls`可查看ls調用的那些system call以及狀況，[gdb](https://jasonblog.github.io/note/gdb/gdbshi_yong_jiao_xue_ff1a_zi_dong_hua_ni_de_debug.html)指令可以原始碼除錯，`perf`為Linux性能工具包，`tcpdump`網路封包擷取。
* BCC: BCC是[eBPF](https://hackmd.io/@sysprog/linux-ebpf)工具的前端介面，而eBPF是擴展的[BPF](https://zh.wikipedia.org/wiki/BPF)，eBPF可動態插入正在運行的Linux系統，其指令可讀取特定事件(例如正在呼叫某個system call)，或者監視系統效能(例如IO時間)，BCC提供了python的前端介面，並嵌入了eBPF工具連接的C程式碼，而eBPF又與核心連接，BCC提供的許多工具均可用於特定應用程式，例如MySQL, Java或Python 程式

****
<h1 id="003">Process Concept</h1> 

  * ## [Concept](#0031) #
  * ## [Process Creation](#0032) #
  * ## [Process Communication](#0033) #


<h2 id="0031">Concept</h2> 

* process: 執行中的程式(動態，存在memory)
* program: 未執行的程式(靜態，存在disk)
* process的活動可由當前的register以及Program Counter表示
* process的記憶體配置通常會包括以下四個section:
  * text section :存放程式碼，大小固定
  * data section :存放全域變數，大小固定，又分初始化及未初始化區域。
  * heap section :process執行時動態分配的記憶體
  * stack section:呼叫函數時的臨時資料儲存(包括函數參數，return值以及區域變數)
  * ![c_process_memory](https://github.com/a13140120a/Operation_System/blob/main/imgs/c_process_memory.png)
* `size [filename]`可以查看Linux 二進位檔的資訊，data顯示的是未初始化的資料，bss是已初始化的資料，dec跟hex分別表示十進位與16進位的三個階段的總和
* [objdump](https://wangchujiang.com/linux-command/c/objdump.html)是gcc工具，可查看執行檔的反組譯碼，符號表等資訊，反組譯出來的是[AT&T格式](https://www.itread01.com/content/1549963466.html)。
* process的狀態可分為以下幾種:
  * new:process正在產生中
  * ready:已經在memory裡面等待被執行
  * running:正在執行，interrupt之後會回到ready狀態
  * waiting:正在等待event發生，例如等待IO結束或收到signal
  * terminated:已完成
* PCB:每個作業系統都存在一個process control block(PCB)，其所記載之process相關資訊包括:
  * pointer to next PCB
  * process state
  * program counter
  * CPU register
  * Scheduling information:ex priority
  * memory management information:ex base register, limit register, page table, segment table
  * IO status information
  * accounting information: 包括帳號，cpu使用時間及數量，時限等等，開了多少file等等。
  * linux pcb是以C的[task_struct](http://lienguang.com/task_struct/) 結構表示，所有的process是以Double Link List的task_struct表示，[請參考詳細說明](https://www.itread01.com/content/1548075798.html)。
* degree of multiprogramming: 目前在記憶體的process數量
* IO bound:較IO傾向的process
* CPU bound:較CPU傾向的process
* queue: 各種scheduling會放在queue裡面
  * Job queue:早期memory比較有限的時候，決定誰能夠load進memory
  * Ready queue:已經在memory，準備等待被執行
  * Device queue:sleep, interrupt或IO時會被踢進device queue，device queue通常不只一種
* swap in/out: virtual memory放進/岀disk
* context switch:當cpu要切換running process的時候需要把process的資訊記錄下來(如PC以及其他暫存器)，這種動作就叫context switch
* Linux 的systemd 這個process的ID永遠是1，是系統啟動時的第一個user process，然後會再create 許多子process
  * ![linux_tree](https://github.com/a13140120a/Operation_System/blob/main/imgs/linux_tree.png)
  * `ps -el`可以看到系統中所有process的完整訊息:
  * PPID代表parent pid(父process id), [NI及PRI代表的意義請看這裡](https://kknews.cc/zh-tw/tech/j85ngl.html), [其他](https://dywang.csie.cyut.edu.tw/dywang/linuxSystem/node77.html)
  * Linux的init 就是 systemd


<h2 id="0032">Process Creation</h2> 

* create process的時候，其resouece可能是:
  * child共享所有parent的資源(如file handle, io device handler)
  * child共享部分parent的資源
  * child不共享parent的資源
* create process的時候，其執順序可能是:
  * parent繼續與child同時執行
  * parent等child執行完之後才繼續執行
* create process的時候，新process 的記憶體空間有可能是:
  * 完全複製parent的process跟資料例如(UNIX的fork()):[example](https://github.com/a13140120a/Operation_System/blob/main/linux_create_process_example.c)
  * create的同時直接將program載入進去child。(例如windows的CreateProcess()):[example](https://github.com/a13140120a/Operation_System/blob/main/win_createprocess_example.cpp)
* parent會因為以下原因殺死child:
  * child使用超過分配的resource
  * 工作結束
  * parent結束
* 呼叫system call `exit()`來結束process
* 殭屍進程:child完成執行(通過exit()，或運行時發生致命錯誤或收到終止信號所致)，但在作業系統的進程表中仍然存在其PCB，其state處於terminated狀態的process，通常會發生在chhild需要保留表項以允許其parent讀取child的退出狀態，也就是在等待parent call `wait()`的process稱為zombie process(殭屍進程)，如果parent沒有呼叫wait就結束的話，child就會變成孤兒進程(orphan)，這時候Linux就會把該orphan設定為init(systemd)的child，並由init處理。

<h2 id="0033">Process Communication</h2> 

* Purpose:
  * information sharing
  * computation speedup: 多執行緒，multicore等等要加速運算就需要彼此溝通
  * modularity: 就像modular的kernel一樣，module, subsystem都需要彼此溝通
* google chrome是一個multiprocess的例子，其分成以下三個process
  * browser:process負責管理UI以及磁碟和網路IO
  * renderer(渲染器):process負責處理呈現的網頁邏輯，包含HTML, JasaScript, 影像等邏輯，每個新的分頁都會有一個renderer
  * plug-in(插件):例如flash, QuickTime
* IPC(interprocess communication)指的是process彼此之間的溝通
* producer生產資訊，comsumer消耗資訊。
* shared memory IPC:
  * 製作comsumer跟producer的共通buffer:
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
  * POSIX shared memory的範例:[producer](https://github.com/a13140120a/Operation_System/blob/main/posix_producer.c)，[consumer](https://github.com/a13140120a/Operation_System/blob/main/posix_comsumer.c)，gcc command`gcc -o consumer posix_producer.c -lrt`
* message passing IPC:
  * 至少具備以下兩種操作:
    ```
    send(message)

    receive(message)
    ```
  * process P跟process Q溝通，則communication link(通訊鏈) implementation可分為以下幾種:
    1. Direct or indirect communication:
       * Direct communication: 
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
       * indirect communication:
         * mailbox可能是為process或者OS所擁有，如果process是owner，那麼只有owner可以receive。
         * 藉由一個mailbox(或稱port)，
         * 每個mailbox都有一個id，只有雙方有共用的mailbox的時候才能溝通
         * 一個link可以和兩個以上的process相結合，
         * 假設P1,P2,P3共用一個mailbox, 當P1傳送訊息至A,而P2,P3都要執行來自A的receive()，有幾種方法可以決定誰可以收到訊息:1. 允許一個link最多只能與兩個process結合(簡化成1to1)。2. 同一時間只有一個process可以call mailbox的receive()。3. 為了選擇能接收的process而使用某種演算法。
    2. blocking(sychronous 同步) or nonblocking(asychronous 非同步):
      * blocking send: 等待直到return
      * nonblocking send: 不等待直接return
      * blocking reveive: 等待直到接到訊息
      * nonblocking reveive: 不論有無都不等待
    3. buffer:
      * zero capacity:buffer size為零，此種情況下，producer必須等待直到comsumer收到資料。
      * bounded capacity:有限buffer size，當buffer滿時producer需等待，當buffer空時comsumer需等待。
      * unbounded capacity:無限buffer size，從不阻塞。
* Windows中提供的內部進程間通信方式為[ALPC](https://zh.wikipedia.org/wiki/%E6%9C%AC%E5%9C%B0%E8%BF%87%E7%A8%8B%E8%B0%83%E7%94%A8#%E5%AE%9E%E7%8E%B0)，只能於同一台機器中使用。
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/3_19_ALPC_Windows.jpg)
* Pipe:
  * 早期的UNIX系統所使用的IPC機制
  * Linux pipe:`ls|less`相等於Windows的`dir|more`
  * implement pipe時，有四個議題必須考慮:
    * 允許單向通信或雙向通訊?
    * 若允許雙向通訊，則是半雙工(同時間只能單方向)，還是全雙工(同時間可以雙向)
    * 通信的兩個process是否必須存在關係(parent-child)
    * 是否可以通過網路，或者必須在同一台機器上?
  * Ordinary pipe(普通管道):
    * 必須是是parent-child的關係，所以只能備用在同一台機器上
    * UNIX 的 Ordinary pipe create方法就是`pipe(int fd[])`
    * producer從write end(寫入端)寫入，consumer從read end(讀取端)讀取
    * fd[0]表示write end，fd[1]表示read end
    * UNIX 的Ordinary pipe不能從產生這個pipe外的process存取
    * 基本上，UNIX的parent透過ordinary pipe和`fork()`出來的child溝通
    * UNIX把pipe當成一個特殊的檔案型態，可以使用普通的`read()`或`write()`來操作。
    * 完成通訊之後就會消失
    * [UNIX Ordinary pipe example](https://github.com/a13140120a/Operation_System/blob/main/unix_pipe.c)
  * Windows的Ordinary pipe叫做anonymous pipe(匿名管道):
    * 如同UNIX是parent-child的關係
    * 可以使用普通的`readFile()`或`writeFile()`來操作。
    * create的API是`CreatePipe()`
    * windows anonymous pipe example: [parent](https://github.com/a13140120a/Operation_System/blob/main/win_pipe_parent.cpp), [child](https://github.com/a13140120a/Operation_System/blob/main/win_pipe_child.cpp)
  * Named Pipes(命名管道):
    * 可以雙向，不需是parent-child的關係，可以多個process一起使用。
    * 完成通訊之後依然可以持續存在
    * UNIX的Named Pipes被稱為FIFO, Create的方法是`mkfifo()`,使用檔案系統的`read()``write()``open()``close()`來操作，並會持續存在直到被作業系統刪除，允許雙向通信但使用半雙工，如果要同時雙向傳輸，通常會建立兩個FIFO，必須在同一台機器上。
    * Windows的Named Pipes比UNIX更豐富，允許全雙工，可以在不同機器間傳輸，Create的方法是`CreateNamedPipe()`，client端可以使用`ConnectNamedPiped()`連接到Named Pipes，然後透過`ReadFile()`與`WriteFile()`還通信。

* Socket:
  * 由IP和port number組成
  * 分為TCP以及UDP兩種。
  * port 22為ssh, port21為FTP, port 80為http
  * 只能使用1024以上的port，因為1024以內都定義好了。
  * 127.0.0.1稱為loopback
  * 傳輸的data可以是任何東西，由programmer自行parse
  * Socket 運作方式:
    * ![Socket](https://github.com/a13140120a/Operation_System/blob/main/imgs/20220120.PNG)
* PRC(Remote procedure call)
  * RPC的底層也是使用Socket實現
  * client端會有一個stub的process負責處理parsing，server端則稱為(skeleton)
  * 當client端呼叫一個remote procedure時，RPC會找到適當的stub, 並且mashaling(重排) parameter，封裝成[XDR(External Data Representation)](https://zh.wikipedia.org/wiki/%E5%A4%96%E9%83%A8%E6%95%B0%E6%8D%AE%E8%A1%A8%E7%A4%BA%E6%B3%95)格式，送到server端處理。
  * Windows的stub程式碼是由使用MIDL(微軟介面定義語言 Microsoft Interface Definition Language)所撰寫編譯而成
  * 遇到Pointer的傳輸時會有複雜的解決方法，其中可以參考微軟的[MIDL Pointers and RPC](https://docs.microsoft.com/en-us/windows/win32/rpc/pointers-and-rpc)
  * 概念圖:
    * ![RPC](https://github.com/a13140120a/Operation_System/blob/main/imgs/20220120-2.PNG)
* Android RPC:
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
 * 好處:
   * Responsiveness:可以即時應對user
   * Resource sharing
   * Economy:相較create process可以不用消耗那麼多的時間，記憶體及其他資源
   * Scalability:可以隨意擴增執行緒數量。
* Concurrency(並行)是指藉由快速切換達到好像一起執行的錯覺，Parallelism(平行)才是真正一起執行
* Challenge:
  * Dividing tasks
  * Data spliting
  * Data dependency:(後面依賴前面的資料)
  * Balance
  * Testing and Debugging
* 分成user thread和kernel thread，user thread會傳給kernel thread去執行:
  * user thread:
    * 較快速
    * 例如Pthread(Linux, Mac), Win32 thread, Java thread
  * kernel thread
    * 因為較複雜，較多檢查以及保護機制，所以較慢


<h2 id="0042">Mode</h2> 
* 分成三種mode:
  * many to one:
    * 一個process 指有一個kernel thread(有很多user thread)
    * 優點是因為resource都sharing所以很快，缺點是一旦被blocking會導致系統停滯
    * ![many_to_one](https://github.com/a13140120a/Operation_System/blob/main/imgs/many_to_one.PNG)
  * one to one:
    * 一個thread被blocking的話，對其他thread沒有影響
    * 缺點是消耗太多資源，但現在電腦不缺資源，所以現代大多數OS(包括Linux和Windows)都是使用此種模式。
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/1to1.PNG)
  * Many to many:
    * 可以有many to one的resource sharing的快速的優點
    * 又可以有one to one那種不容易被blocking導致系統停滯的缺點
    * 難以實現，問題出在於如何mapping, 如果使用情境很單純，mapping的演算法可能會導致產生過多的overhead。
    * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/many_to_many.PNG)

<h2 id="0043">Library</h2> 

* 又分成asynchronous threading(非同步)和synchronous threading(同步)
  * asynchronous threading:不必等child thread結束，才能繼續
  * synchronous threading:必須等child thread結束，才能繼續
* [Pthread example](https://github.com/a13140120a/Operation_System/blob/main/pthread_exm.c), gcc command:`gcc -pthread -o pthread_exm pthread_exm.c`
* [Windows thread example](https://github.com/a13140120a/Operation_System/blob/main/win_createthread.c)

* Implicit Threading: 由compiler和runtime libraries負責管理與新增執行緒，並非由程式負責，分成以下幾種方法:
  * Thread pool
    * 先Create 一個thread，節省去產生一個thread的成本。
    * 可以把資源使用量控制在一定的範圍之內
    * Window threadpool: [QueueUserWorkItem](https://docs.microsoft.com/en-us/windows/win32/api/threadpoollegacyapiset/nf-threadpoollegacyapiset-queueuserworkitem)
    * Java threadpool
  * Fork-join: thread並不是在fork的時候被create，而是在task真正執行的時候才被create，library負責分配建立thread的數量，以及分配task給thread
    * java的fork join: `ForkJoinPool()`
  * [OpenMP](https://zh.wikipedia.org/wiki/OpenMP):是一套支援跨平台共享記憶體方式的多執行緒並行的編程API，支援C,C++和Fortran
    * example:
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

  * GCD(Grand Central Disoatch):MacOS和IOS作業系統的一項技術
    * 和OpenM類似，允許開發人員區分出要平行執行的程式碼區塊
    * GCD會將區塊放進dispatch queue, 當一個區塊從queue pop出去時，會分配thread pool中的一個thread給這個區塊執行
    * 在GCD內部的thread pool由POSIX組成，並由GCD管理pool決定增減thread數量
    * GDC由libdispatch這個程式庫實現，並且以一直到FreeBSD作業系統當中
    * dispatch queue又分成serial和concurrency，serial一次只能pop一個區塊，而concurrency一次可以pop多個區塊並且可以同時執行，兩者都是以FIFO順序pop
      * concurrency又分成以下幾種類別:
        * QOS_CLASS_USER_INTERACTIVE:與USER互動
        * QOS_CLASS_USER_INITIATED:需要USER來啟動，但並不需要像USER_INTERACTIVE一樣快速回應
        * QOS_CLASS_UTILITY:需要較長時間才能完成，但不要求立即獲得結果
        * QOS_CLASS_BACKGROUND:對時間的敏感度不高，例如:信箱備份
    * 對於C/C++/Objective-C語言的簡單區塊範例:
      * ```C
        ^{ printf("hello block!"); }
        ```
    * 對於Swift而已，以下範例示範將task push進dispatch queue:
      * ```C
        let queue = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0)
        dispatch_async(queue, { printf("hello block!") })
        ```
  * [TBB(Intel threading building blocks)](https://en.wikipedia.org/wiki/Threading_Building_Blocks):
    * 是一個 C++ template library，TBB提供任務排班、負載平衡以及支援快取。
    * 也提供平行迴圈、Atomicity操作、互斥鎖等豐富功能。
    * 範例:
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
      
* fork and exec:
  * UNIX的`fork()`有兩種版本: 1. 複製process的所有thread(整個process)。 2. 指複製呼叫`fork()`的那個thread。
  * 而`exec()`則是會取代整個process(包括所有thread)，所以如果呼叫上2的`fork()`再呼叫`exec()`是沒有意義的。
* signal:
  * 所有的signal都遵循相同的過程: 由於event發生而產生 > 產生的signal被送到一個process > 送達後交給signal handler處理
  * signal又分成同步跟非同步，同步包括:記憶體存取違規或除以0，非同步包括ctrl+c或者 timer時間到了。通常非同步signal會被送到另一個process
  * signal handler 又分成default signal handler 以及 user-defined signal handler，default signal handler是在kernel執行，但有可能被user-defined所覆蓋。
  * signal有可能會被傳送到以下幾種選擇:
    * 把訊號傳到有應用的執行緒上。
    * 把訊號直接傳到所有的執行緒上。
    * 把訊號傳到行程中特定的執行緒中。
    * 選一個執行緒，負責接收所有的訊號。
  * [Windows APC](https://docs.microsoft.com/zh-tw/windows/win32/sync/asynchronous-procedure-calls):windows沒有signal，但有APC，允許使用者執行緒設定一個函數, 當一個event發生時就呼叫此函數
* Thread cancellation: 意思是一個thread在完成工作之前結束他，例如多執行緒搜尋資料庫，其中一個找到，其他就都該結束，被取消的thread稱為target thread, target thread又可分成兩種:
  * Asynchronous cancellation(非同步取消):立即終止，有可能會無法釋放所有資源
  * Deferred cancellation(延遲取消):會執行到一個check point才取消，例如:ctrl+c，允許執行緒安全的被取消。
  * Pthread: `pthread_cancel(tid)`或`pthread_cancel(pthread_self())`，pthread支援[三種取銷模式](https://man7.org/linux/man-pages/man3/pthread_setcancelstate.3.html)
  * java的Thread cancellation只要把thread的interrupt狀態設成true就可以了`Thread worker; worker.interrupr()`，檢查:`while(!Thread.currentThread().isInterrupr())`
* TLS(Thread local storage): 
  * 每個thread的各自的獨一無二的資料，不會干擾或影響到別人
  * local variable：只能在自己的function中被看見，而TLS中的資料可以被自己以外的function看見。
* Schedular Activations:
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
  * context: 儲存處理器表現狀態的暫存器組+stack+私有儲存區域
  * 在Windows中thread的主要資料結構：
    * ETHREAD：包含thread所屬process的指標，執行緒起始位址，還有一個指標指向KTHREAD。
    * KTHREAD：包含排班跟同步的資料，kernel stack跟指向TEB的指標。
    * TEB(Thread Envrionent block)：包含ID、use stack、TLS。 
* Linux Thread:
  * Linux沒有區分process跟thread，在Linux中，threads被稱作為tasks。
  * Linux not support mulltithreading，而是使用Pthread實現user-level的thread
  * `fork()`create a new process and **copy** assciated data of parent process
  * `clone()`create a new process and **link** assciated data of parent process(有些memory是share的)，clone 中第三個參數flag有以下幾種選擇
    * CLONE_FS: 共用檔案系統訊息
    * CLONE_VM:share相同的Memory space
    * CLONE_SIGHAND:共用signal handler的函數
    * CLONE_FILES:共用開啟的檔案
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

* CPU burst: 一連串的instruction都在使用CPU的狀態
* IO burst : 一連串的instruction都在做IO的狀態
* CPU bound: CPU burst較多的(process)
* IO bound : IO 較多的(process)
* CPU 的Schedule發生在以下四種狀況  
  1.從running到waiting(例如發生IO)  
  2.從running到ready(例如發生interrupt)  
  3.從waiting到ready(例如IO結束)  
  4.terminates  
* non-preemptive: CPU 的Schedule只發生在上述1跟4的情況，non-preemptive的kernel結構較簡單，對於即時計算較差
* preemptive: CPU 的Schedule可以發生在上述所有的狀況，需要額外的方法防止race condition(競爭情況)
* Linux環境可使用[vmstat](https://charleslin74.pixnet.net/blog/post/434428316)來查看每秒context switch以及interrupt的數量，例如`vmstat 5 6`每五秒統計一次，總共統計六次。
* Linux環境可以利用`cat /proc/254/status`來查看pid=254的process狀態，最後兩行顯示voluntary-ctxt-switches(自願context switch，如IO)以及nonvoluntary-ctxt-switches(非自願，如被preempt)數量。
* ready queue並不一定是FIFO queue。

<h2 id="0052">Scheduling Algorithms</h2> 

* Scheduling Algorithms通常有以下幾種評估標準(Scheduling Criteria):
  * CPU utilization:理論上0~100%，實際上會介於40~90%，可用[top](https://david50.pixnet.net/blog/post/45252072-%5B%E7%AD%86%E8%A8%98%5Dlinux---top%E8%B3%87%E8%A8%8A)查看
  * Throughput: (以系統的角度)每單位時間內完成的工作量，對於cpu scheduling而言就是每單位時間完成的process數量。
  * Turnaround Time: (以single job的角度)submission~cpmpletion的時間
  * wait time:在ready queue的時間
  * response time: (對於interactive很重要)從submission到第一個cpu burst開始的時間。

* FCFS(First Come First Serve):
  * 製作容易，average wait time通常很長，並且會因為cpu burst的變化而有極大差異
  * 不管是non-preemptive還是preemptive都不會改變結果，一樣都是先來先做完。
  * ![fcfs_scheduling](https://github.com/a13140120a/Operation_System/blob/main/imgs/fcfs_scheduling.PNG)
* SJF(Shortest Job First):
  * 可以證明是最小wait time的演算法
  * non-preemptive跟preemptive會得到不同的結果
  * 無法預知下一個cpu burst的長度(但可以使用一些time serious的方式預測，例如[EMA](https://zh.wikipedia.org/wiki/%E7%A7%BB%E5%8B%95%E5%B9%B3%E5%9D%87#%E6%8C%87%E6%95%B8%E7%A7%BB%E5%8B%95%E5%B9%B3%E5%9D%87))
  * [詳細解說](https://www.youtube.com/watch?v=scp5vRE3yVw&list=PL9jciz8qz_zyO55qECi2PD3k6lgxluYEV&index=32)
* RR(Round robin):
  * 每個process會得到一個固定的time quantum，通常是10~100毫秒。
  * 時間越短，overhead越高，時間越長則就變成FCFS
* Priority Scheduling:
  * 每個process會有一個priority, priority越高則越先執行
  * Priority Scheduling也有分non-preemptive跟preemptive，non-preemptive的話在priority較高的process出現時，較低的會被preempt
  * priority可以是內部或外部定義的，內部的包含依據時間限制、記憶體需求、開啟檔案數量、IO burst與CPU burst的比例來計算priority，外部則是由作業系統的一些標準所定義，例如process的重要性等等。
  * 會有starvation的問題，解決的方法是aging，逐漸提高在系統中停留時間較長的process的priority。
  * SJF是按照cpu burst的長度來決定priority的Priority Scheduling
* multilevel queue:
  * 有多個queue, 每個queue有不同的Scheduling Algorithms，也有不同的Priority，通常會放不同類型的process
  * process不能在queue之間移動
  * 要定義如何選擇哪一個queue，通常有兩種做法:
    * 除非priority較高的queue空了，否則不會執行較低的queue
    * 每個queue分到不同比例的cpu時間，譬如priority較高的分到80%做RR Schdule，而較低的分到20%做FCFS等等。
  * ![multilevel_queue](https://github.com/a13140120a/Operation_System/blob/main/imgs/multilevel_queue.PNG)
* multilevel feedback queue:
  * 最通用的Scheduling Algorithm
  * process可以在queue之間移動
  * 會在run time的時候觀察process的behavior來判斷應該要如何在queue之間移動
  * 如果一個process使用cpu的時間太長，就會被排到較低priority的queue，讓IO bound和interative的process放在較高priority的queue
  * 而較低priority的process也會隨著時間增加而增加priority(aging)來避免starvation
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/multilevel_feedback_queue.PNG)
  * 上圖例子當中，最上層queue分配到的time quantum是8ms，如果8ms內未完工的process就會被安排到第二層的queue，這時候如果最上層queue裡面沒有任何process就會開始執行第二層的queue，如果還是執行不完，就會再被安排到最下層的queue，而為了避免starvation，如果在較低priority的queue待太久則會被安排到較高的queue
  * multilevel feedback queue必須決定以下幾種參數:
    * queue的數量
    * 每個queue的Algorithm
    * 何時把porcess提升到較高priority的queue
    * 降低高priority的process到較低queue的方法
    * 當process需要serve的時候該進入哪個queue

<h2 id="0053">Thread Scheduling</h2> 

* 分成user-level 和 kernel-level
* 因為當thread支援得時候,不是只有process要Scheduling, thread也要
* PCS:如果在Many-to-one 和many-to-many model，user-level就必須經過Scheduling，這個過程稱為process-contention scope (PCS)由programmar來做
* SCS:另外一個是由Kernel thread 來schedule，被稱為system-contention scope (SCS)由系統來做
* Pthread中允許process產生期間指定PCS和SCS
  * 預設情況下，對於一個擁有n個執行緒的程式，啟動多少LWP，由哪些LWP來控制哪些執行緒由作業系統來控制，這種狀態被稱為非繫結的。
  * 那麼繫結的含義就很好理解了，只要指定了某個執行緒“綁”在某個LWP上，就可以稱之為繫結的了。
  * `int pthread_attr_setscope(pthread_attr_t *attr, int scope);`的第二個參數scope是[繫結型別](https://www.796t.com/article.php?id=63046)，擁有兩個取值，PTHREAD_SCOPE_SYSTEM（繫結的）和PTHREAD_SCOPE_PROCESS（非繫結的）。
  * Linux採用了one-to-one的方案，這也就是說，Linux的執行緒永遠都是繫結，所以PTHREAD_SCOPE_PROCESS在Linux中不管用，而且會返回ENOTSUP錯誤。
  * 在many-to-many中，PTHREAD_SCOPE_PROCESS使用PCS排班法排班執行緒，將策略地排班user mode的thread在可用的LWP中，而LWP的數目是由library所控制，
  * PTHREAD_SCOPE_SYSTEM則是使用SCS排班法排班執行緒，將產生和連結一個LWP給每個user mode的thread，並使用1對1有效的map thread。
  * [wiki](https://zh.wikipedia.org/wiki/POSIX%E7%BA%BF%E7%A8%8B#%E6%95%B0%E6%8D%AE%E7%B1%BB%E5%9E%8B)
  * [EXAMPLE](https://github.com/a13140120a/Operation_System/blob/main/set_scope.c)，GCC:`gcc -o set_scope set_scope.c -lpthread`

<h2 id="0054">Multi-Processor Scheduling</h2> 

* 又分成asymmetric multiprocessing(非對稱)和symmetric multiprocessing(對稱):
  * asymmetric multiprocessing(AMP):其中一顆cpu為master，負責管理整台電腦上的所有processor，也因為所有的OS code都run在同一個cpu上，同一時間只會有一顆cpu去access system data structure，所以不會有synchronization的問題，scale能力很強，用於大型電腦當中，缺點是浪費一顆cpu不做運算，以及master可能是效能瓶頸。
  * symmetric multiprocessing(SMP): 每個processor的功能都一樣，能自行排班，OS的code run在所有的processor上，同一時間有可能會有多個cpu去access system data structure，所以會有synchronization的問題，能夠scale的能力較弱。
  * SMP又可分為兩種策略:
    * all processes in common ready queue:所有cpu的排班程式去競爭一個ready queue，會有synchronization的問題
    * separate ready queues for each processor:可能會有load balancing不平衡的問題(例如一個cpu的queue滿載，另一個則空)。
* multicore processor將多個核心(core)放在同一個實體晶片上，每個core維持它的架構，因此對作業系統來說好像是一個獨立的processor
* ![processor_vs_os](https://github.com/a13140120a/Operation_System/blob/main/imgs/processor_vs_os.jpg)
* 當processor存取memory時，需耗費大量的時間，這種情況稱為**Memory Stall**(例如cache miss)
* ![MemStall](https://github.com/a13140120a/Operation_System/blob/main/imgs/MemStall.jpg)
* 核心多執行緒化
  * 核心多緒化，多個hardware threads被分配到一個core  
  * ![TwoThrdCore](https://github.com/a13140120a/Operation_System/blob/main/imgs/TwoThrdCore.jpg)
  * Intel使用hyper-threading(或稱simultaneous multithreading,SMT)來將hardware thread分配給一個core
  * 核心多執行緒化有兩種方法:
    * Coarse-grained(粗糙):會執行一條執行緒一直到出現long-latency event(例如Memory stall)，才進行切換並清空(flush)管線，context switch代價較大。
    * fine-grained(精緻):在更小的granularity上面switch thread，例如每個指令之間，fine-grained的架構設計包含thread切換的邏輯(例如一次儲存兩個thread的cache)，因此context switch代價較小。(註:granularity是指在並行計算中，對該任務執行的工作量（或計算量）的度量)
    * [參考](http://ccckmit.wikidot.com/qa:osthread)
  * 實體的core同一時間只能執行一個hardware thread，因此需要兩種層級的排班，分別是**軟體執行緒**與**硬體執行緒**的排班
* Load balancing:
  * push migration: 某個task會會檢查不同processor上的loading，如果不平衡的話就把processes移動(適合整個系統的processor的loading都很低)。
  * pull migration: idle的processor去拉waiting task(適合整個系統的processor的loading都很高)
  * 兩種方法並不互斥，通常會一起使用這兩種方法。
* affinity:
  * process由一個processor轉移到另一個processor上需要將轉移前的processor cache清空，並將轉移後的processor cache填滿，這會造成很大的cost
  * Proccessor affinity:大部分SMP系統會嘗試讓一個process一直在同一個processor上執行，避免process由一個processor轉移到另一個processor上，也就是process對目前執行的processor有affinity(親和性)
  * soft affinity可以轉移到其他processor，hard affinity不行
  * 許多系統同時提供soft affinity和hard affinity
  * linux提供支援affinity的system call:`sched_setaffinity()`
* Load balancing會抵消affinity帶來的bonus
* heterogeneous multiprocessing(HMP):
  * 對於某些系統而言，每個核心使用相同的instruction來執行，但他們有著不同的時脈以及電源管理，這樣的系統稱為HMP。
  * 例如ARM使用[big.LITTLE](https://zh.wikipedia.org/wiki/Big.LITTLE)，高性能的big core與較節能的LITTLE core結合，big core消耗更多的能量，因此只能使用較短的時間，LITTLE消耗較少的能量，所以能夠更長時間使用。
  * CPU scheduler可以將較低耗能(例如背景程式)分配給little，可以節省電量，如果行動裝置組於省電模式，則禁止使用big，windows允許支援HMP。


<h2 id="0055">Real-time Scheduling</h2> 

* 必須在deadline之前完成
* 又分成soft real-time和hard real-time，soft real-time如果miss deadline的話不會造成嚴重的後果，hard real-time必須保證一定要在deadline之前完成(如煞車系統)。
* Event latency: event發生到被處理的延遲時間。
* 影響real-time system的主要因素為interrupt latency(中斷延遲)以及dispatch latency(調度延遲)：
  * interrupt latency包含決定interrupt類型以及context switch的時間
  * dispatch latency包含以下兩個部分:
    * Preemption of any process running in the kernel.
    * Release by low-priority processes resources required by the high-priority process.  
* Real-time Scheduling的scheduler必須要support preemptive以及prioritybased scheduling，並要求週期p必須是固定的，但只能保證soft real-time。
* 設process的執行時間是t、deadline是d、週期時間是p，如果t < d < p，週期任務的頻率就是1/p，
* T1 = (0,4,10)=(Ready, Execution, Period)
* Rate-Monotonic (RM) Scheduling:
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
* POSIX Real-Time Scheduling:
  * 設定執行緒排程演算法的介面是`pthread_attr_setschedpolicy()`，它的完整定義是：`pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);`
  * 第二個參數的選項有SCHED_RR、SCHED_FIFO和SCHED_OTHER(預設)
  * [EXAMPLE](https://github.com/a13140120a/Operation_System/blob/main/set_schedule_policy.c)，GCC:`gcc -o set_schedule_policy set_schedule_policy.c -lpthread`


<h2 id="0056">OS Example</h2> 

* Linux:
  * Linux系統的schedule是base on scheduling class，每個scheduling class會有不同的priority以及不同的algorithms以應對不同的需求。
  * 標準的Linux系統會有兩種scheduling class: default(CFS)和real-time，亦可自行加入新的scheduling class。
  * [CFS](https://blog.xuite.net/ian11832/blogg/23745751)於Linux kernel 2.6.23開始正式被採用，讓所有工作平等的被執行便是這排程的目標
  * CFS將每個thread的負載記錄下來，結合priority以及cpu使用率，較高priority但是IO bound的程式通常負載較低，使用此方法可以確保所有queue都有相近的負載
  * Linux也定義scheduling domain的階層系統，scheduling domain是根據他們共享的資源下去分組的，每個core可能擁有L1 Cache, core之間也可能共享L2 cache, 包含L2 cache的一組被分為domain0和domain1，而這兩個scheduling domain共享L3 cache，並且組成一個**NUMA node**，CFS最初只會在同一個domain裡面做thread的搬移(即domain0或domain1)，下一個層級的load balancing會發生在domain0跟domain1之間，如果整個系統處於忙碌狀態，則CFS不會在每隔Core之間進行load balancing。
* windows
  * 使用以priority為基礎的preemptive排班演算法來排班thread
  * 確保最高priority的thread將一直執行
  * windows kernel處理schedule的部分叫做dispatcher，被dispatcher選到的thread將一直執行直到被更高priority的thread preempt或是遇到IO、終止、或time quantum用完才會停止
  * 使用32層的priority技巧，variable class包含1~15，real-time包含16~31，priority為0的thread被用於記憶體管理。
  * dispatcher為每個priority提供一個queue，從最高到最低traverse，直到發現有ready的thread為止，如果沒有發現任何ready thread，會執行一個叫做idle thread的特殊thread。
  * process通常是NORMAL_PRIORITY_CLASS，除非父process是其他類別或者使用者特別設定。
  * 可使用`SetPriorityClass()`以及`SetThreadPriority()`，其API對應的priority請[參考](https://docs.microsoft.com/zh-tw/windows/win32/procthread/scheduling-priorities#base-priority)
  * 每個thread都會有一個基本優先權，預設是該class中NORMAL的數值。
  * 當thread的time quantum用完之後會被interrupt，並且如果此thread是variable class，他的priority會被降低，但不會被降到低於基本優先權，如果從waiting狀態中被釋放出來，就會提升priority，提升量取決於等待的是什麼，例如鍵盤IO將獲得大幅提升，若是磁碟IO則適量提升。
  * windows對於foreground process(前景)和background process(背景)有所區別，當一個process由background移動到foreground時，windows會增加其排班量(通常是3)
  * windows 7引入了[user-mode scheduling(UMS)](https://zhuanlan.zhihu.com/p/43652554)，允許user獨立於kernel外自行產生以及管理thread，對於大量產生 thread的process，在user mode排班會比在kernel mode排班更有效率，因為不需要介入kernel。
  * 早期版本的windows提供類似的特性，稱為[fiber](https://docs.microsoft.com/zh-tw/windows/win32/procthread/fibers)，[(fiber wiki)](https://zh.wikipedia.org/wiki/%E7%BA%96%E7%A8%8B)，允許user mode的thread(纖程)被map到單一的kernel thread，fiber不能呼叫windows API，因為所有fiber都必須分享他們執行之thread的**TEB**[(Thread Environment Block 執行緒環境區塊)](https://docs.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-teb)，如果windows api 將資訊狀態放入一個fiber的TEB，會讓此資訊被另一個fiber覆蓋，UMS藉由提供每個user thread自己的執行緒內容來克服此障礙。
  * Windows也支援multicore processor系統來進行thread在processor上的排程，使用的技術是建立[SMT Set(邏輯處理器集)](https://docs.microsoft.com/zh-tw/windows-hardware/drivers/debugger/-smt)，例如四核二緒(8 logic processor)包括四個SMT集:{0,1}{2,3}{4,5}{6,7}，Windows能夠維持同一個SMT set中logic processor上的執行緒執行，為了在不同processor之間load balancing，每個thread會分配給一個ideal processor(理想處理器)，每個process都會有一個初始種子值，用於標示屬於該process的thread的理想CPU，該process每建立一個thread，種子值的數字就會增加，藉此分散負載在不同的logic processor上，在SMT系統中，下一個ideal processor的增量會在下一個SMT set中，例如某個process的thread的ideal processor為0,2,4,6,0,2...，如果第二個process的種子值為1，則分配1,3,5,7,1,3....。


<h2 id="0057">Evaluation Methods</h2> 

* Deterministic modeling:依照performance metric去選擇、製作model，可能是要response time最小，還是wait time，或者是real time。
* Queueing model:設計演算法，分析並且證明，[little's law](https://wiki.mbalib.com/zh-tw/%E5%88%A9%E7%89%B9%E5%B0%94%E6%B3%95%E5%88%99)認為，系統中的平均存貨等於存貨單位離開系統的比率（亦即平均需求率）與存貨單位在系統中平均時間的乘積。
* Simulation:以應用程式的方式設計一個電腦系統的模型，用來驅動模型的資料產生可以是隨機分布、常態分布、指數分布、卜瓦松分布等等，並且收集LOG、workload等資料
* Implementation:即使是模擬，也沒辦法得到完全正確的評估，因此還需要Implementation。

<h1 id="006">Synchronization</h1> 

* race condition:
  * [process communication](#0033)裡頭提到的producer與consumer的例子可以修改如下:
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
  * 出現問題:
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
* critical section: 設系統含有多個process，每個process都有一段critical section，critical section可能被存取或者更新，但同一時間內只能有一個process進入。
* remain section:除了critical section以外的程式碼區段。
* 解決critical section problem必須滿足以下三個條件:
  * mutual exclusion: 不能兩個process同時在critical section內
  * progress: 如果cirtical section裡面沒有任何process，此時有一個process想要進入(或本來就在waiting)，應該要可以進入，不應無限的搶先進入。
  * bounded waiting: 一個process已經在waiting進入critical section，而在這個process進入之前應該限制其他process不能無限的搶先進入(應限制其他process進入的次數)
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
* Mutual exclusion:Yes 
* Progress:No，p0離開之後再進入會無法進入，這時p1可能也根本沒有想要進入。
* Bounded-Wait:Yes，p0離開之後一定要先讓p1進入才可以再進入。

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
* Mutual exclusion:Yes 
* Progress:Yes，當p0想進去的時候，p1若已經離開，則flag[1]會設成flase，那麼p0就可以進入
* Bounded-Wait:Yes，當p0想進去的時候flag[0]會設成true，這時如果p1想要重複一直進入便會被擋住，反之亦然

<h2 id="0063">Synchronization Hardware Support</h2> 

* 因為是hardware，所以是Atomic
* memory barriers(或稱memory fences):
  * 大多數現代電腦為了提高效能而採取亂序執行，這使得記憶體屏障成為必須。
  * Peterson’s Solution的亂序執行影響:
  * ![memory_barrier](https://github.com/a13140120a/Operation_System/blob/main/imgs/memory_barrier.PNG)
  * 最終會導致p1跟p2同時進入CS
  * 以下例子，預期的output是x=100:
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
* test_and_set():
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
  * Mutual exclusion:Yes 
  * Progress:Yes，任何一個process離開之後，其他正在waiting的process都可以進入
  * Bounded-Wait:No，假如p0先進到CS，p0結束後p1正在waiting，但p0結束馬上又搶先進到CS導致無限循環，p1 starvation(反之亦然)。
* compare_and_swap():(CAS)
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
  * Mutual exclusion:Yes 
  * Progress:Yes，任何一個process離開之後，其他正在waiting的process都可以進入
  * Bounded-Wait:No，假如p0先進到CS，p0結束後p1正在waiting，但p0結束馬上又搶先進到CS導致無限循環，p1 starvation(反之亦然)。
  * 滿足Bounded-Wait的有限等待互斥compare_and_swap():(令有n個process)
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
* 又可以有兩種實作:
  * busy waiting的方法:會浪費cycle，但不會有context switch，所以如果waiting的時間小於兩次context switch的時間的話busy waiting是較好的選擇
    * wait():
      ```c
      wait(S){
          while (S <=0); // busy waiting
          S--;
      }
      ```
    * signal():
      ```c
      signal(S){
          S++;
      }
      ```
  * 使用queue的方法:不會浪費cycle，但會有context switch(system call)，system call通常會花費較大的cycle，使用情境於將會waiting很久的process。
    * 先定義semaphore:
      ```c
      typedef struct{
          int value;
          struct process * list;
      } semaphore;
      ```
    * wait():
      ```c
      // S->list通常是PCB的list
      wait(semaphore *S){
          S->value--;
          if (S->value < 0)
            /* add this process to S->list */ ; 
            sleep();
      }
      ```
    * signal():
      ```c
      signal(semaphore *S){
          S++;
          if (S->value <= 0)
          /* remove a process(P) from S->list */ ; 
          wakeup(P);
      }
      ```
* 假設p0具有程式碼s0，p1具有程式碼s1，我們要求s1必須在s0完成之後才可以執行，可以使用一個共同的semaphpre synch:
  ```c
  /* p0 */                  /* p1 */
  s0;                       wait(synch);
  signal(synch);            s1;
  ```
* 使用semaphore處理CS problem:
   ```c
   do {
     wait(mutex);
   
        /* critical section */  
     
     signal(synch);  
   
       /*remainder section */ 
   }while (1) 
   ```
  * Mutual exclusion:Yes 
  * Progress:Yes，任何一個process離開之後，其他正在waiting的process都可以進入
  * Bounded-Wait:Yes/No，假如p0先進到CS，p0結束後p1正在waiting，但p0結束馬上又搶先進到CS導致無限循環，p1 starvation(反之亦然)，但實作上其實進入critical section會先進入到一個queue，所以p0結束馬上又搶先到CS的話其實還是要等原本就在waiting的process執行完之後才能進去。
      
<h2 id="0066">Monitor</h2> 

* 因為semaphoer的使用非常的複雜，若是有出錯，有可能造成程式錯誤或者deadlock，為了可以讓programmer可以更方便的處理`wait()`及`signal()`的問題而誕生
* 是一種ADT(abstract data type, 抽象資料型態)
* 類似 OO(物件導向) Language的概念
* 屬於hight level的synchronization處理方式
* Monitor內部的local variable只能由內部的method存取，並確保同一時間只能有一個process在Monitor的內部活動(active)
* Monitor提供特殊的資料型態: condition variable來處理同步問題。
  * condition variable代表的就是某些event發生時他可以trigger其他process(或thread)，或者使一個process(或thread)wait一個event發生
  * condition variable可以透過`wait()`及`signal()`來操作，與semaphoer不同的是，若Monitor內部沒有任何process等待存取該condition variable，則`signal()`會沒有作用，與之相比semaphoer的`signal()`必定會改變semaphoer的狀態。
  * Pthread的condition variable有`broadcast()`方法，可以wakeup所有正在waiting的process(或thread)。
  * condition variable的`wait()`及`signal()`就像
  * 一旦Monitor內部的condition variable call了`wait()`等於是把process加到waiting queue裡面，並且變成inactive，外面的process就可以進入，或者內部本來在wait的process會active
  * `signal()`會dequeue並且wakeup 一個正在waiting的process
*  Hoare Monitor implementation using semaphore:
  *  for the process accessing the monitor:
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
    
  * for the wait(condition) and signal(condition) implementation using semaphore[參考解說](http://learningnote321235.blogspot.com/2018/01/10214-hoare-monitor.html)和[參考2](https://wangwilly.github.io/willywangkaa/2018/08/04/Operating-System-Process-Synchronization-2/):
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
      
  * condition-wait(條件等待) Monitor with one resource:
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

* prioirty inersion: 
  * 假設有三個process，L, M, H，其prioirty L<M<H，假設H 想存取正在被L存取的Semaphore S，這時H必須等待L使用完S，然而此時M突然變得可執行，因此L被context switch到M，但S仍未被釋放，間接導致低優先權M，影響到較高優先權H，必須等待L。
  * 解決的方法是Priority-inheritance protocol，根據這個protocal，所有要存取資源的process必須繼承最高priority的process的priority，上述例子中，L將繼承H的priority，因此就可以阻止M把L preempt，當L使用完S之後priority會恢復到原本的樣子，並context switch到M。


<h2 id="0067">Classical Problem</h2> 

* bounded-buffer problem:
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


* Readers-Writers Problem:
  * first RW problem:
    * reader不需要等待其他的reader結束(reader可以pass lock給下一個reader)，而writer要等待reader結束，有可能會造成writer starvation。
    * init:
      ```c
      semaphore rw_mutex = 1;  // reader跟writer共用
      semaphore mutex = 1;     // 用來保護read_count作加減
      int read_count = 0;      // 目前有多少個reader
      ```
    * reader:
      ```c
      while(true){
          wait(rw_mutex);  // wait if some one read or write
          
          /* **writing** */
          
          signal(rw_mutex);
      }
      
      ```
    * writer:
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


* Dining Philosophers Example:
  * 每個人要吃飯時必須拿起左右兩支筷子
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/Dining_Philosophers.png)
  * 以下方法可以免於deadlock，但不能免於starvation:
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

* Windows synchronization:
  * windows kernel在單一處理器系統下存取share resource的時候會把所有存取此resource的interrupt都mask掉
  * 而多處理器系統下，windowd使用spinlock(自旋鎖)來保護較短的程式碼，且擁有spinlock的thread絕對不會被preempt。
  * 對於kernel space以外的thread synchronization，windows提供dispatcher object(分派器物件)，使用dispatcher object時，thread可以根據包括mutex lock、semaphore、event、counter來做同步。
    * windows 藉由thread獲得mutex lock來存取resource，並在結束時釋放。
    * event和condition variable類似，他可以在某個條件發生時等待一個thread。
    * counter用來通知一個thread的time quantum已經到了。
    * dispatcher object擁有兩個狀態:分別是**signaled state(信號狀態)**和**nonsignaled state(非信號狀態)**
    * signaled state表示thread可以取得此物件(釋放鎖)，反之亦然。
    * dispatcher object與thread之間的關係:當一個thread被一個nonsignaled state的dispatcher object阻擋時，thread的狀態會從ready變成waiting，並放入此object的waiting queue，當dispatcher object變成signaled state的時候，kernel會去檢查是否有任何thread在waiting queue，如果有，kernel就從裡面挑一個(或多個)把他的狀態從waiting設成ready，thread便可以恢復執行。
    * 對於mutex lock dispatcher object而言，如果thread試圖取得一個nonsignaled state的mutex lock dispatcher object時，該thread會被suspend，並放入該object的waiting queue中，當該object轉換到signaled state時，kernel只能選擇釋放一個於waiting queue中的thread，並獲得此mutex lock。
  * critical section object是使用者模式的mutex lock，並且在沒有kernel的干預下釋放。
    * critical section object在等待thread釋放物件時，會先使用spinlock，如果自旋的時間太長，則該thread將配置一個kernel的mutex lock並交出他的CPU
    * critical section非常有效率，因為kernel的mutex lock只有在發生race condition的時候才會被配置。

* Linux synchronization:
  * Linux提供一種簡單數學運算的不可分割指令:(單元整數)`atomic_t`，所有使用該類型的運算都不可中斷:
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
  * Linux 亦提供`mutex_lock()`來保護kernel內的critical section:
    * task在進入critical section之前必須呼叫函數`mutex_lock()`，並在離開之後呼叫`mutex_unlock()`
    * 如果目前lock無法取得，則呼叫`mutex_lock()`的task會被設為sleep，並在可以取得時被wakeup。
  * Linux kernel也提供spinlock和semaphore。
  * Linux中，如果該task已經獲得一個spinlock或mutex lock的話，除非釋放，否則不能再獲得一個相同的鎖。
  * Linux提供`preempt_disable()`和`preempt_enable()`來禁用和啟用preempt。
  * 如果kernel的task正擁有一個lock的時候，kernel是不可以preempt的，為了實現此機制，系統中的每個task都有一個thread-info的struct，包含一個preempt_count的計數器，當task獲得鎖的時候preempt_count+1，反之亦然，當preempt_count的值為0時，才可以被preempt或者被中斷。

* POSIX synchronization:
  * POSIX mutex lock:
    * `int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);`建立互斥鎖，第二個參數attr可以是以下值:  
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
  * POSIX semaphore:
    * 分成named跟unnamed。
    * create a named semaphore and lock(在之前的[example](https://github.com/a13140120a/Operation_System/blob/main/posix_producer.c)就有使用過的`sem_open()`函數):
      ```c
      include <semaphre.h>
      sem_t* sem;
      /* create a named semaphore and initialize to 1 */
      sem = sem_open("SEM",O_CREAT, 0666, 1);
      ```
    * create a named semaphore and lock:
      ```c
      include <semaphre.h>
      sem_t* sem;
      
      sem_init(&sem,0,1)  //第二個參數設定為 0 表示僅供目前的 process 及其 thread 使用。非 0 表示此 semaphore 與其他process共用，第三個參數表示initialize to 1
      ```
   * lock and unlock(named跟unnamed一樣):
     ```c
      sem_wait(sem); //aquire
      /* critical section */
      sem_post(sem); //release
     ```
  * POSIX condition variable:
    * 初始化:
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
* Java  synchronization:
  * Java monitor



























