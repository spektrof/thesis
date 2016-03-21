------------------------------------------------------------------------------------

Használati útmutató:
  A program fordításához szükséges OpenGL(http://cg.elte.hu/~bsc_cg/resources/)
                                és Qt(http://www.qt.io/download-open-source/#section-2).
         
  A felhasználói felület segítségével elvághatjuk a bekódolt síkkal a kockánkat, majd a
  kapott eredmény közül ki tudjuk választani a számunkra megfelelőt.
  Ezt megismételhetjük restart segítségével.  
                                
TODO:
  - Sorter Functions for strategies
  - ActiveAtom marking based on the sorter function result
      
Jó lenne találni minden bughoz reprokat a teszthez s ezeket vhol gyűjteni
BUGS:
  - Haromszogek(altalaban 1, rosszul van osszerakva?? nem jelenik meg)
  - Neha atomsorrend rossz, kerdes megoldja e a centroidos megoldas?
  
-------------------------------------------------------------------------------------

Funkcióbillentyűk:
  - + és - (numpad) : atomok közötti váltás, az aktív atom nem átlátszó
  - t : átlátszóság ki-be kapcsolása
  - p : 3D és 2D nézeti kapcsoló
  - i : egyéb információk lekérése konzolra
  - wasd + mouse left press : kamera mozgatása
  - nyilak : fény mozgatása

Megj.:
  A +/- funkc. billentyű nem működik ha vágó módban vagyunk, 
  ekkor a legördülő menükkel tudjuk váltani a vágandó atom részeit(pozitív, negatív, mind2)
