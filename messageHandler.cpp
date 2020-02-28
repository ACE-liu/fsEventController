#include <messageHandler.h>
#include <iostream>
#include "jsoncpp/json.h"
#include "Util/logger.h"


using namespace std;
using namespace toolkit;


messageHandler::messageHandler(const std::string& httpaddr)
    :_httpaddr(httpaddr),isRunning(false)
{
}

messageHandler::~messageHandler()
{
    release();
}


bool messageHandler::startSendingThread()
{
    _thread = std::thread(&messageHandler::runMessageHandleThread, this);
    return true;
}

void messageHandler::runMessageHandleThread()
{
    isRunning = true;
    while(isRunning)
    {
        std::unique_lock<std::mutex> lck(_mtx);
        cv.wait(lck,[this](){ return messageList.size()>0|| !isRunning;});
        if(!isRunning)
            break;
        lck.unlock();
        const MessageData* curData = NULL;
        {
            lock_guard<std::mutex> guard(_mtx);
            if(messageList.size()<1)
                continue;
            curData = messageList.front();
            messageList.pop();
        }

        string sendData = getSendDataByMessage(curData);
        InfoL<<"send data content: "<<sendData;

        delete curData;
        sendHttpData(sendData);
    }
    InfoL<<"out run thread.......";
}

std::string messageHandler::getSendDataByMessage(const MessageData* data)
{
    std::string rtn;
	Json::Value root;
    if(data == NULL)
    {
        std::cout<<"error msg data is NULL..."<<std::endl;
        return rtn;
    }
    root["caller"] = data->callerName;
    root["caller_ip"] = data->callerIp;
    root["callee"] = data->calleeName;
    root["callee_ip"] = data->calleeIp;
    root["calluuid"] = data->callUuid;
    if(data->type == RECORD_STOP)
    {
        const RecordMessageData* rData = static_cast<const RecordMessageData* >(data);
        root["file_name"] = rData->recordFileName;
        root["file_path"] = rData->recordFilePath;
        root["file_size"] = rData->fileSize;
        root["time_len"] = rData->recordMs;
        root["start_time"] = rData->startTime;
    }

    rtn = root.toStyledString();
    return rtn;
}

size_t messageHandler::OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = static_cast<std::string*>(lpVoid);  
    if( NULL == str || NULL == buffer )  
    {  
        return -1;  
    }  
  
    char* pData = (char*)buffer;  
    str->append(pData, size * nmemb);  
    return nmemb; 
}

bool messageHandler::sendHttpData(const std::string & data)
{
    CURL *curl = NULL;  
    CURLcode code;  
    struct curl_slist* headers = NULL;
    string rtn;
    headers = curl_slist_append(headers, "Content-Type:application/json");
    curl_global_init(CURL_GLOBAL_DEFAULT);  
    curl = curl_easy_init();    
    if (curl)
    {
        code = curl_easy_setopt(curl, CURLOPT_URL, _httpaddr.c_str());  
        if (code != CURLE_OK)  
        {  
            curl_easy_cleanup(curl);
            return false;  
        }
        // code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);  
        // if (code != CURLE_OK)  
        // {  
        //     curl_easy_cleanup(curl);
        //     return false;  
        // }
    }
    else
    {
        curl_easy_cleanup(curl);
        return false; 
    }   
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData); 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&rtn); 
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str()); 
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);  
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2); 
    curl_easy_setopt(curl, CURLOPT_POST, 1);
 
    code = curl_easy_perform(curl);  
    if (code != CURLE_OK)  
    {  
        WarnL<<"send http data failed..."<<code<<"\nrtn: ";
    }   
    else
        InfoL<<"send http data success...rtn: "<<rtn;
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return code == CURLE_OK;
}

void messageHandler::release()
{
   if(isRunning)
   {
        isRunning = false;
        {
            std::unique_lock<std::mutex> lck(_mtx);
            cv.notify_one();
        }
        _thread.join();
   }
}

void messageHandler::feedMessage(const MessageData* message)
{
    if(isRunning)
    {
        std::unique_lock<std::mutex> lck(_mtx);
        this->messageList.emplace(message);
        cv.notify_one();
    }
}
