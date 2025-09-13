/**
 * Hook function to find textures in GTA SA (32-64-bit version)
 * ฟังก์ชันฮุคสำหรับค้นหาเท็กซ์เจอร์ใน GTA SA (เวอร์ชัน 32-64-bit)
 */

RwTexture* (*CTxdStore__TxdStoreFindCB)(const char *szTexture);
RwTexture* CTxdStore__TxdStoreFindCB_hook(const char *szTexture)
{
	static char* texdb[] = { "samp", "mobile", "txd", "gta3", "gta_int", "cutscene", "player", "menu" };
	for(int i = 0; i < sizeof(texdb)/sizeof(texdb[0]); i++)
	{
		// TextureDatabaseRuntime::GetDatabase
		uintptr_t pDatabaseHandle = Patch::CallFunction<uintptr_t>("_ZN22TextureDatabaseRuntime11GetDatabaseEPKc", texdb[i]);
		if(!pDatabaseHandle) continue;

		// DatabaseRegisterCount
  		int iDatabaseRegisterCount = *reinterpret_cast<int*>(Patch::getSym("_ZN22TextureDatabaseRuntime10registeredE"));
		if(iDatabaseRegisterCount)
		{
			// DatabaseRegisterHandleList
			uintptr_t *pDatabaseRegisterHandleList = *reinterpret_cast<uintptr_t**>(Patch::getSym("_ZN22TextureDatabaseRuntime10registeredE"));
			
			int iIndex = 0;
			while(pDatabaseRegisterHandleList[iIndex] != pDatabaseHandle)
			{
				if(++iIndex >= iDatabaseRegisterCount)
				{
					// TextureDatabaseRuntime::Register
					Patch::CallFunction<void>("_ZN22TextureDatabaseRuntime8RegisterEPS_", pDatabaseHandle);

					// TextureDatabaseRuntime::GetTexture
					uintptr_t pTexture = Patch::CallFunction<uintptr_t>("_ZN22TextureDatabaseRuntime10GetTextureEPKc", szTexture);

					// TextureDatabaseRuntime::Unregister
					Patch::CallFunction<void>("_ZN22TextureDatabaseRuntime10UnregisterEPS_", pDatabaseHandle);
					
					if(pTexture) return reinterpret_cast<RwTexture*>(pTexture);
				}
			}
		}
	}

	// RwTexDictionaryGetCurrent
	int iParent = Patch::CallFunction<int>("_Z25RwTexDictionaryGetCurrentv");
	if(iParent)
	{
		while(true)
		{
			// RwTexDictionaryFindNamedTexture
			uintptr_t pTexture = Patch::CallFunction<uintptr_t>("_Z31RwTexDictionaryFindNamedTextureP15RwTexDictionaryPKc", iParent, szTexture);
			if(pTexture) return reinterpret_cast<RwTexture*>(pTexture);

			// TxdParent
			iParent = *(int*)(*reinterpret_cast<int**>(Patch::getSym("_ZL18ms_txdPluginOffset")) + iParent);
			if(!iParent) break;
		}
	}

	return 0;
}
