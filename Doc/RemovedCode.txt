>>

    /* TODO: Use new handle wrapping code

    // ******************************************************************
    // * For now, just check for 'special' cases
    // ******************************************************************
    if(strcmp(ObjectAttributes->ObjectName->Buffer, "\\Device\\Harddisk0\\partition1\\") == 0)
    {
        EmuHandle *iEmuHandle = new EmuHandle;

        iEmuHandle->m_Type = EMUHANDLE_TYPE_PARTITION1;

        *FileHandle = PtrToEmuHandle(iEmuHandle);
    }
    else if(strcmp(ObjectAttributes->ObjectName->Buffer, "\\Device\\Harddisk0\\partition1\\TDATA") == 0)
    {
        EmuHandle *iEmuHandle = new EmuHandle;

        iEmuHandle->m_Type = EMUHANDLE_TYPE_TDATA;

        *FileHandle = PtrToEmuHandle(iEmuHandle);

        // TODO: Update IoStatusBlock if necessary
    }
    else
    {
        EmuPanic();
    }
    */
	
<<

	__asm int 3
	wchar_t wszObjectName[160];

	xntdll::UNICODE_STRING    NtUnicodeString;
	xntdll::OBJECT_ATTRIBUTES NtObjAttr;

    setlocale(LC_ALL, "English");
    mbstowcs(wszObjectName, ObjectAttributes->ObjectName->Buffer, 160);

	NT_RtlInitUnicodeString(&NtUnicodeString, wszObjectName);

	// Initialize NtObjAttr
	NtObjAttr.Length = sizeof(xntdll::OBJECT_ATTRIBUTES);

	InitializeObjectAttributes(&NtObjAttr, &NtUnicodeString, ObjectAttributes->Attributes, ObjectAttributes->RootDirectory, NULL);

    NTSTATUS ret = NT_NtCreateFile
    (
        FileHandle, DesiredAccess, &NtObjAttr, (xntdll::IO_STATUS_BLOCK*)IoStatusBlock,
        (xntdll::LARGE_INTEGER*)AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, NULL, NULL
    );

	if(FAILED(ret))
		MessageBox(g_hEmuWindow, "Bad!", "Cxbx", MB_OK);
	else
		MessageBox(g_hEmuWindow, "Good!", "Cxbx", MB_OK);


<<

// ******************************************************************
// * calculate TLS adjustment
// ******************************************************************
uint32 TlsAdjust = 0;

if(x_Xbe->m_TLS == 0)
    TlsAdjust = -1;
else
{
    // ******************************************************************
    // * necessary tls adjustment values
    // ******************************************************************
    uint32 TlsZF = x_Xbe->m_TLS->dwSizeofZeroFill;
    uint32 TlsEA = x_Xbe->m_TLS->dwDataEndAddr;
    uint32 TlsSA = x_Xbe->m_TLS->dwDataStartAddr;
    
    // ******************************************************************
    // * calculate tls adjustment
    // ******************************************************************
    TlsAdjust  = (TlsZF - TlsSA) + TlsEA + 0x0F;
    TlsAdjust &= 0xFFFFFFF0;
    TlsAdjust += 4;
}
