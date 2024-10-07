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
    - 自带编辑器，调试器正在开发中，测试编辑器在安装目录 yan-editor 文件夹内，main.py 为入口 (已弃用)
    

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
    - 变量的命名符合标识符的规范（除不能以数字开头，不能为保留字，不能使用特殊ascii符号，如:,@等，无其他要求）
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
        // find_num.yan
        // 编写一个查找 [m, n] 内 x 的倍数的程序
        // 导入 math 模块内的 imod 函数到全局命名空间，因为要使用求余运算
        // import 是一个内置函数，用于导入模块
        var mod = import('math.imod')

        var x = 2
        var m = 1
        var n = 100

        for i = m to n + 1 then
            // 若 i % x 为 0，则 i 为 x 的倍数
            // 这里也可以等价写成 if not mod(i, x) then ...
            if mod(i, x) == 0 then
                println(i)
            end
        end
        // end 应与 then 一一对应
    ```

6. Yan 列表及字符串

    6.1 列表

    - Yan 列表是一种动态大小，且可存储任意类型的数据结构，我们通过实例来了解列表的使用
    - 列表的操作函数均为内置函数

    ```javascript
        // list.yan
        var lst = [1, 2, 3] // 定义列表，内部元素用逗号分隔
        println(lst) // 可以直接输出

        // 访问列表中的元素有三种方式
        var a = lst / 0 // 最古老的一种办法，用内部重载的 '/' 运算符接下标访问 (在 Yan 的 python 子集实现仅支持这种方法)

        // 普通的 [下标] 方法
        var b = lst[1]

        // for-in 循环
        for element in lst then
            println(element)
        end
    ````

    列表支持的基础操作有：
    - 求长度 **len()**
    - 增加值 **append() 或 '+'**
    - 删除指定索引的值 **remove() 或 '-'**
    - 拼接 **concat() 或 '\*'**

    我们通过实例了解他们的用法
    ```javascript
        // list_operation.yan
        var lst = [1, 2, 3, 4, 5]
        println(len(lst)) // 求长度

        // 这两种方式的结果都是 lst 变为 [1, 2, 3, 4, 5, 6]
        append(lst, 6) // 用内置函数修改的是原列表本身
        var lst = lst + 6 // 用运算符则是创建了一个新的列表并重新赋给 lst

        var lst = ['1', '2', '3']
        // 其他的操作也同理
        remove(lst, 0) // lst -> ['2', '3'], 0 代表索引
        var lst = lst - 0 // lst -> ['2', '3'], 0 同样代表索引
        concat(lst, ['4', '5', '6'])
        var lst = lst * ['4', '5', '6']

        // 列表可以嵌套
        var lst = [[1, 2, 3], ['1', '2', '3']]

        // 嵌套时可以使用多个 [下标] 访问元素
        println(lst[1][0])
        // 或者使用运算符，但须加括号指明顺序
        println((lst / 1) / 0)
        // 多重 for-in 循环也可以
        for i in lst then
            for j in i then
                println(j)
            end
        end

        // 在数学库里，提供了列表求和求积的操作
        import('math')
        println(math.sum([1, 2, 3, 4]))
        println(math.product([1, 2, 3, 4]))

        // 为增强可读性，Yan 无法对字面量进行操作，需要定义为变量，简单的列表索引除外
        println([1, 2, 3][0]) // 合法
        println([[1, 2, 3], [4, 5, 6]][0][0]) // 二阶以上操作不合法
    ```


    6.2 Yan 字符串

    - 字符串存储一系列字符，Yan 没有内置的"字符"类型，取而代之的是长度为 1 的字符串
    - 字符串字面量由 `''` 包括的内容来声明，不可由 `""` 包括
    - 字符串支持 utf-8
    - 字符串基本操作可以视为不可变的列表, 以 `'+'` 拼接，仅能用 `[] 或 for 循环`遍历, 不允许 '/'

    ```javascript
        // strings.yan
        var str = 'Hello, world!'
        println(str)
    
        // 字符串可以索引，索引结果为一个长度为 1 的字符串
        println(str[0]) // 'H'
        // 理论上可以无限索引下去，长度为 1 的字符串也可以索引，但是这种写法没有意义
        println(str[0][0][0][0][0][0]) // 'H'
        // 同列表一样，无法直接对字面量进行索引操作
        println('hello'[0]) // 不合法

        // 使用 for-in 循环遍历字符串
        for character in str then
            println(character)
        end

        // 字符串长度同样可以由 len() 获取
        println(len(str))

        // 拼接字符串, 可以连续拼接，每次拼接会产生一个新的字符串
        println('1' + '2' + str)
    ```

    字符串的高阶操作

    - 字符串高阶操作实现在 string 内置库中，等到讲解内置库和导入语法的时候再进行说明


