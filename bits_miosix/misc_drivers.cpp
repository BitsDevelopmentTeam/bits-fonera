
#include "misc_drivers.h"
#include "mxgui/level2/input.h"
#include <cctype>
#include <unistd.h>

#ifdef _MIOSIX
using namespace miosix;

// Keyboard
typedef Gpio<GPIOD_BASE,0> r1;
typedef Gpio<GPIOD_BASE,1> r2;
typedef Gpio<GPIOD_BASE,2> r3;
typedef Gpio<GPIOD_BASE,3> r4;
typedef Gpio<GPIOC_BASE,1> c1;
typedef Gpio<GPIOC_BASE,2> c2;
typedef Gpio<GPIOC_BASE,4> c3;
typedef Gpio<GPIOC_BASE,5> c4;

typedef Gpio<GPIOA_BASE,0> button; //FIXME: temporary, until the keyboard is fixed

/**
 * Initializes ADC1 of the microcontroller
 */
static void adcInit()
{
	FastInterruptDisableLock dLock;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	ADC->CCR |= ADC_CCR_TSVREFE   //Enable temperature sensor
	          | ADC_CCR_ADCPRE_1; //ADC prescaler 84MHz/6=14MHz
	ADC1->CR1=0;
	ADC1->CR2=ADC_CR2_ADON; //The first assignment sets the bit
	ADC1->SQR1=0; //Do only one conversion
	ADC1->SQR2=0;
	ADC1->SQR3=0;
	ADC1->SMPR1=7<<18; //480 clock cycles of sample time for temp sensor
}

/**
 * Read an ADC channel
 * \param input input channel, 0 to 16
 * \return ADC result
 */
static unsigned short adcRead(unsigned char input)
{
	ADC1->SQR3=input;
	ADC1->CR2 |= ADC_CR2_SWSTART;
	while((ADC1->SR & ADC_SR_EOC)==0) ;
	return ADC1->DR;
}
#endif //_MIOSIX

//
// class Temeperature
//

Temperature& Temperature::instance()
{
	static Temperature singleton;
	return singleton;
}

float Temperature::get()
{
	#ifdef _MIOSIX
	const int average=32;
	int x=0;
	for(int i=0;i<average;i++) x+=adcRead(16); //16=Temp sensor
	x/=average;
	//The -5 is sort of a calibration value that may be board specific
	//or just wrong. I got it by comparing the result with the one from
	//a thermometer in the same room. Not precise at all.
	//The formula also assumes the ADC Vmax is 3V
	return (static_cast<float>(x)-952.0f)*0.293f-5.0f;
	#else //_MIOSIX
	return 22.0; //For the simulator always return 22
	#endif //_MIOSIX
}

Temperature::Temperature()
{
	#ifdef _MIOSIX
	adcInit();
	#endif //_MIOSIX
}

//
// class Keyboard
//

Keyboard& Keyboard::instance()
{
	static Keyboard singleton;
	return singleton;
}

char Keyboard::getKey()
{
	#ifdef _MIOSIX
	char result;
	keyPressed.get(result);
	return result;
	#else //_MIOSIX
	for(;;)
	{
		mxgui::Event e=mxgui::InputHandler::instance().getEvent();
		if(e.getEvent()==EventType::KeyUp) return toupper(e.getKey());
	}
	#endif //_MIOSIX
}

char Keyboard::getKeyNonBlocking()
{
	#ifdef _MIOSIX
	FastInterruptDisableLock dLock;
	char result;
	if(keyPressed.IRQget(result)==false) result='-';
	return result;
	#else //_MIOSIX
	mxgui::Event e=mxgui::InputHandler::instance().popEvent();
	return e.getEvent()==EventType::KeyUp ? toupper(e.getKey()) : '-';
	#endif //_MIOSIX
}

void Keyboard::keybThread()
{
	#ifdef _MIOSIX
// 	{
// 		FastInterruptDisableLock dLock;
// 		c1::mode(Mode::INPUT_PULL_UP);
// 		c2::mode(Mode::INPUT_PULL_UP);
// 		c3::mode(Mode::INPUT_PULL_UP);
// 		c4::mode(Mode::INPUT_PULL_UP);
// 		r1::mode(Mode::OPEN_DRAIN);
// 		r2::mode(Mode::OPEN_DRAIN);
// 		r3::mode(Mode::OPEN_DRAIN);
// 		r4::mode(Mode::OPEN_DRAIN);
// 	}
// 	bool oldStatus=false;
// 	char key='-';
// 	for(;;)
// 	{
// 		bool pressed=false;
// 		r1::low(); r2::high(); r3::high(); r4::high();
// 		Thread::sleep(5);
// 		if(c1::value()==0) { key='1'; pressed=true; }
// 		if(c2::value()==0) { key='2'; pressed=true; }
// 		if(c3::value()==0) { key='3'; pressed=true; }
// 		if(c4::value()==0) { key='A'; pressed=true; }
// 		r1::high(); r2::low(); r3::high(); r4::high();
// 		Thread::sleep(5);
// 		if(c1::value()==0) { key='4'; pressed=true; }
// 		if(c2::value()==0) { key='5'; pressed=true; }
// 		if(c3::value()==0) { key='6'; pressed=true; }
// 		if(c4::value()==0) { key='B'; pressed=true; }
// 		r1::high(); r2::high(); r3::low(); r4::high();
// 		Thread::sleep(5);
// 		if(c1::value()==0) { key='7'; pressed=true; }
// 		if(c2::value()==0) { key='8'; pressed=true; }
// 		if(c3::value()==0) { key='9'; pressed=true; }
// 		if(c4::value()==0) { key='C'; pressed=true; }
// 		r1::high(); r2::high(); r3::high(); r4::low();
// 		Thread::sleep(5);
// 		if(c1::value()==0) { key='*'; pressed=true; }
// 		if(c2::value()==0) { key='0'; pressed=true; }
// 		if(c3::value()==0) { key='#'; pressed=true; }
// 		if(c4::value()==0) { key='D'; pressed=true; }
// 		if(pressed==false && oldStatus==true) keyPressed.put(key);
// 		oldStatus=pressed;
// 	}

	//FIXME: temporary, until the keyboard is fixed
	{
		FastInterruptDisableLock dLock;
		button::mode(Mode::INPUT);
	}
	bool oldx=false;
	char value='A'; //Since we have just one button, use it for both A and C
	for(;;)
	{
		bool newx=button::value();
		if(newx && !oldx)
		{
			FastInterruptDisableLock dLock;
			keyPressed.IRQput(value);
			value= (value=='A') ? 'C' : 'A'; 
		}
		oldx=newx;
		Thread::sleep(200);
	}
	#endif //_MIOSIX
}

Keyboard::Keyboard()
{
	#ifdef _MIOSIX
	Thread::create(Keyboard::launcher,STACK_MIN,1,this);
	#endif //_MIOSIX
}
