#ifndef _AD9959_H_
#define _AD9959_H_
#include "sys.h"
#include "stdint.h"
//AD9959�Ĵ�����ַ����
#define CSR_ADD   0x00   //CSR ͨ��ѡ��Ĵ���
#define FR1_ADD   0x01   //FR1 ���ܼĴ���1
#define FR2_ADD   0x02   //FR2 ���ܼĴ���2
#define CFR_ADD   0x03   //CFR ͨ�����ܼĴ���
#define CFTW0_ADD 0x04   //CTW0 ͨ��Ƶ��ת���ּĴ���
#define CPOW0_ADD 0x05   //CPW0 ͨ����λת���ּĴ���
#define ACR_ADD   0x06   //ACR ���ȿ��ƼĴ���
#define LSRR_ADD  0x07   //LSR ͨ������ɨ��Ĵ���
#define RDW_ADD   0x08   //RDW ͨ����������ɨ��Ĵ���
#define FDW_ADD   0x09   //FDW ͨ����������ɨ��Ĵ���
//AD9959�ܽź궨��
#define CS			PEout(9)
#define SCLK		PEout(8)
#define UPDATE	PEout(10)
#define PS0			PDout(4)
#define PS1			PEout(7)
#define PS2			PEout(6)
#define PS3			PEout(5)
#define SDIO0		PEout(4)
#define SDIO1		PEout(3)
#define SDIO2		PEout(2)
#define SDIO3		PDout(1)
#define AD9959_PWR			PEout(12)
#define Reset		PEout(11)

void delay1 (u32 length);
void IntReset(void);	  //AD9959��λ
void IO_Update(void);   //AD9959��������
void Intserve(void);		   //IO�ڳ�ʼ��
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp);
void Init_AD9959(void);
void Write_frequence(u8 Channel,u32 Freq);
void Write_Amplitude(u8 Channel, u16 Ampli);
void Write_Phase(u8 Channel,u16 Phase);
















#endif
