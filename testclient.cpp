#include <stdio.h>
#include <stdlib.h>
#include <esl.h>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <memory>
#include <string>
#include <messageHandler.h>
#include <jsoncpp/json.h>
#include "Util/logger.h"
#include "Util/CMD.h"

using namespace std;
using namespace toolkit;

#define CMD_BUFLEN 1024


class CMD_main : public CMD {
public:
    CMD_main() {
        _parser.reset(new OptionParser(nullptr));
        (*_parser) << Option('l',/*该选项简称，如果是\x00则说明无简称*/
                             "level",/*该选项全称,每个选项必须有全称；不得为null或空字符串*/
                             Option::ArgRequired,/*该选项后面必须跟值*/
                             to_string(LTrace).data(),/*该选项默认值*/
                             false,/*该选项是否必须赋值，如果没有默认值且为ArgRequired时用户必须提供该参数否则将抛异常*/
                             "日志等级,LTrace~LError(0~4)",/*该选项说明文字*/
                             nullptr);

        (*_parser) << Option('m',/*该选项简称，如果是\x00则说明无简称*/
                             "max_day",/*该选项全称,每个选项必须有全称；不得为null或空字符串*/
                             Option::ArgRequired,/*该选项后面必须跟值*/
                             "7",/*该选项默认值*/
                             false,/*该选项是否必须赋值，如果没有默认值且为ArgRequired时用户必须提供该参数否则将抛异常*/
                             "日志最多保存天数",/*该选项说明文字*/
                             nullptr);

		string logPath = exeDir() + "log/";

        (*_parser) << Option('d',/*该选项简称，如果是\x00则说明无简称*/
                             "path",/*该选项全称,每个选项必须有全称；不得为null或空字符串*/
                             Option::ArgRequired,/*该选项后面必须跟值*/
                             logPath.c_str(),/*该选项默认值*/
                             false,/*该选项是否必须赋值，如果没有默认值且为ArgRequired时用户必须提供该参数否则将抛异常*/
                             "日志输出路径",/*该选项说明文字*/
                             nullptr);

        (*_parser) << Option('a',/*该选项简称，如果是\x00则说明无简称*/
                             "host",/*该选项全称,每个选项必须有全称；不得为null或空字符串*/
                             Option::ArgRequired,/*该选项后面必须跟值*/
                             "127.0.0.1",/*该选项默认值*/
                             false,/*该选项是否必须赋值，如果没有默认值且为ArgRequired时用户必须提供该参数否则将抛异常*/
                             "ip地址",/*该选项说明文字*/
                             nullptr);

        (*_parser) << Option('p',/*该选项简称，如果是\x00则说明无简称*/
                             "port",/*该选项全称,每个选项必须有全称；不得为null或空字符串*/
                             Option::ArgRequired,/*该选项后面必须跟值*/
                             "8011",/*该选项默认值*/
                             false,/*该选项是否必须赋值，如果没有默认值且为ArgRequired时用户必须提供该参数否则将抛异常*/
                             "端口",/*该选项说明文字*/
                             nullptr);

        (*_parser) << Option('w',/*该选项简称，如果是\x00则说明无简称*/
                             "password",/*该选项全称,每个选项必须有全称；不得为null或空字符串*/
                             Option::ArgRequired,/*该选项后面必须跟值*/
                             "Liu8440",/*该选项默认值*/
                             false,/*该选项是否必须赋值，如果没有默认值且为ArgRequired时用户必须提供该参数否则将抛异常*/
                             "密码",/*该选项说明文字*/
                             nullptr);
        (*_parser) << Option('u',/*该选项简称，如果是\x00则说明无简称*/
                             "httpaddr",/*该选项全称,每个选项必须有全称；不得为null或空字符串*/
                             Option::ArgRequired,/*该选项后面必须跟值*/
                             "http://127.0.0.1:8002/index/hook/on_sip_bill",/*该选项默认值*/
                             false,/*该选项是否必须赋值，如果没有默认值且为ArgRequired时用户必须提供该参数否则将抛异常*/
                             "上传url地址",/*该选项说明文字*/
                             nullptr);
    }

    virtual ~CMD_main() {}
    virtual const char *description() const {
        return "主程序命令参数";
    }
};

