/* This is the test control routine the NetX RTSP module.  All tests are dispatched from this routine.  */

#include "tx_api.h"
#include "nx_api.h"
#include "nx_rtsp_server.h"
#include "nx_rtp_sender.h"
#include "app_rtsp_over_rtp.h"
#include "venc_app.h"

/* Define the stack size for demo tasks. */
#define DEMO_TEST_STACK_SIZE              2048
#define DEMO_RTSP_SERVER_STACK_SIZE       2048
static UCHAR AppServerThread_stack[DEMO_TEST_STACK_SIZE];
static UCHAR rtsp_server_stack[DEMO_RTSP_SERVER_STACK_SIZE];
void sample_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, VOID *dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time));

/* Define rtp cname which is typically used in rtcp sender report. */
#define DEMO_RTP_CNAME                    "AzureRTOS@microsoft.com"

/* Define multicast corresponding parameters. !Note: these parameters in sdp shall be changed with the same values. */
#ifdef DEMO_MULTICAST_ENABLED

#ifndef DEMO_MULTICAST_IP_ADDRESS
#define DEMO_MULTICAST_IP_ADDRESS         IP_ADDRESS(224, 1, 0, 55)
#endif /* DEMO_MULTICAST_IP_ADDRESS */

#ifndef DEMO_MULTICAST_RTP_PORT
#define DEMO_MULTICAST_RTP_PORT           6002
#endif /* DEMO_MULTICAST_RTP_PORT */

#ifndef DEMO_MULTICAST_RTCP_PORT
#define DEMO_MULTICAST_RTCP_PORT          6003
#endif /* DEMO_MULTICAST_RTCP_PORT */

#endif /* DEMO_MULTICAST_ENABLED */

/* The RTSP server listening port.  */
#ifndef DEMO_RTSP_SERVER_PORT
#define DEMO_RTSP_SERVER_PORT             554
#endif /* DEMO_RTSP_SERVER_PORT */

/* The RTSP server thread priority.  */
#ifndef DEMO_RTSP_SERVER_PRIORITY
#define DEMO_RTSP_SERVER_PRIORITY         3
#endif /* DEMO_RTSP_SERVER_PRIORITY */

/* File name shown in rtsp SETUP request */
#ifndef DEMO_RTSP_VIDEO_FILE_NAME
#define DEMO_RTSP_VIDEO_FILE_NAME         "trackID=0"
#endif /* DEMO_RTSP_VIDEO_FILE_NAME */

#ifndef DEMO_RTSP_AUDIO_FILE_NAME
#define DEMO_RTSP_AUDIO_FILE_NAME         "trackID=1"
#endif /* DEMO_RTSP_AUDIO_FILE_NAME */

/* Define RTP payload type for media. !Note: payload type in sdp shall be changed with the same values */
#ifndef DEMO_RTP_PAYLOAD_TYPE_VIDEO
#ifndef DEMO_H264_ENABLED
#define DEMO_RTP_PAYLOAD_TYPE_VIDEO       26
#else
#define DEMO_RTP_PAYLOAD_TYPE_VIDEO       96 /* Use dynamic type range from 96 to 127 */
#endif /* DEMO_H264_ENABLED */
#endif /* DEMO_RTP_PAYLOAD_TYPE_VIDEO */

#ifndef DEMO_RTP_PAYLOAD_TYPE_AUDIO
#ifdef DEMO_AAC_ENABLED
#define DEMO_RTP_PAYLOAD_TYPE_AUDIO       97
#else
#define DEMO_RTP_PAYLOAD_TYPE_AUDIO       11
#endif /* DEMO_AAC_ENABLED */
#endif /* DEMO_RTP_PAYLOAD_TYPE_AUDIO */

/* Define video & audio play fps. !Note: this macro shall be the same as the real FPS to guarantee video playing normally */
#ifndef DEMO_VIDEO_FRAME_PER_SECOND
#define DEMO_VIDEO_FRAME_PER_SECOND       30
#endif /* DEMO_VIDEO_FRAME_PER_SECOND */

#ifndef DEMO_AUDIO_FRAME_PER_SECOND
#define DEMO_AUDIO_FRAME_PER_SECOND       43 /* Reference the comment of DEMO_RTP_AUDIO_SAMPLING_PERIOD
                                                to understand how this macro is calculated and defined. */
