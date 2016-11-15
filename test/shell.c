
/**

    T2 shell, para teste do T2FS - Sistema de arquivos do trabalho 2 de Sistemas Operacionais I

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"

void cmdExit(void);
void cmdMan(void);
void cmdWho(void);
void cmdCp(void);
void cmdFscp(void);
void cmdCreate(void);
void cmdDelete(void);
void cmdOpen(void);
void cmdClose(void);
void cmdRead(void);
void cmdMkdir(void);
void cmdRmdir(void);
void cmdLs(void);
void cmdTrunc(void);

static void dump(char *buffer, int size) {
    int base, i;
    char c;
    for (base=0; base<size; base+=16) {
        printf ("%04d ", base);
        for (i=0; i<16; ++i) {
            if (base+i<size) printf ("%02X ", buffer[base+i]);
            else printf ("   ");
        }

        printf (" *");

        for (i=0; i<16; ++i) {
            if (base+i<size) c = buffer[base+i];
            else c = ' ';

            if (c<' ' || c>'z' ) c = ' ';
            printf ("%c", c );
        }
        printf ("*\n");
    }
}

int main()
{
    char cmd[256];
    char *token;

    printf ("Testing for T2FS - v 1.0\n");
    cmdMan();

    while (1) {
        printf ("T2FS> ");
        //fgets(cmd, 256, stdin);
        gets(cmd);
        if( (token = strtok(cmd," \t")) != NULL ) {
            if (strcmp(token,"exit")==0) { cmdExit(); break; }
            else if (strcmp(token,"man")==0) cmdMan();
            else if (strcmp(token,"who")==0) cmdWho();
            else if (strcmp(token,"cp")==0)  cmdCp();
            else if (strcmp(token,"fscp")==0) cmdFscp();
            else if (strcmp(token,"create")==0) cmdCreate();
            else if (strcmp(token,"del")==0) cmdDelete();
            else if (strcmp(token,"open")==0) cmdOpen();
            else if (strcmp(token,"close")==0) cmdClose();
            else if (strcmp(token,"read")==0) cmdRead();
            else if (strcmp(token,"mkdir")==0 || strcmp(token,"md")==0) cmdMkdir();
            else if (strcmp(token,"rmdir")==0 || strcmp(token,"rm")==0) cmdRmdir();
            else if (strcmp(token,"ls")==0 || strcmp(token,"dir")==0) cmdLs();
	    else if (strcmp(token,"trunc")==0) cmdTrunc();
            else printf ("???\n");
        }
    }

    return 0;
}

/**
Encerra a opera��o do teste
*/
void cmdExit(void) {
    printf ("bye, bye!\n");
}

/**
Informa os comandos aceitos pelo programa de teste
*/
void cmdMan(void) {
    printf ("man                 -> command help\n");
    printf ("exit                -> finish this shell\n");
    printf ("who                 -> shows T2FS authors\n");
    printf ("create  [file]      -> create new [file] in T2FS\n");
    printf ("del     [file]      -> delete [file] from T2FS\n");
    printf ("open    [file]      -> open [file] from T2FS\n");
    printf ("close   [hdl]       -> close [hdl]\n");
    printf ("read    [hdl] [siz] -> read [siz] bytes from file [hdl]\n");
    printf ("trunc   [hdl] [siz] -> truncate file [hdl] to [size] bytes\n");
    printf ("ls      [pathname]  -> list files in [pathname]\n");
    printf ("md      [pathname]  -> create [pathname] dir in T2FS\n");
    printf ("rm      [pathname]  -> deletes [pathname] dir in T2FS\n");
    printf ("cp      [src] [dst] -> copy files: src -> dst\n");
    printf ("fscp -t [src] [dst] -> copy HostFS -> T2FS\n");
    printf ("fscp -f [src] [dst] -> copy T2FS   -> HostFS\n");
}

/**
Chama da fun��o identify2 da biblioteca e coloca o string de retorno na tela
*/
void cmdWho(void) {
    char name[256];
    int err = identify2(name, 256);
    if (err) {
        printf ("Erro: %d\n", err);
        return;
    }
    printf ("%s\n", name);
}

