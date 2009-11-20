Program MCM2KSS;
{$Y-}
{$I-}

VAR  Z80Buffer  : array [0..32768] OF Byte; {Buffer de memoria}
     IFile      : File of Byte;             {Fichero para leer}
     OFile      : File of Byte;             {Para escribir el KSS}
     TFile      : Text;                     {Para escribir el M3U}
     MCMFile    : Array [0..99] Of String;  {Ficheros con las canciones}
     KSSFile    : String;  {Fichero destino para el KSS}
     M3UFile    : String;  {Fichero destino para el M3U}
     Par,Ext    : String;  {Extensi¢n del fichero}
     KSSLength  : Word;    {Tama¤o del KSS}
     CopyLength : Word;    {Tama¤o de la parte Z80}
     B,P,NDC    : Byte;

Procedure ReadFromIFile;
Begin
  If ( IOResult <> 0 ) Then
     Begin
        Halt(2); {No hemos encontrado el fichero que buscabamos}
     End;
  While ( Not(Eof(IFile)) ) do
        Begin
          Read(IFile,Z80Buffer[KSSLength]);
          KSSLength:=KSSLength+1;
        End;
End;

Procedure WriteToOFile;
Begin
  While ( CopyLength < KSSLength ) Do
    Begin
      Write(OFile,Z80Buffer[CopyLength]);
      CopyLength:=CopyLength+1;
    End;
End;

Begin
  {Primer paso: cargamos el driver en memoria}
  KSSLength:=0;
  Assign(IFile,'driver.bin');
  Reset(IFile);
  ReadFromIFile;
  Close(IFile);

  {Segundo paso: ficheros de lectura y escritura}
  MCMFile[0]:='';
  KSSFile:='';
  If ( ParamCount > 0 )
     Then Begin
            P:=0;
            NDC:=0;
            While ( P < ParamCount ) Do
              Begin
                P:=P+1;
                Par:=ParamStr(P);
                Ext:=UpCase(Par[Length(Par)-2])+
                     UpCase(Par[Length(Par)-1])+
                     UpCase(Par[Length(Par)]);
                If ( Ext = 'MCM' )
                   Then Begin
                          MCMFile[NDC]:=Par;
                          NDC:=NDC+1;
                        End
                   Else If ( ( Ext = 'KSS' ) AND ( KSSFile = '' ) )
                           Then Begin
                                  KSSFile:=Par;
                                  Par[Length(Par)-2]:='m';
                                  Par[Length(Par)-1]:='3';
                                  Par[Length(Par)]:='u';
                                  M3UFile:=Par;
                                End;
              End;
          End;

  {NDC lleva el numero de canciones que se quieren incluir}

  {Tercer paso: Colocamos la cancion 0 tras el driver}
  Assign(IFile,MCMFile[0]);
  Reset(IFile);
  ReadFromIFile;
  Close(IFile);

  {Longitud a copiar por el Z80 en Low Endian}
  Z80Buffer[6]:=(KSSLength-16) MOD 256;
  Z80Buffer[7]:=(KSSLength-16) DIV 256;

  {Cuarto paso: Comprobamos si se trata de un fichero multicancion}
  If ( NDC > 1 ) Then Z80Buffer[13]:=NDC Div 2;

  {Escribimos el driver con la cancion 0 pegada}
  CopyLength:=0;
  Assign(OFile,KSSFile);
  Rewrite(OFile);
  WriteToOFile;

  P:=1;
  While ( P < NDC ) Do
        Begin
          KSSLength:=0;
          Assign(IFile,MCMFile[P]);
          Reset(IFile);
          ReadFromIFile;
          Close(IFile);
          While ( KSSLength < 8192 ) Do
                Begin
                  Z80Buffer[KSSLength]:=0;
                  KSSLength:=KSSLength+1;
                End;
          CopyLength:=0;
          WriteToOFile;
          P:=P+1;
        End;
  Close(OFile);

  {Por ultimo generamos el M3U si es necesario}
  If ( M3UFile <> '' )
     Then Begin
            Assign(TFile,M3UFile);
            ReWrite(TFile);
            P:=0;
            If ( NDC = 0 )
               Then Begin
                      {Hay una unica cancion que entro por STDIN}
                      MCMFile[0]:='<STDIN>';
                      NDC:=1;
                    End;
            While ( P < NDC ) Do
                  Begin
                    Write(TFile,KSSFILE+'::KSS, ');
                    Write(TFile,P);
                    Writeln(TFile,','+MCMFile[P]+',,,0');
                    P:=P+1;
                  End;
            Close(TFile);
          End;
End.