#endif /* DEMO_AUDIO_FRAME_PER_SECOND */

#ifndef DEMO_AUDIO_SAMPLE_SIZE
#define DEMO_AUDIO_SAMPLE_SIZE            16 /* Indicate the size in bits for each audio sample. */
#endif /* DEMO_AUDIO_SAMPLE_SIZE */

#ifndef DEMO_AUDIO_CHANNEL_NUM
#define DEMO_AUDIO_CHANNEL_NUM            1
#endif /* DEMO_AUDIO_CHANNEL_NUM */

/* The sampling periods define rtp timestamp increase rate for media. */
#define DEMO_RTP_VIDEO_SAMPLING_PERIOD    (90000 / DEMO_VIDEO_FRAME_PER_SECOND)
#define DEMO_RTP_AUDIO_SAMPLING_PERIOD    (44100 / DEMO_AUDIO_FRAME_PER_SECOND)  /* Assume the default AAC sampling rate is 44100.
                                                                                    Normally, a single ACC frame contains 1024 samples;
                                                                                    So, there are 44100 / 1024 = 43 frames per second.
                                                                                    Therefore, sampling period is 44100 / 43 = 1025. */

/* Define frame play internal for media */
#define DEMO_RTP_VIDEO_PLAY_INTERVAL      (1000 / DEMO_VIDEO_FRAME_PER_SECOND)
#define DEMO_RTP_AUDIO_PLAY_INTERVAL      (1000 / DEMO_AUDIO_FRAME_PER_SECOND)

#ifndef DEMO_PLAY_TIMER_INTERVAL
#define DEMO_PLAY_TIMER_INTERVAL          10 /* Per milliseconds */
#endif /* DEMO_PLAY_TIMER_INTERVAL */

/* Declare the prototypes for the test entry points. */
TX_THREAD            AppServerThread;
NX_RTSP_SERVER       rtsp_0;
NX_RTP_SENDER        rtp_0;

/* Declare events to use in threads. */
TX_EVENT_FLAGS_GROUP demo_test_events;

/* Declare the timer to trigger events (e.g., playing video/audio). */
TX_TIMER             demo_timer;

/* Record the tick runs by global timer. */
ULONG                tick_num = 0;

/* Declare the sample structure to support multiple clients interaction. */
typedef struct SAMPLE_CLIENT_STRUCT
{
    NX_RTSP_CLIENT   *rtsp_client_ptr;

    /* RTP sessions */
    NX_RTP_SESSION   rtp_session_video;
    NX_RTP_SESSION   rtp_session_audio;

    /* Count the number of clients setup in the specific rtp session */
    USHORT           rtp_session_video_client_count;
    USHORT           rtp_session_audio_client_count;

    /* RTP timestamp and NTP timestamp */
    ULONG            rtp_session_video_timestamp;
    ULONG            rtp_session_audio_timestamp;


} SAMPLE_CLIENT;

#ifdef DEMO_MULTICAST_ENABLED
SAMPLE_CLIENT        sample_client_multicast;
#else
SAMPLE_CLIENT        sample_client[NX_RTSP_SERVER_MAX_CLIENTS];
#endif  /* DEMO_MULTICAST_ENABLED */

/* Define an error counter.  */
ULONG                error_counter;


/* Internal functions prototype. */
static VOID app_server_entry(ULONG thread_input);

static UINT rtsp_describe_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length);
static UINT rtsp_setup_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length, NX_RTSP_TRANSPORT *transport_ptr);
static UINT rtsp_play_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length, UCHAR *range_ptr, UINT range_length);
static UINT rtsp_teardown_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length);
static UINT rtsp_pause_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length, UCHAR *range_ptr, UINT range_length);
static UINT rtsp_set_parameter_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length, UCHAR *parameter_ptr, ULONG parameter_length);
static UINT rtsp_disconnect_callback(NX_RTSP_CLIENT *rtsp_client_ptr);

static UINT test_rtcp_receiver_report_callback(NX_RTP_SESSION *session, NX_RTCP_RECEIVER_REPORT *report);
static UINT test_rtcp_sdes_callback(NX_RTCP_SDES_INFO *sdes_info);



