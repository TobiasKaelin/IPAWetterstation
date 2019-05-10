const http = require('http');                                                                                             //Node.js Libary
const fs = require('fs');                                                                                                 //Filesystem Libary
const Serial = require('raspi-serial').Serial;                                                                            //raspi.serial Libary

var maxentries = 10000;                                                                                                   //Maximale Einträge auf Speicherkarte



http.createServer(function (req, res) {                                                                                   //Serverobjekt erstellen

  fs.readFile('/home/pi/myjsonfile.json', "utf-8", function(err, datafromfile) {                                          //Liest File mit Wetterdateneinträge von Verzeichnis
    if (err) {                                                                                                            // Falls Error -> Benachrichtigung und Fehlermeldung in Konsole
      console.log("Datei konnte nicht gelesen werden: "+err);                                                             //
    } else {
      console.log("Daten ausgelesen:")                                                                                    // Ansonsten Bestätigung in Konsole
      
     var Wetter = JSON.parse(datafromfile);                                                                               //String in JavaScript Objekt umwandeln.
      
          
      console.log(req.url)                                                                                                //Benachrichtigung in Konsole welche Seite aufgerufen wird

      if(req.url === '/api/full'){                                                                                        //Falls IP von Basisstation/api/full aufgerufen wird:
      
        res.write(JSON.stringify(Wetter));                                                                                //Schreibt alle Daten (Maximale Einträge) auf Weboberfläche
        res.end();                                                                                                        //
      }                                                                                                                   //
      if(req.url === '/api/1h'){                                                                                          //Falls IP von Basisstation/api/1h aufgerufen wird:
                                                                                                                          
        var oneh = (Date.now() - 3600000);                                                                                //Aktuelle Zeit in ms seit 01.01.1970 - 1h in ms
        var tt;                                           
        for (tt = Wetter.length-1; tt >= 0 ; tt--){                                                                       //Wählt nur Einträge der letzten Stunde an
          if(Wetter[tt].time < oneh){                                                                                     //
            Wetter.splice(tt, 1)                                                                                          //
          }                                                
        }                                                  
        res.write(JSON.stringify(Wetter));                                                                                //Schreibt die Daten von der letzuten Stunde auf Weboberfläche
        res.end();                                                                                                        //
      }
      if(req.url === '/api/24h'){                                                                                         //Falls IP von Basisstation/api/24h aufgerufen wird:

        var oneh = (Date.now() - 86400000);                                                                               //Aktuelle Zeit in ms seit 01.01.1970 - 24h in ms
        var tt;
        for (tt = Wetter.length-1; tt >= 0 ; tt--){                                                                       //Wählt nur Einträge der letzten 24 Stunden an
          if(Wetter[tt].time < oneh){
            Wetter.splice(tt, 1)
          }
        }
        res.write(JSON.stringify(Wetter));                                                                              //Schreibt die Daten von den letzten 24h auf Weboberfläche
        res.end();
      }
      
      if(req.url === '/home' || req.url === "" || req.url === "/" || req.url === undefined){                                                                                          //Falls ip von Basisstation/home aufgerufen wird:
        fs.readFile("index.html", "utf-8", function(err, html){                                        //Lese Daten von index.html Datei
          
    
          
          res.write(html);                                                                                             //Schreibt Index.html Datei auf Weboberfläche
          res.end();                                        
        });    
      }
      if(req.url === '/1h'){                                                                                          //Falls ip von Basisstation/1h aufgerufen wird:
        fs.readFile("1h.html", "utf-8", function(err, html){                                                          //Lese Daten von 1h.html Datei
          
    
          
          res.write(html);                                                                                             //Schreibt 1h.html Datei auf Weboberfläche
          res.end();                                       
        });    
      }
      if(req.url === '/24h'){                                                                                         //Falls ip von Basisstation/24h aufgerufen wird:
        fs.readFile("24h.html", "utf-8", function(err, html){                                                         //Lese Daten von 24h.html Datei
          
    
          
          res.write(html);                                                                                            //Schreibt 24h.html Datei auf Weboberfläche
          res.end(); 
        });    
      }
      
    }
  });

}).listen(80);                                                                                                      //Port 80 für Webserver deklarieren


