/**
  @file flare.c

  @brief Subroutines that control the output of XUV flares.

  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))

  @date Mar 25 2016

*/

#include "vplanet.h"
#include <math.h>
#include <stdio.h>

void BodyCopyFlare(BODY *dest, BODY *src, int foo, int iNumBodies, int iBody) {
  dest[iBody].dFlareMinEnergy = src[iBody].dFlareMinEnergy;
  dest[iBody].dFlareMaxEnergy = src[iBody].dFlareMaxEnergy;
  dest[iBody].dLXUVFlareConst = src[iBody].dLXUVFlareConst;
  dest[iBody].dLXUVFlare      = src[iBody].dLXUVFlare;
  // dest[iBody].dLXUVFlareUpper  = src[iBody].dLXUVFlareUpper;
  // dest[iBody].dLXUVFlareLower  = src[iBody].dLXUVFlareLower;
  dest[iBody].iFlareFFD      = src[iBody].iFlareFFD;
  dest[iBody].iFlareBandPass = src[iBody].iFlareBandPass;
  // dest[iBody].iFlareSlopeUnits = src[iBody].iFlareSlopeUnits;
  dest[iBody].iEnergyBin = src[iBody].iEnergyBin;
  // dest[iBody].dFlareSlope      = src[iBody].dFlareSlope;
  //  dest[iBody].dFlareYInt       = src[iBody].dFlareYInt;
}

/**************** FLARE options ********************/

void ReadFlareEnergyBin(BODY *body,
                        CONTROL *control,
                        FILES *files,
                        OPTIONS *options,
                        SYSTEM *system,
                        int iFile) {
  // This parameter cannot exist in primary file
  int lTmp = -1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn,
                  options->cName,
                  &iTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].iEnergyBin = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].iEnergyBin = (int)options->dDefault;
}

void ReadFlareFFD(BODY *body,
                  CONTROL *control,
                  FILES *files,
                  OPTIONS *options,
                  SYSTEM *system,
                  int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,
                  options->cName,
                  cTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "da", 2)) {
      body[iFile - 1].iFlareFFD = FLARE_FFD_DAVENPORT;
    } else if (!memcmp(sLower(cTmp), "la", 2)) {
      body[iFile - 1].iFlareFFD = FLARE_FFD_LACY;
    } else if (!memcmp(sLower(cTmp), "no", 2)) {
      body[iFile - 1].iFlareFFD = FLARE_FFD_NONE;
    } else {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are DAVENPORT, "
                "LACY, or NONE.\n",
                options->cName,
                cTmp);
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].iFlareFFD = FLARE_FFD_DAVENPORT;
}

void ReadFlareBandPass(BODY *body,
                       CONTROL *control,
                       FILES *files,
                       OPTIONS *options,
                       SYSTEM *system,
                       int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,
                  options->cName,
                  cTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "ke", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_KEPLER;
    } else if (!memcmp(sLower(cTmp), "uv", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_UV;
    } else if (!memcmp(sLower(cTmp), "go", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_GOES;
    } else if (!memcmp(sLower(cTmp), "sr", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_SXR;
    } else if (!memcmp(sLower(cTmp), "te", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_TESS_UV;
    } else if (!memcmp(sLower(cTmp), "bo", 2)) {
      body[iFile - 1].iFlareBandPass = FLARE_BOLOMETRIC;
    } else {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are KEPLER, UV, "
                "GOES, TESSUV, BOLOMETRIC and SXR.\n",
                options->cName,
                cTmp);
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0)
    body[iFile - 1].iFlareBandPass = FLARE_KEPLER;
}


void ReadFlareMinEnergy(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp > 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareMinEnergy =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareMinEnergy =
            dTmp * fdUnitsEnergy(control->Units[iFile].iTime,
                                 control->Units[iFile].iMass,
                                 control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dFlareMinEnergy = options->dDefault;
    }
  }
}


void ReadFlareMaxEnergy(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareMaxEnergy =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareMaxEnergy =
            dTmp * fdUnitsEnergy(control->Units[iFile].iTime,
                                 control->Units[iFile].iMass,
                                 control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dFlareMaxEnergy = options->dDefault;
    }
  }
}

void ReadFlareYInt(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareYInt =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareYInt =
            dTmp / (fdUnitsTime(control->Units[iFile].iTime));
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dFlareYInt = options->dDefault;
    }
  }
}
// TODO: Include the error in the FFD slopes to calculate the upper and higher
// limit of XUV luminosity by flares
/*void ReadFlareYIntErrorUpper(BODY *body,
                           CONTROL *control,
                           FILES *files,
                           OPTIONS *options,
                           SYSTEM *system,
                           int iFile) {
// This parameter cannot exist in primary file
int lTmp = -1;
double dTmp;

AddOptionDouble(files->Infile[iFile].cIn,
                options->cName,
                &dTmp,
                &lTmp,
                control->Io.iVerbose);
if (lTmp >= 0) {
  NotPrimaryInput(iFile,
                  options->cName,
                  files->Infile[iFile].cIn,
                  lTmp,
                  control->Io.iVerbose);
  body[iFile - 1].dFlareYIntErrorUpper = dTmp;
  UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
} else if (iFile > 0)
  body[iFile - 1].dFlareYIntErrorUpper = options->dDefault;
}

void ReadFlareYIntErrorLower(BODY *body,
                           CONTROL *control,
                           FILES *files,
                           OPTIONS *options,
                           SYSTEM *system,
                           int iFile) {
// This parameter cannot exist in primary file
int lTmp = -1;
double dTmp;

AddOptionDouble(files->Infile[iFile].cIn,
                options->cName,
                &dTmp,
                &lTmp,
                control->Io.iVerbose);
if (lTmp >= 0) {
  NotPrimaryInput(iFile,
                  options->cName,
                  files->Infile[iFile].cIn,
                  lTmp,
                  control->Io.iVerbose);
  body[iFile - 1].dFlareYIntErrorLower = dTmp;
  UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
} else if (iFile > 0)
  body[iFile - 1].dFlareYIntErrorLower = options->dDefault;
}*/


void ReadFlareSlope(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dFlareSlope =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dFlareSlope =
            dTmp / (fdUnitsTime(control->Units[iFile].iTime) *
                    log10(fdUnitsEnergy(control->Units[iFile].iTime,
                                        control->Units[iFile].iMass,
                                        control->Units[iFile].iLength)));
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dFlareSlope = options->dDefault;
    }
  }
}


// TODO: Include the error in the FFD slopes to calculate the upper and higher
// limit of XUV luminosity by flares
/*void ReadFlareSlopeErrorUpper(BODY *body,
                            CONTROL *control,
                            FILES *files,
                            OPTIONS *options,
                            SYSTEM *system,
                            int iFile) {
// This parameter cannot exist in primary file
int lTmp = -1;
double dTmp;

AddOptionDouble(files->Infile[iFile].cIn,
                options->cName,
                &dTmp,
                &lTmp,
                control->Io.iVerbose);
if (lTmp >= 0) {
  NotPrimaryInput(iFile,
                  options->cName,
                  files->Infile[iFile].cIn,
                  lTmp,
                  control->Io.iVerbose);
  body[iFile - 1].dFlareSlopeErrorUpper = dTmp;
  UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
} else if (iFile > 0)
  body[iFile - 1].dFlareSlopeErrorUpper = options->dDefault;
}

void ReadFlareSlopeErrorLower(BODY *body,
                            CONTROL *control,
                            FILES *files,
                            OPTIONS *options,
                            SYSTEM *system,
                            int iFile) {
// This parameter cannot exist in primary file
int lTmp = -1;
double dTmp;

AddOptionDouble(files->Infile[iFile].cIn,
                options->cName,
                &dTmp,
                &lTmp,
                control->Io.iVerbose);
if (lTmp >= 0) {
  NotPrimaryInput(iFile,
                  options->cName,
                  files->Infile[iFile].cIn,
                  lTmp,
                  control->Io.iVerbose);
  body[iFile - 1].dFlareSlopeErrorLower = dTmp;
  UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
} else if (iFile > 0)
  body[iFile - 1].dFlareSlopeErrorLower = options->dDefault;
}*/


