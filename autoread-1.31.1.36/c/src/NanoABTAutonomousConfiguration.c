/**
* Sample program that demonstrates enable/disable AutonomousMode.
* @file NanoABTAutonomousConfiguration.c
*/

#include <tm_reader.h>
#include <serial_reader_imp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <sys\timeb.h> 

#ifndef WIN32
#include <unistd.h>
#endif

/* Enable this to use transportListener */
#ifndef USE_TRANSPORT_LISTENER
#define USE_TRANSPORT_LISTENER 0
#endif

#define usage() {errx(1, "Please provide reader URL, such as:\n"\
    "tmr:///com4 --enable  to enable autonomous read (or) \ntmr:///com4 --disable to disable autonomous read\n");}

void errx(int exitval, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);

  exit(exitval);
}

void checkerr(TMR_Reader* rp, TMR_Status ret, int exitval, const char *msg)
{
  if (TMR_SUCCESS != ret)
    {
      errx(exitval, "Error %s: %s\n", msg, TMR_strerr(rp, ret));
    }
}

void serialPrinter(bool tx, uint32_t dataLen, const uint8_t data[],
                                     uint32_t timeout, void *cookie)
{
  FILE *out = cookie;
  uint32_t i;

  fprintf(out, "%s", tx ? "Sending: " : "Received:");
  for (i = 0; i < dataLen; i++)
  {
    if (i > 0 && (i & 15) == 0)
    {
      fprintf(out, "\n         ");
    }
    fprintf(out, " %02x", data[i]);
  }
  fprintf(out, "\n");
}

void stringPrinter(bool tx,uint32_t dataLen, const uint8_t data[],uint32_t timeout, void *cookie)
{
  FILE *out = cookie;

  fprintf(out, "%s", tx ? "Sending: " : "Received:");
  fprintf(out, "%s\n", data);
}

const char* protocolName(TMR_TagProtocol protocol)
{
  switch (protocol)
  {
    case TMR_TAG_PROTOCOL_NONE:
      return "NONE";
    case TMR_TAG_PROTOCOL_ISO180006B:
      return "ISO180006B";
    case TMR_TAG_PROTOCOL_GEN2:
      return "GEN2";
    case TMR_TAG_PROTOCOL_ISO180006B_UCODE:
      return "ISO180006B_UCODE";
    case TMR_TAG_PROTOCOL_IPX64:
      return "IPX64";
    case TMR_TAG_PROTOCOL_IPX256:
      return "IPX256";
    case TMR_TAG_PROTOCOL_ATA:
      return "ATA";
    default:
      return "unknown";
  }
}

void callback(TMR_Reader *reader, const TMR_TagReadData *t, void *cookie);
void statsCallback (TMR_Reader *reader, const TMR_Reader_StatsValues* stats, void *cookie);

