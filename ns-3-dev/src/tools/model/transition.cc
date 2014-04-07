#include "transition.h"

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
		if (messageName.compare("BaseMessage") == 0) return 0;
		if (messageName.compare("Fin") == 0) return 1;
		if (messageName.compare("Syn") == 0) return 2;
		if (messageName.compare("FinSyn") == 0) return 3;
		if (messageName.compare("Rst") == 0) return 4;
		if (messageName.compare("FinRst") == 0) return 5;
		if (messageName.compare("SynRst") == 0) return 6;
		if (messageName.compare("FinSynRst") == 0) return 7;
		if (messageName.compare("Psh") == 0) return 8;
		if (messageName.compare("FinPsh") == 0) return 9;
		if (messageName.compare("SynPsh") == 0) return 10;
		if (messageName.compare("FinSynPsh") == 0) return 11;
		if (messageName.compare("RstPsh") == 0) return 12;
		if (messageName.compare("FinRstPsh") == 0) return 13;
		if (messageName.compare("SynRstPsh") == 0) return 14;
		if (messageName.compare("FinSynRstPsh") == 0) return 15;
		if (messageName.compare("Ack") == 0) return 16;
		if (messageName.compare("FinAck") == 0) return 17;
		if (messageName.compare("SynAck") == 0) return 18;
		if (messageName.compare("FinSynAck") == 0) return 19;
		if (messageName.compare("RstAck") == 0) return 20;
		if (messageName.compare("FinRstAck") == 0) return 21;
		if (messageName.compare("SynRstAck") == 0) return 22;
		if (messageName.compare("FinSynRstAck") == 0) return 23;
		if (messageName.compare("PshAck") == 0) return 24;
		if (messageName.compare("FinPshAck") == 0) return 25;
		if (messageName.compare("SynPshAck") == 0) return 26;
		if (messageName.compare("FinSynPshAck") == 0) return 27;
		if (messageName.compare("RstPshAck") == 0) return 28;
		if (messageName.compare("FinRstPshAck") == 0) return 29;
		if (messageName.compare("SynRstPshAck") == 0) return 30;
		if (messageName.compare("FinSynRstPshAck") == 0) return 31;
		return -1;
	}
}