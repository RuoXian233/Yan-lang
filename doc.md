# Yan 语言文档
1. 语言简介
    - 类型：动态，强类型，解释型脚本语言
    - 解释器语言：C++
    - 运行流程：Source -> Tokens -> AST 交由 Interpreter 直接运行，没有字节码 + 虚拟机等中间步骤
    - 支持简单的面向对象（不支持继承，多态，但可通过高阶函数等特性间接实现）
    - 有简单的模块系统，支持 C++ 与 yan 两种语言的模块
    - Parser 有一些不完善，函数柯里化，链式调用等需要通过中间变量实现

2. 环境配置
    - 从 Release 下载安装包
    - 释放到一个目录中，将可执行文件所在目录添加至环境变量
    - 自带编辑器，调试器正在开发中，测试编辑器在安装目录 yan-editor 文件夹内，main.py为入口
    

3. 编写第一个 Yan 程序 **(注: 以下 Markdown 语法着色均为 javascript)**
    ```javascript
    // hello.yan
    println('Hello, world!')
    ```
    - println() 是内置的函数，可以用来在控制台输出，函数只有一个参数，可以为任意类型，结尾自动添加换行符
    - Yan 使用单引号 `'` 表示字符串，双引号 `"` 不是一个合法的字符串标记
    - Yan 不需要定义 `main()` 函数，也不强制尾随分号

    - 打开终端，输入 `yan hello.yan` 即可看到运行结果
    ----
    
4. Yan 变量
    
    *4.1 基本语法*

    - 变量使用关键字 `var` 定义，与大多数语言不同，只有引用变量可以不带 `var` 关键字，所有赋值操作必须由 `var` 显式说明
    - 变量的命名符合标识符的规范（但不支持中文变量或unicode字符作为标识符）
    - 变量间互相赋值的行为，由其`可变性`决定

    ```javascript
        // variables.yan
        var a = 3  // 变量声明与赋值
        println(a) // 变量作为函数参数, println() 的参数可以是任意类型
        
        var a = 4 // 变量赋值 (与声明是同一个写法)
        println(a)
    ```

    *4.2 基本类型*
    
    - 数字：`Number`，包括整数以及浮点数，整数范围为 `[-2^31 + 1, 2^31 - 1]`
    - 字符串：`String`，没有单独的字符型
    - 列表: `List`，任意长度，任意类型作为元素
    - 字典（对象）类型：`Dictionary | ClassObject`, 键值对，目前键只能为字符串或数值，对象的内部表示也是字典
    - 函数与内置函数类型: `Function | BuiltinFunction`，支持高阶函数
    - 方法（绑定函数）与内置方法: `Method | BuiltinMethod`
    - 可以用 `typeof()` 函数获取表达式的类型

    ```javascript
        // types.yan
        var a = 1
        var b = 'hello, yan!'
        var c = [1, 2, 3, 4, 5]
        var d = {}
        var e = -23.324
        var f = function() -> false

        println(typeof(a)) // 输出 <type 'Number'>
        println(typeof(b)) // 输出 <type 'String'>
        println(typeof(c)) // 输出 <type 'List'>
        println(typeof(d)) // 输出 <type 'Dictionary'>
        println(typeof(e)) // 浮点数和整数的类型是一样的，输出 <type 'Number'>
        println(typeof(f)) // 输出 <type 'Function'>
    ```

    *4.3 可变类型与不可变类型*
    
    - 数字，字符串为不可变类型
    - 列表，字典，对象，函数及方法为可变类型
    - 用 `addressOf()` 函数获取标识符引用内容的地址

    ```javascript
        // mutations.yan
        var a = []
        var b = a
        var c = 3
        var d = c

        println(addressOf(a))
        println(addressOf(b))
        println(addressOf(c))
        println(addressOf(d))
    ```

    输出结果
        
    ```javascript
    >>> yan mutations.yan
    0x55ce10912350
    0x55ce10912350
    0x55ce10913a30
    0x55ce10914280
    ```
    > 很显然，a，b 对应的地址相同；而 c，d，对应的地址不同，这是因为可变类型赋值拷贝引用，不可变类型赋值拷贝值
    
    ---
    *4.4 连续赋值*
    ```javascript
        // multiple_declearation.yan
        var a = 3
        var b = var c = a

        var d = var e = var f = var g = [] 
        // 连续赋值的特性与普通赋值一样
    ```

    *4.5 关于常量*

    Yan 语言并没有内建的保证常量不可变性的机制，一般默认全部大写的变量名指代的是用户定义的常量，内建的三个常量为 `var true = 1`, `var false = 0`, `var null = 0`，用于语义上表示布尔值和空值

    **注: 在 Yan 层面, `false == null` 为 `true`, 但在 C++ 层面，
    常用 `null` 表示空值，因为 `null` 是 C++ 层 `Number` 类地址保持不变的一个预初始化的静态成员**

    *4.6 算术运算*
    - 支持的运算符 `+, -, *, /, ^`
    - 求余运算 `%` 暂时没有被作为运算符，内置 `math` 模块的函数 `math.imod(x, y)` 提供了整数求余的功能，等价于 `x % y`
