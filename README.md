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
## Architecture matérielle de l’objet
![alt text](https://github.com/maximenrs/projetIOT22/blob/main/Images/Structure.png?raw=true)

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
![alt text](https://github.com/maximenrs/projetIOT22/blob/main/Images/Automate.png?raw=true)

## Durée de vie de la batterie
Pour commencer, on peut différencier les différentes classes : 
-   **Classe A** : Cette classe a la consommation énergétique la plus faible. Lorsque l'équipement a des données à envoyer il le fait sans contrôle puis il ouvre 2 fenêtres d'écoute successives pour des éventuels messages provenant du serveur, les durées recommandées sont de 1 puis 2 secondes. Ces 2 fenêtres sont les seules durant lesquelles le serveur peut envoyer à l'équipement les données qu'il a précédemment stockées à son attention.
-   **Classe B** : Cette classe permet un compromis entre la consommation énergétique et le besoin en communication bidirectionnelle. Ces équipements ouvrent des fenêtres de réception à des intervalles programmés par des messages périodique envoyés par le serveur.
-   **Classe C** : Cette classe a la plus forte consommation énergétique mais permet des communications bidirectionnelles n'étant pas programmées. Les équipements ont une fenêtre d'écoute permanente.

## Analyse du cycle de vie du produit
## Analyse des produits concurrents
|Nom du produit|Avantage(s)|Inconvénient(s)|
|--------------|-----------|---------------|
|R602A LoRaWAN Wireless Siren|||
|Avertisseur vocal connecté 126 dB Radio LoRa SmartVOX|||
## Localisation de l'objet





