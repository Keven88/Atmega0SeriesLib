/*
* GccApplication1.cpp
*
* Created: 26.01.2019 16:32:01
* Author : Keven
*/
#include "../inc/MCUSelect.hpp"
//#include "../inc/test.hpp"
#ifndef TEST

#include <stddef.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define MEGA4808

#ifdef MEGA4808
//fix unavailable ports
#undef PORTB
#undef PORTE
///////////////////////
#include "../inc/mega4808/Atmega4808.hpp"
#include "../inc/hw_abstractions/SPI.hpp"
#include "../inc/hw_abstractions/Port.hpp"
#include "../inc/hw_abstractions/Eventsystem.hpp"
#include "../inc/hw_abstractions/TWI.hpp"

#elif defined(MEGA4809)
#include "../inc/mega4809/Atmega4809.hpp"

#endif


using namespace AVR::port;

using PortA = Port<AVR::port::A>;
using PortC = Port<AVR::port::C>;

using spiRessource = AVR::rc::Instance<
AVR::spi::SPI_Comp, // using ressource SPI
AVR::rc::Number<0>, //using instance '0'
AVR::portmux::PortMux<0>>; // using portmux 0 alternative

using twiRessource = AVR::rc::Instance<
AVR::twi::TWI_Comp, // using ressource SPI
AVR::rc::Number<0>, //using instance '0'
AVR::portmux::PortMux<0>>; // using portmux 0 alternative


using RC = AVR::rc::RessourceController<spiRessource,twiRessource>; //acquire ressource
using res = RC::getRessource_t<spiRessource>; //get the ressource
using twires = RC::getRessource_t<twiRessource>;
using spi = AVR::spi::SPIMaster<AVR::spi::notBlocking,res>; // put spi ressource in
using twi = AVR::twi::TWIMaster<AVR::twi::notBlocking,twires>;

using led1 = Pin<PortA, 2>;
using led2 = Pin<PortA, 2>;
using ch0 = AVR::eventsystem::Channel<0>;




static constexpr auto funcref = []() {return spi::receive(); };


enum class error : mem_width {
	Busy = 0, 
	notBusy = 42	
};

static constexpr auto lam = [](){twi::startTransaction<0x0f,AVR::twi::direction::output>(); twi::singleTransfer(42); twi::stopTransaction(); return static_cast<mem_width>(error::notBusy);};

int main() {
	twi::init();

		while(true){
			auto err = twi::doIfAnySet<lam>(twi::status_bits::Busstate_idle , twi::status_bits::Busstate_owner);

			if( err != static_cast<mem_width>(error::notBusy)){
				/*
				if bus is busy...
				*/
			}

			_delay_ms(200);
		
	}
	
}

#endif