void ReadLXUVFlareConst(BODY *body,
                        CONTROL *control,
                        FILES *files,
                        OPTIONS *options,
                        SYSTEM *system,
                        int iFile) {
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,
                  options->cName,
                  &dTmp,
                  &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,
                    options->cName,
                    files->Infile[iFile].cIn,
                    lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dLXUVFlareConst =
            dTmp * dNegativeDouble(*options,
                                   files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dLXUVFlareConst =
            dTmp * fdUnitsEnergyFlux(control->Units[iFile].iTime,
                                     control->Units[iFile].iMass,
                                     control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dLXUVFlareConst = options->dDefault;
    }
  }
}

/* Initiatlize Input Options */

void InitializeOptionsFlare(OPTIONS *options, fnReadOption fnRead[]) {
  int iOpt, iFile;

  fvFormattedString(&options[OPT_FLAREYINT].cName, "dFlareYInt");
  fvFormattedString(&options[OPT_FLAREYINT].cDescr,
          "Y-Intercept for Flare Frequency"); // Where the curve intercepts the
                                              // y axis (y=ax+b, in this case,
                                              // the parameter is "b")
  fvFormattedString(&options[OPT_FLAREYINT].cDefault, "20.9 (Proxima Centauri)");
  options[OPT_FLAREYINT].dDefault   = 20.9;
  options[OPT_FLAREYINT].iType      = 2;
  options[OPT_FLAREYINT].bMultiFile = 1;
  options[OPT_FLAREYINT].dNeg       = 1.0 / DAYSEC;
  fvFormattedString(&options[OPT_FLAREYINT].cNeg, "1/day");
  fvFormattedString(&options[OPT_FLAREYINT].cDimension,"1/time");
  fnRead[OPT_FLAREYINT] = &ReadFlareYInt;
  fvFormattedString(&options[OPT_FLAREYINT].cLongDescr,
          " Y-Intercept for flare frequency distribution. Where the curve "
          "intercepts the"
          "y axis (y=ax+b, in this case, dFlareYInt = 'b'). This value is "
          "valid only for the "
          " energy range of the data fitted.\n");

  // TODO: Include the error in the FFD slopes to calculate the upper and higher
  // limit of XUV luminosity by flares
  /*fvFormattedString(options[OPT_FLAREYINTERRORUPPER].cName, "dFlareYIntErrorUpper");
  fvFormattedString(options[OPT_FLAREYINTERRORUPPER].cDescr, "Y-Intercept upper error");
  fvFormattedString(options[OPT_FLAREYINTERRORUPPER].cDefault, "0.0");
  options[OPT_FLAREYINTERRORUPPER].dDefault   = 0.0;
  options[OPT_FLAREYINTERRORUPPER].iType      = 2;
  options[OPT_FLAREYINTERRORUPPER].bMultiFile = 1;
  fnRead[OPT_FLAREYINTERRORUPPER]             = &ReadFlareYIntErrorUpper;

  fvFormattedString(options[OPT_FLAREYINTERRORLOWER].cName, "dFlareYIntErrorLower");
  fvFormattedString(options[OPT_FLAREYINTERRORLOWER].cDescr, "Y-Intercept lower error");
  fvFormattedString(options[OPT_FLAREYINTERRORLOWER].cDefault, "0.0");
  options[OPT_FLAREYINTERRORLOWER].dDefault   = 0.0;
  options[OPT_FLAREYINTERRORLOWER].iType      = 2;
  options[OPT_FLAREYINTERRORLOWER].bMultiFile = 1;
  fnRead[OPT_FLAREYINTERRORLOWER]             = &ReadFlareYIntErrorLower;
*/
  fvFormattedString(&options[OPT_FLARESLOPE].cName, "dFlareSlope");
  fvFormattedString(&options[OPT_FLARESLOPE].cDescr, "Slope for Flare Frequency");
  fvFormattedString(&options[OPT_FLARESLOPE].cDefault, "-0.68 (Proxima Centauri)");
  options[OPT_FLARESLOPE].dDefault   = -0.68;
  options[OPT_FLARESLOPE].iType      = 2;
  options[OPT_FLARESLOPE].bMultiFile = 1;
  options[OPT_FLARESLOPE].dNeg       = 1.0 / (DAYSEC * log10(1.0e7));
  fvFormattedString(&options[OPT_FLARESLOPE].cNeg, "1/day 1/log10(erg)");
  fvFormattedString(&options[OPT_FLARESLOPE].cDimension,"1/time/energy");
  fnRead[OPT_FLARESLOPE] = &ReadFlareSlope;
  fvFormattedString(
        &options[OPT_FLARESLOPE].cLongDescr,
        " Slope for flare frequency distribution. The user needs to"
        " input the module of the value for this parameter. The negative signal"
        " can be use only for input the value in flares/day 1/log10(erg).  \n");

  // TODO: Include the error in the FFD slopes to calculate the upper and higher
  // limit of XUV luminosity by flares
  /*fvFormattedString(options[OPT_FLARESLOPEERRORUPPER].cName, "dFlareSlopeErrorUpper");
  fvFormattedString(options[OPT_FLARESLOPEERRORUPPER].cDescr, "Slope upper error");
  fvFormattedString(options[OPT_FLARESLOPEERRORUPPER].cDefault, "0.0");
  options[OPT_FLARESLOPEERRORUPPER].dDefault   = 0.0;
  options[OPT_FLARESLOPEERRORUPPER].iType      = 2;
  options[OPT_FLARESLOPEERRORUPPER].bMultiFile = 1;
  fnRead[OPT_FLARESLOPEERRORUPPER]             = &ReadFlareSlopeErrorUpper;

  fvFormattedString(options[OPT_FLARESLOPEERRORLOWER].cName, "dFlareSlopeErrorLower");
  fvFormattedString(options[OPT_FLARESLOPEERRORLOWER].cDescr, "Slope lower error");
  fvFormattedString(options[OPT_FLARESLOPEERRORLOWER].cDefault, "0.0");
  options[OPT_FLARESLOPEERRORLOWER].dDefault   = 0.0;
  options[OPT_FLARESLOPEERRORLOWER].iType      = 2;
  options[OPT_FLARESLOPEERRORLOWER].bMultiFile = 1;
  fnRead[OPT_FLARESLOPEERRORLOWER]             = &ReadFlareSlopeErrorLower;
*/


  fvFormattedString(&options[OPT_FLAREMINENERGY].cName, "dFlareMinEnergy");
  fvFormattedString(&options[OPT_FLAREMINENERGY].cDescr,
          "Minimum Flare Energy to consider");
  fvFormattedString(&options[OPT_FLAREMINENERGY].cDefault, "10^26 J");
  options[OPT_FLAREMINENERGY].dDefault   = 1e26;
  options[OPT_FLAREMINENERGY].iType      = 2;
  options[OPT_FLAREMINENERGY].bMultiFile = 1;
  options[OPT_FLAREMINENERGY].dNeg       = 1e-7;
  fvFormattedString(&options[OPT_FLAREMINENERGY].cNeg, "ergs");
  fvFormattedString(&options[OPT_FLAREMINENERGY].cDimension, "energy");
  fnRead[OPT_FLAREMINENERGY] = &ReadFlareMinEnergy;

  fvFormattedString(&options[OPT_FLAREMAXENERGY].cName, "dFlareMaxEnergy");
  fvFormattedString(&options[OPT_FLAREMAXENERGY].cDescr,
          "Maximum Flare Energy to consider");
  fvFormattedString(&options[OPT_FLAREMAXENERGY].cDefault, "10^29 J");
  options[OPT_FLAREMAXENERGY].dDefault   = 1e29;
  options[OPT_FLAREMAXENERGY].iType      = 2;
  options[OPT_FLAREMAXENERGY].bMultiFile = 1;
  options[OPT_FLAREMAXENERGY].dNeg       = 1e-7;
  fvFormattedString(&options[OPT_FLAREMAXENERGY].cNeg, "ergs");
  fvFormattedString(&options[OPT_FLAREMAXENERGY].cDimension, "energy");
  fnRead[OPT_FLAREMAXENERGY] = &ReadFlareMaxEnergy;

  fvFormattedString(&options[OPT_LXUVFLARECONST].cName, "dLXUVFlareConst");
  fvFormattedString(&options[OPT_LXUVFLARECONST].cDescr, "XUV luminosity of flares");
  fvFormattedString(&options[OPT_LXUVFLARECONST].cDefault, "10^22 Watts or 10^29 erg/s");
  options[OPT_LXUVFLARECONST].dDefault   = 1e22;
  options[OPT_LXUVFLARECONST].iType      = 2;
  options[OPT_LXUVFLARECONST].bMultiFile = 1;
  options[OPT_LXUVFLARECONST].dNeg       = LSUN;
  fvFormattedString(&options[OPT_LXUVFLARECONST].cNeg, "LSUN");
  fvFormattedString(&options[OPT_LXUVFLARECONST].cDimension, "energy/time");
  fnRead[OPT_LXUVFLARECONST] = &ReadLXUVFlareConst;

  // XXX Change to iEnergyBin for next major release
  fvFormattedString(&options[OPT_FLAREENERGYBIN].cName, "dEnergyBin");
  fvFormattedString(&options[OPT_FLAREENERGYBIN].cDescr,
          "Number of energies consider between the minimum and maximum "
          "energies to calculate the luminosity by flares");
  fvFormattedString(&options[OPT_FLAREENERGYBIN].cDefault,
          "100 energies between dFlareMinEnergy and dFlareMaxEnergy");
  options[OPT_FLAREENERGYBIN].dDefault   = 100;
  options[OPT_FLAREENERGYBIN].iType      = 1;
  options[OPT_FLAREENERGYBIN].bMultiFile = 1;
  options[OPT_FLAREENERGYBIN].dNeg       = 1;
  fvFormattedString(&options[OPT_FLAREENERGYBIN].cNeg, "None");
  fvFormattedString(&options[OPT_FLAREENERGYBIN].cDimension,"energy");
  fnRead[OPT_FLAREENERGYBIN] = &ReadFlareEnergyBin;

  fvFormattedString(&options[OPT_FLAREFFD].cName, "sFlareFFD");
  fvFormattedString(&options[OPT_FLAREFFD].cDescr, "Modes of calculate the FFD");
  fvFormattedString(&options[OPT_FLAREFFD].cDefault, "DAVENPORT");
  fvFormattedString(&options[OPT_FLAREFFD].cValues, "DAVENPORT LACY NONE");
  options[OPT_FLAREFFD].iType      = 3;
  options[OPT_FLAREFFD].bMultiFile = 1;
  fnRead[OPT_FLAREFFD]             = &ReadFlareFFD;
  fvFormattedString(&options[OPT_FLAREFFD].cLongDescr,
          " If DAVENPORT is selected, the code will employ the model\n"
          "fro Davenport et al.(2019) the user have\n"
          "to give the mass and Stellar age, dMass and dAge, as well\n"
          "the maximum and minimum energy, dFlareMinEnergy and \n"
          "dFlareMaxEnergy, to calculate the linear (Y-intercept) \n"
          "and angular (slope) coeficients, dA and dB of the flare \n"
          "frequency distribution (FFD), and then the FFD in this \n"
          "energy range. If Lacy is selected, the code will employ \n"
          "the model of Lacy et al. (1976), and the \n"
          "user have to give the same input of the DAVENPORT's mode,\n"
          "plus the linear and angular coeficients, dFlareSlope and \n"
          "dFlareYInt of the FFD. If NONE is selected, the code will\n"
          " use the XUV luminosity given by the user in the input \n"
          "file (dLXUVFlareConst). \n");

  fvFormattedString(&options[OPT_FLAREBANDPASS].cName, "sFlareBandPass");
  fvFormattedString(&options[OPT_FLAREBANDPASS].cDescr,
          "Options of band pass of the input energy of flares");
  fvFormattedString(&options[OPT_FLAREBANDPASS].cDefault, "KEPLER");
  fvFormattedString(&options[OPT_FLAREBANDPASS].cValues,
          "KEPLER UV GOES SXR BOLOMETRIC TESSUV");
  options[OPT_FLAREBANDPASS].iType      = 3;
  options[OPT_FLAREBANDPASS].bMultiFile = 1;
  fnRead[OPT_FLAREBANDPASS]             = &ReadFlareBandPass;
  fvFormattedString(&options[OPT_FLAREBANDPASS].cLongDescr,
          /*
          "If UV or GOES is selected, the code will convert \n"
          "the input energy of flares from the UV band \n"
          "(3000-4300A) or GOES band (1-8A) to the Kepler band \n"
          "(4000-9000A) to calculate the FFD and the SXR \n"
          "band (1.24 - 1239.85A) to calculate the luminosity.\n"
          " If SXR is selected, the code will convert the \n"
          "input energy of flares from the SXR band to the Kepler\n"
          "band to calculate the FFD and will use the same \n"
          "value of energy to calculate the luminosity .\n"
          "If KEPLER is selected, the code will use the same\n"
          "input energy to calculate the FFD and will convert \n"
          "the input energy to SXR band to calculate the \n"
          "luminosity. The conversion values are taken from \n"
          "Osten and Wolk (2015) (doi:10.1088/0004-637X/809/1/79).\n"
          "If TESSUV is selected, then the code will convert\n"
          "the input energy of flares using the conversion value\n"
          "to the band U (2000-2800A) to the TESS data from \n"
          "Gunther et al 2020 (https://doi.org/10.3847/1538-3881/ab5d3a). \n"
          "If the BOLOMETRIC its selected, the code will convert the input \n"
          "energy of flares using the conversion values are taken \n"
          "from Osten and Wolk (2015) (doi:10.1088/0004-637X/809/1/79).\n"
          */
          "Currently suppressed due to execution errors."
  );
  /*
    fvFormattedString(options[OPT_FLARESLOPEUNITS].cName, "sFlareSlopeUnits");
    fvFormattedString(options[OPT_FLARESLOPEUNITS].cDescr,
            "Options to the units of the FFD in which the FFD slopes are
    define"); fvFormattedString(options[OPT_FLARESLOPEUNITS].cDefault, "DAY");
    fvFormattedString(options[OPT_FLARESLOPEUNITS].cValues, "SEC MIN HOUR DAY");
    options[OPT_FLARESLOPEUNITS].iType      = 3;
    options[OPT_FLARESLOPEUNITS].bMultiFile = 1;
    fnRead[OPT_FLARESLOPEUNITS]             = &ReadFlareSlopeUnits;
    fvFormattedString(options[OPT_FLARESLOPEUNITS].cLongDescr,
            " This option allow the user input the units for the inputed FFD "
            "slopes. The options are\n"
            " SEC, for input slopes with FFD in number of flares per seconds, "
            "MIN for number of flares per minutes, \n"
            " HOUR for number of flares per hour, and DAY number of flares per "
            "for days.\n");*/
}

