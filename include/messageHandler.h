#ifndef _MESSAGEHANDLER_H_
#define _MESSAGEHANDLER_H_

#include <string>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <curl/curl.h> //deal with http
#include <condition_variable>

enum MessageType
{
   CHANNEL_CREATE = 0,
   CHANNEL_ANSWER,
   CHANNEL_HANGUP,
   RECORD_STOP
};

struct MessageData
{
   MessageType type;
   std::string callerName;
   std::string calleeName;
   std::string callerIp;
   std::string calleeIp;
};


struct RecordMessageData: public MessageData
{
   RecordMessageData(){
       type = RECORD_STOP;
   };
   std::string recordFilePath;
   std::string recordFileName;
   uint32_t recordMs;
   uint32_t fileSize;
   double startTime;
};



class messageHandler
{
public:
    typedef std::shared_ptr<messageHandler> Ptr;
    messageHandler(const std::string& httpaddr);

    bool startSendingThread();

    ~messageHandler();

    void feedMessage(const MessageData* message);



private:
   void runMessageHandleThread();
   void release();
   bool sendHttpData(const std::string & data);
   std::string getSendDataByMessage(const MessageData* data);
   static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid);

   std::queue<const MessageData*> messageList;
   std::string _httpaddr;
   std::thread _thread;
   std::mutex _mtx;
   std::condition_variable cv;
   bool isRunning;
//    int fd;
};



#endif