/* Define what the initial system looks like.  */
void sample_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, VOID *dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time))
{
UINT   status;


#ifdef DEMO_MULTICAST_ENABLED
    /* Enable IGMP.  */
    status = nx_igmp_enable(ip_ptr);

    /* Join multicast group. */
    status += nx_igmp_multicast_join(ip_ptr, DEMO_MULTICAST_IP_ADDRESS);
    if (status)
        error_counter++;
#endif  /* DEMO_MULTICAST_ENABLED */

    status = nx_ip_fragment_enable(ip_ptr);
    if (status)
        error_counter++;

    /* Create RTP sender */
    nx_rtp_sender_create(&rtp_0, ip_ptr, pool_ptr, DEMO_RTP_CNAME, sizeof(DEMO_RTP_CNAME) - 1);
    nx_rtp_sender_rtcp_receiver_report_callback_set(&rtp_0, test_rtcp_receiver_report_callback);
    nx_rtp_sender_rtcp_sdes_callback_set(&rtp_0, test_rtcp_sdes_callback);

    /* Create RTSP server. */
    nx_rtsp_server_create(&rtsp_0, "RTSP Server", sizeof("RTSP Server") - 1, ip_ptr, pool_ptr,
                          rtsp_server_stack, DEMO_RTSP_SERVER_STACK_SIZE, DEMO_RTSP_SERVER_PRIORITY, DEMO_RTSP_SERVER_PORT, rtsp_disconnect_callback);

    /* Set callback functions. */
    nx_rtsp_server_describe_callback_set(&rtsp_0, rtsp_describe_callback);
    nx_rtsp_server_setup_callback_set(&rtsp_0, rtsp_setup_callback);
    nx_rtsp_server_play_callback_set(&rtsp_0, rtsp_play_callback);
    nx_rtsp_server_teardown_callback_set(&rtsp_0, rtsp_teardown_callback);
    nx_rtsp_server_pause_callback_set(&rtsp_0, rtsp_pause_callback);
    nx_rtsp_server_set_parameter_callback_set(&rtsp_0, rtsp_set_parameter_callback);

    /* Start RTSP server. */
    nx_rtsp_server_start(&rtsp_0);

    printf("RTSP server started!\r\n");


    /* Create event for the play thread */
    status = tx_event_flags_create(&demo_test_events, "Demo events");
    if (status)
        error_counter++;

    /* Create a helper thread for the server. */
    tx_thread_create(&AppServerThread, "Test thread", app_server_entry, 0,
                     AppServerThread_stack, DEMO_TEST_STACK_SIZE,
                     4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);
}

static VOID demo_timer_entry(ULONG address)
{
    tick_num++;
}

