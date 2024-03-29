/*
     state machine for receiving banners
*/
#include "smack.h"
#include "rawsock-pcapfile.h"
#include "proto-preprocess.h"
#include "proto-interactive.h"
#include "proto-banner1.h"
#include "proto-http.h"
#include "proto-ssl.h"
#include "proto-ssh.h"
#include "proto-ftp.h"
#include "proto-smtp.h"
#include "proto-imap4.h"
#include "proto-pop3.h"
#include "proto-vnc.h"
#include "proto-memcached.h"
#include "masscan.h"
#include "masscan-app.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>



struct Patterns patterns[] = {
    {"SSH-1.",      6, PROTO_SSH1, SMACK_ANCHOR_BEGIN},
    {"SSH-2.",      6, PROTO_SSH2, SMACK_ANCHOR_BEGIN},
    {"HTTP/1.",     7, PROTO_HTTP, SMACK_ANCHOR_BEGIN},
    {"220-",        4, PROTO_FTP, SMACK_ANCHOR_BEGIN, 0},
    {"220 ",        4, PROTO_FTP, SMACK_ANCHOR_BEGIN, 1},
    {"+OK ",        4, PROTO_POP3, SMACK_ANCHOR_BEGIN},
    {"* OK ",       5, PROTO_IMAP4, SMACK_ANCHOR_BEGIN},
    {"\x16\x03\x00",3, PROTO_SSL3, SMACK_ANCHOR_BEGIN},
    {"\x16\x03\x01",3, PROTO_SSL3, SMACK_ANCHOR_BEGIN},
    {"\x16\x03\x02",3, PROTO_SSL3, SMACK_ANCHOR_BEGIN},
    {"\x16\x03\x03",3, PROTO_SSL3, SMACK_ANCHOR_BEGIN},
    {"\x15\x03\x00",3, PROTO_SSL3, SMACK_ANCHOR_BEGIN},
    {"\x15\x03\x01",3, PROTO_SSL3, SMACK_ANCHOR_BEGIN},
    {"\x15\x03\x02",3, PROTO_SSL3, SMACK_ANCHOR_BEGIN},
    {"\x15\x03\x03",3, PROTO_SSL3, SMACK_ANCHOR_BEGIN},
    {"RFB 000.000\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 1}, /* UltraVNC repeater mode */
    {"RFB 003.003\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 3}, /* default version for everything */
    {"RFB 003.005\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 3}, /* broken, same as 003.003 */
    {"RFB 003.006\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 3}, /* broken, same as 003.003 */
    {"RFB 003.007\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 7}, 
    {"RFB 003.008\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 8}, 
    {"RFB 003.889\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 8}, /* Apple's remote desktop, 003.007 */
    {"RFB 003.009\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 8}, 
    {"RFB 004.000\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 8}, /* Intel AMT KVM */
    {"RFB 004.001\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 8}, /* RealVNC 4.6 */
    {"RFB 004.002\n", 12, PROTO_VNC_RFB, SMACK_ANCHOR_BEGIN, 8},
    {"STAT pid ",      9, PROTO_MEMCACHED,SMACK_ANCHOR_BEGIN}, /* memcached stat response */
    {0,0}
};




/***************************************************************************
 ***************************************************************************/
unsigned
banner1_parse(
        const struct Banner1 *banner1,
        struct ProtocolState *tcb_state,
        const unsigned char *px, size_t length,
        struct BannerOutput *banout,
        struct InteractiveData *more)
{
    size_t x;
    unsigned offset = 0;
    unsigned proto;


    switch (tcb_state->app_proto) {
    case PROTO_NONE:
    case PROTO_HEUR:
        x = smack_search_next(
                        banner1->smack,
                        &tcb_state->state,
                        px, &offset, (unsigned)length);
        if (x != SMACK_NOT_FOUND)
            proto = patterns[x].id;
        else
            proto = 0xFFFFFFFF;
        if (proto != 0xFFFFFFFF
            && !(proto == PROTO_SSL3 && !tcb_state->is_sent_sslhello)) {
            unsigned i;

            /* re-read the stuff that we missed */
            for (i=0; patterns[i].id && patterns[i].id != tcb_state->app_proto; i++)
                ;

            /* Kludge: patterns look confusing, so add port info to the
             * pattern */
            switch (proto) {
            case PROTO_FTP:
                if (patterns[x].extra == 1) {
                    if (tcb_state->port == 25 || tcb_state->port == 587)
                        proto = PROTO_SMTP;
                }
                break;
            case PROTO_VNC_RFB:
                tcb_state->sub.vnc.version = (unsigned char)patterns[x].extra;
                break;
            }

            tcb_state->app_proto = (unsigned short)proto;

            /* reset the state back again */
            tcb_state->state = 0;

            /* If there is any data from a previous packet, re-parse that */
            {
                const unsigned char *s = banout_string(banout, PROTO_HEUR);
                unsigned s_len = banout_string_length(banout, PROTO_HEUR);

                if (s && s_len)
                banner1_parse(
                                banner1,
                                tcb_state,
                                s, s_len,
                                banout,
                                more);
            }
            banner1_parse(
                            banner1,
                            tcb_state,
                            px, length,
                            banout,
                            more);
        } else {
            banout_append(banout, PROTO_HEUR, px, length);
        }
        break;
    case PROTO_FTP:
            banner_ftp.parse(   banner1,
                             banner1->http_fields,
                             tcb_state,
                             px, length,
                             banout,
                             more);
            break;
        case PROTO_SMTP:
            banner_smtp.parse(   banner1,
                              banner1->http_fields,
                              tcb_state,
                              px, length,
                              banout,
                              more);
            break;
            
    case PROTO_POP3:
            banner_pop3.parse(   banner1,
                              banner1->http_fields,
                              tcb_state,
                              px, length,
                              banout,
                              more);
            break;
    case PROTO_IMAP4:
            banner_imap4.parse(banner1,
                              banner1->http_fields,
                              tcb_state,
                              px, length,
                              banout,
                              more);
            break;
            
    case PROTO_SSH1:
    case PROTO_SSH2:
        /* generic text-based parser
         * TODO: in future, need to split these into separate protocols,
         * especially when binary parsing is added to SSH */
        banner_ssh.parse(   banner1,
                            banner1->http_fields,
                            tcb_state,
                            px, length,
                            banout,
                            more);
        break;
    case PROTO_HTTP:
        banner_http.parse(
                        banner1,
                        banner1->http_fields,
                        tcb_state,
                        px, length,
                        banout,
                        more);
        break;
    case PROTO_SSL3:
        banner_ssl.parse(
                        banner1,
                        banner1->http_fields,
                        tcb_state,
                        px, length,
                        banout,
                        more);
        break;
    case PROTO_VNC_RFB:
        banner_vnc.parse(    banner1,
                             banner1->http_fields,
                             tcb_state,
                             px, length,
                             banout,
                             more);
        break;
    case PROTO_MEMCACHED:
        banner_memcached.parse(    banner1,
                             banner1->http_fields,
                             tcb_state,
                             px, length,
                             banout,
                             more);
        break;
    default:
        LOG(0,  "banner1: internal error\n");
        break;

    }

    return tcb_state->app_proto;
}


/***************************************************************************
 * Create the --banners systems
 ***************************************************************************/
struct Banner1 *
banner1_create(void)
{
    struct Banner1 *b;
    unsigned i;

    b = (struct Banner1 *)malloc(sizeof(*b));
    if (b == NULL)
        exit(1);
    memset(b, 0, sizeof(*b));

    /*
     * These patterns match the start of the TCP stream
     */
    b->smack = smack_create("banner1", SMACK_CASE_INSENSITIVE);
    for (i=0; patterns[i].pattern; i++)
        smack_add_pattern(
                    b->smack,
                    patterns[i].pattern,
                    patterns[i].pattern_length,
                    i,
                    patterns[i].is_anchored);
    smack_compile(b->smack);


    banner_http.init(b);
    banner_vnc.init(b);
    banner_memcached.init(b);

    b->tcp_payloads[80] = &banner_http;
    b->tcp_payloads[8080] = &banner_http;
    
    b->tcp_payloads[443]  = (ProtocolParserStream*)&banner_ssl;  /* HTTP/s */
    b->tcp_payloads[465]  = (ProtocolParserStream*)&banner_ssl;  /* SMTP/s */
    b->tcp_payloads[990]  = (ProtocolParserStream*)&banner_ssl;  /* FTP/s */
    b->tcp_payloads[991]  = (ProtocolParserStream*)&banner_ssl;
    b->tcp_payloads[992]  = (ProtocolParserStream*)&banner_ssl;  /* Telnet/s */
    b->tcp_payloads[993]  = (ProtocolParserStream*)&banner_ssl;  /* IMAP4/s */
    b->tcp_payloads[994]  = (ProtocolParserStream*)&banner_ssl;
    b->tcp_payloads[995]  = (ProtocolParserStream*)&banner_ssl;  /* POP3/s */
    b->tcp_payloads[2083] = (ProtocolParserStream*)&banner_ssl;  /* cPanel - SSL */
    b->tcp_payloads[2087] = (ProtocolParserStream*)&banner_ssl;  /* WHM - SSL */
    b->tcp_payloads[2096] = (ProtocolParserStream*)&banner_ssl;  /* cPanel webmail - SSL */
    b->tcp_payloads[8443] = (ProtocolParserStream*)&banner_ssl;  /* Plesk Control Panel - SSL */
    b->tcp_payloads[9050] = (ProtocolParserStream*)&banner_ssl;  /* Tor */
    b->tcp_payloads[8140] = (ProtocolParserStream*)&banner_ssl;  /* puppet */

    return b;
}


/***************************************************************************
 ***************************************************************************/
void
banner1_destroy(struct Banner1 *b)
{
    if (b == NULL)
        return;
    if (b->smack)
        smack_destroy(b->smack);
    if (b->http_fields)
        smack_destroy(b->http_fields);
    free(b);
}





/***************************************************************************
 * Test the banner1 detection system by throwing random frames at it
 ***************************************************************************/
void
banner1_test(const char *filename)
{
    struct PcapFile *cap;
    unsigned link_type;

    cap = pcapfile_openread(filename);
    if (cap == NULL) {
        LOG(0,  "%s: can't open capture file\n", filename);
        return;
    }

    link_type = pcapfile_datalink(cap);

    for (;;) {
        int packets_read;
        unsigned secs;
        unsigned usecs;
        unsigned origlength;
        unsigned length;
        unsigned char px[65536];
        struct PreprocessedInfo parsed;
        unsigned x;


        packets_read = pcapfile_readframe(
                    cap,    /* capture dump file */
                    &secs, &usecs,
                    &origlength, &length,
                    px, sizeof(px));
        if (packets_read == 0)
            break;


        x = preprocess_frame(px, length, link_type, &parsed);
        if (x == 0)
            continue;

    }

    pcapfile_close(cap);
}

/***************************************************************************
 ***************************************************************************/
int
banner1_selftest()
{
    unsigned i;
    struct Banner1 *b;
    struct ProtocolState tcb_state[1];
    const unsigned char *px;
    unsigned length;
    struct BannerOutput banout[1];
    static const char *http_header =
        "HTTP/1.0 302 Redirect\r\n"
        "Date: Tue, 03 Sep 2013 06:50:01 GMT\r\n"
        "Connection: close\r\n"
        "Via: HTTP/1.1 ir14.fp.bf1.yahoo.com (YahooTrafficServer/1.2.0.13 [c s f ])\r\n"
        "Server: YTS/1.20.13\r\n"
        "Cache-Control: no-store\r\n"
        "Content-Type: text/html\r\n"
        "Content-Language: en\r\n"
        "Location: http://failsafe.fp.yahoo.com/404.html\r\n"
        "Content-Length: 227\r\n"
        "\r\n<title>hello</title>\n";
    px = (const unsigned char *)http_header;
    length = (unsigned)strlen(http_header);


    /*
     * First, test the "banout" subsystem
     */
    if (banout_selftest() != 0) {
        LOG(0,  "banout: failed\n");
        return 1;
    }


    /*
     * Test one character at a time
     */
    b = banner1_create();
    banout_init(banout);

    memset(tcb_state, 0, sizeof(tcb_state[0]));

    for (i=0; i<length; i++) {
        struct InteractiveData more = {0,0};

        banner1_parse(
                    b,
                    tcb_state,
                    px+i, 1,
                    banout,
                    &more);
    }


    {
        const unsigned char *s = banout_string(banout, PROTO_HTTP);
        if (memcmp(s, "HTTP/1.0 302", 11) != 0) {
            printf("banner1: test failed\n");
            return 1;
        }
    }
    banout_release(banout);
    banner1_destroy(b);

    /*
     * Test whole buffer
     */
    b = banner1_create();

    memset(tcb_state, 0, sizeof(tcb_state[0]));

    banner1_parse(
                    b,
                    tcb_state,
                    px, length,
                    banout,
                    0);
    banner1_destroy(b);
    /*if (memcmp(banner, "Via:HTTP/1.1", 11) != 0) {
        printf("banner1: test failed\n");
        return 1;
    }*/


    {
        int x = 0;

        x = banner_ssl.selftest();
        if (x) {
            LOG(0,  "SSL banner: selftest failed\n");
            return 1;
        }

        x = banner_http.selftest();
        if (x) {
            LOG(0,  "HTTP banner: selftest failed\n");
            return 1;
        }

        return x;
    }
}

