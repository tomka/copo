#pragma once

namespace SnowSim {
	namespace Modifiers {

		class Command{
		  public:
		   virtual bool execute(void) =0;
		   virtual ~Command(void){};
		};
	}
}