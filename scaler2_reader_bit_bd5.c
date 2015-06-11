/* Chris Perkins - <cwperkins@lbl.gov> - 2011115
 *
 * Example reader for ScalerII Histogrammed Data Files
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

struct hist_hdr {
  unsigned int version;
  unsigned int bdnum;
  unsigned int runnum;
  unsigned int count;
  unsigned int num_words;
  unsigned int rs_count_lo;
  unsigned int rs_count_hi;
  unsigned int status1;
  unsigned int status2;
  unsigned int status3;
};

/* BBCpmt[0=east/1=west][chan]
 *  chan=0: PMT=9,  tile=10
 *  chan=1: PMT=11,  tile=12
 *  chan=2: PMT=14,  tile=16
 *  chan=3: PMT=16,  tile=18
 * 
 * VPDpmt[0=east/1=west][chan]
 *  chan=0: PMT=1
 *  chan=1: PMT=4
 *  chan=2: PMT=7
 *  chan=3: PMT=10
 *
 * ZDCsum[0=east/1=west][0=front/1=back]
 * BBCcoin
 */  

struct sca_val{
  int bx;                        // bunch crossing
  unsigned long long BBCpmt[2][4]; // (see comments above)
  unsigned long long VPDpmt[2][4]; // (see comments above)
  unsigned long long ZDCsum[2][2]; // (see comments above)
  unsigned long long BBCcoin;
  unsigned long long valSum;
};

enum EW{kEast,kWest};
enum BBCmap{kpmt9tile10,
            kpmt11tile12,
            kpmt14tile16,
            kpmt16tile18};
enum VPDmap{kpmt1,
            kpmt4,
            kpmt7,
            kpmt10};
enum ZDCmap{kFront,kBack};

