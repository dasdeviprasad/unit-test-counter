/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
*   Copyright 2001  PhysioCom Systems,Inc.   All Rights Reserved.           *
*                                                                           *
****************************************************************************/

#define XX_I330C2 	 2
#define XX_SU1A	 	 104
#define XX_SU2R	 	 204
#define XX_CO2	 	 1202
#define XX_DSP12 	 1412

#define	_AudioON		0
#define _AudioA			1
#define _AudioB			2
#define _LightsOn		3
#define _Intensity		4
#define _FlashRate		5
#define _DutyCycle		6
#define _Phase			7
#define _Generator		8
#define _Ground			9
#define _PPGCIntensity		10
#define _PPGDIntensity		11
#define _OutputNumber		12
#define _OutputLevel		13
#define _InputNumber		14
#define _NotchXXhz		15

typedef struct ChannelConfig
{
  char		ch_enable;
  char  	ch_type;
  char		signal_name[80];
  char		channel_name[80];
  double	data_queue[1024];
  int		queue_size;
  int		write_index;
  long		queue_counter;
  int   	outputs;
  int		read_index[8];
  int   	queue_overflow[8];
  double	max_value;
  double	min_value;
  double	data_rate;
  void far	*input[8];
  void far	*e;
} CHNLCFG,	*CHNLCFGPTR;

typedef struct DeviceConfig
{
  int		handle;
  int		device;
  int		comport;
  int		irqlev;
  double	datarate;
  int		baseaddr;
  int		xtalfreq;
  int		smode;
  int		bauddvdr;
  int		acknnum;
  double	notchfreq;
  int		err_cs;   	//Check Summ
  int		err_lb;   	//Low Battery
  int		err_cl;   	//Command byte is lost
  int		err_co;		//Command buffer overflow
  int		err_dl;   	//ADC Data is lost
  int		err_bo;   	//C2 Data Buffer overflow
  int		err_of;   	//Driver Buffer overflow
  int		err_fr;		//Frame Error
  int		err_or;		//Comport overrun
  int		totalch;
  int		channel[24];
  int		type[24];
  int		fast[24];
  CHNLCFGPTR chcfgptr[24];
} DVCCFG, *DVCCFGPTR;

char*   (__cdecl *Initialize_XX)(DVCCFGPTR DeviceConfig);
char*   (__cdecl *Test_Settings)(DVCCFGPTR DeviceConfig);
void    (__cdecl *Update_Indexes)(int Handle);
void    (__cdecl *Close_XX)(int Handle);


void	(__cdecl *Set_File_Name)(char* name,int smode);
int		(__cdecl *Initialize)(int setup_num,int config_num);
char*	(__cdecl *Error_Message)(int error);
int		(__cdecl *Total_Channels)(int handle);
double  (__cdecl *Maximum_Value)(int handle, int signalnumber);
double  (__cdecl *Minimum_Value)(int handle, int signalnumber);
double  (__cdecl *Signal_Rate)(int handle, int signalnumber);
char*	(__cdecl *Signal_Name)(int handle, int signalnumber);
int		(__cdecl *Read_Buffer)(int handle, int signalnumber, double* buffer, int buffsize);
void	(__cdecl *Close)(int handle);
void	(__cdecl *Pause)(int handle);
double  (__cdecl *Send_Command)(int Handle, int Command, double Argument);
int 	(__cdecl *Read_DFT)(int handle, int signalnumber, double* buffer, int buffsize,int bins, double maxfreq, int windowlen);
int     (__cdecl *Is_Impedance_Mode)(int handle);
double	(__cdecl *Reset_CO2_T0_0)(int handle);
int		(__cdecl *Read__Buffer)(int handle, int signalnumber, int index, double* buffer, int buffsize);

DVCCFGPTR dc;
CHNLCFGPTR chptr[24];
