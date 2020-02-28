#include <stdio.h>
#include <stdlib.h>
#include <esl.h>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <sys/select.h>
#include <memory>
#include <messageHandler.h>
#include <jsoncpp/json.h>

using namespace std;

#define CMD_BUFLEN 1024

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
static char prompt_str[1024] = "";

static char *filter_uuid;
static char *logfilter;
messageHandler::Ptr msgHandler = nullptr;


static const char *usage_str =
	"Usage: %s [-H <host>] [-P <port>] [-p <secret>] [-d <level>] [-x command] [-t <timeout_ms>] [profile]\n\n"
	"  -H, --host=hostname             Host to connect\n"
	"  -P, --port=port                 Port to connect (1 - 65535)\n"
	"  -p, --password=password         Password\n";

static int usage(char *name){
	printf(usage_str, name);
	return 1;
}

static const char *cli_usage =
	"Command                    \tDescription\n"
	"-----------------------------------------------\n"
	"/help                      \tHelp\n"
	"/exit, /quit, /bye, ...    \tExit the program.\n"
	"/event, /noevents, /nixevent\tEvent commands.\n"
	"/log, /nolog               \tLog commands.\n"
	"/uuid                      \tFilter logs for a single call uuid\n"
	"/filter                    \tFilter commands.\n"
	"/logfilter                 \tFilter Log for a single string.\n"
	"/debug [0-7]               \tSet debug level.\n"
	"\n";


static int output_printf(const char *fmt, ...)
{
	va_list ap;
	int r;
	va_start(ap, fmt);
	r = vprintf(fmt, ap);
	va_end(ap);
	return r;
}

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

static int process_command(esl_handle_t *handle, const char *cmd)
{
	int r = 0;
	
	while (*cmd == ' ') cmd++;

	esl_mutex_lock(pmutex);

	if ((*cmd == '/' && cmd++) || !strncasecmp(cmd, "...", 3)) {
		if (!strcasecmp(cmd, "help")) {
			output_printf("%s", cli_usage);
			goto end;
		}
		if (!strcasecmp(cmd, "exit") ||
			!strcasecmp(cmd, "quit") ||
			!strcasecmp(cmd, "...") ||
			!strcasecmp(cmd, "bye")
			) {
			esl_log(ESL_LOG_INFO, "Goodbye!\nSee you at ClueCon http://www.cluecon.com/\n");
			r = -1; goto end;
		} else if (!strncasecmp(cmd, "logfilter", 9)) {
			cmd += 9;
			while (cmd && *cmd && *cmd == ' ') {
				cmd++;
			}
			if (!esl_strlen_zero(cmd)) {
				esl_safe_free(logfilter);
				logfilter = strdup(cmd);
			} else {
				esl_safe_free(logfilter);
			}
			output_printf("Logfilter %s\n", logfilter ? "enabled" : "disabled");
		} else if (!strncasecmp(cmd, "uuid", 4)) {
			cmd += 4;
			while (cmd && *cmd && *cmd == ' ') {
				cmd++;
			}
			if (!esl_strlen_zero(cmd)) {
				filter_uuid = strdup(cmd);
			} else {
				esl_safe_free(filter_uuid);
			}
			output_printf("UUID filtering %s\n", filter_uuid ? "enabled" : "disabled");
		} else if (!strncasecmp(cmd, "event", 5) ||
				   !strncasecmp(cmd, "noevents", 8) ||
				   !strncasecmp(cmd, "nixevent", 8) ||
				   !strncasecmp(cmd, "log", 3) ||
				   !strncasecmp(cmd, "nolog", 5) ||
				   !strncasecmp(cmd, "filter", 6)
				   ) {
			esl_send_recv(handle, cmd);
			printf("%s\n", handle->last_sr_reply);
		} else if (!strncasecmp(cmd, "debug", 5)) {
			int tmp_debug = atoi(cmd+6);
			if (tmp_debug > -1 && tmp_debug < 8) {
				esl_global_set_default_logger(tmp_debug);
				output_printf("fs_cli debug level set to %d\n", tmp_debug);
			} else {
				output_printf("fs_cli debug level must be 0 - 7\n");
			}
		} else {
			output_printf("Unknown command [%s]\n", cmd);
		}
	} else {
		char cmd_str[1024] = "";
		const char *err = NULL;

		if (!strncasecmp(cmd, "console loglevel ", 17)) { 
			snprintf(cmd_str, sizeof(cmd_str), "log %s", cmd + 17);
			esl_send_recv(handle, cmd_str);
			printf("%s\n", handle->last_sr_reply);
		}

		snprintf(cmd_str, sizeof(cmd_str), "api %s\nconsole_execute: true\n\n", cmd);
		if (esl_send_recv(handle, cmd_str)) {
			output_printf("Socket interrupted, bye!\n");
			r = -1; goto end;
		}
		if (handle->last_sr_event) {
			if (handle->last_sr_event->body) {
				output_printf("%s\n", handle->last_sr_event->body);
			} else if ((err = esl_event_get_header(handle->last_sr_event, "reply-text")) && !strncasecmp(err, "-err", 4)) {
				output_printf("Error: %s!\n", err + 4);
			}
		}
	}
	
 end:

	esl_mutex_unlock(pmutex);
		
	return r;
}

