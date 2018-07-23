/*
LED controller + memo --- ESP-WROOM-02 ( ESP8266 )
Please rewrite their own the ssid and password.
Please rewrite their own local IP address of "/data/spiffs_01.txt" in the sketch folder.(ex: ws://192.168.4.1)
Use the SPIFFS file system uploader, please upload the spiffs_01.txt file to flash.
Arduino IDE ver 1.6.11 での
ESP8266 board ver 4　実行可能

*/
#include <EasyWebSocket.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <OLEDDisplay.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplayUi.h>

const char* ssid = "ESP-WROOM-02-AP"; //wifi名
const char* password = "xxxxxxxxx"; //8文字以上でWPA2モード。8文字未満はパスワード無しでアクセス可能

long CountTime;

byte cnt = 0;

EasyWebSocket ews;

String html_str1;
String html_str2;
String html_str3;
String html_str4;
String html_str5;
String html_str6;
String html_str7;

String ret_str;
String txt;
String str;
String message1="---";
String message2="---";
String message3="---";
String message4="---";
String message5="---";


int PingSendTime = 30000;

#define ledPin1 12 //GPIO #12 blue
#define ledPin2 13 //GPIO #13 green
#define ledPin3 14 //GPIO #14 red

void setup() 
{
  //The assignment in the Body element of the browser of HTML to a String variable
  html_str1 = "<body style='background:#fff; color:#000;'>\r\n";
  html_str1 += "<font size=5>\r\n";
  html_str1 += "LED controller + memo\r\n";
  html_str1 += "</font><font size=4><br>\r\n";
  html_str1 += "推奨ブラウザ：Chrome ,　推奨機器：スマホ \r\n";
  html_str1 += "</font><br><br>\r\n";
  html_str1 += ews.EWS_BrowserSendRate();
  html_str1 += "<br>\r\n";
  html_str1 += "接続状態 = \r\n";
  html_str1 += ews.EWS_Status_Text(20,"#FF00FF");
  html_str1 += "<br><br>\r\n";
  html_str1 += ews.EWS_TextBox_Send("txt1", "こんにちは ","送信");
  html_str1 += "<br>\r\n";
  html_str1 += "<font size=4>\r\n";
  html_str1 += "受け取ったメッセージリスト（新着5行まで残る）\r\n";
  html_str1 += "</font><br>\r\n";
  //  メッセージ表
  html_str2 = "<table style = 'border-width:medium;background-color:#333;'><tr><td>";
  html_str2 += ews.EWS_BrowserReceiveTextTag("message1",20,"#FFFFFF");
  html_str2 += "</td></tr><tr><td>";
  html_str2 += ews.EWS_BrowserReceiveTextTag("message2",20,"#FFFFFF");
  html_str2 += "</td></tr><tr><td>";
  html_str2 += ews.EWS_BrowserReceiveTextTag("message3",20,"#FFFFFF");
  html_str2 += "</td></tr><tr><td>";
  html_str3 += ews.EWS_BrowserReceiveTextTag("message4",20,"#FFFFFF");
  html_str3 += "</td></tr><tr><td>";
  html_str3 += ews.EWS_BrowserReceiveTextTag("message5",20,"#FFFFFF");
  html_str3 += "</td></tr></table>";
  
  html_str4 = "<br>\r\n";
  html_str4 += "<div style = 'display:inline-block; padding:10px; margin-bottom:10px; border:1px dotted#333;'>\r\n";
  html_str4 += "<font size=5>\r\n";
  html_str4 += "LED \r\n";
  html_str4 += "</font><br>\r\n";
  html_str4 += ews.EWS_On_Momentary_Button("ALL", "全点灯", 65,25,15,"#000000","#AAAAAA");
  html_str4 += "... \r\n";
  html_str4 += ews.EWS_On_Momentary_Button("OUT", "全消灯", 65,25,15,"#FFFFFF","#555555");
  html_str4 += "<br>\r\n";
  
  html_str5 = "<br>\r\n";
  html_str5 += "BLUE<br>\r\n";
  html_str5 += ews.EWS_OnOff_Button("blue",60,25,15,"#FFFFFF","#0000FF");
  html_str5 += ews.EWS_Touch_Slider_T("blue", "Txt1");
  html_str5 += ews.EWS_Sl_Text("Txt1",15,"#5555FF");
  html_str5 += "<br><br>\r\n";
  html_str5 += "GREEN<br>\r\n";
  html_str5 += ews.EWS_OnOff_Button("green",60,25,15,"#FFFFFF","#00FF00");
  html_str5 += ews.EWS_Touch_Slider_T("green", "Txt2");
  html_str5 += ews.EWS_Sl_Text("Txt2",15,"#00ff00");
  html_str5 += "<br><br>\r\n";
  
  html_str6 = "RED<br>\r\n";
  html_str6 += ews.EWS_OnOff_Button("red",60,25,15,"#FFFFFF","#ff0000");
  html_str6 += ews.EWS_Touch_Slider_T("red", "Txt3");
  html_str6 += ews.EWS_Sl_Text("Txt3",15,"#ff0000");
  html_str6 += "</div><br><br>\r\n";

  html_str6 += ews.EWS_WebSocket_Reconnection_Button("再接続", 200, 40, 17);
  html_str6 += "<br><br>\r\n";  
  html_str6 += ews.EWS_Close_Button("切断",150,40,17);
  html_str6 += ews.EWS_Window_ReLoad_Button("再読み込み",150,40,17);
  html_str6 += "</body></html>\r\n";

  html_str7 = ""; //The description here is if the string is too large. If you do not want to use it is empty.
  
  ews.SoftAP_setup(ssid, password);
  
  CountTime = millis();
}

