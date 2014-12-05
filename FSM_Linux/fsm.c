//
// fsm.c
// FSM sample code
//
// Created by Minsuk Lee, 2014.11.1.
// Copyright (c) 2014. Minsuk Lee All rights reserved.
// see LICENSE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <termios.h>

#include "util.h"

#define CONNECT_TIMEOUT 10

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}


//#define TEST_FSM

#define NUM_STATE   3
#define NUM_EVENT   8

enum pakcet_type { F_CON = 0, F_ACK = 2, F_FIN = 1, F_DATA = 3 };       // Packet Type
enum proto_state { wait_CON = 0, CON_sent = 1, CONNECTED = 2 };     // States

// Events
enum proto_event { RCV_CON = 0, RCV_FIN = 1, RCV_ACK = 2, RCV_DATA = 3,
    CONNECT = 4, CLOSE = 5, SEND = 6, TIMEOUT = 7 };

char *pkt_name[] = { "F_CON", "F_ACK", "F_FIN", "F_DATA" };
char *st_name[] =  { "wait_CON", "CON_sent", "CONNECTED" };
char *ev_name[] =  { "RCV_CON", "RCV_FIN", "RCV_ACK", "RCV_DATA",
                     "CONNECT", "CLOSE",   "SEND",    "TIMEOUT"   };

struct state_action {           // Protocol FSM Structure
    void (* action)(void *p);
    enum proto_state next_state;
};

#define MAX_DATA_SIZE   (500)
struct packet {                 // 504 Byte Packet to & from Simulator
    unsigned short type;        // enum packet_type
    unsigned short size;
    char data[MAX_DATA_SIZE];
};

struct p_event {                // Event Structure
    enum proto_event event;
    struct packet packet;
    int size;
};

enum proto_state c_state = wait_CON;         // Initial State
volatile int timedout = 0;

static void timer_handler(int signum)
{
    printf("Timedout\n");
    timedout = 1;
}

static void timer_init(void)
{
    struct sigaction sa;

    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction(SIGALRM, &sa, NULL);
}

void set_timer(int sec)
{
    struct itimerval timer;

    timedout = 0;
    timer.it_value.tv_sec = sec;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;   // Non Periodic timer
    timer.it_interval.tv_usec = 0;
    setitimer (ITIMER_REAL, &timer, NULL);
}
void send_packet(int flag, void *p, int size)
{
    struct packet buf;
    printf("SEND %s\n", pkt_name[flag]);
    
    buf.type = flag;
    if (size) {
        buf.size = size;
        memcpy(buf.data, p, (size > MAX_DATA_SIZE) ? MAX_DATA_SIZE : size);
        Send((char*)&buf, size + sizeof(unsigned short) * 2);
    } else {
        Send((char*)&buf.type, sizeof(unsigned short));
    }
}

static void report_connect(void *p)
{
    printf("Connected\n");
    set_timer(0);           // Stop Timer
    // Report connection made to upper layer
}

static void passive_con(void *p)
{
    send_packet(F_ACK, NULL, 0);
    report_connect(NULL);
}

static void active_con(void *p)
{
    send_packet(F_CON, NULL, 0);
    set_timer(CONNECT_TIMEOUT);
}

static void close_con(void *p)
{
    printf("Connection Closed\n");
    send_packet(F_FIN, NULL, 0);
    // Report Connected Closed to upper layer
}

static void send_data(void *p)
{
    printf("Send Data to peer size:%d\n", ((struct p_event*)p)->size);
    send_packet(F_DATA, &((struct p_event *)p)->packet, ((struct p_event *)p)->size);
}

static void report_data(void *p)
{
    printf("Data Arrived size:%d\n", ((struct p_event*)p)->size);
    // Queue received data for upper layer user
}

