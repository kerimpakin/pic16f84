//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tez_ide_026_prj.h"
#include "tez_header_026.h"
#include "espstd.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "PERFGRAP"
#pragma link "piereg"
#pragma resource "*.dfm"

//--------------------------- global  -------------------------------------

pic_stack stk;

// Ortak BANK'takiler

unsigned char *indf_reg	= pic_select_filereg_location(file_reg,SFR_INDF);
unsigned char *status_reg = pic_select_filereg_location(file_reg,SFR_STATUS);
unsigned char *pcl_reg = pic_select_filereg_location(file_reg,SFR_PCL);
unsigned char *fsr_reg = pic_select_filereg_location(file_reg,SFR_FSR);
unsigned char *pclath_reg = pic_select_filereg_location(file_reg,SFR_PCLATH);
unsigned char *intcon_reg = pic_select_filereg_location(file_reg,SFR_INTCON);

// BANK 0 dakiler

unsigned char *tmr0_reg = locate_ilk(file_reg,SFR_TMR0,0);
unsigned char *porta_reg = locate_ilk(file_reg,SFR_PORTA,0);
unsigned char *portb_reg = locate_ilk(file_reg,SFR_PORTB,0);
unsigned char *eedata_reg = locate_ilk(file_reg,SFR_EEDATA,0);
unsigned char *eeadr_reg = locate_ilk(file_reg,SFR_EEADR,0);

// BANK 1 dekiler

unsigned char *option_reg = locate_ilk(file_reg,SFR_OPTION,1);
unsigned char *trisa_reg = locate_ilk(file_reg,SFR_TRISA,1);
unsigned char *trisb_reg = locate_ilk(file_reg,SFR_TRISB,1);
unsigned char *eecon1_reg = locate_ilk(file_reg,SFR_EECON1,1);
unsigned char *eecon2_reg = locate_ilk(file_reg,SFR_EECON2,1);



//------------------------------global sonu ----------------------------------


#define refresh1        i = 5;/*donguden cik*/

#define refresh2        Label4->Caption = IntToHex(cycle,2);\
                        Label5->Caption = IntToHex(pc,2);\
                        Label6->Caption = IntToHex(reg_w,2);\
                        Label10->Caption = IntToHex(prescaler,2);\
                        Label12->Caption = IntToHex(wdt,2);


#define refresh3 ListView1->Items->Item[0]->SubItems->Clear();\
        ListView1->Items->Item[0]->SubItems->Add(cevir(file_reg[0x00]));\
        ListView1->Items->Item[1]->SubItems->Clear();\
        ListView1->Items->Item[1]->SubItems->Add(cevir(file_reg[0x01]));\
        ListView1->Items->Item[2]->SubItems->Clear();\
        ListView1->Items->Item[2]->SubItems->Add(cevir(file_reg[0x02]));\
        ListView1->Items->Item[3]->SubItems->Clear();\
        ListView1->Items->Item[3]->SubItems->Add(cevir(file_reg[0x03]));\
        ListView1->Items->Item[4]->SubItems->Clear();\
        ListView1->Items->Item[4]->SubItems->Add(cevir(file_reg[0x04]));\
        ListView1->Items->Item[5]->SubItems->Clear();\
        ListView1->Items->Item[5]->SubItems->Add(cevir(file_reg[0x05]));\
        ListView1->Items->Item[6]->SubItems->Clear();\
        ListView1->Items->Item[6]->SubItems->Add(cevir(file_reg[0x06]));\
        /* sfr 7 olmadigindan dikkat*/\
        ListView1->Items->Item[7]->SubItems->Clear();\
        ListView1->Items->Item[7]->SubItems->Add(cevir(file_reg[0x08]));\
        ListView1->Items->Item[8]->SubItems->Clear();\
        ListView1->Items->Item[8]->SubItems->Add(cevir(file_reg[0x09]));\
        ListView1->Items->Item[9]->SubItems->Clear();\
        ListView1->Items->Item[9]->SubItems->Add(cevir(file_reg[0x0A]));\
        ListView1->Items->Item[10]->SubItems->Clear();\
        ListView1->Items->Item[10]->SubItems->Add(cevir(file_reg[0x0B]));\
        \
        ListView2->Items->Item[0]->SubItems->Clear();\
        ListView2->Items->Item[0]->SubItems->Add(cevir(file_reg[0x50]));\
        ListView2->Items->Item[1]->SubItems->Clear();\
        ListView2->Items->Item[1]->SubItems->Add(cevir(file_reg[0x51]));\
        ListView2->Items->Item[2]->SubItems->Clear();\
        ListView2->Items->Item[2]->SubItems->Add(cevir(file_reg[0x52]));\
        ListView2->Items->Item[3]->SubItems->Clear();\
        ListView2->Items->Item[3]->SubItems->Add(cevir(file_reg[0x53]));\
        ListView2->Items->Item[4]->SubItems->Clear();\
        ListView2->Items->Item[4]->SubItems->Add(cevir(file_reg[0x54]));\
        \
        for(int k=0; k<8; k++){\
        ListView5->Items->Item[k]->SubItems->Clear();\
        ListView5->Items->Item[k]->SubItems->Add(IntToHex(stk.goster(k),2));\
        }\
        \
        for (int i=0; i<64; i++){\
        ListView4->Items->Item[i]->SubItems->Clear();\
        ListView4->Items->Item[i]->SubItems->Add(IntToHex(eeprom_field[i],2));\
        }\
        \
        \
        for(int j=0; j<68; j++){\
        ListView6->Items->Item[j]->SubItems->Clear();\
        ListView6->Items->Item[j]->SubItems->Add(IntToHex(file_reg[j+12],2));\
        }



#define SLEEP_MODE \
	if(!(*option_reg & 0x20)/* T0CS */) {/* Dahili osc artimi */\
		prescaler++;\
                prescaler %= 257;\
	} else { /* harici osilator artimi icin */\
	\
		;\
		\
		\
	}\
	\
	if( *option_reg & 0x08 ) { /* wdt secilmisse */\
		if( ((1 << (*option_reg & 0x07)) == prescaler)  ) {/*2^n*/\
                        if( wdt == 0xFF )\
				wdt_overflow = true;\
                        wdt += 1;\
                        prescaler = 0;\
		}\
	}else { /*tmr0 secilmisse */\
                if ( !(sleep_mode) )\
		if( ((2 << (*option_reg & 0x07)) == prescaler)  ) {/* 2^(n+1)*/\
                        if( *tmr0_reg == 0xFF ){\
				*intcon_reg |= 0x04;/* T0IF */\
				tmr0_overflow = true;\
				}\
                        *tmr0_reg += 1;\
                        prescaler = 0;\
		}\
	}\
	\
