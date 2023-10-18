#include <WString.h>


//  const char myString[] PROGMEM = R”rawliteral(<!DOCTYPE html><html style=font-size:16px><meta content="width=device-width,initial-scale=1"name=viewport><meta charset=utf-8><meta content=""name=keywords><meta content=""name=description><meta content=np-template-header-footer-from-plugin name=page_type><title>LED</title><link href=nicepage.css media=screen rel=stylesheet><link href=LED.css media=screen rel=stylesheet><meta content=LED property=og:title><meta content=website property=og:type><body class="u-body u-xl-mode"><header class="u-clearfix u-header u-header"id=sec-8b74><div class="u-clearfix u-sheet u-sheet-1"></div></header><section class="u-clearfix u-section-1"id=sec-9d5b><div class="u-clearfix u-sheet u-sheet-1"><a class="u-btn u-button-style u-hover-palette-1-dark-1 u-palette-1-base u-btn-1"href=/LED/on>ВКЛ</a><p class="u-text u-text-default u-text-1">Состояние:<p class="u-text u-text-default u-text-2">ОТКЛ</p><a class="u-btn u-button-style u-hover-palette-1-dark-1 u-palette-1-base u-btn-2"href=/LED/off>ОТКЛ</a></div></section>)rawliteral”;


const char htmlLedOn[] = R"====(
<!DOCTYPE html>
<html style="font-size: 16px;">


  <head>
    <link rel="shortcut icon" href="data:image/x-icon;," type="image/x-icon"> 
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta charset="utf-8">
    <title>LED</title>

    <meta property="og:title" content="LED">
  </head>
  <body class="u-body u-xl-mode"><header class="u-clearfix u-header u-header" id="sec-8b74"><div class="u-clearfix u-sheet u-sheet-1"></div></header>
    <section class="u-clearfix u-section-1" id="sec-9d5b">
      <div class="u-clearfix u-sheet u-sheet-1">
        <a href="/LED/on" class="u-btn u-button-style u-hover-palette-1-dark-1 u-palette-1-base u-btn-1">ВКЛ</a>
        <a href="/LED/off" class="u-btn u-button-style u-hover-palette-1-dark-1 u-palette-1-base u-btn-2">ОТКЛ</a>

        <p class="u-text u-text-default u-text-1">Состояние:</p>
        <p class="u-text u-text-default u-text-2">ВКЛ</p>
      </div>
    </section>
    
  </body>
</html>  
    )====";


const char htmlLedOff[] = R"====(
<!DOCTYPE html>
<html style="font-size: 16px;">

  <head>
  <link rel="icon" href="data:,">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta charset="utf-8">
    <title>LED</title>

    <meta property="og:title" content="LED">
  </head>
  <body class="u-body u-xl-mode"><header class="u-clearfix u-header u-header" id="sec-8b74"><div class="u-clearfix u-sheet u-sheet-1"></div></header>
    <section class="u-clearfix u-section-1" id="sec-9d5b">
      <div class="u-clearfix u-sheet u-sheet-1">
        <a href="/LED/on" class="u-btn u-button-style u-hover-palette-1-dark-1 u-palette-1-base u-btn-1">ВКЛ</a>
        <a href="/LED/off" class="u-btn u-button-style u-hover-palette-1-dark-1 u-palette-1-base u-btn-2">ОТКЛ</a>

        <p class="u-text u-text-default u-text-1">Состояние:</p>
        <p class="u-text u-text-default u-text-2">ОТКЛ</p>
      </div>
    </section>
    
  </body>
</html>   
    )====";

// String htmlOn2 ="<!DOCTYPE html><html><head><title>Web Server</title><style>form{width:420px}body
// {background-color:#0dcaf4}html{text-align:center}label{display:inline-block;width:240px;text-align:center}button,input{float:center}</style><meta charset=utf-8><title>Ввод числа</title><meta http-equiv=Refresh content=15>
// </head><body><form method=get><div><label>Введите число от 1 до 125:</label></div><p><input type=number size=3 name=num min=1 max=125 step=1 value=1></p><div><button>Отправить значение</button></div></form><span class=dot2>
// </span></body></html>";

    

// <link rel="shortcut icon" href="#" />
//    <link rel="shortcut icon" href="data:image/x-icon;," type="image/x-icon"> 

    // <link rel="icon" type="image/png" href="data:image/png;base64,iVBORw0KGgo=">
//  <link rel="icon" href="data:,">

//  const char myString_1[] PROGMEM = R"rawliteral(<!DOCTYPE html><html style=font-size:16px><meta content="width=device-width,initial-scale=1"name=viewport><meta charset=utf-8><meta content=""name=keywords><meta content=""name=description><meta content=np-template-header-footer-from-plugin name=page_type><title>LED</title><link href=nicepage.css media=screen rel=stylesheet><link href=LED.css media=screen rel=stylesheet><meta content=LED property=og:title><meta content=website property=og:type><body class="u-body u-xl-mode"><header class="u-clearfix u-header u-header"id=sec-8b74><div class="u-clearfix u-sheet u-sheet-1"></div></header><section class="u-clearfix u-section-1"id=sec-9d5b><div class="u-clearfix u-sheet u-sheet-1"><a class="u-btn u-button-style u-hover-palette-1-dark-1 u-palette-1-base u-btn-1"href=/LED/on>ВКЛ</a><p class="u-text u-text-default u-text-1">Состояние:<p class="u-text u-text-default u-text-2">ОТКЛ</p><a class="u-btn u-button-style u-hover-palette-1-dark-1 u-palette-1-base u-btn-2"href=/LED/off>ОТКЛ</a></div></section>)rawliteral";

