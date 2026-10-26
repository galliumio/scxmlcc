/*************************************************************************
 ** Host demo for --gallium: four_states chart (four_states.uxf / .scxml).
 ** Logs each fired transition once in transition_actions::run:
 **   targetless: "transition: in <state>"; with target: "<source> -> <target>".
 ** Diagram "/ action" text only where that transition's action runs.
 *************************************************************************/

#include "four_states.h"
#include <iostream>
#include <string>

using namespace std;

typedef sc_four_states Machine;

static void log_fired(const char *source, const char *target)
{
	cout << "  transition: " << source << " -> " << target << endl;
}

static void log_fired_in(const char *state)
{
	cout << "  transition: in " << state << endl;
}

static void log_action(const char *action)
{
	cout << "  action: " << action << endl;
}

static void print_help()
{
	cout << "four_states (a simple active object demo)\n"
	     << "  s  SimpleActStartReq\n"
	     << "  p  SimpleActStopReq\n"
	     << "  d  Done\n"
	     << "  f  Failed\n"
	     << "  q  quit\n";
}

template<> void Machine::state_actions<Machine::state_root>::enter(Machine::data_model &)
{
	cout << "[enter root]" << endl;
}

template<> void Machine::state_actions<Machine::state_root>::exit(Machine::data_model &)
{
	cout << "[exit root]" << endl;
}

template<> void Machine::state_actions<Machine::state_stopped>::enter(Machine::data_model &)
{
	cout << "[enter stopped]" << endl;
}

template<> void Machine::state_actions<Machine::state_stopped>::exit(Machine::data_model &)
{
	cout << "[exit stopped]" << endl;
}

template<> void Machine::state_actions<Machine::state_starting>::enter(Machine::data_model &)
{
	cout << "[enter starting]" << endl;
}

template<> void Machine::state_actions<Machine::state_starting>::exit(Machine::data_model &)
{
	cout << "[exit starting]" << endl;
}

template<> void Machine::state_actions<Machine::state_started>::enter(Machine::data_model &)
{
	cout << "[enter started]" << endl;
}

template<> void Machine::state_actions<Machine::state_started>::exit(Machine::data_model &)
{
	cout << "[exit started]" << endl;
}

template<> void Machine::state_actions<Machine::state_stopping>::enter(Machine::data_model &)
{
	cout << "[enter stopping]" << endl;
}

template<> void Machine::state_actions<Machine::state_stopping>::exit(Machine::data_model &)
{
	cout << "[exit stopping]" << endl;
}

template<> void Machine::state_actions<Machine::state_fault>::enter(Machine::data_model &)
{
	cout << "[enter fault]" << endl;
}

template<> void Machine::state_actions<Machine::state_fault>::exit(Machine::data_model &)
{
	cout << "[exit fault]" << endl;
}

template<> void Machine::transition_actions<&Machine::state::event_SimpleActStartReq, Machine::state_root>::run(Machine::data_model &)
{
	log_fired_in("root");
	log_action("send SimpleActStartCfm(STATE error)");
}

template<> void Machine::transition_actions<&Machine::state::event_SimpleActStopReq, Machine::state_root, Machine::state_stopping>::run(Machine::data_model &)
{
	log_fired("root", "stopping");
}

template<> void Machine::transition_actions<&Machine::state::event_SimpleActStartReq, Machine::state_stopped, Machine::state_starting>::run(Machine::data_model &)
{
	log_fired("stopped", "starting");
}

template<> void Machine::transition_actions<&Machine::state::event_SimpleActStopReq, Machine::state_stopped>::run(Machine::data_model &)
{
	log_fired_in("stopped");
	log_action("send SimpleActStopCfm(SUCCESS)");
}

template<> void Machine::transition_actions<&Machine::state::event_Done, Machine::state_starting, Machine::state_started>::run(Machine::data_model &)
{
	log_fired("starting", "started");
}

template<> void Machine::transition_actions<&Machine::state::event_Failed, Machine::state_starting, Machine::state_stopping>::run(Machine::data_model &)
{
	log_fired("starting", "stopping");
}

template<> void Machine::transition_actions<&Machine::state::event_Done, Machine::state_stopping, Machine::state_stopped>::run(Machine::data_model &)
{
	log_fired("stopping", "stopped");
}

template<> void Machine::transition_actions<&Machine::state::event_Failed, Machine::state_stopping, Machine::state_fault>::run(Machine::data_model &)
{
	log_fired("stopping", "fault");
}

static bool dispatch_key(Machine &sm, char c)
{
	typedef Machine::state state;
	switch (c) {
	case 's':
	case 'S':
		cout << "key: SimpleActStartReq" << endl;
		sm.dispatch_int(&state::event_SimpleActStartReq);
		return true;
	case 'p':
	case 'P':
		cout << "key: SimpleActStopReq" << endl;
		sm.dispatch_int(&state::event_SimpleActStopReq);
		return true;
	case 'd':
	case 'D':
		cout << "key: Done" << endl;
		sm.dispatch_int(&state::event_Done);
		return true;
	case 'f':
	case 'F':
		cout << "key: Failed" << endl;
		sm.dispatch_int(&state::event_Failed);
		return true;
	case 'q':
	case 'Q':
		return false;
	default:
		cout << "unknown key (h for help)" << endl;
		return true;
	}
}

int main()
{
	Machine sm;
	print_help();
	sm.init();

	string line;
	while (cout << "> " && getline(cin, line)) {
		if (line.empty()) continue;
		if (line == "h" || line == "H" || line == "?") {
			print_help();
			continue;
		}
		if (!dispatch_key(sm, line[0])) break;
	}
	return 0;
}
