/*
 parser.c : analisador sintatico (exemplo de automato com pilha)
 Autor: Edna A. Hoshino

 S  ->   void S_
       | int S_
       | float S_
       | S_
 S_ -> id X 
       | main() { D B } 
       | epsilon
 
 Y  -> id X | main() { D B } 

 X  -> , L ; S
      |; S
      | = num P
      |() {DB} S
      | epsilon
 Y_ -> id X
 P  -> ; S
       | , Y_ S
 Type -> void 
       | int 
       | float

 D -> Type L ; D 
      | epsilon

 L  -> id L_
 L_ -> , L 
     | = num Z_
 Z_ -> , | epsilon
 
 B -> C B 
      | epsilon

 C ->   id C_
      | while (E) C 
      | { B }

C_ ->   = E; 
      | ++;
      | --;
      | += E ;
      | -= E ;
      | /= E ;
      | *= E ;

 E -> TE'
 E'->   +TE' 
      | -TE'
      | epsilon
 T -> FT'
 T'->   * FT' 
       | / FT'
      | epsilon

 F -> (E) 
      | id F_
      | num
      | ++ id
      | -- id
 F_  ->  ++
       | --
       | epsilon

 A saida do analisador apresenta o total de linhas processadas e uma mensagem de erro ou sucesso. 
 Atualmente, nao ha controle sobre a coluna e a linha em que o erro foi encontrado.
*/

#include "lex.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

/* variaveis globais */
int lookahead;
FILE *fin;

int lex();
void S();
void S_();
void Type();
void B();
void C();
void C_();
void E();
void T();
void F();
void F_();
void E_();
void T_();
void D();
void L();
void L_();
void Y();
void Y_();
void X();
void P();
void Z_();

void match(int t)
{
  if(lookahead==t){
    lookahead=lex();
  }
  else{
    printf("\nErro: token %s (cod=%d) esperado.## Encontrado \"%s\" ##\n", terminalName[t], t, lexema);
    exit(1);
  }
}
/**
  S  ->  void S_
       | int S_
       | float S_
       | S_
 */
void S(){
  if(lookahead == VOID){
    match(VOID);
    S_();
  }else if(lookahead == INT){
    match(INT);
    S_();
  }else if(lookahead == FLOAT){
    match(FLOAT);
    S_();
  }else{
    S_();
  }
}
/*
 S_ -> id X 
       | main() { D B } 
       | epsilon
*/
void S_(){
  if(lookahead == ID){
    match(ID);
    X();
  }else if(lookahead == MAIN){
    match(MAIN);
    match(ABRE_PARENT);
    match(FECHA_PARENT);
    match(ABRE_CHAVES);
    D();
    B();
    match(FECHA_CHAVES);
  }else{
    // Eps
  }
}
// Y  -> id X | main() { D B } 
void Y(){
  if(lookahead == ID){
    match(ID);
    X();
  }else{ // Obriga a ser função main
    match(MAIN);
    match(ABRE_PARENT);
    match(FECHA_PARENT);
    match(ABRE_CHAVES);
    D();
    B();
    match(FECHA_CHAVES);
  }
}
/*
 Y_ -> id X
*/
void Y_(){
  match(ID);
  X();
}
 
/*
 X  -> , L ; S
      |; S
      | = num P
      |() {DB} S
      | epsilon
*/
void X(){
  if(lookahead == VIRG){
    match(VIRG);
    L();
    match(PONTO_VIRG);
    S();
  }else if(lookahead == PONTO_VIRG){
    match(PONTO_VIRG);
    S();
  }else if(lookahead == OP_ATRIB){
    match(OP_ATRIB);
    match(NUM);
    P();

  }else if(lookahead == ABRE_PARENT){
    match(ABRE_PARENT);
    match(FECHA_PARENT);
    match(ABRE_CHAVES);
    D();
    B();
    match(FECHA_CHAVES);
    S();
  }else{
    // epsilon
  }
}
/*
  P  -> ; S
       | , Y_ S
*/
void P(){
  if(lookahead==PONTO_VIRG){
    match(PONTO_VIRG);
    S();
  }else if(lookahead==VIRG){
    match(VIRG);
    Y_();
    S();
  }
}

void Type(){  
  if(lookahead==INT){
    match(INT);
  }
  else if(lookahead==FLOAT){
    match(FLOAT);
  }
  else{
    match(VOID);
  }
}
/* D -> Type L ; D 
      | epsilon */
void D()
{
  if(lookahead==INT || lookahead==FLOAT){
    Type();
    L();
    match(PONTO_VIRG);
    D();
  }
}
/* L  -> id L_ */
void L()
{
  match(ID);
  L_();
}
/*  L_ -> , L 
        | epsilon */
