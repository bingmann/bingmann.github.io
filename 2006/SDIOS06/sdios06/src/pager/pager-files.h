// $Id: pager-files.h 198 2006-07-27 07:16:57Z sdi2 $
//
// File:  src/pager/pager-files.h
//
// Description: Implementation of the IDL Interfaces to show task / memory
// structures as virtual files via the name service.

// this file is included from pager-impl.h and all the pager's data structures
// are available.

/** This constructs a /proc-like virtual file system which can be mounted in
 the name service as /task

 It follows this structure:
 /task/												id == 0
 /task/<threadid-as-hexnumber>						id == 1
 /task/<threadid-as-hexnumber>/status				id == 2
 /task/<threadid-as-hexnumber>/mapping				id == 3
 /task/freelist										id == 10

 Where threadid-as-hexnumber can be 0-padded if lookuped via GetEntry, but will
 be enumerated without leading zeros.
*/

struct pager_objectid_t
{
	unsigned int id				: 14;
	unsigned int gthreadid 		: 18;

	explicit inline pager_objectid_t(unsigned int _id, unsigned int _gthrid)
		: id(_id), gthreadid(_gthrid)
	{
	}

	// some evil automatic type conversions
	inline pager_objectid_t(objectid_t oi)
	{
		*reinterpret_cast<objectid_t*>(this) = oi;
	}

	inline operator objectid_t() const
	{
		return *reinterpret_cast<const objectid_t*>(this);
	}

} __attribute__((packed));

ASSERT_SIZE(pager_objectid_t, sizeof(objectid_t));

IDL4_INLINE void pager_GetEntry_implementation(CORBA_Object _caller, const objectid_t directory_handle, const CORBA_char *name, const interfaceid_t interfaceid, L4_ThreadId_t *server, objectid_t *object_handle, idl4_server_environment *_env)
{
	pdbg0("pager: GetEntry(0x%lx, %s, 0x%lx) called by 0x%lx\n",
		  directory_handle, name, interfaceid, _caller.raw);

	pager_objectid_t po = directory_handle;

	switch(po.id)
	{
	case 0:
		// in the root directory
		   
		if (strcmp(name, "freelist") == 0)
		{
			if (interfaceid != IF_FILE_ID)
			{
				pdbg0("pager: GetEntry on freelist was not requesting a file interface\n");
				CORBA_exception_set(_env, ex_not_supported, 0);
				return;
			}

			*server = L4_Myself();
			*object_handle = pager_objectid_t(10, 0);
			return;
		}
		else
		{
			char *endptr;
			unsigned long thrid = strtoul(name, &endptr, 16);
			unsigned long basethrid = thrid >> 14;

			if (endptr && *endptr == 0)
			{
				struct TaskEntry *te = tasklist.find(L4_GlobalId(basethrid, 1));
				if (!te) {
					pdbg0("pager: GetEntry on root directory was not a managed threadid!\n");
					CORBA_exception_set(_env, ex_not_found, NULL);
					return;
				}

				switch(interfaceid)
				{
				case IF_DIRECTORY_ID:
					*server = L4_Myself();
					*object_handle = pager_objectid_t(1, basethrid);
					return;
/*
				case IF_FILE_ID:
					*server = L4_Myself();
					*object_handle = pager_objectid_t(2, basethrid);
					return;
*/
				default:
					pdbg0("pager: GetEntry on root directory was not requesting a file interface\n");
					CORBA_exception_set(_env, ex_not_supported, 0);
					return;
				}
			}
			else
			{
				pdbg0("pager: GetEntry on root directory was not valid threadid.\n");
				CORBA_exception_set(_env, ex_not_found, 0);
				return;
			}
		}
		break;

	case 1:
		// in the one of the task subcatalogs

		if (interfaceid != IF_FILE_ID)
		{
			pdbg0("pager: GetEntry on task subcatalog was not requesting a file interface\n");
			CORBA_exception_set(_env, ex_not_supported, 0);
			return;
		}
		{
			struct TaskEntry *te = tasklist.find(L4_GlobalId(po.gthreadid, 1));
			if (!te) {
				pdbg0("pager: GetEntry on subcatalog was not a managed threadid!\n");
				CORBA_exception_set(_env, ex_not_found, NULL);
				return;
			}
		}

		if (strcmp(name, "status") == 0)
		{
			*server = L4_Myself();
			*object_handle = pager_objectid_t(2, po.gthreadid);
			return;
		}
		else if (strcmp(name, "mapping") == 0)
		{
			*server = L4_Myself();
			*object_handle = pager_objectid_t(3, po.gthreadid);
			return;
		}
		else {
			pdbg0("pager: GetEntry on task subcatalog contained invalid name.\n");
			CORBA_exception_set(_env, ex_not_found, 0);
			return;
		}
		break;

	default:
		pdbg0("pager: GetEntry on unknown directory id %d.\n", po.id);
		CORBA_exception_set(_env, ex_not_found, 0);
		return;
	}
}