static VOID app_server_entry(ULONG thread_input)
{
UINT                 i = 0;
UINT                 status;
ULONG                events = 0;
UCHAR                initialized = NX_FALSE;
ULONG                msw, lsw;
UCHAR               *data;
ULONG                data_length;
#ifdef DEMO_MULTICAST_ENABLED
SAMPLE_CLIENT       *client_ptr = &sample_client_multicast;
#else
SAMPLE_CLIENT       *client_ptr;
#endif /* DEMO_MULTICAST_ENABLED */


    /* Create the global timeout timer.  */
    status = tx_timer_create(&demo_timer, "Demo Timer", demo_timer_entry, 0,
                             (DEMO_PLAY_TIMER_INTERVAL * NX_IP_PERIODIC_RATE / 1000),
                             (DEMO_PLAY_TIMER_INTERVAL * NX_IP_PERIODIC_RATE / 1000),
                             TX_AUTO_ACTIVATE);

    while (1)
    {

        tx_event_flags_get(&demo_test_events, DEMO_ALL_EVENTS, TX_OR_CLEAR, &events, TX_WAIT_FOREVER);

        /*********************************************
        ************** DEMO_PLAY_EVENT ***************
        *********************************************/
        if (events & DEMO_PLAY_EVENT)
        {
            if (initialized == NX_FALSE)
            {
                /* Call user registered callback function to initialize. */
                VENC_APP_EncodingStart();
            }

            /* Set the initialized flag */
            initialized = NX_TRUE;
        }

        /*********************************************
        ************ DEMO_TEARDOWN_EVENT *************
        *********************************************/
        if (events & DEMO_TEARDOWN_EVENT)
        {
#ifdef DEMO_MULTICAST_ENABLED
            if ((client_ptr -> rtp_session_video_client_count == 0) &&
                (client_ptr -> rtp_session_audio_client_count == 0))
            {
                initialized = NX_FALSE;
            }

#else
            /* Check all client count to determine whether to clear the initialized flag */
            for (i = 0; i < NX_RTSP_SERVER_MAX_CLIENTS; i++)
            {
                client_ptr = &sample_client[i];
                if ((client_ptr -> rtp_session_video_client_count) ||
                    (client_ptr -> rtp_session_audio_client_count))
                {
                    break;
                }
            }
            VENC_APP_EncodingStop();
            if (i == NX_RTSP_SERVER_MAX_CLIENTS)
            {
                initialized = NX_FALSE;
            }
#endif
        }

        /*********************************************
        ******** DEMO_VIDEO_DATA_READY_EVENT *********
        *********************************************/
        if (events & DEMO_VIDEO_DATA_READY_EVENT)
        {

            /* Check if a play event has already triggered. */
            if (initialized == NX_FALSE)
            {
                initialized = NX_FALSE;
                continue;
            }

            /* Read video data and transmit it */
            data_length = 0;
            if ((VENC_APP_GetData(&data, &data_length)) || (data_length == 0))
            {
                continue;
            }

            /* Use systick to compute network time for rtcp sender report */
            msw = tick_num / (1000 / DEMO_PLAY_TIMER_INTERVAL);
            lsw = ((ULONG64)tick_num << 32) / TX_TIMER_TICKS_PER_SECOND;

#ifndef DEMO_MULTICAST_ENABLED
            for (i = 0; i < NX_RTSP_SERVER_MAX_CLIENTS; i++)
            {
                client_ptr = &sample_client[i];
#endif /* DEMO_MULTICAST_ENABLED */

                /* Make sure at least one client having setup the connection. */
                if (client_ptr -> rtp_session_video_client_count == 0)
                {
                    continue;
                }

                status = nx_rtp_sender_session_h264_send(&(client_ptr -> rtp_session_video), data, data_length,
                                                         client_ptr -> rtp_session_video_timestamp, msw, lsw, NX_TRUE);
                if (status)
                {
                    printf("Fail to send video frame: %d, %d\r\n", i, status);
                }

                /* Update rtp timestamp video sampling period. */
                client_ptr -> rtp_session_video_timestamp += DEMO_RTP_VIDEO_SAMPLING_PERIOD;
            }
#ifndef DEMO_MULTICAST_ENABLED
        }
#endif /* DEMO_MULTICAST_ENABLED */
    }
}


/* SDP string options */
static UCHAR *sdp =
#ifdef DEMO_MULTICAST_ENABLED
#ifdef DEMO_H264_ENABLED
"v=0\r\ns=H264 video with AAC audio, streamed by the NetX RTSP Server\r\n\
m=video 6002 RTP/AVP 96\r\n\
c=IN IP4 224.1.0.55/128\r\n\
a=rtpmap:96 H264/90000\r\n\
a=fmtp:96 profile-level-id=42A01E; packetization-mode=1\r\n\
a=control:trackID=0\r\n\
m=audio 6002 RTP/AVP 11\r\n\
c=IN IP4 224.1.0.55/128\r\n\
a=rtpmap:11 L16/44100/1\r\n\
a=fmtp:11 emphasis=50-15\r\n\
a=ptime:5\r\n\
a=control:trackID=1\r\n";
#else
"v=0\r\ns=MJPEG video with AAC audio, streamed by the NetX RTSP Server\r\n\
m=video 6002 RTP/AVP 26\r\n\
c=IN IP4 224.1.0.55/128\r\n\
a=rtpmap:26 JPEG/90000\r\n\
a=control:trackID=0\r\n\
m=audio 6002 RTP/AVP 11\r\n\
c=IN IP4 224.1.0.55/128\r\n\
a=rtpmap:11 L16/44100/1\r\n\
a=fmtp:11 emphasis=50-15\r\n\
a=ptime:5\r\n\
a=control:trackID=1\r\n";
#endif /* DEMO_H264_ENABLED */
#else
#ifdef DEMO_H264_ENABLED
"v=0\r\ns=H264 video with AAC audio, streamed by the NetX RTSP Server\r\n\
m=video 0 RTP/AVP 96\r\n\
a=rtpmap:96 H264/90000\r\n\
a=fmtp:96 profile-level-id=42A01E; packetization-mode=1\r\n\
a=control:trackID=0\r\n";
#else
"v=0\r\ns=MJPEG video with AAC audio, streamed by the NetX RTSP Server\r\n\
m=video 0 RTP/AVP 26\r\n\
a=rtpmap:26 JPEG/90000\r\n\
a=control:trackID=0\r\n\
m=audio 0 RTP/AVP 11\r\n\
a=rtpmap:11 L16/44100/1\r\n\
a=fmtp:11 emphasis=50-15\r\n\
a=ptime:5\r\n\
a=control:trackID=1\r\n";
#endif /* DEMO_H264_ENABLED */
#endif /* DEMO_MULTICAST_ENABLED */