if( tmr0_overflow == true ){\
        tmr0_overflow = false;\
        *tmr0_reg = 0;\
	if( (0x80 & *intcon_reg) && (*intcon_reg & 0x20) ) { \
                sleep_mode = false;/*interruptla wake up*/\
                StatusBar1->Panels->Items[2]->Text = " ";\
                *intcon_reg &= 0x7F;/* GIE Disable ediliyor. */ \
                stk.push(pc);\
                *status_reg &= 0xE7;/* uuu00uuu */\
                *status_reg |= 0x10;/* TO set */\
                *pclath_reg = 0;\
                *pcl_reg = 4;\
	}\
}else if (wdt_overflow == true ){\
        wdt_overflow = false;\
        wdt = 0;\
        *status_reg &= 0x07;/* z,dc,c ayni*/\
        if ( !sleep_mode )\
                *status_reg |= 0x08;/*PD set*/\
        sleep_mode = false;\
        StatusBar1->Panels->Items[2]->Text = " ";\
        Button3->Click();/* resetleme */\
}



#define INCREMENT_CYCLE \
	cycle++;\
	SLEEP_MODE;


////////////////////////////////// makro sonu /////////////////////////////////////


//-----------------------------------------------------------------------------
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Ac1Click(TObject *Sender)
{
OpenDialog1->Filter = "Hex Dosyalar (*.HEX)|*.hex|Butun Dosyalar (*.*)|*.*";
        if (OpenDialog1->Execute()){
                ListBox1->Items->LoadFromFile(OpenDialog1->FileName);
                ListView3->Items->Clear();

                StatusBar1->Panels->Items[0]->Text = String(" Acilan Dosya : ")
                + String(OpenDialog1->FileName.c_str());

                Button1->Enabled = true;
                Button2->Enabled = false;
                Button3->Enabled = false;
                Button4->Enabled = false;
                Button5->Enabled = false;
                Button7->Enabled = false;

                Button6->Click();

        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Cikis1Click(TObject *Sender)
{
  exit(0);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Font1Click(TObject *Sender)
{
        FontDialog1->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FontDialog1Apply(TObject *Sender, HWND Wnd)
{
        ListBox1->Font = FontDialog1->Font;
        ListView3->Font = FontDialog1->Font;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Renk1Click(TObject *Sender)
{
        if(ColorDialog1->Execute())
                Form1->Color = ColorDialog1->Color;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
        GroupBox1->Enabled = true;

        TrackBar1->Enabled = true;

        Button1->Enabled = false;
        Button2->Enabled = true;
        Button3->Enabled = true;
        Button4->Enabled = true;
        Button5->Enabled = true;
        Button7->Enabled = true;
        Button6->Enabled = true;
        Button8->Enabled = true;
        Button13->Enabled = true;



        bool config = false;
        char g[BUFSIZ];
        strcpy(g,OpenDialog1->FileName.c_str()); 

        String liste1, liste2, liste3, liste4;

        mem_p1 = (unsigned short*) malloc (sizeof(unsigned short)*1024);

        unsigned short *mem1;
        mem1 = (unsigned short*)malloc(sizeof(unsigned short)*1024);
        mem2 = mem1;
        for (int i=0; i<1024; i++){  // default degerleri nop
                mem1[i] = bosluk;
                mem_p1[i] = bosluk;
                }


        int ret = 0;
        int i = 0;
        FILE *fp;

        fp = fopen(g,"r");

	int chr;
        char str[50];
	int m = 0;
	int pc = 0;// program counter
	int str_len;

        int p = 0;

	unsigned char checksum = 0;
	inhx8m_format fmt;
	char str_num[10];
	char str2[5];

        bool bank = false;


//*************************************

	while( ( chr = fgetc(fp) ) != EOF )
	{
		checksum = 0;
		if( chr == ':' )
		{
                // length - Mevcut satirda kac byte data oldugunu gosterir.
		read_file(fp,str_num,2); // ilk 1 byte'i oku
                fmt.length = strtol(str_num, NULL, 16);
		checksum += fmt.length;// checksum'a ekle

                //line - Mevcut satirdaki datalarin baslangic adresini gosterir.
                // burada 2 byte data var ancak checksumla 1'er byte toplanmasi
                // gerektiginden once toplama sonra atama yap.
		read_file(fp, str_num, 2);
                checksum += strtol(str_num, NULL, 16);

                if ( str_num[0] & 4 )
                        config = true;

                read_file(fp, str2, 2);
                checksum += strtol(str2, NULL, 16);

                strncat(str_num, str2, 2); // birlestirip hex olarak sakla
                fmt.line = strtol(str_num, NULL, 16);

                if ( (fmt.line != 0 ) && (fmt.line <= 1024) )
                        pc = fmt.line/2 ;  // org ayari icin


                // type - Mevcut satirdaki verilerin tipi (nasil ele alinacagi);
                read_file(fp, str_num, 2);
                fmt.type = strtol(str_num, NULL, 16);
                checksum += fmt.type;


                // data - Hex data, low byte-high byte sirasinda
                // burada onemli olan low-high byte'lari
                // takas edip istedigimiz hale getirmek

                memset(str2, 0, sizeof(str2));  // 0'la

                if ( config ){
                fmt.type = 4;  // data degil, mem'e atma, checksumla toplama
                config = false;
                }

                for(m = 0; m < fmt.length && pc < 1024; m++)
                {
                read_file(fp, str_num,2);
                strncat(str2,str_num,2);

                checksum += strtol(str_num, NULL, 16);

                checksum%=256;// toplanan komutlarin toplami 255 i asamaz.
                str_len = strlen(str2);

                if( str_len >= 4)    // her 2 byte da swap et ve mem'e aktar
                {
                  swap2byte(str2);
                  if(fmt.type == DATA)
                     mem1[pc++] = (unsigned short)strtol(str2, NULL, 16);

                  memset(str2,0,sizeof(str2));
                }
                }

                // checksum
		read_file(fp, str_num, 2);
                fmt.checksum = strtol(str_num, NULL, 16);

                // checksum formulunu uygula ve okunan dosyati kontrol et
        if( (((255 - checksum + 1)%256) != fmt.checksum) && ( !config ))
                ShowMessage("error checksum");

        }
	}

	fclose(fp);



//*******************************
for( p = 0; p < pc; p++)
{
	for(int h = 0; h < 6; h++)
	{
                switch((mem1[p] & mask[h]))
		{
		case ADDWF:

                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "ADDWF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case ANDWF:

                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "ANDWF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;


                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case CLRF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "CLRF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);

                        extended1;


                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case CLRW:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "CLRW";

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);

                        h = 5;
			break;

		case COMF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "COMF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case DECF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "DECF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case DECFSZ:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "DECFSZ";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case INCF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "INCF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case INCFSZ:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "INCFSZ";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case IORWF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "IORWF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
                        break;

		case MOVF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "MOVF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case MOVWF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "MOVWF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case NOP:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "NOP";

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);

                        h = 5;
			break;

		case RLF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "RLF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case RRF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "RRF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
                        break;

		case SUBWF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "SUBWF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case SWAPF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "SWAPF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case XORWF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "XORWF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = ((mem1[p] & 0x0080)?'f':'w');

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case BCF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "BCF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = (mem1[p] & 0x0380)>> 7;

                        if ( ((mem1[p] & 0x007f) == STATUS)
                                &&(((mem1[p] & 0x0380)>> 7 ) == 0x05) )
                                        bank = false;
                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case BSF:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "BSF";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = (mem1[p] & 0x0380)>> 7;

                        if ( ((mem1[p] & 0x007f) == STATUS)
                                &&(((mem1[p] & 0x0380)>> 7 ) == 0x05) )
                                        bank = true;

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case BTFSC:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "BTFSC";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = (mem1[p] & 0x0380)>> 7;

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;


		case BTFSS:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "BTFSS";
                        liste2 = IntToHex(mem1[p] & 0x007f,2);
                        liste3 = (mem1[p] & 0x0380)>> 7;

                        extended1;

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);
                        ListView3->Items->Item[p]->SubItems->Add(liste3);

                        h = 5;
			break;

		case ADDLW:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "ADDLW";
                        liste2 = "0x" + IntToHex(mem1[p] & 0x00ff,2);

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case ANDLW:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "ANDLW";
                        liste2 = "0x" + IntToHex(mem1[p] & 0x00ff,2);

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case CALL:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "CALL";
                        liste2 = "0x" + IntToHex(mem1[p] & 0x07ff,3);

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case RETURN:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "RETURN";

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);

                        h = 5;
			break;

		case CLRWDT:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "CLRWDT";

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);

                        h = 5;
			break;

		case GOTO:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "GOTO";
                        liste2 = "0x" + IntToHex(mem1[p] & 0x07ff,3);

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case IORLW:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "IORLW";

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);

                        h = 5;
			break;

		case MOVLW:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "MOVLW";
                        liste2 = "0x" + IntToHex(mem1[p] & 0x00ff,2);

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case RETFIE:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "RETFIE";

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);

                        h = 5;
			break;

		case RETLW:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "RETLW";
                        liste2 = "0x" + IntToHex(mem1[p] & 0x00ff,2);

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case SLEEP:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "SLEEP";

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);

                        h = 5;
			break;

		case SUBLW:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "SUBLW";
                        liste2 = "0x" + IntToHex(mem1[p] & 0x00ff,2);

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
			break;

		case XORLW:
                        mem_p1[p] = mem1[p] & mask[h];

                        liste1 = "XORLW";
                        liste2 = "0x" + IntToHex(mem1[p] & 0x00ff,2);

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add(liste1);
                        ListView3->Items->Item[p]->SubItems->Add(liste2);

                        h = 5;
    			break;

                case bosluk:    // org dan dolayi bos kalabilir

                        mem_p1[p] = mem1[p] & mask[h];

                        ListView3->Items->Add();
                        ListView3->Items->Item[p]->Caption = IntToHex(p,3);
                        ListView3->Items->Item[p]->SubItems->Add("null");

                        h = 5;

		default:
			break;
		}
	}
   }
  ListView3->Selected = ListView3->Items->Item [0];
  ListView3->Selected->MakeVisible( true);
}



