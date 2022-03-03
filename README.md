TP CAPTEUR

Connection sur la carte Nucleo STM32 :
  - Pour utiliser le capteur BMP280, nous avons utilisé la liaison I2C1
       - SCL : PB6
       - SDA : PB7
  - Pour utiliser la liaison série avec la carte Raspberry, nous utilisons USART3 :
       -  USART3_TX : PC4
       -  USART3_RX : PC5
  - Pour utiliser le moteur pas à pas, on a utilisé la liaison CAN1
       - CAN1_TX : PB9
       - CAN1_RX : PB8

Nous avons travaillé avec la Raspberry Pi 0 - 1. Avec pour adresse IP 192.168.88.220
  - Pour s'y connecter, on a défini : 
       - Identifiant : user
       - Password    : password

Pour lancer le serveur on se place dans le dossier dev_server et on entre la commande suivante :
   - $ FLASK_APP=hello.py FLASK_ENV=development flask run --host 0.0.0.0

Connection sur notre serveur : http://192.168.88.220:5000/api/welcome 


Ce qui est fonctionnel :
  - A noter que pour que le serveur fonctionne bien il faut compiler la carte STM32 avant de lancer le serveur.
  - On peut réaliser un POST (avec path = /api/temp/) pour stocker la valeur de la température dans un tableau.
   Cette valeur est retournée pour vérifier sa cohérence.
  - On peut réaliser un POST (avec path = /api/pres/) pour stocker la valeur de la pression dans un tableau.
   Cette valeur est retournée pour vérifier sa cohérence.
  - Sur la STM32, la commande du moteur pas à pas fonctionne bien, à l'aide des fonctions, MOTOR_init() et MOTOR_angle(int angle).
   Nous avons réalisé sur le serveur la fonction avec la méthode POST (avec path = /api/scale/<int:K>) pour mettre à jour l'angle, mais n'avons eu le temps de vérifier son bon fonctionnement.
   La valeur pour commander le moteur pas à pas est égale à température * K/100

Pour vérifier le bon fonctionnement du capteur de température et de pression sans utiliser le serveur, on peut soit utiliser le minicom de la Raspberry pour envoyer GET_T ou GET_P, soit regarder sur la liaison UART2 la valeur de la température et de la pression qui s'affiche juste après un RESET.
De plus, initialement l'angle du moteur pas à pas est égal à la température. 

Ce qui n'est pas fonctionnel :
   - Les GET n'ont pas été réalisés pour récupérer les valeurs stockées de la température et de la pression.
   - Les DELETE pour supprimer des valeurs stockées n'ont pas été réalisés non plus.
   - L'accéléromètre n'a pas été utilisé.

Attention à bien éteindre la Raspberry Pi avant de la débrancher : sudo shutdown -h now