// static char *EVENT_NAMES[] = {
// 	"CUSTOM",
// 	"CLONE",
// 	"CHANNEL_CREATE",
// 	"CHANNEL_DESTROY",
// 	"CHANNEL_STATE",
// 	"CHANNEL_CALLSTATE",
// 	"CHANNEL_ANSWER",
// 	"CHANNEL_HANGUP",
// 	"CHANNEL_HANGUP_COMPLETE",
// 	"CHANNEL_EXECUTE",
// 	"CHANNEL_EXECUTE_COMPLETE",
// 	"CHANNEL_HOLD",
// 	"CHANNEL_UNHOLD",
// 	"CHANNEL_BRIDGE",
// 	"CHANNEL_UNBRIDGE",
// 	"CHANNEL_PROGRESS",
// 	"CHANNEL_PROGRESS_MEDIA",
// 	"CHANNEL_OUTGOING",
// 	"CHANNEL_PARK",
// 	"CHANNEL_UNPARK",
// 	"CHANNEL_APPLICATION",
// 	"CHANNEL_ORIGINATE",
// 	"CHANNEL_UUID",
// 	"API",
// 	"LOG",
// 	"INBOUND_CHAN",
// 	"OUTBOUND_CHAN",
// 	"STARTUP",
// 	"SHUTDOWN",
// 	"PUBLISH",
// 	"UNPUBLISH",
// 	"TALK",
// 	"NOTALK",
// 	"SESSION_CRASH",
// 	"MODULE_LOAD",
// 	"MODULE_UNLOAD",
// 	"DTMF",
// 	"MESSAGE",
// 	"PRESENCE_IN",
// 	"NOTIFY_IN",
// 	"PRESENCE_OUT",
// 	"PRESENCE_PROBE",
// 	"MESSAGE_WAITING",
// 	"MESSAGE_QUERY",
// 	"ROSTER",
// 	"CODEC",
// 	"BACKGROUND_JOB",
// 	"DETECTED_SPEECH",
// 	"DETECTED_TONE",
// 	"PRIVATE_COMMAND",
// 	"HEARTBEAT",
// 	"TRAP",
// 	"ADD_SCHEDULE",
// 	"DEL_SCHEDULE",
// 	"EXE_SCHEDULE",
// 	"RE_SCHEDULE",
// 	"RELOADXML",
// 	"NOTIFY",
// 	"PHONE_FEATURE",
// 	"PHONE_FEATURE_SUBSCRIBE",
// 	"SEND_MESSAGE",
// 	"RECV_MESSAGE",
// 	"REQUEST_PARAMS",
// 	"CHANNEL_DATA",
// 	"GENERAL",
// 	"COMMAND",
// 	"SESSION_HEARTBEAT",
// 	"CLIENT_DISCONNECTED",
// 	"SERVER_DISCONNECTED",
// 	"SEND_INFO",
// 	"RECV_INFO",
// 	"RECV_RTCP_MESSAGE",
// 	"CALL_SECURE",
// 	"NAT",
// 	"RECORD_START",
// 	"RECORD_STOP",
// 	"PLAYBACK_START",
// 	"PLAYBACK_STOP",
// 	"CALL_UPDATE",
// 	"FAILURE",
// 	"SOCKET_DATA",
// 	"MEDIA_BUG_START",
// 	"MEDIA_BUG_STOP",
// 	"CONFERENCE_DATA_QUERY",
// 	"CONFERENCE_DATA",
// 	"CALL_SETUP_REQ",
// 	"CALL_SETUP_RESULT",
// 	"CALL_DETAIL",
// 	"DEVICE_STATE",
// 	"TEXT",
// 	"ALL"
// };

static int running = 1;
static int thread_running = 0, thread_up = 0, check_up = 0;
static esl_mutex_t * pmutex =NULL;
static char switchname[256] = "";

static char *filter_uuid;
static char *logfilter;
messageHandler::Ptr msgHandler = nullptr;


static void plain_events(esl_handle_t *handle)
{
	char cmd_str[1024] = {0};	
	// snprintf(cmd_str, sizeof(cmd_str), "event json CHANNEL_CREATE\n\n");
	// esl_send_recv(handle, cmd_str);
	// snprintf(cmd_str, sizeof(cmd_str), "event json CHANNEL_ANSWER\n\n");
	// esl_send_recv(handle, cmd_str);
	// snprintf(cmd_str, sizeof(cmd_str), "event json CHANNEL_HANGUP\n\n");
	// esl_send_recv(handle, cmd_str);
	// snprintf(cmd_str, sizeof(cmd_str), "event json CHANNEL_HANGUP_COMPLETE\n\n");
	// esl_send_recv(handle, cmd_str);
	// snprintf(cmd_str, sizeof(cmd_str), "event json CHANNEL_EXECUTE\n\n");
	// esl_send_recv(handle, cmd_str);
	// snprintf(cmd_str, sizeof(cmd_str), "event json CHANNEL_EXECUTE_COMPLETE\n\n");
	// esl_send_recv(handle, cmd_str);
	snprintf(cmd_str, sizeof(cmd_str), "event json RECORD_START\n\n");
	esl_send_recv(handle, cmd_str);
	snprintf(cmd_str, sizeof(cmd_str), "event json RECORD_STOP\n\n");
	esl_send_recv(handle, cmd_str);
	// snprintf(cmd_str, sizeof(cmd_str), "event json CHANNEL_BRIDGE\n\n");
	// esl_send_recv(handle, cmd_str);
	// snprintf(cmd_str, sizeof(cmd_str), "event json CHANNEL_UNBRIDGE\n\n");
	// esl_send_recv(handle, cmd_str);

}



