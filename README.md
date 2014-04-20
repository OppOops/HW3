1. 更改了dhrystone的內容, 寫成同一個class之外, 省略了Proc_2()的測試.

2. 自行寫了java_array, java_object, field_value 等data structure, 由vm本身作reference和heap的maintain.
  (create_instance, create_array, ...)

3. 寫完invoke function, 改進對java static method的支援, maintain了parameter在function傳遞的值

4. 完成剩餘6x個instruction

---------------------------------------------

0. 先將virtual-method放好對應的refrence (此步驟未測量時間)

1. 改進原本dispatch尋找opcode的效率, 使用事先建好的lookuptable, 時間由 51 sec 降為 20 sec.

2. 去除冗長的計算, 改用較少的function call(例如移除debug message, 不必要的type check) 時間由20 sec 降為 16.5 sec

3. 改進load_to_reg以及store_to_reg的效率, 時間由 16.5 sec 降為 13.6 sec

4. 使用direct threaded code, 時間由 13.6 sec 降為 11.1 sec

5. 將load_to_reg以及store_to_reg改成define marco, 時間由 11.1 sec 降為 8.08 sec

6. aosp dalvik-vm 時間 約為 7秒, JVM約為0.2秒

p.s. 1. 由於不會用makefile定義marco, 所以必須手動更改simple_dvm.h 的內容
  
     2. 將bytecode.c中與vm相關函式另外寫到dvm.c, direct threaded code 寫在 bytecode_thread.c 中
    
     3. 最終結果:

	     576  71.6%  71.6%      576  71.6% dispatch_DTC
	     119  14.8%  86.4%      119  14.8% invoke_virtual_method
	      45   5.6%  92.0%       45   5.6% op_utils_invoke_35c_parse (inline)
	      32   4.0%  96.0%       32   4.0% copy_parameter
	      15   1.9%  97.9%       60   7.5% op_utils_invoke_35c_parse
	       9   1.1%  99.0%        9   1.1% runMethod
	       4   0.5%  99.5%        4   0.5% store_double_to_reg
	       3   0.4%  99.9%        3   0.4% move_bottom_half_result_to_reg
	       1   0.1% 100.0%        1   0.1% load_reg_to_double
	       0   0.0% 100.0%      658  81.8% _init

github網址:  https://github.com/OppOops/HW3