int main(int argc, char *argv[])
{
  TMR_Reader r, *rp;
  TMR_Status ret;
  TMR_Region region;
  TMR_SR_UserConfigOp config;
  TMR_ReadPlan plan;
  TMR_TagOp op;
  char AutonomousMode[10] = {0};
  uint8_t antennaList[]={1};
  uint8_t i = 0;
  uint8_t antennaCount = 1;
  bool uniquebydata = false;

  uint32_t asyncOntime = 1000;
  uint32_t power = 2700;
  uint32_t baudrate = 115200;
  uint8_t readLen = 0x03;
  TMR_GEN2_LinkFrequency freq = TMR_GEN2_LINKFREQUENCY_250KHZ;
  TMR_GEN2_Session session = TMR_GEN2_SESSION_S0;
  TMR_GEN2_Target target = TMR_GEN2_TARGET_A;
  TMR_GEN2_TagEncoding tagencoding = TMR_GEN2_MILLER_M_8;

#if USE_TRANSPORT_LISTENER
  TMR_TransportListenerBlock tb;
#endif
  TMR_String model;
  char str[64];
  model.value = str;
  model.max = 64;
  if (argc < 2 )
  {
    usage();
  }
  for (i = 2; i < argc; i++)
  {
    if(0x00 == strcmp("--enable",argv[i]))
    {
      if(AutonomousMode[0] == '\0')
      {
        strcpy(AutonomousMode,"true");
      }
      else
      {
        fprintf(stdout, "Duplicate argument: --enable or --disable specified more than once\n");
        usage();
      }
    }
    else if(0x00 == strcmp("--disable",argv[i]))
    {
      if(AutonomousMode[0] == '\0')
      {
        strcpy(AutonomousMode,"false");
      }
      else
      {
        fprintf(stdout, "Duplicate argument: --enable or --disable specified more than once\n");
        usage();
      }
    }
    else
    {
      fprintf(stdout, "Argument %s is not recognized\n", argv[i]);
      usage();
    }
  }
  if(AutonomousMode[0] == '\0')
  {
    fprintf(stdout, "Not Providing any Autonomous Read Option\n");
    usage();
  }

  rp = &r;
  ret = TMR_create(rp, argv[1]);
  checkerr(rp, ret, 1, "creating reader");

#if USE_TRANSPORT_LISTENER

  if (TMR_READER_TYPE_SERIAL == rp->readerType)
  {
    tb.listener = serialPrinter;
  }
  else
  {
    tb.listener = stringPrinter;
  }
  tb.cookie = stdout;

  TMR_addTransportListener(rp, &tb);
#endif

  ret = TMR_connect(rp);
  checkerr(rp, ret, 1, "connecting reader");

  if(0x00 == strcmp(AutonomousMode,"false"))
  {
    ret = TMR_init_UserConfigOp(&config, TMR_USERCONFIG_CLEAR);
    checkerr(rp, ret, 1, "Initialization configuration: reset all saved configuration params");
    ret = TMR_paramSet(rp, TMR_PARAM_USER_CONFIG, &config);
    checkerr(rp, ret, 1, "setting user configuration option: reset all configuration parameters");
    printf("User config set option:reset all configuration parameters - autonomous read is disabled..\n");

  }
  else
  {
    region = TMR_REGION_NA3;
    ret = TMR_paramSet(rp, TMR_PARAM_REGION_ID, &region);
    checkerr(rp, ret, 1, "setting region");
    printf("region set to NA3\n");

    ret = TMR_paramSet(rp, TMR_PARAM_BAUDRATE, &baudrate);           
    checkerr(rp, ret, 1, "setting the BAUDRATE value");
    printf("baudrate set to 115200\n");

    ret = TMR_paramSet(rp, TMR_PARAM_RADIO_READPOWER, &power);
    checkerr(rp, ret, 1, "setting the READ POWER value");
    printf("readpower set to 2700\n");

    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_BLF, &freq);
    checkerr(rp, ret, 1, "setting the BLF value");
    printf("Gen2 BLF set to 250KHz\n");

    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_SESSION, &session);
    checkerr(rp, ret, 1, "setting the SESSION value");
    printf("Gen2 Session set to S0\n");

    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_TARGET, &target);
    checkerr(rp, ret, 1, "setting the TARGET value");
    printf("Gen2 Target set to A\n");

    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_TAGENCODING, &tagencoding);
    checkerr(rp, ret, 1, "setting the TAGENCODING value");
    printf("Gen2 TagEncoding set to M8\n");

    ret = TMR_paramSet(rp, TMR_PARAM_READ_ASYNCONTIME, &asyncOntime);
     checkerr(rp, ret, 1, "setting async on time");
    printf("AsyncOnTime set to 1000ms\n");

    ret = TMR_paramSet(rp, TMR_PARAM_TAGREADDATA_UNIQUEBYDATA, &uniquebydata);
    checkerr(rp, ret, 1, "setting uniquebydata value");
    printf("UniqueByData set to false\n");

    /* Read Plan */

    ret = TMR_RP_init_simple(&plan, antennaCount, antennaList, TMR_TAG_PROTOCOL_GEN2, 1000);
    checkerr(rp, ret, 1, "initializing simple readplan");

    /* Embedded Tagop */

    ret = TMR_TagOp_init_GEN2_ReadData(&op, TMR_GEN2_BANK_TID, 0x03, readLen);
    checkerr(rp, ret, 1, "creating tagop: GEN2 read data");
    ret = TMR_RP_set_tagop(&plan, &op);
    checkerr(rp, ret, 1, "setting tagop");

    /* The Reader Stats, Currently supporting only Temperature field */
