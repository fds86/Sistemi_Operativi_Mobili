/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "mpu6050-esp32", "index.html", [
    [ "Wire Cheat Sheet (ESP32 + PlatformIO)", "md_docs_2wire__library__cheat__sheet.html", [
      [ "Setup", "md_docs_2wire__library__cheat__sheet.html#autotoc_md1", [
        [ "1) begin(sda, scl, frequency)", "md_docs_2wire__library__cheat__sheet.html#autotoc_md2", null ],
        [ "2) setClock(frequency)", "md_docs_2wire__library__cheat__sheet.html#autotoc_md3", null ],
        [ "3) setTimeOut(ms)", "md_docs_2wire__library__cheat__sheet.html#autotoc_md4", null ],
        [ "4) getTimeOut()", "md_docs_2wire__library__cheat__sheet.html#autotoc_md5", null ]
      ] ],
      [ "Scrittura verso uno slave", "md_docs_2wire__library__cheat__sheet.html#autotoc_md6", [
        [ "5) beginTransmission(address)", "md_docs_2wire__library__cheat__sheet.html#autotoc_md7", null ],
        [ "6) write(value)", "md_docs_2wire__library__cheat__sheet.html#autotoc_md8", null ],
        [ "7) endTransmission(sendStop)", "md_docs_2wire__library__cheat__sheet.html#autotoc_md9", null ]
      ] ],
      [ "Lettura da uno slave", "md_docs_2wire__library__cheat__sheet.html#autotoc_md10", [
        [ "8) requestFrom(address, len, stopBit)", "md_docs_2wire__library__cheat__sheet.html#autotoc_md11", null ],
        [ "9) available()", "md_docs_2wire__library__cheat__sheet.html#autotoc_md12", null ],
        [ "10) read()", "md_docs_2wire__library__cheat__sheet.html#autotoc_md13", null ]
      ] ],
      [ "Flusso tipico per leggere registri MPU-6050", "md_docs_2wire__library__cheat__sheet.html#autotoc_md14", null ],
      [ "Note rapide", "md_docs_2wire__library__cheat__sheet.html#autotoc_md15", null ],
      [ "Valori esadecimali in Wire.write (MPU-6050)", "md_docs_2wire__library__cheat__sheet.html#autotoc_md16", null ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html"
];

const SYNCONMSG = 'click to disable panel synchronization';
const SYNCOFFMSG = 'click to enable panel synchronization';
const LISTOFALLMEMBERS = 'List of all members';