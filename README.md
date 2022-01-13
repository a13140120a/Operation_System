# Operation_System

* ## [Overview](#001) #

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



<h1 id="002">Process Management</h1> 

* Interrupt: 
* nonpreemptive: process 可自願放棄cpu


<h1 id="003">Process Coordination</h1> 
























