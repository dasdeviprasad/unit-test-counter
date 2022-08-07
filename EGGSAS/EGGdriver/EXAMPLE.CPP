/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
*   Copyright 2002  PhysioCom Systems,Inc.   All Rights Reserved.           *
*                                                                           *
****************************************************************************/


#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <winbase.h>
#include <string.h>
#include "driverh.h"

short ID[32];

int main(void)
{
HINSTANCE hDLL;     // Handle to DLL
int handle;	    // Handle or Error
double data[1024];
int i,num,total;

		
	// Loading the DLL

	hDLL = LoadLibrary("XXDRIVER.DLL");

	if(hDLL == NULL)
	{
		printf("Can't load DLL\n");	getch();
		return(1);
	}

	// Obtaining addresses of the functions
	Initialize     = (int   (__cdecl *)(int,int))GetProcAddress(hDLL,(LPCSTR)"Initialize");
	Error_Message  = (char* (__cdecl *)(int))GetProcAddress(hDLL,(LPCSTR)"Error_Message");
	Read_Buffer    = (int   (__cdecl *)(int,int,double*,int))GetProcAddress(hDLL,(LPCSTR)"Read_Buffer");
	Total_Channels = (int   (__cdecl *)(int))GetProcAddress(hDLL,(LPCSTR)"Total_Channels");
	Maximum_Value  = (double(__cdecl *)(int,int))GetProcAddress(hDLL,(LPCSTR)"Maximum_Value");
	Minimum_Value  = (double(__cdecl *)(int,int))GetProcAddress(hDLL,(LPCSTR)"Minimum_Value");
	Signal_Rate    = (double(__cdecl *)(int,int))GetProcAddress(hDLL,(LPCSTR)"Signal_Rate");
	Signal_Name    = (char* (__cdecl *)(int,int))GetProcAddress(hDLL,(LPCSTR)"Signal_Name");
	Close          = (void  (__cdecl *)(int))GetProcAddress(hDLL,(LPCSTR)"Close");
	Pause          = (void  (__cdecl *)(int))GetProcAddress(hDLL,(LPCSTR)"Pause");
	Send_Data	   = (int   (__cdecl *)(int,short*))GetProcAddress(hDLL,(LPCSTR)"Send_Data");
	Get_Data	   = (int   (__cdecl *)(int,short*))GetProcAddress(hDLL,(LPCSTR)"Get_Data");
	Send_Command   = (double(__cdecl *)(int,int,double))GetProcAddress(hDLL,(LPCSTR)"Send_Command");
   	
	if((Initialize == NULL)||
	(Error_Message == NULL)||
	(Read_Buffer == NULL)||
	(Total_Channels == NULL)||
	(Maximum_Value == NULL)||
	(Minimum_Value == NULL)||
	(Signal_Rate == NULL)||
	(Signal_Name == NULL)||
	(Close == NULL)||
	(Pause == NULL)||
	(Send_Data == NULL)||
	(Get_Data == NULL)||
	(Send_Command == NULL))
	{
	   printf("Can't find functions\n");getch();
	   FreeLibrary(hDLL);
       return(1);
	}

	//Configuration # 1
	handle = Initialize(1,1);
	if(handle < 0)
	{
	  puts(Error_Message(-handle));
	  FreeLibrary(hDLL);
	  printf("\n"); getch();
	  return(1);
	}

	//Read device ID number
	Get_Data(handle, ID);
	printf("OLD ID = %04hX,%04hX,%04hX,%04hX,%04hX,%04hX,%04hX,%04hX\n",ID[0],ID[1],ID[2],ID[3],ID[4],ID[5],ID[6],ID[7]);

	printf("Do you want new ID number? y/n\n");
	if(toupper(getch()) == 'Y')
	{
FILE *fp;
		if((fp = fopen("EGG_ID.txt","r+t")) != NULL) 
		{ 
			fscanf(fp,"%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x",&ID[0],&ID[1],&ID[2],&ID[3],&ID[4],&ID[5],&ID[6],&ID[7]);
			printf("NEW ID = %04hX,%04hX,%04hX,%04hX,%04hX,%04hX,%04hX,%04hX\n",ID[0],ID[1],ID[2],ID[3],ID[4],ID[5],ID[6],ID[7]);
			fclose(fp);
			ID[8] = 0xBF10;		//  WriteID PassWord 1
			ID[9] = 0x3D5C;		//  WriteID PassWord 2 
			//Write device ID number
			Send_Data(handle, ID);
		}
		else
		{
			printf("Unable to open file\n");
		}
		Close(handle);
		FreeLibrary(hDLL);
		getch();
		return(0);
	}


	total = Total_Channels(handle);
	if(total < 0)
	{
	  puts(Error_Message(-total)); printf("\n"); getch();
	  Close(handle);
	  FreeLibrary(hDLL);
	  return(1);
	}

	//Signal names
	for( i = 0 ; i < total ; ++i ) puts(Signal_Name(handle,i));
	//Minimum value of signal
	for( i = 0 ; i < total ; ++i ) printf("  %8.3f  ",Maximum_Value(handle,i));
	printf("\n");
	//Maximum value of signal
	for( i = 0 ; i < total ; ++i ) printf("  %8.3f  ",Minimum_Value(handle,i));
	printf("\n");
	//Data rate of signal
	for( i = 0 ; i < total ; ++i ) printf("  %8.3f  ",Signal_Rate(handle,i));
	printf("\n");


	//Main loop
	while(!kbhit())
	{
	  for( i = 0 ; i < total ; ++i )
	  {
	    //if we get a new data ?
	    num = Read_Buffer(handle,i,data,1024);
	    if(num > 0) printf("  %8.3f  ",data[num-1]);//Print only the last data
	    if(num == 0) printf("            ");		//Print nothing
        if(num < 0) { puts(Error_Message(-num)); printf("\n");}//Print error
	  }
	  printf("\r");
	  Sleep(4);//Don`t use Sleep() in your program !!!
	}

	printf("\n");
	//Stop the device
	Close(handle);

	getch();
//	getch();


	//Configuration # 2
	handle = Initialize(1,2);
	if(handle < 0)
	{
	  puts(Error_Message(-handle)); printf("\n"); getch();
	  FreeLibrary(hDLL);
	  return(1);
	}

	total = Total_Channels(handle);
	if(total < 0)
	{
	  puts(Error_Message(-total)); printf("\n"); getch();
	  Close(handle);
	  FreeLibrary(hDLL);
	  return(1);
	}

	//Signal names
	for( i = 0 ; i < total ; ++i ) puts(Signal_Name(handle,i));
	//Minimum value of signal
	for( i = 0 ; i < total ; ++i ) printf("  %8.3f  ",Maximum_Value(handle,i));
	printf("\n");
	//Maximum value of signal
	for( i = 0 ; i < total ; ++i ) printf("  %8.3f  ",Minimum_Value(handle,i));
	printf("\n");
	//Data rate of signal
	for( i = 0 ; i < total ; ++i ) printf("  %8.3f  ",Signal_Rate(handle,i));
	printf("\n");


	//Main loop
	while(!kbhit())
	{
	  for( i = 0 ; i < total ; ++i )
	  {
	    //if we get a new data ?
	    num = Read_Buffer(handle,i,data,1024);
	    if(num > 0) printf("  %8.3f  ",data[num-1]);//Print only the last data
	    if(num == 0) printf("            ");		//Print nothing
        if(num < 0) { puts(Error_Message(-num)); printf("\n");}//Print error
	  }
	  printf("\r");
	  Sleep(1);//Don`t use Sleep() in your program !!!
	}
	printf("\n");
	//Stop the device
	Close(handle);

	FreeLibrary(hDLL);

	getch();


    return(0);
}
