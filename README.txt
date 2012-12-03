# Projet « Encapsulation cryptographique dans un protocole de communication chiffré et signé »
# AVERTY, COHAN, COLLEONI, PERCOT-TÉTU

                  // //  //
         __    ____||_//  //
       _/__--~~        ~~~-_
      /  /___        ___    \
     /  /(  +)      ( + )    |
    /  |  ~~~    __  ~~~   _/\/|
   |    \  ___.-~  ~-.___  \   / 
    \    \(     ` '      ~~)|   \
     \     )              / |    \
      \/   /              \ |    |
      /   |               | |    |
     |    /               |  \__/
     |    \_            _/      |    ___
     \      ~----...---~       /_.-~~ _/
      \_                      |    _-~ 
        \                    /  _-~ 
         ~-.__             _/--~
        _.-~  ~~~-----~~~~~
       ~-.-. _-~     /_ ._ \  
            ~       ~  ~  ~-


Utilisation:

- Autorité de certification :
	- Compilation : makefile dans le dossier "build" (ne compile volontairement QUE sur Linux pour 
ne pas remttre en cause la sécurité du protocole)
	- Lancement sans paramètre.
	- Port d'écoute : 65535 (sur toutes les interfaces)

- Client de messagerie :
	- Pareil que pour l'Autorité de certification.
	- Lancement : $executable <login> <ip autorité certification> (le client s'identifie alors auprès 
de l'autorité avec le login spécifié)
	- Envoi d'un message : <login destinataire> <message>
	- Port d'écoute : 65534

Enjoy' !

