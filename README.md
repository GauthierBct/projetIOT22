# Mini Projet IoT 2021-2022 
# Réseau de sirènes d’alarme LoRaWAN

## Objectif
L’objectif du mini-projet est d’étudier la réalisation d’un objet connecté en LoRaWAN pour la sécurité des biens et des personnes.

L’objet LoRaWAN est identifié par son DevEUI et associé à une pièce ou à un lieu. Il peut être enregistré sur un réseau public national en OTAA ou sur un réseau privé en OTAA comme CampusIoT. Cet objet pourrait participer à un réseau maillé (“mesh”) LoRa (type Amazon Sidewalk).

L’objet sirène remonte régulièrement des mesures prises sur les capteurs (TH, smoke, CO,CO2...) équipant celui-ci au format LPP.

L’objet sirène est déclenché soit par un bouton poussoir (ie panic button), on utilisera ce [bouton poussoir](https://www.gotronic.fr/art-bouton-poussoir-arcade-jaune-bd23j-29602.htm), soit par observation d’un événement “tragique” (changement “brutale” de la température, CO2, CO, fumée …) que l'on fera avec le [capteur SCD30](https://www.seeedstudio.com/Grove-CO2-Temperature-Humidity-Sensor-SCD30-p-2911.html). Quand la sirène est munie d’un [PIR Motion sensor](https://wiki.seeedstudio.com/Grove-PIR_Motion_Sensor/), elle peut détecter et envoyer périodiquement la présence probable de personnes à proximité d’elle.
  
L’objet sirène envoie périodiquement un message de status (ie heart beat) une fois déclenchée.

## Architecture globale du réseau de sirènes d’alarme
## Sécurité globale : clé de chiffrage
Pour échanger des données, tous les appareils doivent être activés par le réseau.
On a 2 types de procédures d'activation :
- Activation par la voie des airs (OTAA)
- Activation par personnalisation (ABP)

![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/Activation.png?raw=true)

Dans notre cas nous utiliserons OTAA cela signifie que :
- Nécessite des informations sur l'EUI du dispositif, l'EUI de l'application et la clé de l'application.
- Le dispositif initie une poignée de main avec le serveur pour obtenir son adresse et un "nonce" => l'adresse du dispositif change après chaque activation.
- Les 2 clés de session sont dérivées de la clé d'application et du nonce.

Dans notre main.c cela correspond aux lignes suivantes : 
```c
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x64, 0x05, 0xe0, 0xfd, 0xa2, 0x58, 0x65, 0x82 };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x01, 0x3B, 0xF2, 0x33, 0x0C, 0x6D, 0xA2, 0x03, 0x4D, 0xDE, 0x8E, 0xD0, 0x07, 0xB6, 0x4E, 0x6A };
```

## Architecture matérielle de l’objet
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/Structure.png?raw=true)

## Coût de la BOM de notre produit
|Matériel|Quantité|Prix unitaire|Prix total|Remarques|
|--------|--------|-------------|----------|-|
|PIR Motion Sensor|1|8,90 €|8,90 €|Permet de détecter régulièrement la présence probable de personnes à proximité de l'alarme.| 
|Capteur de CO2, de T° et d'humidité|1|68,80 €|68,80 €|Ce capteur va permettre de définir si la situation est critique ou non en comparant les mesures aux seuils d'une situation "normale".|
|LEDs|2|0,0447 €|0,0894 €|Permet d'afficher l'état actuel aux utilisateurs a proximité de l'alarme.|
|Buzzer|1|0,39 €|0,39 €|Permet de communiquer aux utilisateurs à proximité de l'alarme que la situation est dangereuse.|
|LoRa E5 Development Kit|1|23,46 €|23,46 €|Permet la communication LoRa|
|Bouton poussoir d'arcade|1|2,50 €|2,50 €|Permet de lancer le mode test et d'arrêter l'alarme|
|Système complet|5000|112,36 €|561 800 €|
## Coût de certification ETSI du produit et le coût de certification LoRa Alliance du produit
### Certification ETSI
- Normes : ETSI EN 319 411-1, RGS
- Durée de la certification : 1 à 3 ans 
- Tarif (HT) pour la durée maximale : 267€/an soit 800€ (HT)
### Certification LoRa Alliance
- Cotisation annuelle : 10 000 $
- 1 licence gratuite de LCTT (LoRaWAN Certification Test Tool) incluse
- Frais de certification du produit : 1 000 $ par produit
## Implémentation du logiciel embarqué
## Format LPP des messages LoRaWAN uplink et downlink
- Message UpLink
	- **Température**  : Format LPP Temperature sur le canal 0
	- **Concentration en CO2** : Format LPP Analog sur le canal 1
	- **Humidité** : Format LPP RelativeHumidity sur le canal 2
	- **Mouvement** : Format LPP Presence sur le canal 3
## Logiciel embarqué de l’objet sirène
## Métriques logiciel du logiciel embarqué
## Changements de comportement de l’objet en fonction des événements
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/Automate.png?raw=true)

## Durée de vie de la batterie
Pour commencer, on peut différencier les différentes classes : 
-   **Classe A** : Cette classe a la consommation énergétique la plus faible. Lorsque l'équipement a des données à envoyer il le fait sans contrôle puis il ouvre 2 fenêtres d'écoute successives pour des éventuels messages provenant du serveur, les durées recommandées sont de 1 puis 2 secondes. Ces 2 fenêtres sont les seules durant lesquelles le serveur peut envoyer à l'équipement les données qu'il a précédemment stockées à son attention.
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/ClasseA.png?raw=true)

-   **Classe B** : Cette classe permet un compromis entre la consommation énergétique et le besoin en communication bidirectionnelle. Ces équipements ouvrent des fenêtres de réception à des intervalles programmés par des messages périodique envoyés par le serveur.
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/ClasseB.png?raw=true)

-   **Classe C** : Cette classe a la plus forte consommation énergétique mais permet des communications bidirectionnelles n'étant pas programmées. Les équipements ont une fenêtre d'écoute permanente.
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/ClasseC.png?raw=true)


## Analyse du cycle de vie du produit
## Analyse des produits concurrents
|Nom du produit|Avantage(s)|Inconvénient(s)|
|--------------|-----------|---------------|
|R602A LoRaWAN Wireless Siren|Secouru par batterie intégrée / Extrêmement simple à installer et utiliser / Grande couverture sonore (126 dB) / Communication sans fil longue distance (jusqu'à 1,2 km)|Forte consommation en énergie / Prix très élevé|
|Avertisseur vocal connecté 126 dB Radio LoRa SmartVOX|||
## Localisation de l'objet
## Options du système
Nous utilisons Cayenne afin de visualiser nos données mais également d'envoyer un mail ainsi qu'un SMS pour prévenir que la concentration de CO2 a dépassé un seuil critique et donc que l'alarme est déclenché.