IDL4_PUBLISH_PAGER_GETENTRY(pager_GetEntry_implementation);

IDL4_INLINE void pager_EnumerateEntry_implementation(CORBA_Object _caller, const objectid_t directory_handle, const interfaceid_t interfaceid, const L4_Word_t _entrynum, L4_ThreadId_t *server, objectid_t *object_handle, CORBA_char **name, idl4_server_environment *_env)
{
	pdbg2("pager: EnumerateEntry(0x%lx, 0x%lx, %lu) called by 0x%lx\n",
		  directory_handle, interfaceid, _entrynum, _caller.raw);

	pager_objectid_t po = directory_handle;

	if(interfaceid != (interfaceid_t)ANY_INTERFACE) {
		*server = L4_nilthread;
		*object_handle = 0;
		*name = "(error)";
		return;
	}	

	L4_Word_t entrynum = _entrynum;

	switch(po.id)
	{
	case 0:
		// in the root directory
		
		if (entrynum == 0)
		{
			*server = L4_Myself();
			*name = "freelist";
			*object_handle = pager_objectid_t(10, 0);
			return;
		}
		else
		{
			entrynum--;

			const TaskEntry *te = tasklist.begin();

			if (!te)
			{
				*server = L4_nilthread;
				*object_handle = 0;
				*name = "(end)";
				return;
			}

			while(entrynum > 0)
			{
				if (te == te->next)
				{
					*server = L4_nilthread;
					*object_handle = 0;
					*name = "(end)";
					return;
				}

				te = te->next;
				entrynum--;
			}

			*server = L4_Myself();
			*object_handle = pager_objectid_t(1, L4_ThreadNo(te->thread));
			snprintf(*name, 32, "%lx",
					 L4_GlobalId(L4_ThreadNo(te->thread), 1).raw);
			return;
		}
		break;

	case 1:
		// in the one of the task subcatalogs

		{
			struct TaskEntry *te = tasklist.find(L4_GlobalId(po.gthreadid, 1));
			if (!te) {
				pdbg0("pager: GetEntry on subcatalog was not a managed threadid!\n");
				CORBA_exception_set(_env, ex_not_found, NULL);
				return;
			}
		}

		*server = L4_Myself();

		switch(entrynum)
		{
		case 0:
			*object_handle = pager_objectid_t(2, po.gthreadid);
			*name = "status";
			return;
			
		case 1:
			*object_handle = pager_objectid_t(3, po.gthreadid);
			*name = "mapping";
			return;

		default:
			*server = L4_nilthread;
			*object_handle = 0;
			*name = "(end)";
			return;
		}
		break;

	default:
		pdbg0("pager: EnumerateEntry on unknown directory id %d.\n", po.id);
		CORBA_exception_set(_env, ex_not_found, 0);
		return;
	}
}

IDL4_PUBLISH_PAGER_ENUMERATEENTRY(pager_EnumerateEntry_implementation);

#include "MyOStream.h"