void ReadOptionsFlare(BODY *body,
                      CONTROL *control,
                      FILES *files,
                      OPTIONS *options,
                      SYSTEM *system,
                      fnReadOption fnRead[],
                      int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTFLARE; iOpt < OPTENDFLARE; iOpt++) {
    if (options[iOpt].iType != -1)
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
  }
}

/******************* Verify FLARE ******************/

void PropsAuxFlare(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                   int iBody) {
  /* Nothing */
}


void VerifyFlareFFD(BODY *body,
                    CONTROL *control,
                    OPTIONS *options,
                    UPDATE *update,
                    double dAge,
                    int iBody) {

  // Assign luminosity
  if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) {
    body[iBody].dLXUVFlare =
          fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
    if (options[OPT_FLAREFFD].iLine[iBody + 1] >= 0) {
      if (control->Io.iVerbose >= VERBINPUT)
        printf("INFO: The FFD will follow Davenport et. al, 2019 model.\n");
    }
  } else if (body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    body[iBody].dLXUVFlare =
          fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
    if (options[OPT_FLAREFFD].iLine[iBody + 1] >= 0) {
      if (control->Io.iVerbose >= VERBINPUT)
        printf("INFO: The FFD will remain constant during all the "
               "simulation.\n");
    }
  } else if (body[iBody].iFlareFFD == FLARE_FFD_NONE) {
    body[iBody].dLXUVFlare = body[iBody].dLXUVFlareConst;
    if (options[OPT_FLAREFFD].iLine[iBody + 1] >= 0) {
      if (control->Io.iVerbose >= VERBINPUT)
        printf("INFO: The FFD will remain constant during all the "
               "simulation.\n");
    }
  }
}