#if 0/*  Change to "if (1)" to enable reader stats */
{
    TMR_Reader_StatsFlag setFlag = TMR_READER_STATS_FLAG_TEMPERATURE;

    /** request for the statics fields of your interest, before search */
    ret = TMR_paramSet(rp, TMR_PARAM_READER_STATS_ENABLE, &setFlag);
    checkerr(rp, ret, 1, "setting the  fields");
}
#endif

    /* Autonomous read */
    /* To disable autonomous read make enableAutonomousRead flag to false and do SAVEWITHRREADPLAN */

    ret = TMR_RP_set_enableAutonomousRead(&plan, true);
    checkerr(rp, ret, 1, "setting autonomous read");

    /* Commit read plan */
    ret = TMR_paramSet(rp, TMR_PARAM_READ_PLAN, &plan);
    checkerr(rp, ret, 1, "setting read plan");

    /* Init UserConfigOp structure to save read plan configuration */
    ret = TMR_init_UserConfigOp(&config, TMR_USERCONFIG_SAVE_WITH_READPLAN);
    checkerr(rp, ret, 1, "Initializing user configuration: save read plan configuration");
    ret = TMR_paramSet(rp, TMR_PARAM_USER_CONFIG, &config);
    checkerr(rp, ret, 1, "setting user configuration: save read plan configuration");
    printf("User config set option:save read plan configuration\n");

    /* Restore the save read plan configuration */
    /* Init UserConfigOp structure to Restore all saved configuration parameters */
    ret = TMR_init_UserConfigOp(&config, TMR_USERCONFIG_RESTORE);
    checkerr(rp, ret, 1, "Initialization configuration: restore all saved configuration params");
    ret = TMR_paramSet(rp, TMR_PARAM_USER_CONFIG, &config);
    checkerr(rp, ret, 1, "setting configuration: restore all saved configuration params");
    printf("User config set option:restore all saved configuration params\n");

    printf("Autonomous read is started ...\n");
    /**
     * Calling restore method will invoke the Autonomous read.'
     * Adding below some psudo code to show the serial read responses
     * coming from module.
     **/

    /* Extract Autonomous read responses */
#ifdef TMR_ENABLE_BACKGROUND_READS
{
    TMR_ReadListenerBlock rlb;
    TMR_StatsListenerBlock slb;

    rlb.listener = callback;
    rlb.cookie = NULL;

    slb.listener = statsCallback;
    slb.cookie = NULL;

    ret = TMR_addReadListener(rp, &rlb);
    checkerr(rp, ret, 1, "adding read listener");

    ret = TMR_addStatsListener(rp, &slb);
    checkerr(rp, ret, 1, "adding the stats listener");

    ret = TMR_receiveAutonomousReading(rp, NULL, NULL);
    checkerr(rp, ret, 1, "Autonomous reading");

#ifndef WIN32
    sleep(2);
#else
    Sleep(2000);
#endif

    /* remove the listener to stop receiving the tags */
    ret = TMR_removeReadListener(rp, &rlb);
    checkerr(rp, ret, 1, "remove read listener");

    /* remove the transport listener */
#if USE_TRANSPORT_LISTENER
    ret = TMR_removeTransportListener(rp, &tb);
    checkerr(rp, ret, 1, "remove transport listener");
#endif
}
#else
    /* code for non thread platform */
    TMR_TagReadData trd;

    TMR_TRD_init(&trd);
    ret = ret = TMR_receiveAutonomousReading(rp, NULL, NULL);
    checkerr(rp, ret, 1, "Autonomous reading");
#endif

 }

    TMR_destroy(rp);
    return 0;
}

void
callback(TMR_Reader *reader, const TMR_TagReadData *t, void *cookie)
{
  char epcStr[128];

  TMR_bytesToHex(t->tag.epc, t->tag.epcByteCount, epcStr);
  printf("Background read: %s\t antenna:%d\t readCount:%d\t rssi:%d\t frequency:%d\n", epcStr, t->antenna, t->readCount, t->rssi, t->frequency);
  if (0 < t->data.len)
  {
    char dataStr[255];
    TMR_bytesToHex(t->data.list, t->data.len, dataStr);
    printf("TID data(%d): %s\n", t->data.len, dataStr);
  }
}

void 
statsCallback (TMR_Reader *reader, const TMR_Reader_StatsValues* stats, void *cookie)
{
  /** Each  field should be validated before extracting the value */
  /** Currently supporting only temperature value */
  if (TMR_READER_STATS_FLAG_TEMPERATURE & stats->valid)
  {
    printf("Temperature %d(C)\n", stats->temperature);
  }
}
