# Operation_System

* ## [Overview](#001) #
* ## [OS Structure](#002) #
* ## [Process Concept](#003)
 
* ## [Process Coordination](#003) #
* ## [Memory Management](#004) #
* ## [Storage Management](#005) #
* ## [Protection and Security](#006) #
* ## [Distributed Systems](#007) #
* ## [Special Purpose Systems](#008) #
* ## [Case Studies](#009) # 


<h1 id="001">Overview</h1> 

  * ## [Definition](#0011) #
  
<h2 id="0011">Definition</h2> 

  *  **Resource allocator**: 能有效的分配及管理資源，並且滿足公平性，即時性等要求。 
  *  **Control program**: 控制user program 的執行以及控制hardware，避免電腦因不當操作產生error 或崩潰。
  *  **Kernel**: 一個always running 的核心程式。
  *  作業系統是屬於event-driven(事件驅動)，也就是他依據命令、應用程式、IO devices以及interrupt來工作。 
<h2 id="0011">History</h2>   

  * 演進: Batch -> Multi-programming -> Time-sharing 
    * batch: 一次只能一個user 執行一個process
    * Multi-programming: 一次可以執行多個process，不會因為IO 而idle 在那邊，使用的是interrupt 的技術，但是沒有跟user 的interactive
    * Time-sharing: 可以來回在很多個process 之間切換，讓人產生好像一次執行多個process的錯覺，可以interactive。  
    * [更多詳細資訊](https://medium.com/@a131401203/%E4%B8%AD%E6%96%87%E7%B3%BB%E4%B9%8B%E4%BD%9C%E6%A5%AD%E7%B3%BB%E7%B5%B1%E6%BC%94%E9%80%B2%E5%8F%B2-610986e9ee3c)

<h2 id="0011">Interrupt</h2>   

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


<h2 id="0012">Timer</h2>   

* 通常在mother board上
* 每隔一段時間會導致interrupt
* 避免cpu陷入無窮迴圈或卡死
* 用來implement Time-sharing
* 透過load-timer這個privileged instruction可以修改間隔時間

<h2 id="001">Duel-Mode Operation</h2> 

* 為了防止user program摧毀硬體，OS會分成兩種mode: 分別是user mode跟kernel mode
* 有Hardware support的硬體會有一個bit, 當進入到kernel mode的時候OS會先把這個bit設為0，回到user mode之後會再設回1。
* 只要發生interrupt就代表進入kernel mode
* Privileged instruction只能在kernel mode 時候執行，在cpu設計的時候privieleged instruction就已經定義好了
* 一但在user mode執行priviledged instruction cpu會通知OS使OS產生錯誤訊息並阻擋執行。
* Privileged instruction只能藉由呼叫System Call之後OS觸發interrupt然後轉換到kernel mode執行。

<h2 id="0012">Component 名詞定義</h2>   

* CPU: 執行instruction的硬體,通常內含暫存器以及L1 cache
* Processor: 一個實體晶片，包含一個或多個CPU
* Core: CPU的基本計算單位
* Muti-Core: 同一個CPU上包含多個Core


<h1 id="002">OS structure</h1> 

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

<h2 id="0022">API</h2>   

* API(application porgramming interface): OS提供的程式設計介面，API會去呼叫System Call, 一個API可對應到0或多個System Call
  * Linux 的Api可以藉由man api_function查看:例如man read 可以查看read()這個api的資訊
  * UNIX系列的API叫做POSIX
  * Windows 就叫windows api
  * JAVA的API: Java Api(與jvm的interface)

* api與system call比較圖:
  ![api與system call比較圖](https://github.com/a13140120a/Operation_System/blob/main/imgs/sys_api.PNG)

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

* 傳遞參數至作業系統有三種方式:
  * 使用register
  * 使用存放在memory中的block或table
  * 使用stack

<h2 id="0023">loader and linker</h2>   

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


<h2 id="0023">OS設計與製作</h2>   

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

<h2 id="0023">Structure</h2>   

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

<h2 id="0024">系統啟動</h2>  

* [啟動程式](https://zh.wikipedia.org/wiki/%E5%95%9F%E5%8B%95%E7%A8%8B%E5%BC%8F):
  * BIOS的作用是初始化和測試硬體元件，以及從大容量儲存裝置（如硬碟）載入啟動程式，並由啟動程式載入作業系統
  * 許多BIOS程式都只能在特定電腦型號或特定主機板型號上執行。早年，BIOS儲存於ROM晶片上；現在的BIOS多儲存於快閃記憶體晶片上，這方便了BIOS的更新。
  * 最近許多電腦系統已經使用[UEFI](https://zh.wikipedia.org/wiki/%E7%B5%B1%E4%B8%80%E5%8F%AF%E5%BB%B6%E4%BC%B8%E9%9F%8C%E9%AB%94%E4%BB%8B%E9%9D%A2)取代[BIOS](https://zh.wikipedia.org/wiki/BIOS)
  * GRUB是一個用於UNIX系統的開源啟動程式，非常靈活，可在啟動時修改kernel參數，甚至可以選擇啟動不同的kernel，例如Linux的/proc/cmdline中的BOOT_INAGE是要載入到記憶體的核心映象檔的名稱，而root指定根檔案系統的唯一標識符
  * Android不使用GRUB，最常見的啟動程式是[LK](https://baike.baidu.hk/item/lk/623671)  [(LK詳細)](https://www.itread01.com/content/1502160009.html)

<h2 id="0024">除錯</h2>  

* core dump: 當程序運行的過程中異常終止或崩潰，作業系統會將程序當時的內存狀態記錄下來，保存在一個文件中，這種行為就叫做Core Dump
* crush dump: 當系統crash 的時候就要 collecting data for crash dump
* /proc是存在kernel中的偽檔案系統，/proc/2155將包含ID為2155的process 的訊息
* 可用`ps`查看process狀態，`top`查看process即時訊息，`vmstat`查看系統記憶體使用情況，`netstat`查看網路，`iostat`查看磁碟IO狀況
* `strace`可以查看任何process調用的system call,例如`strace ls`可查看ls調用的那些system call以及狀況，[gdb](https://jasonblog.github.io/note/gdb/gdbshi_yong_jiao_xue_ff1a_zi_dong_hua_ni_de_debug.html)指令可以原始碼除錯，`perf`為Linux性能工具包，`tcpdump`網路封包擷取。
* BCC: BCC是[eBPF](https://hackmd.io/@sysprog/linux-ebpf)工具的前端介面，而eBPF是擴展的[BPF](https://zh.wikipedia.org/wiki/BPF)，eBPF可動態插入正在運行的Linux系統，其指令可讀取特定事件(例如正在呼叫某個system call)，或者監視系統效能(例如IO時間)，BCC提供了python的前端介面，並嵌入了eBPF工具連接的C程式碼，而eBPF又與核心連接，BCC提供的許多工具均可用於特定應用程式，例如MySQL, Java或Python 程式

<h1 id="003">Process Concept</h1> 


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


<h2 id="0031">Process Creation</h2> 

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

<h2 id="0031">Process Communication</h2> 

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
* Pipe:
  * 早期的UNIX系統所使用的IPC機制
  * implement pipe時，有四個議題必須考慮:
    * 允許單向通信或雙向通訊?
    * 若允許雙向通訊，則是半雙工(同時間只能單方向)，還是全雙工(同時間可以雙向)
    * 通信的兩個process是否必須存在關係(parent-child)
    * 是否可以通過網路，或者必須在同一台機器上?
  * Ordinary pipe(普通管道):
    * UNIX 的 Ordinary pipe create方法就是`pipe(int fd[])`
    * producer從write end(寫入端)寫入，consumer從read end(讀取端)讀取
    * fd[0]表示write endㄝ，fd[1]表示read end
    * UNIX 的Ordinary pipe不能從產生這個pipe外的process存取
    * 基本上，UNIX的parent透過ordinary pipe和`fork()`出來的child溝通
    * UNIX把pipe當成一個特殊的檔案型態，可以使用普通的`read()`或`write()`來操作。
  * Windows的Ordinary pipe叫做anonymous pipe:
    * 如同UNIX是parent-child的關係
    * 可以使用普通的`readFile()`或`writeFile()`來操作。
    * create的API是`CreatePipe()`


* POSIX IPC:
  * POSIX shared memory的範例:[producer](https://github.com/a13140120a/Operation_System/blob/main/posix_producer.c)，[consumer](https://github.com/a13140120a/Operation_System/blob/main/posix_comsumer.c)
  * [UNIX Ordinary pipe example](https://github.com/a13140120a/Operation_System/blob/main/unix_pipe.c)
* Windows:
  * [ALPC](https://zh.wikipedia.org/wiki/%E6%9C%AC%E5%9C%B0%E8%BF%87%E7%A8%8B%E8%B0%83%E7%94%A8#%E5%AE%9E%E7%8E%B0)
  * ![](https://github.com/a13140120a/Operation_System/blob/main/imgs/3_19_ALPC_Windows.jpg)
  * windows anonymous pipe example: [parent](https://github.com/a13140120a/Operation_System/blob/main/win_pipe_parent.cpp), [child](https://github.com/a13140120a/Operation_System/blob/main/win_pipe_child.cpp)


* nonpreemptive: process 可自願放棄cpu


<h1 id="003">Process Coordination</h1> 
