// void VerifyEnergyBin(BODY *body, OPTIONS *options, UPDATE *update, int iBody)
// {

// This may become useful once flare evolution is included
/* update[iBody].iaType[update[iBody].iEnergyBin][0]     = 1;
 update[iBody].iNumBodies[update[iBody].iEnergyBin][0] = 1;
 update[iBody].iaBody[update[iBody].iEnergyBin][0]     = malloc(
           update[iBody].iNumBodies[update[iBody].iEnergyBin][0] * sizeof(int));
 update[iBody].iaBody[update[iBody].iEnergyBin][0][0] = iBody;
 update[iBody].pdDEnergyBinDt =
       &update[iBody].daDerivProc[update[iBody].iEnergyBin][0];*/
//}


void VerifyLXUVFlare(BODY *body, OPTIONS *options, UPDATE *update, int iBody) {

  // This may become useful once flare evolution is included
  /*update[iBody].iaType[update[iBody].iLXUVFlare][0]     = 1;
  update[iBody].iNumBodies[update[iBody].iLXUVFlare][0] = 1;
  update[iBody].iaBody[update[iBody].iLXUVFlare][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iLXUVFlare][0] *
  sizeof(int)); update[iBody].iaBody[update[iBody].iLXUVFlare][0][0] = iBody;
  update[iBody].pdDLXUVFlareDt =
        // update[iBody].fdLXUVFlare =
        &update[iBody].daDerivProc[update[iBody].iLXUVFlare][0];*/
}


void AssignFlareDerivatives(BODY *body,
                            EVOLVE *evolve,
                            UPDATE *update,
                            fnUpdateVariable ***fnUpdate,
                            int iBody) {
  /* No derivatives yet for flare.
  //This may become useful once flare evolution is included*/
  // fnUpdate[iBody][update[iBody].iLXUVFlare][0] = &fdLXUVFlare;
  // fnUpdate[iBody][update[iBody].iEnergyBin][0] = &pdDEnergyBinDt; // NOTE:
  // This points to the value of the EnergyBin!
}

void NullFlareDerivatives(BODY *body,
                          EVOLVE *evolve,
                          UPDATE *update,
                          fnUpdateVariable ***fnUpdate,
                          int iBody) {
  /* No derivatives yet for flare.
  This may become useful once flare evolution is included*/
  fnUpdate[iBody][update[iBody].iLXUV][0] = &fndUpdateFunctionTiny;
  // fnUpdate[iBody][update[iBody].iLXUVFlare][0] =
  //       &fndUpdateFunctionTiny; // NOTE: This points to the value of the
  //  LXUVFlare!
  // fnUpdate[iBody][update[iBody].iEnergyBin][0] =
  //       &fndUpdateFunctionTiny; // NOTE: This points to the value of the
  //  EnergyBin!
}

void VerifyFlare(BODY *body,
                 CONTROL *control,
                 FILES *files,
                 OPTIONS *options,
                 OUTPUT *output,
                 SYSTEM *system,
                 UPDATE *update,
                 int iBody,
                 int iModule) {
  /* Mass must be in proper range */

  if (body[iBody].dMass < MINMASSFLARE * MSUN ||
      body[iBody].dMass > MAXMASSFLARE * MSUN) {
    fprintf(stderr,
            "ERROR: Mass of %s must be between %.3lf and %.3lf\n",
            body[iBody].cName,
            MINMASSFLARE,
            MAXMASSFLARE);
    LineExit(files->Infile[iBody + 1].cIn, options[OPT_MASS].iLine[iBody + 1]);
  }

  if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT ||
      body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    if (body[iBody].dAge <= 0) {
      fprintf(stderr,
              "ERROR: %s must be > 0 for flare FFD calculations.\n",
              options[OPT_AGE].cName);
      LineExit(files->Infile[iBody + 1].cIn, options[OPT_AGE].iLine[iBody + 1]);
    }

    if (body[iBody].iEnergyBin <= 0) {
      fprintf(stderr,
              "ERROR: %s must be > 0 for flare FFD calculations.\n",
              options[OPT_FLAREENERGYBIN].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_FLAREENERGYBIN].iLine[iBody + 1]);
    }

    if (body[iBody].dFlareMinEnergy <= 0 || body[iBody].dFlareMaxEnergy <= 0) {
      fprintf(stderr,
              "ERROR: %s and %s must be > 0.\n",
              options[OPT_FLAREMINENERGY].cName,
              options[OPT_FLAREMAXENERGY].cName);
      if (options[OPT_FLAREMINENERGY].iLine[iBody + 1] >= 0) {
        LineExit(files->Infile[iBody + 1].cIn,
                 options[OPT_FLAREMINENERGY].iLine[iBody + 1]);
      }
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_FLAREMAXENERGY].iLine[iBody + 1]);
    }

    if (body[iBody].dFlareMinEnergy >= body[iBody].dFlareMaxEnergy) {
      fprintf(stderr,
              "ERROR: %s must be smaller than %s.\n",
              options[OPT_FLAREMINENERGY].cName,
              options[OPT_FLAREMAXENERGY].cName);
      LineExit(files->Infile[iBody + 1].cIn,
               options[OPT_FLAREMAXENERGY].iLine[iBody + 1]);
    }
  }


  /* If the XUV luminosity by flares is given by the user, the code do not allow
  output FFD and flare energies because the code do not calculate: the user
  already inputed the XUV luminosity by flares and the flare module doesn't need
  this information anymore*/
  if (body[iBody].iFlareFFD == FLARE_FFD_NONE) {
    int iCol;
    for (iCol = 0; iCol < files->Outfile[iBody].iNumCols; iCol++) {
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ1].cName,
                 strlen(output[OUT_FLAREFREQ1].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ1].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ2].cName,
                 strlen(output[OUT_FLAREFREQ2].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ2].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ3].cName,
                 strlen(output[OUT_FLAREFREQ3].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ3].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQ4].cName,
                 strlen(output[OUT_FLAREFREQ4].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQ4].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQMIN].cName,
                 strlen(output[OUT_FLAREFREQMIN].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQMIN].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQMID].cName,
                 strlen(output[OUT_FLAREFREQMID].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQMID].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREFREQMAX].cName,
                 strlen(output[OUT_FLAREFREQMAX].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREFREQMAX].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY1].cName,
                 strlen(output[OUT_FLAREENERGY1].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY1].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY2].cName,
                 strlen(output[OUT_FLAREENERGY2].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY2].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY3].cName,
                 strlen(output[OUT_FLAREENERGY3].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY3].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGY4].cName,
                 strlen(output[OUT_FLAREENERGY4].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGY4].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGYMIN].cName,
                 strlen(output[OUT_FLAREENERGYMIN].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGYMIN].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGYMID].cName,
                 strlen(output[OUT_FLAREENERGYMID].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGYMID].cName);
      }
      if (memcmp(files->Outfile[iBody].caCol[iCol],
                 output[OUT_FLAREENERGYMAX].cName,
                 strlen(output[OUT_FLAREENERGYMAX].cName)) == 0) {
        /* Match! */
        fprintf(stderr,
                "WARNING: Output option %s only allowed with FFD model "
                "DAVENPORT or LACY \n",
                output[OUT_FLAREENERGYMAX].cName);
      }
    }
  }

  /* if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) || (body[iBody].iFlareFFD
   == FLARE_FFD_LACY) { if (body[iBody].dFlareMinEnergy = 0) { fprintf(stderr,
   "ERROR: %s must be >= 0. ", input[OPT_FLAREMINENERGY].cName);
       }
   }*/


  VerifyLXUVFlare(body, options, update, iBody);
  control->fnForceBehavior[iBody][iModule]   = &fnForceBehaviorFlare;
  control->fnPropsAux[iBody][iModule]        = &PropsAuxFlare;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyFlare;
}