void __fastcall TForm1::Button5Click(TObject *Sender)
{
        
       if ( ilk_tur ){
                mem = mem2;
	        sleep_mode = false;
                StatusBar1->Panels->Items[2]->Text = " ";
        }


        if ( RBO_INT ){
                if ( (*portb_reg & 0x01) & *trisb_reg )
                        *intcon_reg |= 0x02;    // harici int var
                RBO_INT = 0;
        }


       portb_const2 = *portb_reg & 0xF0; // port B high degisimi int icin
       if (portb_const1 != portb_const2)
                *intcon_reg |= 0x01;


    tmp = 0; // bit islemleri icin gecici

		/******************************************/
		/*  Buraya bu kontrollerin koyulma sebebi */
		/*  adres decode etme isleminin           */
		/*  ilk decode maskesinde butun           */
		/*  kotrollerin saglanmasi.               */
		/******************************************/


    /***********************************************************/
    /*                    Indirect Address icin                */
    /***********************************************************/
    if ( *fsr_reg <= 0x4F ){// gpr bellek sinirinda ise
        // eger degerler degismisse
        if ( (fsr_tmp_adr == *fsr_reg) && (fsr_tmp_val == file_reg[*fsr_reg]) )
                file_reg[*fsr_reg] = *indf_reg;// 3
        else {
                *indf_reg = file_reg[*fsr_reg];// 1
                fsr_tmp_adr = *fsr_reg;        // 2
                fsr_tmp_val = file_reg[*fsr_reg];
        }

      /********* numarlari açiklayan yorumlar ********************/
      // 1) indf'e adresteki degeri at
      // 2) indf'le adresteki degeri adres ya da
      //    deger degisince birbirine esitlemk icin gecici olarak sakla
      // 3) adresteki degeri indf yoluyla degis

    }else
        *fsr_reg = 0x50;

	/***********************************************************/
	/*                    EEPROM                               */
	/***********************************************************/

        if( *eecon1_reg & 0x01 ) {      // okuma ?
	        *eedata_reg = ((*eeadr_reg > 0x3F)?0:eeprom_field[*eeadr_reg]);
		*eecon1_reg &= 0xFE;    //read bit '0'la
		++cycle;
        }

	if( *eecon1_reg & 0x04 ) {      // yazma ?

         if( eerom_key ){        //0x55 set edildiyse bir onceki instruction'da

	   if( *eecon2_reg == 0xAA){       //0x55 0xAA kosulu saglandiysa yaz
	     if( *eeadr_reg <= 0x3F ){       //adres siniri icinde ise yaz
	        eeprom_field[*eeadr_reg] = *eedata_reg;
		*eecon1_reg |= 0x10;    // yazma tamam flagi
             }
	     /* kullanici isterse baslik dosyasindan cycle'i ayarlayabilir */
	     /* cunku uC icinde bu cycle Voltage ve Isýya */
	     /* gore degismekte */
	     cycle += eerom_cycle;
             eerom_key = 0;
           }

         }else if( *eecon2_reg == 0x55 ){       //eerom_key set ediliyorki
	 	                                // diger cycle'da pic'e
	 	                                // yazarken kontrol icin
	        eerom_key = 1;
        }
    }

    /**************************************************************/
    /*                         INTERRUPT KONTROLU                 */
    /**************************************************************/

    if( 0x80 & *intcon_reg ) {// INT INTCON Register'da set edildiyse

				// TMR0 makroda

    /********************/
    /* EE2ROM INTERRUPT */
    /********************/

    if( (*intcon_reg & 0x40) && (*eecon1_reg & 0x10 ) ){
        sleep_mode = false;     // interruptla wake up
        StatusBar1->Panels->Items[2]->Text = " ";
        *intcon_reg &= 0x7F;    //GIE Disable ediliyor.
        stk.push(pc);
        *pclath_reg = 0;
        *pcl_reg = 4;
        pc = 4;

    }

    /*******************/
    /* portB Interrupt */
    /*******************/

    //PORTB interrupt'i set edildiyse
    if ( (*intcon_reg & 0x08) && (*intcon_reg & 0x01) ) {
        sleep_mode = false;     // interruptla wake up
        StatusBar1->Panels->Items[2]->Text = " ";
        *intcon_reg &= 0x7F;    //GIE Disable ediliyor.
        portb_const1 = portb_const2;
        stk.push(pc);
        *pclath_reg = 0;
        *pcl_reg = 4;
        pc = 4;


    }else if((*intcon_reg & 0x10)&&(*intcon_reg & 0x02)) {

    /*************************/
    /* RB0/INT ucu Interrupt */
    /*************************/
    sleep_mode = false;     // interruptla wake up
    StatusBar1->Panels->Items[2]->Text = " ";
    *intcon_reg &= 0x7F;    // GIE Disable ediliyor...
    stk.push(pc);
    *pclath_reg = 0;
    *pcl_reg = 4;
    pc = 4;

    }

}



   if( sleep_mode == false )
   //switch( mem[pc] & mask[i%6] ) // maske no 0-5 arasi olmali.
   switch ( mem_p1[pc] )
   {
     case ADDWF:
        tmp_pc = pc;  // tmp'ye pc degeri korumak icin
        tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);
        // hedef reg adresini al
        *pcl_reg = (unsigned char)++pc;

        *status_reg &= 0xfe;   // carry bitini sifirla
        carry_check = *tmp_file_reg + reg_w;// onceden topla ve carry tesbit et
        *status_reg |= (carry_check >> 8);// carry set ediliyor

        if ( !carry_check )
                *status_reg |= 0x04;   // zero bitini set et
        else
                *status_reg &= 0xFB;

        if ( carry_check >= 0x0F )
        *status_reg |= 0x02;            // digit carry bitini set et

        if( (mem[tmp_pc] & 0x80) ){
         if ( (mem[tmp_pc] & 0x7F) != 0x03 )// status'de yazilamayan bitler var
             *tmp_file_reg += reg_w;

         // option degisirse prescaler = 0
         if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )
                prescaler = 0;
         }else
                reg_w += *tmp_file_reg;



        INCREMENT_CYCLE;
        refresh1;

        break;


     case ANDWF:

        tmp_pc = pc;//tmp'ye pc degeri korumak icin
	*pcl_reg =(unsigned char)++pc;
        // hedef reg adresini al
        tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

        if ( *tmp_file_reg & reg_w )
                *status_reg &= 0xFB;
        else
                *status_reg |= 0x04; // zero bitini set et

        if( (mem[tmp_pc] & 0x80) ){
           // option degisirse prescaler = 0
          if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )
                prescaler = 0;

          if ( (mem[tmp_pc] & 0x7F) == 0x03 ) // status'de yazilamayan bitler var
                *tmp_file_reg &= ( reg_w & 0x20 );  // xx?x xxxx
          else
                *tmp_file_reg &= reg_w;
        }else
		reg_w &= *tmp_file_reg;


        INCREMENT_CYCLE;
        refresh1;

        break;

     case CLRF:

     tmp_pc = pc;
     *pcl_reg =(unsigned char)++pc;

     if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
        prescaler = 0;

     if ( (mem[tmp_pc] & 0x7F) == 0x03 )             // status'de yazilamayan bitler
        *pic_select_filereg_location(file_reg,mem[pc-1]) &= 0xDF ; // xx0x xxxx
     else
        *pic_select_filereg_location(file_reg,mem[pc-1]) = 0;

     *status_reg |= 0x04; // zero bitini set et

     INCREMENT_CYCLE;
     refresh1;


     break;

   case CLRW:

   *pcl_reg =(unsigned char)++pc;

   reg_w = 0;
   *status_reg |= 0x04; // zero bitini set et

   INCREMENT_CYCLE;

   refresh1;

   break;

   case COMF:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;
   tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

   if( (mem[tmp_pc] & 0x80) ) {
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) == 0x03 ){ // status'de yazilamayan bitler var
                if ( *tmp_file_reg & 0x20 )
                        *tmp_file_reg &= 0xDF;      // '0'la
                else
                        *tmp_file_reg |= 0x20;
        }else
                *tmp_file_reg = ~ *tmp_file_reg;

        if ( !*tmp_file_reg )
                *status_reg |= 0x04; // zero bitini set et
        else
                *status_reg &= 0xFB;
   }else{
        reg_w = ~ *tmp_file_reg;

        if ( !reg_w )
                *status_reg |= 0x04; // zero bitini set et
        else
                *status_reg &= 0xFB;
   }

   INCREMENT_CYCLE;
   refresh1;

   break;


   case DECF:

   tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);
   tmp_pc = pc;// tmp degeri pc degerini korumak icin
   *pcl_reg =(unsigned char)++pc;

   if( (mem[tmp_pc] & 0x80) ){
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) != 0x03 ){// status'de yazilamayan bitler var
                *tmp_file_reg -= 1;
                if ( !*tmp_file_reg )
                        *status_reg |= 0x04; // zero bitini set et
                else
                        *status_reg &= 0xFB;
        }
   }else{
        *tmp_file_reg -= 1;
        reg_w = *tmp_file_reg;
        if ( !reg_w )
                *status_reg |= 0x04; // zero bitini set et
        else
                *status_reg &= 0xFB;
   }

   INCREMENT_CYCLE;
   refresh1;

   break;


   case DECFSZ:

   if( (mem[pc] & 0x80) ){
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[pc] & 0x7F) == 0x03 )   // status'de yazilamayan bitler var
                tmp = true;     // status '0' olmayacagindan hep '1'
        else
                tmp = --*pic_select_filereg_location(file_reg,mem[pc]);
   }else
        tmp = reg_w = *pic_select_filereg_location(file_reg,mem[pc]) - 1;

   if( tmp ) {
        *pcl_reg =(unsigned char)++pc;
        INCREMENT_CYCLE;
   }else {
        pc += 2;
        *pcl_reg =(unsigned char)pc;
        INCREMENT_CYCLE;
        INCREMENT_CYCLE;
   }

   refresh1;

   break;


   case INCF:

   tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);
   tmp_pc = pc;// tmp degeri pc degerini korumak icin
   *pcl_reg =(unsigned char)++pc;

   if( (mem[tmp_pc] & 0x80) ){
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) != 0x03 ){ // status'de yazilamayan bitler var
                *tmp_file_reg += 1;
                if ( !tmp_file_reg )
                        *status_reg |= 0x04;    // zero bitini set et
                else
                        *status_reg &= 0xFB;
                }
        }else{
                reg_w = *tmp_file_reg + 1;
                if ( !reg_w )
                        *status_reg |= 0x04; // zero bitini set et
                else
                        *status_reg &= 0xFB;
        }

   INCREMENT_CYCLE;
   refresh1;

   break;


   case INCFSZ:

   if( (mem[pc] & 0x80) ){
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[pc] & 0x7F) == 0x03 )  // status'de yazilamayan bitler var
                tmp = true;              // status '0' olmayacagindan hep '1'
        else
                tmp = ++*pic_select_filereg_location(file_reg,mem[pc]);
   }else
        tmp = reg_w = *pic_select_filereg_location(file_reg,mem[pc]) + 1;

   if( tmp ) {
                *pcl_reg =(unsigned char)++pc;
                INCREMENT_CYCLE;
   } else {
                pc += 2;
                *pcl_reg =(unsigned char)pc;
                INCREMENT_CYCLE;
                INCREMENT_CYCLE;
   }

   refresh1;
   break;


   case IORWF:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;
   tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

   if ( *tmp_file_reg | reg_w )
        *status_reg &= 0xFB;
   else
        *status_reg |= 0x04; // zero bitini set et


   if( (mem[tmp_pc] & 0x80) ) {
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) == 0x03 )// status'de yazilamayan bitler var
                *tmp_file_reg |= (reg_w & 0x20);
        else
                *tmp_file_reg |= reg_w;
   }else
        reg_w |= *tmp_file_reg;



   INCREMENT_CYCLE;
   refresh1;
   break;


   case MOVF:
   tmp_pc = pc;// tmp degeri pc degerini korumak icin
   *pcl_reg =(unsigned char)++pc;

   if( (mem[tmp_pc] & 0x80) ){
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;
        /* Ayni adres icerikleri kopyalandigi icin
        * Hicbirsey yapmaya gerek yok
        */
        ;
   }else {
        reg_w = *pic_select_filereg_location(file_reg,mem[tmp_pc]);
        if ( !reg_w )
                *status_reg |= 0x04; // zero bitini set et
        else
                *status_reg &= 0xFB;
        }

   INCREMENT_CYCLE;
   refresh1;
   break;


   case MOVWF:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
   prescaler = 0;

   if ( (mem[tmp_pc] & 0x7F) == 0x03 )       // status'de yazilamayan bitler var
        *pic_select_filereg_location(file_reg,mem[pc-1]) |= ( reg_w & 0x20 );// xx?x xxxx
   else
        *pic_select_filereg_location(file_reg,mem[pc-1]) = reg_w;


   INCREMENT_CYCLE;
   refresh1;
   break;


   case NOP:

        *pcl_reg =(unsigned char)++pc;
        INCREMENT_CYCLE;
        refresh1;
        break;


   case RLF:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

   tmp = *status_reg & 0x01; // carry bitini al

   if( (mem[tmp_pc] & 0x80) ) {
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) != 0x03 ){     // status'se yapma

                carry_check = ((*tmp_file_reg & 0x80)>>7) ; // 00000001 yada 0

                *tmp_file_reg <<= 1;
                *tmp_file_reg |= tmp;// status carry tasi
                *status_reg &= 0xfe; // carry kullanilinca bosalt

                if ( carry_check )
                        *status_reg |= 0x01;//status reg carry set ediliyor
                }
        }else {
                carry_check = ((*tmp_file_reg & 0x80)>>7) ; // 00000001 yada 0

                reg_w = (*tmp_file_reg << 1) | tmp;
                *status_reg &= 0xfe; // carry kullanilinca bosalt

                if ( carry_check )
                        *status_reg |= 0x01;//status reg carry set ediliyor
        }

   INCREMENT_CYCLE;
   refresh1;
   break;


   case RRF:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;
   tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

   tmp = (*status_reg & 0x01) << 7; // carry bitini al

   if( (mem[tmp_pc] & 0x80) ) {
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) != 0x03 ){     // status'se yapma
                carry_check = ((*tmp_file_reg & 0x01) << 7) ; // 1000000 yada 0
                *tmp_file_reg >>= 1;
                *tmp_file_reg |= tmp;// status carry tasi
                *status_reg &= 0xfe; // carry kullanilinca bosalt

                if ( carry_check ) // ??
                *status_reg |= 0x01;//status reg carry set ediliyor
                }
        }else {

                carry_check = ((*tmp_file_reg & 0x01) << 7) ; // 10000000 yada 0

                reg_w = (*tmp_file_reg >> 1) | tmp;
                *status_reg &= 0xfe; // carry kullanilinca bosalt

                if ( carry_check )
                *status_reg |= 0x01;//status reg carry set ediliyor
        }

   INCREMENT_CYCLE;
   refresh1;
   break;


   case SUBWF:

   tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);
   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   if (reg_w > (mem[tmp_pc] & 0xff)){
        carry_check = ~reg_w;
        carry_check += (mem[tmp_pc] & 0xff) + 1;
        *status_reg &= 0xfe; // sonuc "-"
   }else{
        carry_check = (mem[tmp_pc] & 0xff) - reg_w;
        *status_reg |= 0x01; // sonuc "+" veya "0"
   }

   if (carry_check >= 0x0F)
        *status_reg |= 0x02; // digit carry bitini set et

   if( (mem[tmp_pc] & 0x80) ){
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) != 0x03 ){     // status'se yapma
                *tmp_file_reg = (unsigned char)carry_check;
                if ( !*tmp_file_reg )
                        *status_reg |= 0x04;
                else
                        *status_reg &= 0xFB;
                }
        }else{
                reg_w = (unsigned char)carry_check;
                if ( !reg_w )
                        *status_reg |= 0x04;
                else
                        *status_reg &= 0xFB;
        }

   INCREMENT_CYCLE;
   refresh1;
   break;


   case SWAPF: //f registerinin icindeki ilk nibble ile 2.yi yer degistir, w yada f'e yaz

   tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);
   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   if( (mem[tmp_pc] & 0x80) ){
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) != 0x03 ){     // status'se yapma
                tmp = *tmp_file_reg & 0xf0; //ilk nibble
                tmp >>= 4;
                *tmp_file_reg = ((*tmp_file_reg & 0x0f) << 4) | tmp;
        }
   }else {
        tmp = *tmp_file_reg & 0xf0;
        tmp >>= 4;
        reg_w = ((*tmp_file_reg & 0x0f) << 4) | tmp;
   }

   INCREMENT_CYCLE;
   refresh1;
   break;


   case XORWF:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   if( (mem[tmp_pc] & 0x80) ){
        if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
                prescaler = 0;

        if ( (mem[tmp_pc] & 0x7F) != 0x03 ){     // status'se yapma
                tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);
                *tmp_file_reg ^= reg_w;
                if( !*tmp_file_reg )
                        *status_reg |= 0x04;
                else
                        *status_reg &= 0xFB;
                }
        }else {
                reg_w ^= *pic_select_filereg_location(file_reg,mem[tmp_pc]);
                if ( !reg_w )
                        *status_reg |= 0x04;
                else
                        *status_reg &= 0xFB;
        }

   INCREMENT_CYCLE;
   refresh1;
   break;


   case BCF:// f registerinin icindeki sayinin b.ninci bitini sifirlar

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;
   tmp_file_reg = pic_select_filereg_location(file_reg,mem[tmp_pc]);

   tmp = ( mem[tmp_pc] & 0x380 ) >> 7;
   tmp = ( tmp ? (1 << (tmp)) :1 );
   tmp = ~tmp;

   if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
        prescaler = 0;

   if ( (mem[tmp_pc] & 0x7F) == 0x03 ){     // status ?
        if ( (~tmp) & 0x20 )
                *tmp_file_reg &= tmp;
   }else
        *tmp_file_reg &= tmp;

   INCREMENT_CYCLE;
   refresh1;
   break;


   case BSF:

   tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   tmp = ( mem[tmp_pc] & 0x380 ) >> 7;
   tmp = (tmp?(1 << (tmp)):1);

   if ( (mem[tmp_pc] & 0x7F) != SELECT_SFR_OPTION )// option degisirse prescaler = 0
        prescaler = 0;

   if ( (mem[tmp_pc] & 0x7F) == 0x03 ){     // status ?
        if ( tmp & 0x20 )
                *tmp_file_reg |= tmp;
   }else
        *tmp_file_reg |= tmp;

   INCREMENT_CYCLE;
   refresh1;
   break;

   
   case BTFSC:

   tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);

   tmp = ( mem[pc] & 0x380 ) >> 7;         // kacinci bit
   tmp = ( tmp?(1 << (tmp)):1 );           // 0'sa 1 al veya tmp kadar shift
   // tmp &= *pic_select_filereg_location(file_reg,( mem[pc] & 0x7F ));
   tmp &= *tmp_file_reg;

   if( tmp ) {
        *pcl_reg =(unsigned char)++pc;
        INCREMENT_CYCLE;
   } else {
        pc += 2;
        *pcl_reg =(unsigned char)pc;
        INCREMENT_CYCLE;
        INCREMENT_CYCLE;
   }

   refresh1;
   break;


   case BTFSS:

   tmp_file_reg = pic_select_filereg_location(file_reg,mem[pc]);

   tmp = ( mem[pc] & 0x380 ) >> 7; // kacinci bit
   tmp = ( tmp?(1 << (tmp)):1 ); // 0'sa 1 al veya tmp kadar shift
   //tmp &= *pic_select_filereg_location(file_reg,( mem[pc] & 0x7F ));
   tmp &= *tmp_file_reg;

   if( tmp ) {
        pc += 2;
        *pcl_reg =(unsigned char)pc;
        INCREMENT_CYCLE;
        INCREMENT_CYCLE;
   } else {
        *pcl_reg =(unsigned char)++pc;
        INCREMENT_CYCLE;
   }

   refresh1;
   break;


   case ADDLW:

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
   else
        *status_reg &= 0xFB;

   INCREMENT_CYCLE;
   refresh1;
   break;


   case ANDLW:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   reg_w &= (mem[tmp_pc] & 0xff);
   if (!reg_w)
        *status_reg |= 0x04;
   else
        *status_reg &= 0xFB;

   INCREMENT_CYCLE;
   refresh1;
   break;

   
   case CALL:

   stk.push(pc);

   pc = ( mem[pc] & 0x07ff );
   *pcl_reg = pc & 0xFF;
   *pclath_reg = (pc >> 8);

   INCREMENT_CYCLE;
   INCREMENT_CYCLE;

   refresh1;
   break;


   case RETURN:

   *pcl_reg = pc = stk.pop() + 1;
   INCREMENT_CYCLE;
   INCREMENT_CYCLE;

   refresh1;
   break;


   case CLRWDT:

   *status_reg |= 0x18; // status icindeki 3. ve 4. flag set ediliyor.

   prescaler = 0;
   wdt = 0;

   *pcl_reg =(unsigned char)++pc;
   INCREMENT_CYCLE;
   INCREMENT_CYCLE;

   refresh1;
   break;


   case GOTO:

   *pcl_reg = pc = ( mem[pc] & 0x07ff );
   INCREMENT_CYCLE;
   INCREMENT_CYCLE;

   refresh1;
   break;


   case IORLW:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   reg_w |= (mem[tmp_pc] & 0xff);
   if (!reg_w)
   *status_reg |= 0x04;
   else
   *status_reg &= 0xFB;

   INCREMENT_CYCLE;
   INCREMENT_CYCLE;

   refresh1;
   break;


   case MOVLW:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   reg_w = (mem[tmp_pc] & 0xff);

   INCREMENT_CYCLE;

   refresh1;
   break;


   case RETFIE:           // interrupt dan donus

   *intcon_reg |= 0x80;

   *pcl_reg = pc = stk.pop();
   INCREMENT_CYCLE;
   INCREMENT_CYCLE;

   refresh1;
   break;


   case RETLW:

   reg_w = mem[pc] & 0xff;

   *pcl_reg = pc = stk.pop() + 1;

   INCREMENT_CYCLE;
   INCREMENT_CYCLE;

   refresh1;
   break;


   case SLEEP:

   *status_reg |= 0x10;/* TO SET */
   *status_reg &= 0xF7;/* PD CLEAR */

   wdt = 0;
   prescaler = 0;

   sleep_mode = true;
   StatusBar1->Panels->Items[2]->Text = "SLEEP mod !";
   *pcl_reg =(unsigned char)++pc;

   INCREMENT_CYCLE;

   refresh1;
   break;


   case SUBLW:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   if (reg_w > (mem[tmp_pc] & 0xff)){
        reg_w = ~reg_w;
        reg_w += (mem[tmp_pc] & 0xff) + 1;
        *status_reg &= 0xfe; // sonuc "-"
   }else{
        reg_w = (mem[tmp_pc] & 0xff) - reg_w;
        *status_reg |= 0x01; // sonuc "+" veya "0"
   }

   if ( reg_w  >= 0x0F )
        *status_reg |= 0x02; // digit carry bitini set et

   if ( !reg_w )
        *status_reg |= 0x04; // zero biti set
   else
        *status_reg &= 0xFB;

   INCREMENT_CYCLE;

   refresh1;
   break;


   case XORLW:

   tmp_pc = pc;
   *pcl_reg =(unsigned char)++pc;

   reg_w ^= (mem[tmp_pc] & 0xff) ;
   if ( !reg_w )
        *status_reg |= 0x04;
   else
        *status_reg &= 0xFB;

   INCREMENT_CYCLE;

   refresh1;
   break;

   default:
        break;
   }else {
        SLEEP_MODE;
        i = 5;   // donguden cik
        refresh2;
        refresh3;

   }
   
   ilk_tur = false;



