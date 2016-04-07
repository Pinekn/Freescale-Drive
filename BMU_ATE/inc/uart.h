#ifndef _UART_H_


extern int Uart1_Init(void);
extern int Uart2_Init(void);
extern int Uart1_Read(char *buf, unsigned int length);
extern int Uart2_Read(char *buf, unsigned int length);
extern int Uart1_Write(char *buf, unsigned int length);
extern int Uart2_Write(char *buf, unsigned int length);
extern void Uart1_Close(void);
extern void Uart2_Close(void);
extern void Uart1_ClearReadBuf(void);
extern void Uart2_ClearReadBuf(void);


#endif