void fnForceBehaviorFlare(BODY *body,
                          MODULE *module,
                          EVOLVE *evolve,
                          IO *io,
                          SYSTEM *system,
                          UPDATE *update,
                          fnUpdateVariable ***fnUpdate,
                          int iBody,
                          int iModule) {

  if (body[iBody].dLXUVFlare < 0)
    body[iBody].dLXUVFlare = 0;
  else
    body[iBody].dLXUVFlare = fdLXUVFlare(body, evolve->dTimeStep, iBody);
}


void InitializeBodyFlare(BODY *body, CONTROL *control, UPDATE *update,
                         int iBody, int iModule) {
  // double *daEnergyERGXUV, *daLXUVFlare, *daFFD, *daEnergyJOUXUV, *daLogEner;
  // double *daLogEnerXUV, *daEnergyERG, *daEnergyJOU, *daEnerJOU;
  body[iBody].daEnergyERGXUV = malloc((body[iBody].iEnergyBin+1) * sizeof(double));
  body[iBody].daLXUVFlare    = malloc((body[iBody].iEnergyBin+1) * sizeof(double));
  body[iBody].daEnergyJOUXUV = malloc((body[iBody].iEnergyBin+1) * sizeof(double));
  body[iBody].daFFD          = malloc((body[iBody].iEnergyBin+1) * sizeof(double));
  body[iBody].daLogEner      = malloc((body[iBody].iEnergyBin+1) * sizeof(double));
  body[iBody].daLogEnerXUV   = malloc((body[iBody].iEnergyBin+1) * sizeof(double));
  body[iBody].daEnergyERG    = malloc((body[iBody].iEnergyBin+1) * sizeof(double));
  body[iBody].daEnergyJOU    = malloc((body[iBody].iEnergyBin+1) * sizeof(double));
  body[iBody].daEnerJOU      = malloc((body[iBody].iEnergyBin+1) * sizeof(double));

  body[iBody].dLXUVFlare = fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
}

void InitializeModuleFlare(CONTROL *control, MODULE *module) {
  /* Anything Here? */
}

/**************** FLARE update ****************/

void InitializeUpdateFlare(BODY *body, UPDATE *update, int iBody) {
  /* STELLAR calculates LXUV from the star's properties, but FLARE calculates
   * LXUV as a primary variable. It is the only possible update. */

  /* No primary variables for FLARE yet
  update[iBody].iNumVars++;
  update[iBody].iNumLXUV++;
  */
}

void FinalizeUpdateLXUVFlare(BODY *body, UPDATE *update, int *iEqn, int iVar,
                             int iBody, int iFoo) {
  /* No primary variables for FLARE yet*/
  update[iBody].iaModule[iVar][*iEqn] = FLARE;
  update[iBody].iNumLXUV              = (*iEqn)++;
}

// TODO: Maybe add this to each FlareFreq output? Energies do not evolve, so do
// not need this.
/*void FinalizeUpdateFlareFreqMax(BODY *body, UPDATE *update, int *iEqn, int
  iVar, int iBody, int iFoo) {
  // No primary variables for FLARE yet
  //  update[iBody].iaModule[iVar][*iEqn] = FLARE;
  update[iBody].iNumFlareFreqMax              = (*iEqn)++;
}
*/

/***************** FLARE Halts *****************/

/* Halt for massive flare? No Flares? */

void CountHaltsFlare(HALT *halt, int *iNumHalts) {
}

void VerifyHaltFlare(BODY *body, CONTROL *control, OPTIONS *options, int iBody,
                     int *iHalt) {
}

/************* FLARE Outputs ******************/

/* NOTE: If you write a new Write subroutine here you need to add the associate
   block of initialization in InitializeOutputFlare below */

void WriteLXUVFlare(BODY *body,
                    CONTROL *control,
                    OUTPUT *output,
                    SYSTEM *system,
                    UNITS *units,
                    UPDATE *update,
                    int iBody,
                    double *dTmp,
                    char **cUnit) {
  *dTmp = fdLXUVFlare(body, control->Evolve.dTimeStep, iBody);
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}

