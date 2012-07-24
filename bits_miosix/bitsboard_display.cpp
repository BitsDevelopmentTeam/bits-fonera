/***************************************************************************
 *   Copyright (C) 2011, 2012 by Terraneo Federico                         *
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

#include <cstring>
#include <miosix.h>
#include "bitsboard_display.h"

using namespace std;
using namespace miosix;

// Display
typedef Gpio<GPIOB_BASE,13> sck;    //Connected to clock of 4094 and 4040
typedef Gpio<GPIOB_BASE,15> mosi;   //Connected to data of 4094
typedef Gpio<GPIOB_BASE,7> dres;    //Connected to reset of 4040
typedef Gpio<GPIOB_BASE,12> nflm;   //Negated of FLM signal to display
typedef Gpio<GPIOB_BASE,14> nm;     //Negated of M signal to display
typedef Gpio<GPIOB_BASE,8> dispoff; //DISPOFF signal to display

unsigned short *framebuffer;
static volatile char sequence=0; //Used for pulse generation

static void dmaRefill()
{
	DMA1_Stream4->CR=0;
	DMA1_Stream4->PAR=reinterpret_cast<unsigned int>(&SPI2->DR);
	DMA1_Stream4->M0AR=reinterpret_cast<unsigned int>(framebuffer);
	DMA1_Stream4->NDTR=2048;
	DMA1_Stream4->CR=DMA_SxCR_PL_1    | //High priority DMA stream
					 DMA_SxCR_MSIZE_0 | //Read  16bit at a time from RAM
					 DMA_SxCR_PSIZE_0 | //Write 16bit at a time to SPI
					 DMA_SxCR_MINC    | //Increment RAM pointer
					 DMA_SxCR_DIR_0   | //Memory to peripheral direction
					 DMA_SxCR_TEIE    | //Interrupt on error
					 DMA_SxCR_TCIE    | //Interrupt on completion
					 DMA_SxCR_EN;       //Start the DMA
}

void DMA1_Stream4_IRQHandler()
{
	DMA1->HIFCR=DMA_HIFCR_CTCIF4  |
                DMA_HIFCR_CTEIF4  |
                DMA_HIFCR_CDMEIF4 |
                DMA_HIFCR_CFEIF4;
	dmaRefill();

	//The following sequence is to generate a pulse of FLM that can "catch" an
	//LP pulse (www.webalice.it/fede.tft/spi_as_lcd_controller/waveform_lp_flm.png)
	//and since the frequency of LP depends on the SPI clock, these delays need
	//to be changed accordingly if the SPI clock changes.
	sequence=0;
	TIM7->ARR=100; //100us delay
	TIM7->CR1 |= TIM_CR1_CEN;
}

void TIM7_IRQHandler()
{
	TIM7->SR=0;
	switch(sequence)
	{
		case 0:
			sequence=1;
			TIM7->ARR=100; //100us delay
			TIM7->CR1 |= TIM_CR1_CEN;
			nflm::low();
			break;
		case 1:
			sequence=2;
			TIM7->ARR=100; //100us delay
			TIM7->CR1 |= TIM_CR1_CEN;
			if(nm::value()) nm::low(); else nm::high(); //Toggle M
			break;
		case 2:
			nflm::high();
			break;
	}
}

void initializeDisplay()
{
	{
		FastInterruptDisableLock dLock;
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN | RCC_APB1ENR_TIM7EN;
		sck::mode(Mode::ALTERNATE_OD);
		sck::alternateFunction(5);
		mosi::mode(Mode::ALTERNATE_OD);
		mosi::alternateFunction(5);
		dres::mode(Mode::OPEN_DRAIN);
		nflm::mode(Mode::OPEN_DRAIN);
		nm::mode(Mode::OPEN_DRAIN);
		dispoff::mode(Mode::OPEN_DRAIN);
	}

	nflm::high();
	nm::high();
	dispoff::high();

	dres::high();
	delayUs(10);
	dres::low();
	delayUs(10);

	framebuffer=new unsigned short[2048]; //256*128/16=2048
	memset(framebuffer,0,4096);

	TIM7->CR1=TIM_CR1_OPM;
	TIM7->DIER=TIM_DIER_UIE;
	TIM7->PSC=84-1; //84MHz/84=1MHz (1us resolution)
	TIM7->CNT=0;

	dmaRefill();
	NVIC_SetPriority(DMA1_Stream4_IRQn,2);//High priority for DMA
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	NVIC_SetPriority(TIM7_IRQn,3);//High priority for TIM7
	NVIC_EnableIRQ(TIM7_IRQn);

	SPI2->CR2=SPI_CR2_TXDMAEN;
	SPI2->CR1=SPI_CR1_DFF      | //16bit mode
			  SPI_CR1_SSM      | //SS pin not connected to SPI
			  SPI_CR1_SSI      | //Internal SS signal pulled high
			  SPI_CR1_LSBFIRST | //Send LSB first
			  SPI_CR1_MSTR     | //Master mode
			  SPI_CR1_SPE      | //SPI enabled, master mode
			  SPI_CR1_BR_0     |
			  SPI_CR1_BR_1;      //42MHz/16=2.625MHz (/4 by the 4094)=0.66MHz

	Thread::sleep(1000);
	dispoff::low();
}
