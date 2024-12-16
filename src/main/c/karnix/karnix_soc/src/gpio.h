#ifndef GPIO_H_
#define GPIO_H_


typedef struct
{
  volatile uint32_t INPUT;
  volatile uint32_t OUTPUT;
  volatile uint32_t OUTPUT_ENABLE;
} Gpio_Reg;


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

#endif /* GPIO_H_ */


