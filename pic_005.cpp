// pic.cpp : Defines the entry point for the console application.
//
/*
 * Kerim BASOL
 */
//#include "stdafx.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stack>
#include "pic_stack.h"
#include "espstd.h"


using namespace std;

#define EOL '\n'
#define DATA 0


#define PIC_DEBUG \
	if ( getch() == 'q' ) \
		exit(0);	
		
#define MEM_DEBUG(str) \
	printf("pc = 0x%0.4x cycle = %0.4d w = 0x%0.2X st = 0x%0.2X %s  \n",pc,cycle, reg_w ,*status_reg, #str); \
	printf("portA = %s portB = %s intcon = %s option_reg = %s\n",convertbin(*porta_reg,8),convertbin(*portb_reg,8),convertbin(*intcon_reg,8), convertbin(*option_reg,8));\
	printf("trisA = %s trisB = %s \n",convertbin(*trisa_reg,8),convertbin(*trisb_reg,8));\
	cout << "tmr0: ";puts(convertbin(*tmr0_reg,8)); \
	mem[SELECT_FILEREG_EMPTY_LOC] = 0; \
	i = -1;/* maskeyi sifirlamak icin */
	  
#define SLEEP_MODE \
	/* Prescaler Kontrol Hatti */\
	if(!(*option_reg & 0x20)/* T0CS */) {/* Dahili komut prescaler artimi */\
		prescaler++;\
	} else { /* harici osilator artimi icin */\
	\
		/* Disari icin ozel fonksiyon yazilacak thread gerekebilir */;\
		\
		\
	}\
	\
	/* prescaler uygun degere ulastiginda tmr0/wdt artimi */\
	if( *option_reg & 0x08 ) { /* wdt artimi icin */\
		if( ((1 << (*option_reg & 0x07)) == prescaler)  ) {\
			if( *tmr0_reg == 0xFF ){\
				*intcon_reg |= 0x04;/* T0IF */\
				tmr0_overflow = true;\
				}\
				*tmr0_reg += 1;\
				prescaler = 0;\
		}\
	}else { /*tmr0 artimi icin */\
		if( ((2 << (*option_reg & 0x07)) == prescaler)  ) {\
				if( *tmr0_reg == 0xFF ){\
				*intcon_reg |= 0x04;/* T0IF */\
				tmr0_overflow = true;\
				}\
				*tmr0_reg += 1;\
				prescaler = 0;\
		}\
	}\
	\
	/* Watchdog ve tmr0 intterrupt rutin check */\
	if( tmr0_overflow == true ){\
		tmr0_overflow = false;\
	if( (0x80 & *intcon_reg) && (*intcon_reg & 0x20) ) { \
		if(*tmr0_reg == 0x00)/* TMR0 0xFF to 0x00 Overflow */ {\
		\
		if( *option_reg & 0x08 /* PSA */) {\
			/* WATCHDOG TIMER */\
			*status_reg &= 0xEF;/* TO sifirlandi*/\
			sleep_mode = false;\
			goto RESET;\
			}else {/* tmr0 intrerrupt check */\
				sleep_mode = false;\
				*intcon_reg &= 0x7F;/* GIE Disable ediliyor. */ \
				stk.push(pc);\
				*pclath_reg = 0;\
				*pcl_reg = 4;\
			}\
		}\
		\
	}\
}

#define INCREMENT_CYCLE \
	cycle++;\
	SLEEP_MODE;
	


typedef struct inhx8m_format
{
	char length;
	short line;
	short type;
	short *data;
	unsigned char checksum;

} inhx8m_format;

/* Siralama buyukten kucuge dogru olmak zorunda */ 

const unsigned int mask[] = { 0xffff,0xff9f,0xff80,0xff00,0xfe00,0xfc00 };

#define COMMANDS 35
#define ADDWF	0x0700
#define ANDWF	0x0500
#define CLRF	0x0180
#define CLRW	0x0100
#define COMF	0x0900
#define DECF	0x0300
#define DECFSZ	0x0b00
#define INCF	0x0a00
#define INCFSZ	0x0f00
#define IORWF	0x0400
#define MOVF	0x0800
#define MOVWF	0x0080
#define	NOP		0x0000
#define	RLF		0x0d00
#define RRF		0x0c00
#define SUBWF	0x0200
#define SWAPF	0x0e00
#define XORWF	0x0600
#define BCF		0x1000
#define BSF		0x1400
#define BTFSC	0x1800
#define BTFSS	0x1c00
#define ADDLW	0x3e00
#define ANDLW	0x3900
#define CALL	0x2000
#define CLRWDT	0x0064
#define	GOTO	0x2800
#define	IORLW	0x3800
#define	MOVLW	0x3000
#define	RETFIE	0x0009
#define RETLW	0x3400
#define RETURN	0x0008
#define	SLEEP	0x0063
#define	SUBLW	0x3c00
#define	XORLW	0x3a00

/* PIC's Real Mems */

#define	SFR_INDF	0x00
#define	SFR_TMR0	0x01
#define	SFR_OPTION	0x01
#define SFR_PCL		0x02
#define	SFR_STATUS	0x03
#define SFR_FSR		0x04
#define SFR_PORTA	0x05
#define SFR_TRISA	0x05
#define	SFR_PORTB	0x06
#define	SFR_TRISB	0x06
#define	SFR_EEDATA	0x08
#define SFR_EECON1	0x08
#define	SFR_EEADR	0x09
#define	SFR_EECON2	0x09
#define	SFR_PCLATH	0x0A
#define	SFR_INTCON	0x0B

/* pic_select_filereg_location function's MACROS */

#define SELECT_SFR_STATUS	0x03
#define SELECT_SFR_STATUS_RP0MASK 32
#define SELECT_SFR_INDF	0x00
#define SELECT_SFR_TMR0	0x01
#define SELECT_SFR_PCL		0x02
#define SELECT_SFR_FSR		0x04
#define SELECT_SFR_PORTA	0x05
#define SELECT_SFR_PORTB	0x06
#define SELECT_SFR_EEDATA	0x08
#define SELECT_SFR_EEADR	0x09
#define SELECT_SFR_PCLATH	0x0A
#define SELECT_SFR_INTCON	0x0B
//extended ram dekiler
#define SELECT_SFR_OPTION	0x50
#define SELECT_SFR_TRISA	0x51
#define SELECT_SFR_TRISB	0x52
#define SELECT_SFR_EECON1	0x53
#define SELECT_SFR_EECON2	0x54

unsigned char *
pic_select_filereg_location(unsigned char *mem, int _operator)
{

/***********************************************************************/
/*                MACRO icinde  LOCAL degisken bulundugu icin          */
/*                           Makro Tanimlamalari                       */

#define SELECT_SFR_STATUS_RP0	mem[0x03] & SELECT_SFR_STATUS_RP0MASK
#define SELECT_SFR_MEM_LOC	( ( SELECT_SFR_STATUS_RP0 & SELECT_SFR_STATUS_RP0MASK) << 2 )
#define SELECT_SFR_SECOND_LOC_SUM	0x50
#define SELECT_FILEREG_EMPTY_LOC	SELECT_SFR_SECOND_LOC_SUM + 5
/***********************************************************************/
	_operator &= 0x7f;

	if(_operator >= 0x00 && _operator <= 0x0B )// SFR(Special File Register) register
	{
		/* status cozumlemesi */
		switch( _operator )
		{

			/* Degisen registler 
			 * Not: Bunlar 2 bank arasinda
			 * sabit olmadiklari icin
			 */

		case 0x01: 

			if(  SELECT_SFR_STATUS_RP0 ){
				return &mem[ SELECT_SFR_OPTION ]; // bank 1
			}else {
				return &mem[ _operator ];  // bank 0
			}

		case 0x05:
			
			if(  SELECT_SFR_STATUS_RP0 ){
				return &mem[ SELECT_SFR_TRISA ];
			}else {
				return &mem[ _operator ];
			}

		case 0x06:
			
			if(  SELECT_SFR_STATUS_RP0 ){
				return &mem[ SELECT_SFR_TRISB ];
			}else {
				return &mem[ _operator ];
			}

		case 0x08:
			
			if(  SELECT_SFR_STATUS_RP0 ){
				return &mem[ SELECT_SFR_EECON1 ];
			}else {
				return &mem[ _operator ];
			}

		case 0x09:
				
			if(  SELECT_SFR_STATUS_RP0 ){
				return &mem[ SELECT_SFR_EECON2 ];
			}else {
				return &mem[ _operator ];
			}

			break;

		default:
			/* Diger Bank'larda ayni olan Registerlar */
			return &mem[_operator];

			break;
		}


	}else if(_operator >= 0x0C && _operator <= 0x4F) // GPR(General Purpose Register) register
	{
		return &mem[_operator];
	}

	/* Kullanilmayan bellek adresi */

	return &mem[SELECT_FILEREG_EMPTY_LOC];

}


void
pic_decompileOpcode(unsigned short mem)
{	
	int i;
	
	for(i = 0; i < 6; i++)
	{	
		switch((mem & mask[i]))
		{
		case ADDWF:
			printf("ADDWF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;

		case ANDWF:
			printf("ADDWF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;

		case CLRF:
			printf("CLRF\t0x%0.3X\n",(mem & 0x007f));
			return;
		
		case CLRW:
			printf("CLRW");
			return;
			
		case COMF:
			printf("COMF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;
		
		case DECF:
			printf("DECF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;
			
		case DECFSZ:
			printf("DECFSZ\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;

		case INCF:
			printf("INCF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;
			
		case INCFSZ:
			printf("INCFSZ\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;
			
		case IORWF:
			printf("IORWF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
				return;
			break;

		case MOVF:
			printf("MOVF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;

		case MOVWF:
			printf("MOVWF\t0x%0.3X\n",(mem & 0x007f));
			return;
			
		case NOP:
			puts("NOP");
			return;
			
		case RLF:
			printf("RLF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;
			
		case RRF:
			printf("RRF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;
			
		case SUBWF:
			printf("SUBWF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;
		
		case SWAPF:
			printf("SWAPF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;

		case XORWF:
			printf("XORWF\t0x%0.3X,0x%X\n",(mem & 0x007f),((mem & 0x0080)?1:0));
			return;
		
		case BCF:
			printf("BCF\t0x%0.3X,0x%X\n",(mem & 0x007f), (mem & 0x0380)>> 7);
			return;
			

		case BSF:
			printf("BSF\t0x%0.3X,0x%X\n",(mem & 0x007f), (mem & 0x0380)>> 7);
			return;

		case BTFSC:
			printf("BTFSC\t0x%0.3X,0x%X\n",(mem & 0x007f), (mem & 0x0380)>> 7);
			return;
			break;

		case BTFSS:
			printf("BTFSS\t0x%0.3X,0x%X\n",(mem & 0x007f), (mem & 0x0380)>> 7);
			return;
			
		case ADDLW:
			printf("ADDLW\t0x%0.3X\n",(mem & 0x00ff));
			return;
			

		case ANDLW:
			printf("ANDLW\t0x%0.3X\n",(mem & 0x00ff));
			return;

		case CALL:
			printf("CALL\t0x%0.3X\n",(0x7ff & mem) );
			return;

		case RETURN:
			puts("RETURN");
			return;

		case CLRWDT:
			puts("CLRWDT");
			return;

		case GOTO:
			printf("GOTO\t0x%0.3X\n",(0x07ff & mem) );
			return;
			
		case IORLW:
			printf("IORLW");
			return;

		case MOVLW:
			printf("MOVLW\t0x%0.3X\n",(mem & 0x00ff));
			return;

		case RETFIE:
			puts("RETFIE");
			return;

		case RETLW:
			printf("RETLW\t0x%0.3X\n",(mem & 0x00ff));
			return;
			
		case SLEEP:
			puts("SLEEP");
			return;
		
		case SUBLW:
			printf("SUBLW\t0x%0.3X\n",(0x00ff & mem));
			return;
			
		case XORLW:
			printf("XORLW\t0x%0.3X\n",(0x00ff & mem));
			return;
		
		default:
			break;

		}
	}
}
/*inhx16 formatinda kullanilmayacaktir, sadece inhx8m icin gecerli*/
void 
swap2byte(char *swp)
{
	char tmp;
	tmp = swp[0];
	
	swp[0] = swp[2];
	swp[2] = tmp;

	tmp = swp[1];

	swp[1] = swp[3];
	swp[3] = tmp;


}

int
read_file(FILE *fp, char *str, int nmb)
{
		int chr;
		
		
		int i = 0;
		while( (chr = fgetc(fp)) != EOF )
		{
			if( i == nmb ){
				ungetc(chr,fp);
				break;
			}

			if( chr == EOL )
				return -1;
			
			str[i++] = chr;
		
		}
			
			str[i] = '\0';

		return 0;

}

int
parse_inhx8m(char *fname,unsigned short *mem, int mem_len)
{
	FILE *fp;
	int chr;
	int i = 0;
	int pc = 0;// program counter
	int str_len;

	unsigned char checksum = 0;
	inhx8m_format fmt;
	char str_num[10];
	char str2[5];

	if( fname == NULL ) {
		cout << "0" << endl;
		return -1;
	}

	if( !( fp = fopen(fname,"r") ) ) {
		
		cout << "Girilen dosya adi bulunamadi! Tekrar deneyin." << endl;
		
		return -1;
	}
	
	while( ( chr = fgetc(fp) ) != EOF ) 
	{
		/*
		 * Beginning of the inhx8m format
		 */
		
		checksum = 0;

		if( chr == ':' )
		{	
		//	puts("------------------");
			/* length */
			read_file(fp,str_num,2);
			
			fmt.length = strtol(str_num, NULL, 16);
			checksum += fmt.length;

		//	printf("fmt.length = %d\n",fmt.length);
			
			/* line */

			read_file(fp, str_num, 2);
			
			checksum += strtol(str_num, NULL, 16);

			//fmt.line = strtol(str_num, NULL, 16);

			read_file(fp, str2, 2);
			
			checksum += strtol(str2, NULL, 16);

			strncat(str_num, str2, 2);

			fmt.line = strtol(str_num, NULL, 16);

		//	printf("fmt.line = %d\n", fmt.line);
			
			/* type */
			read_file(fp, str_num, 2);
			
			fmt.type = strtol(str_num, NULL, 16);
			
			checksum += fmt.type;
			
		//	printf("fmt.type = %d\n", fmt.type);
			
			/* data */
			memset(str2, 0, sizeof(str2));
			for(i = 0; i < fmt.length && pc < mem_len; i++)
			{				
				read_file(fp, str_num,2);
				strncat(str2,str_num,2);
				checksum += strtol(str_num, NULL, 16);

				checksum%=256;// toplanan komutlarin toplami 255 i asamaz.
				str_len = strlen(str2);				
				
				if( str_len >= 4)
				{
					swap2byte(str2);// littleendian2bigendian
		
					if(fmt.type == DATA){
						mem[pc++] = (unsigned short)strtol(str2, NULL, 16);
					}
					memset(str2,0,sizeof(str2));
				}
				

			}

			/* checksum */

			read_file(fp, str_num, 2);

			fmt.checksum = strtol(str_num, NULL, 16);
			
			if( (255 - checksum + 1) != fmt.checksum )
				return -1;

		//	puts("------------------");

		}
	}
	
//	cout << "pc = " << pc << endl;
	fclose(fp);
	return pc;


}

void
pic_mem_execute(unsigned short *mem, int memsize)
{

	
	int i,j,pc = 0,cycle = 0;
	char eerom_cycle = 2;			//ayarlanabilir olacagi icin	
	bool eerom_key;					//yazma kontrolu icin
	bool sleep_mode = false;		//ilk pic acildiginda sleep modunda olmayacak
	unsigned short carry_check = 0;	//carry kontrolu icin	
	unsigned short prescaler = 0;
	unsigned char tmp;				// bit islemleri icin gecici
	unsigned short tmp_pc;
	unsigned char *tmp_file_reg;
		
	register unsigned char reg_w = 0;

	pic_stack stk;

	unsigned char *file_reg = (unsigned char *)calloc(0x54, sizeof(unsigned char)); //file registerlarini olustur
	
	unsigned char *eeprom_field = (unsigned char *)calloc (0x3F, sizeof (unsigned char)); //eeprom'a yer ac 64x8

	
	/**********************************************************/
	/*                   Ortak BANK Secimi                    */
	/**********************************************************/

	unsigned char *indf_reg	= pic_select_filereg_location(file_reg,SFR_INDF);
	unsigned char *status_reg = pic_select_filereg_location(file_reg,SFR_STATUS);
	unsigned char *pcl_reg = pic_select_filereg_location(file_reg,SFR_PCL);
	unsigned char *fsr_reg = pic_select_filereg_location(file_reg,SFR_FSR);
	unsigned char *pclath_reg = pic_select_filereg_location(file_reg,SFR_PCLATH);
	unsigned char *intcon_reg = pic_select_filereg_location(file_reg,SFR_INTCON);

	/**********************************************************/
	/*                    BANK 0  Secimi                      */
	/**********************************************************/

	*status_reg |= 0x00;//BANK0 ?????????????

	unsigned char *tmr0_reg = pic_select_filereg_location(file_reg,SFR_TMR0);
	unsigned char *porta_reg = pic_select_filereg_location(file_reg,SFR_PORTA);
	unsigned char *portb_reg = pic_select_filereg_location(file_reg,SFR_PORTB);
	unsigned char *eedata_reg = pic_select_filereg_location(file_reg,SFR_EEDATA);
	unsigned char *eeadr_reg = pic_select_filereg_location(file_reg,SFR_EEADR);
	
	/**********************************************************/
	/*                    BANK 1  Secimi                      */
	/**********************************************************/

	*status_reg = 0xFF;//BANK1
	
	unsigned char *trisa_reg = pic_select_filereg_location(file_reg,SFR_TRISA);
	unsigned char *trisb_reg = pic_select_filereg_location(file_reg,SFR_TRISB);
	unsigned char *eecon1_reg = pic_select_filereg_location(file_reg,SFR_EECON1);
	unsigned char *eecon2_reg = pic_select_filereg_location(file_reg,SFR_EECON2);
	unsigned char *option_reg = pic_select_filereg_location(file_reg,SFR_OPTION);
	
/*************************************************/
/*           PIC icin POWER ON degerleri         */
/*************************************************/
	bool tmr0_overflow  = false;

POWERUP:	

	*indf_reg = 0;
	*tmr0_reg = 0;
	*pcl_reg = 0;
	*status_reg = 0x18;
	*fsr_reg = 0;
	*porta_reg = 0;
	*portb_reg = 0;
	*eedata_reg = 0;
	*eeadr_reg = 0;
	*pclath_reg = 0;//program counter'a  baglanacak
	*intcon_reg = 0; 
	

	*option_reg = 0xFF;	
	*trisa_reg = 0x1F;
	*trisb_reg = 0xFF;	
	*eecon1_reg = 0;
	*eecon2_reg = 0;
			
	goto START;

	
/*************************************************/
/*           PIC icin RESET degerleri            */
/*************************************************/
	
RESET:

	*indf_reg = 0;
//  *tmr0_reg = 0;
	*pcl_reg = 0;
	*status_reg = 0x18 ; /* MCLR baglanirsa degisecek */
//	*fsr_reg = 0;  
//	*porta_reg = 0;
//	*portb_reg = 0;
//	*eedata_reg = 0;
//	*eeadr_reg = 0;
	*pclath_reg = 0;//program counter'a  baglanacak
	*intcon_reg &= 0x01; 
	

	*option_reg = 0xFF;	
	*trisa_reg = 0x1F;
	*trisb_reg = 0xFF;	
	*eecon1_reg = 0; /* write err icin kontrol ? */
//	*eecon2_reg = 0;

	goto START;

/****************************************/
/*      PIC ile degil ama Kodun kendine */
/*      ait reset degerleri             */
/****************************************/

START:
	
	sleep_mode = false;
	unsigned char portb_const = 0;	//Baslangicta INT olusmamasi (portb_reg = 0)
									// bu iki degisken degerini ayni
									// tutuyoruz......

	/*******************************************/
	/*             Opcode Decode               */
	/*******************************************/

	for(i = 0;;i++)
	{	
		
		
		tmp = 0; // bit islemleri icin gecici
		
		/***************************************/
		/* Buraya bu kontrol'un koyulma sebebi */
		/*	adres decode etme isleminin ilk    */
		/*  ilk decode maskesinde butun        */
		/*  kotrollerin saglanmasi.            */
		/***************************************/

		if( i == 0) {
			
			/**********************************************************/
			/*                    PC Check                            */
			/**********************************************************/

			if( pc > memsize )//Bellek tasmasi pic16f84 icin
				pc -= (memsize + 1);
			
			pc &= 0xFF00;
			pc |= *pcl_reg;
			
			*pclath_reg = 0x0000;
			*pclath_reg |= (pc >> 8);

			/***********************************************************/
			/*                    Indirect Address Check               */
			/***********************************************************/
			
			fsr_reg = pic_select_filereg_location(file_reg, *indf_reg);

			/***********************************************************/
			/*                    EEROM Check                          */
			/***********************************************************/
		
			if( *eecon1_reg & 0x01 ) {//read check
				*eedata_reg = ((*eeadr_reg > 0x3F)?0:eeprom_field[*eeadr_reg]);
				*eecon1_reg &= 0xFE;//read bit '0'
				++cycle;
			}

			if( *eecon1_reg & 0x03 ) {// write check
				if( eerom_key ){//0x55 set edildiyse bir onceki instruction'da
					
					if( *eecon2_reg == 0xAA){//0x55 0xAA kosulu saglandiysa yaz
						
						if( *eeadr_reg <= 0x3F ){//adres siniri icinde ise yaz
							eeprom_field[*eeadr_reg] = *eedata_reg;
							if((0x80 & *intcon_reg) && (*intcon_reg & 0x40)){
								/**************************/
								/* EE2ROM INTERRUPT CHECK */
								/**************************/
								*intcon_reg &= 0x7F;//GIE Disable ediliyor.
					
								stk.push(pc);
								*pclath_reg = 0x00;
								*pcl_reg = 0x04;
	
							}
						}
					
						/* kullanici kendi eli ile cycle ayarlayacak */
						/* cunku uC icinde bu cycle Voltage ve Isýya */
						/* gore degismekte */
						cycle += eerom_cycle;
					}
				
					eerom_key = 0;
	
				}else if( *eecon2_reg == 0x55 ){ //eerom_key set ediliyorki
					                             // diger cycle'da pic'e
					                             // yazma seq'de kontrol icin
					eerom_key = 1;
				}
			}
		
			/**************************************************************/
			/*                         INTERRUPT KONTROLU                 */
			/**************************************************************/
			
			if( 0x80 & *intcon_reg ) {// INT INTCON Register'da set edildiyse
				
				// Not: EEROM INTERRUPT Check ve TMR0 burda degilller
				// TMR0 instruction'lar icine gomuldu
				// EEROM ise kendi WRITE blogu icinde
				
				/**********************/
				/* RB0 Interrupt check*/
				/**********************/

				if( portb_const != (*portb_reg & 0xF0)//port icerigi degisiyorsa
					&& *intcon_reg & 0x09 ) {	      //PORTB interrupt'i set edildiyse
				
					*intcon_reg &= 0x7F;//GIE Disable ediliyor...
					portb_const = (*portb_reg & 0xF0);
					stk.push(pc);
					*pclath_reg = 0;
					*pcl_reg = 4;
				
				}else if(*intcon_reg & 0x12) {
			
				/******************************/
				/* RB0/INT ucu Interrupt check*/
				/******************************/

					*intcon_reg &= 0x7F;//GIE Disable ediliyor...

					stk.push(pc);
					*pclath_reg = 0;
					*pcl_reg = 4;
					/* OPTION REG SET EDILECEK */	
				}
		
			}

		}
		
		
		/*****************************************************************/
		/*                      INSTRUCTION DECODE                       */
		/*****************************************************************/
		
	if( sleep_mode == false )
		switch((mem[pc] & mask[i%6])) // maske no 0-5 arasi olmali.
		{
		case ADDWF:
			
			PIC_DEBUG;
			MEM_DEBUG(ADDWF);
			tmp_pc = pc;//tmp'ye pc degeri korumak icin
			*pcl_reg =(unsigned char)++pc;

			tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);
			
			*status_reg &= 0xfe; // carry bitini sifirla
			carry_check = *tmp_file_reg + reg_w; //ikisini onceden topla ve carry tesbit et
			*status_reg |= (carry_check >> 8); // carry set ediliyor

			if (!carry_check)							
				*status_reg |= 0x04; // zero bitini set et 

			if (carry_check >= 0x0F)
				*status_reg |= 0x02; // digit carry bitini set et 

			if( (mem[tmp_pc] & 0x80) )
				*tmp_file_reg += reg_w;
			else 
				reg_w += *tmp_file_reg;
			
			INCREMENT_CYCLE
			break;

		case ANDWF:
			
			PIC_DEBUG;
			MEM_DEBUG(ANDWF);
			tmp_pc = pc;//tmp'ye pc degeri korumak icin
			*pcl_reg =(unsigned char)++pc;

			if (!*pic_select_filereg_location(file_reg,mem[tmp_pc]) & reg_w)
				*status_reg |= 0x04; // zero bitini set et 

			if( (mem[tmp_pc] & 0x80) )
				*pic_select_filereg_location(file_reg,mem[tmp_pc]) &= reg_w;
			else 
				reg_w &= *pic_select_filereg_location(file_reg,mem[tmp_pc]);
			
			
			INCREMENT_CYCLE;
			break;

		case CLRF:
			
			PIC_DEBUG;
			MEM_DEBUG(CLRF);
			*pcl_reg =(unsigned char)++pc;

			*pic_select_filereg_location(file_reg,mem[pc-1]) = 0;						
			*status_reg |= 0x04; // zero bitini set et 
			
			
			INCREMENT_CYCLE;
			break;
		
		case CLRW:
			
			PIC_DEBUG;
			MEM_DEBUG(CLRW);
			*pcl_reg =(unsigned char)++pc;

			reg_w = 0;						
			*status_reg |= 0x04; // zero bitini set et 
			
			INCREMENT_CYCLE;
			break;
			
		case COMF:
			
			PIC_DEBUG;
			MEM_DEBUG(COMF);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			if( (mem[tmp_pc] & 0x80) ) {
				tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);
				*tmp_file_reg = ~ *tmp_file_reg;
				if (!*tmp_file_reg)
					*status_reg |= 0x04; // zero bitini set et 
				
			} else{
				reg_w = ~ *pic_select_filereg_location(file_reg,mem[tmp_pc]);
				if (!reg_w)
					*status_reg |= 0x04; // zero bitini set et 
			}
			
			
			
			INCREMENT_CYCLE;
			break;
		
		case DECF:

			PIC_DEBUG;
			tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);
			
			MEM_DEBUG(DECF);
			tmp_pc = pc;// tmp degeri pc degerini korumak icin
			*pcl_reg =(unsigned char)++pc;

			if( (mem[tmp_pc] & 0x80) ){
				*tmp_file_reg -= 1;
				if (!*tmp_file_reg)
					*status_reg |= 0x04; // zero bitini set et 
			}else{				
				*tmp_file_reg -= 1;
				reg_w = *tmp_file_reg;
				if (!reg_w)
					*status_reg |= 0x04; // zero bitini set et 
			}
			
			INCREMENT_CYCLE;
			break;		
			
		case DECFSZ:
			
			PIC_DEBUG;
			
			if( (mem[pc] & 0x80) )
				tmp = --*pic_select_filereg_location(file_reg,mem[pc]);
			else
				tmp = reg_w = *pic_select_filereg_location(file_reg,mem[pc]) - 1;
			
			MEM_DEBUG(DECFSZ);

			if( tmp ) {
				*pcl_reg =(unsigned char)++pc;
				INCREMENT_CYCLE;
			}else {
				pc += 2;
				INCREMENT_CYCLE;
				INCREMENT_CYCLE;
			}

			break;

		case INCF:

			PIC_DEBUG;
			MEM_DEBUG(INCF);
			tmp_pc = pc;// tmp degeri pc degerini korumak icin
			*pcl_reg =(unsigned char)++pc;

			if( (mem[tmp_pc] & 0x80) ){
				(*pic_select_filereg_location(file_reg,mem[tmp_pc]))++;
				if (!*pic_select_filereg_location(file_reg,mem[tmp_pc]))
					*status_reg |= 0x04; // zero bitini set et
			}else{
				reg_w = *pic_select_filereg_location(file_reg,mem[tmp_pc]) + 1;
				if (!reg_w)
					*status_reg |= 0x04; // zero bitini set et 
			}
			
			INCREMENT_CYCLE;
			break;
			
		case INCFSZ:
			
			PIC_DEBUG;
			
			if( (mem[pc] & 0x80) )
				tmp = ++*pic_select_filereg_location(file_reg,mem[pc]);
			else
				tmp = reg_w = *pic_select_filereg_location(file_reg,mem[pc]) + 1;

			MEM_DEBUG(INCFSZ);

			if( tmp ) {
				*pcl_reg =(unsigned char)++pc;
				INCREMENT_CYCLE;
			} else {
				pc += 2;
				INCREMENT_CYCLE;
				INCREMENT_CYCLE;
			}

			break;
			
		case IORWF:
			
			PIC_DEBUG;
			MEM_DEBUG(IORWF);
			tmp_pc = pc;// tmp degeri pc degerini korumak icin
			*pcl_reg =(unsigned char)++pc;

			if( (mem[tmp_pc] & 0x80) ) {
				tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);
				*tmp_file_reg |= *tmp_file_reg;
				if (!*tmp_file_reg)
					*status_reg |= 0x04; // zero bitini set et			
			}
			else{
				reg_w |= *pic_select_filereg_location(file_reg,mem[tmp_pc]);
				if (!reg_w)
					*status_reg |= 0x04; // zero bitini set et
			}
			
			INCREMENT_CYCLE;
			break;
			

		case MOVF:
			
			PIC_DEBUG;
			MEM_DEBUG(MOVF);
			tmp_pc = pc;// tmp degeri pc degerini korumak icin
			*pcl_reg =(unsigned char)++pc;

			if( (mem[tmp_pc] & 0x80) ) {
				/* Ayni adres icerikleri kopyalandigi icin 
				 * Hicbirsey yapmaya gerek yok
				 */
				if (!*pic_select_filereg_location(file_reg,mem[tmp_pc]))
					*status_reg |= 0x04; // zero bitini set et				

			}else {
				reg_w = *pic_select_filereg_location(file_reg,mem[tmp_pc]);
				if (!reg_w)
					*status_reg |= 0x04; // zero bitini set et
			}
			
			INCREMENT_CYCLE;
			break;

		case MOVWF:						
			
			PIC_DEBUG;
			MEM_DEBUG(MOVWF);
			tmp_pc = pc;// tmp degeri pc degerini korumak icin
			*pcl_reg =(unsigned char)++pc;

			*pic_select_filereg_location(file_reg,mem[tmp_pc]) = reg_w;
			
			INCREMENT_CYCLE;
			break;
			
		case NOP:
			
			PIC_DEBUG;
			
			MEM_DEBUG(NOP);
			*pcl_reg =(unsigned char)++pc;
			INCREMENT_CYCLE;
			break;
			
		case RLF: 
			
			PIC_DEBUG;
			MEM_DEBUG(RLF);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;
			
			tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

			tmp = *status_reg & 0x01; // carry bitini al			

			if( (mem[tmp_pc] & 0x80) ) {				
				
				carry_check = ((*tmp_file_reg & 0x80)>>7) ; // 00000001 yada 0
				
				*tmp_file_reg <<= 1;
				*tmp_file_reg |= tmp;// status carry tasi
				*status_reg &= 0xfe; // carry kullanilinca bosalt
				
				if ( carry_check )
					*status_reg |= 0x01;//status reg carry set ediliyor
				
			}else {				
				
				carry_check = ((*tmp_file_reg & 0x80)>>7) ; // 00000001 yada 0
				
				reg_w = (*tmp_file_reg << 1) | tmp;				
				*status_reg &= 0xfe; // carry kullanilinca bosalt
				
				if ( carry_check )
					*status_reg |= 0x01;//status reg carry set ediliyor				
			}
			
			INCREMENT_CYCLE;
			break;
			
		case RRF:
			
			// her rotate de msb carry'e
			// carry de lsb'ye tasinacak

			PIC_DEBUG;
			MEM_DEBUG(RRF);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

			tmp = (*status_reg & 0x01) << 7; // carry bitini al			

			if( (mem[tmp_pc] & 0x80) ) {				
				
				carry_check = ((*tmp_file_reg & 0x01) << 7) ; // 1000000 yada 0
				
				*tmp_file_reg >>= 1;
				*tmp_file_reg |= tmp;// status carry tasi
				*status_reg &= 0xfe; // carry kullanilinca bosalt

				if ( carry_check ) // ??
					*status_reg |= 0x01;//status reg carry set ediliyor
				
			}else {				
				
				carry_check = ((*tmp_file_reg & 0x01) << 7) ; // 10000000 yada 0
				
				reg_w = (*tmp_file_reg >> 1) | tmp;
				*status_reg &= 0xfe; // carry kullanilinca bosalt
				
				if ( carry_check )
					*status_reg |= 0x01;//status reg carry set ediliyor				
			}
			
			INCREMENT_CYCLE;
			break;
			
		case SUBWF:
			
			PIC_DEBUG;
			tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);
			
			MEM_DEBUG(SUBWF);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			*status_reg &= 0xfe;
			carry_check = *tmp_file_reg - reg_w;
			carry_check &= 0x100;
			*status_reg |= (carry_check >> 8); // carry set ediliyor

			if (carry_check >= 0x0F)
				*status_reg |= 0x02; // digit carry bitini set et 

			if( (mem[tmp_pc] & 0x80) ){
				*tmp_file_reg -= reg_w;
				if (!*tmp_file_reg)
					*status_reg |= 0x04;
			}else{ 
				reg_w = *tmp_file_reg - reg_w;
				if (!reg_w)
					*status_reg |= 0x04;
			}
								
			INCREMENT_CYCLE;
			break;
		
		case SWAPF:

			PIC_DEBUG;

			tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);
			
			MEM_DEBUG(SWAPF);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			if( (mem[tmp_pc] & 0x80) ){
				
				tmp = *tmp_file_reg & 0xf0; //ilk nibble
				tmp >>= 4;				
				*tmp_file_reg = ((*tmp_file_reg & 0x0f) << 3) | tmp;
			}
			
			else {		
				tmp = *tmp_file_reg & 0xf0;
				tmp >>= 4;				
				reg_w = ((*tmp_file_reg & 0x0f) << 3) | tmp;
			}			
					
			INCREMENT_CYCLE;
			break;

		case XORWF:

			PIC_DEBUG;
			MEM_DEBUG(XORWF);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			if( (mem[tmp_pc] & 0x80) )
			{
				tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);
				*tmp_file_reg ^= *tmp_file_reg;
				if(!*tmp_file_reg)
					*status_reg |= 0x04;
			}else {
				reg_w ^= *pic_select_filereg_location(file_reg,mem[tmp_pc]);
				if (!reg_w)
					*status_reg |= 0x04;
			}
			
			INCREMENT_CYCLE;
			break;
		
		case BCF:

			PIC_DEBUG;
			MEM_DEBUG(BCF);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			/* Test edildi onaylandi */

			tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

			tmp = ( mem[tmp_pc] & 0x380 ) >> 7; // kacinci bitin sifirlanacagini bul
			tmp = (tmp?(1 << (tmp)):1); // 0 kontrolu
			tmp = ~tmp;

			*tmp_file_reg &= tmp;	

			INCREMENT_CYCLE;
			break;
			

		case BSF:
			
			PIC_DEBUG;
			
			/* Test edildi onaylandi */
			tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);
			
			MEM_DEBUG(BSF);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			tmp = ( mem[tmp_pc] & 0x380 ) >> 7; // kacinci bitin sifirlanacagini bul
			tmp = (tmp?(1 << (tmp)):1); // 0 kontrolu

			*tmp_file_reg |= tmp;
			
			INCREMENT_CYCLE;
			break;

		case BTFSC:
			
			PIC_DEBUG;
			
			tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);

			tmp = ( mem[pc] & 0x380 ) >> 7; // kacinci bitin sifirlanacagini bul
			tmp = ( tmp?(1 << (tmp)):1 ); // 0 kontrolu

			MEM_DEBUG(BTFSC);

			if( tmp ) {
				*pcl_reg =(unsigned char)++pc;
				INCREMENT_CYCLE;
			} else {
				pc += 2;
				INCREMENT_CYCLE;
				INCREMENT_CYCLE;
			}
			break;
			

		case BTFSS:

			PIC_DEBUG;

			tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);

			tmp = ( mem[pc] & 0x380 ) >> 7; // kacinci bitin sifirlanacagini bul
			tmp = ( tmp?(1 << (tmp)):1 ); // 0 kontrolu
			
			MEM_DEBUG(BTFSS);
			
			if( tmp ) {
				*pcl_reg = pc += 2;
				INCREMENT_CYCLE;
				INCREMENT_CYCLE;
			} else {
				*pcl_reg =(unsigned char)++pc;
				INCREMENT_CYCLE;
			}

			break;
			
		case ADDLW:

			PIC_DEBUG;
			MEM_DEBUG(ADDLW);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			*status_reg &= 0xfe;
			carry_check = (mem[tmp_pc] & 0xff) + reg_w;
			*status_reg |= (carry_check >> 8); // carry set ediliyor

			if (carry_check >= 0x0F)
				*status_reg |= 0x02; // digit carry bitini set et 
			
			reg_w += (mem[tmp_pc] & 0xff);
			if (!reg_w)
				*status_reg |= 0x04;
			
			INCREMENT_CYCLE;
			break;			

		case ANDLW:

			PIC_DEBUG;
			MEM_DEBUG(ANDLW);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			reg_w &= (mem[tmp_pc] & 0xff);
			if (!reg_w)
				*status_reg |= 0x04;
			
			INCREMENT_CYCLE;
			break;

		case CALL:
			
			PIC_DEBUG;

			stk.push(pc);
			
			MEM_DEBUG(CALL);
			*pcl_reg = pc = ( mem[pc] & 0x07ff );
			INCREMENT_CYCLE;
			INCREMENT_CYCLE;
			break;

		case RETURN:
			
			PIC_DEBUG;
			
			MEM_DEBUG(RETURN);
			*pcl_reg = pc = stk.pop() + 1;
			INCREMENT_CYCLE;
			INCREMENT_CYCLE;
			break;

		case CLRWDT: // status set edilecek
			
			PIC_DEBUG;
			
			*status_reg |= 0x18; // status icindeki 3. ve 4. flag set ediliyor...s

			MEM_DEBUG(CLRWDT);
			*pcl_reg =(unsigned char)++pc;
			INCREMENT_CYCLE;
			INCREMENT_CYCLE;
			break;

		case GOTO:
			
			PIC_DEBUG;
			
			MEM_DEBUG(GOTO);
			*pcl_reg = pc = ( mem[pc] & 0x07ff );			
			INCREMENT_CYCLE;
			INCREMENT_CYCLE;
			break;
			
		case IORLW:

			PIC_DEBUG;
			MEM_DEBUG(IORLW);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;
			
			reg_w |= (mem[tmp_pc] & 0xff);
			if (!reg_w)
				*status_reg |= 0x04;
			
			INCREMENT_CYCLE;
			INCREMENT_CYCLE;
			break;

		case MOVLW:

			PIC_DEBUG;
			MEM_DEBUG(MOVLW);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			reg_w = (mem[tmp_pc] & 0xff);

			INCREMENT_CYCLE;
			break;

		case RETFIE:
			
			PIC_DEBUG;

			*intcon_reg |= 0x80;

			MEM_DEBUG(RETFIE);
			*pcl_reg = pc = stk.pop();
			INCREMENT_CYCLE;
			INCREMENT_CYCLE;
			break;

		case RETLW:
			
			PIC_DEBUG;
			
			reg_w = mem[pc] & 0xff;
			
			MEM_DEBUG(RETLW);

			*pcl_reg = pc = stk.pop() + 1;

			INCREMENT_CYCLE;
			INCREMENT_CYCLE;
			break;
			
		case SLEEP:
			
			PIC_DEBUG;
			
			*status_reg |= 0x10;/* TO SET */
			*status_reg &= 0xF7;/* PD CLEAR */
			*tmr0_reg = 0;
			prescaler = 0;
			sleep_mode = true;
			MEM_DEBUG(SLEEP);
			*pcl_reg =(unsigned char)++pc;
			INCREMENT_CYCLE;
			break;
		
		case SUBLW:

			PIC_DEBUG;
			MEM_DEBUG(SUBLW);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;
			
			*status_reg &= 0xfe;
			carry_check = (mem[tmp_pc] & 0xff) - reg_w;
			carry_check &= 0x100;
			*status_reg |= (carry_check >> 8); // carry set ediliyor

			if (carry_check >= 0x0F)
				*status_reg |= 0x02; // digit carry bitini set et 

			reg_w = (mem[tmp_pc] & 0xff) - reg_w;
			if (!reg_w)
				*status_reg |= 0x04;
			
			INCREMENT_CYCLE;			
			break;
			
		case XORLW:

			PIC_DEBUG;
			MEM_DEBUG(XORLW);
			tmp_pc = pc;
			*pcl_reg =(unsigned char)++pc;

			reg_w ^= (mem[tmp_pc] & 0xff) ;
			if (!reg_w)
				*status_reg |= 0x04;
			
			INCREMENT_CYCLE;
			break;
		
		default:
			break;

		}
	else { 
		
		printf("Sleep mode'da tmr0 = 0x%02X\r", *tmr0_reg);
		SLEEP_MODE;
		i = -1;
		for(j = 0;j < 10000000;j++);
	}

	}//for=1

}

/* Uyari */

int 
main(int argc, char** argv)
{

	system("cls");
	
	//argv[1] = "C:\\tezz\\sleep.hex";
	//argc = 3;
	int ret;
	
	unsigned short *pic_mem;
	unsigned short *pic_mem2;

	int i;
	
	if( argc < 2 ){
		fprintf(stderr,"Lutfen Decompile dosya adini girin!\n"
					   "%s <dosya_adi>\n",argv[0]);

		return 1;
	}

	if( !( pic_mem = (unsigned short *)malloc(sizeof(short)*1024) ) )
	{
		fprintf(stderr, "not enough memory!\n");
		return 1;
	}

	ret = parse_inhx8m(argv[1],pic_mem,1024);
	
	if( ret < 0 ){
		fprintf(stderr, "%s adinda bir dosya bulunamadi!\n",argv[1]);
		return 1;
	}

	pic_mem2 = pic_mem;

	for(i = 0; i < ret; i++) {
		fprintf(stdout,"0x%0.4X: ",i);		
		pic_decompileOpcode(*pic_mem);
		pic_mem++;
	}

	pic_mem_execute(pic_mem2, 0x3FF); // bellege alinan komutlari isle
	
	//cout << ret << endl;
	if( ret == -1 )
		cerr << "error:" << endl;
	return 0;
}