// portlarin giris - cikis bitlerinin ayar kismi
        tmp_porta = ~*trisa_reg;
        tmp_portb = ~*trisb_reg;
        *porta_reg &= (tmp_porta & 0x1F); // porta = 000X XXXX
        *portb_reg &= tmp_portb; // dis girisler her tur 0 laniyor 2 portda da


// ledleri yakan kisim
        tmp_porta = *porta_reg;
        tmp_portb = *portb_reg;

        if ( tmp_porta & 0x01 ){    // led 1
                Image24->Visible = 1;
                Image32->Visible = 0;
        }else{
                Image24->Visible = 0;
                Image32->Visible = 1;
        }
        tmp_porta >>= 1;

        if ( tmp_porta & 0x01 ){    // led 2
                Image23->Visible = 1;
                Image31->Visible = 0;
        }else{
                Image23->Visible = 0;
                Image31->Visible = 1;
        }
        tmp_porta >>= 1;

        if ( tmp_porta & 0x01 ){    // led 3
                Image22->Visible = 1;
                Image30->Visible = 0;
        }else{
                Image22->Visible = 0;
                Image30->Visible = 1;
        }
        tmp_porta >>= 1;

        if ( tmp_porta & 0x01 ){    // led 4
                Image21->Visible = 1;
                Image29->Visible = 0;
        }else{
                Image21->Visible = 0;
                Image29->Visible = 1;
        }
        tmp_porta >>= 1;

        if ( tmp_porta & 0x01 ){    // led 5
                Image20->Visible = 1;
                Image28->Visible = 0;
        }else{
                Image20->Visible = 0;
                Image28->Visible = 1;
        }
        tmp_porta >>= 1;


        if ( tmp_portb & 0x01 ){    // led 1
                Image8->Visible = 0;
                Image16->Visible = 1;
        }else{
                Image8->Visible = 1;
                Image16->Visible = 0;
        }
        tmp_portb >>= 1;

        if ( tmp_portb & 0x01 ){    // led 2
                Image7->Visible = 0;
                Image15->Visible = 1;
        }else{
                Image7->Visible = 1;
                Image15->Visible = 0;
        }
        tmp_portb >>= 1;

        if ( tmp_portb & 0x01 ){    // led 3
                Image6->Visible = 0;
                Image14->Visible = 1;
        }else{
                Image6->Visible = 1;
                Image14->Visible = 0;
        }
        tmp_portb >>= 1;

        if ( tmp_portb & 0x01 ){    // led 4
                Image5->Visible = 0;
                Image13->Visible = 1;
        }else{
                Image5->Visible = 1;
                Image13->Visible = 0;
        }
        tmp_portb >>= 1;

        if ( tmp_portb & 0x01 ){    // led 5
                Image4->Visible = 0;
                Image12->Visible = 1;
        }else{
                Image4->Visible = 1;
                Image12->Visible = 0;
        }
        tmp_portb >>= 1;

        if ( tmp_portb & 0x01 ){    // led 6
                Image3->Visible = 0;
                Image11->Visible = 1;
        }else{
                Image3->Visible = 1;
                Image11->Visible = 0;
        }
        tmp_portb >>= 1;

        if ( tmp_portb & 0x01 ){    // led 7
                Image2->Visible = 0;
                Image10->Visible = 1;
        }else{
                Image2->Visible = 1;
                Image10->Visible = 0;
        }
        tmp_portb >>= 1;

        if ( tmp_portb & 0x01 ){    // led 8
                Image1->Visible = 0;
                Image9->Visible = 1;
        }else{
                Image1->Visible = 1;
                Image9->Visible = 0;
        }