____

    
5. Yan 控制语句
    
    Yan 语言有 3 种类型的控制语句：`if`, `while`, `for`，控制流语句均有表达式版本和语句版本
    
    *5.0 比较运算符与逻辑运算符*
    ```javascript
        // 比较运算符表示的是一些结果为布尔值的运算符
        // 值比较运算符只能用于数和字符串直接

        // == 值相等
        0 == 1 // false
        1 == 1 // true
        '1' == '1' // true

        // != 值不等
        0 != 1 // true
        1 != 1 // false
        
        // 大小比较
        1 > 2 // false
        2 <= 1 // false

        // 逻辑运算符用于在布尔值之间进行运算，结果仍为布尔值
        // and 和运算，左右两端都为 true 时表达式结果为 true
        true and false // false
        true and true // true
        false and true // false
        false and false // false
    
        // or 或运算，左右两端任意一端为 true 时表达式结果为 true
        true or false // true
        false or true // true
        true or true // true
        false or false // false

        // not 运算, 只接受右侧一个输入
        // 右侧为 true 结果为 false，右侧为 false 结果为 true
        not true // false
        not false // true
    ```    

    **注：由于 Yan 没有内建的 `Bool` 类型，所有返回布尔值的表达式在结果为 `true` 时返回 `1`，反之返回 `0`**
    

    *5.1 if 语句*

    表达式版本

    - **基础语法：if <表达式> then <表达式> [elif <表达式> then <表达式>]\* [else <表达式>] [end]**
    - **[] 表示可选部分 (出现 0 次或 1 次), []\* 表示可选，可重复部分 (任意次数)，其余为必须部分, <>内为需要根据实际替换的部分**
    - 用于逻辑判断的表达式返回值须为数，非 0 则视为 `true`，0 则视为 `false`
    - **if <表达式> then <表达式>** 等价于 **if <表达式> then <表达式> else 0** 

    ```javascript
        // if_expr.yan
        var a = 1
        var b = if a == 0 then 'a is 0' else 'a is not 0'
        // 赋值等号后的整体是一个 if 表达式
        // 根据 a == 0 这个条件选择性的返回 then 后的值或 else 后的值
        var c = 0
        var d = if c == 1 then 0 elif c == 0 then 2 else 1
        // elif 分支在中间，可以增加判断的情况数

        println(b) // a is not 0
        println(d) // 2
    ```

    语句版本

    绝大多数时候，控制流语句决定的不只是值，而是程序的走向，语句版本的语法与表达式版本基本类似
    ```javascript
        // if_statement.yan
        var a = 2
        if a > 0 then
            // 在 then 后换行或者使用 ; 表明后面跟得是一系列语句
            println('a > 0') 
            // 这里还可以继续写别的语句
        else
            // else 分支同理
            println('a <= 0')
        // 在最后一个分支后用 end 表明整个 if 语句的结束
        end

        var password = '114514'
        if password == '' then
            println('You should input your password!')
        elif password == '114514' then
            println('Password matched!')
        else
            println('Password unmatched!')
        end
    ```

    5.2 while 语句

    - while 语句会循环执行控制块内的代码，直到条件不满足为止
    - while 并没有严格的表达式版本，它的表达式版仅作为循环的简化写法，并不产生值
    - **基本语法：while <表达式> then <表达式> [end]**

    ```javascript
        // while_statement.yan
        var i = 1
        var sum = 0
        while i <= 100 then
            var sum = sum + i // 理解为 sum += i，暂不支持 += 等就地运算符
        end

        println(sum) // 5050

        // 当然上述也可以用表达式版的 while 等价书写
        var i = 1
        var sum = 0
        while i <= 100 then var sum = sum + i
        // 只有当语句版的 while 块内仅有一行变量赋值或函数调用时才可这样简化
        println(sum)
    ```

    5.3 for 语句

    - `for` 语句的作用也是循环
    - **语法：for <变量名> = <初始值> to <终止值> [step <表达式>] then <表达式> [end]**
    - 初始值和 `step` 关键字后指定的表达式值必须为 `Number` 类型的值
    - `step` 后的值用于指定每次循环计数器变量的变化量，可正可负，不写默认为 1
    - **变量的取值区间为 [初始值, 终止值)，左闭右开**

    ```javascript
        // for_statement.yan
        var result = 0
        for i = 0 to 100 then
            var result = result + i // 其实就是自动设定了一个循环计数器
        end
        println(result) // 5050

        for i = 0 to 10 step 2 then 
            println(i) // 输出 [0, 10) 内所有偶数
        end
    ```

    `for` 语句也有表达式版，它的表达式版一般被称为**列表产生式**，它会记录每次循环计数器的值，放在一个列表里，并作为整个表达式的结果
    ```javascript
        // for_expr.yan
        var lst = for i = 0 to 100 then i
        // then 后的表达式即为 for 表达式每次记录的值
        println(lst)
    ```

    运行结果
    ```javascipt
    >>> yan for_expr.yan
    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99]
    ```

    for 语句还有一种形式，这种形式也成为 `for-each 语句` 或是 `range-based for loop`
    - 用途：枚举（遍历）列表中的每一个元素
    - 基本语法：**for <循环变量> in <可迭代表达式> then <表达式> [end]**
    ```javascript
        // foreach.yan
        var lst = [1, 2, 3, 4, 5]
        for i in lst then
            // 输出 lst 列表内的每一个值
            // 循环变量 i 此时不再单单是一个计数器，而是代表了 lst 列表内的每一个值
            // 有多少个元素，就有多少次循环
            println(i) 
        end

        // 当然遍历列表这种事传统的 for 循环也能做到
        // len() 是一个内置函数，用于取得列表内元素的个数或字符串的长度
        for i = 0 to len(lst) then
            // lst[i] 是下标取值语法，表示取列表中第 i 个值，下标 0 表示第一个
            println(lst[i])
        end

        // 这样的 for 语句同样可以改写为表达式形式
        var new_lst = for i in lst then i * 2
        println(new_lst) // [2, 4, 6, 8, 10]
    
        // 有一个内置函数 range() 可以等效替代传统的 for 语句
        for i in range(100) then
        // 等价于 for i = 0 to 100 then 
            println(i)
        end
    ```

    **for, while, if 语句可以相互嵌套**
    ```javascript
        // 编写一个查找 [m, n] 内 x 的倍数的程序
        // 导入 math 模块内的 imod 函数到全局命名空间，因为要使用求余运算
        // import 是一个内置函数，用于导入模块
        var mod = import('math.imod')

        var x = 2
        var m = 1
        var n = 100

        for i = m to n then
            // 若 i % x 为 0，则 i 为 x 的倍数
            // 这里也可以等价写成 if not mod(i, x) then ...
            if mod(i, x) == 0 then
                println(i)
            end
        end
        // end 应与 then 一一对应
    ```

6. Yan 列表及字符串
    