static char command_buf[CMD_BUFLEN+1] = "";
static const char *basic_gets(int *cnt)
{
	int x = 0;
	printf("%s", prompt_str);
	// if (global_profile->batch_mode) fflush(stdout);
	memset(&command_buf, 0, sizeof(command_buf));
	for (x = 0; x < (sizeof(command_buf) - 1); x++) {
		int c = getchar();
		if (c < 0) {
			if (fgets(command_buf, sizeof(command_buf) - 1, stdin) != command_buf) {
				break;
			}
			command_buf[strlen(command_buf)-1] = '\0'; /* remove endline */
			break;
		}
		command_buf[x] = (char) c;
		if (command_buf[x] == '\n') {
			command_buf[x] = '\0';
			break;
		}
	}
	*cnt = x;
	return command_buf;
}

static int stdout_writable(void)
{
#ifndef WIN32
	fd_set set;
	int fd = fileno(stdout);
	struct timeval to;
	memset(&to, 0, sizeof(to));
	FD_ZERO(&set);
	FD_SET(fd, &set);
	to.tv_sec = 0;
	to.tv_usec = 100000;
	if (select(fd + 1, NULL, &set, NULL, &to) > 0) {
		return FD_ISSET(fd, &set);
	} else {
		return 0;
	}
#else
	return 1;
#endif
}

static void handle_receive_event(const esl_handle_t *handle)
{
    string revBody = handle->last_event->body;
	// std::cout <<"revbody:\n"<<revBody<<std::endl;
	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(revBody, root)){
		std::cout <<"parse failed ............===============liuliu";
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
		std::cout<<"get "<<eventName<<"  event........."<<std::endl;
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
			std::cout<<"record file is empty!"<<std::endl;
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
		std::cout<<"unknown get "<<eventName<<"  event........."<<std::endl;
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
			// std::cout<<"status ok for event... "<<type<<std::endl;

			aok = stdout_writable();

			if (handle->last_event) {
				handle_receive_event(handle);
		}
		else
		{
			std::cout<<"status ok for event... =======else"<<std::endl;
		}
		
	}
	}

	std::cout<<"exit msg_thread_run..."<<std::endl;

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
    int argv_error = 0;
	char temp_host[128];
	int argv_host = 0;
	char temp_user[256];
	char temp_pass[128];
	int temp_port = 8021;
    int argv_port = 0;
	int opt;
	int count = 0;
	char cmd_str[2048] = {0};
	bool connected = false;
	int loops =2;
	const char *line = NULL;
	char httpaddr[256] ={0};
	bool hasHttpaddr =false;

	static struct option options[] = {
		{"host", 1, 0, 'H'},
		{"port", 1, 0, 'P'},
		{"password", 1, 0, 'p'},
		{"httpaddr", 1, 0, 'U'},
		{0, 0, 0, 0}
	};
    
	std::cout<<"at test..."<<std::endl;
	esl_mutex_create(&pmutex);
	for(;;) {
	int option_index = 0;
	opt = getopt_long(argc, argv, "H:P:p:U:", options, &option_index);
	if (opt == -1) break;
	switch (opt) {
		case 'H':
			esl_set_string(temp_host, optarg);
			argv_host = 1;
			break;
		case 'P':
			temp_port= atoi(optarg);
			if (temp_port > 0 && temp_port < 65536) {
				argv_port = 1;
			} else {
				printf("ERROR: Port must be in range 1 - 65535\n");
				argv_error = 1;
			}
			break;
		case 'p':
			esl_set_string(temp_pass, optarg);
			// argv_pass = 1;
			break;
		case 'U':
			esl_set_string(httpaddr, optarg);
			hasHttpaddr = true;
			break;
		}
	}
	if (argv_error) {
		printf("\n");
		return usage(argv[0]);
	}
	if(!hasHttpaddr)
	{
		std::cout<<"need httpaddr param!"<<std::endl;
		return -1;
	}
    std::cout <<temp_host<<" \t"<<temp_port <<" \t"<<temp_pass<<"\t"<<httpaddr<<std::endl;
	if(!argv_host)
		strcpy(temp_host, "localhost");

	connected = false;
	while (--loops > 0) {
		memset(&handle, 0, sizeof(handle));
		if (esl_connect(&handle, temp_host, temp_port, NULL, temp_pass)!= ESL_SUCCESS) {
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
         return -1;
	
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
		std::cout<<"get switch name: "<<switchname<<std::endl;
	} else {
		esl_set_string(switchname, "default");
	}

	snprintf(prompt_str, sizeof(prompt_str), "freeswitch@%s> ", switchname);


	

	if (esl_thread_create_detached(msg_thread_run, &handle) != ESL_SUCCESS) {
		printf("Error starting thread!\n");
		esl_disconnect(&handle);
		return 0;
	}
    
	plain_events(&handle);

	while (running > 0) {
		int r;

		line = basic_gets(&count);
		if (count > 1 && !esl_strlen_zero(line)) {
			char *p, *cmd = strdup(line);
			assert(cmd);
			if ((p = strrchr(cmd, '\r')) || (p = strrchr(cmd, '\n'))) {
				*p = '\0';
			}

			if ((r = process_command(&handle, cmd))) {
				running = r;
			}
			free(cmd);
		}
		usleep(1000);
	}
    std::cout<<"at quit=================="<<std::endl;
	esl_disconnect(&handle);
	thread_running = 0;

	do {
		esl_mutex_lock(pmutex);
		check_up = thread_up;
		esl_mutex_unlock(pmutex);
		usleep(10000);
	} while (check_up > 0);
	
	esl_mutex_destroy(&pmutex);
	return 0;
}