7. Yan 函数

    Yan 支持高阶函数，匿名函数，函数嵌套等特性
    我们从代码学习函数的定义与使用

    7.1 基本语法
    
    - lambda 函数定义语法：**function [<函数名>]([<参数名>[, <参数名>]\*]) -> <表达式>**
    - 普通函数定义语法：**function <函数名>([<参数名>[, <参数名>]\*]) <函数体:语句块> end**
    - 函数调用语法: **<函数名>([<参数名>[, <参数名>]\*])**
    - 函数返回值语法: **return [<表达式>]**
    - 函数若无显式返回或 `return` 关键字后无表达式，默认返回 `0`
    - 函数参数类型，返回值完全动态

    ```javascript
        // functions.yan
        // 定义普通的无参函数
        function foo()
            println('Inside foo function!')
        end

        // 调用函数
        foo()

        // 定义一个有参函数，有返回值函数
        // a, b 为形式参数，调用时其值会被替换为传入实参所对应的值
        function add(a, b)
            return a + b
        end

        // 下面这些调用都是合法的，函数完全动态
        var a1 = add(1, 2)
        var a2 = add('hello,', ' world!')
        var a3 = add([1, 2, 3], 4)
    
        // 像 println(), 之前用的 typeof(), addressOf(), append() 等均为函数，只不过他们是内置函数
    
        function add2(a, b) -> a + b
        // add2 和 add 是完全等价的

        // 若需定义匿名函数，省略 lambda 函数的函数名即可
        function(a, b) -> a + b
    ```

    7.2 高阶函数
   
    - 可以将一个函数赋给一个变量，也可以将其作为另一个函数的参数或返回值
    - 函数赋值仅拷贝对于函数的引用，不复制函数本身

    ```javascript
        // advanced_functions.yan

        function f()
            println('Hello, world!')
        end

        var foo = f
        // 可以和使用 f 函数一样使用 foo
        foo()

        // 最常用的函数赋值可能是匿名函数的赋值
        // 为保证可读性，经常这么书写
        var toFloat = function(x) -> x + 0.0

        // 说明一下这里的操作
        // 若希望将整数转为!!近似相等!!(math.isClose 描述的近似相等,不过绝大多数情况下也可以用 ==) 的浮点数，可以直接 + 0.0


        // 函数作为函数参数，那么作为参数的函数称为回调函数
        function callFunc(f)
            return f()
        end

        // 那么此时任意无参函数均可以通过这个 'callFunc(f)' 来调用
        callFunc(f) // f 是上边的那个函数，为实参

        // 函数也可以作为函数返回值
        // 一般与函数嵌套定义一起使用
        function produceAdder(x)
            function adder(v)
                // 嵌套函数与外部函数作用域独立，adder() 内无法访问到 produceAdder(x) 的参数 x
                // 使用 nonlocal x 将 adder() 作为一个闭包函数，可以捕获生成其时 x 的值 
                nonlocal x
                return v + x
            end
            return adder
        end

        var adder = produceAdder(3)
        println(adder(2, 3)) // adder 是一个可以让输入值加 3 并输出的函数

        // 直接 println() 函数会输出函数定义时实际的名字和它的地址
        function g() -> null

        var h = g
        println(h) // 输出：<function g at 一个地址>，因为 h 只是 g 的一个引用
    ```

    7.3 变长参数与可选参数

    - 仅有内置函数，部分库函数存在可选参数，C++ 实现的 API 提供了可选参数的声明方法，但是 Yan 语言没有可选参数的声明 (以后可能作为新特性)
    - 作为弥补，Yan 语言支持了 C++ 实现无法达到的可变参数(变长参数)

    ```javascript
        // args.yan

        // 可选参数，例如内置的 input() 函数
        var s1 = input() // 无参，等效于 readLine()
        var s2 = input('Please input your name: ') // 有参数时为提示符
        // 其他的内置函数的参数说明在讲解内置函数时提及

        // 变长参数没有单独的语法或关键字，仅由参数名决定
        // 变长参数声明为: '_ + 变长参数名 + _', 例如 _args_, _num_, _cfg_ 等等
        
        function prints(_args_)
            // 变长参数在入参后解析为一个列表，直接用 for 循环遍历即可
            // 函数体内引用变长参数时去掉两头的下划线
            for arg in args then
                println(arg)
            end
        end

        // 变长参数可以跟在固定参数后
        function newList(length, _values_)
            var lst = []
            for i = 0 to length then
                append(lst, values[i])
            end
            return lst
        end1

        // 固定参数必须有值，变长参数可以一个没有
        var l1 = newList(3, 1, 2, 3) // [1, 2, 3]
        var l2 = newList(2, 3, 3) // [3, 3]
        var l3 = newList(100) // []

        // 一个函数最多有一个变长参数，且必须在参数列表的最末尾
        function error(_args1_, _args2_)
            return null
        end // 此函数不合法

        function error2(_args1_, a)
            return null
        end // 此函数同样不合法
    ```

