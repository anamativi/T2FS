
/*************************************************************************

	Funções que realizam a leitura e escrita do subsistema de E/S no disco usado pelo T2FS
	
	Essas funções são realizadas com base nos setores lógicos do disco.
	Os setores são endereçados através de sua numeração sequencial, a partir de ZERO. 
	O setor lógico tem, sempre, 256 bytes (SECTOR_SIZE)
	
	Versão: 16.2
	
*************************************************************************/

#ifndef __apidisk_h__
#define __apidisk_h__

#define SECTOR_SIZE 256

/*------------------------------------------------------------------------
Função:	Realiza leitura de um setor lógico do disco

Entra:	sector -> setor lógico a ser lido, iniciando em ZERO
	buffer -> ponteiro para a área de memória onde colocar os dados lidos do disco
	
Retorna:"0", se a leitura foi realizada corretamente
	Valor diferente de zero, caso tenha ocorrido algum erro.
------------------------------------------------------------------------*/
int read_sector (unsigned int sector, unsigned char *buffer);


/*------------------------------------------------------------------------
Função:	Realiza escrita de um setor lógico do disco

Entra:	sector -> setor lógico a ser escrito, iniciando em ZERO
	buffer -> ponteiro para a área de memória onde estão os dados a serem escritos no disco
	
Retorna:"0", se a leitura foi realizada corretamente
	Valor diferente de zero, caso tenha ocorrido algum erro.
------------------------------------------------------------------------*/
int write_sector (unsigned int sector, unsigned char *buffer);

#endif