// performance graph portb ye bagli

        if ( tmp_portb_reg != *portb_reg ){
                PerformanceGraph1->DataPoint(clRed,*portb_reg );
                PerformanceGraph1->Update();
                tmp_portb_reg = *portb_reg;
        }



// 7 segment display
        switch ( *portb_reg )
        {
                case ( 0x3F ):  // '0'
                        Image48->BringToFront();
                break;

                case ( 0x06 ):  // '1'
                        Image47->BringToFront();
                break;

                case ( 0x5B ):  // '2'
                        Image46->BringToFront();
                break;

                case ( 0x4F ):  // '3'
                        Image45->BringToFront();
                break;

                case ( 0x66 ):  // '4'
                        Image44->BringToFront();
                break;

                case ( 0x6D ):  // '5'
                        Image43->BringToFront();
                break;

                case ( 0x7D ):  // '6'
                        Image42->BringToFront();
                break;

                case ( 0x07 ):  // '7'
                        Image41->BringToFront();
                break;

                case ( 0x7F ):  // '8'
                        Image40->BringToFront();
                break;

                case ( 0x6F ):  // '9'
                        Image39->BringToFront();
                break;

                case ( 0x77 ):  // 'A'
                        Image38->BringToFront();
                break;

                case ( 0x7C ):  // 'b'
                        Image37->BringToFront();
                break;

                case ( 0x39 ):  // 'C'
                        Image36->BringToFront();
                break;

                case ( 0x5E ):  // 'd'
                        Image35->BringToFront();
                break;

                case ( 0x79 ):  // 'E'
                        Image34->BringToFront();
                break;

                case ( 0x71 ):  // 'F'
                        Image33->BringToFront();
                break;

                case ( 0x80 ):  // ' nokta '
                        Image50->BringToFront();
                break;

                default:
                        Image49->BringToFront();
                break;

        }