int main(int argc, char *argv[]) {
  struct hist_hdr hist_hdr_l;
  char filename_l[100];
  int fd;
  int bytes_read;
  int i,j,k;
  unsigned int chn_cnt[3];
  unsigned long long channel;
  unsigned long long count;
  unsigned long long sum = 0;
  int debug_l = 0;

  char outfile[100];
  char outfilechan[100];
  FILE *fout;
  //FILE * foutchan;
  struct sca_val scaler[128];
  int bunch;
  int chnl_bbc;
  int chnl_zdc;
  int chnl_vpd;
  //  unsigned long long val[120];

  //Initialize scaler
  for(i=0;i<120;i++)
  {
    scaler[i].bx=i;

    for(j=0;j<2;j++)
    {
      for(k=0;k<4;k++)
      {
        scaler[i].BBCpmt[j][k]=0;
        scaler[i].VPDpmt[j][k]=0;
        if(k<2) scaler[i].ZDCsum[j][k]=0;
      };
    };
    scaler[i].BBCcoin=0;
    scaler[i].valSum=0;
  };

  printf("Usage: scaler2_reader_bd5 [filename] [debug]\n");

  if(argc > 1) strcpy(filename_l, argv[1]);
  else sprintf(filename_l, "out.dat");
  if(argc > 2) debug_l = atoi(argv[2]);
  printf("Opening datafile: %s\n", filename_l);

  // Open File
  fd = open(filename_l, O_RDONLY);
  if(fd == -1) {
    printf("Error opening datafile: %s\n", filename_l);
    return -1;
  }

  // Read Version
  bytes_read = read(fd, &hist_hdr_l, 4);
  if(hist_hdr_l.version == 1) {    // Data Format Version 1
    // Read Rest of Header
    bytes_read = read(fd, &(hist_hdr_l.bdnum), sizeof(struct hist_hdr) - 4);
    if(bytes_read != (sizeof(struct hist_hdr) - 4)) {
      printf("Error reading header\n");
      close(fd);
      return -1;
    }

    // Print Header
    printf("Runnumber = %8.8d, BoardNumber = %2.2d\n", hist_hdr_l.runnum, hist_hdr_l.bdnum);
    printf("  Data Format Version = %d, Count = %d\n", hist_hdr_l.version, hist_hdr_l.count);
    printf("  Num Data Words = %d (0x%8.8x)\n", hist_hdr_l.num_words, hist_hdr_l.num_words);
    printf("  Num Channels = %d (0x%8.8x),  RS Count = 0x%8.8x%8.8x\n", hist_hdr_l.num_words / 3, hist_hdr_l.num_words / 3, hist_hdr_l.rs_count_hi, hist_hdr_l.rs_count_lo);
    printf("  Status Words = 0x%8.8x  0x%8.8x  0x%8.8x\n", hist_hdr_l.status1, hist_hdr_l.status2, hist_hdr_l.status3);


    sprintf(outfile,"datfiles/run%d_%d.dat", hist_hdr_l.runnum,hist_hdr_l.bdnum);
    sprintf(outfilechan,"chanfiles/run%d_%d.dat", hist_hdr_l.runnum,hist_hdr_l.bdnum);
    fout=fopen(outfile,"w");
    //foutchan=fopen(outfilechan,"w");

    // Read Channels and Counts
    for(i=0; i < (hist_hdr_l.num_words / 3) ; i++) {
      bytes_read = read(fd, chn_cnt, 12);
      if(bytes_read != 12) {
        printf("Error reading Channel/Count\n");
        close(fd);
        printf("%d\n", i);
        return -1;
      }
      channel = ((unsigned long long)(chn_cnt[2]) << 16) | ((unsigned long long)(chn_cnt[1]) >> 16);
      count = (((unsigned long long)(chn_cnt[1]) & 0xffff) << 32) | (unsigned long long)(chn_cnt[0]);

      /*------Analyze Channel------*/
      //Read channel
      bunch = (int) (channel >> 25) & 0x7f;
      //printf("bx=%d\n",bunch);
      //printf("%d\n",bunch);
      if(bunch < 120)
      {
        if((channel >> 0) & 0x1) scaler[bunch].BBCpmt[kWest][kpmt9tile10] += count;
        if((channel >> 1) & 0x1) scaler[bunch].BBCpmt[kWest][kpmt11tile12] += count;
        if((channel >> 2) & 0x1) scaler[bunch].BBCpmt[kWest][kpmt14tile16] += count;
        if((channel >> 3) & 0x1) scaler[bunch].BBCpmt[kWest][kpmt16tile18] += count;

        if((channel >> 4) & 0x1) scaler[bunch].BBCpmt[kEast][kpmt9tile10] += count;
        if((channel >> 5) & 0x1) scaler[bunch].BBCpmt[kEast][kpmt11tile12] += count;
        if((channel >> 6) & 0x1) scaler[bunch].BBCpmt[kEast][kpmt14tile16] += count;
        if((channel >> 7) & 0x1) scaler[bunch].BBCpmt[kEast][kpmt16tile18] += count;

        if((channel >> 8) & 0x1) scaler[bunch].VPDpmt[kWest][kpmt1] += count;
        if((channel >> 9) & 0x1) scaler[bunch].VPDpmt[kWest][kpmt4] += count;
        if((channel >> 10) & 0x1) scaler[bunch].VPDpmt[kWest][kpmt7] += count;
        if((channel >> 11) & 0x1) scaler[bunch].VPDpmt[kWest][kpmt10] += count;

        if((channel >> 12) & 0x1) scaler[bunch].VPDpmt[kEast][kpmt1] += count;
        if((channel >> 13) & 0x1) scaler[bunch].VPDpmt[kEast][kpmt4] += count;
        if((channel >> 14) & 0x1) scaler[bunch].VPDpmt[kEast][kpmt7] += count;
        if((channel >> 15) & 0x1) scaler[bunch].VPDpmt[kEast][kpmt10] += count;

        if((channel >> 16) & 0x1) scaler[bunch].ZDCsum[kEast][kFront] += count;
        if((channel >> 17) & 0x1) scaler[bunch].ZDCsum[kWest][kFront] += count;
        if((channel >> 18) & 0x1) scaler[bunch].ZDCsum[kEast][kBack] += count;
        if((channel >> 19) & 0x1) scaler[bunch].ZDCsum[kWest][kBack] += count;

        if((channel >> 20) & 0x1) scaler[bunch].BBCcoin += count;
        scaler[bunch].valSum += count;

      }else
      {
        printf("Bunch Crossing %d Out of Range\n",bunch);
      }
      /*------Done------*/
      sum += count;
      if(debug_l) printf("Channel 0x%12.12llx = 0x%12.12llx Counts\n", channel, count);
    }
    printf("\n");
    printf("RS Count = 0x%8.8x%8.8x, Count Sum = 0x%16.16llx\n", hist_hdr_l.rs_count_hi, hist_hdr_l.rs_count_lo, sum);
  } else {
    printf("Unknown Data File Format Version: %d\n", hist_hdr_l.version);
  }

  close(fd);


  for(i=0;i<120;i++)
  {
    fprintf(fout, "%d ", scaler[i].bx);

    for(j=0; j<2; j++)
    {
      for(k=0; k<4; k++)
      {
        fprintf(fout,"%lld ",scaler[i].BBCpmt[j][k]);
      };
    };

    for(j=0; j<2; j++)
    {
      for(k=0; k<4; k++)
      {
        fprintf(fout,"%lld ",scaler[i].VPDpmt[j][k]);
      };
    };

    for(j=0; j<2; j++)
    {
      for(k=0; k<2; k++)
      {
        fprintf(fout,"%lld ",scaler[i].ZDCsum[j][k]);
      };
    };
    
    fprintf(fout,"%lld ",scaler[i].BBCcoin);
    fprintf(fout,"%lld\n",scaler[i].valSum);
  } 

  fclose(fout);
  //fclose(foutchan);

  return;
}
