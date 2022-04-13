// Programme par Xavier.
// Permet d'envoyer une page web a partir d'un fichier htm sur une carte SD
// Fonctionnel chez moi le 13/04

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
 
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xBD, 0x16 }; // Adresse MAC du Shield Ethernet
IPAddress ip(192, 168, 1, 91); 
EthernetServer server(80);  // Création du serveur port 80 (site web)
 
File webFile;
 
void setup()
{
    Ethernet.begin(mac, ip);  // initialisation des composants Ethernet
    server.begin();           // pour les clients web
    Serial.begin(9600);       // pour débugage
 
    // initialisation de la carte SD
    Serial.println("Préparation de la carte SD...");
    if (!SD.begin(4)) 
    {
        Serial.println("ERREUR - Carte SD Non installé !");
        return;
    }
    Serial.println("INFO - Carte SD prête.");
    if (!SD.exists("index.htm")) 
    {
        Serial.println("ERREUR - impossible de trouver le fichier !");
        return;
    }
    Serial.println("INFO - index.html chargé.");
}
 
void loop()
{
    EthernetClient client = server.available(); 
 
    if (client) 
    {
        boolean currentLineIsBlank = true;
        while (client.connected()) 
        {
            if (client.available()) 
            {   // données client dispo
                char c = client.read(); 
                if (c == '\n' && currentLineIsBlank) 
                {
                    // Envoie d'une réponse HTTP Standar
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // Envoie de la page web
                    webFile = SD.open("index.htm");        // ouverture du fichier
                    Serial.println("Page envoyé");
                    if (webFile) 
                    {
                        while(webFile.available()) 
                        {
                            client.write(webFile.read()); // Envoie de la page web au client
                        }
                        webFile.close();
                    }
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') 
                {
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') 
                {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // Fin if (client.available())
        } // Fin while (client.connected())
        delay(1);      // Temps pour que le navigateur récupère les données
        client.stop(); // Fermeture de la connection
    } // Fin if (client)
}
