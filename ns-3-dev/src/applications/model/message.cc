#include "message.h"

Message::Message(uint8_t* m) {
	//std::cout << "Entering Message" << std::endl;
	msg = m;
	type = FindMsgType();
	size = FindMsgSize();

	encMsg = NULL;
	uint8_t *ptr = EncMsgOffset();
	if (ptr != NULL) {
		encMsg = new Message(ptr);
	}
	//std::cout << "Exiting Message" << std::endl;
}

int Message::FindMsgType() {
	return ((BaseMessage*)msg)->type;
}

int Message::FindMsgSize() {
	return ((BaseMessage*)msg)->size + 0;	
}

void Message::ChangeBaseMessage(int field, char* value) {
	//std::cout << "Entering ChangeBaseMessage" << std::endl;
	BaseMessage *ptr = (BaseMessage*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	//std::cout << "Exiting ChangeBaseMessage" << std::endl;
}

void Message::ChangeRequest(int field, char* value) {
	//std::cout << "Entering ChangeRequest" << std::endl;
	Request *ptr = (Request*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	//std::cout << "Exiting ChangeRequest" << std::endl;
}

void Message::ChangeReply(int field, char* value) {
	//std::cout << "Entering ChangeReply" << std::endl;
	Reply *ptr = (Reply*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 3) {
		int64_t *cur = &(ptr->v);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 4) {
		uint64_t *cur = &(ptr->rid);
		if (value[0] == '=') {
			*cur = (uint64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (uint64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (uint64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (uint64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (uint64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (uint64_t)rand();
		}
	}
	if (field == 5) {
		char *cur = ptr->digest;
		strncpy(cur, value, 16);
	}
	if (field == 6) {
		int *cur = &(ptr->replica);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 7) {
		int *cur = &(ptr->reply_size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	//std::cout << "Exiting ChangeReply" << std::endl;
}

void Message::ChangePrePrepare(int field, char* value) {
	//std::cout << "Entering ChangePrePrepare" << std::endl;
	PrePrepare *ptr = (PrePrepare*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 3) {
		int64_t *cur = &(ptr->view);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 4) {
		int64_t *cur = &(ptr->seqno);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 5) {
		char *cur = ptr->digest;
		strncpy(cur, value, 16);
	}
	if (field == 6) {
		int *cur = &(ptr->rset_size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 7) {
		short *cur = &(ptr->n_big_reqs);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 8) {
		short *cur = &(ptr->non_det_size);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 9) {
		char *cur = (char*)(msg+sizeof(short)+sizeof(short)+sizeof(int)+sizeof(int64_t)+sizeof(int64_t)+sizeof(char)*16+sizeof(int)+sizeof(short)+sizeof(short));
		strncpy(cur, value, ptr->rset_size);
	}
	//std::cout << "Exiting ChangePrePrepare" << std::endl;
}

void Message::ChangePrepare(int field, char* value) {
	//std::cout << "Entering ChangePrepare" << std::endl;
	Prepare *ptr = (Prepare*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 3) {
		int64_t *cur = &(ptr->view);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 4) {
		int64_t *cur = &(ptr->seqno);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 5) {
		char *cur = ptr->digest;
		strncpy(cur, value, 16);
	}
	if (field == 6) {
		int *cur = &(ptr->id);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 7) {
		int *cur = &(ptr->padding);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	//std::cout << "Exiting ChangePrepare" << std::endl;
}

void Message::ChangeCommit(int field, char* value) {
	//std::cout << "Entering ChangeCommit" << std::endl;
	Commit *ptr = (Commit*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 3) {
		int64_t *cur = &(ptr->view);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 4) {
		int64_t *cur = &(ptr->seqno);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 5) {
		int *cur = &(ptr->id);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 6) {
		int *cur = &(ptr->padding);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	//std::cout << "Exiting ChangeCommit" << std::endl;
}

void Message::ChangeCheckpoint(int field, char* value) {
	//std::cout << "Entering ChangeCheckpoint" << std::endl;
	Checkpoint *ptr = (Checkpoint*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 3) {
		int64_t *cur = &(ptr->seqno);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 4) {
		char *cur = ptr->digest;
		strncpy(cur, value, 16);
	}
	if (field == 5) {
		int *cur = &(ptr->id);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 6) {
		int *cur = &(ptr->padding);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	//std::cout << "Exiting ChangeCheckpoint" << std::endl;
}

void Message::ChangeStatus(int field, char* value) {
	//std::cout << "Entering ChangeStatus" << std::endl;
	Status *ptr = (Status*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 3) {
		int64_t *cur = &(ptr->v);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 4) {
		int64_t *cur = &(ptr->ls);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 5) {
		int64_t *cur = &(ptr->le);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 6) {
		int *cur = &(ptr->id);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 7) {
		short *cur = &(ptr->sz);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 8) {
		short *cur = &(ptr->brsz);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 9) {
		int *cur = &(ptr->vcs_size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 10) {
		char *cur = (char*)(msg+sizeof(short)+sizeof(short)+sizeof(int)+sizeof(int64_t)+sizeof(int64_t)+sizeof(int64_t)+sizeof(int)+sizeof(short)+sizeof(short)+sizeof(int));
		strncpy(cur, value, ptr->vcs_size);
	}
	//std::cout << "Exiting ChangeStatus" << std::endl;
}

void Message::ChangeViewChange(int field, char* value) {
	//std::cout << "Entering ChangeViewChange" << std::endl;
	ViewChange *ptr = (ViewChange*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 3) {
		int64_t *cur = &(ptr->v);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 4) {
		int64_t *cur = &(ptr->ls);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 5) {
		char *cur = ptr->ckpts;
		strncpy(cur, value, 48);
	}
	if (field == 6) {
		int *cur = &(ptr->id);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 7) {
		short *cur = &(ptr->n_ckpts);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 8) {
		short *cur = &(ptr->n_reqs);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 9) {
		int *cur = &(ptr->prepared_size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 10) {
		unsigned int *cur = (unsigned int*)(msg+sizeof(short)+sizeof(short)+sizeof(int)+sizeof(int64_t)+sizeof(int64_t)+sizeof(char)*48+sizeof(int)+sizeof(short)+sizeof(short)+sizeof(int));
		for (int i = 0; i < ptr->prepared_size; i++) {
			if (value[0] == '=') {
				*cur = (unsigned int)atoi(value+1);
			} else if (value[0] == '+') {
				*cur += (unsigned int)atoi(value+1);
			} else if (value[0] == '-') {
				*cur -= (unsigned int)atoi(value+1);
			} else if (value[0] == '*') {
				*cur *= (unsigned int)atoi(value+1);
			} else if (value[0] == '/') {
				*cur /= (unsigned int)atoi(value+1);
			} else if (value[0] == 'r') {
				*cur = (unsigned int)rand();
			}
			cur++;
		}
	}
	if (field == 11) {
		char *cur = (char*)(msg+sizeof(short)+sizeof(short)+sizeof(int)+sizeof(int64_t)+sizeof(int64_t)+sizeof(char)*48+sizeof(int)+sizeof(short)+sizeof(short)+sizeof(int)+sizeof(unsigned int)*ptr->prepared_size);
		strncpy(cur, value, 16);
	}
	//std::cout << "Exiting ChangeViewChange" << std::endl;
}

void Message::ChangeNewView(int field, char* value) {
	//std::cout << "Entering ChangeNewView" << std::endl;
	NewView *ptr = (NewView*)msg;

	if (field == 0) {
		short *cur = &(ptr->type);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 1) {
		short *cur = &(ptr->extra);
		if (value[0] == '=') {
			*cur = (short)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (short)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (short)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (short)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (short)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (short)rand();
		}
	}
	if (field == 2) {
		int *cur = &(ptr->size);
		if (value[0] == '=') {
			*cur = (int)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int)rand();
		}
	}
	if (field == 3) {
		int64_t *cur = &(ptr->v);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 4) {
		int64_t *cur = &(ptr->min);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	if (field == 5) {
		int64_t *cur = &(ptr->max);
		if (value[0] == '=') {
			*cur = (int64_t)atoi(value+1);
		} else if (value[0] == '+') {
			*cur += (int64_t)atoi(value+1);
		} else if (value[0] == '-') {
			*cur -= (int64_t)atoi(value+1);
		} else if (value[0] == '*') {
			*cur *= (int64_t)atoi(value+1);
		} else if (value[0] == '/') {
			*cur /= (int64_t)atoi(value+1);
		} else if (value[0] == 'r') {
			*cur = (int64_t)rand();
		}
	}
	//std::cout << "Exiting ChangeNewView" << std::endl;
}

void Message::ChangeValue(int field, char* value) {
	//std::cout << "Entering ChangeValue " << type << " " << field << " " << value << std::endl;
	switch (type) {
		case BASEMESSAGE:
			ChangeBaseMessage(field, value);
			break;
		case REQUEST:
			ChangeRequest(field, value);
			break;
		case REPLY:
			ChangeReply(field, value);
			break;
		case PREPREPARE:
			ChangePrePrepare(field, value);
			break;
		case PREPARE:
			ChangePrepare(field, value);
			break;
		case COMMIT:
			ChangeCommit(field, value);
			break;
		case CHECKPOINT:
			ChangeCheckpoint(field, value);
			break;
		case STATUS:
			ChangeStatus(field, value);
			break;
		case VIEWCHANGE:
			ChangeViewChange(field, value);
			break;
		case NEWVIEW:
			ChangeNewView(field, value);
			break;
	}
	//std::cout << "Exiting ChangeValue" << std::endl;
}

int Message::StrToType(const char *str) {
	//std::cout << "Entering StrToType" << std::endl;
	if (strcmp(str, "BaseMessage") == 0) {
		return BASEMESSAGE;
	}
	if (strcmp(str, "Request") == 0) {
		return REQUEST;
	}
	if (strcmp(str, "Reply") == 0) {
		return REPLY;
	}
	if (strcmp(str, "PrePrepare") == 0) {
		return PREPREPARE;
	}
	if (strcmp(str, "Prepare") == 0) {
		return PREPARE;
	}
	if (strcmp(str, "Commit") == 0) {
		return COMMIT;
	}
	if (strcmp(str, "Checkpoint") == 0) {
		return CHECKPOINT;
	}
	if (strcmp(str, "Status") == 0) {
		return STATUS;
	}
	if (strcmp(str, "ViewChange") == 0) {
		return VIEWCHANGE;
	}
	if (strcmp(str, "NewView") == 0) {
		return NEWVIEW;
	}
	//std::cout << "Exiting StrToType" << std::endl;
	return -1;
}

uint8_t* Message::EncMsgOffset() {
	//std::cout << "Entering EncMsgOffset" << std::endl;
	if (type == BASEMESSAGE)
			return NULL;
	if (type == REQUEST)
			return NULL;
	if (type == REPLY)
			return NULL;
	if (type == PREPREPARE)
			return NULL;
	if (type == PREPARE)
			return NULL;
	if (type == COMMIT)
			return NULL;
	if (type == CHECKPOINT)
			return NULL;
	if (type == STATUS)
			return NULL;
	if (type == VIEWCHANGE)
			return NULL;
	if (type == NEWVIEW)
			return NULL;
	//std::cout << "Exiting EncMsgOffset" << std::endl;
	return NULL;
}