static inline bool GetOStream(pager_objectid_t po, mystd::ostringstream &os)
{
	using namespace mystd;

	switch(po.id)
	{
	case 2:
	{
		struct TaskEntry *te = tasklist.find(L4_GlobalId(po.gthreadid, 1));
		if (!te) {
			pdbg0("pager: Read on subcatalog was not a managed threadid!\n");
			return false;
		}

		os << "Task " << hex << te->thread.raw << ":\n"
		   << "\n";

		os << "status: " << (te->status == TS_NEWBORN ? "newborn" :
							 te->status == TS_RUNNING ? "running" :
							 te->status == TS_ZOMBIE ? "zombie" : "invalid")
		   << "\n";

		os << "heaplimit: " << hex << te->heaplimit << "\n";

		if (te->status == TS_ZOMBIE)
			os << "retcode: " << dec << te->retcode << "\n";
		else
			os << "retcode: invalid\n";

		os << "waitfor: " << hex << te->waitfor.raw << "\n";
		
		return true;
	}
	case 3:
	{
		struct TaskEntry *te = tasklist.find(L4_GlobalId(po.gthreadid, 1));
		if (!te) {
			pdbg0("pager: Read on subcatalog was not a managed threadid!\n");
			return false;
		}

		os << "Mappings of task " << hex << te->thread.raw << "\n";

		const struct PageMappingEntry *pme = te->mapping.begin();

		while(pme)
		{
			os << "area " << hex << L4_Address(pme->userpage)
			   << " - " << (L4_Address(pme->userpage) + L4_Size(pme->userpage))
			   << " size " << dec << L4_Size(pme->userpage)
			   << " => " << hex << L4_Address(pme->backing) << " size " << dec << L4_Size(pme->backing)
			   << "\n";

			if (pme->next == pme) break;
			pme = pme->next;
		}
		return true;
	}
	case 10:
	{
		// freelist

		os << "Freelist of the buddy system\n";

		for(unsigned int fln = 0; fln < buddysystem.get_freelistnum(); fln++)
		{
			const PageFreeEntry *iter = buddysystem.get_freelist(fln)->begin();

			while(iter)
			{
				os << "addr " << hex << L4_Address(iter->page) << " size " << dec << L4_Size(iter->page) << "\n";

				if (iter->next == iter) break;
				iter = iter->next;
			}
		}

		os << "Total free memory: " << dec << (buddysystem.available() / 1024) << "kb\n";

		return true;
	}
	}
	return false;
}

static mystd::ostringstream gostringstream;	// this must be static, as it has a static buffer

IDL4_INLINE void pager_Read_implementation(CORBA_Object _caller, const objectid_t filehandle, const uint32_t pos, const idlsize_t readsize, buffer_t *buffer, idl4_server_environment *_env)
{
	pdbg2("pager: Read(0x%lx, %u, %u) called by 0x%lx\n",
		  filehandle, pos, readsize, _caller.raw);

	pager_objectid_t po = filehandle;
	gostringstream.clear();

	if (!GetOStream(po, gostringstream))
	{
		pdbg0("pager: Read on invalid filehandle 0x%lx\n", filehandle);
		buffer->_length = 0;
		CORBA_exception_set(_env, ex_invalid_objectid, 0);
		return;
	}

	assert(readsize <= 8000);

	buffer->_length = gostringstream.read(pos, buffer->_buffer, readsize);
}

IDL4_PUBLISH_PAGER_READ(pager_Read_implementation);

IDL4_INLINE void pager_Write_implementation(CORBA_Object _caller, const objectid_t filehandle, const uint32_t pos, idlsize_t *byteswritten, const buffer_t *buffer, idl4_server_environment *_env)
{
	pdbg0("pager: Write() go away!\n");
	CORBA_exception_set(_env, ex_not_supported, 0);
}

IDL4_PUBLISH_PAGER_WRITE(pager_Write_implementation);

IDL4_INLINE void pager_GetFileSize_implementation(CORBA_Object _caller, const objectid_t filehandle, idlsize_t *filesize, idl4_server_environment *_env)
{
	pdbg0("pager: GetFileSize(0x%lx) called by 0x%lx\n",
		  filehandle, _caller.raw);

	*filesize = 0; // virtual files are 0 size.
}

IDL4_PUBLISH_PAGER_GETFILESIZE(pager_GetFileSize_implementation);
