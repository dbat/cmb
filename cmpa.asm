.686
.xmm
.model flat, stdcall
option casemap: none

.data
.code

public f1CompareMem3
f1CompareMem3 proc P1:DWORD, P2:DWORD, Len:DWORD
;function f1CompareMem3(const P1, P2: pointer; const Length: integer): integer; overload; asm
  mov ecx,Len
  mov eax,P1
  mov edx,P2

  add eax,ecx;
  add edx,ecx;

  not ecx; // xor ecx,-1; // -ecx -1;
  add eax,-8;
  add edx,-8;
  add ecx,9;

  push ebx;
  push esi;
  mov ebx,eax;
  jg @@Dword

  mov eax,[ebx+ecx];
  mov esi,[edx+ecx];

  cmp eax,esi;
  jne @@Ret0;

  lea eax,[ebx+ecx]
  add ecx,4;
  and eax,3;

  sub ecx,eax;
  jg @@Dword;

@@DwordLoop:
  mov eax,[ebx+ecx];
  mov esi,[edx+ecx];

  cmp eax,esi;
  jne @@Ret0;

  mov eax,[ebx+ecx+4];
  mov esi,[edx+ecx+4];

  cmp eax,esi;
  jne @@Ret0;

  add ecx,8;
  jg @@Dword;
  mov eax,[ebx+ecx];
  mov esi,[edx+ecx];

  cmp eax,esi;
  jne @@Ret0;

  mov eax,[ebx+ecx+4];
  mov esi,[edx+ecx+4];

  cmp eax,esi;
  jne @@Ret0;
  add ecx,8;
  jle @@DwordLoop;

@@Dword:
  cmp ecx,4;
  jg @@Word;

  mov eax,[ebx+ecx];
  mov esi,[edx+ecx];

  cmp eax,esi; 
  jne @@Ret0;
  add ecx,4
@@Word:
  cmp ecx,6;
  jg @@Byte;

  movzx eax,word ptr [ebx+ecx];
  movzx esi,word ptr [edx+ecx];

  cmp eax,esi;
  jne @@Ret0;
  add ecx,2;
@@Byte:
  cmp ecx,7;
  jg @@Ret1;
  movzx eax,byte ptr [ebx+7];
  movzx esi,byte ptr [edx+7];
  jmp @@Ret1;

@@Ret0:
  bswap eax;
  bswap esi;
  
@@Ret1:
  sub eax,esi;
  pop esi;
  pop ebx;
  ret
f1CompareMem3 endp

public posdiff_sse
posdiff_sse proc p1: DWORD, p2: DWORD, len: DWORD
;function posdiff_sse(const p1, p2: pchar; const len: integer): integer; stdcall;
;// returns position of byte different. 1-based. 0 means not found.
;asm
  mov ecx,len;
  xor eax,eax;
  test ecx,ecx;
  jle @@Stop;
  push esi;
  push edi;
  mov esi,p1
  mov edi,p2

  shr ecx,4;
  pcmpeqb xmm3,xmm3; // set all bit set

  @@Loop_SSE2:
    movdqu xmm0,oword ptr [esi];
    movdqu xmm1,oword ptr [edi];
    pcmpeqb xmm0,xmm1
    pxor xmm0,xmm3
    pmovmskb eax,xmm0
    test eax,eax;
    jnz @@found
    add esi,16;
    add edi,16;
    sub ecx,1;
    jge @@Loop_SSE2
    ;//jmp @@done
  @@found:
    bsf eax,eax; // will not set zero flags if eax contain value before bsf
    jz @@done;   // even if eax zero after bsf (eax before bsf = 1)
    add eax,esi;
    add eax,1;   // 1-based since zero will denotes a not found value
    mov ecx,len
    sub eax,p1
    xor edx,edx
    cmp eax,ecx
    cmovg eax,edx
  @@done: ;//emms
    pop edi;
    pop esi;
  @@Stop: ret
;end;
posdiff_sse endp;

public posdiff_mmx
posdiff_mmx proc p1: DWORD, p2: DWORD, len: DWORD
;function posdiff_mmx(const p1, p2: pchar; const len: integer): integer; stdcall;
;// returns position of byte different. 1-based. 0 means not found.
;asm
  mov ecx,len;
  xor eax,eax;
  test ecx,ecx;
  jle @@Stop;
  push esi;
  push edi;
  mov esi,p1
  mov edi,p2
  shr ecx,3;

  pcmpeqb mm3,mm3; // set all bit set

  @@Loop8:
    movq mm0,qword ptr [esi];
    movq mm1,qword ptr [edi];
    pcmpeqb mm0,mm1
    pxor mm0,mm3
    pmovmskb eax,mm0
    test eax,eax;
    jnz @@found
    add esi,8;
    add edi,8;
    sub ecx,1;
    jge @@Loop8
    ;//jmp @@done
  @@found:
    bsf eax,eax; // will not set zero flags if eax contain value before bsf
    jz @@done;   // even if eax zero after bsf (eax before bsf = 1)
    add eax,esi;
    add eax,1;   // 1-based since zero will denotes a not found value
    mov ecx,len
    sub eax,p1
    xor edx,edx
    cmp eax,ecx
    cmovg eax,edx
  @@done: emms
    pop edi;
    pop esi;
  @@Stop: ret
;end;
posdiff_mmx endp;


end
