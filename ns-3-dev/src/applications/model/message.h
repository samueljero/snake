#include "ns3/uinteger.h"
#include <string.h>
#ifndef MESSAGE_H
#define MESSAGE_H
typedef struct {
	short type;
	short extra;
	int size;
} BaseMessage;

typedef struct {
	short type;
	short extra;
	int size;
} Request;

typedef struct {
	short type;
	short extra;
	int size;
	int64_t v;
	uint64_t rid;
	char digest[16];
	int replica;
	int reply_size;
} Reply;

typedef struct {
	short type;
	short extra;
	int size;
	int64_t view;
	int64_t seqno;
	char digest[16];
	int rset_size;
	short n_big_reqs;
	short non_det_size;
} PrePrepare;

typedef struct {
	short type;
	short extra;
	int size;
	int64_t view;
	int64_t seqno;
	char digest[16];
	int id;
	int padding;
} Prepare;

typedef struct {
	short type;
	short extra;
	int size;
	int64_t view;
	int64_t seqno;
	int id;
	int padding;
} Commit;

typedef struct {
	short type;
	short extra;
	int size;
	int64_t seqno;
	char digest[16];
	int id;
	int padding;
} Checkpoint;

struct PP_info {
	int64_t v;
	int n;
	int proof;
	int64_t breqs;
};

typedef struct {
	short type;
	short extra;
	int size;
	int64_t v;
	int64_t ls;
	int64_t le;
	int id;
	short sz;
	short brsz;
	int vcs_size;
} Status;

typedef struct {
	short type;
	short extra;
	int size;
	int64_t v;
	int64_t ls;
	char ckpts[48];
	int id;
	short n_ckpts;
	short n_reqs;
	int prepared_size;
} ViewChange;

typedef struct {
	short type;
	short extra;
	int size;
	int64_t v;
	int64_t min;
	int64_t max;
} NewView;

enum MessageType {BASEMESSAGE=0, REQUEST=1, REPLY=2, PREPREPARE=3, PREPARE=4, COMMIT=5, CHECKPOINT=6, STATUS=7, VIEWCHANGE=8, NEWVIEW=9};

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
	void ChangeRequest(int field, char* value);
	void ChangeReply(int field, char* value);
	void ChangePrePrepare(int field, char* value);
	void ChangePrepare(int field, char* value);
	void ChangeCommit(int field, char* value);
	void ChangeCheckpoint(int field, char* value);
	void ChangeStatus(int field, char* value);
	void ChangeViewChange(int field, char* value);
	void ChangeNewView(int field, char* value);
};
#endif

#define MSG 10
#define FIELD 11

