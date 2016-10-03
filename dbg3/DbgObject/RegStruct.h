#pragma once

typedef struct _EFLAGS
{
	unsigned CF : 1;  // 进位或错位
	unsigned Reserve1 : 1;
	unsigned PF : 1;  // 计算结果低位包含偶数个1时，此标志为1
	unsigned Reserve2 : 1;
	unsigned AF : 1;  // 辅助进位标志，当位3处有进位或借位时该标志为1
	unsigned Reserve3 : 1;
	unsigned ZF : 1;  // 计算结果为0时，此标志为1
	unsigned SF : 1;  // 符号标志，计算结果为负时该标志为1
	unsigned TF : 1;  // * 陷阱标志，此标志为1时，CPU每次仅会执行1条指令
	unsigned IF : 1;  // 中断标志，为0时禁止响应（屏蔽中断），为1时恢复
	unsigned DF : 1;  // 方向标志
	unsigned OF : 1;  // 溢出标志，计算结果超出机器表达范围时为1，否则为0
	unsigned IOPL : 2;  // 用于标明当前任务的I/O特权级
	unsigned NT : 1;  // 任务嵌套标志
	unsigned Reserve4 : 1;
	unsigned RF : 1;  // 调试异常相应控制标志位，为1禁止响应指令断点异常
	unsigned VM : 1;  // 为1时启用虚拟8086模式
	unsigned AC : 1;  // 内存对齐检查标志
	unsigned VIF : 1;  // 虚拟中断标志
	unsigned VIP : 1;  // 虚拟中断标志
	unsigned ID : 1;  // CPUID检查标志
	unsigned Reserve5 : 10;
}EFLAGS,*PEFLAGS;

typedef struct _DBG_REG6
{
	/*
	//     断点命中标志位，如果位于DR0~3的某个断点被命中，则进行异常处理前，对应
	// 的B0~3就会被置为1。
	*/
	unsigned B0 : 1;  // Dr0断点触发置位
	unsigned B1 : 1;  // Dr1断点触发置位
	unsigned B2 : 1;  // Dr2断点触发置位
	unsigned B3 : 1;  // Dr3断点触发置位
	/*
	// 保留字段
	*/
	unsigned Reserve1 : 9;
	/*
	// 其它状态字段
	*/
	unsigned BD : 1;  // 调制寄存器本身触发断点后，此位被置为1
	unsigned BS : 1;  // 单步异常被触发，需要与寄存器EFLAGS的TF联合使用
	unsigned BT : 1;  // 此位与TSS的T标志联合使用，用于接收CPU任务切换异常
	/*
	// 保留字段
	*/
	unsigned Reserve2 : 16;
}DBG_REG6,*PDBG_REG6;

typedef struct _DBG_REG7
{
	/*
	// 局部断点(L0~3)与全局断点(G0~3)的标记位
	*/
	unsigned L0 : 1;  // 对Dr0保存的地址启用 局部断点
	unsigned G0 : 1;  // 对Dr0保存的地址启用 全局断点
	unsigned L1 : 1;  // 对Dr1保存的地址启用 局部断点
	unsigned G1 : 1;  // 对Dr1保存的地址启用 全局断点
	unsigned L2 : 1;  // 对Dr2保存的地址启用 局部断点
	unsigned G2 : 1;  // 对Dr2保存的地址启用 全局断点
	unsigned L3 : 1;  // 对Dr3保存的地址启用 局部断点
	unsigned G3 : 1;  // 对Dr3保存的地址启用 全局断点

	/*
	 LE,GE【已经弃用】用于降低CPU频率，以方便准确检测断点异常
	*/
	unsigned LE : 1;
	unsigned GE : 1;
	/*
	// 保留字段
	*/
	unsigned Reserve1 : 3;
	/*
	// 保护调试寄存器标志位，如果此位为1，则有指令修改条是寄存器时会触发异常
	*/
	unsigned GD : 1;
	/*
	// 保留字段
	*/
	unsigned Reserve2 : 2;
	/*
	// 保存Dr0~Dr3地址所指向位置的断点类型(RW0~3)与断点长度(LEN0~3)，状态描述如下：
	*/
	unsigned RW0 : 2;  // 设定Dr0指向地址的断点类型
	unsigned LEN0 : 2;  // 设定Dr0指向地址的断点长度
	unsigned RW1 : 2;  // 设定Dr1指向地址的断点类型
	unsigned LEN1 : 2;  // 设定Dr1指向地址的断点长度
	unsigned RW2 : 2;  // 设定Dr2指向地址的断点类型
	unsigned LEN2 : 2;  // 设定Dr2指向地址的断点长度
	unsigned RW3 : 2;  // 设定Dr3指向地址的断点类型
	unsigned LEN3 : 2;  // 设定Dr3指向地址的断点长度
}DBG_REG7,*PDBG_REG7;

