#pragma once
#include "Modifier.h"
#include "../polygonal_mesh.h"
#include <Model/World.h>
#include <boost/function.hpp>
#include <boost/signal.hpp>
#include <Utilities/Data.h>
#include <Utilities/Keys.h>
#include <fltk/ValueInput.h>
//#include <fltk/CheckButton.h>
#include <fltk/Choice.h>
#include <fltk/InvisibleBox.h>
#include <Utilities/Timer.h>
#include <GUI/AORWidget.h>

namespace SnowSim {
	namespace Modifiers {

		class ModifierSnowStability : public Modifier
		{
		private:

			typedef SnowSim::Model::ILaunchSite<PrecisionType>*						PLaunchSite;
			typedef std::set<PLaunchSite, Utilities::Data::f_launchsite_sort>		LS_Set;

			// An enumeration declaring the different termination criteria types
			enum TerminationCriteria
			{
				Time,
				Iterations
			};

			/// The world
			SnowSim::Model::World* m_world;

			/// selected index in terminantion criteria list
			int m_selected_index;

			/// Timer for measuring duration of stabilization
			SnowSim::Utilities::Timer m_timer;

			/// terminantion criteria choise
			fltk::Choice m_choise_termination;
			/// input: maximum of iterations until termination
			fltk::ValueInput m_input_iterations;
			/// input: max. time until terminations (sec)
			fltk::ValueInput m_input_maxTime;
			/// input: max. distance to a launch site of a dropped
			/// off avalance hit from one edge group to another
			fltk::ValueInput m_input_maxDistToHitLS;
			/// checkbutton: Make interactive info available
			fltk::CheckButton m_cbStepwise;

			/// AOR label
			fltk::InvisibleBox m_label_aor;
			/// aor choise
			fltk::Choice m_choise_aor;
			/// aor: constant 1
			fltk::ValueInput m_aor_constant1;
			/// aor: seperating angle
			fltk::ValueInput m_aor_sep_angle;
			/// aor: constant 2
			fltk::ValueInput m_aor_constant2;
			/// aor: linear: m
			fltk::ValueInput m_aor_linear_m;
			/// aor: linear: n
			fltk::ValueInput m_aor_linear_n;
			/// aor: stable
			fltk::ValueInput m_aor_stable_angle;

			/// aor: widget
			::GUI::AORWidget<PrecisionType>* m_aor_widget;

			// initial aor values for all possibilities
			PrecisionType m_aor_val_c1, m_aor_val_c2, m_aor_val_sep_a,
				m_aor_val_m, m_aor_val_n, m_aor_val_stable_angle;

			// the selected aor function index
			int m_selected_aor_index;

			/// Names for the termination criteria
			static const std::vector<const char*> m_terminationCriteria;

			// Names for the aor functions
			static const std::vector<const char*> m_aor_functions;

			/// True, if the user can do stepwise stabilization
			/// by pressing a defined key
			bool m_stepwise;

			/// key press handling
			void OnKeyPressed(SNOW_KEY);

			/// Key for forwarding stepwise stabilization
			const SNOW_KEY m_keyForward;

			/// Key for fast forwarding stepwise stabilization
			const SNOW_KEY m_keyFastForward;

			/// static callback for cbEGvisible_cb
			static inline void callback_settings(fltk::Widget *w,void *data)  {
				((ModifierSnowStability*)data)->callback_settings();
			}
			void callback_settings();

			/// the key event signal/slots connection
			boost::signals::connection m_keyevent_connection;

			/// A function pointer to the actual evaluation
			/// function variants in this class.
			boost::function< bool (int iteration, LS_Set* u1) > m_func_continue;

			/// An evaluator method which returns
			/// false if too many iterations have been made.
			bool evalIteratrions(int iteration, LS_Set* u1);

			/// An evaluator method which returns
			/// false if too many iterations have been made.
			bool evalTime(int iteration, LS_Set* u1);

			/// The maximum amount of iterations to make
			int m_maxIterations;

			/// The maximum amount of iterations to make
			double m_maxTimeInSec;

			/// The maximum amount of iterations to make [sec]
			double m_averageTimePerIteration;

			/// max. distance to a launch site of a dropped
			/// off avalance hit from one edge group to another [cm]
			PrecisionType m_maxDistToLS;

			/// The actual worker method
			bool Stabilize();

			/// Handler for change events from the snow model
			void OnSnowModelChanged(SnowModel* sm);
			/// Handler connection
			boost::signals::connection m_connection_OnSnowModelChanged;

		private:
			// gui update method
			void update_ui();

			// update functions objects
			void update_bindings();

		public:
			ModifierSnowStability(SnowSim::Model::World* world);
			~ModifierSnowStability(void);

			bool execute(void);
			const char * getName();

			void activate();
			void deactivate();

			static const char * getModifierName() {
				return "Stabilize Snow";
			}
		};
	}
}
