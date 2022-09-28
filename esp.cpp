//---------------------------------------------------------------------------
#include <windows.h>
#include <vcl.h>
#pragma hdrstop
USERES("esp.res");
USEFORM("C:\Program Files\Borland\CBuilder5\Projects\tez_\tez_ide_026_prj.cpp", Form1);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        HANDLE procHandle = GetCurrentProcess();

        /*if (!SetProcessPriorityBoost(procHandle, TRUE))
                ShowMessage("Process hatasi"); */

        if( !SetPriorityClass(procHandle, HIGH_PRIORITY_CLASS) )
                ShowMessage("Process hatasi");




        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
