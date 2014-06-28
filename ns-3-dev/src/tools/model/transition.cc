#include "transition.h"
#include <ns3/message.h>

namespace ns3 {
	int Transition::GetMsgTypeFromAction(string action) {
		if (action.size() > 2 && action.compare(0, 2, "M_") == 0) {
			string name = action.substr(2, action.size() - 2);
			return GetMsgType(name);
		}
		return -1;
	}

	int Transition::GetTransitionType(int rcvd, int send) {
		if ( rcvd ==  -1  && send ==  -1 ) {
			return 1 ;
}
		if ( rcvd ==  -1  && send ==  1 ) {
			return 2 ;
}
		if ( rcvd ==  -1  && send ==  17 ) {
			return 3 ;
}
		if ( rcvd ==  16  && send ==  -1 ) {
			return 4 ;
}
		if ( rcvd ==  1  && send ==  16 ) {
			return 5 ;
}
		if ( rcvd ==  17  && send ==  16 ) {
			return 6 ;
}
		if ( rcvd ==  16  && send ==  16 ) {
			return 7 ;
}
		if ( rcvd ==  -1  && send ==  2 ) {
			return 8 ;
}
		if ( rcvd ==  -1  && send ==  2 ) {
			return 9 ;
}
		if ( rcvd ==  2  && send ==  18 ) {
			return 10 ;
}
		if ( rcvd ==  4  && send ==  -1 ) {
			return 11 ;
}
		if ( rcvd ==  -1  && send ==  4 ) {
			return 12 ;
}
		if ( rcvd ==  18  && send ==  16 ) {
			return 13 ;
}
		if ( rcvd ==  -1  && send ==  -1 ) {
			return 14 ;
}
		if ( rcvd ==  -1  && send ==  -1 ) {
			return 15 ;
}
		return -1;
	}

	int Transition::GetMsgType(string messageName) {
            if (messageName.compare(0, 2, "M_") == 0) {
                messageName = messageName.substr(2);
            }
	    return Message::StrToType(messageName.c_str());
	}
}