/* If DEMO_AAC_ENABLED defined, replace PCM corresponding string with below string. */
// m=audio 0 RTP/AVP 97\r\n\
// a=rtpmap:97 mpeg4-generic/44100/1\r\n\
// a=fmtp:97 mode=AAC-hbr; SizeLength=13\r\n\


static UINT rtsp_describe_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length)
{
UINT status;


    status = nx_rtsp_server_sdp_set(rtsp_client_ptr, sdp, strlen((const char *)sdp));
    printf("RTSP request received: DESCRIBE.\r\n");
    return(status);
}

static UINT rtsp_setup_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length, NX_RTSP_TRANSPORT *transport_ptr)
{
UINT status;
UINT rtp_port, rtcp_port;
SAMPLE_CLIENT *client_ptr = NX_NULL;
#ifndef DEMO_MULTICAST_ENABLED
UINT i;
#endif /* DEMO_MULTICAST_ENABLED */


    /* Print information from the client */
    printf("RTSP request received: SETUP.\r\nuri: %s\r\nclient RTP port %d, RTCP port %d, IP %x\r\n", uri,
           transport_ptr -> client_rtp_port, transport_ptr -> client_rtcp_port, transport_ptr -> client_ip_address.nxd_ip_address.v4);

    /* Get the created and found ports */
    status = nx_rtp_sender_port_get(&rtp_0, &rtp_port, &rtcp_port);
    if (status)
    {
        return(status);
    }
    transport_ptr -> server_rtp_port = rtp_port;
    transport_ptr -> server_rtcp_port = rtcp_port;

#ifdef DEMO_MULTICAST_ENABLED
    /* Judge and change to multicast if received ip address is 0 */
    if (transport_ptr -> client_ip_address.nxd_ip_address.v4 != 0)
    {
        return(NX_NOT_SUCCESSFUL);
    }

    /* Assign multicast ip address and rtp/rtcp ports */
    transport_ptr -> client_ip_address.nxd_ip_version = NX_IP_VERSION_V4;
    transport_ptr -> client_ip_address.nxd_ip_address.v4 = DEMO_MULTICAST_IP_ADDRESS;
    transport_ptr -> client_rtp_port = DEMO_MULTICAST_RTP_PORT;
    transport_ptr -> client_rtcp_port = DEMO_MULTICAST_RTCP_PORT;
    transport_ptr -> multicast_ttl = NX_RTP_SENDER_TIME_TO_LIVE;

    /* Directly use sample_client_multicast */
    client_ptr = &sample_client_multicast;
#else
    /* Find and store the RTSP client pointer.  */
    for (i = 0; i < NX_RTSP_SERVER_MAX_CLIENTS; i++)
    {
        if (sample_client[i].rtsp_client_ptr == rtsp_client_ptr)
        {
            client_ptr = &(sample_client[i]);
            break;
        }
        else if (sample_client[i].rtsp_client_ptr == NX_NULL)
        {
            sample_client[i].rtsp_client_ptr = rtsp_client_ptr;
            client_ptr = &(sample_client[i]);
            break;
        }
    }
    if (client_ptr == NX_NULL)
    {
        return(NX_NOT_SUCCESSFUL);
    }
#endif /* DEMO_MULTICAST_ENABLED */

    if (strstr((const char *)uri, DEMO_RTSP_VIDEO_FILE_NAME))
    {
        printf("Setup Video (track 0)..\r\n");

#ifdef DEMO_MULTICAST_ENABLED
        if (client_ptr -> rtp_session_video_client_count == 0)
        {
#endif /* DEMO_MULTICAST_ENABLED */

            /* Setup rtp sender video session */
            status = nx_rtp_sender_session_create(&rtp_0, &(client_ptr -> rtp_session_video), DEMO_RTP_PAYLOAD_TYPE_VIDEO,
                                                  transport_ptr -> interface_index, &(transport_ptr -> client_ip_address),
                                                  transport_ptr -> client_rtp_port, transport_ptr -> client_rtcp_port);
            if (status)
            {
                printf("Fail to create video session\r\n");

                /* Reset the client pointer if error status happens */
                client_ptr -> rtsp_client_ptr = NX_NULL;
                return(status);
            }

            /* Obtain generated ssrc */
            status = nx_rtp_sender_session_ssrc_get(&(client_ptr -> rtp_session_video), &(transport_ptr -> rtp_ssrc));
            if (status)
            {

                /* Reset the client pointer if error status happens */
                client_ptr -> rtsp_client_ptr = NX_NULL;
                return(status);
            }

            /* Reset corresponding variables */
            client_ptr -> rtp_session_video_timestamp = (ULONG)NX_RAND();
#ifdef DEMO_MULTICAST_ENABLED
        }
#endif /* DEMO_MULTICAST_ENABLED */

        /* Increase the number of setup client. */
        client_ptr -> rtp_session_video_client_count++;
    }
    else if (strstr((const char *)uri, DEMO_RTSP_AUDIO_FILE_NAME))
    {
        printf("Setup Audio (track 1)..\r\n");

#ifdef DEMO_MULTICAST_ENABLED
        if (client_ptr -> rtp_session_audio_client_count == 0)
        {
#endif /* DEMO_MULTICAST_ENABLED */

            /* Setup rtp sender audio session */
            status = nx_rtp_sender_session_create(&rtp_0, &(client_ptr -> rtp_session_audio), DEMO_RTP_PAYLOAD_TYPE_AUDIO,
                                                  transport_ptr -> interface_index, &(transport_ptr -> client_ip_address),
                                                  transport_ptr -> client_rtp_port, transport_ptr -> client_rtcp_port);
            if (status)
            {
                printf("Fail to create audio session\r\n");

                /* Reset the client pointer if error status happens */
                client_ptr -> rtsp_client_ptr = NX_NULL;
                return(status);
            }

            /* Obtain generated ssrc */
            status = nx_rtp_sender_session_ssrc_get(&(client_ptr -> rtp_session_audio), &(transport_ptr -> rtp_ssrc));
            if (status)
            {

                /* Reset the client pointer if error status happens */
                client_ptr -> rtsp_client_ptr = NX_NULL;
                return(status);
            }

            /* Reset corresponding variables */
            client_ptr -> rtp_session_audio_timestamp = (ULONG)NX_RAND();

#ifndef DEMO_AAC_ENABLED
            nx_rtp_sender_session_sample_factor_set(&(client_ptr -> rtp_session_audio), (DEMO_AUDIO_SAMPLE_SIZE / 8) * DEMO_AUDIO_CHANNEL_NUM);
#endif /* DEMO_AAC_ENABLED */
#ifdef DEMO_MULTICAST_ENABLED
        }
#endif /* DEMO_MULTICAST_ENABLED */

        /* Increase the number of setup client. */
        client_ptr -> rtp_session_audio_client_count++;
    }
    else
    {
        printf("Invalid track ID!\r\n");

#ifndef DEMO_MULTICAST_ENABLED
        /* Reset the client pointer and flags if error status happens */
        client_ptr -> rtsp_client_ptr = rtsp_client_ptr;
        client_ptr -> rtp_session_video_client_count = 0;
        client_ptr -> rtp_session_audio_client_count = 0;
#endif /* DEMO_MULTICAST_ENABLED */

        return(NX_NOT_SUCCESSFUL);
    }

    return(NX_SUCCESS);
}