// TODO: Include the error in the FFD slopes to calculate the upper and higher
// limit of XUV luminosity by flares
/*
void WriteLXUVFlareUpper(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char cUnit[]) {
  *dTmp = body[iBody].dLXUVFlareUpper;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}


void WriteLXUVFlareLower(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char **cUnit) {
  *dTmp = body[iBody].dLXUVFlareLower;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergyFlux(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergyFlux(units, cUnit);
  }
}
*/
void WriteFlareFreq1(BODY *body,
                     CONTROL *control,
                     OUTPUT *output,
                     SYSTEM *system,
                     UNITS *units,
                     UPDATE *update,
                     int iBody,
                     double *dTmp,
                     char **cUnit) {
  *dTmp = body[iBody].dFlareFreq1;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreq2(BODY *body,
                     CONTROL *control,
                     OUTPUT *output,
                     SYSTEM *system,
                     UNITS *units,
                     UPDATE *update,
                     int iBody,
                     double *dTmp,
                     char **cUnit) {
  *dTmp = body[iBody].dFlareFreq2;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreq3(BODY *body,
                     CONTROL *control,
                     OUTPUT *output,
                     SYSTEM *system,
                     UNITS *units,
                     UPDATE *update,
                     int iBody,
                     double *dTmp,
                     char **cUnit) {
  *dTmp = body[iBody].dFlareFreq3;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreq4(BODY *body,
                     CONTROL *control,
                     OUTPUT *output,
                     SYSTEM *system,
                     UNITS *units,
                     UPDATE *update,
                     int iBody,
                     double *dTmp,
                     char **cUnit) {
  *dTmp = body[iBody].dFlareFreq4;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreqMin(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char **cUnit) {
  *dTmp = body[iBody].dFlareFreqMin;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreqMid(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char **cUnit) {
  *dTmp = body[iBody].dFlareFreqMid;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareFreqMax(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char **cUnit) {
  *dTmp = body[iBody].dFlareFreqMax;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp *= fdUnitsTime(units->iTime);
    fsUnitsRate(units->iTime, cUnit);
  }
}
void WriteFlareEnergy1(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char **cUnit) {
  *dTmp = body[iBody].dFlareEnergy1;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFlareEnergy2(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char **cUnit) {
  *dTmp = body[iBody].dFlareEnergy2;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFlareEnergy3(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char **cUnit) {
  *dTmp = body[iBody].dFlareEnergy3;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFlareEnergy4(BODY *body,
                       CONTROL *control,
                       OUTPUT *output,
                       SYSTEM *system,
                       UNITS *units,
                       UPDATE *update,
                       int iBody,
                       double *dTmp,
                       char **cUnit) {
  *dTmp = body[iBody].dFlareEnergy4;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}
void WriteFlareEnergyMin(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char **cUnit) {
  *dTmp = body[iBody].dFlareEnergyMin;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}
void WriteFlareEnergyMid(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char **cUnit) {
  *dTmp = body[iBody].dFlareEnergyMid;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}

void WriteFlareEnergyMax(BODY *body,
                         CONTROL *control,
                         OUTPUT *output,
                         SYSTEM *system,
                         UNITS *units,
                         UPDATE *update,
                         int iBody,
                         double *dTmp,
                         char **cUnit) {
  *dTmp = body[iBody].dFlareEnergyMax;
  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    fvFormattedString(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsEnergy(units->iTime, units->iMass, units->iLength);
    fsUnitsEnergy(units, cUnit);
  }
}
void InitializeOutputFlare(OUTPUT *output, fnWriteOutput fnWrite[]) {

  fvFormattedString(&output[OUT_FLAREFREQ1].cName, "FlareFreq1");
  fvFormattedString(&output[OUT_FLAREFREQ1].cDescr,
          "First value of flare frequency range");
  fvFormattedString(&output[OUT_FLAREFREQ1].cNeg, "/day");
  output[OUT_FLAREFREQ1].bNeg       = 1;
  output[OUT_FLAREFREQ1].dNeg       = DAYSEC;
  output[OUT_FLAREFREQ1].iNum       = 1;
  output[OUT_FLAREFREQ1].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ1]           = &WriteFlareFreq1;

  fvFormattedString(&output[OUT_FLAREFREQ2].cName, "FlareFreq2");
  fvFormattedString(&output[OUT_FLAREFREQ2].cDescr,
          "Second value of flare frequency range");
  fvFormattedString(&output[OUT_FLAREFREQ2].cNeg, "/day");
  output[OUT_FLAREFREQ2].bNeg       = 1;
  output[OUT_FLAREFREQ2].dNeg       = DAYSEC;
  output[OUT_FLAREFREQ2].iNum       = 1;
  output[OUT_FLAREFREQ2].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ2]           = &WriteFlareFreq2;

  fvFormattedString(&output[OUT_FLAREFREQ3].cName, "FlareFreq3");
  fvFormattedString(&output[OUT_FLAREFREQ3].cDescr,
          "Third value of flare frequency range");
  fvFormattedString(&output[OUT_FLAREFREQ3].cNeg, "/day");
  output[OUT_FLAREFREQ3].bNeg       = 1;
  output[OUT_FLAREFREQ3].dNeg       = DAYSEC;
  output[OUT_FLAREFREQ3].iNum       = 1;
  output[OUT_FLAREFREQ3].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ3]           = &WriteFlareFreq3;

  fvFormattedString(&output[OUT_FLAREFREQ4].cName, "FlareFreq4");
  fvFormattedString(&output[OUT_FLAREFREQ4].cDescr,
          "Fourth value of flare frequency range");
  fvFormattedString(&output[OUT_FLAREFREQ4].cNeg, "/day");
  output[OUT_FLAREFREQ4].bNeg       = 1;
  output[OUT_FLAREFREQ4].dNeg       = DAYSEC;
  output[OUT_FLAREFREQ4].iNum       = 1;
  output[OUT_FLAREFREQ4].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQ4]           = &WriteFlareFreq4;

  fvFormattedString(&output[OUT_FLAREFREQMIN].cName, "FlareFreqMin");
  fvFormattedString(&output[OUT_FLAREFREQMIN].cDescr,
          "Frequency of the flares with the lowest energy");
  fvFormattedString(&output[OUT_FLAREFREQMIN].cNeg, "/day");
  output[OUT_FLAREFREQMIN].bNeg       = 1;
  output[OUT_FLAREFREQMIN].dNeg       = DAYSEC;
  output[OUT_FLAREFREQMIN].iNum       = 1;
  output[OUT_FLAREFREQMIN].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQMIN]           = &WriteFlareFreqMin;

  fvFormattedString(&output[OUT_FLAREFREQMID].cName, "FlareFreqMid");
  fvFormattedString(&output[OUT_FLAREFREQMID].cDescr,
          "Frequency of the flares with the middle energy in the energy range");
  fvFormattedString(&output[OUT_FLAREFREQMID].cNeg, "/day");
  output[OUT_FLAREFREQMID].bNeg       = 1;
  output[OUT_FLAREFREQMID].dNeg       = DAYSEC;
  output[OUT_FLAREFREQMID].iNum       = 1;
  output[OUT_FLAREFREQMID].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQMID]           = &WriteFlareFreqMid;

  fvFormattedString(&output[OUT_FLAREFREQMAX].cName, "FlareFreqMax");
  fvFormattedString(&output[OUT_FLAREFREQMAX].cDescr,
          "Frequency of the flares with the highest energy");
  fvFormattedString(&output[OUT_FLAREFREQMAX].cNeg, "/day");
  output[OUT_FLAREFREQMAX].bNeg       = 1;
  output[OUT_FLAREFREQMAX].dNeg       = DAYSEC;
  output[OUT_FLAREFREQMAX].iNum       = 1;
  output[OUT_FLAREFREQMAX].iModuleBit = FLARE;
  fnWrite[OUT_FLAREFREQMAX]           = &WriteFlareFreqMax;


  fvFormattedString(&output[OUT_FLAREENERGY1].cName, "FlareEnergy1");
  fvFormattedString(&output[OUT_FLAREENERGY1].cDescr, "First value of flare energy range");
  fvFormattedString(&output[OUT_FLAREENERGY1].cNeg, "ergs");
  output[OUT_FLAREENERGY1].bNeg       = 1;
  output[OUT_FLAREENERGY1].dNeg       = 1.0e7;
  output[OUT_FLAREENERGY1].iNum       = 1;
  output[OUT_FLAREENERGY1].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY1]           = &WriteFlareEnergy1;

  fvFormattedString(&output[OUT_FLAREENERGY2].cName, "FlareEnergy2");
  fvFormattedString(&output[OUT_FLAREENERGY2].cDescr,
          "Second value of flare energy range");
  fvFormattedString(&output[OUT_FLAREENERGY2].cNeg, "ergs");
  output[OUT_FLAREENERGY2].bNeg       = 1;
  output[OUT_FLAREENERGY2].dNeg       = 1.0e7;
  output[OUT_FLAREENERGY2].iNum       = 1;
  output[OUT_FLAREENERGY2].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY2]           = &WriteFlareEnergy2;

  fvFormattedString(&output[OUT_FLAREENERGY3].cName, "FlareEnergy3");
  fvFormattedString(&output[OUT_FLAREENERGY3].cDescr, "Third value of flare energy range");
  fvFormattedString(&output[OUT_FLAREENERGY3].cNeg, "ergs");
  output[OUT_FLAREENERGY3].bNeg       = 1;
  output[OUT_FLAREENERGY3].dNeg       = 1.0e7;
  output[OUT_FLAREENERGY3].iNum       = 1;
  output[OUT_FLAREENERGY3].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY3]           = &WriteFlareEnergy3;

  fvFormattedString(&output[OUT_FLAREENERGY4].cName, "FlareEnergy4");
  fvFormattedString(&output[OUT_FLAREENERGY4].cDescr,
          "Fourth value of flare energy range");
  fvFormattedString(&output[OUT_FLAREENERGY4].cNeg, "ergs");
  output[OUT_FLAREENERGY4].bNeg       = 1;
  output[OUT_FLAREENERGY4].dNeg       = 1.0e7;
  output[OUT_FLAREENERGY4].iNum       = 1;
  output[OUT_FLAREENERGY4].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGY4]           = &WriteFlareEnergy4;

  fvFormattedString(&output[OUT_FLAREENERGYMIN].cName, "FlareEnergyMin");
  fvFormattedString(&output[OUT_FLAREENERGYMIN].cDescr, "Minimum flare energy");
  fvFormattedString(&output[OUT_FLAREENERGYMIN].cNeg, "ergs");
  output[OUT_FLAREENERGYMIN].bNeg       = 1;
  output[OUT_FLAREENERGYMIN].dNeg       = 1.0e7;
  output[OUT_FLAREENERGYMIN].iNum       = 1;
  output[OUT_FLAREENERGYMIN].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGYMIN]           = &WriteFlareEnergyMin;

  fvFormattedString(&output[OUT_FLAREENERGYMID].cName, "FlareEnergyMid");
  fvFormattedString(&output[OUT_FLAREENERGYMID].cDescr,
          "Middle flare energy in the range of energy values");
  fvFormattedString(&output[OUT_FLAREENERGYMID].cNeg, "ergs");
  output[OUT_FLAREENERGYMID].bNeg       = 1;
  output[OUT_FLAREENERGYMID].dNeg       = 1.0e7;
  output[OUT_FLAREENERGYMID].iNum       = 1;
  output[OUT_FLAREENERGYMID].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGYMID]           = &WriteFlareEnergyMid;

  fvFormattedString(&output[OUT_FLAREENERGYMAX].cName, "FlareEnergyMax");
  fvFormattedString(&output[OUT_FLAREENERGYMAX].cDescr, "Maximum flare energy");
  fvFormattedString(&output[OUT_FLAREENERGYMAX].cNeg, "ergs");
  output[OUT_FLAREENERGYMAX].bNeg       = 1;
  output[OUT_FLAREENERGYMAX].dNeg       = 1.0e7;
  output[OUT_FLAREENERGYMAX].iNum       = 1;
  output[OUT_FLAREENERGYMAX].iModuleBit = FLARE;
  fnWrite[OUT_FLAREENERGYMAX]           = &WriteFlareEnergyMax;


  fvFormattedString(&output[OUT_LXUVFLARE].cName, "LXUVFlare");
  fvFormattedString(&output[OUT_LXUVFLARE].cDescr, "XUV Luminosity from flares");
  fvFormattedString(&output[OUT_LXUVFLARE].cNeg, "LSUN");
  output[OUT_LXUVFLARE].bNeg       = 1;
  output[OUT_LXUVFLARE].dNeg       = 1. / LSUN;
  output[OUT_LXUVFLARE].iNum       = 1;
  output[OUT_LXUVFLARE].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLARE]           = &WriteLXUVFlare;

  // TODO: Include the error in the FFD slopes to calculate the upper and higher
  // limit of XUV luminosity by flares
  /*fvFormattedString(output[OUT_LXUVFLAREUPPER].cName, "LXUVFlareUpper");
  fvFormattedString(output[OUT_LXUVFLAREUPPER].cDescr,
          "Upper limit value to XUV Luminosity from flares when considerer "
          "flare rate slope and Y-intercept errors");
  fvFormattedString(output[OUT_LXUVFLAREUPPER].cNeg, "LSUN");
  output[OUT_LXUVFLAREUPPER].bNeg       = 1;
  output[OUT_LXUVFLAREUPPER].dNeg       = 1. / LSUN;
  output[OUT_LXUVFLAREUPPER].iNum       = 1;
  output[OUT_LXUVFLAREUPPER].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLAREUPPER]           = &WriteLXUVFlareUpper;

  fvFormattedString(output[OUT_LXUVFLARELOWER].cName, "LXUVFlareLower");
  fvFormattedString(output[OUT_LXUVFLARELOWER].cDescr,
          "Lower limit value to XUV Luminosity from flares when considerer "
          "flare rate slope and Y-intercept errors");
  fvFormattedString(output[OUT_LXUVFLARELOWER].cNeg, "LSUN");
  output[OUT_LXUVFLARELOWER].bNeg       = 1;
  output[OUT_LXUVFLARELOWER].dNeg       = 1. / LSUN;
  output[OUT_LXUVFLARELOWER].iNum       = 1;
  output[OUT_LXUVFLARELOWER].iModuleBit = FLARE;
  fnWrite[OUT_LXUVFLARELOWER]           = &WriteLXUVFlareLower;*/
}

