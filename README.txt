# Projet � Encapsulation cryptographique dans un protocole de communication chiffr� et sign� �
# AVERTY, COHAN, COLLEONI, PERCOT-T�TU

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

- Autorit� de certification :
	- Compilation : makefile dans le dossier "build" (ne compile volontairement QUE sur Linux pour 
ne pas remttre en cause la s�curit� du protocole)
	- Lancement sans param�tre.
	- Port d'�coute : 65535 (sur toutes les interfaces)

- Client de messagerie :
	- Pareil que pour l'Autorit� de certification.
	- Lancement : $executable <login> <ip autorit� certification> (le client s'identifie alors aupr�s 
de l'autorit� avec le login sp�cifi�)
	- Envoi d'un message : <login destinataire> <message>
	- Port d'�coute : 65534

Enjoy' !