/**
Copia arquivo dentro do T2FS
Os parametros s�o:
    primeiro parametro => arquivo origem
    segundo parametro  => arquivo destino
*/
void cmdCp(void) {

    // Pega os nomes dos arquivos origem e destion
    char *src = strtok(NULL," \t");
    char *dst = strtok(NULL," \t");
    if (src==NULL || dst==NULL) {
        printf ("Missing parameter\n");
        return;
    }
    // Abre o arquivo origem, que deve existir
    FILE2 hSrc = open2 (src);
    if (hSrc<0) {
        printf ("Open source file error: %d\n", hSrc);
        return;
    }
    // Cria o arquivo de destino, que ser� resetado se existir
    FILE2 hDst = create2 (dst);
    if (hDst<0) {
        close2(hSrc);
        printf ("Create destination file error: %d\n", hDst);
        return;
    }
    // Copia os dados de source para destination
    char buffer[2];
    while( read2(hSrc, buffer, 1) == 1 ) {
        write2(hDst, buffer, 1);
    }
    // Fecha os arquicos
    close2(hSrc);
    close2(hDst);

    printf ("Files successfully copied\n");
}

/**
Copia arquivo de um sistema de arquivos para o outro
Os parametros s�o:
    primeiro parametro => dire��o da copia
        -t copiar para o T2FS
        -f copiar para o FS do host
    segundo parametro => arquivo origem
    terceiro parametro  => arquivo destino
*/
void cmdFscp(void) {
    // Pega a dire��o e os nomes dos arquivos origem e destion
    char *direcao = strtok(NULL, " \t");
    char *src = strtok(NULL," \t");
    char *dst = strtok(NULL," \t");
    if (direcao==NULL || src==NULL || dst==NULL) {
        printf ("Missing parameter\n");
        return;
    }
    // Valida dire��o
    if (strncmp(direcao, "-t", 2)==0) {
        // src == host
        // dst == T2FS

        // Abre o arquivo origem, que deve existir
        FILE *hSrc = fopen(src, "r+");
        if (hSrc==NULL) {
            printf ("Open source file error\n");
            return;
        }
        // Cria o arquivo de destino, que ser� resetado se existir
        FILE2 hDst = create2 (dst);
        if (hDst<0) {
            fclose(hSrc);
            printf ("Create destination file error: %d\n", hDst);
            return;
        }
        // Copia os dados de source para destination
        char buffer[2];
        while( fread((void *)buffer, (size_t)1, (size_t)1, hSrc) == 1 ) {
            write2(hDst, buffer, 1);
        }
        // Fecha os arquicos
        fclose(hSrc);
        close2(hDst);
    }
    else if (strncmp(direcao, "-f", 2)==0) {
        // src == T2FS
        // dst == host

        // Abre o arquivo origem, que deve existir
        FILE2 hSrc = open2 (src);
        if (hSrc<0) {
            printf ("Open source file error: %d\n", hSrc);
            return;
        }
        // Cria o arquivo de destino, que ser� resetado se existir
        FILE *hDst = fopen(dst, "w+");
        if (hDst==NULL) {
            printf ("Open destination file error\n");
            return;
        }
        // Copia os dados de source para destination
        char buffer[2];
        while ( read2(hSrc, buffer, 1) == 1 ) {
            fwrite((void *)buffer, (size_t)1, (size_t)1, hDst);
        }
        // Fecha os arquicos
        close2(hSrc);
        fclose(hDst);
    }
    else {
        printf ("Invalid copy direction\n");
        return;
    }

    printf ("Files successfully copied\n");
}

/**
Cria o arquivo informado no parametro
Retorna eventual sinaliza��o de erro
Retorna o HANDLE do arquivo criado
*/
void cmdCreate(void) {
    FILE2 hFile;

    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }

    hFile = create2 (token);
    if (hFile<0) {
        printf ("Error: %d\n", hFile);
        return;
    }

    printf ("File created with handle %d\n", hFile);
}

