# Operation_System

* ## [Overview](#001) #
* ## [OS Structure](#002) #

* ## [Process Management](#002) 
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

* Windows and UNIX API example:
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






<h1 id="002">Process Management</h1> 

* Interrupt: 
* nonpreemptive: process 可自願放棄cpu


<h1 id="003">Process Coordination</h1> 
