static UINT rtsp_play_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length, UCHAR *range_ptr, UINT range_length)
{
UINT status;
UINT video_seq, audio_seq, video_rtptime, audio_rtptime;
SAMPLE_CLIENT *client_ptr;
#ifndef DEMO_MULTICAST_ENABLED
UINT i;
#endif


    printf("RTSP request received: PLAY.\r\n");

#ifndef DEMO_MULTICAST_ENABLED
    /* Search and find the RTSP client. */
    for (i = 0; i < NX_RTSP_SERVER_MAX_CLIENTS; i++)
    {
        if (sample_client[i].rtsp_client_ptr == rtsp_client_ptr)
        {
            client_ptr = &sample_client[i];
            break;
        }
    }
    if (i == NX_RTSP_SERVER_MAX_CLIENTS)
    {
        printf("Fail to find rtsp client!\r\n");
        return(NX_NOT_SUCCESSFUL);
    }
#else
    /* Directly use sample_client_multicast */
    client_ptr = &sample_client_multicast;
#endif /* DEMO_MULTICAST_ENABLED */

    if (client_ptr -> rtp_session_video_client_count)
    {
        /* Retrieve the sequence number through rtp sender functions */
        nx_rtp_sender_session_sequence_number_get(&(client_ptr -> rtp_session_video), &video_seq);

        /* Assign recorded timestamps */
        video_rtptime = client_ptr -> rtp_session_video_timestamp;

        /* Set rtp information into rtsp client */
        status = nx_rtsp_server_rtp_info_set(rtsp_client_ptr, DEMO_RTSP_VIDEO_FILE_NAME, sizeof(DEMO_RTSP_VIDEO_FILE_NAME) - 1, video_seq, video_rtptime);
        if (status)
        {
            return(status);
        }
    }

    if (client_ptr -> rtp_session_audio_client_count)
    {

        /* Retrieve the sequence number through rtp sender functions */
        nx_rtp_sender_session_sequence_number_get(&(client_ptr -> rtp_session_audio), &audio_seq);

        /* Assign recorded timestamps */
        audio_rtptime = client_ptr -> rtp_session_audio_timestamp;

        /* Set rtp information into rtsp client */
        status = nx_rtsp_server_rtp_info_set(rtsp_client_ptr, DEMO_RTSP_AUDIO_FILE_NAME, sizeof(DEMO_RTSP_VIDEO_FILE_NAME) - 1, audio_seq, audio_rtptime);
        if (status)
        {
            return(status);
        }
    }

    /* Trigger the play event */
    tx_event_flags_set(&demo_test_events, DEMO_PLAY_EVENT, TX_OR);

    return(NX_SUCCESS);
}

