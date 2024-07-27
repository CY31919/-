# 手写数据库连接池

## 1.项目背景

数据库本质上是对磁盘操作，如果数据库访问过多、磁盘IO过多，会遇到访问瓶颈。解决瓶颈可以在数据库和服务器应用的中间加一层缓存数据库，将热点数据存储到内存中。还可以增加连接池，用于减少高并发情况下“三次握手、`MySQLSever`连接认证，`MySQLSever`关闭回收资源和`TCP`四次挥手”所耗费的性能。

## 2.相关技术

`MySQL`数据库编程、单例模式、`queue`队列容器、`C++11`多线程编程、线程互斥、线程同步通信和`unique_lock`、基于`CAS`的原子整形、智能指针`shared_ptr`、`lambda`表达式、生产者消费者模型。

## 3.功能介绍

1.线程安全的单例模式：连接池只需要一个实例，同时要保证空闲连接全部维护在一个线程安全的Connection队列中，使用线程互斥锁保证队列的安全性。

2.从`ConnectionPool`中可以获得和`MySQL`的连接`Connection`。

3.创建新的连接时是动态创建的。由生产者线程，当线程池为空且没有达到数量上限时，生产连接，消费者线程从连接池中获取连接。

4.由定时线程，检测是否有大于`initSize`的线程空闲`maxIdleTime`，需要被回收。

5.用户使用智能指针`shared_ptr`来管理，用lambda表达式定制连接归还到连接池功能。

6.使用`mutex`互斥锁实现超时连接

## 4.数据库环境配置

1.项目属性-C、C++-常规-附加包含目录-填写`mysql.h`头文件的路径

2.链接器-常规-附加库目录-填写`libmysql.lib`的路径

3.链接器-输入-附加依赖项-填写`libmysql.li b`的名称（在之前的附加项后输入;）

4.将`libmysql.dll`动态链接库复制一份到工程目录下

5.以上所有文件按都在`MySQL`的安装目录下

## 5.常见的预定义宏

1. `__FILE__`
   - 包含当前源文件的名称，通常是带路径的文件名字符串。
   - 示例输出：`"source.cpp"`
2. `__LINE__`
   - 包含当前行号，是一个整数字面量。
   - 示例输出：`42`
3. `__DATE__`
   - 包含编译日期的字符串，格式为 `"Mmm dd yyyy"`。
   - 示例输出：`"Jul 25 2024"`
4. `__TIME__`
   - 包含编译时间的字符串，格式为 `"hh:mm:ss"`。
   - 示例输出：`"10:27:45"`
5. `__TIMESTAMP__`
   - 包含最后一次修改文件的时间戳，格式为 `"Ddd Mmm dd hh:mm:ss yyyy"`。
   - 示例输出：`"Fri Jul 25 10:27:45 2024"`
6. `__FUNCTION__`
   - 包含当前函数的名称，是`C99`和C++标准的一部分。
   - 示例输出：`"main"`
7. `__LINE__`
   - 包含当前行号，是一个整数字面量。
   - 示例输出：`42`

## 6.数据库相关操作函数

### `mysql_real_connect`

 函数用于在 C/C++ 程序中连接到 `MySQL`数据库。它的参数用于指定连接所需的各种信息。这个函数在连接成功时返回一个 `MYSQL` 指针，在连接失败时返回 `nullptr`。以下是每个参数的解释：

```c++
MYSQL* mysql_real_connect(MYSQL *mysql, const char *host,
                          const char *user, const char *passwd,
                          const char *db, unsigned int port,
                          const char *unix_socket, unsigned long client_flag);
```

### 参数解释

1. **`mysql`**

   - 类型：`MYSQL *`
   - 说明：指向已初始化的 `MYSQL` 结构体的指针。这个指针通常是通过调用 `mysql_init()` 函数获得的。