void L_()
{
  if (lookahead == VIRG){
    match(VIRG);
    L();
  }else if(lookahead == OP_ATRIB){
    match(OP_ATRIB);
    match(NUM);
    Z_();
  }
}
void Z_(){
   if(lookahead == VIRG){
     match(VIRG);
     L();
   }
}
void B(){
  if(lookahead==ID || lookahead==WHILE || lookahead==ABRE_CHAVES){
    C();
    B();
  }
}
/* 

 C ->   id C_
      | while (E) C 
      | { B }

 */
void C(){
  if(lookahead==ID){
    match(ID);
    C_();
  }
  else if(lookahead==WHILE){
    match(WHILE);
    match(ABRE_PARENT);
    E();
    match(FECHA_PARENT);
    C();
  }
  else if(lookahead==ABRE_CHAVES){
    match(ABRE_CHAVES);
    B();
    match(FECHA_CHAVES);
  }
}
/*
C_ ->   = E; 
      | ++;
      | --;
      | += E ;
      | -= E ;
      | /= E ;
      | *= E ;
*/
void C_(){
 if(lookahead == OP_ATRIB){
   match(OP_ATRIB);
   E();
   match(PONTO_VIRG);
 }else if(lookahead == OP_PLUSPLUS){
   match(OP_PLUSPLUS);
   match(PONTO_VIRG);
 }else if(lookahead ==  OP_PLUS_ATRIB){
   match(OP_PLUS_ATRIB);
   E();
   match(PONTO_VIRG);
 }else if(lookahead == OP_MINUS_ATRIB){
   match(OP_MINUS_ATRIB);
   E();
   match(PONTO_VIRG);
 }else if(lookahead == OP_DIV_ATRIB){
   match(OP_DIV_ATRIB);
   E();
   match(PONTO_VIRG);
 }else if(lookahead == OP_MULT_ATRIB){
   match(OP_MULT_ATRIB);
   E();
   match(PONTO_VIRG);
 }else{ // Obrigado a fazer --;
   match(OP_MINUSMINUS);
   match(PONTO_VIRG);
 }
}

void E(){
  T();
  E_();
}

void T(){
  F();
  T_();
}
/*
 E'->   +TE' 
      | -TE'
      | epsilon
*/
void E_(){
  if(lookahead==OP_ADIT){
    match(OP_ADIT);
    T();
    E_();
  }else if(lookahead == OP_MINUS){
    match(OP_MINUS);
    T();
    E_();
  }else{
    // epsilon
  }
}
/*
 F -> (E) 
      | id F_
      | num
      | ++ id
      | -- id
*/
void F(){
  if(lookahead==ABRE_PARENT){
    match(ABRE_PARENT);
    E();
    match(FECHA_PARENT);
  }else if(lookahead==ID){
      match(ID);
      F_();
  }else if(lookahead==OP_PLUSPLUS){
      match(OP_PLUSPLUS);
      match(ID);
  }else if(lookahead==OP_MINUSMINUS){
      match(OP_MINUSMINUS);
      match(ID);
  }else{ // Obriga a ter um número
      match(NUM);
  }
}
/*
 F_  -> ++
       | --
       | epsilon
*/
void F_(){
  if(lookahead == OP_PLUSPLUS){
   match(OP_PLUSPLUS);
  }else if(lookahead == OP_MINUSMINUS){
   match(OP_MINUSMINUS);
  }else{
    //epsilon
  }
}
/*
 T'->   * FT' 
       | / FT'
      | epsilon
*/
void T_(){
  if(lookahead==OP_MULT){
    match(OP_MULT);
    F();
    T_();
  }else if(lookahead==OP_DIV){
    match(OP_DIV);
    F();
    T_();
  }else{
    // epsilon
  }
}

/*******************************************************************************************
 parser(): 
 - efetua o processamento do automato com pilha AP
 - devolve uma mensagem para indicar se a "palavra" (programa) estah sintaticamente correta.
********************************************************************************************/
char *parser()
{
  lookahead=lex();
  S();
  if(lookahead==FIM)
    return("Programa sintaticamente correto!");
  else
    return("Fim de arquivo esperado");
}

int main(int argc, char**argv)
{
  if(argc<2){
    printf("\nUse: compile <filename>\n");
    return 1;
  }
  else{
    printf("\nAnalisando lexica e sintaticamente o programa: %s", argv[1]);
    fin=fopen(argv[1], "r");
    if(!fin){
      printf("\nProblema na abertura do programa %s\n", argv[1]);
      return 1;
    }
    printf("\nTotal de linhas processadas: %d\nResultado: %s\n", lines, parser());
    fclose(fin);
    return 0;
  }
}