// butonlarin kontrolu

        if ( buton_A0 )
                *porta_reg |= (*trisa_reg & 0x01);

        if ( buton_A1 )
                *porta_reg |= (*trisa_reg & 0x02);

        if ( buton_A2 )
                *porta_reg |= (*trisa_reg & 0x04);

        if ( buton_A3 )
                *porta_reg |= (*trisa_reg & 0x08);

        if ( buton_A4 )
                *porta_reg |= (*trisa_reg & 0x10);




/**********************************************************/
/*               pc kontrolu ve ayristirma                */
/**********************************************************/

        if( pc > 1024 )//Bellek tasmasi halinde
                pc -= (1024 + 1);

        // not: pc komutlarin icinde mem'le artiriliyor o yuzden 0'la

        pc &= 0xFF00;
        pc |= *pcl_reg;

        *pclath_reg = 0x0000;
        *pclath_reg |= (pc >> 8);


// execute edilen satirin gosterimi


if ( ListView3->Items->Item [((*pclath_reg << 8) | *pcl_reg)] ){
ListView3->Selected = ListView3->Items->Item [((*pclath_reg << 8) | *pcl_reg)];
ListView3->Selected->MakeVisible( true);
}


// degerleri tazele
        refresh2;
        refresh3;

}

void __fastcall TForm1::Button3Click(TObject *Sender)
{
/* MCLR reset degerleri*/
         pc = 0;
         if ( Button1->Enabled == false ){
                ListView3->Selected = ListView3->Items->Item [0];
                ListView3->Selected->MakeVisible( true);
                }

         wdt = 0;
         prescaler = 0;
         RBO_INT = false;

	*indf_reg = 0;
	*pcl_reg = 0;

        if (sleep_mode){
                *status_reg &= 0x07;
                *status_reg |= 0x10;
                sleep_mode = false;
                StatusBar1->Panels->Items[2]->Text = " ";
        }else
	        *status_reg &= 0x1F;

	*pclath_reg = 0;
	*intcon_reg &= 0x01;
	*option_reg = 0xFF;
	*trisa_reg = 0x1F;
	*trisb_reg = 0xFF;
	*eecon1_reg = 0;
	*eecon2_reg = 0;

        refresh2;
        refresh3;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
/* power-on reset degerleri */
        Image49->BringToFront();

        buton_A0 = 0;
        buton_A1 = 0;
        buton_A2 = 0;
        buton_A3 = 0;
        buton_A4 = 0;


        Label13->Visible = 0;
        Label14->Visible = 1;
        Label15->Visible = 0;
        Label16->Visible = 1;
        Label17->Visible = 0;
        Label18->Visible = 1;
        Label19->Visible = 0;
        Label20->Visible = 1;

        cycle = 0;
        reg_w = 0;
        pc = 0;

        if ( Button1->Enabled == false ){
                ListView3->Selected = ListView3->Items->Item [0];
                ListView3->Selected->MakeVisible( true);
                }

        wdt = 0;
        prescaler = 0;
        RBO_INT = false;
        sleep_mode = false;
        StatusBar1->Panels->Items[2]->Text = " ";

	*indf_reg = 0;
	*tmr0_reg = 0;
	*pcl_reg = 0;
	*status_reg = 0x18;
	*fsr_reg = 0;
	*porta_reg = 0;
	*portb_reg = 0;
	*eedata_reg = 0;
	*eeadr_reg = 0;
	*pclath_reg = 0;
	*intcon_reg = 0;

	*option_reg = 0xFF;
	*trisa_reg = 0x1F;
	*trisb_reg = 0xFF;
	*eecon1_reg = 0;
	*eecon2_reg = 0;

        refresh2;
        refresh3;

}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button2Click(TObject *Sender)
{

        if( !emulate ){
                Button2->Caption = "Stop Emulate";
                StatusBar1->Panels->Items[1]->Text = "Emulating";
                emulate = true;
                Timer1->Enabled = true;
        }else{
                Button2->Caption = "Emulate";
                StatusBar1->Panels->Items[1]->Text = " ";
                emulate = false;
                Timer1->Enabled = false;
                }
}


