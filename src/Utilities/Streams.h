// from: http://www.dreamincode.net/code/snippet925.htm

#pragma once

#include <iostream>
#include <sstream>

using std::string;

namespace Utilities {
	namespace Streams {

		class throw_to_begin_cout {};
		class throw_to_end_cout {};

		class mout { // my_out
		private:
		  bool do_cout; //
		public:
		  template<class T>
		  mout& operator << (T &t){
			if (do_cout) std::cout << t;
			std::ostringstream ss;
			string s;
			ss << t;
			s=ss.str();
			appendText(s);
			return *this;
		  }
		  mout& operator << (std::ostream& (*p)(std::ostream&)){
			if (do_cout) std::cout << std::endl;
			string g("\n");
			appendText(g);
			return *this;
		  }
		  mout& operator << (throw_to_begin_cout& t){
			do_cout=true;
			return *this;
		  }
		  mout& operator << (throw_to_end_cout& t){
			do_cout=false;
			return *this;
		  }
		  /*virtual..?*/ void appendText(const string& s){
			// Do whatever you want to do with the text here.
		  }
		};
	}
}

int main(){
  ::Utilities::Streams::mout cout;
  ::Utilities::Streams::throw_to_begin_cout dc;
  ::Utilities::Streams::throw_to_end_cout nc;
  int num=5;
  cout << dc << "Hello my favorite number is " << num << std::endl;
  cout << "Can you see: \n" << nc << "this?" << " Haha you can't see this!" << std::endl;
  system("PAUSE");
}
