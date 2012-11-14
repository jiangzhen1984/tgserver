
#ifndef TG_EVENT_H
#define TG_EVENT_H



struct TGEvent{
	int token;
	int type;
	void * data;
	struct TGEvent * pNext;
};


// push new event to queue
//	0 client quit
//	1 client request
// Note: the event doesn't copy pData
//		only point to this. 
void pushEventToQueue(const int type,void * pData);


// returned head of event queue
// event object must be free after use
struct TGEvent * head();


// return current queue size
int getQueueSize();


// cancel all events and free memeory
// the attribute of TGEvent data as char * free 
void cancelAllEvents();

// free struct Event memory
// Attribute pData of struct TGEvent as char * free 
void freeTGEvent(struct TGEvent * pEv);

#endif

