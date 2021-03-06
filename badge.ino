#include <EEPROM.h>

#define DATA_PIN 12
#define LATCH_PIN 11
#define CLOCK_PIN 10
#define BUTTON_PIN 9
#define COL_18 8
#define COL_17 7

volatile uint8_t scene;
volatile uint8_t debounce = false;
const uint8_t number_of_scenes = 2;


//////////GOOD STUFF HERE///////
//CHANGE THIS TO CHANGE SCROLLING TEXT PHRASE
//Heart: {  Paw: |  Sparkle: } Smiley: ~ 
const char* phrase = "}UwU}";

//Can draw static image using this, LED active low
//(Change loop function to not clear bitmap and not text scroll)
volatile uint8_t bitmap[16][18];
volatile int frame_start = 0;
volatile int frame_end = 0;
volatile int dt = 0;


const uint8_t alphabet[][7][7] PROGMEM = { //Font 1 DOS font A-Z only
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1}},//as SPACE
  {{1,1,1,0,0,1,1},{1,1,0,0,0,0,1},{1,1,0,0,0,0,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,1,1,1,1},{1,1,1,0,0,1,1}},//!
  {{1,0,0,1,0,0,1},{1,1,0,1,1,0,1},{1,1,0,1,1,0,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1}},//"
  {{1,0,0,1,0,0,1},{1,0,0,1,0,0,1},{0,0,0,0,0,0,0},{1,0,0,1,0,0,1},{0,0,0,0,0,0,0},{1,0,0,1,0,0,1},{1,0,0,1,0,0,1}},//#
  {{1,1,1,0,1,1,1},{1,0,0,0,0,0,1},{0,0,1,0,1,1,1},{1,0,0,0,0,0,1},{1,1,1,0,1,0,0},{0,0,0,0,0,0,1},{1,1,1,0,1,1,1}},//$
  {{1,1,1,1,1,1,1},{1,0,0,1,1,0,0},{0,1,0,1,0,0,1},{0,0,1,0,0,1,1},{1,1,0,0,1,0,0},{1,0,0,1,0,1,0},{0,0,1,1,0,0,1}},//%
  {{1,1,0,0,0,1,1},{1,0,1,1,0,0,1},{1,1,0,0,0,1,1},{1,0,0,0,0,1,1},{0,0,1,1,0,0,0},{0,0,1,1,0,0,1},{1,0,0,0,0,1,0}},//&
  {{1,1,1,0,0,1,1},{1,1,1,1,0,1,1},{1,1,1,0,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1}},//'
  {{1,1,1,1,0,0,1},{1,1,1,0,0,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,1,0,0,1,1},{1,1,1,1,0,0,1}},//(
  {{1,0,0,1,1,1,1},{1,1,0,0,1,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,0,0,1,1,1},{1,0,0,1,1,1,1}},//)
  {{1,1,1,1,1,1,1},{1,0,0,1,0,0,1},{1,1,0,0,0,1,1},{0,0,0,0,0,0,0},{1,1,0,0,0,1,1},{1,0,0,1,0,0,1},{1,1,1,1,1,1,1}},//*
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,0,1,1,1},{1,1,1,0,1,1,1},{1,0,0,0,0,0,1},{1,1,1,0,1,1,1},{1,1,1,0,1,1,1}},//+
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,0,0,1,1},{1,1,1,1,0,1,1},{1,1,1,0,1,1,1}},//,
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,0,0,0,0,1},{1,1,0,0,0,0,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1}},//-
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1}},//.
  {{0,0,1,1,1,1,1},{1,0,0,1,1,1,1},{1,1,0,0,1,1,1},{1,1,1,0,0,1,1},{1,1,1,1,0,0,1},{1,1,1,1,1,0,0},{1,1,1,1,1,1,0}},///
  {{1,0,0,0,0,0,1},{0,0,1,1,0,0,0},{0,0,1,0,0,0,0},{0,0,0,0,1,0,0},{0,0,0,1,1,0,0},{0,0,0,1,1,0,0},{1,0,0,0,0,0,1}},//0
  {{1,1,0,0,1,1,1},{1,0,0,0,1,1,1},{0,0,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{0,0,0,0,0,0,1}},//1
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{1,1,1,1,1,0,0},{1,1,1,0,0,0,1},{1,0,0,0,1,1,1},{0,0,1,1,1,0,0},{0,0,0,0,0,0,0}},//2
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{1,1,1,1,1,0,0},{1,1,0,0,0,0,1},{1,1,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//3
  {{1,1,1,0,0,0,1},{1,1,0,0,0,0,1},{1,0,0,1,0,0,1},{0,0,1,1,0,0,1},{0,0,0,0,0,0,0},{1,1,1,1,0,0,1},{1,1,1,0,0,0,0}},//4
  {{0,0,0,0,0,0,0},{0,0,1,1,1,1,1},{0,0,0,0,0,0,1},{1,1,1,1,1,0,0},{1,1,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//5
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,1,1},{0,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//6
  {{0,0,0,0,0,0,0},{0,0,1,1,1,0,0},{1,1,1,1,0,0,1},{1,1,1,0,0,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1}},//7
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//8
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,0},{1,1,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//9
  {{1,1,1,1,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,1,1,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,1,1,1,1,1}},//:
  {{1,1,1,1,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,1,1,1,1,1},{1,1,0,0,1,1,1},{1,1,1,0,1,1,1},{1,1,0,1,1,1,1}},//;
  {{1,1,1,1,0,0,1},{1,1,1,0,0,1,1},{1,1,0,0,1,1,1},{1,0,0,1,1,1,1},{1,1,0,0,1,1,1},{1,1,1,0,0,1,1},{1,1,1,1,0,0,1}},//<
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,0,0,0,0,0,1},{1,1,1,1,1,1,1},{1,0,0,0,0,0,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1}},//=
  {{1,0,0,1,1,1,1},{1,1,0,0,1,1,1},{1,1,1,0,0,1,1},{1,1,1,1,0,0,1},{1,1,1,0,0,1,1},{1,1,0,0,1,1,1},{1,0,0,1,1,1,1}},//>
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{1,1,1,1,1,0,0},{1,1,1,1,0,0,1},{1,1,1,0,0,1,1},{1,1,1,1,1,1,1},{1,1,1,0,0,1,1}},//?
  {{1,0,0,0,0,0,1},{0,1,1,1,1,1,0},{0,1,1,0,0,0,0},{0,1,0,1,1,0,0},{0,1,1,0,0,0,0},{0,1,1,1,1,1,1},{1,0,0,0,0,0,1}},//@
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,0,0,0,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0}},//A 
  {{0,0,0,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{0,0,0,0,0,0,1}},//B 
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,1,1},{0,0,1,1,1,1,1},{0,0,1,1,1,1,1},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//C
  {{0,0,0,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{0,0,0,0,0,0,1}},//D
  {{0,0,0,0,0,0,0},{1,0,0,1,1,1,0},{1,0,0,1,0,1,1},{1,0,0,0,0,1,1},{1,0,0,1,0,1,1},{1,0,0,1,1,1,0},{0,0,0,0,0,0,0}},//E
  {{0,0,0,0,0,0,0},{1,0,0,1,1,1,0},{1,0,0,1,0,1,1},{1,0,0,0,0,1,1},{1,0,0,1,0,1,1},{1,0,0,1,1,1,1},{0,0,0,0,1,1,1}},//F
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,1,1},{0,0,1,1,0,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,0}},//G
  {{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,0,0,0,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0}},//H
  {{1,0,0,0,0,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,0,0,0,0,1,1}},//I
  {{1,1,1,0,0,0,0},{1,1,1,1,0,0,1},{1,1,1,1,0,0,1},{1,1,1,1,0,0,1},{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{1,0,0,0,0,1,1}},//J
  {{0,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,1,0,0,1},{1,0,0,0,0,1,1},{1,0,0,1,0,0,1},{1,0,0,1,1,0,0},{0,0,0,1,1,0,0}},//K
  {{0,0,0,0,1,1,1},{1,0,0,1,1,1,1},{1,0,0,1,1,1,1},{1,0,0,1,1,1,1},{1,0,0,1,1,1,0},{1,0,0,1,1,0,0},{0,0,0,0,0,0,0}},//L
  {{0,1,1,1,1,1,0},{0,0,1,1,1,0,0},{0,0,0,1,0,0,0},{0,0,0,0,0,0,0},{0,0,1,0,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0}},//M
  {{0,0,1,1,1,0,0},{0,0,0,1,1,0,0},{0,0,0,0,1,0,0},{0,0,1,0,0,0,0},{0,0,1,1,0,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0}},//N
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//O
  {{0,0,0,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,0,0,0,1},{1,0,0,1,1,1,1},{1,0,0,1,1,1,1},{0,0,0,0,1,1,1}},//P
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,0,1,0,0},{0,0,1,0,0,0,0},{1,0,0,0,0,0,1},{1,1,1,1,1,0,0}},//Q
  {{0,0,0,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{0,0,0,1,1,0,0}},//R
  {{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,1,1},{1,0,0,0,0,0,1},{1,1,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//S
  {{0,0,0,0,0,0,1},{0,1,0,0,1,0,1},{0,1,0,0,1,0,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,0,0,0,0,1,1}},//T
  {{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//U   
  {{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,1,0,0,1},{1,1,0,0,0,1,1},{1,1,1,0,1,1,1}},//V
  {{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,0,1,0,0},{0,0,0,0,0,0,0},{0,0,0,1,0,0,0},{0,0,1,1,1,0,0},{0,1,1,1,1,1,0}},//W
  {{0,0,1,1,1,0,0},{1,0,0,1,0,0,1},{1,1,0,0,0,1,1},{1,1,0,0,0,1,1},{1,1,0,0,0,1,1},{1,0,0,1,0,0,1},{0,0,1,1,1,0,0}},//X
  {{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{1,0,0,0,0,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,0,0,0,0,1,1}},//Y
  {{0,0,0,0,0,0,0},{0,0,1,1,1,0,0},{0,1,1,1,0,0,1},{1,1,1,0,0,1,1},{1,1,0,0,1,1,0},{1,0,0,1,1,0,0},{0,0,0,0,0,0,0}},//Z
  {{1,1,0,0,0,0,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,1,0,0,0,0,1}},//[
  {{0,1,1,1,1,1,1},{0,0,1,1,1,1,1},{1,0,0,1,1,1,1},{1,1,0,0,1,1,1},{1,1,1,0,0,1,1},{1,1,1,1,0,0,1},{1,1,1,1,1,0,0}},//Backslash
  {{1,1,0,0,0,0,1},{1,1,1,1,0,0,1},{1,1,1,1,0,0,1},{1,1,1,1,0,0,1},{1,1,1,1,0,0,1},{1,1,1,1,0,0,1},{1,1,1,0,0,0,1}},//]
  {{1,1,1,0,1,1,1},{1,1,0,0,0,1,1},{1,0,0,1,0,0,1},{0,0,1,1,1,0,0},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1}},//^
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{0,0,0,0,0,0,0}},//_
  {{1,1,1,0,0,1,1},{1,1,1,0,1,1,1},{1,1,1,1,0,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,1,1,1,1,1,1}},//`
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,0,0,0,0,1,1},{1,1,1,1,0,0,1},{1,0,0,0,0,0,1},{0,0,1,1,0,0,1},{1,0,0,0,1,0,0}},//a
  {{0,0,0,1,1,1,1},{1,0,0,1,1,1,1},{1,0,0,1,1,1,1},{1,0,0,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,0,0,0,1}},//b
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,1,1},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//c
  {{1,1,1,0,0,0,1},{1,1,1,1,0,0,1},{1,1,1,1,0,0,1},{1,0,0,0,0,0,1},{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{1,0,0,0,1,0,0}},//d
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,0,0,0,0,0},{0,0,1,1,1,1,1},{1,0,0,0,0,0,1}},//e
  {{1,1,1,0,0,0,1},{1,1,0,0,1,0,0},{1,1,0,0,1,1,1},{1,0,0,0,0,1,1},{1,1,0,0,1,1,1},{1,1,0,0,1,1,1},{1,0,0,0,0,1,1}},//f
  {{1,1,1,1,1,1,1},{1,0,0,0,1,0,0},{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{1,0,0,0,0,0,1},{1,1,1,1,0,0,1},{1,0,0,0,0,1,1}},//g
  {{0,0,0,1,1,1,1},{1,0,0,1,1,1,1},{1,0,0,1,0,0,1},{1,0,0,0,1,0,0},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{0,0,0,1,1,0,0}},//h
  {{1,1,1,0,0,1,1},{1,1,1,1,1,1,1},{1,1,0,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,0,0,0,0,1}},//i
  {{1,1,1,1,1,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,0,0},{1,1,1,1,1,0,0},{1,1,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//j
  {{0,0,0,1,1,1,1},{1,0,0,1,1,1,1},{1,0,0,1,1,0,0},{1,0,0,1,0,0,1},{1,0,0,0,0,1,1},{1,0,0,1,0,0,1},{0,0,0,1,1,0,0}},//k
  {{1,1,0,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,0,0,0,0,1}},//l
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{0,0,1,1,0,0,1},{0,0,0,0,0,0,0},{0,0,1,0,1,0,0},{0,0,1,0,1,0,0},{0,0,1,0,1,0,0}},//m
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{0,0,1,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0}},//n
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,0,0,0,0,0,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,0,0,0,1}},//o
  {{1,1,1,1,1,1,1},{0,0,1,0,0,0,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,0,0,0,1},{1,0,0,1,1,1,1},{0,0,0,0,1,1,1}},//p
  {{1,1,1,1,1,1,1},{1,0,0,0,0,0,1},{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{1,0,0,0,0,0,1},{1,1,1,1,0,0,1},{1,1,1,0,0,0,0}},//q
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{0,0,1,0,0,0,0},{1,0,0,0,1,0,0},{1,0,0,1,1,1,1},{1,0,0,1,1,1,1},{0,0,0,0,1,1,1}},//r
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,0,0,0,0,0,1},{0,0,1,1,1,1,1},{1,0,0,0,0,0,1},{1,1,1,1,1,0,0},{1,0,0,0,0,0,1}},//s
  {{1,1,1,1,0,1,1},{1,1,1,0,0,1,1},{1,0,0,0,0,0,0},{1,1,1,0,0,1,1},{1,1,1,0,0,1,1},{1,1,1,0,0,1,0},{1,1,1,1,0,0,1}},//t
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{0,0,1,1,0,0,1},{1,0,0,0,1,0,0}},//u
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{0,0,1,1,1,0,0},{0,0,1,1,1,0,0},{1,0,0,1,0,0,1},{1,1,0,0,0,1,1},{1,1,1,0,1,1,1}},//v
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{0,0,1,1,1,0,0},{0,0,1,0,1,0,0},{0,0,1,0,1,0,0},{0,0,0,0,0,0,0},{1,0,0,1,0,0,1}},//w
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{0,0,1,1,1,0,0},{1,0,0,1,0,0,1},{1,1,0,0,0,1,1},{1,0,0,1,0,0,1},{0,0,1,1,1,0,0}},//x
  {{1,1,1,1,1,1,1},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,0,0,1,1,0,0},{1,1,0,0,0,0,0},{1,1,1,1,1,0,0},{1,0,0,0,0,0,1}},//y
  {{1,1,1,1,1,1,1},{1,1,1,1,1,1,1},{1,0,0,0,0,0,0},{1,0,1,1,0,0,1},{1,1,1,0,0,1,1},{1,1,0,0,1,1,0},{1,0,0,0,0,0,0}},//z
  {{1,1,1,1,1,1,1},{1,0,0,1,0,0,1},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{1,0,0,0,0,0,1},{1,1,0,0,0,1,1},{1,1,1,0,1,1,1}},//Heart "{"
  {{1,1,0,1,0,1,1},{0,1,0,1,0,1,0},{0,1,1,1,1,1,0},{1,1,0,0,0,1,1},{1,0,0,0,0,0,1},{1,0,0,0,0,0,1},{1,1,0,1,0,1,1}},//Paw "|"
  {{1,1,1,0,1,1,1},{1,1,1,0,1,1,1},{1,1,0,1,0,1,1},{0,0,1,1,1,0,0},{1,1,0,1,0,1,1},{1,1,1,0,1,1,1},{1,1,1,0,1,1,1}},//Diamond "}"
  {{1,0,0,0,0,0,1},{0,1,1,1,1,1,0},{0,1,0,1,0,1,0},{0,1,1,1,1,1,0},{0,1,0,1,0,1,0},{0,1,1,0,1,1,0},{1,0,0,0,0,0,1}},//Smile "~"
};

