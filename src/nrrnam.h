/******************************************************************************/
/*                                                                            */
/* File name    : NRRNAM.H                                                    */
/*                                                                            */
/* Title        : Unsupported NRRNAMs table include file                      */
/*                                                                            */
/* Author       : Dave Blaschke                                               */
/*                deblaschke@yahoo.com                                        */
/*                                                                            */
/* History      : Version Date    Item                                        */
/*                2.45.18 28Feb00 Removed import by name restriction          */
/*                                                                            */
/******************************************************************************/

UNSNRRNAM UnsNRRNAM[] =                          /* Unsupported NRRNAMs       */
          {
             /*
              * The reason that the following PMDRAG APIs are unsupported is
              * that between OS/2 2.10 and 2.30  their names were changed to
              * include "16" in the prefix.  For example,  DRGACCESSDRAGINFO
              * became DRG16ACCESSDRAGINFO.
              */
             { "PMDRAG", 33L,
                { "DRGACCESSDRAGINFO",
                  "DRGADDSTRHANDLE",
                  "DRGALLOCDRAGINFO",
                  "DRGALLOCDRAGTRANSFER",
                  "DRGDELETEDRAGINFOSTRHANDLES",
                  "DRGDELETESTRHANDLE",
                  "DRGDRAG",
                  "DRGFREEDRAGINFO",
                  "DRGFREEDRAGTRANSFER",
                  "DRGGETPS",
                  "DRGPOSTTRANSFERMSG",
                  "DRGPUSHDRAGINFO",
                  "DRGQUERYDRAGITEM",
                  "DRGQUERYDRAGITEMCOUNT",
                  "DRGQUERYDRAGITEMPTR",
                  "DRGQUERYNATIVERMF",
                  "DRGQUERYNATIVERMFLEN",
                  "DRGQUERYSTRNAME",
                  "DRGQUERYSTRNAMELEN",
                  "DRGQUERYTRUETYPE",
                  "DRGQUERYTRUETYPELEN",
                  "DRGRELEASEPS",
                  "DRGSENDTRANSFERMSG",
                  "DRGSETDRAGPOINTER",
                  "DRGSETDRAGIMAGE",
                  "DRGSETDRAGITEM",
                  "DRGVERIFYNATIVERMF",
                  "DRGVERIFYRMF",
                  "DRGVERIFYTRUETYPE",
                  "DRGVERIFYTYPE",
                  "DRGVERIFYTYPESET",
                  "DRGDRAGFILES",
                  "DRGACCEPTDROPPEDFILES"
                }
             },

             /*
              * The reason that the following  PMPIC APIs are unsupported is
              * that between OS/2 2.30 and 2.40  their names were changed to
              * mixed case.  For example,  PICPRINT became PicPrint.
              */
             { "PMPIC", 7L,
                { "PICPRINT",
                  "PICICHG",
                  "PIF2MET",
                  "PRFPIF2MET",
                  "PIC32PRINT",
                  "PIC32ICHG",
                  "PRF32PIF2MET"
                }
             }
          };
