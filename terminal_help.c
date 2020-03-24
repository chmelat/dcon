/*
 *  Example commands
 */

#include <stdio.h>

void terminal_help(void)
{
  static char *msg[] = {
    "!AA\tdelimiter for valid response",
    "?AA\tdelimiter for invalid response\n",
    "%AANNTTCCFF(cr)\tSet module configuration",
    "  AA (range 00-FF) is 2-char. hex. address",
    "  NN is a new hexadecimal address",
    "  TT is the type (input range)",
    "  CC is a baud rate code",
    "  FF is a hex. number that eq. 8-bit parameter",
    "  7   6   5   4   3   2   1   0",
    "  it  ch  n   n   n   n   df  df ",
    "  it-integ. time 1-50Hz, 2-60Hz pow.; ch-checksum 0-dis.,1-enab.; n-not used",
    "  df-data format 00 engineering, 01 % of FSR, 10 two's comp. hex.",
    "  Example: (%01020F0680) Adr1->2; TC->K; 9600bps; 50Hz power, no chks\n",
    "$AA7CiRrr\tSets the single channel range configuration",
    "  7 command to set the channel range code",
    "  Ci i specifies the input channel to be set (0..n)",
    "  Rrr rr represents the type code of the channel to be set",
    "    0F: TC type K; 07: +4 to 20 mA; 08: +/- 10 V; 09: +/- 5 V",
    "  Example: (%017C0R07) Addres 1, channel 0 set to input 4 to 20 mA",
    "    -> !01\n",
    "$AA8Ci\tReads the single channel range configuration",
    "  -> !AACiRrr",
    "#AA\tRead the anolog inputs of all channels",
    "#AAN\tRead the analog input of the specific channel",
    "$AA0\tPerform a span calibration",
    "$AA1\tPerform a zero calibration",
    "$AA2\tRead the module configuration",
    "  -> !AATTCCFF",
    "$AA3\tReads the CJC temperature",
    "$AA5VV\tEnables/Disables the channel",
    "$AA6\tReads the channel Enable/Disable status",
    "  -> !AAVV",
    "$AA9\tReads the CJC Offset",
    "  -> !AA(Data)",
    "$AA9SNNNN\tSets the CJC Offset",
    "  -> !AA",
    "$AAA\tReads the Analog Inputs of All Channels in Hex Format",
    "$AAB\tReads the Channel Diagnostic Status",
    "  -> !AANN",
    "$AAF\tReads the firmware version",
    "  -> !AA(Data)",
    "$AAM\tReads the module name",
    "  -> !AA(Data)",
    "$AAP\tReads the protocol",
    "$AAPN\tSets the protocol",
    "$AAS0\tInternal calibration",
     0};
  char **p = msg;

  printf("Some commands of DCON protocol:\n");
  while (*p)
    printf("%s\n", *p++);

  printf("\n");
}
