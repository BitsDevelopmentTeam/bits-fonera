 /***************************************************************************
 *   Copyright (C) 2011 by Terraneo Federico                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

 // Driving a framebuffer-less LCD with SPI + DMA
 // Code designed for use with the Miosix kernel.

#include <cstdio>
#include <cstring>
#include <cctype>
#include <miosix.h>
#include "bitsboard_display.h"
#include "logo.h"
#include "logo-open.h"
#include "logo-closed.h"

using namespace std;
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

void keyboardThread(void*)
{
	{
		FastInterruptDisableLock dLock;
		c1::mode(Mode::INPUT_PULL_UP);
		c2::mode(Mode::INPUT_PULL_UP);
		c3::mode(Mode::INPUT_PULL_UP);
		c4::mode(Mode::INPUT_PULL_UP);
		r1::mode(Mode::OPEN_DRAIN);
		r2::mode(Mode::OPEN_DRAIN);
		r3::mode(Mode::OPEN_DRAIN);
		r4::mode(Mode::OPEN_DRAIN);
	}
	bool oldStatus=false;
	char key='-';
	for(;;)
	{
		bool pressed=false;
		r1::low(); r2::high(); r3::high(); r4::high();
		Thread::sleep(5);
		if(c1::value()==0) { key='1'; pressed=true; }
		if(c2::value()==0) { key='2'; pressed=true; }
		if(c3::value()==0) { key='3'; pressed=true; }
		if(c4::value()==0) { key='A'; pressed=true; }
		r1::high(); r2::low(); r3::high(); r4::high();
		Thread::sleep(5);
		if(c1::value()==0) { key='4'; pressed=true; }
		if(c2::value()==0) { key='5'; pressed=true; }
		if(c3::value()==0) { key='6'; pressed=true; }
		if(c4::value()==0) { key='B'; pressed=true; }
		r1::high(); r2::high(); r3::low(); r4::high();
		Thread::sleep(5);
		if(c1::value()==0) { key='7'; pressed=true; }
		if(c2::value()==0) { key='8'; pressed=true; }
		if(c3::value()==0) { key='9'; pressed=true; }
		if(c4::value()==0) { key='C'; pressed=true; }
		r1::high(); r2::high(); r3::high(); r4::low();
		Thread::sleep(5);
		if(c1::value()==0) { key='*'; pressed=true; }
		if(c2::value()==0) { key='0'; pressed=true; }
		if(c3::value()==0) { key='#'; pressed=true; }
		if(c4::value()==0) { key='D'; pressed=true; }
		if(pressed==false && oldStatus==true)
			iprintf("\n-->%c\n",key);
		oldStatus=pressed;
	}
}

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

void tempThread(void*)
{
	adcInit();
	for(;;)
	{
		Thread::sleep(120*1000);
		const int average=32;
		int x=0;
		for(int i=0;i<average;i++) x+=adcRead(16); //16=Temp sensor
		x/=average;
		//The -5 is sort of a calibration value that may be board specific
		//or just wrong. I got it by comparing the result with the one from
		//a thermometer in the same room. Not precise at all.
		//The formula also assumes the ADC Vmax is 3V
		int t=(x-952)*0.293f-5;
		iprintf("-->T %d\n",t,x);
	}
}

int main()
{
	Thread::create(keyboardThread,2048);
	Thread::create(tempThread,2048);
	initializeDisplay();
	memcpy(framebuffer,logo,4096);

	for(;;)
	{
		char line[256];
		fgets(line,sizeof(line),stdin);
		for(;;)
		{
			int l=strlen(line);
			if(l==0 || (line[l-1]!='r' && line[l-1]!='\n')) break;
			line[l-1]='\0';
		}
		puts(line);
		if(strcmp(line,"open")==0) memcpy(framebuffer,logo_open,4096);
		else if(strcmp(line,"closed")==0) memcpy(framebuffer,logo_closed,4096);
	}
}