//---------------------------------------------------------------------------


void __fastcall TForm1::Timer1Timer(TObject *Sender)
{

        //HANDLE procHandle = GetCurrentProcess();

        /*if( !SetPriorityClass(procHandle, HIGH_PRIORITY_CLASS) )
                ShowMessage("Process hatasi");*/

        /*if( !SetThreadPriority(procHandle,THREAD_PRIORITY_TIME_CRITICAL) )
             ShowMessage("Process Hatasi");*/

        /*if( !SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL) )
                             ShowMessage("Process Hatasi");*/
        Button5->Click();

}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button7Click(TObject *Sender)
{
        RBO_INT = !RBO_INT;
        if ( RBO_INT )
                *portb_reg |= (*trisb_reg & 0x01);
        else
                if ( *trisb_reg & 0x01 )
                        *portb_reg &= 0xFE;

        ListView1->Items->Item[6]->SubItems->Clear();
        ListView1->Items->Item[6]->SubItems->Add(cevir(file_reg[0x06]));
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button6Click(TObject *Sender)
{
         sleep_mode = false;
         StatusBar1->Panels->Items[2]->Text = " ";

         stk.sil();

         for (int i=12; i<80; i++)       // gpr degerleri 0
                file_reg[i] = 0;
         

         for (int i=0; i<64; i++)         // eeprom degerleri 0
                eeprom_field[i] = 0;

         ilk_tur = true;

         Button4->Click();
         for (int i=0; i<64; i++){
         ListView4->Items->Item[i]->SubItems->Clear();
         ListView4->Items->Item[i]->SubItems->Add(IntToHex(eeprom_field[i],2));
         }

         for(int j=0; j<68; j++){
         ListView6->Items->Item[j]->SubItems->Clear();
         ListView6->Items->Item[j]->SubItems->Add(IntToHex(file_reg[j+12],2));
         }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
        TrackBar1->Min = 1;
        TrackBar1->Max = 2000;
        TrackBar1->Position = 1000;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{
        Timer1->Interval = TrackBar1->Position ;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button12Click(TObject *Sender)
{
        buton_A0 = !buton_A0;
        Label13->Visible = !Label13->Visible;
        Label14->Visible = !Label14->Visible;
        Button5->SetFocus();

        if ( buton_A0 )
                *porta_reg |= (*trisa_reg & 0x01);
        else
                if ( *trisa_reg & 0x01 )
                        *porta_reg &= 0xFE;

        ListView1->Items->Item[5]->SubItems->Clear();
        ListView1->Items->Item[5]->SubItems->Add(cevir(file_reg[0x05]));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button11Click(TObject *Sender)
{
        buton_A1 = !buton_A1;
        Label15->Visible = !Label15->Visible;
        Label16->Visible = !Label16->Visible;
        Button5->SetFocus();
        if ( buton_A1 )
                *porta_reg |= (*trisa_reg & 0x02);
        else
                if ( *trisa_reg & 0x02 )
                        *porta_reg &= 0xFD;

        ListView1->Items->Item[5]->SubItems->Clear();
        ListView1->Items->Item[5]->SubItems->Add(cevir(file_reg[0x05]));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button10Click(TObject *Sender)
{
        buton_A2 = !buton_A2;
        Label17->Visible = !Label17->Visible;
        Label18->Visible = !Label18->Visible;
        Button5->SetFocus();

        if ( buton_A2 )
                *porta_reg |= (*trisa_reg & 0x04);
        else
                if ( *trisa_reg & 0x04 )
                        *porta_reg &= 0xFB;

        ListView1->Items->Item[5]->SubItems->Clear();
        ListView1->Items->Item[5]->SubItems->Add(cevir(file_reg[0x05]));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button9Click(TObject *Sender)
{
        buton_A3 = !buton_A3;
        Label19->Visible = !Label19->Visible;
        Label20->Visible = !Label20->Visible;
        Button5->SetFocus();

        if ( buton_A3 )
                *porta_reg |= (*trisa_reg & 0x08);
        else
                if ( *trisa_reg & 0x08 )
                        *porta_reg &= 0xF7;

        ListView1->Items->Item[5]->SubItems->Clear();
        ListView1->Items->Item[5]->SubItems->Add(cevir(file_reg[0x05]));
}
//---------------------------------------------------------------------------




void __fastcall TForm1::Button8Click(TObject *Sender)
{
*pcl_reg += 1;
if ( ListView3->Items->Item [((*pclath_reg << 8) | *pcl_reg)] ){
ListView3->Selected = ListView3->Items->Item [((*pclath_reg << 8) | *pcl_reg)];
ListView3->Selected->MakeVisible( true);
}

        if( pc > 1024 )//Bellek tasmasi halinde
                pc -= (1024 + 1);

        // not: pc komutlarin icinde mem'le artiriliyor o yuzden 0'la

        pc &= 0xFF00;
        pc |= *pcl_reg;

        *pclath_reg = 0x0000;
        *pclath_reg |= (pc >> 8);

        refresh2;
        refresh3;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button13Click(TObject *Sender)
{
*pcl_reg -= 1;
if ( ListView3->Items->Item [((*pclath_reg << 8) | *pcl_reg) ] ){
ListView3->Selected = ListView3->Items->Item [((*pclath_reg << 8) | *pcl_reg) ];
ListView3->Selected->MakeVisible( true);
}
        if( pc > 1024 )//Bellek tasmasi halinde
                pc -= (1024 + 1);

        // not: pc komutlarin icinde mem'le artiriliyor o yuzden 0'la

        pc &= 0xFF00;
        pc |= *pcl_reg;

        *pclath_reg = 0x0000;
        *pclath_reg |= (pc >> 8);

        refresh2;
        refresh3;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button14Click(TObject *Sender)
{
        buton_A4 = !buton_A4;
        Label22->Visible = !Label22->Visible;
        Label23->Visible = !Label23->Visible;
        Button5->SetFocus();

        if ( buton_A4 )
                *porta_reg |= (*trisa_reg & 0x10);
        else
                if ( *trisa_reg & 0x10 )
                        *porta_reg &= 0x0F;

        ListView1->Items->Item[5]->SubItems->Clear();
        ListView1->Items->Item[5]->SubItems->Add(cevir(file_reg[0x05]));
}
//---------------------------------------------------------------------------





void __fastcall TForm1::About1Click(TObject *Sender)
{

        ShowMessage("Programmer Kerim Basol \n www.esp-solutions.com");

}
//---------------------------------------------------------------------------