/************ FLARE Logging Functions **************/

void LogOptionsFlare(CONTROL *control, FILE *fp) {

  /* Anything here?

  fprintf(fp,"-------- FLARE Options -----\n\n");
  */
}

void LogFlare(BODY *body,
              CONTROL *control,
              OUTPUT *output,
              SYSTEM *system,
              UPDATE *update,
              fnWriteOutput fnWrite[],
              FILE *fp) {

  /* Anything here?
   int iOut;

   fprintf(fp,"\n----- FLARE PARAMETERS ------\n");
   for (iOut=OUTSTARTFLARE;iOut<OUTBODYSTARTFLARE;iOut++) {
     if (output[iOut].iNum > 0)
       WriteLogEntry(control,output[iOut],body,system,fnWrite[iOut],fp,update,0);
   }*/
}

void LogBodyFlare(BODY *body,
                  CONTROL *control,
                  OUTPUT *output,
                  SYSTEM *system,
                  UPDATE *update,
                  fnWriteOutput fnWrite[],
                  FILE *fp,
                  int iBody) {
  int iOut;

  fprintf(fp, "----- FLARE PARAMETERS (%s)------\n", body[iBody].cName);
  for (iOut = OUTSTARTFLARE; iOut < OUTENDFLARE; iOut++) {
    if (output[iOut].iNum > 0)
      WriteLogEntry(body,
                    control,
                    &output[iOut],
                    system,
                    update,
                    fnWrite[iOut],
                    fp,
                    iBody);
  }
}

void AddModuleFlare(CONTROL *control, MODULE *module, int iBody, int iModule) {
  module->iaModule[iBody][iModule] = FLARE;

  module->fnCountHalts[iBody][iModule]        = &CountHaltsFlare;
  module->fnReadOptions[iBody][iModule]       = &ReadOptionsFlare;
  module->fnLogBody[iBody][iModule]           = &LogBodyFlare;
  module->fnVerify[iBody][iModule]            = &VerifyFlare;
  module->fnAssignDerivatives[iBody][iModule] = &AssignFlareDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullFlareDerivatives;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltFlare;

  module->fnInitializeUpdate[iBody][iModule]   = &InitializeUpdateFlare;
  module->fnInitializeBody[iBody][iModule]     = &InitializeBodyFlare;
  module->fnFinalizeUpdateLXUV[iBody][iModule] = &FinalizeUpdateLXUVFlare;

  /* XXX I think this approach to multi-module outputs is defunct
  module->fnFinalizeOutputFunction[iBody][iModule] =
  &FinalizeOutputFunctionFlare;
  */
}

/************* FLARE Functions ************/

// Functions to convert the input band pass to the right one (SXR)

double fdBandPassXUV(BODY *body, int iBody, double dInputEnergy) {
  double dLogEnergyXUV = 0.0;

  if (body[iBody].iFlareBandPass == FLARE_KEPLER) { // Band pass 4000 – 9000 Å
    // 1.875 comes from the Osten & Wolk 2015
    // (https://ui.adsabs.harvard.edu/abs/2015ApJ...809...79O/abstract), this is
    // in Table 2 - the SXR makes up 30% of the bolometric energy of the flare,
    // and the Kepler bandpass makes up 16%. So if you have the Kepler energy or
    // flux, multiply it by 30/16=1.875 to get the SXR energy.
    dLogEnergyXUV = log10(dInputEnergy * 1.875);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_UV) { // Band pass 3000 – 4300 Å
    dLogEnergyXUV = log10(dInputEnergy * 2.727);
  } else if (body[iBody].iFlareBandPass == FLARE_GOES) { // Band pass 1 - 8 Å
    dLogEnergyXUV = log10(dInputEnergy * 5);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_SXR) { // Band pass 1.24 - 1239.85 Å
    dLogEnergyXUV = log10(dInputEnergy);
  }
  /* TODO: Leave it here for when the data in the future allows for the
    conversion of a kepler bandpass to these another bandpass. else if
    (body[iBody].iFlareBandPass == FLARE_FUV) { // Band pass 1.24 - 1239.85 Å
     dEnergyXUV = dInputEnergy;
     dLogEnergyXUV = log10(dInputEnergy);
    }
    else if (body[iBody].iFlareBandPass == FLARE_NUV) { // Band pass 1.24 -
    1239.85 Å dEnergyXUV = dInputEnergy; dLogEnergyXUV = log10(dInputEnergy);
    }
    else if (body[iBody].iFlareBandPass == FLARE_LYALPHA) { // Band pass 1.24 -
    1239.85 Å dEnergyXUV = dInputEnergy; dLogEnergyXUV = log10(dInputEnergy);
    } */
  else if (body[iBody].iFlareBandPass ==
           FLARE_TESS_UV) { // Band pass 1.24 - 1239.85 Å
    dLogEnergyXUV = log10(dInputEnergy * (0.3 / 0.076));
  } else if (body[iBody].iFlareBandPass ==
             FLARE_BOLOMETRIC) { // Band pass 1.24 - 1239.85 Å
    dLogEnergyXUV = log10(dInputEnergy * 0.3);
  }
  return dLogEnergyXUV;
}


