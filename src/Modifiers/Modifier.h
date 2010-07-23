#pragma once

#include "../Commands/Command.h"
#include <fltk/Group.h>
#include <string>
#include <stdlib.h>
#include <string.h>


namespace SnowSim {
	namespace Modifiers {

		class Modifier : public Command, public fltk::Group{
		  public:
			// Constructor(s) & Destructor
			Modifier(int height, const char *name = "Modifier")
				  : fltk::Group(5,200,170,height,name,false)
			{
				std::string lbl_txt("@b;@C#0066B3;");
				lbl_txt.append(name);

				char* new_lbl = static_cast<char*>( malloc( sizeof(char) * lbl_txt.size() + 1 ) );
				if ( new_lbl != NULL ) {
					strcpy(new_lbl, lbl_txt.c_str());
					this->label(new_lbl);
				}
			}

			virtual ~Modifier(void)
			{
			}

			// Command
			virtual bool execute(void) = 0;

			virtual const char * getName() = 0;

			virtual void activate()
			{
				fltk::Group::activate();
			}

			virtual void deactivate()
			{
				fltk::Group::deactivate();
			}

		};
	}
}