struct state_action p_FSM[NUM_STATE][NUM_EVENT] = {
  //  for each event:
  //  RCV_CON,                    RCV_FIN,                 RCV_ACK,                       RCV_DATA,
  //  CONNECT,                    CLOSE,                   SEND,                          TIMEOUT

  // - wait_CON state
  {{ passive_con, CONNECTED }, { NULL, wait_CON },      { NULL, wait_CON },            { NULL, wait_CON },
   { active_con,  CON_sent },  { NULL, wait_CON },      { NULL, wait_CON },            { NULL, wait_CON }},

  // - CON_sent state
  {{ passive_con, CONNECTED }, { close_con, wait_CON }, { report_connect, CONNECTED }, { NULL,      CON_sent },
   { NULL,        CON_sent },  { close_con, wait_CON }, { NULL,           CON_sent },  { close_con, wait_CON }},

  // - CONNECTED state
  {{ NULL, CONNECTED },        { close_con, wait_CON }, { NULL,      CONNECTED },      { report_data, CONNECTED },
   { NULL, CONNECTED },        { close_con, wait_CON }, { send_data, CONNECTED },      { NULL,        CONNECTED }},
};

struct p_event *get_event(void)
{
    static struct p_event event;    // not thread-safe
    
loop:
    // Check if there is user command
    if (!kbhit()) {
        // Check if timer is timed-out
        if(timedout) {
            timedout = 0;
            event.event = TIMEOUT;
            goto got_it;
        } else {
            // Check Packet arrival by event_wait()
            ssize_t n = Recv((char*)&event.packet, MAX_DATA_SIZE);
            if (n > 0) {
                // if then, decode header to make event
                switch (event.packet.type) {
                    case F_CON:  event.event = RCV_CON;  break;
                    case F_ACK:  event.event = RCV_ACK;  break;
                    case F_FIN:  event.event = RCV_FIN;  break;
                    case F_DATA: event.event = RCV_DATA; break;
                    default:
                        goto loop;
                }
                goto got_it;
            }
        }
    } else {
        int n = getchar();
        switch (n) {
            case '0': event.event = CONNECT; break;
            case '1': event.event = CLOSE;   break;
            case '2':
                event.event = SEND;
                event.size = 0;
                break;
            case '3': return NULL;  // QUIT
            default:
                goto loop;
        }
        goto got_it;
    }
    goto loop;
    
got_it:
    return &event;
}

void
Protocol_Loop(void)
{
    struct p_event *eventp;

    timer_init();
    while (1) {
        printf("Current State = %s\n", st_name[c_state]);

        /* Step 0: Get Input Event */
        if((eventp = get_event()) == NULL)
            break;
        printf("EVENT : %s\n",ev_name[eventp->event]);
        /* Step 1: Do Action */
        if (p_FSM[c_state][eventp->event].action)
            p_FSM[c_state][eventp->event].action(eventp);
        else
            printf("No Action for this event\n");

        /* Step 2: Set Next State */
        c_state = p_FSM[c_state][eventp->event].next_state;
    }
}

int
main(int argc, char *argv[])
{
    ChannelNumber channel;
    ID id;
    int rateOfPacketLoss;

    printf("Channel : ");
    scanf("%d",&channel);
    printf("ID : ");
    scanf("%d",&id);
    printf("Rate of Packet Loss (0 ~ 100)%% : ");
    scanf("%d",&rateOfPacketLoss);
    if (rateOfPacketLoss < 0)
        rateOfPacketLoss = 0;
    else if (rateOfPacketLoss > 100)
        rateOfPacketLoss = 100;
        
    
    // SIMULATOR_INITIALIZE
    if (Login(channel, id, rateOfPacketLoss) == -1) {
        printf("Login Failed\n");
        return -1;
    }

    printf("Entering protocol loop...\n");
    printf("type number '[0]CONNECT', '[1]CLOSE', '[2]SEND', or '[3]QUIT'\n");
    Protocol_Loop();

    // SIMULATOR_CLOSE

    return 0;
}

