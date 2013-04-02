//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "math.h"
#include <time.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
        int line = 0;
        HANDLE* hThreads;
        int* A;
        int* B;
        int* C;
        int ArrayLength = 500;
        int ThreadCount = 4;
        HANDLE SynEvent;
        clock_t start, end;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{

A = (int*)malloc(ArrayLength * ArrayLength * sizeof(int));
B = (int*)malloc(ArrayLength * ArrayLength * sizeof(int));
C = (int*)malloc(ArrayLength * ArrayLength * sizeof(int));


Randomize();
for(int i=0;i<500;i++)
for(int j=0;j<500;j++)
{
 A[i,j]=random(100);
 B[i,j]=random(100);
 C[i,j]=0;
}
}
//---------------------------------------------------------------------------

__fastcall MyThread::MyThread(bool CreateSuspended):TThread(CreateSuspended)
 {

 }
//---------------------------------------------------------------------------
void __fastcall MyThread::Execute()
{

        FreeOnTerminate = false;
        WaitForSingleObject(SynEvent,INFINITE);

        for(int j = 0; j < ArrayLength; j++)
        for(int k = 0; k < ArrayLength; k++)
                C[this->line,j] += A[this->line,k] * B[k,j];

}
//----------------------------------------------------------------------------
void MyThread::SetLine(int index)
{
 this->line = index;
}
//-----------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
 free(A);
 free(B);
 free(C);
 free(hThreads);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
  ThreadCount = Edit1->Text.ToInt();
  hThreads = (HANDLE*)malloc(ThreadCount * sizeof(HANDLE));
  SynEvent=CreateEvent(NULL,TRUE,FALSE,"event");     //создаем событие с ручным сбросом.
  start = clock();
  Chart1->Series[0]->Clear();

  for(int i=0;i<ThreadCount;i++){
        MyThread *thread = new MyThread(true);//true - в приостановленном режиме
        thread->Priority = tpLower;
        hThreads[i]=thread;
        }

  while(ThreadCount>0)
   {
     ResetEvent(SynEvent);
     for(int i=0;i<ThreadCount;i++)
     {
      if(line==ArrayLength-1)break;
      MyThread* p = (MyThread*)hThreads[i];
      p->SetLine(line++);
      p->Resume();
     }
     SetEvent(SynEvent);
     WaitForMultipleObjects(ThreadCount, &hThreads[0],true/*what all*/, INFINITE);
     if(line==ArrayLength-1)//последняя строка
     {
       if(ThreadCount>=2){
                DrawThreads();
                ThreadCount--;
                line = 0;
                }else{
                        DrawThreads();
                        break;
                        }
     }
   }
   free(hThreads);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::DrawThreads()
{
   end = clock();
   double result = ((double)(end - start));//ms; //промежуточное время
   Chart1->Series[0]->Add(result,ThreadCount);
   ListBox1->Items->Add(IntToStr(start)+" "+IntToStr(end));
   Form1->Repaint();
   start = clock();
}