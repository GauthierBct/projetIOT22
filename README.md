# Mini Projet IoT 2021-2022 
# Réseau de sirènes d’alarme LoRaWAN

## Objectifs
L’objectif du mini-projet est d’étudier la réalisation d’un objet connecté en LoRaWAN pour la sécurité des biens et des personnes.

L’objet LoRaWAN est identifié par son DevEUI et associé à une pièce ou à un lieu. Il peut être enregistré sur un réseau public national en OTAA ou sur un réseau privé en OTAA comme CampusIoT. Cet objet pourrait participer à un réseau maillé (“mesh”) LoRa (type Amazon Sidewalk).

L’objet sirène remonte régulièrement des mesures prises sur les capteurs (TH, smoke, CO,CO2...) équipant celui-ci au format LPP.

L’objet sirène est déclenché soit par un bouton poussoir (ie panic button), on utilisera ce [bouton poussoir](https://www.gotronic.fr/art-bouton-poussoir-arcade-jaune-bd23j-29602.htm), soit par observation d’un événement “tragique” (changement “brutale” de la température, CO2, CO, fumée …) que l'on fera avec le [capteur SCD30](https://www.seeedstudio.com/Grove-CO2-Temperature-Humidity-Sensor-SCD30-p-2911.html). Quand la sirène est munie d’un [PIR Motion sensor](https://wiki.seeedstudio.com/Grove-PIR_Motion_Sensor/), elle peut détecter et envoyer périodiquement la présence probable de personnes à proximité d’elle.
  
L’objet sirène envoie périodiquement un message de status (ie heart beat) une fois déclenchée.

Une vidéo de démonstration est disponible [ici](https://www.youtube.com/watch?v=ilunLYDT-cQ).

## Architecture globale du réseau de sirènes d'alarme

![img-reseau-alarme](https://github.com/GauthierBct/projetIOT22/blob/main/img/Reseau.png)

L'image ci-dessous illustre la manière dont nous avons imaginé le fonctionnement de notre réseau d'alarmes connectées. Les alarmes communiquent entre elles en P2P (peer to peer) mais aussi avec un réseau LoRaWAN. Si une sirène se déclanche, elle peut directement informer toutes les autres de s'allumer également et dans un second temps, remonter les informations au serveur via le réseau LoRaWAN.

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
Après avoir définis les composants que nous allons utiliser, nous arrivons à la structure suivante (pour le end device). Ce dispositif nécessite une gateway connectée au réseau privé [CampusIOT](https://github.com/CampusIoT).

![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/Structure.png?raw=true)
## Coût de la BOM de notre produit
|Matériel|Quantité|Prix unitaire|Prix total|Remarques|
|--------|--------|-------------|----------|-|
|PIR Motion Sensor|1|8,90 €|8,90 €|Permet de détecter régulièrement la présence probable de personnes à proximité de l'alarme.| 
|Capteur de CO2, de T° et d'humidité|1|68,80 €|68,80 €|Ce capteur va permettre de définir si la situation est critique ou non en comparant les mesures aux seuils d'une situation "normale".|
|LED|1|0,0447 €|0,0447 €|Permet d'afficher l'état actuel aux utilisateurs a proximité de l'alarme.|
|Buzzer|1|0,39 €|0,39 €|Permet de communiquer aux utilisateurs à proximité de l'alarme que la situation est dangereuse.|
|LoRa E5 Development Kit|1|23,46 €|23,46 €|Permet la communication LoRa|
|Bouton poussoir d'arcade|1|2,50 €|2,50 €|Permet de lancer le mode test et d'arrêter l'alarme|
|Système complet|5000|104,13 €|520 650 €|
## Coût de certification ETSI du produit et le coût de certification LoRa Alliance du produit
### Certification ETSI
- Normes : ETSI EN 319 411-1, RGS
- Durée de la certification : 1 à 3 ans 
- Tarif (HT) pour la durée maximale : 267€/an soit 800€ (HT)
### Certification LoRa Alliance
- Cotisation annuelle : 10 000 $
- 1 licence gratuite de LCTT (LoRaWAN Certification Test Tool) incluse
- Frais de certification du produit : 1 000 $ par produit
## Format LPP des messages LoRaWAN uplink et downlink
- Message UpLink
	- **Température**  : Format LPP Temperature sur le canal 0
	- **Concentration en CO2** : Format LPP Analog sur le canal 1
	- **Mouvement** : Format LPP Presence sur le canal 2
	- **Test** : Format LPP Digital Output sur le canal 3
## Logiciel embarqué de l’objet sirène
Tout le code necessaire est disponible dans le [main.c](https://github.com/GauthierBct/projetIOT22/blob/main/main.c). On relève la valeur du CO2 toutes les 20 secondes et si elle est supérieure à un seuil, on envoie un message LoRa et déclanche la sirène. 
Si un appui court a lieu alors on passe en mode test, c'est à dire on fait sonner l'alarme et envoie une trame LoRa.
Tout appui long (3 secondes) permet de couper le buzzer.
## Métriques logiciel du logiciel embarqué
- Nombres de lignes : 230
- Taille du fichier binaire : 256 ko

## Changements de comportement de l’objet en fonction des événements
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/Automate.png?raw=true)

Dans un souci d'économie d'énergie, nous faisons boucler toutes les 20 secondes pour éviter de faire des réveils trop souvent et envoyons des trames LoRa uniquement dans le cas d'une détection.

## Durée de vie de la batterie
Pour commencer, on peut différencier les différentes classes : 
-   **Classe A** : Cette classe a la consommation énergétique la plus faible. Lorsque l'équipement a des données à envoyer il le fait sans contrôle puis il ouvre 2 fenêtres d'écoute successives pour des éventuels messages provenant du serveur, les durées recommandées sont de 1 puis 2 secondes. Ces 2 fenêtres sont les seules durant lesquelles le serveur peut envoyer à l'équipement les données qu'il a précédemment stockées à son attention.
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/ClasseA.png?raw=true)

-   **Classe B** : Cette classe permet un compromis entre la consommation énergétique et le besoin en communication bidirectionnelle. Ces équipements ouvrent des fenêtres de réception à des intervalles programmés par des messages périodique envoyés par le serveur.
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/ClasseB.png?raw=true)

-   **Classe C** : Cette classe a la plus forte consommation énergétique mais permet des communications bidirectionnelles n'étant pas programmées. Les équipements ont une fenêtre d'écoute permanente.
![alt text](https://github.com/GauthierBct/projetIOT22/blob/main/img/ClasseC.png?raw=true)

## Analyse des produits concurrents
|Nom du produit|Avantage(s)|Inconvénient(s)|
|--------------|-----------|---------------|
|Avertisseur vocal connecté 126 dB Radio LoRa SmartVOX|Secouru par batterie intégrée / Extrêmement simple à installer et utiliser / Grande couverture sonore (126 dB)|Forte consommation en énergie / Prix très élevé / Communication sans fil longue distance (jusqu'à 1,2 km)|
|R602A LoRaWAN Wireless Siren|Extrêmement simple à installer et utiliser / durée de vie de 5 ans (avec batterie) / Prix correcte (179 $)|Couverture sonore (80 dB)|
|MClimate CO2 Sensor and Notifier LoRaWAN|Extrêmement simple à installer et utiliser / durée de vie de 10 ans (avec piles) / Prix correcte (160 €)|Couverture sonore très faible (10 dB)|
|Alarme autonome ineo-sense ACS Switch Buzz®|Extrêmement simple à installer et utiliser / durée de vie de 5 ans (avec pile remplacable) / Détection ouverture de porte / Couverture sonore (100 dB)|Aucun|
Alarme autonome ineo-sense ACS Switch Buzz®

## Options du système
Nous utilisons Cayenne afin de visualiser nos données mais également d'envoyer un mail ainsi qu'un SMS pour prévenir que la concentration de CO2 a dépassé un seuil critique et donc que l'alarme est déclenché.
Il possède également un bouton test pour vérifier le fonctionnement de tous les capteurs mais également la connexion LoRa. 

Une idée d'amélioration aurait été d'envoyer un message UPLINK pour notifier de la détection de feu et d'utiliser les messages DOWNLINK pour demander plus d'informations au end device (température, présence ou non d'une personne, concentration de CO2, test ou non). 