static UINT rtsp_teardown_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length)
{
SAMPLE_CLIENT *client_ptr;
#ifndef DEMO_MULTICAST_ENABLED
UINT i;


    printf("RTSP request received: TEARDOWN.\r\n");

    /* Find the RTSP client pointer.  */
    for (i = 0; i < NX_RTSP_SERVER_MAX_CLIENTS; i++)
    {
        if (sample_client[i].rtsp_client_ptr == rtsp_client_ptr)
        {
            client_ptr = &(sample_client[i]);
            break;
        }
    }
    if (i == NX_RTSP_SERVER_MAX_CLIENTS)
    {
        printf("Fail to find rtsp client!\r\n");
        return(NX_NOT_SUCCESSFUL);
    }
#else
    /* Directly use sample_client_multicast */
    client_ptr = &sample_client_multicast;
#endif /* DEMO_MULTICAST_ENABLED */

    /* Decrease session client count */
    if (client_ptr -> rtp_session_video_client_count > 0)
    {
        client_ptr -> rtp_session_video_client_count--;
#ifdef DEMO_MULTICAST_ENABLED
        if (client_ptr -> rtp_session_video_client_count == 0)
#endif
        {
            client_ptr -> rtsp_client_ptr = NX_NULL;
            nx_rtp_sender_session_delete(&(client_ptr -> rtp_session_video));
        }
    }
    if (client_ptr -> rtp_session_audio_client_count > 0)
    {
        client_ptr -> rtp_session_audio_client_count--;
#ifdef DEMO_MULTICAST_ENABLED
        if (client_ptr -> rtp_session_audio_client_count == 0)
#endif
        {
            client_ptr -> rtsp_client_ptr = NX_NULL;
            nx_rtp_sender_session_delete(&(client_ptr -> rtp_session_audio));
        }
    }

    /* Trigger the tear down event */
    tx_event_flags_set(&demo_test_events, DEMO_TEARDOWN_EVENT, TX_OR);

    return(NX_SUCCESS);
}