2. **`host`**

   - 类型：`const char *`
   - 说明：`MySQL` 服务器的主机名或 IP 地址。对于本地服务器，可以使用 `"localhost"` 或 `"127.0.0.1"`。

3. **`user`**

   - 类型：`const char *`
   - 说明：用于连接 `MySQL` 数据库的用户名。

4. **`passwd`**

   - 类型：`const char *`
   - 说明：用于连接 `MySQL` 数据库的密码。

5. **`db`**

   - 类型：`const char *`
   - 说明：要连接的数据库的名称。可以为 `NULL`，这样可以稍后选择数据库。

6. **`port`**

   - 类型：`unsigned int`
   - 说明：`MySQL` 服务器监听的端口号。通常是 `3306`。如果使用默认端口，可以传递 `0`。

7. **`unix_socket`**

   - 类型：`const char *`
   - 说明：用于连接到 `MySQL` 服务器的 Unix 套接字文件路径。在连接到本地 `MySQL` 服务器时使用。如果不需要，可以传递 `NULL`。

8. **`client_flag`**

   - 类型：`unsigned long`

   - 说明：用于指定连接的其他选项。通常传递 0

     。可以使用的标志包括：

     - `CLIENT_FOUND_ROWS`：返回被找到（匹配）的行数，而不是修改的行数。
     - `CLIENT_MULTI_STATEMENTS`：允许执行多语句查询。

### `mysql_query` 

函数用于在 C/C++ 程序中向 `MySQL` 数据库发送 `SQL` 查询。它接受一个已连接的 `MYSQL` 对象指针和一个包含 `SQL` 查询字符串的指针。

```
int mysql_query(MYSQL *mysql, const char *query);
```

- 如果查询成功，返回 `0`。
- 如果查询失败，返回非零值。可以使用 `mysql_error` 函数获取错误消息。

## 87.单例模式的实现

1.构造函数私有化

2.提供访问接口，获取连接池对象实例

3.线程安全体现在：对于static对象的初始化，会由编译器自动lock和unlock

## 8.保证“连接数量”的线程安全

`atomic_in`  `_connectionCnt`

用来记录所创建的Connection连接的总数量，不能超过maxSize；要用atomic_int，因为涉及到++操作，而int的++操作不是线程安全的。而atomic_int是线程安全的，在多个线程进行++操作是安全的，`atomic<int>` 是 C++ 标准库中的一个模板类，用于提供对整数类型的原子操作支持，以保证在多线程环境下的线程安全性。原子操作是指那些不能被中断的操作，确保多个线程在操作同一数据时不会出现数据竞争（race condition）。

#### 9.1`atomic<int>` 的线程安全特性

1. **原子性**：`atomic<int>` 确保对整数的所有读写操作都是原子性的，这意味着这些操作在执行时不会被其他线程中断。原子操作不会出现部分完成的状态。
2. **无锁机制**：`atomic<int>` 的实现通常是无锁的，依赖于硬件提供的原子操作指令（如 `cmpxchg`、`xchg` 等），从而避免了传统锁机制（如 `mutex`）带来的性能开销和潜在的死锁问题。
3. **内存模型**：`atomic<int>` 支持不同的内存排序（memory ordering），包括：
   - `memory_order_relaxed`：不做任何同步操作，仅保证操作的原子性。
   - `memory_order_acquire`：保证当前操作及其之后的操作不会被重排序到当前操作之前。
   - `memory_order_release`：保证当前操作及其之前的操作不会被重排序到当前操作之后。
   - `memory_order_acq_rel`：同时具有 `acquire` 和 `release` 的效果。
   - `memory_order_seq_cst`：提供最强的内存序保证，确保所有线程都按照全局顺序观察到原子操作。

#### 线程不安全的后果

如果在多线程环境中使用普通的 `int` 而不是 `atomic<int>`，可能会出现数据竞争，导致以下问题：