double fdBandPassKepler(BODY *body, int iBody, double dInputEnergy) {
  double dLogEnergy = 0.0;

  if (body[iBody].iFlareBandPass == FLARE_KEPLER) { // Band pass 4000 – 9000 Å
    dLogEnergy = log10(dInputEnergy);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_UV) { // Band pass 3000 – 4300 Å
    dLogEnergy = log10(dInputEnergy * 1.455);
  } else if (body[iBody].iFlareBandPass == FLARE_GOES) { // Band pass 1 - 8 Å
    dLogEnergy = log10(dInputEnergy * 2.667);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_SXR) { // Band pass 1.24 - 1239.85 Å
    dLogEnergy = log10(dInputEnergy * 0.5334);
  } else if (body[iBody].iFlareBandPass == FLARE_TESS_UV) { // Band pass 1 - 8 Å
    dLogEnergy = log10(dInputEnergy * 2.10526315789);
  } else if (body[iBody].iFlareBandPass ==
             FLARE_BOLOMETRIC) { // Band pass 1.24 - 1239.85 Å
    dLogEnergy = log10(dInputEnergy * 0.16);
  }
  return dLogEnergy;
}


// Davenport et al. (2019), Equation 3 and Table 1 coefficients
#define DAVENPORT_A1 -0.07
#define DAVENPORT_A2 0.79
#define DAVENPORT_A3 -1.06
#define DAVENPORT_B1 2.01
#define DAVENPORT_B2 -25.15
#define DAVENPORT_B3 33.99

static double fdFFDCumulativeRate(double dLogEnergyErg,
                                  double dSlopeDayErg,
                                  double dYIntDay) {
  // Returns cumulative flare frequency in flares/second.
  return pow(10.0, dSlopeDayErg * dLogEnergyErg + dYIntDay) / DAYSEC;
}

// Davenport linear coefficient for log(nu/day) = a*log(E/erg) + b.
double fdDavenport(double dA1, double dA2, double dA3, double dStarAge,
                   double dStarMass) {
  double dAgeMyr, dMassSolar;

  dAgeMyr   = dStarAge / (YEARSEC * 1.0e6);
  dMassSolar = dStarMass / MSUN;

  // VerifyFlare enforces physically valid inputs; these guards avoid NaN.
  if (dAgeMyr <= 0 || dMassSolar <= 0) {
    return 0.0;
  }

  return (dA1 * log10(dAgeMyr)) + (dA2 * dMassSolar) + dA3;
}

// fdFFD calculates cumulative flare frequency for either FFD mode.
double fdFFD(BODY *body,
             int iBody,
             double dLogEnergy,
             double dFlareSlope,
             double dFlareYInt) {
  double dSlopeDayErg, dYIntDay;

  dSlopeDayErg = dFlareSlope;
  dYIntDay     = dFlareYInt;

  // LACY inputs are stored in SI-like units and converted here.
  if (body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    dSlopeDayErg = -dFlareSlope * DAYSEC * log10(1.0e7);
    dYIntDay     = dFlareYInt * DAYSEC;
  }

  return fdFFDCumulativeRate(dLogEnergy, dSlopeDayErg, dYIntDay);
}

double fdEnergyJoulesXUV(double dLogEnergyXUV) {
  double dEnergyJOUXUV;

  dEnergyJOUXUV = pow(10, (dLogEnergyXUV));

  return dEnergyJOUXUV;
}


double fdLXUVFlare(BODY *body, double dDeltaTime, int iBody) {
  double dFlareSlope = 0.0;
  double dFlareYInt  = 0.0;
  double dLXUVFlare = 0.0;
  double dLogEnergyMinERG, dLogEnergyMaxERG, dLogEnergyMin, dLogEnergyMax;
  double dLogEnergyMinXUV, dEnergyStep;
  int i;

  (void)dDeltaTime;

  if (body[iBody].iFlareFFD == FLARE_FFD_DAVENPORT) {
    // Davenport et al. (2019), Eq. 3, Table 1.
    dFlareSlope = fdDavenport(DAVENPORT_A1, DAVENPORT_A2, DAVENPORT_A3,
                              body[iBody].dAge, body[iBody].dMass);
    dFlareYInt = fdDavenport(DAVENPORT_B1, DAVENPORT_B2, DAVENPORT_B3,
                             body[iBody].dAge, body[iBody].dMass);
  } else if (body[iBody].iFlareFFD == FLARE_FFD_LACY) {
    dFlareSlope = body[iBody].dFlareSlope;
    dFlareYInt  = body[iBody].dFlareYInt;
    // TODO: Let this here for allow the user input the slopes errors in the
    // future
    /*dFlareSlopeErrorUpper = body[iBody].dFlareSlopeErrorUpper;
    dFlareSlopeErrorLower = body[iBody].dFlareSlopeErrorLower;
    dFlareYIntErrorUpper = body[iBody].dFlareYIntErrorUpper;
    dFlareYIntErrorLower = body[iBody].dFlareYIntErrorLower;*/
  } else if (body[iBody].iFlareFFD == FLARE_FFD_NONE) {
    return body[iBody].dLXUVFlareConst;
  } else {
    return 0.0;
  }

  // 1. Convert the user-selected energy limits to XUV and Kepler spaces.
  dLogEnergyMinXUV = fdBandPassXUV(body, iBody, body[iBody].dFlareMinEnergy);
  dLogEnergyMin    = fdBandPassKepler(body, iBody, body[iBody].dFlareMinEnergy);
  dLogEnergyMax    = fdBandPassKepler(body, iBody, body[iBody].dFlareMaxEnergy);

  // Davenport uses log10(E/erg), so convert Joules -> ergs in log-space.
  dLogEnergyMinERG = dLogEnergyMin + 7.0;
  dLogEnergyMaxERG = dLogEnergyMax + 7.0;
  dEnergyStep      = (dLogEnergyMaxERG - dLogEnergyMinERG) / body[iBody].iEnergyBin;

  // 2. Build energy grids.
  for (i = 0; i < body[iBody].iEnergyBin + 1; i++) {
    body[iBody].daEnergyJOUXUV[i] = fdEnergyJoulesXUV(dLogEnergyMinXUV + i * dEnergyStep);
    body[iBody].daLogEner[i]      = dLogEnergyMinERG + i * dEnergyStep;
    body[iBody].daEnerJOU[i]      = pow(10, dLogEnergyMin + i * dEnergyStep);
  }

  body[iBody].dFlareEnergy1   = body[iBody].daEnerJOU[0];
  body[iBody].dFlareEnergy2   = body[iBody].daEnerJOU[1];
  body[iBody].dFlareEnergy3   = body[iBody].daEnerJOU[2];
  body[iBody].dFlareEnergy4   = body[iBody].daEnerJOU[3];
  body[iBody].dFlareEnergyMin = body[iBody].daEnerJOU[0];
  body[iBody].dFlareEnergyMid = body[iBody].daEnerJOU[(int)(body[iBody].iEnergyBin / 2.)];
  body[iBody].dFlareEnergyMax = body[iBody].daEnerJOU[body[iBody].iEnergyBin];

  // 3. Evaluate cumulative FFD and integrate flare XUV luminosity.
  for (i = 0; i < body[iBody].iEnergyBin + 1; i++) {
    body[iBody].daFFD[i] = fdFFD(body, iBody, body[iBody].daLogEner[i],
                                 dFlareSlope, dFlareYInt);
  }

  body[iBody].dFlareFreq1   = body[iBody].daFFD[0];
  body[iBody].dFlareFreq2   = body[iBody].daFFD[1];
  body[iBody].dFlareFreq3   = body[iBody].daFFD[2];
  body[iBody].dFlareFreq4   = body[iBody].daFFD[3];
  body[iBody].dFlareFreqMin = body[iBody].daFFD[0];
  body[iBody].dFlareFreqMid = body[iBody].daFFD[(int)(body[iBody].iEnergyBin / 2)];
  body[iBody].dFlareFreqMax = body[iBody].daFFD[body[iBody].iEnergyBin];

  for (i = 0; i < body[iBody].iEnergyBin; i++) {
    body[iBody].daLXUVFlare[i] =
          (body[iBody].daEnergyJOUXUV[i + 1] - body[iBody].daEnergyJOUXUV[i]) *
          ((body[iBody].daFFD[i + 1] + body[iBody].daFFD[i]) / 2.0);
    dLXUVFlare += body[iBody].daLXUVFlare[i];
  }

  return dLXUVFlare;
}
