#include "ns3/uinteger.h"
#include <string.h>
#include <arpa/inet.h>
#ifndef MESSAGE_H
#define MESSAGE_H
typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} BaseMessage;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} Fin;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} Syn;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinSyn;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} Rst;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinRst;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} SynRst;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinSynRst;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} Psh;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinPsh;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} SynPsh;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinSynPsh;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} RstPsh;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinRstPsh;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} SynRstPsh;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinSynRstPsh;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} Ack;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} SynAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinSynAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} RstAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinRstAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} SynRstAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinSynRstAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} PshAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinPshAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} SynPshAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinSynPshAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} RstPshAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinRstPshAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} SynRstPshAck;

typedef struct {
	uint16_t src;
	uint16_t dest;
	uint32_t seq;
	uint32_t ack;
	uint16_t res : 4;
	uint16_t size : 4;
	uint16_t type : 5;
	uint16_t urg : 1;
	uint16_t ece : 1;
	uint16_t cwr : 1;
	uint16_t window;
	uint16_t chk;
	uint16_t urgptr;
} FinSynRstPshAck;

enum MessageType {BASEMESSAGE=0, FIN=1, SYN=2, FINSYN=3, RST=4, FINRST=5, SYNRST=6, FINSYNRST=7, PSH=8, FINPSH=9, SYNPSH=10, FINSYNPSH=11, RSTPSH=12, FINRSTPSH=13, SYNRSTPSH=14, FINSYNRSTPSH=15, ACK=16, FINACK=17, SYNACK=18, FINSYNACK=19, RSTACK=20, FINRSTACK=21, SYNRSTACK=22, FINSYNRSTACK=23, PSHACK=24, FINPSHACK=25, SYNPSHACK=26, FINSYNPSHACK=27, RSTPSHACK=28, FINRSTPSHACK=29, SYNRSTPSHACK=30, FINSYNRSTPSHACK=31};

class Message {
	public:

	uint8_t *msg;
	int type;
	int size;

	Message *encMsg;

	Message(uint8_t* msg);

	int FindMsgType();
	int FindMsgSize(); 
	uint8_t* EncMsgOffset();
	static int StrToType(const char* str);
	void ChangeValue(int field, char* value);
	void ChangeBaseMessage(int field, char* value);
	void ChangeFin(int field, char* value);
	void ChangeSyn(int field, char* value);
	void ChangeFinSyn(int field, char* value);
	void ChangeRst(int field, char* value);
	void ChangeFinRst(int field, char* value);
	void ChangeSynRst(int field, char* value);
	void ChangeFinSynRst(int field, char* value);
	void ChangePsh(int field, char* value);
	void ChangeFinPsh(int field, char* value);
	void ChangeSynPsh(int field, char* value);
	void ChangeFinSynPsh(int field, char* value);
	void ChangeRstPsh(int field, char* value);
	void ChangeFinRstPsh(int field, char* value);
	void ChangeSynRstPsh(int field, char* value);
	void ChangeFinSynRstPsh(int field, char* value);
	void ChangeAck(int field, char* value);
	void ChangeFinAck(int field, char* value);
	void ChangeSynAck(int field, char* value);
	void ChangeFinSynAck(int field, char* value);
	void ChangeRstAck(int field, char* value);
	void ChangeFinRstAck(int field, char* value);
	void ChangeSynRstAck(int field, char* value);
	void ChangeFinSynRstAck(int field, char* value);
	void ChangePshAck(int field, char* value);
	void ChangeFinPshAck(int field, char* value);
	void ChangeSynPshAck(int field, char* value);
	void ChangeFinSynPshAck(int field, char* value);
	void ChangeRstPshAck(int field, char* value);
	void ChangeFinRstPshAck(int field, char* value);
	void ChangeSynRstPshAck(int field, char* value);
	void ChangeFinSynRstPshAck(int field, char* value);
};
#endif

#define MSG 32
#define FIELD 12