volatile uint8_t image[16][18] = {
  {0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0},
  {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0},
  {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,0},
  {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
  {0,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,1,1},
  {0,0,1,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1},
  {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {0,0,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
  {0,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,1,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0},
};


void setup() {
  Serial.begin(9600);
  pinMode(DATA_PIN,  OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(COL_18,    OUTPUT); //Col 18
  pinMode(COL_17,    OUTPUT); //Col 17

  digitalWrite(LATCH_PIN, LOW); 
  clear_bmp();

  scene = EEPROM.read(0);
}

float i = 0.f;
void loop() {
  dt = frame_end - frame_start;
  frame_start = frame_end;

  check_scene();
  switch(scene) {
    case 0:
      i -= dt / 60000.f;
      if ((int)(-i) == strlen(phrase)*8+22) i = 0.f;
      
      clear_bmp();
      text_stream((int)i);
      push();
    break;
    case 1:
      clear_bmp();
      for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 18; j++) {
          bitmap[i][j] = image[i][j];
        }
      }
      push();
  }

  EEPROM.update(0, scene);
  frame_end = micros();
}

void check_scene() {
  if (!digitalRead(BUTTON_PIN)) {
    if (!debounce) scene++;
    debounce = true;
  }
  else debounce = false;

  if (scene >= number_of_scenes) scene = 0;
}


void clear_bmp() { //Clears bitmap, readys bitmap to be drawn upon
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 18; j++) {
      bitmap[i][j] = 1;
    }
  }
}
void fill_bmp() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 18; j++) {
      bitmap[i][j] = 0;
    }
  }
}
void plot_letter(int row, int col, int size_x, int size_y, uint8_t* bmp) { //Draws an image of irregular size to the bitmap buffer
  if (row < 1-size_y || 17 < row) return;
  if (col < 1-size_x || 15 < col) return;

  int start_x = (col < 0) ? 0 : col;
  int end_x = ((col + size_x) > 16) ? 16 : (col + size_x);
  int start_y = (row < 0) ? 0 : row;
  int end_y = ((row + size_y) > 18) ? 18 : (row + size_y); 

  for (int i = start_x, a = (start_x == 0) ? size_x - end_x : 0; i < end_x; i++, a++) {
    for (int j = start_y, b = (start_y == 0) ? size_y - end_y : 0; j < end_y; j++, b++) {
      bitmap[i][j] = pgm_read_byte_near(bmp + (a * size_x + b));
    }
  }
}
void text_stream(int x) { //Create text phrase that scrolls along the draw window
  for (int i = 0; i < strlen(phrase); i++) {
    plot_letter(22 + x + i*8,5,7,7,&alphabet[phrase[i]-32][0][0]);
  }
}

void push() { //Pushes out the Bitmap to the LEDS, essentially the draw call function
  for (int i = 0; i < 16; i++) {
    byte reg1 = (bitmap[i][0] << 7) | (bitmap[i][1] << 6) | (bitmap[i][2] << 5) | (bitmap[i][3] << 4) | (bitmap[i][4] << 3) | (bitmap[i][5] << 2) | (bitmap[i][6] << 1) | (bitmap[i][7]);
    byte reg2 = (bitmap[i][8] << 7) | (bitmap[i][9] << 6) | (bitmap[i][10] << 5) | (bitmap[i][11] << 4) | (bitmap[i][12] << 3) | (bitmap[i][13] << 2) | (bitmap[i][14] << 1) | (bitmap[i][15]);

    digitalWrite(LATCH_PIN, LOW);

    digitalWrite(COL_18, bitmap[i-1][17]);
    digitalWrite(COL_17, bitmap[i-1][16]);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, reg2);//Fourth Register Col 8-16
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, reg1);//Third Register Col 0-8
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, 0x8000 >> i);//Second Register Row 8-16
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, 0x8000 >> (i + 8));//First Register Row 0-8

    digitalWrite(LATCH_PIN, HIGH);
    
  }
}