/**
Apaga o arquivo informado no parametro
Retorna eventual sinaliza��o de erro
*/
void cmdDelete(void) {

    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }

    int err = delete2(token);
    if (err<0) {
        printf ("Error: %d\n", err);
        return;
    }

    printf ("File %s was deleted\n", token);
}

/**
Abre o arquivo informado no parametro [0]
Retorna sinaliza��o de erro
Retorna HANDLE do arquivo retornado
*/
void cmdOpen(void) {
    FILE2 hFile;

    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }

    hFile = open2 (token);
    if (hFile<0) {
        printf ("Error: %d\n", hFile);
        return;
    }

    printf ("File opened with handle %d\n", hFile);
}

/**
Fecha o arquivo cujo handle � o parametro
Retorna sinaliza��o de erro
Retorna mensagem de opera��o completada
*/
void cmdClose(void) {
    FILE2 handle;

    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }

    if (sscanf(token, "%d", &handle)==0) {
        printf ("Invalid parameter\n");
        return;
    }

    int err = close2(handle);
    if (err<0) {
        printf ("Error: %d\n", err);
        return;
    }

    printf ("Closed file with handle %d\n", handle);
}

void cmdRead(void) {
    FILE2 handle;
    int size;

    // get first parameter => file handle
    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }
    if (sscanf(token, "%d", &handle)==0) {
        printf ("Invalid parameter\n");
        return;
    }

    // get second parameter => number of bytes
    token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }
    if (sscanf(token, "%d", &size)==0) {
        printf ("Invalid parameter\n");
        return;
    }

    // Alloc buffer for reading file
    char *buffer = malloc(size);
    if (buffer==NULL) {
        printf ("Memory full\n");
        return;
    }

    // get file bytes
    int err = read2(handle, buffer, size);
    if (err<0) {
        printf ("Error: %d\n", err);
        return;
    }
    if (err==0) {
        printf ("Empty file\n");
        return;
    }

    // show bytes read
    dump(buffer, err);
    printf ("%d bytes read from file-handle %d\n", err, handle);
}

/**
Cria um novo diretorio
*/
void cmdMkdir(void) {
    // get first parameter => pathname
    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }
    // change working dir
    int err = mkdir2(token);
    if (err<0) {
        printf ("Error: %d\n", err);
        return;
    }

    printf ("Created new directory\n");
}

/**
Apaga um diretorio
*/
void cmdRmdir(void) {
    // get first parameter => pathname
    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }
    // change working dir
    int err = rmdir2(token);
    if (err<0) {
        printf ("Error: %d\n", err);
        return;
    }

    printf ("Directory was erased\n");
}

void cmdLs(void) {

    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }

    // Abre o diret�rio pedido
    DIR2 d;
    d = opendir2(token);
    if (d<0) {
        printf ("Open dir error: %d\n", d);
        return;
    }

    // Coloca diretorio na tela
    DIRENT2 dentry;
    while ( readdir2(d, &dentry) == 0 ) {
        printf ("%c %8u %s\n", (dentry.fileType?'d':'-'), dentry.fileSize, dentry.name);
    }

    closedir2(d);


}


/**
Chama a fun��o truncate2() da biblioteca e coloca o string de retorno na tela
*/
void cmdTrunc(void) {
    FILE2 handle;
    int size;

    // get first parameter => file handle
    char *token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }
    if (sscanf(token, "%d", &handle)==0) {
        printf ("Invalid parameter\n");
        return;
    }

    // get second parameter => number of bytes
    token = strtok(NULL," \t");
    if (token==NULL) {
        printf ("Missing parameter\n");
        return;
    }
    if (sscanf(token, "%d", &size)==0) {
        printf ("Invalid parameter\n");
        return;
    }
    
    // posiciona CP na posicao selecionada
    int err = seek2(handle, size);
    if (err<0) {
        printf ("Error seek2: %d\n", err);
        return;
    }
    
    // trunca
    err = truncate2(handle);
    if (err<0) {
        printf ("Error truncate2: %d\n", err);
        return;
    }

    // show bytes read
    printf ("file-handle %d truncated to %d bytes\n", handle, size );
}