static UINT rtsp_pause_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length, UCHAR *range_ptr, UINT range_length)
{
    printf("RTSP request received: PAUSE.\r\n");

    return(NX_SUCCESS);
}

static UINT rtsp_set_parameter_callback(NX_RTSP_CLIENT *rtsp_client_ptr, UCHAR *uri, UINT uri_length, UCHAR *parameter_ptr, ULONG parameter_length)
{
    printf("RTSP request received: SET PARAMETER.\r\n");

    return(NX_SUCCESS);
}

static UINT rtsp_disconnect_callback(NX_RTSP_CLIENT *rtsp_client_ptr)
{

    /* Trigger the tear down event */
    tx_event_flags_set(&demo_test_events, DEMO_TEARDOWN_EVENT, TX_OR);

    return(NX_SUCCESS);
}

static UINT test_rtcp_receiver_report_callback(NX_RTP_SESSION *session, NX_RTCP_RECEIVER_REPORT *report)
{
#ifndef DEMO_MULTICAST_ENABLED
UINT i;
#endif /* DEMO_MULTICAST_ENABLED */


    printf("==== RTCP Receive report ====\r\n");

    /* Search the rtsp client table and find which session it is*/
#ifdef DEMO_MULTICAST_ENABLED
    if (session == &(sample_client_multicast.rtp_session_video))
    {
        printf("\r\nVideo Session: multicast\r\n");
    }
    else if (session == &(sample_client_multicast.rtp_session_audio))
    {
        printf("\r\nAudio Session: multicast\r\n");
    }
    else
    {

        /* Unknown session, return directly. */
        return(NX_SUCCESS);
    }
#else
    for (i = 0; i < NX_RTSP_SERVER_MAX_CLIENTS; i++)
    {
        if (session == &(sample_client[i].rtp_session_video))
        {
            printf("Video Session: %d\r\n", (i + 1));
            break;
        }
        else if (session == &(sample_client[i].rtp_session_audio))
        {
            printf("Audio Session: %d\r\n", (i + 1));
            break;
        }
    }
    if (i == NX_RTSP_SERVER_MAX_CLIENTS)
    {

        /* Unknown session, return directly. */
        return(NX_SUCCESS);
    }
#endif /* DEMO_MULTICAST_ENABLED */

    printf("receiver SSRC:%u\r\n", report -> receiver_ssrc);
    printf("fraction lost:%d / 256\r\n", report -> fraction_loss);
    printf("cumulative number of packets lost:%d\r\n", report -> packet_loss);
    printf("extended highest sequence number received:%u\r\n", report -> extended_max);
    printf("inter-arrival jitter:%u\r\n", report -> jitter);
    printf("last SR timestamp:%u\r\n", report -> last_sr);
    printf("delay since last SR timestamp:%u (%d milliseconds)\r\n\r\n", report -> delay, report -> delay * 1000 / 65536);

    /* Update the timeout of RTSP server since the RTCP message proves liveness.  */
#ifdef DEMO_MULTICAST_ENABLED
    nx_rtsp_server_keepalive_update(sample_client_multicast.rtsp_client_ptr);
#else
    nx_rtsp_server_keepalive_update(sample_client[i].rtsp_client_ptr);
#endif /* DEMO_MULTICAST_ENABLED */

    return(NX_SUCCESS);
}

static UINT test_rtcp_sdes_callback(NX_RTCP_SDES_INFO *sdes_info)
{
    printf("\r\n==== RTCP SDES report ====\r\n");
    printf("SSRC: %u\r\n", sdes_info -> ssrc);
    printf("Length: %d\r\n", sdes_info -> cname_length);
    printf("CNAME: %.*s\r\n", sdes_info -> cname_length, sdes_info -> cname);
    return NX_SUCCESS;
}
