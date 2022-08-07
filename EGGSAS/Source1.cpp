void CReportView::vSet_graph_summ_characteristics(HWND hGraph, unsigned int iTitleResourceID)
  {
  CString cstrTitle;
  int iLineTypes[3], iTypes[11];
//  int iType;
  double dNullData, fYLoc[11], dValue;
//  double dAxisCntrl, dX;
  DWORD dwArray[3];
    // Specify the numbers at the tick marks for the Y-axis.
  char szText[] = "|L0 %\t|L10 %\t|L20 %\t|L30 %\t|L40 %\t|L50 %\t|L60 %\t|L70 %\t|L80 %\t|L90 %\t|L100 %";
  char szText1[] = "|hBL\t|h10\t|h20\t|h30";

  if(hGraph != NULL)
    {
  		// Make sure zeros are plotted
    dNullData = -.01F;
    PEvset(hGraph, PEP_fNULLDATAVALUEX, &dNullData, 1);
    PEvset(hGraph, PEP_fNULLDATAVALUE, &dNullData, 1);

      // Remove plotting method from pop-up menu.
    PEnset(hGraph, PEP_bALLOWPLOTCUSTOMIZATION, FALSE);

      // Give the graph a main title.
    cstrTitle.LoadString(iTitleResourceID);
    PEszset(hGraph, PEP_szMAINTITLE, (char *)(const char *)cstrTitle);
    PEszset(hGraph, PEP_szSUBTITLE, ""); // no subtitle
//    cstrTitle.LoadString(IDS_PERCENTAGE);
//    PEszset(hGraph, PEP_szYAXISLABEL, (char *)(const char *)cstrTitle); // Y axis label.
    cstrTitle.LoadString(IDS_PERIOD);
    PEszset(hGraph, PEP_szXAXISLABEL, (char *)(const char *)cstrTitle);  // X axis label.

    PEnset(hGraph, PEP_nFONTSIZE, PEFS_LARGE); // Set to large font size.

    PEnset(hGraph, PEP_bMARKDATAPOINTS, TRUE); // Mark the data points.
    PEnset(hGraph, PEP_nHOTSPOTSIZE, PEHSS_SMALL); // Set size of marked data points.

/****************
      // Manually Control X Axis Grid Line Density
    dAxisCntrl = 10.0f; // Multiples of grid tick marks where values are placed.
    PEvset(hGraph, PEP_fMANUALXAXISLINE, &dAxisCntrl, 1);
    dAxisCntrl = 10.0F; // Where grid tick marks are placed.
    PEvset(hGraph, PEP_fMANUALXAXISTICK, &dAxisCntrl, 1);
    PEnset(hGraph, PEP_bMANUALXAXISTICKNLINE, TRUE);
*******************/
      // Label the X axis for the period.
      // Remove everything on the X-axis.
//    PEnset(hGraph, PEP_nSHOWXAXIS, PESA_EMPTY);
    PEnset(hGraph, PEP_nSHOWXAXIS, PESA_LABELONLY); //PESA_AXISLABELS);
    cstrTitle.LoadString(IDS_PERIOD);
    PEszset(hGraph, PEP_szXAXISLABEL, (char *)(const char *)cstrTitle);  // X axis label.

/****************************************************** // Not using.
      // Set the minimum and maximum X-axis scale values.
    PEnset(hGraph, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
    dValue = (double)0.0;
    PEvset(hGraph, PEP_fMANUALMINX, &dValue, 1);
    dValue = (double)30.0;
    PEvset(hGraph, PEP_fMANUALMAXX, &dValue, 1);
    PEvsetcell(hGraph, PEP_szaPOINTLABELS, 0, "BL");
    PEvsetcell(hGraph, PEP_szaPOINTLABELS, 1, "10");
    PEvsetcell(hGraph, PEP_szaPOINTLABELS, 2, "20");
    PEvsetcell(hGraph, PEP_szaPOINTLABELS, 3, "30");
       PEnset(hGraph, PEP_bSIMPLEPOINTLEGEND, TRUE);
       PEnset(hGraph, PEP_bSIMPLELINELEGEND, TRUE);
       PEnset(hGraph, PEP_nLEGENDSTYLE, PELS_1_LINE);
       PEnset(hGraph, PEP_nGRIDLINECONTROL, PEGLC_NONE);
//int nArray[4];
//nArray[0] = 0;
//nArray[1] = 10;
//nArray[2] = 20;
//nArray[3] = 30;
//PEvset (hGraph, PEP_naALTFREQUENCIES, nArray, 4);
      // Specify type of tick mark.
    iTypes[0] = PELAT_GRIDTICK;
    iTypes[1] = PELAT_GRIDTICK;
    iTypes[2] = PELAT_GRIDTICK;
    iTypes[3] = PELAT_GRIDTICK;
    PEvset(hGraph, PEP_naVERTLINEANNOTATIONTYPE, iTypes, 4);
************************************************/

      // Set the minimum and maximum X-axis scale values.
    PEnset(hGraph, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
    dValue = (double)0.0;
    PEvset(hGraph, PEP_fMANUALMINX, &dValue, 1);
    dValue = (double)30.0;
    PEvset(hGraph, PEP_fMANUALMAXX, &dValue, 1);
      // Specify type of tick mark.
    iTypes[0] = PELAT_GRIDTICK;
    iTypes[1] = PELAT_GRIDTICK;
    iTypes[2] = PELAT_GRIDTICK;
    iTypes[3] = PELAT_GRIDTICK;
    PEvset(hGraph, PEP_naVERTLINEANNOTATIONTYPE, iTypes, 4);
      // Specify where on the Y-axis the tick marks go.
    fYLoc[0] = 0;
    fYLoc[1] = 10;
    fYLoc[2] = 20;
    fYLoc[3] = 30;
    PEvset(hGraph, PEP_faVERTLINEANNOTATION, fYLoc, 4);
      // Specify the numbers at the tick marks for the X-axis.
    PEvset(hGraph, PEP_szaVERTLINEANNOTATIONTEXT, szText1, 4);
      // Make the bottom margin wide enough to display the annotations.
    PEszset(hGraph, PEP_szBOTTOMMARGIN, "BL");
      // Set flag so the above data is used.
    PEnset(hGraph, PEP_bSHOWANNOTATIONS, TRUE);
    PEnset(hGraph, PEP_bSHOWVERTLINEANNOTATIONS, TRUE);

      // Label the Y axis using percentage.
      // Remove everything on the Y-axis.
    PEnset(hGraph, PEP_nSHOWYAXIS, PESA_EMPTY);
      // Set the minimum and maximum Y-axis scale values.
    PEnset(hGraph, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
    dValue = (double)0.0;
    PEvset(hGraph, PEP_fMANUALMINY, &dValue, 1);
    dValue = (double)100.0;
    PEvset(hGraph, PEP_fMANUALMAXY, &dValue, 1);

      // Specify type of tick mark.
    iTypes[0] = PELAT_GRIDTICK;
    iTypes[1] = PELAT_GRIDTICK;
    iTypes[2] = PELAT_GRIDTICK;
    iTypes[3] = PELAT_GRIDTICK;
    iTypes[4] = PELAT_GRIDTICK;
    iTypes[5] = PELAT_GRIDTICK;
    iTypes[6] = PELAT_GRIDTICK;
    iTypes[7] = PELAT_GRIDTICK;
    iTypes[8] = PELAT_GRIDTICK;
    iTypes[9] = PELAT_GRIDTICK;
    iTypes[10] = PELAT_GRIDTICK;
    PEvset(hGraph, PEP_naHORZLINEANNOTATIONTYPE, iTypes, 11);
      // Specify where on the Y-axis the tick marks go.
    fYLoc[0] = 0;
    fYLoc[1] = 10;
    fYLoc[2] = 20;
    fYLoc[3] = 30;
    fYLoc[4] = 40;
    fYLoc[5] = 50;
    fYLoc[6] = 60;
    fYLoc[7] = 70;
    fYLoc[8] = 80;
    fYLoc[9] = 90;
    fYLoc[10] = 100;
    PEvset(hGraph, PEP_faHORZLINEANNOTATION, fYLoc, 11);
      // Specify the numbers at the tick marks for the X-axis.
    PEvset(hGraph, PEP_szaHORZLINEANNOTATIONTEXT, szText, 11);
      // Set flag so the above data is used.
    PEnset(hGraph, PEP_bSHOWANNOTATIONS, TRUE);
      // Make the left margin wide enough to display the annotations.
    PEszset(hGraph, PEP_szLEFTMARGIN, "100%   ");

      // remove the legends for the subsets.
    int iTemp = -1;
    PEvsetcell(hGraph, PEP_naSUBSETSTOLEGEND, 0, &iTemp);

      // subset colors
    dwArray[0] = CONTROL_RANGE_COLOR; // High control
    dwArray[1] = PATIENT_COLOR; // Patient
    dwArray[2] = CONTROL_RANGE_COLOR; // Low control
    PEvset(hGraph, PEP_dwaSUBSETCOLORS, dwArray, 3);
		
      // subset line types
    iLineTypes[0] = PELT_DASH; // High control
    iLineTypes[1] = PELT_MEDIUMSOLID; //PELT_EXTRATHICKSOLID; //PELT_THICKSOLID; //PELT_MEDIUMSOLID; //PELT_THINSOLID; // Patient
    iLineTypes[2] = PELT_DASH; // Low control
    PEvset(hGraph, PEP_naSUBSETLINETYPES, iLineTypes, 3);

/*****************
      // Display "BL" for the baseline at the point 0.0. 
    PEnset(hGraph, PEP_bSHOWANNOTATIONS, TRUE);
    PEnset(hGraph, PEP_bSHOWVERTLINEANNOTATIONS, TRUE);
    dX = (double)0.0;
    PEvsetcell(hGraph, PEP_faVERTLINEANNOTATION, 0, &dX);
    iType = PELT_THINSOLID;
    PEvsetcell(hGraph, PEP_naVERTLINEANNOTATIONTYPE, 0, &iType);
//    PEvsetcell(hGraph, PEP_szaVERTLINEANNOTATIONTEXT, 0, "|h  \n      BL");
    cstrTitle.LoadString(IDS_BL);
    cstrTitle.Insert(0, "|h        ");
    PEvsetcell(hGraph, PEP_szaVERTLINEANNOTATIONTEXT, 0, (char *)(const char *)cstrTitle);
********************/
      // Increase line annotation text size
    PEnset(hGraph, PEP_nLINEANNOTATIONTEXTSIZE, 125); //150);
      // Don't draw any grid lines.
    PEnset(hGraph, PEP_nGRIDLINECONTROL, PEGLC_NONE);
    }
  }

