#ifndef GPIO_H_
#define GPIO_H_


typedef struct
{
  volatile uint32_t INPUT;
  volatile uint32_t OUTPUT;
  volatile uint32_t OUTPUT_ENABLE;
} Gpio_Reg;

#define GPIO0		((Gpio_Reg*)(0xF0000000))
#define GPIO1		((Gpio_Reg*)(0xF0001000))
#define	GPIO		GPIO0

#define	GPIO_IN_KEY0		(1 << 0)
#define	GPIO_IN_KEY1		(1 << 1)	
#define	GPIO_IN_KEY2		(1 << 2)	
#define	GPIO_IN_KEY3		(1 << 3)	
#define	GPIO_IN_CONFIG_PIN	(1 << 31)	

#define	GPIO_OUT_LED0		(1 << 0)
#define	GPIO_OUT_LED1		(1 << 1)
#define	GPIO_OUT_LED2		(1 << 2)
#define	GPIO_OUT_LED3		(1 << 3)
#define	GPIO_OUT_EEPROM_WP	(1 << 30)

#define	GPIO_PIN0	(1 << 0)
#define	GPIO_PIN1	(1 << 1)
#define	GPIO_PIN2	(1 << 2)
#define	GPIO_PIN3	(1 << 3)
#define	GPIO_PIN4	(1 << 4)
#define	GPIO_PIN5	(1 << 5)
#define	GPIO_PIN6	(1 << 6)
#define	GPIO_PIN7	(1 << 7)
#define	GPIO_PIN8	(1 << 8)
#define	GPIO_PIN9	(1 << 9)
#define	GPIO_PIN10	(1 << 10)
#define	GPIO_PIN11	(1 << 11)
#define	GPIO_PIN12	(1 << 12)
#define	GPIO_PIN13	(1 << 13)
#define	GPIO_PIN14	(1 << 14)
#define	GPIO_PIN15	(1 << 15)

#endif /* GPIO_H_ */


