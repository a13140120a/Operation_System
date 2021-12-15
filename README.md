# Operation_System

* ## [Overview](#001) #

* ## [Process Management](#002) 
* ## [Process Coordination](#002) #
* ## [Memory Management](#003) #
* ## [Storage Management](#004) #
* ## [Protection and Security](#005) #
* ## [Distributed Systems](#006) #
* ## [Special Purpose Systems](#007) #
* ## [Case Studies](#008) # 



<h1 id="001">Overview</h1> 

  * ## [Definition](#0011) #
  
<h2 id="0011">Definition</h2> 

  * ### **Resource allocator**: 能有效的分配及管理資源，並且滿足公平性，即時性等要求。 
  * ### **Control program**: 控制user program 的執行以及控制hardware，避免電腦因不當操作產生error 或崩潰。
  * ### **Kernel**: 一個always running 的核心程式。
<h2 id="0011">History</h2>   

  * 演進: Batch -> Multi-programming -> Time-sharing 
    * batch: 一次只能一個user 執行一個process
    * Multi-programming: 一次可以執行多個process，不會因為IO 而idle 在那邊，但是沒有跟user 的interactive
    * Time-sharing: 可以來回在很多個process 之間切換，讓人產生好像一次執行多個process，可以的interactive，使用的是interrupt 的技術
    [(詳細資訊)](https://jeffery12366yahoo-com.medium.com/os-chapter-0-base-inf-38636044ccdf)




























