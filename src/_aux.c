#include <stdlib.h>
#include <apidisk.h>
#include <bitmap2.h>
#include <t2fs.h>

char    id[4];          		/* Identificação do sistema de arquivo. É formado pelas letras “T2FS”.             */
WORD    version;        		/* Versão atual desse sistema de arquivos: (valor fixo 0x7E0=2016; 2=2º semestre). */
WORD    superblockSize; 		/* Quantidade de setores que formam o superbloco.                                  */
WORD    freeBlocksBitmapSize; 	/* Quantidade de setores usados para armazenar o bitmap de blocos de dados livres e ocupados. */
WORD    freeInodeBitmapSize;	/* Quantidade de setores usados para armazenar o bitmap de i-nodes livres e ocupados. */
WORD    inodeAreaSize;			/* Quantidade de setores usados para armazenar os i-nodes do sistema.                 */
WORD    blockSize;				/* Quantidade de setores que formam um bloco lógico.                                  */
DWORD   diskSize;				/* Quantidade total de setores na partição T2FS. Inclui o superbloco, áreas de bitmap, área de i-node e blocos de dados */

void info(char *s, char *out)
{

	printf("T2FS INFO : %s", s);

	if (out[0]!='\0') 
		printf("\t (%s)", out);
	
	printf("\n");
	fflush(stdout);

	return;
}

void infoN(char *s, unsigned int out)
{	

	printf("T2FS INFO : %s", s);

	if (out!=NOINFO) 
		printf("\t (%X)", out); // [OBS] "%X" printa hexadecimais em caixa alta!
	
	printf("\n");
	fflush(stdout);

	return;
}

