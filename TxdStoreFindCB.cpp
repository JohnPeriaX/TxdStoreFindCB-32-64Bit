/**
 * Hook function to find textures in GTA SA (64-bit version)
 * ฟังก์ชันฮุคสำหรับค้นหาเท็กซ์เจอร์ใน GTA SA (เวอร์ชัน 64-bit)
 */
uintptr_t (*CTxdStore__TxdStoreFindCB)(const char *szTexture);
uintptr_t CTxdStore__TxdStoreFindCB_hook(const char *szTexture)
{
    // Texture databases to search
    // ฐานข้อมูลเท็กซ์เจอร์ที่จะค้นหา
    static const char* texdb[] = { "samp", "gta_int", "gta3" };
    
    // Search in all texture databases
    // ค้นหาในทุกฐานข้อมูลเท็กซ์เจอร์
    for(int i = 0; i < 3; i++)
    {
        // Get texture database handle
        // รับแฮนเดิลของฐานข้อมูลเท็กซ์เจอร์
        uintptr_t pDatabaseHandle = ((uintptr_t (*)(const char *))(g_libGTASA + 0x287AF4))(texdb[i]);
        if(!pDatabaseHandle) continue;

        // Check if database is registered
        // ตรวจสอบว่าฐานข้อมูลลงทะเบียนแล้วหรือไม่
        int iDatabaseRegisterCount = *(int*)(g_libGTASA + 0x89AA10); // +8 for 64-bit alignment
        
        if(iDatabaseRegisterCount > 0)
        {
            // Get list of registered handles
            // รับรายการแฮนเดิลที่ลงทะเบียนแล้ว
            uintptr_t *pDatabaseRegisterHandleList = *(uintptr_t**)(g_libGTASA + 0x89AA18); // +16 for 64-bit
            
            // Search in registered databases
            // ค้นหาในฐานข้อมูลที่ลงทะเบียนแล้ว
            for(int iIndex = 0; iIndex < iDatabaseRegisterCount; iIndex++)
            {
                if(pDatabaseRegisterHandleList[iIndex] == pDatabaseHandle)
                    break;
                    
                // If not found in registered databases
                // ถ้าไม่พบในฐานข้อมูลที่ลงทะเบียนแล้ว
                if(iIndex == iDatabaseRegisterCount - 1)
                {
                    // Temporarily register the database
                    // ลงทะเบียนฐานข้อมูลชั่วคราว
                    ((void (*)(uintptr_t))(g_libGTASA + 0x2865D8))(pDatabaseHandle);

                    // Get the requested texture
                    // ดึงเท็กซ์เจอร์ที่ต้องการ
                    uintptr_t pTexture = ((uintptr_t (*)(const char *))(g_libGTASA + 0x286718))(szTexture);

                    // Unregister the database
                    // ยกเลิกการลงทะเบียนฐานข้อมูล
                    ((void (*)(uintptr_t))(g_libGTASA + 0x2866A4))(pDatabaseHandle);

                    if(pTexture) {
                        return pTexture;
                    }
                }
            }
        }
    }

    // Get current texture dictionary
    // รับพจนานุกรมเท็กซ์เจอร์ปัจจุบัน
    uintptr_t iParent = ((uintptr_t (*)(void))(g_libGTASA + 0x27427C))();
    
    if(iParent)
    {
        // Search in current and parent dictionaries
        // ค้นหาในพจนานุกรมปัจจุบันและพจนานุกรมแม่
        while(true)
        {
            // Find texture by name
            // ค้นหาเท็กซ์เจอร์ด้วยชื่อ
            uintptr_t pTexture = ((uintptr_t (*)(uintptr_t, const char *))(g_libGTASA + 0x27417C))(iParent, szTexture);
            if(pTexture) {
                return pTexture;
            }

            // Get parent dictionary (64-bit pointer access)
            // รับพจนานุกรมแม่ (การเข้าถึงพอยน์เตอร์แบบ 64-bit)
            uintptr_t pTxdParentStruct = *(uintptr_t*)(g_libGTASA + 0xD28608);
            if(!pTxdParentStruct) break;
            
            iParent = *(uintptr_t*)(pTxdParentStruct + iParent * sizeof(uintptr_t));
            if(!iParent) break;
        }
    }

    return 0; // Texture not found
    // ไม่พบเท็กซ์เจอร์
}