static void handle_receive_event(const esl_handle_t *handle)
{
    string revBody = handle->last_event->body;
	// std::cout <<"revbody:\n"<<revBody<<std::endl;
	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(revBody, root)){
		WarnL <<"parse failed ............";
		return;
	}
    std::string eventName = root["Event-Name"].asString();

	string callerName = root["Caller-Username"].asString();
	string calleeName = root["Caller-Destination-Number"].asString();
	string callerIp = root["Caller-Network-Addr"].asString();
	string calleeIp = root["Other-Leg-Network-Addr"].asString();
	string callUuid = root["variable_call_uuid"].asString();
    if(eventName == "CHANNEL_CREATE" )
	{
        if(root["Caller-Direction"].asString() == "outbound")
		    return;
		MessageData* mData = new MessageData();
		mData->type = CHANNEL_CREATE;
		mData->callUuid = callUuid;
		mData->callerName = callerName;
		mData->callerIp = callerIp;
		mData->calleeIp = calleeIp;
		mData->calleeName = calleeName;
		msgHandler->feedMessage(mData);
	}
	else if(eventName == "CHANNEL_BRIDGE"||eventName == "CHANNEL_UNBRIDGE")
	{
        std::cout<<"get "<<eventName<<"  event........."<<std::endl;
		MessageData* mData = new MessageData();
		if(eventName == "CHANNEL_BRIDGE")
		    mData->type = CHANNEL_ANSWER;
		else
		    mData->type = CHANNEL_HANGUP;
		mData->callUuid = callUuid;
		mData->callerName = callerName;
		mData->callerIp = callerIp;
		mData->calleeIp = calleeIp;
		mData->calleeName = calleeName;
		msgHandler->feedMessage(mData);
	}
	else if(eventName =="RECORD_STOP")
	{
		InfoL<<"get "<<eventName<<"  event........."<<std::endl;
		string filePath = root["Record-File-Path"].asString();
		string fileName = filePath;
		string::size_type index;
		if((index = filePath.rfind('/')) != string::npos)
		{
			fileName = filePath.substr(index+1);
		}
		else if((index = filePath.rfind('\\')) != string::npos)
		{
			fileName = filePath.substr(index+1);
		}
		string recordMsStr = root["variable_record_ms"].asString();
		uint32_t recordMs = stoi(recordMsStr);
		if(recordMs == 0)
		{
			WarnL<<"record file is empty!";
			return;
		}

		string recordFileSize = root["variable_record_file_size"].asString();
		uint32_t fileSize = stoi(recordFileSize);

		string recordStartTime = root["Caller-Channel-Bridged-Time"].asString();
		double startTime = stod(recordStartTime);
		RecordMessageData* mData = new RecordMessageData();
		mData->callUuid = callUuid;
		mData->callerName = callerName;
		mData->callerIp = callerIp;
		mData->calleeIp = calleeIp;
		mData->calleeName = calleeName;
		mData->recordFileName = fileName;
		mData->recordFilePath = filePath;
		mData->fileSize = fileSize;
		mData->recordMs = recordMs;
		mData->startTime = startTime;
		msgHandler->feedMessage(mData);
	}
	else
	{
		InfoL<<"unknown get "<<eventName<<"  event.........";
	}
}