int init()
{
	BYTE buffer[SECTOR_SIZE];
	int ret, i;
	struct t2fs_record *fakeRootRecord;
	
	if (!t2fsReady)
	{
		info("Initialize: Read Superblock...", "\0");
		ret = read_sector(0, (char*)buffer);
		if (ret)
			return fatal("Initialize: Cannot read superblock", NOINFO, 0);

		// 00-03 - 4Bytes - Identificacao do sistema de arquivo. T2FS.
		for (i=0; i<4;i++) // Copia o nome para a estrutura
			superblock.id[i] = (char)buffer[i];	
		
		info("Initialize: FS Type", superblock.id);

		// 04-05 - 2Bytes - Versão atual desse sistema de arquivos: 7E0=2016; 1=primeiro semestre). 
		superblock.version 		= *((WORD*) &(buffer[4])); 
		infoN("Initialize: FS Version", superblock.version);
		
		// 06-07 - 2Bytes - Tamanho do superblock
		superblock.superBlockSize 	= *((WORD*) &(buffer[6]));
		infoN("Initialize: SuperblockSize", superblock.superBlockSize);

		// 08-09 - 2Bytes - /* Quantidade de setores usados para armazenar o bitmap de blocos de dados livres e ocupados. */
		superblock.freeBlocksBitmapSize 		= *((WORD*) &(buffer[8]));
		infoN("Initialize: freeBlocksBitmapSize", superblock.freeBlocksBitmapSize);
		
		// 10-11 - 2Bytes - /* Quantidade de setores usados para armazenar o bitmap de i-nodes livres e ocupados. */
		superblock.freeInodeBitmapSize 		= *((WORD*) &(buffer[10]));
		infoN("Initialize: freeInodeBitmapSize", superblock.freeInodeBitmapSize);

		
		// 12-13 - 2Bytes - /*Quantidade de setores usados para armazenar os i-nodes do sistema.  */
		superblock.inodeAreaSize 		= *((WORD*) &(buffer[12]));
		infoN("Initialize: inodeAreaSize", superblock.inodeAreaSize);
				
		// 14-15- 2Bytes - Tamanho dos blocos (Num de setores logicos contidos)
		superblock.blockSize 		= *((WORD*) &(buffer[14]));
		infoN("Initialize: BlockSize", superblock.blockSize);
		
		// 16-17 - 4Bytes - Tamanho dos discos
		superblock.diskSize 		= *((DWORD*) &(buffer[16]));
		infoN("Initialize: DiskSize", superblock.diskSize);
		

		// Inicializa arquivos e diretorios abertos
		for (i=0; i<MAX_OPENFILES; i++)
		{
			openFiles[i].valid = INVALID_RECORD;
			openDirs[i].valid = INVALID_RECORD;
		}
		
		wdPath = (struct t2fs_path*) malloc2(sizeof(struct t2fs_path));
		wdPath->s[0] = '/'; wdPath->s[1] = '\0';
		wdPath->next = NULL;
		
		// Initialize a fake rootRecord to read the real one using the existing functions
		fakeRootRecord = (struct t2fs_record*) malloc2(sizeof(struct t2fs_record));
		
		fakeRootRecord -> TypeVal = DIRECTORY_RECORD; 
		fakeRootRecord -> name[0] = '.'; fakeRootRecord -> name[1] = '\0'; 
		fakeRootRecord -> blocksFileSize = superblock.rootSize;
		fakeRootRecord -> bytesFileSize = superblock.rootSize * BLOCK_SIZE;
		fakeRootRecord -> dataPtr[0] = (superblock.superBlockSize + superblock.bitmapSize);
		fakeRootRecord -> dataPtr[1] = (superblock.superBlockSize + superblock.bitmapSize) + 1;

		t2fsReady = 1;
		
		// Two possibilities not specified in the specs:
		// 1: The entry . located on the root directory contains dataPtr[0]=rootStart , dataPtr[1]=rootStart+1, singleIndPtr = XXX (dont care, should not happen)
		// 2: The entry . located on the root directory contains dataPtr[0] or [1] = garbage, invalid.
		
		if (recordGet(".", DIRECTORY_RECORD, fakeRootRecord, &rootRecord))
			return warning("Init: Could not find . entry on root dir.", NOINFO, 0);
		
		infoN("Init: root typeval: ", rootRecord->TypeVal);
		info("Init: root name: ", rootRecord->name);
		infoN("Init: root blocksFileSize: ", rootRecord->blocksFileSize);
		infoN("Init: root bytesFileSize: ", rootRecord->bytesFileSize);
		infoN("Init: root dataPtr 0: ", rootRecord->dataPtr[0]);
		infoN("Init: root dataPtr 1: ", rootRecord->dataPtr[1]);
		infoN("Init: root singleIndPtr : ", rootRecord->singleIndPtr);
		infoN("Init: root doubleIndPtr : ", rootRecord->doubleIndPtr);
		
		// Para maior consistencia, atualiza os records necessarios no root.
		
		if (rootRecord->dataPtr[0] != fakeRootRecord->dataPtr[0])
		{
			infoN("Init: Updating root . dataPtr[0] entry on disk to avoid errors...", NOINFO);
			rootRecord->dataPtr[0] = (superblock.superBlockSize + superblock.bitmapSize);
			if (recordUpdate(fakeRootRecord, rootRecord))
				infoN("Init: Root . entry dataPtr[0] contains inconsistent data and we could not fix this. This may or not lead to errors.", NOINFO);
		}
		
		if (rootRecord->dataPtr[1] != fakeRootRecord->dataPtr[1])
		{
			infoN("Init: Updating root . dataPtr[1] entry on disk to avoid errors...", NOINFO);
			rootRecord->dataPtr[1] = (superblock.superBlockSize + superblock.bitmapSize) + 1;
			if (recordUpdate(fakeRootRecord, rootRecord))
				infoN("Init: Root . entry dataPtr[1] contains inconsistent data and we could not fix this. This may or not lead to errors.", NOINFO);
		}
		
		
		gcollector(fakeRootRecord);

		info("Initialize: done.", "\0");
	}
	return 0;
}