8. Yan 内置函数
    
    ***以下参数声明均为 `参数名: 参数类型 [= 默认值]`***

    **含有默认值的参数可以省略**
    
    ***内置函数参数名均以 `_` 开头***

    **以下所有未注明的返回 `0` 的函数均指的是 `null` 所代表的值**

    **`Object` 为所有类型的基类，即代表任意类型**

    8.1. IO 函数
    - `print(_obj)`: 在 `stdout` 输出传入参数的字符串形式 (无追加换行符)
        - 参数 1： **_obj: Object**
        - 返回值： 0 (null 指定的值)
    - `println(_obj)`: 在 `stdout` 输出传入参数的字符串形式 (追加换行符号)
        - 参数与返回值同 `print(_obj)`
    - `readLine()`: 从 `stdin` 读取一行输入并以字符串形式返回
        - 参数：无
        - 返回值：输入的字符串: **String**
    - `input(_prompt)`：输出以 _prompt 参数指定的提示符开头，并执行与 `readLine()` 完全一致的读取返回操作
        - 参数 1: **_prompt: String = ''**
        - 返回值：输入的字符串: **String**
        - 引发异常: `TypeError`
    - `readFile(_filename)`: 以文本形式打开参数 _filename 所指定的文件，并返回其内容的字符串形式
        - 参数 1: **_filename: String**
        - 返回值: 文本文件的内容: **String**
        - 引发异常: `OSError`, `TypeError`, `RuntimeError`
    - `writeFile(_filename, _str, _mode)`：以文本形式打开参数 _filename 所指定的文件，并向其写入参数 _str 所指定的内容，追加写入亦或是清空写入取决于参数 _mode 所指定的模式
        - 参数 1: **_filename: String**
        - 参数 2: **_str: String**
        - 参数 3: **_mode: String \[其值只能为 'w' 或 'wa'\] = 'w'**
        - 返回值: 0
        - 引发异常: 同 `readFile(_filename)`
    
    8.2 工具函数
    - `typeof(_object)`: 以字符串形式获取参数 _object 的值对应的类型
        - 参数 1: **_object: Object**
        - 返回值: 类型描述: **String**
    - `len(_seq)`: 返回参数 _seq 代表的序列的长度
        - 注释: **序列** 特指重载了 `Len()` 方法的类型，内置的字符串，列表，字典均可视为序列，在非序列类型上使用 `len()` 将引发 `TypeError`
        - 参数 1: **_seq: Object[impls Len()]**
        - 返回值: 长度: **Number[Integer]**
        - 引发异常：`TypeError`
    - `parseInt(_str)`: 返回参数 _str 指定字符串的整数表达形式，
    若转换失败，返回 0
        - 参数 1: **_str: String**
        - 返回值: 整数值: **Number[Integer]**
        - 引发异常：`TypeError`
    - `parseFloat(_str)`: 返回参数 _str 指定字符串的浮点数表达形式，若转换失败，返回 0
        - 参数 1: **_str: String**
        - 返回值: 浮点数值: **Number[Floating]**
        - 引发异常: `TypeError`
    - `str(_object)`: 返回参数 _object 的字符串形式
        - 参数 1: **_object: Object**
        - 返回值：字符串值：**String**
    - `eval(_code)`: 创建新的执行上下文，解释并运行参数 _code 指定的代码片段，新上下文在代码执行结束后立即销毁
        - 参数 1: **_code: String**
        - 返回值: 0
        - 引发异常: 所有类型的异常
    - `panic(_cause)`: 引发 `Panic` 类型的异常，参数 _cause 的字符串形式将作为异常的描述信息
        - 参数 1: **_cause: Object**
        - 返回值: 不返回
        - 引发异常: `Panic`
    - `isInteger(_num)`: 辨别参数指定的值是否为整数值
        - 参数 1: **_num: Number**
        - 返回值: 0 或 1 代表的布尔值: **Number**
        - 引发异常: `TypeError`
    - `isFloating(_num)`: 等价于 `not isInteger(_num)`
    - `addressOf(_object)`: 获取参数 _object 指向对象的地址，以十六进制字符串返回
        - 参数 1: **_object: Object**
        - 返回值: 字符串地址值: **String**
    - `del(_varName)`: 从当前上下文符号表中找到字符串`_varName`代表的变量，移除它并释放所在内存空间
        - 参数 1: **_varName: String**
        - 返回值：0
        - 引发异常: `RuntimeError`
    - `global(_varName, _value)`: 在局部上下文使用时，从全局符号表内查找字符串 `_varName` 代表的变量，并将其跨作用域修改为参数 _value 指定的值
        - 参数 1: **_varName: String**
        - 参数 2: **_value: Object**
        - 返回值: 0
        - 引发异常: `TypeError`, `RuntimeError`
    - `builtins()`: 获取一个包含了所有内置函数名的列表
        - 返回值：`List[String]`

    8.3 列表，映射操作函数
    - `append(_lst, _val)`: 向参数 _lst 指定的列表追加参数 _val 指定的值
        - 参数 1: **_lst: List**
        - 参数 2: **_val: Object**
        - 返回值: 0
        - 引发异常: `TypeError`
    - `remove(_lst, _index)`: 移除参数 _lst 指定的列表索引 _index 位处的值
        - 参数 1: **_lst: List**
        - 参数 2: **_index: Number[Integer]**
        - 返回值: 0
        - 引发异常: `RuntimeError`, `TypeError`

    - `concat(_lst1, _lst2)`: 拼接参数 _lst2 所指定的列表到 `_lst1` 上
        - 参数 1: **_lst1: List**
        - 参数 2: **_lst2: List**
        - 返回值：0
        - 引发异常：`TypeError`
    - `set(_lst, _index, _value)`: 修改参数 _lst 指定的列表 索引 _index 的值为参数 _value 的值
        - 参数 1: **_lst: List**
        - 参数 2: **_index: Number[Integer]**
        - 参数 3: **_value: Object**
        - 返回值: 0
        - 引发异常: `RuntimeError`, `TypeError`
    - `range(_et, _st, _step)`: 返回一个包含了 `[_st, _et)` 且每个元素间隔为 _step 的列表
        - 参数 1: **_et: Number**
        - 参数 2: **_st: Number = 0**
        - 参数 3: **_step: Number = 1**
        - 返回值：包含区间值的列表: **List**
        - 引发异常: `TypeError`
    - `keys(_mapping)`: 返回一个包含了映射所有键值的列表
        - 注释: **映射** 一般指类对象或字典
        - 参数 1: **_mapping: Object[impls GetAttr() + SetAttr()]**
        - 返回值：键值列表：**List[String]**
        - 引发异常: `TypeError`
    - `values(_mapping)`: 返回一个包含了映射所有值的列表
        - 参数，引发异常同`keys(_mapping)`
        - 返回值: 值列表: **List**

    8.4 数学与计算函数
    - `sin(x), cos(x), tan(x), ln(x), log(x), abs(x), sqrt(x)`
        - 参数 1: **x: Number**
        - 返回值: 计算结果: **Number**
        - 注释: 更多数学函数参见模块 `math` 的说明