static void *msg_thread_run(esl_thread_t *me, void *obj)
{
	esl_handle_t *handle = (esl_handle_t *) obj;
	thread_running = 1;
	esl_mutex_lock(pmutex);
	thread_up = 1;
	esl_mutex_unlock(pmutex);
	while(thread_running && handle->connected) {
		int aok = 1;
		esl_status_t status;
		esl_mutex_lock(pmutex);
		status = esl_recv_event_timed(handle, 10, 1, NULL);
		esl_mutex_unlock(pmutex);
		
		if (status == ESL_BREAK) {
			usleep(1000);
		} else if (status == ESL_FAIL) {
			esl_log(ESL_LOG_WARNING, "Disconnected.\n");
			running = -1; thread_running = 0;
		} else if (status == ESL_SUCCESS) {

			// const char *type = esl_event_get_header(handle->last_event, "content-type");


			if (handle->last_event) {
				handle_receive_event(handle);
		}
		else
		{
			InfoL<<"status ok for event... "<<std::endl;
		}
		
	}
	}

	InfoL<<"exit msg_thread_run...";

	esl_mutex_lock(pmutex);  
	thread_up = 0;
	esl_mutex_unlock(pmutex);  
	thread_running = 0;
	esl_log(ESL_LOG_DEBUG, "Thread Done\n");
	return NULL;
}

int main(int argc, char *argv[])
{
	esl_handle_t handle = {{0}};
	int count = 0;
	char cmd_str[2048] = {0};
	bool connected = false;
	int loops =2;
	const char *line = NULL;


	CMD_main cmd_main;
	try {
		cmd_main.operator()(argc, argv);
	} catch (std::exception &ex) {
		cout << ex.what() << endl;
		return -1;
	}
    
	LogLevel logLevel = (LogLevel) cmd_main["level"].as<int>();
	string host = cmd_main["host"];
	int port = cmd_main["port"].as<int>();
	string pwd = cmd_main["password"];
	string httpaddr = cmd_main["httpaddr"];
	string logPath = cmd_main["path"];

	esl_mutex_create(&pmutex);


	
	Logger::Instance().add(std::shared_ptr<ConsoleChannel>(new ConsoleChannel()));

	auto fileChannel = std::make_shared<FileChannel>("FileChannel", logPath, logLevel);
	//日志最多保存天数
	fileChannel->setMaxDay(cmd_main["max_day"]);
	Logger::Instance().add(fileChannel);

	Logger::Instance().setWriter(std::shared_ptr<LogWriter>(new AsyncLogWriter()));
    
	InfoL<<host<<" \t"<<port <<" \t"<<pwd<<"\t"<<httpaddr<<"\t"<<logPath<<std::endl;
	connected = false;
	while (--loops > 0) {
		memset(&handle, 0, sizeof(handle));
		if (esl_connect(&handle, host.c_str(), port, NULL, pwd.c_str())!= ESL_SUCCESS) {
			esl_global_set_default_logger(7);
			esl_log(ESL_LOG_ERROR, "Error Connecting [%s]\n", handle.err);

			sleep(1);
			esl_log(ESL_LOG_INFO, "Retrying\n");
		} else {
			connected = true;
			break;
		}
	}
	if(!connected)
	{
		ErrorL<<"连接freeswitch失败，退出...........";
		return -1;
	}
	
	msgHandler.reset(new messageHandler(httpaddr));
	msgHandler->startSendingThread();

	esl_send_recv(&handle, "api status\n\n");

	if (handle.last_sr_event && handle.last_sr_event->body) {
		printf("%s\n", handle.last_sr_event->body);
	} else {
		// this is unlikely to happen with api or bgapi (which is hardcoded above) but prefix but may be true for other commands
		printf("%s\n", handle.last_sr_reply);
	}

	snprintf(cmd_str, sizeof(cmd_str), "api switchname\n\n");
	esl_send_recv(&handle, cmd_str);
	if (handle.last_sr_event && handle.last_sr_event->body) {
		esl_set_string(switchname, handle.last_sr_event->body);
		InfoL<<"get switch name: "<<switchname<<std::endl;
	} else {
		esl_set_string(switchname, "default");
	}



	

	if (esl_thread_create_detached(msg_thread_run, &handle) != ESL_SUCCESS) {
		printf("Error starting thread!\n");
		esl_disconnect(&handle);
		return 0;
	}
    
	plain_events(&handle);

	//设置退出信号处理函数
	static semaphore sem;
	signal(SIGINT, [](int) {
		InfoL << "SIGINT:exit";
		signal(SIGINT, SIG_IGN);// 设置退出信号
		sem.post();
	});// 设置退出信号
	sem.wait();
	thread_running = 0;

	do {
		esl_mutex_lock(pmutex);
		check_up = thread_up;
		esl_mutex_unlock(pmutex);
		InfoL<<check_up;
		usleep(10000);
	} while (check_up > 0);
	esl_disconnect(&handle);
	esl_mutex_destroy(&pmutex);
	return 0;
}
