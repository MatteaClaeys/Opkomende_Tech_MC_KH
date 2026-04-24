# Fysiek prototype

De gebruikte microcontroller voor dit project is een [NodeMCU 32S](/imgs/nodemcu.jpg). Hierbij worden GPIO16 en GPIO17 gebruikt als seriële communicatiepinnen (RX en TX). Deze moeten kruislings verbonden worden met de TX- en RX-pinnen van de [DFPlayer Mini](/imgs/dfpin.png).

Vervolgens wordt de 5V-pin van de microcontroller verbonden met de VCC-pin van de DFPlayer. Ook alle GND-aansluitingen moeten met elkaar verbonden worden om een gemeenschappelijke massa te creëren.

De 3 Watt speaker wordt aangesloten op de twee SPK-pinnen van de DFPlayer. De polariteit hiervan maakt niet uit.

Ten slotte worden de vier drukknoppen aangesloten. Eén zijde van elke knop wordt verbonden met GND, en de andere zijde met de GPIO-pinnen 14, 25, 26 en 27 van de microcontroller.

*De pinouts van de componenten vindt u door op de onderstreepte blauwe tekst te klikken.*

## wiring diagrama
![](/imgs/ElektrischSchema.png)
![](/imgs/drawingschema.png)