9. Yan 文件IO，导入，内置模块与异常

    9.1 文件IO
    - 文件操作在内置函数与 `fs` 内置模块中均有实现
    - 内置函数仅允许简单的读取/写入文本文件
    - `fs` 库同时包含文件系统相关 API

    我们先来看内置函数的使用，这通常是实现快速写入少量内容，一次性读取的最简单的办法

    ```javascript
        // file_io_builtins.yan

        // 参照上文对于内置函数的说明，readFile, writeFile的使用也非常简单
        println(readFile('test.txt'))
        // 以文本形式打开 test.txt, 读取并一次性返回其所有内容，并自动关闭文件流

        var s = readLine()
        writeFile('content.txt', s)
        // 从控制台读取输入, 并写入 content.txt
        // 此时的写入模式为清空写入
        // 若需要追加写入文件, 在第三个参数指定 'a' 模式即可
        writeFile('content.txt', s, 'wa')
        // 第三个模式参数若只写 'w' 与省略等价

        // 大量重复的调用 readFile(), writeFile() 并不是一个理想的实现
        
        // 尽量避免以下代码
        var l = ['1', '2', '3', '4', '5']
        for i in l then
            writeFile('nums.txt', i, 'wa')
        end
    ```

    现在我们来看 `fs` 库提供的 API
    - **函数**
    - `fs.open(file: String, mode: String = 'r') -> FileObject`
    - `fs.close(fileObject: FileObject) -> null`
    - `fs.getFileType(path: String) -> Number[Integer]`
    - `fs.getFilePermissions(path: String) -> String`
    - `fs.exists(path: String) -> Bool (aka 'Number[Integer]')`
    - `fs.getFreeSpace(path: String) -> List[String | Number[Integer]]`
    - `fs.getFileSize(path: String) -> String | Number[Integer]`
    - `fs.formatSize(size: Number[Integer]) -> String`
    - `fs.listDirectory(dir: String) -> List[String]`
    - `fs.getLastWriteTime(path: String) -> String`
    - `fs.getHardLinksCount(path: String) -> Number[Integer]`
    - **类对象**
    - `FileObject`
        - 属性 **name: String**
        - 方法 1: **read(this: FileObject) -> String**
        - 方法 2: **write(this: FileObject, content: String) -> null**
        - 方法 3: **eof(this: FileObject) -> Bool (aka Number[Integer])** 注: 返回文件指针是否达到文件末尾
        - 方法 4: **readLines(this: FileObject) -> List[String]**
        - 方法 5: **readBuf(this: FileObject, size: Number[Integer]) -> String**
        - 方法 6: **reverse(this: FileObject, size: Number[Integer]) -> null**
        - 方法 7: **length(this: FileObject) -> Number[Integer] | String** 注: 返回文件长度

    ```javascript   
        // file_io_fs.yan
        // 导入 `fs` 模块
        // 一般使用 `import` (别的方式在模块说明)
        import('fs')
        // 这样导入之后就可以用`模块名.模块内成员`的方式访问模块方法
        // 打开文件流：使用 fs.open() 函数

        var file = fs.open('test.txt')
    ```

    **file.open()** 的第二个参数也是模式参数，它的可选模式更多
    - **'r': 读取文件**
    - **'w': 写入文件**
    - **'rw': 可读可写**
    - **'wa': 追加写入**
    (二进制读写模式暂未实现)
    - **'rb': 二进制读取**
    - **'wb': 二进制写入**
    - **'wba': 二进制追加写入**
    - **缺省: 等价于 'r'**

    组合中字母的顺序不允许调换，除上述 7 种模式符外其余均会引发 `ValueError`
    ```javascript
        // 打开文件流后，就可以对文件对象进行操作
        // 上文是以 文本只读 模式打开的
        // 调用 file 对象的 read() 方法读取文件中所有内容并返回
        var content = file.read()
        // 别忘了手动关闭文件流
        // 用 fs.close() 函数传入打开的文件对象即可
        fs.close(file)
    ```

    > 方法 FileObject.readBuf() 与 FileObject.reverse() 用于随机读取，移动文件指针，其中 readBuf(n) 读取并前进 n 个字节，reverse(n) 反向移动 n 个字节


    fs 库其余的一些函数均为文件系统相关的 API
    ```javascript
        filesystem.yan

        // 导入 `fs` 模块
        import('fs')

        // 获取文件类型
        var type = fs.getFileType('test.txt')
        // 0: 普通文件
        // 1: 目录
        // 2: 符号链接
        // 3: 块设备文件
        // 4: 字符设备文件
        // 5: 套接字文件
        // 6: 管道文件


        // 获取文件权限
        var perm = fs.getFilePermissions('test.txt')
        // 若无权限则返回空字符串
        // 否则返回类似 'rw-r--r--' 的字符串

        // 获取磁盘剩余空间
        var space = fs.getFreeSpace('/')
        // 第一个元素为总空间，第二个元素为剩余空间
        // 单位为字节
        // 参数理论上任意路径均可，最好是根目录 (Windows 下一次只能统计一个磁盘 (分卷)，所以为盘符, 若统计全部需要遍历所有盘符)

        // 获取文件大小
        var size = fs.getFileSize('test.txt')
        // 单位为字节

        // 格式化文件大小并输出
        var sizeStr = fs.formatSize(size)
        println(sizeStr)
        // 输出类似 '1.23 MB' 的字符串

        // 列出目录下的文件
        var files = fs.listDirectory('.')
        // 返回一个包含文件名的列表

        // 获取文件最后修改时间
        var time = fs.getLastWriteTime('test.txt')
        // 格式为 'YYYY-MM-DD HH:MM:SS'

        // 获取文件硬链接数
        var links = fs.getHardLinksCount('test.txt')
        // 若无硬链接则返回 1
        // 以上函数均可在任意路径下调用，但若路径不存在或无权限则会引发 `OSError` 异常
    ```

    9.2 Yan 模块
    
    模块可以分为 Yan 语言实现的模块和 C++ 实现的库
    - 由 Yan 语言编写的模块，一般以 `.yan` 结尾
    - 由 C++ 编写的库，一般以 `.so` (windows 为 `.dll`) 结尾
    - 内置模块位于 `builtins` 目录下 (Yan 环境变量中 `builtins-import-path` 所指定的值)
    - 内置库位于 `libs` 目录下 (Yan 环境变量中 `libs-import-path` 所指定的值)


    模块导入使用两个内置的**函数** `import()` 和 `require()`, 均接受单个字符串为参数，
    有如下形式:
    - `import(模块名)` 将模块名所指定的模块包装成对象导入当前作用域，无返回值
    - `import(模块名.成员)` 将模块名所指定的模块的成员导入当前作用域，返回其值
    - `require(模块名)` 将模块内所有符号导入当前作用域，不包装对象，无返回值
    - `require('@' + 库名.成员名)` 从 C++ 库中加载一个符号，返回其值
    
    > 其中，模块名可为以 '/' 分割的相对路径

    ```javascript
        // modules.yan
        import('math')
        // 导入 math 模块，并将其包装成对象导入当前作用域
        // 其成员可以直接使用，如 `math.pi`

        var cbrt = import('math.cbrt')
        // 导入 math 模块的 cbrt 函数，并将其导入当前作用域
        // 其值可以直接使用，如 `cbrt(27)`

        require('math')
        // 导入 math 模块的所有符号，但不包装成对象
        // 其成员可以直接使用，如 `pi`

        var exists = require('@fs.Exists')
        // 导入 C++ 库 fs 中的 Exists() 函数
        // 其值可以直接使用，如 `exists('test.txt')`
    ```

    内置的模块基本都是 C++ 实现的，但也有一些由 Yan 语言实现的模块，如 `math` 模块，

    使用 C++ 实现的模块一般需要一个存根文件，用 Yan 语言的 `require + @` 语法编写，用于一次性导入所有符号
    
    例如内置库 fs 的存根文件为 `fs.yan`

    ```javascript
        // fs.yan (位于 builtins/ 下)
        var open = require('@fs.Open')
        var close = require('@fs.Close')
        var exists = require('@fs.Exists')
        var getFreeSpace = require('@fs.GetFreeSpace')
        var getFilePermissions = require('@fs.GetFilePermissions')
        var getFileType = require('@fs.GetFileType')
        var getFileSize = require('@fs.GetFileSize')
        var formatSize = require('@fs.FormatSize')
        var listDirectory = require('@fs.ListDirectory')
        var getHardLinksCount = require('@fs.GetHardLinksCount')
        var getLastWriteTime = require('@fs.GetLastWriteTime')
    
        // fs 库文件本身存在于 libs/fs.so
    ```

    只有这样做，才可以对 C++ 模块使用导入语法的前三种形式

    导入的路径解析优先级：
    - 无论何种导入：当前路径下最先
    - 使用相对路径的导入
    - 环境变量中指定的内置模块 (内置库) 路径
    

    导入失败时会引发 `OSError` 异常，有以下可能
    - 模块路径解析失败
    - 模块导入过程中出现错误 (解释器会捕获并打印模块内的错误信息)
    - 库导入过程中出现错误
    - 导入的成员不存在

    ** 若是库导入失败，解释器会视为致命错误，直接退出，其余导入错误视为普通异常 **


    9.3 Yan 异常与 defer 语句
    
    Yan 没有异常处理子句，用户自定义的异常可以由 `panic()` 引发崩溃，解释器的运行时异常和 panic 可用一定手段恢复

    解释器异常分为如下类别:

    - 词法/语法错误 (parse/syntax error)
    - 运行时错误 (runtime error)
    - 致命错误 (fatal error)
    - panic

    ```javascript
        // 词法/语法错误语法错误通常仅包含行号，文件名，错误描述等信息
        // 运行时错误通常包含行号，列号，文件名，错误描述，关联源代码，调用堆栈等信息
        // 致命错误会导致解释器崩溃，包含 C++ 异常信息
        // panic 通常由用户自定义的 panic() 函数引发，包含用户自定义的信息

        @abc // 出现非法符号 @，引发词法错误 IllgalCharacterError
        
        >>> File "<stdin>", line 1
            IllegalCharacterError: Found unexpected '@'

        println(0 // 括号未匹配，引发一种语法错误 SyntaxError
        >>> File "<stdin>", line 1
            SyntaxError: Mismatched '(' while calling function (unfinished)

        var a = 1 / 0  /* 除数为 0，引发运行时错误 RuntimeError */
        >>> RuntimeError: Division by zero
              (Possibly related source: `var a = 1/0`)
            Stack Traceback:
              at <module> [<stdin>:1:11]
        
        panic('error') /* 引发用户自定义的运行时 panic 异常，描述信息为 'error' */
        >>> Panic: error
              (Possibly related source: `panic('error')`) 
            Stack Traceback:
              at panic [<stdin>:1:7]
              at <module> [<stdin>:1:1]

        require('@not-exist-lib.foo') /* 导入不存在库，引发致命错误 */

        >>>
        Fatal: Dynamic lib 'not-exists-lib' opened failed: lib/yan-not-exists-lib.so: cannot open shared object file: No such file or directory
        yan: yan-lang.hpp:5874: RuntimeResult* (* LoadNativeFunctionImplementation(const std::string&, const std::string&))(Context*): 
          Assertion `dylib != nullptr' failed.
        Aborted (core dumped)

        // 此报错信息因平台而异，此处为普通 GNU/Linux 上运行时的信息
    ```

    `defer` 语句用于在函数调用链中引发异常后自动执行一段代码，常用于资源释放，如文件关闭等。
    - 无论函数是否引发异常，`defer` 语句一定会执行

    
    在 `defer` 语句中执行的代码可以选择将程序从异常中恢复，并继续执行后续代码，或是直接退出程序

    因此 `defer` 语句是 yan 语言中重要的异常处理方式

    ```javascript
        function f()
            var x = 1 / 0 // 此处引发 RuntimeError: Division by zero
        end

        f() // 直接调用 f 会引发运行时异常

        // 现在我们定义一个异常处理函数
        function handler()
            println('Exception occured!')
        end

        function g()
            // defer 后可以跟一个表达式，一般都是函数调用，写在可能会发生异常的代码块之前
            defer handler()
            var x = 1 / 0
        end

        g() // 调用 g，会发现在原有的报错信息之前增加了 handler() 内的输出

        // 输出示例
        Exception occurred!
        RuntimeError: Division by zero
            (Possibly related source: `...`)
        Stack Traceback:
            ...

        // 异常还是引发了，但是我们成功的在异常结束之前执行了操作
    ```

    若是我们希望从异常中恢复，可以在 `defer` 语句制定的错误处理函数中使用 `recover()` 函数
    - `recover()` 函数有一个可选参数，可以是任意类型的值，用于发生错误时替代原有函数的返回值

    ```javascript
        function handler()
            println('Error occured!')
            recover()
            // 执行 recover 后，异常将会被清除，发生异常的立即函数返回值，程序会继续运行
        end

        function f()
            defer handler()
            var x = 1 / 0
        end

        f()
        println('Recover from error!')

        // 输出
        Error occured!
        Recover from error!

        // 若是发生异常的函数需要有特定返回值，可以传入 recover()

        function newHandler()
            println('Error occured! Return a string `error`')
            recover('error')
        end

        function g()
            defer newHandler()
            var x = 1 / 0
        end

        var result = g()
        println('Recover from error, result: ' + result)

        // 输出
        Error occured! Return a string `error`
        Recover from error, result: error

        function h()
            // 若不需要异常处理函数，只需捕获异常，也可以直接 defer recover()
            defer recover()
            var x = 1 / 0
        end
    ```

    若是希望了解异常发生的具体信息，以及解释器运行状态，我们介绍另一个内置库 `inspect`， 以及 `__lastexc__` 这个特殊变量的使用

    - 先来看 `__lastexc__`，这是一个会在异常处理函数（即 `defer` 关键字后调用的自定义函数）中自动定义的一个变量，它是一个字典，包含了上次发生异常的信息:
    - **category**: 异常具体类别，如 `RuntimeError`, `TypeError`
    - **details**: 异常描述信息
    - **filename**: 发生异常的文件名
    - **line**: 异常引发的行号
    - **column**: 异常引发的代码位置 (配合行号定位)
    - 除行列号为数字类型，其余均为字符串形式

    下面是 `__lastexc__` 的使用

    ```javascript
        // 导入一下 format 函数，方便输出信息
        var format = import('string.format')

        function handler()
            var exc = __lastexc__
            println(format('Caught Error: %s: %s [at %s:%d]', exc.category, exc.details, exc.filename, exc.line))
            recover()
        end

        function f()
            defer handler()
            var x = 1 / 0
        end

        f()
        println('Recovered')

        // 示例输出
        Caught Error: RuntimeError: Division by zero [at debug.yan:11]
        Recovered
    ```