console.log("Server gestartet auf Port "+80);                                                                       //Bestätigung in Konsole falls Server gestartet ist
var serial = new Serial();                                                                                          // Variable für Daten über den Seriellen-Eingang
serial.open(() => {                                 
  var buffer = "";                                                                                                  //Buffer hinzufügen
  serial.on('data', function(datafromserial) {                                                                      //Daten einlesen
    try {
      buffer += datafromserial;                                                                                     //Daten mit Buffer kombinieren
      console.log("Empfange Daten")
      if(buffer.indexOf("\n") >= 0){                                                                                //Pruft ob Buffer leer ist.
        var packets = buffer.split("\n");                                                                           //Trennt die einkommenden Daten in Pakete
        for(var i = 0; i < (packets.length - 1); i++){     
          if(packets[i] !== "" && packets[i] !== " "){
            var time = Date.now();                                                                                  //Aktuelles Datum in Variable schreiben
            

           
           
            var obj;                                                                                               //Variable "obj" ertsellen
            try {                                 
              obj = JSON.parse(packets[i]);                                                                       //Verwandelt String in Objekt
            } catch(err){                                                                                         // Falls Fehler beim parsen:
              buffer = "";                                                                                        // Buffer leeren
              console.log("Fehler beim parsen: "+err, packets[i]);                                                // Benachrichtigung in Konsole
              return;                                                         
            }
            obj.time = time
                                                                                        
            console.log("Daten erfolgreich Empfangen: ", obj);
        
            fs.readFile('/home/pi/myjsonfile.json', "utf-8", function(err, datafromfile) {                       //Lese Datei von SD Karte
              if (err) {                                                                                         //Falls Fehler:
                console.log("Datei konnte nicht gelesen werden: "+err);                                          //Benachrichtigung in Konsole und Fehlercode
              } else {                                                                                           //ansonsten
                if(datafromfile == ""){
                  datafromfile = "[]";
                }
                var Wetter = JSON.parse(datafromfile);                                                           //Verwandelt Datei von SD Karte in Objekt
                Wetter.push(obj)                                                                                 //Fügt neuste Wetterdaten an Objekt an                                      
                if(Wetter.length > maxentries){                                                                  //Falls Wetterdatei länger als Variable "maxentries"
                  var mentries;
                  for (mentries = maxentries; mentries <= Wetter.length; mentries++){                            //Gehe zum ältesten Eintrag
                    Wetter.shift();                                                                              //Lösche ältesten Eintrag
                    console.log("lösche element")                                                                //Benachrichtigung in Konsole
                  }
                }
                Wetter = JSON.stringify(Wetter);                                                                 //Objekt in String verwandeln
                fs.writeFile('/home/pi/myjsonfile.json', Wetter, 'utf8', function(err){                          //String in JSON File schreiben
                  if(err){                                                                                       //Falls Fehler:
                    console.log("Datei konnte nicht geschrieben werden: "+err);                                  //Benachrichtigung in Konsole und Fehlercode
                  } else {                                                                                       //Ansonsten

                    console.log("Datei erfolgreich geschrieben")                                                 //Benachrichtigung in Konsole
                  }
                });
                


              }
            });
          
          }
        }
        var leftover = "";                                        //Definiere leere Variable
        if(packets[(packets.length - 1)] !== ""){                 // Falls Buffer nicht leer
          leftover = packets[(packets.length - 1)];               // Buffer leeren
        }
        buffer = leftover;
      } 
    } catch(err){                                                 //Liest Fehlermeldung
      console.error("Fehler beim Empfangen der Daten: "+err);     //Benachrichtigung in Konsole mit Fehlercode
    }
  });


});


