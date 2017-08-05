//------------------------------------------------------------------------------
// File: ActiveCf.h
//
// Desc: Contains the data formats for the transfer of VfW4 filters via the
//       clipboard.
//
// Copyright (c) 1992-2001, Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#define CFSTR_VFW_FILTERLIST "Video for Windows 4 Filters"

typedef struct tagVFW_FILTERLIST{
    UINT  cFilters;                     // number of CLSIDs in aClsId
    CLSID aClsId[1];                    // ClsId of each filter
    
} VFW_FILTERLIST;