void loop() {
  ews.EWS_HandShake(html_str1, html_str2, html_str3, html_str4, html_str5, html_str6, html_str7);
//  String str;
  str = txt;
  if(ret_str != "_close"){
    if(millis()-CountTime > 10){//Data transmission from WROOM (ESP8266) every 500ms
      //cnt = 0;
      //read_word();
      //ews.EWS_ESP8266_Str_SEND(str, "wroomTXT");
      CountTime = millis();
    }

    ret_str = ews.EWS_ESP8266CharReceive(PingSendTime);
    if(ret_str != "\0"){
      Serial.println(ret_str);
      if(ret_str != "Ping"){
        if(ret_str[0] != 't'){
          int ws_data = (ret_str[0]-0x30)*100 + (ret_str[1]-0x30)*10 + (ret_str[2]-0x30);
          switch(ret_str[4]){
            case 'B':
              LED_PWM(1, ledPin1, floor(ws_data/2));
              break;
            case 'G':
              LED_PWM(1, ledPin2, floor(ws_data/2));
              break;
            case 'R':
              LED_PWM(1, ledPin3, floor(ws_data/2));
              break;
            case '_':
              LED_PWM(2, 0, floor(ws_data/2));
              break;
            case 'b':
              LED_PWM(1, ledPin1, ws_data);
              break;
            case 'g':
              LED_PWM(1, ledPin2, ws_data);
              break;
            case 'r':
              LED_PWM(1, ledPin3, ws_data);
              break;
            case '-':
              LED_PWM(2, 0, ws_data);
              break;
            case 'A':
              LED_PWM(3, 0, ws_data);
              break;
            case 'O':
              LED_PWM(4, 0, ws_data);
              break;
          }
        }else if(ret_str[0] == 't'){
          txt = ret_str.substring(ret_str.indexOf('|')+1, ret_str.length()-1);
          Serial.println(txt);
          //cnt=1;
          //read_word();
          //ews.EWS_ESP8266_Str_SEND(txt, "wroomTXT");
          message_list();
        }
      }
    }
  }else if(ret_str == "_close"){
    //read_word();
    //ews.EWS_ESP8266_Str_SEND(str, "wroomTXT");
    CountTime = millis();
    ret_str = "";
  }
}
//void read_word( ){
//  switch(cnt){
//    case 0:
//      str = "No messege";
//      break;
//    case 1:
//      str = txt;
//      break;
//  }
//}
//**************************************************************
void message_list(){
  if(message1=="---"){
    message1=txt;
  }
  else if(message1!="---"){
    if(message2=="---"){
      message2=txt;
    }
    else if(message2!="---"){
      if(message3=="---"){
        message3=txt;
      }
      else if(message3!="---"){
        if(message4=="---"){
          message4=txt;
        }
        else if(message4!="---"){
          if(message5=="---"){
            message5=txt;
          }
          else if(message5!="---"){ //  すべて埋まったらずらす
            message1=message2;
            message2=message3;
            message3=message4;
            message4=message5;
            message5=txt;
          }
        }
      }
    }
  }
  //ews.EWS_ESP8266_Str_SEND(txt, "message1");
  ews.EWS_ESP8266_Str_SEND(message1, "message1");
  ews.EWS_ESP8266_Str_SEND(message2, "message2");
  ews.EWS_ESP8266_Str_SEND(message3, "message3");
  ews.EWS_ESP8266_Str_SEND(message4, "message4");
  ews.EWS_ESP8266_Str_SEND(message5, "message5");
  //Serial.println();
}
void LED_PWM(byte Led_gr, byte Led_Pin, int data_i)
{
Serial.println(data_i);
  switch(Led_gr){
    case 1:
      analogWrite(Led_Pin, data_i);
      break;
    case 2:
      if(data_i < 34){
        analogWrite(ledPin1, data_i*8);
        analogWrite(ledPin2, 0);
        analogWrite(ledPin3, 0);
      }else if(data_i > 33 && data_i < 67){
        analogWrite(ledPin2, (data_i-33)*8);
        analogWrite(ledPin1, 0);
        analogWrite(ledPin3, 0);
      }else if(data_i > 66){
        analogWrite(ledPin3, (data_i-66)*7.78);
        analogWrite(ledPin1, 0);
        analogWrite(ledPin2, 0);
      }
      break;
    case 3:
      analogWrite(ledPin1, 100);
      analogWrite(ledPin2, 100);
      analogWrite(ledPin3, 100);
      break;
    case 4:
      analogWrite(ledPin1, 0);
      analogWrite(ledPin2, 0);
      analogWrite(ledPin3, 0);
      break;
  }
}