1. **丢失更新**：多个线程可能同时读取相同的值并进行修改，最后的结果会丢失其中一些更新。例如，两个线程同时读取 `counter` 的值为 5，然后分别增加 1，如果没有原子性保证，最终 `counter` 的值可能还是 6，而不是期望的 7。
2. **不一致状态**：由于缺乏同步机制，不同线程可能会看到变量的不同值，导致程序行为不可预测。
3. **崩溃或死锁**：数据竞争会导致未定义行为，可能会引发程序崩溃或死锁，严重影响程序的稳定性和可靠性。

## 9.`C++` 类的成员函数作为线程函数的解决方法

在 C++ 中使用 `std::thread` 时，线程函数可以是普通函数、函数对象（functor）、或 lambda 表达式。当需要在 C++ 类中使用成员函数作为线程函数，由于成员函数包含隐含的 `this` 指针参数，因此不能直接使用成员函数。可以使用 `std::bind` 或 lambda 表达式来解决这个问题。本项目采用绑定器解决。

### 可连接线程 vs. 分离线程

1. **可连接线程（`joinable thread`）**：
   - 这是默认创建的线程类型。
   - 主线程可以调用 `std::thread::join` 方法来等待这个线程完成。
   - 主线程会阻塞直到这个线程运行结束。
2. **分离线程（detached thread）**：
   - 调用 `std::thread::detach` 将线程设置为分离状态。
   - 分离线程与主线程独立运行，主线程不会等待它完成。主线程结束后，分离线程也会自动结束。
   - 一旦设置为分离状态，线程的资源会在线程运行结束后自动释放。

```C++
//启动一个新的线程作为连接生产者
//this指向调用该成员函数的对象。
thread produce(std::bind(&ConnectionPool::produceConnectionTask,this));
produce.detach();
//启动一个新线程检测，是否有额外生产的空闲连接，生存时间超过maxIdleTime，释放掉
thread scanner(std::bind(&ConnectionPool::sacnnerConnectionTask, this));
scanner.detach();
```



## 10.如何实现在独立的线程中生产新连接（生产者线程实现、保证队列访问的线程安全）



```C++
void ConnectionPool::produceConnectionTask() {
	for (;;) {
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty()) {
			cv.wait(lock);//队列里还有空闲连接，阻塞等待
		}
		//cv.wait(lock, []{ return _connectionQue.empty(); });
	}
	//没有空闲连接了，且连接数量没有达到上限,创建新链接
	if (_connectionCnt < _maxSize) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		_connectionQue.push(p);
		_connectionCnt++;
	}
	//通知消费者线程，可以消费连接了
	cv.notify_all();
}
```

`std::condition_variable` 是条件变量，用于在某个条件发生时通知一个或多个线程。它通常与 `std::mutex` 一起使用，确保线程安全地访问共享数据。

`unique_lock<mutex> lock(_queueMutex);`通过互斥锁，可以保证同时只有一个线程可以访问缓冲区，避免数据冲突。

### 两种使用方式的比较

#### `cv.wait(lock);`

```C++
while (！_connectionQue.empty()) {
    cv.wait(lock);
}
```

这个代码片段的意图是等待直到 `_connectionQue` 非空。它的工作方式如下：

1. **等待**：线程调用 `cv.wait(lock)`，这会释放 `lock` 并挂起线程，直到条件变量被通知。
2. **检查条件**：当线程被唤醒时，它会重新获得 `lock` 并检查循环条件 `_connectionQue.empty()` 是否满足。
3. **继续等待**：如果 `_connectionQue` 仍然非空，线程会再次调用 `cv.wait(lock)` 并进入等待状态。

**问题**：

- 这种方式有一个潜在的问题，称为**虚假唤醒**。即使条件变量被通知，线程也可能被唤醒而条件并未真正满足。这就要求我们使用一个循环来检查条件。
- 每次调用 `cv.wait(lock)` 都会释放 `lock` 并挂起线程，直到线程被通知并重新获得 `lock`。

#### `cv.wait(lock, []{ return _connectionQue.empty(); })`

