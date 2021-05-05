Turn back before it is too late

ENDELIGE SCENARIE:

Række 1 er sinken sinkmote1
  Modtager data og visualiserer denne (sender til konsollen så det kan vises i matlab) 

Række 2 består af 2 aggregators- aggmote1 & aggmote2
  Aggregerer modtaget data over 10 sekunder - sender til sink. 
  Data til sink består af: { average, min, max, median, antal_data_points }
      Data points er så sink kan se hvor mange datasamples der indgår, og hvor højt den modtagede aggregerede data skal vægtes. 
  
Række 3 består af 4 sources - sourcemote1 - sourcemote4
  Vi sender temperaturdata hvert sekund --> prædefineret array i matlab. 
  sourcemote1 & 2 sender til aggmote1    |    sourcemote3& 4 sender til aggmote2
  source "suppresser" duplicates i data, således at den ikke sender 24 grader celsius 1 gang i sekundet.

Første PROOF OF CONCEPT SCENARIE:
kun 1 aggregator med 2 tilhørende sources


Arbejdsopgaver: 
1. Lav kommunikationsprotocol mellem sources -> aggregator  og aggregators -> sink
    a. Selve data der sendes imellem
    b. IP lookup table i en header fil. (Andy og toby er godt i gang med dette)
2. Lav source rækken
3. Lav aggregator rækken
4. Lav sink som aggregerer modtaget data og smider det ud i konsollen (så man kan proppe det i matlab)
4. Lav source data, muligvis i matlab
5. Lav data visualisering i matlab, samt udregning af precision, accuracy osv. 
