#pragma once

typedef struct _EFLAGS
{
	unsigned CF : 1;  // ��λ���λ
	unsigned Reserve1 : 1;
	unsigned PF : 1;  // ��������λ����ż����1ʱ���˱�־Ϊ1
	unsigned Reserve2 : 1;
	unsigned AF : 1;  // ������λ��־����λ3���н�λ���λʱ�ñ�־Ϊ1
	unsigned Reserve3 : 1;
	unsigned ZF : 1;  // ������Ϊ0ʱ���˱�־Ϊ1
	unsigned SF : 1;  // ���ű�־��������Ϊ��ʱ�ñ�־Ϊ1
	unsigned TF : 1;  // * �����־���˱�־Ϊ1ʱ��CPUÿ�ν���ִ��1��ָ��
	unsigned IF : 1;  // �жϱ�־��Ϊ0ʱ��ֹ��Ӧ�������жϣ���Ϊ1ʱ�ָ�
	unsigned DF : 1;  // �����־
	unsigned OF : 1;  // �����־������������������ﷶΧʱΪ1������Ϊ0
	unsigned IOPL : 2;  // ���ڱ�����ǰ�����I/O��Ȩ��
	unsigned NT : 1;  // ����Ƕ�ױ�־
	unsigned Reserve4 : 1;
	unsigned RF : 1;  // �����쳣��Ӧ���Ʊ�־λ��Ϊ1��ֹ��Ӧָ��ϵ��쳣
	unsigned VM : 1;  // Ϊ1ʱ��������8086ģʽ
	unsigned AC : 1;  // �ڴ�������־
	unsigned VIF : 1;  // �����жϱ�־
	unsigned VIP : 1;  // �����жϱ�־
	unsigned ID : 1;  // CPUID����־
	unsigned Reserve5 : 10;
}EFLAGS,*PEFLAGS;

typedef struct _DBG_REG6
{
	/*
	//     �ϵ����б�־λ�����λ��DR0~3��ĳ���ϵ㱻���У�������쳣����ǰ����Ӧ
	// ��B0~3�ͻᱻ��Ϊ1��
	*/
	unsigned B0 : 1;  // Dr0�ϵ㴥����λ
	unsigned B1 : 1;  // Dr1�ϵ㴥����λ
	unsigned B2 : 1;  // Dr2�ϵ㴥����λ
	unsigned B3 : 1;  // Dr3�ϵ㴥����λ
	/*
	// �����ֶ�
	*/
	unsigned Reserve1 : 9;
	/*
	// ����״̬�ֶ�
	*/
	unsigned BD : 1;  // ���ƼĴ����������ϵ�󣬴�λ����Ϊ1
	unsigned BS : 1;  // �����쳣����������Ҫ��Ĵ���EFLAGS��TF����ʹ��
	unsigned BT : 1;  // ��λ��TSS��T��־����ʹ�ã����ڽ���CPU�����л��쳣
	/*
	// �����ֶ�
	*/
	unsigned Reserve2 : 16;
}DBG_REG6,*PDBG_REG6;

typedef struct _DBG_REG7
{
	/*
	// �ֲ��ϵ�(L0~3)��ȫ�ֶϵ�(G0~3)�ı��λ
	*/
	unsigned L0 : 1;  // ��Dr0����ĵ�ַ���� �ֲ��ϵ�
	unsigned G0 : 1;  // ��Dr0����ĵ�ַ���� ȫ�ֶϵ�
	unsigned L1 : 1;  // ��Dr1����ĵ�ַ���� �ֲ��ϵ�
	unsigned G1 : 1;  // ��Dr1����ĵ�ַ���� ȫ�ֶϵ�
	unsigned L2 : 1;  // ��Dr2����ĵ�ַ���� �ֲ��ϵ�
	unsigned G2 : 1;  // ��Dr2����ĵ�ַ���� ȫ�ֶϵ�
	unsigned L3 : 1;  // ��Dr3����ĵ�ַ���� �ֲ��ϵ�
	unsigned G3 : 1;  // ��Dr3����ĵ�ַ���� ȫ�ֶϵ�

	/*
	 LE,GE���Ѿ����á����ڽ���CPUƵ�ʣ��Է���׼ȷ���ϵ��쳣
	*/
	unsigned LE : 1;
	unsigned GE : 1;
	/*
	// �����ֶ�
	*/
	unsigned Reserve1 : 3;
	/*
	// �������ԼĴ�����־λ�������λΪ1������ָ���޸����ǼĴ���ʱ�ᴥ���쳣
	*/
	unsigned GD : 1;
	/*
	// �����ֶ�
	*/
	unsigned Reserve2 : 2;
	/*
	// ����Dr0~Dr3��ַ��ָ��λ�õĶϵ�����(RW0~3)��ϵ㳤��(LEN0~3)��״̬�������£�
	*/
	unsigned RW0 : 2;  // �趨Dr0ָ���ַ�Ķϵ�����
	unsigned LEN0 : 2;  // �趨Dr0ָ���ַ�Ķϵ㳤��
	unsigned RW1 : 2;  // �趨Dr1ָ���ַ�Ķϵ�����
	unsigned LEN1 : 2;  // �趨Dr1ָ���ַ�Ķϵ㳤��
	unsigned RW2 : 2;  // �趨Dr2ָ���ַ�Ķϵ�����
	unsigned LEN2 : 2;  // �趨Dr2ָ���ַ�Ķϵ㳤��
	unsigned RW3 : 2;  // �趨Dr3ָ���ַ�Ķϵ�����
	unsigned LEN3 : 2;  // �趨Dr3ָ���ַ�Ķϵ㳤��
}DBG_REG7,*PDBG_REG7;