```C++
cv.wait(lock, []{ return _connectionQue.empty(); });
```

这个代码片段是一个更简洁的方法，它结合了等待和条件检查：

1. **等待并检查条件**：线程调用 `cv.wait(lock, condition)`，其中 `condition` 是一个可调用对象（这里是 lambda 表达式），它用于检查条件是否满足。并在条件变为 `true` 时继续执行。这样，当 `_connectionQue.empty()` 为 `true` 时，线程会继续执行，而在 `_connectionQue` 非空时线程会被挂起。

2. **通知**：线程在被通知后，会重新获得 `lock` 并执行 `condition`。如果 `condition` 返回 `true`，线程继续执行；如果 `condition` 返回 `false`，线程将继续等待。

## 11.如何实现，连接使用结束后自动返回连接池（智能指针、消费者的实现、重写删除器）

使用智能指针，智能指针出作用域会自动析构，进而释放连接，可以重定义删除器，使之不释放而是返回队列

智能指针是 C++ 标准库中的一种模板类，用于自动管理动态内存的生命周期，避免手动管理内存可能带来的内存泄漏和悬空指针问题。智能指针主要包括 `std::unique_ptr`、`std::shared_ptr` 和 `std::weak_ptr`。

### 智能指针的类型

1. **`std::unique_ptr`**：
   - 具有唯一所有权，即某一时刻只能有一个 `std::unique_ptr` 指向特定对象。
   - 不允许拷贝，但可以转移所有权（使用 `std::move`）。
   - 适用于明确所有权的场景。
2. **`std::shared_ptr`**：
   - 具有共享所有权，即多个 `std::shared_ptr` 可以共享指向同一对象的所有权。
   - 使用引用计数来管理对象的生命周期，当最后一个 `std::shared_ptr` 被销毁时，所管理的对象会被释放。
   - 适用于需要共享资源的场景。
3. **`std::weak_ptr`**：
   - 不控制对象的生命周期，只是对 `std::shared_ptr` 所管理对象的弱引用。
   - 防止循环引用导致的内存泄漏。
   - 可以通过 `std::weak_ptr` 安全地获取 `std::shared_ptr`，确保对象存在。

```C++
shared_ptr<Connection> ConnectionPool::getConnection() {
	unique_lock<mutex> lock(_queueMutex);
	while(_connectionQue.empty()) {
		//尝试获取互斥锁并等待条件变量 cv 被通知或超时（_connectionTimeOu毫秒）
		if (_connectionQue.empty()&&cv_status::timeout== cv.wait_for(lock, chrono::milliseconds(_connectionTimeOut))) {
			LOG("获取连接超时.......获取连接失败");
			return nullptr;
		}
	}
	//创建智能指针sp,指向队头元素
	//用lambda表达式重写智能指针的删除器，使之出作用域后connection被回收而不是析构
	shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection*ptr_conn) {
		//保证访问_connectionQue的线程安全，必须先拿到锁才能访问否则等待
		unique_lock<mutex> lock(_queueMutex);
		//lock_guard<mutex> lock(_queueMutex);
		_connectionQue.push(ptr_conn);
		});
	//队头出队
	_connectionQue.pop();
	//通知生产者，检查是否需要生产连接
	cv.notify_all();

	return sp;
}
```

## 12.释放超时连接

```C++
void ConnectionPool::sacnnerConnectionTask() {
	for (;;) {
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize) {
			Connection* ptr = _connectionQue.front();
			if (ptr->getAliveeTime() >=(_maxIdleTime * 1000)) {
				_connectionQue.pop();
				delete ptr;//释放
				_connectionCnt--;
			}
		}
	}
}
```



# 压力测试

测试单线程情况下，使用连接池和不使用连接池，插入同样数量数据所消耗的时间差。

测试四单线程情况下，使用连接池和不使用连接池，插入同样数量数据所消耗的时间差。
