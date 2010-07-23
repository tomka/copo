// events.cpp : Defines the entry point for the console application.
//

#include "events.h"
#include <string>

using std::string;
using namespace zx;

class Subject
{
	int m_val;
	string m_name;

public:
	event<int> ValueChanged;
	event<int, const char*> ValueAndNameChanged;  // no need for having event1<A> and event2<A, B>

	void SetValue(int x) 
	{
		m_val = x;
		ValueChanged(x);
	}

	void SetValueAndName(int x, const string& name) 
	{
		m_val = x;
		m_name = name;
		ValueAndNameChanged(x, name.c_str());
	}

};

class Observer
{
public:
	void OnValueChanged(int i){ printf(" - non-static handler for SetValue called, new value = %ld\n", i); }
	static void OnValueChangedStatic(int i) { printf(" - static handler for SetValue called, new value = %ld\n", i); }

	void OnValueAndNameChanged(int i, const char* name)
	{
		printf(" - non-static handler for SetValueAndName called, new value = %ld, new name = '%s'\n", 
			i, name); 
	}
};

void GlobalOnValueChanged(int i)
{
	printf(" - global handler for SetValue called, new value = %ld\n", i);
}
void GlobalOnValueAndNameChanged(int i, const char* name)
{
	printf(" - global handler for SetValueAndName called, new value = %ld, new name = '%s'\n", 
		i, name); 
}


/* EXAMPLE

int _tmain(int argc, _TCHAR* argv[])
{
	Subject subject;
	Observer observer;

	subject.ValueChanged += Observer::OnValueChangedStatic; // static member method as handler
	subject.ValueChanged += GlobalOnValueChanged;           // global method as handler
	subject.ValueChanged += event_handler(&observer, &Observer::OnValueChanged); // non-static member
	subject.ValueChanged += Observer::OnValueChangedStatic; // duplicate will be ignored

	printf("ValueChanged event test:\n");
	printf("%ld listeners are attached to ValueChanged event\n\n", subject.ValueChanged.getSubscriberCount());

	subject.SetValue(3);

	subject.ValueChanged -= GlobalOnValueChanged;
	subject.ValueChanged -= Observer::OnValueChangedStatic;
	subject.ValueChanged -= event_handler(&observer, &Observer::OnValueChanged);

	printf("\n%ld listeners are attached to ValueChanged event\n\n", subject.ValueChanged.getSubscriberCount());


	printf("ValueAndNameChanged event test:\n");
	subject.ValueAndNameChanged += GlobalOnValueAndNameChanged;
	subject.ValueAndNameChanged += event_handler(&observer, &Observer::OnValueAndNameChanged);

	printf("%ld listeners are attached to ValueAndNameChanged event\n\n", subject.ValueAndNameChanged.getSubscriberCount());

	subject.SetValueAndName(5, "x y");

	subject.ValueAndNameChanged -= GlobalOnValueAndNameChanged;
	subject.ValueAndNameChanged -= event_handler(&observer, &Observer::OnValueAndNameChanged);

	printf("\n%ld listeners are attached to ValueAndNameChanged event\n\n", subject.ValueAndNameChanged.getSubscriberCount());

	return 0;
}

*/